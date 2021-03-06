#pragma once

#include "util/exe.h"
#include "util/stream.h"
#include "util/timer.h"
#include "../build_env.h"

#include <json/json.h>
#include <boost/process.hpp>

#include <filesystem>
#include <vector>

namespace xpp::run::java
{
    // Java build main
    int build_java_config(const Json::Value& build_config, BuildEnv& env);
}
