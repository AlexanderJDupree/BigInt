-- File: premake5.lua

-- Brief: Build script for the premake build system. Run 'premake5 gmake' to 
--        build GNU specific makefiles. 

-- Author: Alexander DuPree

-- WORKSPACE CONFIGURATION --
workspace "BigInt"
    configurations { "Debug", "Release" }
    platforms { "x64", "x86" }

    local project_action = "UNDEFINED"
    if _ACTION ~= nill then project_action = _ACTION end

    location (project_action)

    -- Remove -- to put gmake files at top directory
    -- filter "action:gmake*" 
     --   location "."

    -- PLATFORM CONFIGURATIONS --
    filter "x64"
        defines "BIGINT__x64"
        architecture "x86_64"

    filter "x86" 
        defines "BIGINT__x86"
        architecture "x86"

    -- COMPILER/LINKER CONFIG --
    flags "FatalWarnings"
    warnings "Extra"

    filter "configurations:Debug*" defines   { "DEBUG" } symbols "On"
    filter "configurations:Release*" defines { "NDEBUG" } optimize "On"

    filter "toolset:gcc"
        buildoptions { 
            "-Wall", "-Wextra", "-Werror"
        }

    filter {} -- close filter

project "BigInt"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.platform}/%{cfg.shortname}"
    targetname "BigInt"
    buildoptions "-std=c11"

    local source = "src/"
    local include = "include/"

    files (source .. "**.c")
    includedirs (include)

project "Tests"
    kind "ConsoleApp"
    language "C++"
    links "BigInt"
    targetdir "bin/%{cfg.platform}/tests/"
    targetname "test_%{cfg.shortname}"
    buildoptions "-std=c++11"

    local include  = "include/"
    local test_src = "tests/"
    local test_inc = "third_party/"

    files (test_src .. "**.cpp")

    includedirs { test_inc, include }

