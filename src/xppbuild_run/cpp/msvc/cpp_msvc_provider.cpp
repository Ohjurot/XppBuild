#include "cpp_msvc_provider.h"

#include <boost/process.hpp>

int xpp::run::cpp::MsvcProvider::build_cpp_config(const CppBuildConfig& cpp_config, const Json::Value& impl_build_config, BuildEnv& env)
{
    int return_code = -1;

    // Extract msvc options
    const Json::Value toolchain_v = impl_build_config.get("toolchain", Json::nullValue);
    const Json::Value win_sdk_dir_v = impl_build_config.get("win_sdk_dir", Json::nullValue);
    const Json::Value win_sdk_version_v = impl_build_config.get("win_sdk_version", Json::nullValue);
    const Json::Value build_for_uwp_v = impl_build_config.get("build_for_uwp", false);
    const Json::Value static_runtime_v = impl_build_config.get("static_runtime", false);
    const Json::Value clr_v = impl_build_config.get("clr", Json::nullValue);

    // Validate values
    if (
        (toolchain_v != Json::nullValue && toolchain_v.isString()) &&
        (win_sdk_dir_v != Json::nullValue && win_sdk_dir_v.isString()) &&
        (win_sdk_version_v != Json::nullValue && win_sdk_version_v.isString()) &&
        (build_for_uwp_v.isBool()) &&
        (static_runtime_v.isBool()) &&
        (clr_v == Json::nullValue || clr_v.isArray())
        )
    {
        // Extract common arguments
        const std::filesystem::path toolchain_path = toolchain_v.asString();
        const std::filesystem::path win_sdk_include_path = win_sdk_dir_v.asString() / std::filesystem::path("Include") / win_sdk_version_v.asString();
        const std::filesystem::path win_sdk_lib_path = win_sdk_dir_v.asString() / std::filesystem::path("Lib") / win_sdk_version_v.asString();
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
        std::filesystem::path msvc_ld_exe = util::make_exe_path(toolchain_path / "bin" / "HostX64" / msvc_compiler_sub_dir / "link");

        // Arguments for cl
        std::vector<std::string> msvc_cl_args;

        // Build default arguments
        add_msvc_include_path(msvc_cl_args, toolchain_path, win_sdk_include_path);
        add_msvc_cl_args(msvc_cl_args, cpp_config, static_runtime);

        // Assert object dir existance
        if (!std::filesystem::exists(cpp_config.intermediate_dir))
            std::filesystem::create_directories(cpp_config.intermediate_dir);

        // Clear old pdb
        if (std::filesystem::exists(std::filesystem::path(cpp_config.output_file).replace_extension("pdb")))
            std::filesystem::remove(std::filesystem::path(cpp_config.output_file).replace_extension("pdb"));

        // Source files
        std::vector<std::filesystem::path> obj_files;
        for (auto it = cpp_config.source_files.begin(); it != cpp_config.source_files.end(); ++it)
        {
            // Generate out path
            std::filesystem::path obj_file = (cpp_config.intermediate_dir / (*it).filename()).replace_extension("obj");
            obj_files.push_back(obj_file);

            // Compile a single file
            return_code = compile_singel_file(msvc_cl_args, msvc_cl_exe, *it, obj_file, env);
            if (return_code != 0)
                break;
        }

        // Check if linking can be done
        if (return_code == 0)
        {
            // Invoking linking
            if (cpp_config.output_type != OutputType::StaticLib)
            {
                return_code = link_exe_or_dll(cpp_config, obj_files, msvc_ld_exe, toolchain_path, win_sdk_lib_path, env);
            }
            else
            {
                // TODO: Lib linking
            }

            // Evaluate
            if (return_code == 0)
            {
                
            }
            else
            {
                env.err << "Failed to link sources\n";
            }
        }
        else
        {
            env.err << "C++ source compilation failed! skipping linking\n";
        }
    }
    else
    {
        env.err << "Error in msvc build configuration: Requires all of the following parameters "
            << "[string: toolchain] and optional but type fixed parameters [boo: build_for_uwp, bool: static_runtime, array: clr_v]\n";
    }

    return return_code;
}

