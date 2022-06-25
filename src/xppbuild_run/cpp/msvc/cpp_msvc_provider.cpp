#include "cpp_msvc_provider.h"

#include <boost/process.hpp>

int xpp::run::cpp::MsvcProvider::build_cpp_config(const CppBuildConfig& cpp_config, const Json::Value& impl_build_config, BuildEnv& env)
{
    int return_code = -1;

    // Extract msvc options
    const Json::Value toolchain_v = impl_build_config.get("toolchain", Json::nullValue);
    const Json::Value win_sdk_v = impl_build_config.get("win_sdk", Json::nullValue);
    const Json::Value build_for_uwp_v = impl_build_config.get("build_for_uwp", false);
    const Json::Value static_runtime_v = impl_build_config.get("static_runtime", false);
    const Json::Value clr_v = impl_build_config.get("clr", Json::nullValue);

    // Validate values
    if (
        (toolchain_v != Json::nullValue && toolchain_v.isString()) &&
        (win_sdk_v == Json::nullValue || win_sdk_v.isString()) &&
        (build_for_uwp_v.isBool()) &&
        (static_runtime_v.isBool()) &&
        (clr_v == Json::nullValue || clr_v.isArray())
        )
    {
        // Extract common arguments
        const std::filesystem::path toolchain_path = toolchain_v.asString();
        const std::filesystem::path win_sdk_path = (win_sdk_v != Json::nullValue ? win_sdk_v.asString() : "");
        const bool build_for_uwp = build_for_uwp_v.asBool();
        const bool static_runtime = static_runtime_v.asBool();

        // TODO: Extract CLR
        if (clr_v != Json::nullValue)
            throw std::runtime_error("Not implemented!");

        // Target architecture
        std::string msvc_compiler_sub_dir = "x64";
        if (cpp_config.architecture == Architecture::x32)
        {
            msvc_compiler_sub_dir = "x86";
        }

        // Path to cl exe
        std::filesystem::path msvc_cl_exe = util::make_exe_path(toolchain_path / "bin" / "HostX64" / msvc_compiler_sub_dir / "cl");

        // Arguments for cl
        std::vector<std::string> msvc_cl_args;

        // stdlib include directory
        msvc_cl_args.push_back("/I" + (toolchain_path / "atlmfc" / "include").generic_string());
        msvc_cl_args.push_back("/I" + (toolchain_path / "include").generic_string());
        
        // WinSDK include
        if (win_sdk_path != "")
        {
            msvc_cl_args.push_back("/I" + (win_sdk_path / "ucrt").generic_string());
            msvc_cl_args.push_back("/I" + (win_sdk_path / "um").generic_string());
            msvc_cl_args.push_back("/I" + (win_sdk_path / "shared").generic_string());
            msvc_cl_args.push_back("/I" + (win_sdk_path / "winrt").generic_string());
            msvc_cl_args.push_back("/I" + (win_sdk_path / "cppwinrt").generic_string());
        }

        // Invoke project specific cl arguments
        build_msvc_cl_args(msvc_cl_args, cpp_config, impl_build_config);

        // Object dir
        if (!std::filesystem::exists(cpp_config.intermediate_dir))
            std::filesystem::create_directories(cpp_config.intermediate_dir);

        // Source files
        for (auto it = cpp_config.source_files.begin(); it != cpp_config.source_files.end(); ++it)
        {
            if (std::filesystem::exists(*it))
            {
                msvc_cl_args.push_back("/Fo" + (cpp_config.intermediate_dir / (*it).filename()).replace_extension("obj").generic_string());
                msvc_cl_args.push_back((*it).generic_string());

                // Invoke cpp build
                boost::process::ipstream bp_msvc_cl_out;
                boost::process::ipstream bp_msvc_cl_err;
                boost::process::child msvc_cl_process(msvc_cl_exe.generic_string(), boost::process::args(msvc_cl_args), boost::process::std_out > bp_msvc_cl_out, boost::process::std_err > bp_msvc_cl_err);
                msvc_cl_process.wait();

                // Read back
                util::copy_stream(env.out, bp_msvc_cl_out);
                util::copy_stream(env.err, bp_msvc_cl_err);

                // Reset
                msvc_cl_args.erase(msvc_cl_args.end() - 1);
                msvc_cl_args.erase(msvc_cl_args.end() - 1);

                // Set code
                if (msvc_cl_process.exit_code() == 0)
                {
                    return_code = 0;
                }
                else
                {
                    env.err << "cl.exe failed with code " << msvc_cl_process.exit_code() << "\n";
                    break;
                }
            }
            else
            {
                env.err << "Can't find file " << *it << " (SKIPPED)\n";
            }
        }
    }
    else
    {
        env.err << "Error in msvc build configuration: Requires all of the following parameters "
            << "[string: toolchain] and optional but type fixed parameters [boo: build_for_uwp, bool: static_runtime, array: clr_v]\n";
    }

    return return_code;
}

void xpp::run::cpp::MsvcProvider::build_msvc_cl_args(std::vector<std::string>& msvc_cl_args, const CppBuildConfig& cpp_config, const Json::Value& impl_build_config)
{
    // Only compile no linking and nologo
    msvc_cl_args.push_back("/c");
    msvc_cl_args.push_back("/nologo");
    msvc_cl_args.push_back("/EHsc");

    // Include directory
    for (auto it = cpp_config.include_dirs.begin(); it != cpp_config.include_dirs.end(); ++it)
    {
        msvc_cl_args.push_back("/I" + (*it).generic_string());
    }

    // Preprocessor macros
    for (auto it = cpp_config.preprocessor_macros.begin(); it != cpp_config.preprocessor_macros.end(); ++it)
    {
        msvc_cl_args.push_back("/D" + *it);
    }

    // TODO: DEBUG

    // TODO: Code generation

    // TODO: PCH

    // Cpp Verison
    switch (cpp_config.cpp_version)
    {
    case CppVersion::c11:
        msvc_cl_args.push_back("/std:c11");
        break;
    case CppVersion::c17:
        msvc_cl_args.push_back("/std:c17");
        break;
    case CppVersion::cpp14:
        msvc_cl_args.push_back("/std:c++14");
        break;
    case CppVersion::cpp17:
        msvc_cl_args.push_back("/std:c++17");
        break;
    case CppVersion::cpp20:
        msvc_cl_args.push_back("/std:c++20");
        break;
    }

    // Permissive
    msvc_cl_args.push_back("/permissive" + std::string(cpp_config.strict_cpp ? "-" : ""));

    // C++ coroutines and modules
    if (cpp_config.cpp_enable_coroutines)
        msvc_cl_args.push_back("/await");
    if (cpp_config.cpp_enable_modules)
        msvc_cl_args.push_back("/experimental:module");

    // TOOD: MSVC custom args
}
