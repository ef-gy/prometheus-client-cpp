/**\file
 * \brief Prometheus C++ client core version number
 *
 * This header contains the version number of the prometheus client headers.
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

#if !defined(PROMETHEUS_VERSION_H)
#define PROMETHEUS_VERSION_H

/**\brief Prometheus base name space
 *
 * Everything related to the prometheus client is contained in this namespace.
 */
namespace prometheus {
/**\brief Prometheus version
 *
 * This is the version number of the prometheus client library. It's a single
 * integer - how unusual ;).
 */
static const unsigned int version = 1;
}

#endif
