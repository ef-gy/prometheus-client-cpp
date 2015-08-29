/**\file
 * \ingroup example-programmes
 * \brief "Hello World" HTTP Server
 *
 * An example HTTP server that serves a simple "Hello World!" on /, and a 404 on
 * all other resources.
 *
 * Call it like this:
 * \code
 * $ ./hello http:localhost:8080
 * \endcode
 *
 * With localhost and 8080 being a host name and port of your choosing. Then,
 * while the programme is running, open a browser and go to
 * http://localhost:8080/ and you should see the familiar greeting.
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
 * \see Source Code: https://github.com/jyujin/prometheus-client-cpp
 * \see Licence Terms: https://github.com/jyujin/prometheus-client-cpp/COPYING
 */

#define ASIO_DISABLE_THREADS
#include <prometheus/http.h>
#include <ef.gy/cli.h>

using namespace efgy;
using namespace prometheus;
using asio::ip::tcp;
using asio::local::stream_protocol;

/**\brief Hello World request handler
 *
 * This function serves the familiar "Hello World!" when called.
 *
 * \param[out] session The HTTP session to answer on.
 *
 * \returns true (always, as we always reply).
 */
template <class transport>
static bool hello(typename net::http::server<transport>::session &session,
                  std::smatch &) {
  session.reply(200, "Hello World!");

  return true;
}

/**\brief Hello World request handler for /quit
 *
 * When this handler is invoked, it stops the ASIO IO handler (after replying,
 * maybe...).
 *
 * \note Having this on your production server in this exact way is PROBABLY a
 *       really bad idea, unless you gate it in an upstream forward proxy. Or
 *       you have some way of automatically respawning your server. Or both.
 *
 * \param[out] session The HTTP session to answer on.
 *
 * \returns true (always, as we always reply).
 */
template <class transport>
static bool quit(typename net::http::server<transport>::session &session,
                 std::smatch &) {
  session.reply(200, "Good-Bye, Cruel World!");

  session.server.io.get().stop();

  return true;
}

template <class sock>
static std::size_t setup(net::endpoint<sock> lookup,
                         io::service &service = io::service::common()) {
  return lookup.with([&service](typename sock::endpoint & endpoint)->bool {
    net::http::server<sock> *s = new net::http::server<sock>(endpoint, service);

    s->processor.add("^/$", hello<sock>);
    s->processor.add("^/quit$", quit<sock>);
    s->processor.add(http::regex, http::common<sock>);

    return true;
  });
}

static cli::option oHTTPSocket("http:unix:(.+)", [](std::smatch &m)->bool {
  return setup(net::endpoint<stream_protocol>(m[1])) > 0;
});

static cli::option oHTTP("http:(.+):([0-9]+)", [](std::smatch &m)->bool {
  return setup(net::endpoint<tcp>(m[1], m[2])) > 0;
});

/**\brief Main function for the HTTP/IRC demo
 *
 * Main function for the network server hello world programme.
 *
 * Sets up server(s) as per the given command line arguments. Invalid arguments
 * are ignored.
 *
 * \param[in] argc Process argument count.
 * \param[in] argv Process argument vector
 *
 * \returns 0 when nothing bad happened, 1 otherwise.
 */
int main(int argc, char *argv[]) {
  try {
    int rv = cli::options<>::common().apply(argc, argv) == 0;

    io::service::common().run();

    return rv;
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  } catch (std::system_error &e) {
    std::cerr << "System Error: " << e.what() << "\n";
  }

  return 1;
}
