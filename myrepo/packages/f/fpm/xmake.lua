package("fpm")
    set_homepage("https://mikelankamp.github.io/fpm")
    set_description("C++ header-only fixed-point math library")
    set_license("MIT")

    -- add_urls("https://github.com/MikeLankamp/fpm/archive/refs/tags/$(version).tar.gz",
    --          "https://github.com/MikeLankamp/fpm.git")
    -- add_versions("v1.1.0", "e34941b4cd1d1f1fbe9ecf39b740cb20b9c272a832f63be27fa24eddf400e51b")
    add_urls("https://github.com/MikeLankamp/fpm.git")

    add_deps("cmake")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_ACCURACY=OFF")
        table.insert(configs, "-DBUILD_BENCHMARK=OFF")
        table.insert(configs, "-DBUILD_TESTS=OFF")
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <string>
            static void test() {
                
            }
        ]]}, {includes = "fpm/fixed.hpp",configs = {languages = "c++20"}}))
    end)
