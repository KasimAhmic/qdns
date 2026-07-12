#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <spdlog/spdlog.h>

#include "app_config.hpp"

namespace po = boost::program_options;

int main(const int argc, char *argv[]) {
    po::options_description options{"qDNS options"};

    std::string configFile{};

    options.add_options()("help,h", "Print this help message");
    options.add_options()("version,v", "Print version number");
    options.add_options()("config,c", po::value<std::string>(&configFile), "Configuration file");

    AppConfig config{};

    try {
        po::variables_map variables;
        po::store(po::parse_command_line(argc, argv, options), variables);
        po::notify(variables);

        if (variables.contains("help")) {
            std::cout << options << std::endl;
            return 0;
        }

        if (variables.contains("version")) {
            std::cout << "qDNS version 1.0.0" << std::endl;
            return 0;
        }

        if (!variables.contains("config")) {
            std::cout << options << std::endl;
            return 1;
        }

        config = AppConfig(configFile);

    } catch (const po::error &e) {
        spdlog::error("Error parsing command line arguments: {}", e.what());
        return 1;
    }

    spdlog::info("Starting qDNS...");

    spdlog::info("qDNS stopped");

    return 0;
}
