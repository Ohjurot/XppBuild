#include "xpp_run.h"

#include <boost/program_options.hpp>

#include <iostream>

int main(int argc, char** argv)
{
    int return_code = -1;

    // Declare the supported options.
    boost::program_options::options_description allowed_options("Allowed options");
    allowed_options.add_options()
        ("help", "produce this help message")
        ("buildfile", boost::program_options::value<std::string>()->default_value("build.json")->required(), "Specifies the build description that shall be run")
        ("silent", "Disables application output")
        ;

    // Handle errors and call main
    try
    {
        // Parse cmds
        boost::program_options::variables_map args;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, allowed_options), args);
        if (args.count("help"))
        {
            std::cout << allowed_options;
        }
        else
        {
            // rais and execute
            boost::program_options::notify(args);
            return_code = xpp::run::app_main(args);
        }
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what();
    }
    return return_code;
}

