#include "xpp_run.h"

#include <argparse/argparse.hpp>

#include <iostream>

int main(int argc, char** argv)
{
    int return_code = -1;

    // Create argument parser
    argparse::ArgumentParser args("xppbuild-run", "1.0");
    args.add_description("");

    // Setup arguments
    args.add_argument<std::string>("-b", "--buildfile")
        .help("Specifies the build description that shall be run")
        .default_value<std::string>("build.json")
        .required()
        ;
    args.add_argument("-s", "--silent")
        .help("Disables application output")
        .default_value(false)
        .implicit_value(true)
        ;

    // Parse command line and run app
    try 
    {
        args.parse_args(argc, argv);
        return_code = xpp::run::app_main(args);
    }
    catch (std::exception& ex)
    {
        // Print errors occurred during parsing
        std::cout << ex.what();
    }

    return return_code;
}

