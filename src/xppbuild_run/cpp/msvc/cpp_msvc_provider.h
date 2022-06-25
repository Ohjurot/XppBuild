#pragma once

#include "../cpp_provider.h"

#include "util/exe.h"
#include "util/stream.h"

#include <filesystem>
#include <string>
#include <vector>

namespace xpp::run::cpp
{
    // MSVC implementation
    class MsvcProvider : public ICppProvider
    {
        public:
            int build_cpp_config(const CppBuildConfig& cpp_config, const Json::Value& impl_build_config, BuildEnv& env) override;
    
        private:
            void build_msvc_cl_args(std::vector<std::string>& msvc_cl_args, const CppBuildConfig& cpp_config, const Json::Value& impl_build_config);
    };
}
