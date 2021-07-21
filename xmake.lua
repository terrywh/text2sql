-- 注意：此 xmake.lua 文件仅用于独立编译，集成编译相关依赖请使用上层 CMakeLists.txt

set_project("text2sql")
set_languages("c17","cxx17")

vendor = {
    xbond = "/data/vendor/xbond",
    boost = "/data/vendor/boost-1.75",
}

add_includedirs(
    "$(scriptdir)/vendor/sqlite",
    vendor["boost"] .. "/include")

add_linkdirs(
    vendor["xbond"] .. "/lib",
    vendor["boost"] .. "/lib")
    
add_links(
    "xbond")

add_syslinks(
    "boost_json",
    "boost_regex",
    "boost_program_options",
    "boost_filesystem",
    "boost_system",
    "pthread",
    "dl")

target("text2sql")
    set_kind("binary")
    add_rules("mode.debug", "mode.release")
    add_files("vendor/sqlite/sqlite3.c", {defines = {"SQLITE_ENABLE_JSON1"}})
    add_files("src/**.cpp")
    add_ldflags("-static")
