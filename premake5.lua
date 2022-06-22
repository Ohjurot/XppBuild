-- Includes
include("conanbuildinfo.premake.lua")
include("scripts/premake_functions.lua")

-- Main Workspace
workspace "XppBuild"
    -- Import conan gennerate config
    conan_basic_setup()

    -- Required projects
    include("src/xppbuild_run/premake5.lua")
