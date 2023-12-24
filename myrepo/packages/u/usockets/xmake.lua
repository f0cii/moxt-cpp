package("usockets")
    set_homepage("https://github.com/uNetworking/uSockets")
    set_description("Miniscule cross-platform eventing, networking & crypto for async applications")
    set_license("Apache-2.0")

    add_urls("https://github.com/uNetworking/uSockets/archive/refs/tags/$(version).tar.gz",
             "https://github.com/uNetworking/uSockets.git")
    add_versions("v0.8.6", "16eba133dd33eade2f5f8dd87612c04b5dd711066e0471c60d641a2f6a988f16")

    add_deps("openssl")
    add_deps("libuv")

    -- set_languages("c99", "c++17")

    on_install(function (package)
        local configs = {}
        io.writefile("xmake.lua", [[
            add_rules("mode.release", "mode.debug")
            add_requires("openssl")
            add_requires("libuv")
            target("usockets")
                set_kind("$(kind)")
                -- set_languages("c99", "c++17")
                set_languages("c++17")
                add_files("src/*.c")
                add_files("src/eventing/*.c")
                add_files("src/crypto/*.c")
                add_files("src/crypto/*.cpp")
                add_files("src/io_uring/*.c")
                add_includedirs("src")
                add_headerfiles("src/(*.h)")
                add_defines("LIBUS_USE_OPENSSL")
                add_defines("LIBUS_USE_LIBUV")
                -- add_defines("LIBUS_USE_IO_URING")
                add_packages("openssl")
                add_packages("libuv")
        ]])
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
        -- os.cp(path.join("include", "*"), package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test()
            {
                
            }
        ]]}, {configs = {languages = "c++17"}, includes = { "libusockets.h"} }))
    end)
