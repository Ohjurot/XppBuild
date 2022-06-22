#pragma once

#include <filesystem>

#ifndef XPP_SYS_EXE_EXT
    #if defined(XPP_WINDOWS)
        #define XPP_SYS_EXE_EXT ".exe"
    #endif
    #if defined(XPP_LINUX) or defined(XPP_OSX) 
        #define XPP_SYS_EXE_EXT ""
    #endif
#endif

namespace xpp::util
{
    inline std::filesystem::path make_exe_path(const std::filesystem::path& source_file)
    {
        return source_file.generic_string() + XPP_SYS_EXE_EXT;
    }
}