void xpp::run::cpp::MsvcProvider::add_msvc_include_path(std::vector<std::string>& msvc_cl_args, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path)
{
    // stdlib include directory
    args_add_include_path(msvc_cl_args, msvc_toolchain_path / "atlmfc" / "include");
    args_add_include_path(msvc_cl_args, msvc_toolchain_path / "include");

    // WinSDK include
    if (win_sdk_path != "")
    {
        args_add_include_path(msvc_cl_args, win_sdk_path / "ucrt");
        args_add_include_path(msvc_cl_args, win_sdk_path / "um");
        args_add_include_path(msvc_cl_args, win_sdk_path / "shared");
        args_add_include_path(msvc_cl_args, win_sdk_path / "winrt");
        args_add_include_path(msvc_cl_args, win_sdk_path / "cppwinrt");
    }
}

void xpp::run::cpp::MsvcProvider::add_msvc_lib_path(std::vector<std::string>& msvc_link_args, const CppBuildConfig& cpp_config, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path)
{
    // stdlib lib path
    args_add_lib_path(msvc_link_args, msvc_toolchain_path / "lib" / (cpp_config.architecture == Architecture::x64 ? "x64" : "x86"));
    args_add_lib_path(msvc_link_args, msvc_toolchain_path / "atlmfc" / "lib" / (cpp_config.architecture == Architecture::x64 ? "x64" : "x86"));

    // WinSDK lib path
    args_add_lib_path(msvc_link_args, win_sdk_path / "ucrt" / (cpp_config.architecture == Architecture::x64 ? "x64" : "x86"));
    args_add_lib_path(msvc_link_args, win_sdk_path / "um" / (cpp_config.architecture == Architecture::x64 ? "x64" : "x86"));
}

void xpp::run::cpp::MsvcProvider::add_msvc_cl_args(std::vector<std::string>& msvc_cl_args, const CppBuildConfig& cpp_config, bool static_runtime)
{
    // Static setup
    args_add(msvc_cl_args, 3, "/nologo", "/c", "/GA"); // No copyright, Only compile, Optimize for windows, 

    // Include directory
    for (auto it = cpp_config.include_dirs.begin(); it != cpp_config.include_dirs.end(); ++it)
    {
        args_add_include_path(msvc_cl_args, *it);
    }

    // Preprocessor macros
    for (auto it = cpp_config.preprocessor_macros.begin(); it != cpp_config.preprocessor_macros.end(); ++it)
    {
        args_add_define(msvc_cl_args, *it);
    }

    // Debugging
    if (cpp_config.is_debug_build)
    {
        args_add(msvc_cl_args, 2, "/JMC", "/Zi");
    }

    // Runtime
    if (cpp_config.is_debug_build)
    {
        if (static_runtime)
        {
            args_add(msvc_cl_args, "/MTd");
        }
        else
        {
            args_add(msvc_cl_args, "/MDd");
        }
    }
    else
    {
        if (static_runtime)
        {
            args_add(msvc_cl_args, "/MT");
        }
        else
        {
            args_add(msvc_cl_args, "/MD");
        }
    }

    // PDB File
    args_add(msvc_cl_args, "/Fd" + std::filesystem::path(cpp_config.output_file).replace_extension("pdb").generic_string());

    // SIMD
    switch (cpp_config.max_simd_level)
    {
        case SimdSets::IA32:
            args_add(msvc_cl_args, "/arch:IA32");
            break;
        case SimdSets::SSE:
            args_add(msvc_cl_args, "/arch:SSE");
            break;
        case SimdSets::SSE2:
            args_add(msvc_cl_args, "/arch:SSE2");
            break;
        case SimdSets::AVX:
            args_add(msvc_cl_args, "/arch:AVX");
            break;
        case SimdSets::AVX2:
            args_add(msvc_cl_args, "/arch:AVX2");
            break;
        case SimdSets::AVX512:
            args_add(msvc_cl_args, "/arch:AVX512");
            break;
    }

    // Exception behaviour
    switch (cpp_config.exception_mode)
    {
        case ExceptionMode::std:
        case ExceptionMode::std_strict:
            args_add(msvc_cl_args, "/EHsc");
            break;
    }

    // Floating point model
    switch (cpp_config.floatingpoint_mode)
    {
        case FPMode::fast:
            args_add(msvc_cl_args, "/fp:fast");
            break;
        case FPMode::percise:
            args_add(msvc_cl_args, "/fp:precise");
            break;
        case FPMode::strict:
            args_add(msvc_cl_args, "/fp:strict");
            break;
    }

    // String pooling
    if (cpp_config.string_pooling)
    {
        args_add(msvc_cl_args, "/GF");
    }

    // Optimization
    switch (cpp_config.optimization_mode)
    {
        case OptimizationMode::size:
            args_add(msvc_cl_args, "/O1");
            break;
        case OptimizationMode::speed:
            args_add(msvc_cl_args, "/O2");
            break;
    }

    // RTTI
    args_add(msvc_cl_args, std::string("/GR") + (cpp_config.enable_rtti ? "" : "-"));

    // TODO: PCH

    // Cpp Verison
    switch (cpp_config.cpp_version)
    {
        case CppVersion::c11:
            args_add(msvc_cl_args, "/std:c11");
            break;
        case CppVersion::c17:
            args_add(msvc_cl_args, "/std:c17");
            break;
        case CppVersion::cpp14:
            args_add(msvc_cl_args, "/std:c++14");
            break;
        case CppVersion::cpp17:
            args_add(msvc_cl_args, "/std:c++17");
            break;
        case CppVersion::cpp20:
            args_add(msvc_cl_args, "/std:c++20");
            break;
    }

    // Permissive
    args_add(msvc_cl_args, "/permissive" + std::string(cpp_config.strict_cpp ? "-" : ""));

    // C++ coroutines and modules
    if (cpp_config.cpp_enable_coroutines)
        args_add(msvc_cl_args, "/await");
    if (cpp_config.cpp_enable_modules)
        args_add(msvc_cl_args, "/experimental:module");

    // TOOD: MSVC custom args

    // Additional args
    for (auto it = cpp_config.compiler_args.begin(); it != cpp_config.compiler_args.end(); ++it)
    {
        args_add(msvc_cl_args, *it);
    }
}

