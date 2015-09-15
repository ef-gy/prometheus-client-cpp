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
 * Copyright (c) 2015, Magnus Achim Deininger <magnus@ef.gy>
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
