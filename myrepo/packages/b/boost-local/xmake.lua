package("boost-local")
    set_homepage("https://github.com/boostorg/beast")
    set_description("HTTP and WebSocket built on Boost.Asio in C++11")

    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "../../../../contrib/beast-boost-1.82.0"))

    on_install("windows", "linux", "macosx", "android", "iphoneos", "cross", function (package)
        local configs = {
            "-DBeast_BUILD_EXAMPLES=OFF",
            "-DBeast_BUILD_TESTS=OFF",
            "-DBeast_ENABLE_HANDLER_TRACKING=OFF",
            "-DBoost_USE_STATIC_LIB=ON"
        }
        import("package.tools.cmake").install(package, configs)
        os.cp("include/*", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            -- #include <boost/beast/core.hpp>
            -- #include <boost/beast/http.hpp>
            -- #include <boost/beast/version.hpp>
            -- #include <boost/asio/connect.hpp>
            -- #include <boost/asio/ip/tcp.hpp>
            -- #include <cstdlib>
            -- #include <iostream>
            -- #include <string>

            -- namespace beast = boost::beast;     // from <boost/beast.hpp>
            -- namespace http = beast::http;       // from <boost/beast/http.hpp>
            -- namespace net = boost::asio;        // from <boost/asio.hpp>
            -- using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
            static void test() {
                
            }
        ]]}, {includes = "boost/beast.core.hpp",configs = {languages = "c++11"}}))
    end)