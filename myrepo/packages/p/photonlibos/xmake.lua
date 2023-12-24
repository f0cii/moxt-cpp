package("photonlibos")
    set_homepage("https://github.com/alibaba/PhotonLibOS")
    set_description("Write fast and agile C++ programs with Photon!")
    set_license("Apache-2.0")

    add_urls("https://github.com/alibaba/PhotonLibOS.git")
    -- add_urls("https://github.com/numenresearch/PhotonLibOS.git")
    
    -- add_urls("https://github.com/alibaba/PhotonLibOS/archive/refs/tags/$(version).tar.gz",
    --           "https://github.com/alibaba/PhotonLibOS.git")
    -- add_versions("v0.7.1", "59aa4d377df26917295870515823eada787d6ffe0921a577c69e96d995d803eb")
    -- add_versions("v0.7.3", "73f31ee330d100a4a59e5c6c4b525b3ef51fd8502997dc771a12264f627f1962")

    add_deps("cmake")

    on_install(function (package)
        local configs = {}

        -- table.insert(configs, {"-Wno-error=unused-result", "-Wno-error=packed"})
        table.insert(configs, "-DPHOTON_CXX_STANDARD=20")

        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        
        -- 添加编译参数
        -- table.insert(configs, {"-fpack-struct"})
        
        import("package.tools.cmake").install(package, configs)
        if package:is_plat("windows") then
            -- os.trycp(path.join(package:buildir(), "output", "*.dll"), package:installdir("bin"))
            os.trycp(path.join(package:buildir(), "output", "*.lib"), package:installdir("lib"))
        else
            -- os.trycp(path.join(package:buildir(), "output", "*.so"), package:installdir("lib"))
            os.trycp(path.join(package:buildir(), "output", "*.a"), package:installdir("lib"))
        end
        os.cp(path.join("include", "*"), package:installdir("include"))
    end)

    on_test(function (package)
        -- assert(package:has_cfuncs("foo", {includes = "foo.h"}))
    end)
