/* HTTP server fragments.
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

#if !defined(PROMETHEUS_HTTP_H)
#define PROMETHEUS_HTTP_H

#include <cxxhttp/http.h>
#include <prometheus/metric.h>

namespace prometheus {
namespace http {
static const std::string resource = "/metrics";

static metric::gauge sessions("http_sessions_total", {"transport"});
static metric::gauge servers("http_servers_total", {"transport"});
static metric::gauge servlets("http_servlets_total", {"transport"});
static metric::gauge clients("http_clients_total", {"transport"});

template <class transport>
static void updateLabels(const std::string &label) {
  const auto &ser =
      efgy::global<std::set<cxxhttp::http::server<transport> *>>();
  const auto &cli =
      efgy::global<std::set<cxxhttp::http::client<transport> *>>();
  const auto &srv =
      efgy::global<std::set<cxxhttp::httpd::servlet<transport> *>>();

  servers.labels({label}).set(ser.size());
  servlets.labels({label}).set(srv.size());
  clients.labels({label}).set(cli.size());

  std::size_t ses = 0;

  for (const auto &a : ser) {
    ses += a->sessions.size();
  }
  for (const auto &a : cli) {
    ses += a->sessions.size();
  }

  sessions.labels({label}).set(ses);
}

template <class transport>
static void metrics(typename cxxhttp::http::server<transport>::session &session,
                    std::smatch &) {
  static auto &reg = efgy::global<collector::base>();
  updateLabels<cxxhttp::transport::tcp>("tcp");
  updateLabels<cxxhttp::transport::unix>("unix");

  session.reply(200, {{"Content-Type", "text/plain; version=0.0.4"}},
                reg.text());
}
}
}

#endif
