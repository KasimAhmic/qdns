#include "app_config.hpp"

#include <fstream>

#include <boost/json/parse.hpp>

AppConfig::AppConfig(const std::filesystem::path &configFile) {
    std::ifstream stream{configFile};

    if (!stream) {
        throw std::runtime_error("Failed to open configuration file: " + configFile.string());
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();

    const boost::json::value document = boost::json::parse(buffer.str());
    const boost::json::object &root = document.as_object();

    this->port = static_cast<uint16_t>(readUInt(root, "port", DEFAULT_PORT, 1, 65535));
    this->logLevel = static_cast<spdlog::level::level_enum>(readUInt(root,
                                                                     "logLevel",
                                                                     DEFAULT_LOG_LEVEL,
                                                                     spdlog::level::trace,
                                                                     spdlog::level::off));
}

AppConfig::~AppConfig() = default;
