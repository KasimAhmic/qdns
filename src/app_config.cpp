#include "app_config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include <boost/json/parse.hpp>

#include "logging.hpp"

namespace q::config {
  AppConfig::AppConfig(const boost::json::object &config)
      : logger(q::logging::getLogger(logging::Component::Config)),

        version(this->readVersion(config, key::VERSION)),
        logLevel(this->readLogLevel(config, key::LOG_LEVEL, defaults::LOG_LEVEL)),
        port(static_cast<uint16_t>(this->readValueBetween<int64_t>(config, key::PORT, defaults::PORT, 1, 65535))) {}

  AppConfig::~AppConfig() = default;

  AppConfig AppConfig::loadFromFile(const std::filesystem::path &configFile) {
    const std::ifstream stream{configFile};

    if (!stream) {
      throw ConfigException::unableToOpenFile(configFile.string());
    }

    std::stringstream buffer{};
    buffer << stream.rdbuf();

    try {
      const boost::json::value document = boost::json::parse(buffer.str());
      const boost::json::object &root   = document.as_object();

      return AppConfig(root);
    } catch (const boost::system::system_error &e) {
      throw ConfigException::invalidFileFormat(e.code().message());
    }
  }

  // Specific methods for reading properties with custom validation logic

  Version AppConfig::readVersion(const boost::json::object &root, const std::string_view &propertyName) const {
    if (const auto *version = root.if_contains(propertyName)) {
      constexpr auto minVersion = std::to_underlying(Version::V1);
      constexpr auto maxVersion = std::to_underlying(Version::Count) - 1;

      if (version->is_number() && !version->is_double()) {
        const auto result = boost::json::value_to<int64_t>(*version);

        if (result >= minVersion && result <= maxVersion) {
          return static_cast<Version>(result);
        }
      }

      throw ConfigException::invalidFileVersion("Version must be an integer between " + std::to_string(minVersion) +
                                                " and " + std::to_string(maxVersion));
    }

    throw ConfigException::missingRequiredProperty(std::string(propertyName));
  }

  spdlog::level::level_enum AppConfig::readLogLevel(const boost::json::object &root,
                                                    const std::string_view &propertyName,
                                                    const spdlog::level::level_enum defaultValue) const {
    const std::string defaultLogLevelStr = spdlog::level::to_string_view(defaultValue).data();

    const std::string logLevelStr = this->readValue<std::string>(root, propertyName, defaultLogLevelStr);

    const spdlog::level::level_enum logLevel = spdlog::level::from_str(logLevelStr);

    if (logLevel == spdlog::level::off && logLevelStr != "off") {
      this->logger->warn("Property '{}' has an invalid log level '{}', using default value '{}'",
                         propertyName,
                         logLevelStr,
                         defaultLogLevelStr);
      return defaultValue;
    }

    return logLevel;
  }

  ConfigException::ConfigException(const ErrorCode errorCode, const std::string &message)
      : std::runtime_error(message), errorCode(errorCode) {}

  ConfigException::~ConfigException() = default;

  ConfigException ConfigException::unableToOpenFile(const std::string &filePath) {
    return ConfigException(ErrorCode::UnableToOpenConfigFile, "Unable to open file: " + filePath);
  }

  ConfigException ConfigException::invalidFileFormat(const std::string &details) {
    return ConfigException(ErrorCode::InvalidConfigFileFormat, "Invalid file format: " + details);
  }

  ConfigException ConfigException::invalidFileVersion(const std::string &details) {
    return ConfigException(ErrorCode::InvalidConfigFileVersion, "Invalid file version: " + details);
  }

  ConfigException ConfigException::missingRequiredProperty(const std::string &propertyName) {
    return ConfigException(ErrorCode::MissingRequiredConfigProperty, "Missing required property: " + propertyName);
  }
} // namespace q::config
