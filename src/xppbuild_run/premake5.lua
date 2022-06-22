-- Project
project "XppBuild-Run"
    -- Basic project setup
    kind "ConsoleApp"
    xpp_cpp_project("xppbuild_run")
    xpp_cpp_macros()
    
    -- Link
    links { "XppBuild-Common" }
    includedirs { "%{wks.location}/src/xppbuild_common" }

    -- Custom defines
    defines { "XPP_BUILD_RUN" }

    -- Customize debugging
    debugargs { "--buildfile", "java.build.jsonc" }
