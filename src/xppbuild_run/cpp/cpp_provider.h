#pragma once

#include "cpp_build_config.h"
#include "../build_env.h"

#include <json/json.h>

namespace xpp::run::cpp
{
    // Interface that will be implemented by compiler extensions
    class ICppProvider
    {
        public:
            virtual int build_cpp_config(const CppBuildConfig& cpp_config, const Json::Value& impl_build_config, BuildEnv& env) = 0;
    };
}
