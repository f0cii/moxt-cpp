package("nontype_functional")
    set_homepage("https://github.com/zhihaoy/nontype_functional")
    set_description("Complete implementation of std::function, std::function_ref, and std::move_only_function")

    --add_urls("https://github.com/zhihaoy/nontype_functional/-/archive/$(version).tar.gz",
    --         "https://github.com/zhihaoy/nontype_functional.git")
    add_urls("https://github.com/zhihaoy/nontype_functional.git")

    -- add_versions("v1.0.1", "fa1478d29e6af9cd266bcc5c1c3372c34137efdb0109d5761885a8768336f449")

    on_install(function (package)
        os.cp("include/**", package:installdir("include"))
    end)

    --on_test(function (package)
    --    assert(package:has_cfuncs("foo", {includes = "foo.h"}))
    --end)
    on_test(function (package)
        assert(package:check_cxxsnippets())
    end)
