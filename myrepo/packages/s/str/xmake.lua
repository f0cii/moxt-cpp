package("str")
    set_homepage("https://github.com/MengRao/str")
    set_description("A SIMD optimized fixed-length string class along with an adaptive hash table for fast searching")
    set_license("MIT")

    add_urls("https://github.com/MengRao/str.git")
    add_versions("2021.07.04", "80389e30ff07bfd5d71f2bdcfad23db6c1353c2c")

    on_install(function (package)
        -- local configs = {}
        -- io.writefile("xmake.lua", [[
        --     add_rules("mode.release", "mode.debug")
        --     target("str")
        --         set_kind("$(kind)")
        --         add_files("src/*.c")
        --         add_headerfiles("src/(*.h)")
        -- ]])
        -- if package:config("shared") then
        --     configs.kind = "shared"
        -- end
        -- import("package.tools.xmake").install(package, configs)
        os.cp(path.join(".", "*.h"), package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <iostream>
            static void test() {
            }
        ]]}, {includes = "iostream", configs = {languages = "c++20"}}))
    end)
