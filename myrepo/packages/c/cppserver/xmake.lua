package("cppserver")
    set_homepage("https://github.com/chronoxor/CppServer")
    set_description("Ultra fast and low latency asynchronous socket server & client C++ library with support TCP, SSL, UDP, HTTP, HTTPS, WebSocket protocols and 10K connections problem solution")
    set_license("MIT")

    add_urls("https://github.com/chronoxor/CppServer/-/archive/$(version).tar.gz",
             "https://github.com/chronoxor/CppServer.git")

    add_versions("1.0.4.1", "252f66d6c245641b9ed73eae77324513e597debd0615523d16b83b043d31035f")

    add_deps("cmake")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets()) 
    end)
