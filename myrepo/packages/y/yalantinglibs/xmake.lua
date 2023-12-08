package("yalantinglibs")
    set_homepage("https://alibaba.github.io/yalantinglibs/")
    set_description("A collection of C++20 libraries, include coro_rpc, struct_pack, struct_json, struct_xml, struct_pb, easylog, async_simple")
    set_license("Apache-2.0")

    add_urls("https://github.com/alibaba/yalantinglibs/archive/refs/tags/$(version).tar.gz",
             "https://github.com/alibaba/yalantinglibs.git")
    add_versions("v0.1.1", "60ab50756c556d41c4d77ec63da3cfc0717e74aa37db9c309016810551499448")
    add_versions("v0.2.0", "4b087a240e547aa47cf03f286d32dc6bbb202ec3926101f69e459f888a94822a")
    add_versions("v0.2.5", "8b1cd59153d6f9ab0329ee097111bd5e84193386ade97e4008b52e596b4a520e")

    add_deps("cmake")

    on_install(function (package)
        os.cp(path.join("include", "*"), package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <iostream>
            static void test() {
            }
        ]]}, {includes = "ylt/struct_pack.hpp", configs = {languages = "c++20"}}))
    end)