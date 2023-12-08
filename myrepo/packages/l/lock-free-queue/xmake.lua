package("lock-free-queue")
    set_homepage("https://github.com/alpc62/lock-free-queue")
    set_description("C/C++Non-Blocking Lock-Free/Wait-Free Circular-Queue")
    set_license("Apache-2.0")

    add_urls("https://github.com/alpc62/lock-free-queue.git")

    on_install(function (package)
        os.cp(path.join("include", "*.hpp"), package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets()) 
    end)
