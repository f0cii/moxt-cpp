package("nanoid-cpp-local")
    set_homepage("https://github.com/mcmikecreations/nanoid_cpp")
    set_description("A tiny, URL-friendly, unique string ID generator for C++, implementation of ai's nanoid!")

    add_deps("cmake")

    add_urls("https://github.com/mcmikecreations/nanoid_cpp.git")

    on_install("windows", "linux", "mingw", "macosx", "android", "iphoneos", "cross", function (package)
        local configs = {
            "-DBUILD_TESTS=OFF",
            "-DBUILD_EXAMPLES=OFF",
        }
        import("package.tools.cmake").install(package, configs)
        os.cp("inc/*", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <nanoid/nanoid.h>
            static void test() {
                
            }
        ]]}, {includes = "nanoid/nanoid.h",configs = {languages = "c++11"}}))
    end)