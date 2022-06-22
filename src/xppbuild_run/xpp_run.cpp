#include "xpp_run.h"

int xpp::run::app_main(const boost::program_options::variables_map& args)
{
    int return_code = -1;
    std::filesystem::path json_file_path;

    // Evaluate silent
    std::ostream* out_stream = &std::cout;
    util::NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);
    if (args.count("silent"))
    {
        out_stream = &null_stream;
    }

    // Build environment
    Json::Value build_result;
    std::stringstream memory_err_stream;
    std::stringstream memory_out_stream;
    BuildEnv env{ 
        .out = memory_out_stream, 
        .err = memory_err_stream,
        .result = build_result 
    };

    // Open json file
    json_file_path = args["buildfile"].as<std::string>();
    build_result["build_file"] = json_file_path.generic_string();

    if (std::filesystem::exists(json_file_path))
    {
        // Read json
        Json::Value build_config;
        if (util::json_read_from_file(json_file_path, build_config, *out_stream))
        {
             // Read type
            Json::Value build_type_v = build_config.get("type", Json::nullValue);
            if (build_type_v != Json::nullValue)
            {
                static std::string build_type = build_type_v.asString();
                
                // JAVA
                if (build_type == "java")
                {
                    // Find java node
                    const Json::Value java_build_config = build_config.get("java", Json::nullValue);
                    if (java_build_config != Json::nullValue)
                    {
                        return_code = java::build_java_config(java_build_config, env);
                    }
                    else
                    {
                        env.err << "Can't find java root node in " << json_file_path << "!\n";
                    }
                }
                // C++ MSVC (TODO)
                else if (build_type == "cpp\\msvc")
                {
                    // TODO: Implement msvc
                    env.err << "MSVC\n";
                }
                else
                {
                    env.err << "The build type \"" << build_type << "\" specified in " << json_file_path << " is unknown!\n";
                }
            }
            else 
            {
                env.err << "The build configuration " << json_file_path << " does not speicify a build type!\n";
            }
        }
    }
    else
    {
        env.err << "Can't find file " << json_file_path << "\n";
    }

    // Append output streams to result
    env.result["stdout"] = memory_out_stream.str();
    env.result["stderr"] = memory_err_stream.str();
    env.result["return_code"] = return_code;

    // Compute build result and print to requested out stream
    Json::FastWriter json_writer;
    *out_stream << json_writer.write(env.result);

    return return_code;
}
