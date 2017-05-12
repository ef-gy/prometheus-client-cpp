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
static const std::string regex = "/metrics";

template <class transport>
static bool http(typename cxxhttp::http::server<transport>::session &session,
                 collector::registry<collector::base> &reg) {
  session.reply(200, {{"Content-Type", "text/plain; version=0.0.4"}},
                reg.text());

  return true;
}

template <class transport>
static bool common(typename cxxhttp::http::server<transport>::session &session,
                   std::smatch &) {
  return http<transport>(session,
                         collector::registry<collector::base>::common());
}
}
}

#endif
