#pragma once

#include "../cpp_provider.h"

#include "util/exe.h"
#include "util/stream.h"

#include <filesystem>
#include <string>
#include <vector>
#include <cstdarg>

namespace xpp::run::cpp
{
    // MSVC implementation
    class MsvcProvider : public ICppProvider
    {
        public:
            int build_cpp_config(const CppBuildConfig& cpp_config, const Json::Value& impl_build_config, BuildEnv& env) override;
    
        private:
            static inline void args_add(std::vector<std::string>& msvc_cl_args, const std::string& arg)
            {
                msvc_cl_args.push_back(arg);
            }
            static inline void args_add_include_path(std::vector<std::string>& msvc_cl_args, const std::filesystem::path& path)
            {
                msvc_cl_args.push_back("/I" + path.generic_string());
            }
            static inline void args_add_lib_path(std::vector<std::string>& msvc_ld_args, const std::filesystem::path& path)
            {
                msvc_ld_args.push_back("/LIBPATH:" + path.generic_string());
            }
            static inline void args_add_define(std::vector<std::string>& msvc_cl_args, const std::string& define)
            {
                msvc_cl_args.push_back("/D" + define);
            }

            // va_args add
            static inline void args_add(std::vector<std::string>& msvc_cl_args, unsigned int count, ...)
            {
                va_list vargs;
                va_start(vargs, count);
                for (unsigned int i = 0; i < count; i++)
                {
                    const char* arg = va_arg(vargs, const char*);
                    args_add(msvc_cl_args, arg);
                }
                va_end(vargs);
            }

            // Adds default include and lib paths
            static void add_msvc_include_path(std::vector<std::string>& msvc_cl_args, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path);
            static void add_msvc_lib_path(std::vector<std::string>& msvc_link_args, const CppBuildConfig& cpp_config, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path);

            // Function for default msvc argument setup
            static void add_msvc_cl_args(std::vector<std::string>& msvc_cl_args, const CppBuildConfig& cpp_config, bool static_runtime);
            static void add_msvc_linker_args(std::vector<std::string>& msvc_linker_args, const CppBuildConfig& cpp_config, const std::filesystem::path& msvc_link_exe, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path);

            // Compiles a singel file
            static int compile_singel_file(std::vector<std::string>& msvc_cl_args, const std::filesystem::path& msvc_cl_exe, const std::filesystem::path& source_file, const std::filesystem::path& obj_file, BuildEnv& env);

            // Links obj files into an exe or dll
            static int link_exe_or_dll(const CppBuildConfig& cpp_config, std::vector<std::filesystem::path> obj_file, const std::filesystem::path& msvc_link_exe, const std::filesystem::path& msvc_toolchain_path, const std::filesystem::path& win_sdk_path, BuildEnv& env);

    };
}
