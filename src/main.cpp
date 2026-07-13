#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <spdlog/spdlog.h>

#include "app_config.hpp"
#include "build_info.hpp"
#include "logging.hpp"

namespace po = boost::program_options;

int main(const int argc, char *argv[]) {
  logging::initialize();

  po::options_description options{"qDNS options"};

  std::string configFile{};

  options.add_options()("help,h", "Print this help message");
  options.add_options()("version,v", "Print version number");
  options.add_options()("about,a", "Print information about the executable");
  options.add_options()("config,c", po::value<std::string>(&configFile), "Configuration file");

  try {
    po::variables_map variables;
    po::store(po::parse_command_line(argc, argv, options), variables);
    po::notify(variables);

    if (variables.contains("help")) {
      std::cout << options << std::endl;
      return 0;
    }

    if (variables.contains("version")) {
      std::cout << "qDNS " << build_info::version << std::endl;

      return 0;
    }

    if (variables.contains("about")) {
      std::cout << "qDNS " << build_info::version << std::endl << std::endl;
      std::cout << "Build information:" << std::endl;
      std::cout << "    Build type: " << build_info::build_type << std::endl;
      std::cout << "    Build timestamp: " << build_info::build_timestamp << std::endl;
      std::cout << "    Compiler: " << build_info::compiler_id << " " << build_info::compiler_version << std::endl;
      std::cout << "    Host system: " << build_info::host_system << ", architecture: " << build_info::host_architecture
                << std::endl;
      std::cout << "    Target system: " << build_info::target_system
                << ", architecture: " << build_info::target_architecture << std::endl;

      return 0;
    }

    if (!variables.contains("config")) {
      std::cout << options << std::endl;
      return 1;
    }

  } catch (const po::error &e) {
    std::cout << "Error parsing command line arguments: " << e.what() << std::endl;
    return 1;
  }

  const auto logger = logging::getLogger("qdns");

  const AppConfig config{configFile, logging::getLogger("config")};

  spdlog::set_level(config.getLogLevel());

  logger->info("Starting qDNS...");

  logger->info("qDNS stopped");

  return 0;
}
