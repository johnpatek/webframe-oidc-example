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
    void application::configure_desktop(webframe::desktop_config *config)
    {
        std::cerr << "[oidcapp] configure_desktop: enter" << std::endl;
        _cache_handler = std::make_unique<cache_handler>();
        std::cerr << "[oidcapp] configure_desktop: exit" << std::endl;
    }

    void application::configure_server(webframe::server_config *config, int argc, const char **argv)
    {
        std::cerr << "[oidcapp] configure_server: enter argc=" << argc << std::endl;
        argparser args;
        args.parse(argc, argv);
        config->set_port(args.get_port());
        std::cerr << "[oidcapp] configure_server: parsed port=" << args.get_port()
                  << " archive=\"" << args.get_archive_path() << "\"" << std::endl;
        _archive_handler.open_archive(args.get_archive_path());
        std::cerr << "[oidcapp] configure_server: archive opened" << std::endl;
    }

    void application::configure_router(webframe::router *ctrl)
    {
        std::cerr << "[oidcapp] configure_router: enter cache_handler="
                  << (_cache_handler ? "yes" : "no") << std::endl;
        if(_cache_handler)
        {
            ctrl->add_route("/cacheIPC", _cache_handler.get());
            std::cerr << "[oidcapp] configure_router: added /cacheIPC" << std::endl;
        }
        ctrl->set_default(&_archive_handler);
        std::cerr << "[oidcapp] configure_router: default handler set" << std::endl;
    }

    void application::launch_desktop(webframe::desktop_context *context)
    {
        std::cerr << "[oidcapp] launch_desktop: enter" << std::endl;
        const std::filesystem::path exe_path(context->get_exe_path());
        const std::filesystem::path exe_dir = exe_path.parent_path();
        std::filesystem::path archive_path(exe_dir);
        std::filesystem::path cache_path(exe_dir);
        archive_path.append("desktop.db");
        cache_path.append("cache.db");
        std::cerr << "[oidcapp] launch_desktop: archive=\"" << archive_path.string()
                  << "\" cache=\"" << cache_path.string() << "\"" << std::endl;
        _archive_handler.open_archive(archive_path.string());
        _cache_handler->open_cache(cache_path.string());
        std::cerr << "[oidcapp] launch_desktop: resources opened" << std::endl;
        webframe::application::launch_desktop(context);
        std::cerr << "[oidcapp] launch_desktop: base launch returned" << std::endl;
    }

    void application::launch_server(webframe::server_context *context)
    {
        std::cerr << "[oidcapp] launch_server: enter" << std::endl;
        webframe::application::launch_server(context);
        std::cerr << "[oidcapp] launch_server: base launch returned" << std::endl;
    }
}

WEBFRAME_MAIN(oidcapp::application)