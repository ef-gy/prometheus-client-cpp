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
          collector::registry<collector::base> &reg =
              collector::registry<collector::base>::common())
      : collector::base(pName, reg, "counter"), val(0) {}

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

protected:
  T val;
};

template <typename T = long long> class gauge : public collector::base {
public:
  gauge(const std::string &pName,
        collector::registry<collector::base> &reg =
            collector::registry<collector::base>::common())
      : collector::base(pName, reg, "gauge"), val(0) {}

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

protected:
  T val;
};
}

static bool setDefaultMetrics(collector::registry<collector::base> &reg) {
  (new metric::gauge<long long>("process_start_time_seconds", reg))
      ->setToCurrentTime();

  return true;
}
}

#endif
