#pragma once

#include <json/json.h>

#include <iostream>

namespace xpp::run
{
    // Build environment
    struct BuildEnv
    {
        std::ostream& out;
        std::ostream& err;
        Json::Value& result;
    };
}
