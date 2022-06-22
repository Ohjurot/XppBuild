-- Project
project "XppBuild-Run"
    -- Basic project setup
    xpp_cpp_project("xppbuild_run")
    xpp_cpp_macros()
    
    -- Custom defines
    defines { "XPP_BUILD_RUN" }

    -- Customize debugging
    debugargs { "-b", "java.build.jsonc" }
