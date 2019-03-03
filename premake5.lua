-- File: premake5.lua

-- Brief: Build script for the premake build system. Run 'premake5 gmake' to 
--        build GNU specific makefiles. 

-- Author: Alexander DuPree

-- WORKSPACE CONFIGURATION --
workspace "BigInt"
    configurations { "debug", "release", "tests"}
    platforms { "x64", "x86", "8bit" }

    if _ACTION == "clean" then
        os.rmdir("bin/")
        os.rmdir("gmake/")
        os.rmdir("gmake2/")
    end

    local project_action = "UNDEFINED"
    if _ACTION ~= nill then project_action = _ACTION end

    location (project_action)

    -- PLATFORM CONFIGURATIONS --
    filter "platforms:x64"
        defines "BIGINT__x64"
        architecture "x86_64"

    filter "platforms:x86" 
        defines "BIGINT__x86"
        architecture "x86"

    filter "platforms:8bit"
        defines "BIGINT__8bit"

    -- COMPILER/LINKER CONFIG --
    flags "FatalWarnings"
    warnings "Extra"

    filter "configurations:debug*"   
        defines { "DEBUG" } 
        symbols "On"

    filter "configurations:release*" 
        defines { "NDEBUG" } 
        optimize "On"

    filter "configurations:tests*"  
        defines { "UNIT_TESTS" } 
        symbols "On"

    filter "toolset:gcc"
        buildoptions { 
            "-Wall", "-Wextra", "-Werror"
        }

    filter {} -- close filter

project "BigInt"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"
    targetname "BigInt_%{cfg.platform}"

    local source = "src/"
    local include = "include/"

    files (source .. "**.c")
    includedirs (include)

    filter { "action:gmake or action:gmake2" }
        buildoptions "-std=c11"

    filter {} -- close filter

project "Tests"
    kind "ConsoleApp"
    language "C++"
    links "BigInt"
    targetdir "bin/%{cfg.platform}/tests/"
    targetname "%{cfg.platform}_tests"

    local include  = "include/"
    local test_src = "tests/"
    local test_inc = "third_party/"

    files (test_src .. "**.cpp")

    includedirs { test_inc, include }

    filter { "action:gmake or action:gmake2" }
        buildoptions "-std=c++11"

    filter { "configurations:Tests" }
        postbuildcommands ".././bin/%{cfg.platform}/tests/%{cfg.platform}_tests"

    filter {} -- close filter

