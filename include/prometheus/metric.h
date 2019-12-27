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

namespace prometheus {
namespace metric {
template <class T>
T &labels(T &c, const std::vector<std::string> &labelValues) {
  const auto newLabels = c.applyLabels(labelValues);
  const auto ls = c.labelString(newLabels);
  if (!c.child[ls]) {
    c.child[ls] = new T(c.name, c.help, c.labelNames, c, newLabels);
  }
  return *((T *)c.child[ls]);
}

/* Counter metric.
 *
 * This is the metric to use for anything where the value is only supposed to
 * increase, which is why there is only an `inc()` function and nothing to set
 * the value directly. The advantage is that if a process restarts and this
 * resets to zero, your monitoring system can work out that this happened and
 * there won't be any measuring discontinuity.
 *
 * Typical values for this would be consumed CPU cycles, or the number of
 * processed requests.
 */
class counter : public collector {
 public:
  counter(const std::string &pName, const std::string &pHelp,
          const std::vector<std::string> &pLabels = {},
          collector &reg = efgy::global<collector>(),
          const std::map<std::string, std::string> &pLabel = {})
      : collector(pName, "counter", pHelp, pLabels, reg, pLabel) {}

  void set(long long v) {
    value = v > value ? v : value;
    wasSet = true;
  }

  void inc(long long v = 1) { set(value + (v >= 0 ? v : 0)); }

  counter &labels(const std::vector<std::string> &labelValues) {
    return metric::labels(*this, labelValues);
  }
};

/* Gauge metric.
 *
 * This kind of metric can have its value decrease as well as increase. This is
 * for values you measure that are not intended to be continually increasing in
 * value, like RAM usage.
 */
class gauge : public collector {
 public:
  gauge(const std::string &pName, const std::string &pHelp,
        const std::vector<std::string> &pLabels = {},
        collector &reg = efgy::global<collector>(),
        const std::map<std::string, std::string> &pLabel = {})
      : collector(pName, "gauge", pHelp, pLabels, reg, pLabel) {}

  void set(long long v) {
    value = v;
    wasSet = true;
  }

  void inc(long long v = 1) { set(value + v); }

  void dec(long long v = 1) { set(value - v); }

  void setToCurrentTime(void) { set(std::time(0)); }

  gauge &labels(const std::vector<std::string> &labelValues) {
    return metric::labels(*this, labelValues);
  }
};

class histogram : public collector {
 public:
  histogram(const std::string &pName, const std::string &pHelp,
            const std::vector<std::string> &pLabels = {},
            collector &reg = efgy::global<collector>(),
            const std::map<std::string, std::string> &pLabel = {})
      : collector(pName + "_bucket", "histogram", pHelp, pLabels, reg, pLabel),
        count(pName + "_count", pHelp, pLabels, *this, pLabel),
        sum(pName + "_sum", pHelp, pLabels, *this, pLabel),
        inf(pName + "_bucket", pHelp, pLabels, *this, pLabel) {
    inf.label["le"] = "+Inf";
  }

  histogram &labels(const std::vector<std::string> &labelValues) {
    return metric::labels(*this, labelValues);
  }

  histogram &bucket(long long b) {
    if (!buckets[b]) {
      auto label = activeLabels();
      label["le"] = std::to_string(b);

      const auto ls = labelString(label);
      buckets[b] = new counter(name, help, labelNames, *this, label);
      child[ls] = buckets[b];
    }
    return *this;
  }

  void observe(long long val) {
    for (const auto &b : buckets) {
      if (value <= b.first) {
        b.second->inc();
      }
    }

    inf.inc();
    count.inc();
    sum.inc(val);
  }

 protected:
  std::map<long long, counter *> buckets;

  counter count;
  gauge sum;
  counter inf;
};
}  // namespace metric

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
}  // namespace special
}  // namespace prometheus

#endif
