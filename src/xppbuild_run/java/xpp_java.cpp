#include "xpp_java.h"

int xpp::run::java::build_java_config(const Json::Value& build_config, BuildEnv& env)
{
    int return_code = -1;

    // Measure time
    util::Timer timer;

    // Echoing
    env.out << "Performing Java build..." << std::endl;
    env.result["build_type"] = "java";

    // Read all json config values
    Json::Value jdk_path_v = build_config.get("jdk_path", Json::nullValue);
    Json::Value java_version_v = build_config.get("java_version", Json::nullValue);
    Json::Value jar_output_v = build_config.get("jar_output", Json::nullValue);
    Json::Value class_output_v = build_config.get("class_output", Json::nullValue);
    Json::Value java_src_root_v = build_config.get("java_src_root", Json::nullValue);
    Json::Value java_src_v = build_config.get("java_src", Json::nullValue);
    Json::Value java_res_v = build_config.get("java_res", Json::nullValue);
    Json::Value class_path_v = build_config.get("class_path", Json::nullValue);

    // Validate json
    if (
        (jdk_path_v != Json::nullValue) && jdk_path_v.isString() &&
        (java_version_v != Json::nullValue) && java_version_v.isUInt() &&
        (jar_output_v != Json::nullValue) && jar_output_v.isString() &&
        (class_output_v != Json::nullValue) && class_output_v.isString() &&
        (java_src_root_v != Json::nullValue) && java_src_root_v.isString() &&
        (java_src_v != Json::nullValue) && java_src_v.isArray() &&
        (java_res_v != Json::nullValue) && java_res_v.isArray() &&
        (class_path_v != Json::nullValue) && class_path_v.isArray() &&
        true
        )
    {
        // Build path to java exes
        std::filesystem::path javac_exe = util::make_exe_path(std::filesystem::path(jdk_path_v.asCString()) / "bin" / "javac");
        std::filesystem::path jar_exe = util::make_exe_path(std::filesystem::path(jdk_path_v.asCString()) / "bin" / "jar");

        // Prepare javac compilation static options
        std::vector<std::string> javac_args;
        javac_args.push_back("--release");
        javac_args.push_back(std::to_string(java_version_v.asUInt()));
        javac_args.push_back("-d");
        javac_args.push_back(class_output_v.asString());

        // Add javac compile classpath
        std::stringstream javac_class_path;
        bool first_entry_placed = false;
        for (
            auto it = class_path_v.begin();
            it != class_path_v.end();
            it++
            )
        {
            // Check if type is good and process it
            if ((*it).isString())
            {
                javac_class_path << (first_entry_placed ? ":" : "") << (*it).asString();
                first_entry_placed = true;
            }
            else
            {
                env.err << "Classpath at position " << it - class_path_v.begin() << " is not of type string!\n";
            }
        }
        javac_args.push_back("--class-path");
        javac_args.push_back(javac_class_path.str());

        // Add javac compile source files
        for (
            auto it = java_src_v.begin();
            it != java_src_v.end();
            it++
            )
        {
            // convert to json and check
            Json::Value java_source_v = *it;
            if (java_source_v.isString())
            {
                std::filesystem::path source_file = std::filesystem::path(java_src_root_v.asString()) / java_source_v.asString();
                javac_args.push_back(source_file.generic_string());
            }
            else
            {
                env.err << "java source at position " << it - java_source_v.begin() << " is not of type string! Ignoring source entry!\n";
            }
        }

        // Clean obj directory
        if (std::filesystem::exists(class_output_v.asString()))
        {
            std::filesystem::remove_all(class_output_v.asString());
        }

        // Invoke java build
        boost::process::ipstream bp_javac_out;
        boost::process::ipstream bp_javac_err;
        boost::process::child java_compile_process(javac_exe.generic_string(), boost::process::args(javac_args), boost::process::std_out > bp_javac_out, boost::process::std_err > bp_javac_err);
        java_compile_process.wait();
        
        // Read back
        util::copy_stream(env.out, bp_javac_out);
        util::copy_stream(env.err, bp_javac_err);

        // Check return code
        if (java_compile_process.exit_code() == 0)
        {
            // Copy resource files
            for (
                auto it = java_res_v.begin();
                it != java_res_v.end();
                it++
                )
            {
                // Make source and destination path
                std::filesystem::path res_source = std::filesystem::path(java_src_root_v.asString()) / (*it).asString();
                std::filesystem::path res_dest = std::filesystem::path(class_output_v.asString()) / (*it).asString();

                // Check file and copy
                if (std::filesystem::exists(res_source))
                {
                    std::filesystem::copy(res_source, res_dest);
                }
                else
                {
                    env.err << "Resource file " << res_source << " does not exists! Skipping the resource!\n";
                }
            }

            // Prepare jar linking
            std::vector<std::string> jar_args;
            jar_args.push_back("--create");
            jar_args.push_back("--file");
            jar_args.push_back(jar_output_v.asString());
            jar_args.push_back("-C");
            jar_args.push_back(class_output_v.asString());
            jar_args.push_back(".");


            // Invoke jar packing
            boost::process::ipstream bp_jar_out;
            boost::process::ipstream bp_jar_err;
            boost::process::child jar_pack_process(jar_exe.generic_string(), boost::process::args(jar_args), boost::process::std_out > bp_jar_out, boost::process::std_err > bp_jar_err);
            jar_pack_process.wait();

            // Read back
            util::copy_stream(env.out, bp_jar_out);
            util::copy_stream(env.err, bp_jar_err);

            // Check result
            if (jar_pack_process.exit_code() == 0)
            {
                // Store builded jar
                env.result["build_artifacts"].append(jar_output_v);

                // OK
                return_code = 0;
            }
            else
            {
                env.err << "jar failed with code " << jar_pack_process.exit_code() << "\n";
            }
        }
        else
        {
            env.err << "javac failed with code " << java_compile_process.exit_code() << "\n";
        }
    }
    else
    {
        env.err << "Error in java build configuration: Requires all of the following parameters "
                << "[string: jdk_path, uint: java_version, string: jar_output, string: class_output, string: java_src_root, array: java_src, array: java_res, array: class_path]\n";
    }

    // Build time
    timer.stop();
    env.result["build_time"] = timer.duration_float_seconds();

    // Write note
    env.out << "Java build " << (return_code == 0 ? "succeeded" : "failed") << " with code " << return_code << "\n";

    return return_code;
}
