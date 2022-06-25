#pragma once

#include <json/json.h>

#include <vector>
#include <filesystem>
#include <string>

namespace xpp::run::cpp
{
    // C++ language version
    enum class CppVersion : uint8_t
    {
        c11,
        c17,
        cpp14,
        cpp17,
        cpp20,

        latest = std::numeric_limits<uint8_t>::max()
    };

    // C++ architectures
    enum class Architecture : uint8_t
    {
        x32,
        x64,
    };

    // Floating point mode
    enum class FPMode : uint8_t
    {
        percise,
        strict,
        fast
    };

    // Mode of optimization
    enum class OptimizationMode : uint8_t
    {
        none,
        speed,
        size
    };

    // SIMD instruction sets
    enum class SimdSets : uint8_t
    {
        none,
        IA32,
        SSE,
        SSE2,
        AVX,
        AVX2,
        AVX512
    };

    // How exceptions are done
    enum class ExceptionMode : uint8_t
    {
        none,
        std,
        std_strict
    };

    // General cpp build configuration
    struct CppBuildConfig
    {
        // Files
        std::vector<std::filesystem::path> source_files;
        std::vector<std::filesystem::path> include_dirs;

        // Output
        std::filesystem::path intermediate_dir;
        
        // Defined C++ preproc macros
        std::vector<std::string> preprocessor_macros;

        // Debugging
        bool is_debug_build = false;

        // PCH
        bool use_pch_header = false;
        std::string pch_header;

        // CPP version and strictness
        CppVersion cpp_version;
        bool strict_cpp = false;

        // CPP Features
        bool cpp_enable_coroutines = false;
        bool cpp_enable_modules = false;

        // Code generation
        Architecture architecture = Architecture::x64;
        SimdSets max_simd_level = SimdSets::none;
        ExceptionMode exception_mode = ExceptionMode::std;
        FPMode floatingpoint_mode = FPMode::strict;
        bool string_pooling = true;
        OptimizationMode optimization_mode = OptimizationMode::none;
        bool enable_rtti = true;

    };

    // Parses a json configuration to the correct build configuration
    CppBuildConfig json_to_cpp_build_info(const Json::Value& config);
}
