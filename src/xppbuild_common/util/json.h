#pragma once

#include <json/json.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace xpp::util
{
    bool json_read_from_file(const std::filesystem::path json_file, Json::Value& json_data, std::ostream& os);
}
