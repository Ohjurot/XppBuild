#include "xpp_run.h"

int xpp::run::app_main(const boost::program_options::variables_map& args)
{
    int return_code = -1;
    std::filesystem::path json_file_path;

    // Measure time
    util::Timer timer;

    // Evaluate silent
    std::ostream* ptr_out_stream = &std::cout;
    util::NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);
    if (args.count("silent"))
    {
        ptr_out_stream = &null_stream;
    }
    std::ostream& out_stream = *ptr_out_stream;

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
    env.result["build_file"] = json_file_path.generic_string();

    if (std::filesystem::exists(json_file_path))
    {
        // Read json
        Json::Value build_config;
        if (util::json_read_from_file(json_file_path, build_config, env.err))
        {
            // Read type
            Json::Value build_type_v = build_config.get("type", Json::nullValue);
            if (build_type_v != Json::nullValue && build_type_v.isString())
            {
                static std::string build_type = build_type_v.asString();
                
                // Examples:
                // JAVA: "java", ""
                // CPP: "cpp", "msvc" or "cpp", "clang"
                std::string build_type_major = build_type;
                std::string build_type_minor;

                // Check if minor exists
                size_t split_pos = build_type_major.find_first_of('\\');
                if (split_pos != std::string::npos)
                {
                    build_type_minor = build_type_major.substr(split_pos + 1);
                    build_type_major = build_type_major.substr(0, split_pos);
                }
                
                // JAVA
                if (build_type_major == "java")
                {
                    // Find java node
                    const Json::Value java_build_config = build_config.get("java", Json::nullValue);
                    if (java_build_config != Json::nullValue && java_build_config.isObject())
                    {
                        return_code = java::build_java_config(java_build_config, env);
                    }
                    else
                    {
                        env.err << "Can't find java root object node in " << json_file_path << "!\n";
                    }
                }
                // C++
                else if (build_type_major == "cpp" && build_type_minor != "")
                {
                    // Search for implementation
                    cpp::CppProviderRegistry::init();
                    cpp::ICppProvider* impl = cpp::CppProviderRegistry::get_provider(build_type_minor);
                    if (impl)
                    {
                        // Parse common cpp configuration
                        const Json::Value cpp_build_config_v = build_config.get("cpp", Json::nullValue);
                        const Json::Value cpp_impl_build_config_v = build_config.get(build_type_minor, Json::nullValue);
                        if (
                            cpp_build_config_v != Json::nullValue && cpp_build_config_v.isObject() &&
                            cpp_impl_build_config_v != Json::nullValue && cpp_impl_build_config_v.isObject()
                            )
                        {
                            // Parse cpp configuration
                            const cpp::CppBuildConfig cpp_build_config = cpp::json_to_cpp_build_info(cpp_build_config_v);

                            // Invoke cpp action
                            return_code = impl->build_cpp_config(cpp_build_config, cpp_impl_build_config_v, env);
                        }
                        else
                        {
                            env.err << "Can't find cpp or " << build_type_minor << " root object node in " << json_file_path << "!\n";
                        }
                    }
                    else
                    {
                        env.err << "No implementation for " << build_type << "!\n";
                    }
                }
                else
                {
                    env.err << "The build type \"" << build_type << "\" specified in " << json_file_path << " is unknown!\n";
                }
            }
            else 
            {
                env.err << "The build configuration " << json_file_path << " does not specify a build type!\n";
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

    // Build time
    timer.stop();
    env.result["build_time"] = timer.duration_float_seconds();

    // Compute build result and print to requested out stream
    Json::FastWriter json_writer;
    out_stream << json_writer.write(env.result);

    return return_code;
}
