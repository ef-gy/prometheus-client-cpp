/**\file
 * \ingroup example-programmes
 * \brief Prometheus Example Server
 *
 * An example HTTP server that only serves /metrics and a 404 on all other
 * resources.
 *
 * Call it like this:
 * \code
 * $ ./server http:localhost:8080
 * \endcode
 *
 * With localhost and 8080 being a host name and port of your choosing. Then,
 * while the programme is running, open a browser and go to
 * http://localhost:8080/metrics and you should see the rather small list of
 * metrics.
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

#define ASIO_DISABLE_THREADS
#define USE_DEFAULT_IO_MAIN
#include <prometheus/httpd.h>
