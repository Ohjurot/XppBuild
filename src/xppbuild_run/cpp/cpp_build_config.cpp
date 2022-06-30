#include "cpp_build_config.h"

xpp::run::cpp::CppBuildConfig xpp::run::cpp::json_to_cpp_build_info(const Json::Value& config)
{
    CppBuildConfig build_config;

    // Source files
    const Json::Value source_files_v = config.get("source_files", Json::nullValue);
    if (source_files_v != Json::nullValue && source_files_v.isArray())
    {
        for (auto it = source_files_v.begin(); it != source_files_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.source_files.push_back(it->asString());
            }
        }
    }

    // Include dirs
    const Json::Value include_dirs_v = config.get("include_dirs", Json::nullValue);
    if (include_dirs_v != Json::nullValue && include_dirs_v.isArray())
    {
        for (auto it = include_dirs_v.begin(); it != include_dirs_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.include_dirs.push_back(it->asString());
            }
        }
    }

    // Object directory
    build_config.intermediate_dir = config.get("obj_directory", "").asString();

    // Output file
    build_config.output_file = config.get("output", "out").asString();

    // Output type
    const Json::Value output_type_v = config.get("output_type", Json::nullValue);
    if (output_type_v != Json::nullValue && output_type_v.isString())
    {
        std::string output_type_s = output_type_v.asString();
        if (output_type_s == "app\\console")
            build_config.output_type = OutputType::ConsoleApp;
        else if (output_type_s == "app\\native")
            build_config.output_type = OutputType::NativeApp;
        else if (output_type_s == "lib\\static")
            build_config.output_type = OutputType::StaticLib;
        else if (output_type_s == "lib\\dynamic")
            build_config.output_type = OutputType::DynamicLib;
    }

    // Lib dirs
    const Json::Value lib_path_v = config.get("lib_path", Json::nullValue);
    if (lib_path_v != Json::nullValue && lib_path_v.isArray())
    {
        for (auto it = lib_path_v.begin(); it != lib_path_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.lib_dirs.push_back(it->asString());
            }
        }
    }

    // Lib links
    const Json::Value lib_links_v = config.get("lib_links", Json::nullValue);
    if (lib_links_v != Json::nullValue && lib_links_v.isArray())
    {
        for (auto it = lib_links_v.begin(); it != lib_links_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.lib_links.push_back(it->asString());
            }
        }
    }

    // Preprocessor macro
    const Json::Value preprocessor_macros_v = config.get("preprocessor_macros", Json::nullValue);
    if (preprocessor_macros_v != Json::nullValue && preprocessor_macros_v.isArray())
    {
        for (auto it = preprocessor_macros_v.begin(); it != preprocessor_macros_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.preprocessor_macros.push_back(it->asString());
            }
        }
    }

    // Debug build
    build_config.is_debug_build = config.get("debug_build", false).asBool();

    // Code generation
    const Json::Value code_generation_v = config.get("code_generation", Json::nullValue);
    if (code_generation_v != Json::nullValue && code_generation_v.isObject())
    {
        // Architecture
        const Json::Value architecture_v = code_generation_v.get("architecture", Json::nullValue);
        if (architecture_v != Json::nullValue && architecture_v.isString())
        {
            std::string architecture_s = architecture_v.asString();
            if (architecture_s == "x32")
                build_config.architecture = Architecture::x32;
            else if (architecture_s == "x64")
                build_config.architecture = Architecture::x64;
        }

        // Enable simd
        const Json::Value simd_v = code_generation_v.get("simd", Json::nullValue);
        if (simd_v != Json::nullValue && simd_v.isString())
        {
            std::string simd_s = simd_v.asString();
            if (simd_s == "IA32")
                build_config.max_simd_level = SimdSets::IA32;
            else if (simd_s == "SSE")
                build_config.max_simd_level = SimdSets::SSE;
            else if (simd_s == "SSE2")
                build_config.max_simd_level = SimdSets::SSE2;
            else if (simd_s == "AVX")
                build_config.max_simd_level = SimdSets::AVX;
            else if (simd_s == "AVX2")
                build_config.max_simd_level = SimdSets::AVX2;
            else if (simd_s == "AVX512")
                build_config.max_simd_level = SimdSets::AVX512;
        }

        // Exception mode
        const Json::Value exception_v = code_generation_v.get("exception", Json::nullValue);
        if (exception_v != Json::nullValue && exception_v.isString())
        {
            std::string exception_s = exception_v.asString();
            if (exception_s == "none")
                build_config.exception_mode = ExceptionMode::none;
            else if (exception_s == "std")
                build_config.exception_mode = ExceptionMode::std;
            else if (exception_s == "std_strict")
                build_config.exception_mode = ExceptionMode::std_strict;
        }

        // Floating point mode
        const Json::Value fp_model_v = code_generation_v.get("fp_model", Json::nullValue);
        if (fp_model_v != Json::nullValue && fp_model_v.isString())
        {
            std::string fp_model_s = fp_model_v.asString();
            if (fp_model_s == "precise")
                build_config.floatingpoint_mode = FPMode::percise;
            else if (fp_model_s == "strict")
                build_config.floatingpoint_mode = FPMode::strict;
            else if (fp_model_s == "fast")
                build_config.floatingpoint_mode = FPMode::fast;
        }

        // String pooling
        const Json::Value string_pooling_v = code_generation_v.get("string_pooling", true);
        build_config.string_pooling = string_pooling_v.isBool() ? string_pooling_v.asBool() : true;

        // Optimization
        const Json::Value optimization_v = code_generation_v.get("optimization", Json::nullValue);
        if (optimization_v != Json::nullValue && optimization_v.isString())
        {
            std::string optimization_s = optimization_v.asString();
            if (optimization_s == "speed")
                build_config.optimization_mode = OptimizationMode::speed;
            else if (optimization_s == "size")
                build_config.optimization_mode = OptimizationMode::size;
        }

        // RTTI
        const Json::Value rtti_v = code_generation_v.get("rtti", true);
        build_config.enable_rtti = rtti_v.isBool() ? rtti_v.asBool() : true;
    }

    // Language features
    const Json::Value language_v = config.get("language", Json::nullValue);
    if (language_v != Json::nullValue && language_v.isObject())
    {
        // Cpp version
        const Json::Value cpp_version_v = language_v.get("cpp_version", Json::nullValue);
        if (cpp_version_v != Json::nullValue && cpp_version_v.isString())
        {
            std::string cpp_version_s = cpp_version_v.asString();
            if (cpp_version_s == "c11")
                build_config.cpp_version = CppVersion::c11;
            else if (cpp_version_s == "c17")
                build_config.cpp_version = CppVersion::c17;
            else if (cpp_version_s == "c++14")
                build_config.cpp_version = CppVersion::cpp14;
            else if (cpp_version_s == "c++17")
                build_config.cpp_version = CppVersion::cpp17;
            else if (cpp_version_s == "c++20")
                build_config.cpp_version = CppVersion::cpp20;
            else if (cpp_version_s == "latest")
                build_config.cpp_version = CppVersion::latest;
        }

        // permissive
        const Json::Value permissive_v = language_v.get("permissive", false);
        build_config.strict_cpp = !(permissive_v.isBool() ? permissive_v.asBool() : false);

        // RTTI
        const Json::Value coroutine_v = language_v.get("coroutine", true);
        build_config.cpp_enable_coroutines = coroutine_v.isBool() ? coroutine_v.asBool() : true;

        // RTTI
        const Json::Value modules_v = language_v.get("modules", true);
        build_config.cpp_enable_modules = modules_v.isBool() ? modules_v.asBool() : true;
    }

    // PCH Header
    const Json::Value pch_v = config.get("pch", Json::nullValue);
    build_config.use_pch_header = (pch_v != Json::nullValue && pch_v.isString());
    if (build_config.use_pch_header)
    {
        build_config.pch_header = pch_v.asString();
    }

    // Additional compiler args
    const Json::Value compiler_args_v = config.get("compiler_args", Json::nullValue);
    if (compiler_args_v != Json::nullValue && compiler_args_v.isArray())
    {
        for (auto it = compiler_args_v.begin(); it != compiler_args_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.compiler_args.push_back(it->asString());
            }
        }
    }

    // Additional linker args
    const Json::Value linker_args_v = config.get("linker_args", Json::nullValue);
    if (linker_args_v != Json::nullValue && linker_args_v.isArray())
    {
        for (auto it = linker_args_v.begin(); it != linker_args_v.end(); ++it)
        {
            if (it->isString())
            {
                build_config.linker_args.push_back(it->asString());
            }
        }
    }

    return build_config;
}
