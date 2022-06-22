#pragma once

#include "util/stream.h"
#include "util/json.h"
#include "java/xpp_java.h"
#include "build_env.h"

#include <argparse/argparse.hpp>
#include <json/json.h>

#include <filesystem>
#include <string>

namespace xpp::run
{
    int app_main(const argparse::ArgumentParser& args);
}
