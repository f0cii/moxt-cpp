package("cpp-optparse")
    set_homepage("https://github.com/weisslj/cpp-optparse")
    set_description("Python's excellent OptionParser in C++")
    set_license("MIT")

    add_urls("https://github.com/weisslj/cpp-optparse.git")
    add_versions("2022.05.02", "2265d647232249a53a03b411099863ceca35f0d3")

    on_install(function (package)
        local configs = {}
        io.writefile("xmake.lua", [[
            add_rules("mode.release", "mode.debug")
            target("cpp-optparse")
                set_kind("$(kind)")
                add_headerfiles("*.h")
                add_files("*.cpp")
        ]])
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
        os.cp(path.join(".", "*.h"), package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets()) 
    end)
