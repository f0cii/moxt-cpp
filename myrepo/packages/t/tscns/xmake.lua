package("tscns")
    set_homepage("https://github.com/MengRao/tscns")
    set_description("A low overhead nanosecond clock based on x86 TSC")
    set_license("MIT")

    add_urls("https://github.com/MengRao/tscns.git")
    add_versions("2022.07.18", "580322f853f51e9133a8a1b56336fb31f70e78ad")

    on_install(function (package)
        os.cp("tscns.h", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                TSCNS tn;
                tn.init();
            }
        ]]}, {configs = {languages = "c++20"}, includes = "tscns.h"}))
    end)
