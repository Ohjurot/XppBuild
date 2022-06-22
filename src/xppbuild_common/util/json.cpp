#include "json.h"

bool xpp::util::json_read_from_file(const std::filesystem::path json_file, Json::Value& json_data, std::ostream& os)
{
    bool ok = false;
    Json::Reader json_reader;
    std::stringstream ss_temp;
    std::ifstream file_in;

    // Open file
    file_in.open(json_file);
    if (file_in.is_open())
    {
        // Read file content
        ss_temp << file_in.rdbuf();

        // Parse json
        if (json_reader.parse(ss_temp, json_data, false))
        {
            ok = true;
        }
        else
        {
            os << "Error(s) while parsing" << json_file << ":\n" << json_reader.getFormattedErrorMessages() << "\n";
        }
    }
    else
    {
        os << "Can't open " << json_file << "\n";
    }

    return ok;
}
