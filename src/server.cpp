/* Prometheus Example Server
 *
 * An example HTTP server that only serves /metrics and a 404 on all other
 * resources.
 *
 * Call it like this:
 *
 *     $ ./server http:localhost:8080
 *
 * With localhost and 8080 being a host name and port of your choosing. Then,
 * while the programme is running, open a browser and go to
 * http://localhost:8080/metrics and you should see the rather small list of
 * metrics.
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

#define ASIO_DISABLE_THREADS
#define USE_DEFAULT_IO_MAIN
#include <prometheus/httpd.h>

#include <cxxhttp/httpd-options.h>
#include <cxxhttp/httpd-trace.h>
