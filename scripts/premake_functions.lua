-- Basic C++ project settings
function xpp_cpp_project(src_dir_name)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "%{wks.location}/bin/%{cfg.buildcfg}/"
    objdir "%{wks.location}/bin/%{cfg.buildcfg}/obj/%{prj.name}/"
    location ("%{wks.location}/src/" .. src_dir_name)
    debugdir "%{wks.location}/app"

    linkoptions { conan_exelinkflags }

    files { 
        "%{prj.location}/**.h", 
        "%{prj.location}/**.cpp", 
        "%{prj.location}/**.lua" 
    }

    filter "configurations:Debug"
        symbols "On"
    filter {}

    filter "configurations:Release"
        optimize "On"
    filter {}
end

-- Basic C++ preprocessor macros
function xpp_cpp_macros()
    filter "configurations:Debug"
        defines { "DEBUG", "XPP_DEBUG" }
    filter {}

    filter "configurations:Release"
        defines { "NDEBUG", "XPP_RELEASE" }
    filter {}

    filter "system:windows"
        defines {"WINVER=0x0A00", "_WIN32_WINNT=0x0A00", "XPP_WINDOWS" }
    filter {}
end
