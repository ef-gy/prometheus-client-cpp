/* Prometheus C++ client core version number
 *
 * This header contains the version number of the prometheus client headers.
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

#if !defined(PROMETHEUS_VERSION_H)
#define PROMETHEUS_VERSION_H

namespace prometheus {
/* Prometheus version
 *
 * This is the version number of the prometheus client library. It's a single
 * integer.
 */
static const unsigned int version = 1;
}

#endif
