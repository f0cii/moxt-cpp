package("spscqueue")
    set_homepage("https://github.com/rigtorp/SPSCQueue")
    set_description("A bounded single-producer single-consumer wait-free and lock-free queue written in C++11")
    set_license("MIT")

    add_urls("https://github.com/rigtorp/SPSCQueue/archive/refs/tags/$(version).tar.gz",
             "https://github.com/rigtorp/SPSCQueue.git")
    add_versions("v1.1", "10f290c72e60ba0d7999c65926f96fab0b1c2f360ce5bbcfe0cf69448d067af5")

    add_deps("cmake")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            static void test() {
            }
        ]]}, {includes = "iostream", configs = {languages = "c++20"}}))
    end)
