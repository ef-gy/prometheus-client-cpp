/**\file
 * \brief libefgy HTTP servlet registration.
 *
 * This file defines static variables to automatically register libefgy HTTP
 * servlets, which are exported using the standard syntax. It also pulls in the
 * libefgy httpd proper, which in turn already sets up command line parsing for
 * 'http:...' arguments, which spawn the necessary daemons.
 *
 * TL;DR: pull in this file to export stuff to prometheus if you can't be
 * bothered to set everything up yourself.
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

#if !defined(PROMETHEUS_HTTPD_H)
#define PROMETHEUS_HTTPD_H

#include <ef.gy/httpd.h>
#include <prometheus/http.h>

namespace prometheus {
namespace httpd {
static efgy::httpd::servlet<asio::ip::tcp> tcp(http::regex,
                                               http::common<asio::ip::tcp>);
static efgy::httpd::servlet<asio::local::stream_protocol>
    unix(http::regex, http::common<asio::local::stream_protocol>);
}
}

#endif
