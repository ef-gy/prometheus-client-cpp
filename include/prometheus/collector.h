/**\file
 *
 * \copyright
 * This file is part of a Prometheus CPP client, which is released as open
 * source under the terms of an MIT/X11-style licence, described in the COPYING
 * file.
 *
 * \see Documentation: https://ef.gy/documentation/prometheus-client-cpp
 * \see Source Code: https://github.com/ef-gy/prometheus-client-cpp
 * \see Licence Terms: https://github.com/ef-gy/prometheus-client-cpp/COPYING
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
template <class base = base> class registry;
}

static bool setDefaultMetrics(collector::registry<collector::base> &reg);

namespace collector {
template <class base> class registry {
public:
  registry(bool addDefaultCollectors = true) : collectors() {
    if (addDefaultCollectors) {
      setDefaultMetrics(*this);
    }
  }

  virtual ~registry(void) {}

  static registry &common(void) {
    static registry reg;

    return reg;
  }

  virtual std::string text(void) const {
    std::string reply = "";

    for (const auto &c : collectors) {
      reply += c->text();
    }

    return reply;
  }

  void add(const registry &pCollector) { collectors.insert(&pCollector); }
  void remove(const registry &pCollector) { collectors.erase(&pCollector); }

protected:
  std::set<const registry *> collectors;
};

class hub : public registry<base> {
public:
  hub(registry<base> &pRegistry = registry<base>::common())
      : registry<base>(false), root(pRegistry) {
    root.add(*this);
  }

  virtual ~hub(void) { root.remove(*this); }

protected:
  registry<base> &root;
};

class base : public hub {
public:
  base(const std::string &pName, const std::string &pType = "",
       const std::vector<std::string> &pLabels = std::vector<std::string>(),
       registry<base> &pRegistry = registry<base>::common(),
       const std::map<std::string, std::string> &pLabel =
           std::map<std::string, std::string>())
      : name(pName), hub(pRegistry), type(pType), help(), child(),
        labelNames(pLabels), label(pLabel) {}

  virtual ~base(void) {
    for (auto &c : child) {
      delete c.second;
    }
  }

  virtual std::string value(void) const = 0;

  virtual std::string text(void) const {
    std::string reply;
    const std::string ls = labelString(activeLabels());
    if (ls == "") {
      if (type != "") {
        reply += "# TYPE " + name + " " + type + "\n";
      }
      if (help != "") {
        reply += "# HELP " + name + " " + help + "\n";
      }
    }
    reply += registry<base>::text();
    reply += name + ls + " " + value();
    if ((bool)timestamp) {
      std::ostringstream os("");
      os << (long long)timestamp;
      reply += " " + os.str();
    }
    return reply + "\n";
  }

  const std::string name;
  const std::string type;
  std::string help;

  efgy::maybe<long long> timestamp;

  base &updateTimestamp(efgy::maybe<long long> t = efgy::maybe<long long>()) {
    if ((bool)t) {
      timestamp = t;
    } else {
      timestamp = efgy::maybe<long long>((long long)std::time(0));
    }

    return *this;
  }

  std::map<std::string, std::string> activeLabels(void) const {
    std::map<std::string, std::string> rv;
    for (const auto &l : label) {
      if (l.second != "") {
        rv[l.first] = l.second;
      }
    }
    return rv;
  }

protected:
  std::map<std::string, base *> child;
  std::map<std::string, std::string> label;
  const std::vector<std::string> labelNames;

  std::map<std::string, std::string>
  applyLabels(const std::vector<std::string> &labelValues) const {
    std::map<std::string, std::string> rv = activeLabels();
    for (std::size_t i = 0; i < labelValues.size() && i < labelNames.size();
         i++) {
      rv[labelNames[i]] = labelValues[i];
    }
    return rv;
  }

  static std::string
  labelString(const std::map<std::string, std::string> &labels) {
    if (labels.size() == 0) {
      return "";
    }

    bool first = true;
    std::string reply = "{";
    for (const auto &l : labels) {
      if (first) {
        first = false;
      } else {
        reply += ",";
      }
      reply += escape(l.first) + "=\"" + escape(l.second) + "\"";
    }
    return reply + "}";
  }

  static const std::string escape(const std::string &s) {
    std::string r = "";
    for (const auto &c : s) {
      switch (c) {
      case '\\':
        r += "\\\\";
        break;
      case '"':
        r += "\\\"";
        break;
      default:
        r += c;
        break;
      }
    }
    return r;
  }
};
}
}

#endif
