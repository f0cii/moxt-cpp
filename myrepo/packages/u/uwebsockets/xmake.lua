package("uwebsockets")
    set_homepage("https://github.com/uNetworking/uWebSockets")
    set_description("Simple, secure & standards compliant web server for the most demanding of applications")
    set_license("Apache-2.0")

    add_urls("https://github.com/uNetworking/uWebSockets/archive/refs/tags/$(version).tar.gz",
             "https://github.com/uNetworking/uWebSockets.git")
    add_versions("v20.44.0", "c40df632d32a41d3e8b273930eb540b90ccf731503f7c1b1aefc3cc6b4836d7c")
    add_versions("v20.51.0", "6794e7895eb8cc182024a0ae482a581eaa82f55f7cca53ae88b30738449f3cb9")

    add_deps("zlib")
    add_deps("usockets")

    on_install(function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
        os.cp(path.join("src", "*"), package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test()
            {
                
            }
        ]]}, {configs = {languages = "c++17"}, includes = { "ClientApp.h"} }))
    end)
