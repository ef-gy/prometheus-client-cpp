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

#if !defined(PROMETHEUS_HTTP_H)
#define PROMETHEUS_HTTP_H

#include <ef.gy/http.h>
#include <prometheus/metric.h>

namespace prometheus {
namespace http {
static const std::string regex = "/metrics";

template <class transport>
static bool http(typename efgy::net::http::server<transport>::session &session,
                 collector::registry<collector::base> &reg) {
  session.reply(200, "Content-Type: text/plain; version=0.0.4\n", reg.text());

  return true;
}

template <class transport>
static bool
common(typename efgy::net::http::server<transport>::session &session,
       std::smatch &) {
  return http<transport>(session,
                         collector::registry<collector::base>::common());
}
}
}

#endif
