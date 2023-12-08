set_project("moxt")
set_version("1.0.0")
set_xmakever("2.8.5")

set_encodings("utf-8")
set_languages("c++20")  -- c++20/c++23

add_rules("mode.debug", "mode.release")

add_repositories("my-repo-local myrepo")
-- add_repositories("my-repo git@github.com:numenresearch/xmake-repo.git")

add_requires("simdjson-local", {alias = 'simdjson'})

if not is_plat("windows") then
	add_requires("libbacktrace")
end

add_requires("fast_float_local", {alias = 'fast_float'})
add_requires("fmt", {configs = {header_only = true}})
add_requires("fmtlog-local", {alias = 'fmtlog'})
add_requires("nanobench")
add_requires("concurrentqueue")
add_requires("nanoid-cpp-local", {alias = 'nanoid-cpp'})
add_requires("zlib 1.2.12", {system=false})
add_requires("openssl", {system=false})
add_requires("tscns")

add_requires("boost", {configs = {json = true, thread = true, context = true, program_options = true,
    regex = true, system = true, filesystem = true, date_time = true, url = true, coroutine = true}})

add_requires("abseil")
add_requires("libaio", {system=false})  -- , configs = {shared = false}

-- sudo apt install libssl-dev
if not is_plat("windows") then
    -- sudo apt install e2fslibs-dev
    -- sudo apt install zlib1g-dev

    -- sudo apt install libssl-dev libcurl4-openssl-dev libaio-dev
    -- sudo apt install -y pkg-config
    add_requires("photonlibos", {configs = {shared = false, build_shared = false, static = "photon_sole"}})
    
    add_requires("libcds")
end

add_requires("asio")
add_requires("libuv")
add_requires("snmalloc-local", {alias = 'snmalloc'})
add_requires("spscqueue")
add_requires("mpmcqueue")
add_requires("nontype_functional")
add_requires("yyjson-local", {alias = 'yyjson'})

-- include subprojects
includes("*/*xmake.lua")
