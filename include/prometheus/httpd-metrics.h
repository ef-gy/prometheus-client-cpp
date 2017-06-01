/* HTTP server metrics servlet.
 *
 * Implements a `cxxhttp` servlet that provides a handler for the /metrics
 * location, which exports statistics to a Prometheus server.
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

#if !defined(PROMETHEUS_HTTPD_METRICS_H)
#define PROMETHEUS_HTTPD_METRICS_H

#include <cxxhttp/httpd.h>
#include <prometheus/metric.h>

namespace prometheus {
namespace httpd {
static metric::gauge sessions(
    "http_sessions_total",
    "Active HTTP sessions, broken out by transport type.", {"transport"});
static metric::gauge servers(
    "http_servers_total", "Active HTTP servers, broken out by transport type.",
    {"transport"});
static metric::counter queries(
    "http_queries_total",
    "Served HTTP queries over all sessions, broken out by transport type.",
    {"transport"});
static metric::gauge clients(
    "http_clients_total", "Active HTTP clients, broken out by transport type.",
    {"transport"});

template <class transport>
static void updateLabels(const std::string &label) {
  const auto &ser =
      efgy::global<std::set<cxxhttp::http::server<transport> *>>();
  const auto &cli =
      efgy::global<std::set<cxxhttp::http::client<transport> *>>();

  servers.labels({label}).set(ser.size());
  clients.labels({label}).set(cli.size());

  std::size_t ses = 0;
  std::size_t q = 0;

  for (const auto &a : ser) {
    ses += a->sessions.size();
    for (const auto &s : a->sessions) {
      q += s->queries();
    }
  }
  for (const auto &a : cli) {
    ses += a->sessions.size();
    for (const auto &s : a->sessions) {
      q += s->queries();
    }
  }

  sessions.labels({label}).set(ses);
  queries.labels({label}).set(q);
}

namespace metrics {
static void metrics(cxxhttp::http::sessionData &session, std::smatch &) {
  static auto &reg = efgy::global<collector>();
  updateLabels<cxxhttp::transport::tcp>("tcp");
  updateLabels<cxxhttp::transport::unix>("unix");

  session.reply(200, reg.text(),
                {{"Content-Type", "text/plain; version=0.0.4"}});
}

/* Metrics resource regexp.
 *
 * Prometheus metrics are exported on `/metrics` as is the default for
 * Prometheus clients.
 */
static const std::string resource = "/metrics";

static cxxhttp::http::servlet servlet(resource, metrics);
}
}
}

#endif
