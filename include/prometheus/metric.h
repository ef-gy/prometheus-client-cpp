/* Prometheus metric types.
 *
 * See also:
 * * Documentation: https://ef.gy/documentation/prometheus-client-cpp
 * * Source Code: https://github.com/ef-gy/prometheus-client-cpp
 * * Licence: https://github.com/ef-gy/prometheus-client-cpp/blob/master/COPYING
 *
 * @copyright
 * This file is part of a Prometheus CPP client, which is released as open
 * source under the terms of an MIT/X11-style licence, described in the COPYING
 * file.
 */

#if !defined(PROMETHEUS_METRIC_H)
#define PROMETHEUS_METRIC_H

#include <prometheus/collector.h>
#include <sstream>

namespace prometheus {
namespace metric {
namespace custom {
template <typename T>
class counter : public collector {
 public:
  counter(const std::string &pName, const std::string &pHelp,
          const std::vector<std::string> &pLabels = std::vector<std::string>(),
          collector &reg = efgy::global<collector>(),
          const std::map<std::string, std::string> &pLabel =
              std::map<std::string, std::string>())
      : collector(pName, "counter", pHelp, pLabels, reg, pLabel), val(0) {}

  virtual std::string value(void) const {
    std::ostringstream oss("");
    oss << val;
    return oss.str();
  }

  counter &inc(const T &v = 1) {
    val += v >= 0 ? v : 0;
    wasSet = true;
    return *this;
  }

  counter &set(const T &v) {
    val = v > val ? v : val;
    wasSet = true;
    return *this;
  }

  counter &labels(const std::vector<std::string> &labelValues) {
    const auto newLabels = applyLabels(labelValues);
    const auto ls = labelString(newLabels);
    if (!child[ls]) {
      child[ls] = new counter(name, help, labelNames, *this, newLabels);
    }
    return *((counter *)child[ls]);
  }

 protected:
  T val;
};

template <typename T>
class gauge : public collector {
 public:
  gauge(const std::string &pName, const std::string &pHelp,
        const std::vector<std::string> &pLabels = std::vector<std::string>(),
        collector &reg = efgy::global<collector>(),
        const std::map<std::string, std::string> &pLabel =
            std::map<std::string, std::string>())
      : collector(pName, "gauge", pHelp, pLabels, reg, pLabel), val(0) {}

  virtual std::string value(void) const {
    std::ostringstream oss("");
    oss << val;
    return oss.str();
  }

  gauge &inc(const T &v = 1) {
    val += v;
    wasSet = true;
    return *this;
  }

  gauge &dec(const T &v = 1) {
    val -= v;
    wasSet = true;
    return *this;
  }

  gauge &set(const T &v) {
    val = v;
    wasSet = true;
    return *this;
  }

  gauge &setToCurrentTime(void) {
    val = std::time(0);
    wasSet = true;
    return *this;
  }

  gauge &labels(const std::vector<std::string> &labelValues) {
    const auto newLabels = applyLabels(labelValues);
    const auto ls = labelString(newLabels);
    if (!child[ls]) {
      child[ls] = new gauge(name, help, labelNames, *this, newLabels);
    }
    return *((gauge *)child[ls]);
  }

 protected:
  T val;
};

template <typename T>
class histogram : public collector {
 public:
  histogram(
      const std::string &pName, const std::string &pHelp,
      const std::vector<std::string> &pLabels = std::vector<std::string>(),
      collector &reg = efgy::global<collector>(),
      const std::map<std::string, std::string> &pLabel =
          std::map<std::string, std::string>())
      : collector(pName, "histogram", pHelp, pLabels, reg, pLabel),
        count(pName, pLabels, *this, pLabel),
        sum(pName, pLabels, *this, pLabel),
        inf(pName, pLabels, *this, pLabel) {
    inf.label["le"] = "+Inf";
  }

  histogram &labels(const std::vector<std::string> &labelValues) {
    const auto newLabels = applyLabels(labelValues);
    const auto ls = labelString(newLabels);
    if (!child[ls]) {
      child[ls] = new histogram(name, help, labelNames, *this, newLabels);
    }
    return *((histogram *)child[ls]);
  }

  counter<T> &bucket(const T &val) const {
    for (const auto &b : buckets) {
      if (val <= b.first()) {
        return *b.second();
      }
    }

    return inf;
  }

  histogram &observe(const T &val) {
    bucket(val).inc();
    count.inc(val);
    sum.inc();
    return *this;
  }

 protected:
  counter<T> count;
  gauge<T> sum;

  std::map<T, counter<T> *> buckets;
  counter<T> inf;
};
}

using gauge = custom::gauge<long long>;
using counter = custom::counter<long long>;
using histogram = custom::histogram<long long>;
}

namespace special {
class processStartTime : public metric::gauge {
 public:
  processStartTime(collector &reg = efgy::global<collector>())
      : metric::gauge("process_start_time_seconds",
                      "UNIX time stamp of when the process has started.", {},
                      reg) {
    setToCurrentTime();
  }
};

static processStartTime processStartTimeMetric;
}
}

#endif
