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

#ifndef OIDCAPP_HPP
#define OIDCAPP_HPP

#include <argparse/argparse.hpp>
#include <hyperpage.hpp>
#include <webframe.hpp>
#include <unqlite.h>

#include <string>
#include <string_view>

namespace oidcapp
{
    /**
     * archive_handler
     *
     * This is just for loading static content. It is not of any
     * particular importance beyond providing an interface for the
     * user to interact with the relevant OIDC endpoints.
     */
    class archive_handler : public webframe::handler
    {
    public:
        archive_handler() = default;
        ~archive_handler() = default;
        
        void open_archive(const std::string &path);

        void handle_get(const webframe::request *req, webframe::response *res) override;
        
    private:
        std::unique_ptr<hyperpage::reader> _archive;
    };

    /**
     * cache_handler
     *
     * Local storage access for desktop apps to mitigate security
     * issues with the WebView cookie storage.
     */
    class cache_handler : public webframe::handler
    {
    public:
        cache_handler() = default;
        ~cache_handler() = default;

        void open_cache(const std::string &path);
        
        void handle_get(const webframe::request *req, webframe::response *res) override;
        void handle_put(const webframe::request *req, webframe::response *res) override;
        void handle_delete(const webframe::request *req, webframe::response *res) override;
        
    private:
        static std::string parse_key(const webframe::request *req);
        static std::string kvget(unqlite *db, const std::string &key);
        static void kvput(unqlite *db, const std::string &key, const std::string_view &value);
        static void kvdel(unqlite *db, const std::string &key);
        std::unique_ptr<unqlite, decltype(&unqlite_close)> _db{nullptr, &unqlite_close};
    };

    /**
     * argparser
     * 
     * Parses command-line arguments for server runtime.
     */
    class argparser
    {
    public:
        argparser();
        ~argparser() = default;

        void parse(int argc, const char **argv);

        int get_port() const;
        const std::string& get_archive_path() const;

    private:
        int _port;
        std::string _archive_path;
        ::argparse::ArgumentParser _parser;
    };

    class application : public webframe::application
    {
    public:
        void configure_desktop(webframe::desktop_config *config) override;
        void configure_server(webframe::server_config *config, int argc, const char **argv) override;
        void configure_router(webframe::router *ctrl) override;
        void launch_desktop(webframe::desktop_context *context) override;
        void launch_server(webframe::server_context *context) override;

    private:
        archive_handler _archive_handler;
        std::unique_ptr<cache_handler> _cache_handler;
    };

}

#endif