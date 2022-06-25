#pragma once

#include "../cpp_provider.h"

namespace xpp::run::cpp
{
    // Clang implementation
    class ClangProvider : public ICppProvider
    {
        public:
            int build_cpp_config(const CppBuildConfig& cpp_config, const Json::Value& impl_build_config, BuildEnv& env) override;

    };
}
