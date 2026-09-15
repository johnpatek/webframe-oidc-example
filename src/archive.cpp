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

namespace oidcapp
{
    void archive_handler::open_archive(const std::string &path)
    {
        _archive = std::make_unique<hyperpage::reader>(path);
    }

    void archive_handler::handle_get(const webframe::request *req, webframe::response *res)
    {
        std::string path(req->get_path());
        if(path.empty() || path == "/")
        {
            path = "/index.html";
        }
        auto page = _archive->load(path);
        if (!page)
        {
            throw webframe::exception::not_found;
        }
        res->set_status(200);
        res->set_header("Content-Type", page->get_mime_type());
        res->set_body(page->get_content(), page->get_length());
    }
}