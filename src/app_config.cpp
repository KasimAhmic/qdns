#include "app_config.hpp"

#include <fstream>

#include <boost/json/parse.hpp>

AppConfig::AppConfig() = default;

AppConfig::AppConfig(const std::filesystem::path &configFile) {
    std::ifstream stream{configFile};

    if (!stream) {
        throw std::runtime_error("Failed to open configuration file: " + configFile.string());
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();

    const boost::json::value document = boost::json::parse(buffer.str());
    const boost::json::object &root = document.as_object();

    this->port = readProperty<uint16_t>(root, "port", 55555);
    this->logLevel = readProperty<uint8_t>(root, "logLevel", 1);
}

AppConfig::~AppConfig() = default;
