#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <spdlog/spdlog.h>

#include "app_config.hpp"
#include "build_info.hpp"
#include "error.hpp"
#include "logging.hpp"

namespace po = boost::program_options;

int main(const int argc, char *argv[]) {
  qdns::logging::initialize();

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
      std::cout << "qDNS " << qdns::build_info::version << std::endl;

      return 0;
    }

    if (variables.contains("about")) {
      namespace bi = qdns::build_info;

      std::cout << "qDNS " << bi::version << std::endl << std::endl;
      std::cout << "Build information:" << std::endl;
      std::cout << "    Build type: " << bi::build_type << std::endl;
      std::cout << "    Build timestamp: " << bi::build_timestamp << std::endl;
      std::cout << "    Compiler: " << bi::compiler_id << " " << bi::compiler_version << std::endl;
      std::cout << "    Host system: " << bi::host_system << ", arch: " << bi::host_architecture << std::endl;
      std::cout << "    Target system: " << bi::target_system << ", arch: " << bi::target_architecture << std::endl;

      return 0;
    }

    if (!variables.contains("config")) {
      std::cout << "Error: Configuration file not specified" << std::endl << std::endl;
      std::cout << options << std::endl;
      return 1;
    }

    const auto logger = qdns::logging::getLogger(qdns::logging::Component::App);

    const qdns::config::AppConfig config = qdns::config::AppConfig::loadFromFile(configFile);

    spdlog::set_level(config.getLogLevel());

    logger->info("Starting qDNS...");

    logger->info("qDNS stopped");

  } catch (const po::error &e) {
    std::cout << "Error parsing command line arguments:" << std::endl;
    std::cout << "  > " << e.what() << std::endl;
    return 1;
  } catch (const qdns::error::ConfigError &e) {
    std::cout << "Configuration error:" << std::endl;
    std::cout << "  > " << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cout << "An error occurred:" << std::endl;
    std::cout << "  > " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cout << "Unknown error occurred" << std::endl;
    return 1;
  }

  return 0;
}