void xpp::run::cpp::MsvcProvider::add_msvc_linker_args(std::vector<std::string>& msvc_link_args, const CppBuildConfig& cpp_config, const std::filesystem::path& msvc_link_exe, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path)
{
    // Default args
    args_add(msvc_link_args, 7, "/NOLOGO", "/MANIFEST", "/NXCOMPAT", "/DYNAMICBASE", "/INCREMENTAL", "/ERRORREPORT:PROMPT", "/MANIFESTUAC:level = 'asInvoker' uiAccess = 'false'", "/TLBID:1");

    // Debug
    if (cpp_config.is_debug_build)
    {
        args_add(msvc_link_args, "/DEBUG");
    }

    // Machine target
    args_add(msvc_link_args, "/MACHINE:" + std::string(cpp_config.architecture == Architecture::x64 ? "X64" : "X86"));

    // DLL / App-Console / App-Windows
    if (cpp_config.output_type == OutputType::ConsoleApp)
    {
        args_add(msvc_link_args, "/SUBSYSTEM:CONSOLE");
    }
    else if (cpp_config.output_type == OutputType::NativeApp)
    {
        args_add(msvc_link_args, "/SUBSYSTEM:WINDOWS");
    }
    else if (cpp_config.output_type == OutputType::DynamicLib)
    {
        args_add(msvc_link_args, "/DLL");
    }

    // Add lib dirs
    add_msvc_lib_path(msvc_link_args, cpp_config, msvc_toolchain_path, win_sdk_path);

    // Output files
    std::filesystem::path out_file = cpp_config.output_file;
    std::filesystem::path pdb_file = std::filesystem::path(out_file).replace_extension("pdb");
    args_add(msvc_link_args, "/OUT:" + out_file.generic_string());
    args_add(msvc_link_args, "/PDB:" + pdb_file.generic_string());

    // Intermediate file
    std::filesystem::path manifest_file = (cpp_config.intermediate_dir / out_file.filename()).replace_extension(".intermediate.manifest");
    std::filesystem::path iobj_file = (cpp_config.intermediate_dir / out_file.filename()).replace_extension("iobj");
    std::filesystem::path ilk_file = (cpp_config.intermediate_dir / out_file.filename()).replace_extension("ilk");
    args_add(msvc_link_args, "/ManifestFile:" + manifest_file.generic_string());
    args_add(msvc_link_args, "/LTCGOUT:" + iobj_file.generic_string());
    args_add(msvc_link_args, "/ILK:" + ilk_file.generic_string());

    // Lib dirs
    for (auto it = cpp_config.lib_dirs.begin(); it != cpp_config.lib_dirs.end(); ++it)
    {
        args_add_lib_path(msvc_link_args, *it);
    }

    // Link libs
    for (auto it = cpp_config.lib_links.begin(); it != cpp_config.lib_links.end(); ++it)
    {
        args_add(msvc_link_args, *it);
    }

    // Additional arguments
    for (auto it = cpp_config.linker_args.begin(); it != cpp_config.linker_args.end(); ++it)
    {
        args_add(msvc_link_args, *it);
    }
}


