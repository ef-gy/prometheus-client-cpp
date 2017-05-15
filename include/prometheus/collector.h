/* Prometheus Client Base Collector
 *
 * Contains the collector class, which is the root for all other metric
 * collector classes.
 *
 * Prometheus metric collectors are a tree-shaped construct, at least with all
 * the support for labels that is mandated in the specs.
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

#if !defined(PROMETHEUS_COLLECTOR_H)
#define PROMETHEUS_COLLECTOR_H

#include <ctime>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace prometheus {
class collector {
 public:
  const std::string name;
  const std::string type;
  const std::string help;

  long long timestamp;

  bool wasSet;
  bool haveTimestamp;
  std::set<const collector *> descendant;

  collector(void) : root(*this), haveTimestamp(false), wasSet(false) {}

  collector(
      const std::string &pName, const std::string &pType = "",
      const std::string &pHelp = "",
      const std::vector<std::string> &pLabels = std::vector<std::string>(),
      collector &pRegistry = efgy::global<collector>(),
      const std::map<std::string, std::string> &pLabel =
          std::map<std::string, std::string>())
      : name(pName),
        root(pRegistry),
        type(pType),
        help(pHelp),
        labelNames(pLabels),
        label(pLabel),
        haveTimestamp(false),
        wasSet(false) {
    root.descendant.insert(this);
  }

  virtual ~collector(void) {
    root.descendant.erase(this);
    for (auto &c : child) {
      delete c.second;
    }
  }

  virtual std::string value(void) const { return "0"; }

  std::string text(void) const {
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
    std::string subtext;
    for (const auto &c : descendant) {
      subtext += c->text();
    }
    reply += subtext;
    if (wasSet || subtext.empty()) {
      reply += name + ls + " " + value();
      if (haveTimestamp) {
        std::ostringstream os("");
        os << timestamp;
        reply += " " + os.str();
      }
      reply += "\n";
    }
    return reply;
  }

  collector &updateTimestamp(void) {
    haveTimestamp = false;

    return *this;
  }

  collector &updateTimestamp(long long t) {
    haveTimestamp = true;
    timestamp = t;

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
  collector &root;
  std::map<std::string, collector *> child;
  std::map<std::string, std::string> label;
  const std::vector<std::string> labelNames;

  std::map<std::string, std::string> applyLabels(
      const std::vector<std::string> &labelValues) const {
    std::map<std::string, std::string> rv = activeLabels();
    for (std::size_t i = 0; i < labelValues.size() && i < labelNames.size();
         i++) {
      rv[labelNames[i]] = labelValues[i];
    }
    return rv;
  }

  static std::string labelString(
      const std::map<std::string, std::string> &labels) {
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

#endif
