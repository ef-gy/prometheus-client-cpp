/**\file
 *
 * \copyright
 * Copyright (c) 2015, Magnus Achim Deininger <magnus@ef.gy>
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Documentation: https://ef.gy/documentation/prometheus-client-cpp
 * \see Source Code: https://github.com/jyujin/prometheus-client-cpp
 * \see Licence Terms: https://github.com/jyujin/prometheus-client-cpp/COPYING
 */

#if !defined(PROMETHEUS_METRIC_H)
#define PROMETHEUS_METRIC_H

#include <prometheus/collector.h>
#include <sstream>

namespace prometheus {
namespace metric {
template <typename T = long long> class counter : public collector::base {
public:
  counter(const std::string &pName,
          const std::vector<std::string> &pLabels = std::vector<std::string>(),
          collector::registry<collector::base> &reg =
              collector::registry<collector::base>::common(),
          const std::map<std::string, std::string> &pLabel =
              std::map<std::string, std::string>())
      : collector::base(pName, "counter", pLabels, reg, pLabel), val(0) {}

  virtual std::string value(void) const {
    std::ostringstream oss("");
    oss << val;
    return oss.str();
  }

  counter &inc(const T &v = 1) {
    val += v >= 0 ? v : 0;
    return *this;
  }

  counter &set(const T &v) {
    val = v > val ? v : val;
    return *this;
  }

  counter &labels(const std::vector<std::string> &labelValues) {
    const auto newLabels = applyLabels(labelValues);
    const auto ls = labelString(newLabels);
    if (!child[ls]) {
      child[ls] = new counter(name, labelNames, *this, newLabels);
    }
    return *((counter *)child[ls]);
  }

protected:
  T val;
};

template <typename T = long long> class gauge : public collector::base {
public:
  gauge(const std::string &pName,
        const std::vector<std::string> &pLabels = std::vector<std::string>(),
        collector::registry<collector::base> &reg =
            collector::registry<collector::base>::common(),
        const std::map<std::string, std::string> &pLabel =
            std::map<std::string, std::string>())
      : collector::base(pName, "gauge", pLabels, reg, pLabel), val(0) {}

  virtual std::string value(void) const {
    std::ostringstream oss("");
    oss << val;
    return oss.str();
  }

  gauge &inc(const T &v = 1) {
    val += v;
    return *this;
  }

  gauge &dec(const T &v = 1) {
    val -= v;
    return *this;
  }

  gauge &set(const T &v) {
    val = v;
    return *this;
  }

  gauge &setToCurrentTime(void) {
    val = std::time(0);
    return *this;
  }

  gauge &labels(const std::vector<std::string> &labelValues) {
    const auto newLabels = applyLabels(labelValues);
    const auto ls = labelString(newLabels);
    if (!child[ls]) {
      child[ls] = new gauge(name, labelNames, *this, newLabels);
    }
    return *((gauge *)child[ls]);
  }

protected:
  T val;
};

template <typename T = long long> class histogram : public collector::base {
public:
  histogram(const std::string &pName, const std::vector<std::string> &pLabels =
                                          std::vector<std::string>(),
            collector::registry<collector::base> &reg =
                collector::registry<collector::base>::common(),
            const std::map<std::string, std::string> &pLabel =
                std::map<std::string, std::string>())
      : collector::base(pName, "histogram", pLabels, reg, pLabel),
        count(pName, pLabels, *this, pLabel),
        sum(pName, pLabels, *this, pLabel), inf(pName, pLabels, *this, pLabel) {
    inf.label["le"] = "+Inf";
  }

  histogram &labels(const std::vector<std::string> &labelValues) {
    const auto newLabels = applyLabels(labelValues);
    const auto ls = labelString(newLabels);
    if (!child[ls]) {
      child[ls] = new histogram(name, labelNames, *this, newLabels);
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

static bool setDefaultMetrics(collector::registry<collector::base> &reg) {
  (new metric::gauge<long long>("process_start_time_seconds", {}, reg))
      ->setToCurrentTime();

  return true;
}
}

#endif
