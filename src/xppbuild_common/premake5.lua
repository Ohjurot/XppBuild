-- Project
project "XppBuild-Common"
    -- Basic project setup
    kind "StaticLib"
    xpp_cpp_project("xppbuild_common")
    xpp_cpp_macros()
    
    -- Custom defines
    defines { "XPP_BUILD_COMMON" }
