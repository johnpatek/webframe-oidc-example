/*
 * Copyright (c) 2026 John R. Patek Sr.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <oidcapp.hpp>

#include <iostream>

namespace oidcapp
{
    argparser::argparser() : _parser("oidcapp")
    {
        std::cerr << "[oidcapp] argparser: constructor enter" << std::endl;
        _parser.add_argument("-p", "--port")
            .help("Port to listen on")
            .default_value(8080)
            .scan<'i', int>();
        _parser.add_argument("-a", "--archive")
            .help("Path to the hyperpage archive to serve")
            .required();
        std::cerr << "[oidcapp] argparser: constructor exit" << std::endl;
    }

    void argparser::parse(int argc, const char **argv)
    {
        std::cerr << "[oidcapp] argparser: parse enter argc=" << argc << std::endl;
        _parser.parse_args(argc, argv);
        std::cerr << "[oidcapp] argparser: parse_args returned" << std::endl;
        _port = _parser.get<int>("--port");
        std::cerr << "[oidcapp] argparser: port=" << _port << std::endl;
        _archive_path = _parser.get<std::string>("--archive");
        std::cerr << "[oidcapp] argparser: archive=\"" << _archive_path << "\"" << std::endl;
    }

    int argparser::get_port() const
    {
        return _port;
    }

    const std::string& argparser::get_archive_path() const
    {
        return _archive_path;
    }
}