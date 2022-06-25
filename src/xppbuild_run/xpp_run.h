#pragma once

#include "util/stream.h"
#include "util/json.h"
#include "java/xpp_java.h"
#include "cpp/cpp_provider.h"
#include "cpp/cpp_registry.h"
#include "cpp/cpp_build_config.h"
#include "build_env.h"

#include <boost/program_options.hpp>
#include <json/json.h>

#include <filesystem>
#include <string>

namespace xpp::run
{
    int app_main(const boost::program_options::variables_map& args);
}
