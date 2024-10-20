package("fmtlog-local")
    set_homepage("https://github.com/MengRao/fmtlog")
    set_description("fmtlog is a performant fmtlib-style logging library with latency in nanoseconds.")
    set_license("MIT")

    add_urls("https://github.com/MengRao/fmtlog/archive/refs/tags/$(version).tar.gz",
             "https://github.com/MengRao/fmtlog.git")
    add_versions("v2.1.2", "d286184e04c3c3286417873dd2feac524c53babc6cd60f10179aa5b10416ead7")
    -- add_urls("https://github.com/numenresearch/fmtlog.git")

    -- add_deps("cmake", "fmt")
    add_deps("fmt", {configs = {header_only = true}})
    -- add_deps("cmake")
    if is_plat("linux") then
        add_syslinks("pthread")
    end

    on_install("linux", "macosx", "windows", function (package)
        -- local configs = {}
        -- table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        -- io.replace("CMakeLists.txt", "add_subdirectory(fmt)", "", {plain = true})
        -- io.replace("CMakeLists.txt", "add_subdirectory(test)", "", {plain = true})
        -- io.replace("CMakeLists.txt", "add_subdirectory(bench)", "", {plain = true})
        -- import("package.tools.cmake").install(package, configs, {packagedeps = "fmt"})
        -- if package:config("shared") then
        --     os.tryrm(path.join(package:installdir("lib"),  "*.a"))
        -- else
        --     os.tryrm(path.join(package:installdir("lib"),  "*.dll"))
        --     os.tryrm(path.join(package:installdir("lib"),  "*.dylib"))
        --     os.tryrm(path.join(package:installdir("lib"),  "*.so"))
        -- end
        local configs = {}
        io.writefile("xmake.lua", [[
            add_rules("mode.release", "mode.debug")
            add_requires("fmt", {configs = {header_only = true}})
            target("istrstr")
                set_kind("$(kind)")
                set_languages("c++20")
                add_files("*.cc")
                add_includedirs(".")
                add_headerfiles("fmtlog.h")
                add_packages("fmt")
        ]])
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
        os.cp("fmtlog.h", package:installdir("include/fmtlog"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                logi("A info msg");
            }
        ]]}, {configs = {languages = "c++20"}, includes = "fmtlog/fmtlog.h"}))
    end)
