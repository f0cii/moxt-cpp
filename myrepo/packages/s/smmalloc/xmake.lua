package("smmalloc")
    set_homepage("https://github.com/SergeyMakeev/smmalloc")
    set_description("Blazing fast memory allocator designed for video games")
    set_license("MIT")

    add_urls("https://github.com/SergeyMakeev/smmalloc.git")
    add_versions("2023.06.21", "f262cbc2e3bd9b6dbb7cb5ad75f569aa892ad44d")

    add_deps("cmake")

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets()) 
    end)