int xpp::run::cpp::MsvcProvider::compile_singel_file(std::vector<std::string>& msvc_cl_args, const std::filesystem::path& msvc_cl_exe, const std::filesystem::path& source_file, const std::filesystem::path& obj_file, BuildEnv& env)
{
    int return_code = -1;

    if (std::filesystem::exists(source_file))
    {
        // Add out file and source file
        msvc_cl_args.push_back("/Fo" + obj_file.generic_string());
        msvc_cl_args.push_back(source_file.generic_string());

        // Invoke cpp build
        boost::process::ipstream bp_msvc_cl_out;
        boost::process::ipstream bp_msvc_cl_err;
        boost::process::child msvc_cl_process(msvc_cl_exe.generic_string(), boost::process::args(msvc_cl_args), boost::process::std_out > bp_msvc_cl_out, boost::process::std_err > bp_msvc_cl_err);
        msvc_cl_process.wait();

        // Read back
        util::copy_stream(env.out, bp_msvc_cl_out);
        util::copy_stream(env.err, bp_msvc_cl_err);

        // Remove out and source file again
        msvc_cl_args.erase(msvc_cl_args.end() - 1);
        msvc_cl_args.erase(msvc_cl_args.end() - 1);

        // Check result of single source compile
        if (msvc_cl_process.exit_code() == 0)
        {
            return_code = 0;
        }
        else
        {
            env.err << "cl.exe failed with code " << msvc_cl_process.exit_code() << "\n";
        }
    }
    else
    {
        env.err << "Can't find file " << source_file << " (SKIPPED)\n";
    }

    return return_code;
}

int xpp::run::cpp::MsvcProvider::link_exe_or_dll(const CppBuildConfig& cpp_config, std::vector<std::filesystem::path> obj_file, const std::filesystem::path& msvc_link_exe, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path, BuildEnv& env)
{
    int return_code = -1;

    // Arguments for link
    std::vector<std::string> msvc_link_args;

    // Arguments
    add_msvc_linker_args(msvc_link_args, cpp_config, msvc_link_exe, msvc_toolchain_path, win_sdk_path);

    // Input object files
    for (auto it = obj_file.begin(); it != obj_file.end(); ++it)
    {
        args_add(msvc_link_args, (*it).generic_string());
    }

    // Linker action
    boost::process::ipstream bp_msvc_ld_out;
    boost::process::ipstream bp_msvc_ld_err;
    boost::process::child msvc_ld_process(msvc_link_exe.generic_string(), boost::process::args(msvc_link_args), boost::process::std_out > bp_msvc_ld_out, boost::process::std_err > bp_msvc_ld_err);
    msvc_ld_process.wait();

    // Read back
    util::copy_stream(env.out, bp_msvc_ld_out);
    util::copy_stream(env.err, bp_msvc_ld_err);

    // Check code
    return_code = msvc_ld_process.exit_code();
    if (return_code != -1)
    {
        env.result["build_artifacts"].append(cpp_config.output_file.generic_string());
    }

    return return_code;
}
