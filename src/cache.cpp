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
    void cache_handler::open_cache(const std::string &path)
    {
        unqlite *db;
        if (unqlite_open(&db, path.c_str(), UNQLITE_OPEN_CREATE | UNQLITE_OPEN_READWRITE) != UNQLITE_OK)
        {
            throw std::runtime_error("Failed to open cache database");
        }
        _db.reset(db);
    }

    void cache_handler::handle_get(const webframe::request *req, webframe::response *res)
    {
        const std::string key = parse_key(req);
        const std::string value = kvget(_db.get(), key);
        res->set_status(200);
        res->set_body(reinterpret_cast<const uint8_t *>(value.data()), value.size());
    }

    void cache_handler::handle_put(const webframe::request *req, webframe::response *res)
    {
        const std::string key = parse_key(req);
        const auto &[data, size] = req->get_body();
        std::string_view value(reinterpret_cast<const char *>(data), size);
        kvput(_db.get(), key, value);
        res->set_status(200);
    }

    void cache_handler::handle_delete(const webframe::request *req, webframe::response *res)
    {
        const std::string key = parse_key(req);
        kvdel(_db.get(), key);
        res->set_status(200);
    }

    std::string cache_handler::parse_key(const webframe::request *req)
    {
        std::string key;
        webframe::uri uri(req->get_uri());
        if (!uri.get_query("key", key))
        {
            throw webframe::exception::bad_request;
        }
        return key;
    }

    std::string cache_handler::kvget(unqlite *db, const std::string &key)
    {
        std::string value;
        unqlite_int64 value_size(0);
        int rc = unqlite_kv_fetch(db, key.data(), key.size(), nullptr, &value_size);
        if (rc == UNQLITE_NOTFOUND)
        {
            throw webframe::exception::not_found;
        }
        else if (rc != UNQLITE_OK)
        {
            throw webframe::exception::internal_server_error;
        }
        value.resize(value_size);
        if (unqlite_kv_fetch(db, key.data(), key.size(), value.data(), &value_size) != UNQLITE_OK)
        {
            throw webframe::exception::internal_server_error;
        }
        return value;
    }

    void cache_handler::kvput(unqlite *db, const std::string &key, const std::string_view &value)
    {
        if (unqlite_kv_store(db, key.data(), key.size(), value.data(), value.size()) != UNQLITE_OK)
        {
            throw webframe::exception::internal_server_error;
        }
    }

    void cache_handler::kvdel(unqlite *db, const std::string &key)
    {
        const int rc = unqlite_kv_delete(db, key.data(), key.size());
        if (rc == UNQLITE_NOTFOUND)
        {
            throw webframe::exception::not_found;
        }
        else if (rc != UNQLITE_OK)
        {
            throw webframe::exception::internal_server_error;
        }
    }

}