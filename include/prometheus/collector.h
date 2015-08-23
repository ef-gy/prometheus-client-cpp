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

#if !defined(PROMETHEUS_COLLECTOR_H)
#define PROMETHEUS_COLLECTOR_H

#include <ef.gy/maybe.h>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <ctime>

namespace prometheus {
namespace collector {
class base;
class registry;
}

static bool setDefaultMetrics(collector::registry &reg);

namespace collector {
class registry {
public:
  registry(bool addDefaultCollectors = true) : collectors() {
    if (addDefaultCollectors) {
      setDefaultMetrics(*this);
    }
  }

  static registry &common(void) {
    static registry reg;

    return reg;
  }

  void add(const base &pCollector) { collectors.insert(&pCollector); }

  std::set<const base *> collectors;
};

class base {
public:
  base(const std::string &pName, registry &pRegistry = registry::common(),
       const std::string &pType = "")
      : name(pName), registry(pRegistry), type(pType), help() {
    registry.add(*this);
  }

  virtual ~base(void) {}

  const std::string name;
  const std::string type;
  virtual std::string value(void) const = 0;
  std::string help;
  std::map<std::string, std::string> label;

  efgy::maybe<long long> timestamp;

  base &updateTimestamp(void) {
    timestamp = efgy::maybe<long long>((long long) std::time(0));
    return *this;
  }

protected:
  registry &registry;
};
}
}

#endif
