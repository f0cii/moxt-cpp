target("moxt")
	set_kind("shared")

	add_includedirs(".")
    add_includedirs("..")
	add_headerfiles("**.hpp")

	add_files("**.cpp")

	add_defines("ASIO_STANDALONE")

	if is_plat("windows") then
		add_defines("_WIN32")
		add_defines("ASYNC_SIMPLE_HAS_NOT_AIO")
		add_cxflags("/bigobj")
	else
		add_defines("ASYNC_SIMPLE_HAS_NOT_AIO");
		if not is_arch("arm64", "arm64-v8a") then
			-- linux arm64

			add_cxxflags("-mavx2")	-- SIMD
			add_cxxflags("-mcx16")	-- for snmalloc
			add_cxxflags("-msse4.1")
    		add_cxxflags("-msse4.2")
		else
			-- arm64
			-- arm64 dd_cxflags("-mcx16") is ignored
			-- add_cxxflags("-mcx16")	-- for snmalloc
		end
	end

	if not is_plat("windows") then
		add_packages("libbacktrace")
		add_cxflags("-fpermissive")
	end
	
	add_packages("fpm")
	add_packages("boost")

	add_packages("fmt")
	add_packages("fmtlog")
	add_packages("spdlog")
	add_packages("quill")
	add_packages("abseil")
	add_packages("simdjson")
	add_packages("sonic-cpp")
	add_packages("double-conversion")
	add_packages("fast_float")
    add_packages("snmalloc")
	add_packages("nanoid-cpp")
    add_packages("openssl")
    add_packages("photonlibos", {links = {"photon_sole"}})
    add_packages("libcurl")
    add_packages("libaio")
	add_packages("concurrentqueue")
    add_packages("parallel-hashmap")
	add_packages("yyjson")
	add_packages("nanobench")
    add_packages("redis-plus-plus")

	if not is_plat("windows") then
        -- add_ldflags("-latomic")	-- for snmalloc
    end

	-- 计算 Tulip Indicators 的根目录
	local tulipindicators_root = path.join(os.scriptdir(), "../")

	-- 选择 Debug 或 Release 模式
	local tulipindicators_lib_dir = ""
	if is_mode("debug") then
		tulipindicators_lib_dir = path.join(tulipindicators_root, "build/linux/x86_64/debug")
	else
		tulipindicators_lib_dir = path.join(tulipindicators_root, "build/linux/x86_64/release")
	end
	add_packages("tulipindicators")
	add_linkdirs(tulipindicators_lib_dir)
	add_links("tulipindicators")


target("moxt_sole")
	set_kind("static")

	add_includedirs(".")
    add_includedirs("..")
	add_headerfiles("**.hpp")

	add_files("**.cpp")

	add_defines("ASIO_STANDALONE")

	if is_plat("windows") then
		add_defines("_WIN32")
		add_defines("ASYNC_SIMPLE_HAS_NOT_AIO")
		add_cxflags("/bigobj")
	else
		add_defines("ASYNC_SIMPLE_HAS_NOT_AIO");
		if not is_arch("arm64", "arm64-v8a") then
			-- linux arm64

			add_cxxflags("-mavx2")	-- SIMD
			add_cxxflags("-mcx16")	-- for snmalloc
			add_cxxflags("-msse4.1")
    		add_cxxflags("-msse4.2")
		else
			-- arm64
			-- arm64 dd_cxflags("-mcx16") is ignored
			-- add_cxxflags("-mcx16")	-- for snmalloc
		end
	end

	if not is_plat("windows") then
		add_packages("libbacktrace")
		add_cxflags("-fpermissive")
	end
	add_packages("fpm")
	add_packages("boost")

	add_packages("fmt")
	add_packages("fmtlog")
	add_packages("spdlog")
	add_packages("quill")
	add_packages("abseil")
	add_packages("simdjson")
    add_packages("sonic-cpp")
	add_packages("double-conversion")
	add_packages("fast_float")
    add_packages("snmalloc")
	add_packages("nanoid-cpp")
    add_packages("openssl")
    add_packages("photonlibos", {links = {"photon_sole"}})
    add_packages("libcurl")
    add_packages("libaio")
	add_packages("concurrentqueue")
    add_packages("parallel-hashmap")
	add_packages("yyjson")
	add_packages("nanobench")
    add_packages("redis-plus-plus")
	add_packages("tulipindicators")
