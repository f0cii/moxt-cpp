package("libcds")
    set_homepage("http://libcds.sourceforge.net/doc/cds-api/index.html")
    set_description("A C++ library of Concurrent Data Structures")

    add_urls("https://github.com/khizmax/libcds/archive/refs/tags/$(version).tar.gz",
             "https://github.com/khizmax/libcds.git")
    add_versions("v2.3.3", "f090380ecd6b63a3c2b2f0bdb27260de2ccb22486ef7f47cc1175b70c6e4e388")

    add_deps("cmake")
    add_deps("boost")

    on_install(function (package)
        local configs = {}
        io.writefile("xmake.lua", [[
            add_rules("mode.release", "mode.debug")
            target("libcds")
                set_kind("$(kind)")
                add_files("src/*.cpp")
                add_includedirs("./")
                add_headerfiles("include/(**.h)")
        ]])
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
        os.cp("include/*.h", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            static void test() {
            }
        ]]}, {includes = "iostream", configs = {languages = "c++20"}}))
    end)
