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
#include <sstream>
#include <string>
#include <vector>

#include <ef.gy/global.h>

namespace prometheus {

/* Collector class.
 *
 * We use a tree structure for our metrics, meaning that each metric is a kind
 * of collector, and label sets for a metric end up as a proper sub-metric for
 * them.
 *
 * When generating the /metrics output, we generate our own and then the
 * remainder for all sub-metrics, recursively.
 *
 * Generally you want to use one of the pre-defined metrics types, as they have
 * the type information set correctly for typical use cases.
 */
class collector {
 public:
  const std::string name;
  const std::string type;
  const std::string help;

  long long timestamp;

  bool wasSet = false;
  bool haveTimestamp = false;

  long long value = 0;
  collector &root;
  std::map<std::string, collector *> child;
  std::map<std::string, std::string> label;
  const std::vector<std::string> labelNames;

  collector(void) : root(*this), beacon(*this, root.descendant) {}

  collector(const std::string &pName, const std::string &pType,
            const std::string &pHelp, const std::vector<std::string> &pLabels,
            collector &pRegistry,
            const std::map<std::string, std::string> &pLabel)
      : name(pName),
        type(pType),
        help(pHelp),
        root(pRegistry),
        label(pLabel),
        labelNames(pLabels),
        beacon(*this, root.descendant) {}

  virtual ~collector(void) {
    for (auto &c : child) {
      delete c.second;
    }
  }

  std::string text(void) const {
    std::string reply;
    const std::string ls = labelString(activeLabels());
    if (!name.empty() && ls.empty()) {
      reply += "# TYPE " + name + " " + type + "\n";
      reply += "# HELP " + name + " " + help + "\n";
    }
    std::string subtext;
    for (const auto &c : descendant) {
      if (c != this) {
        subtext += c->text();
      }
    }
    reply += subtext;
    if (wasSet || subtext.empty()) {
      reply += name + ls + " " + std::to_string(value);
      if (haveTimestamp) {
        std::ostringstream os("");
        os << timestamp;
        reply += " " + os.str();
      }
      reply += "\n";
    }
    return reply;
  }

  /* Update the metric's timestamp to a given value.
   * @t The timestamp to associate with the value.
   *
   * Only use if you have an actual value that you know out of band to be
   * right for the value you set. If you don't set a timestamp then none is sent
   * back to clients, and the current time is assumed.
   */
  void updateTimestamp(long long t) {
    timestamp = t;
    haveTimestamp = true;
  }

  std::map<std::string, std::string> activeLabels(void) const {
    std::map<std::string, std::string> rv;
    for (const auto &l : label) {
      if (!l.second.empty()) {
        rv[l.first] = l.second;
      }
    }
    return rv;
  }

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
    std::string reply;
    for (const auto &l : labels) {
      if (!reply.empty()) {
        reply += ",";
      }
      reply += l.first + "=\"" + escape(l.second) + "\"";
    }
    return reply.empty() ? "" : "{" + reply + "}";
  }

 protected:
  static std::string escape(const std::string &s) {
    std::string r;
    for (const auto &c : s) {
      switch (c) {
        case '\\':
        case '"':
          r.push_back('\\');
      }
      r.push_back(c);
    }
    return r;
  }

  efgy::beacons<collector> descendant;
  efgy::beacon<collector> beacon;
};
}  // namespace prometheus

#endif
