/* libefgy HTTP servlet registration.
 *
 * This file defines static variables to automatically register libefgy HTTP
 * servlets, which are exported using the standard syntax. It also pulls in the
 * libefgy httpd proper, which in turn already sets up command line parsing for
 * 'http:...' arguments, which spawn the necessary daemons.
 *
 * TL;DR: pull in this file to export stuff to prometheus if you can't be
 * bothered to set everything up yourself.
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

#if !defined(PROMETHEUS_HTTPD_H)
#define PROMETHEUS_HTTPD_H

#include <cxxhttp/httpd.h>
#include <prometheus/http.h>

namespace prometheus {
namespace httpd {
static cxxhttp::httpd::servlet<asio::ip::tcp> tcp(http::resource,
                                                  http::metrics<asio::ip::tcp>);
static cxxhttp::httpd::servlet<asio::local::stream_protocol> unix(
    http::resource, http::metrics<asio::local::stream_protocol>);
}
}

#endif
