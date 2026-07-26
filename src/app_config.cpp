#include "app_config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/json/parse.hpp>

#include "error.hpp"
#include "logging.hpp"

namespace qdns::config {
  AppConfig::AppConfig(const boost::json::object &config)
      : logger(logging::getLogger(logging::Component::Config)),

        version(this->readVersion(config, key::VERSION)),
        logLevel(this->readLogLevel(config, key::LOG_LEVEL, AppConfig::DEFAULT_LOG_LEVEL)),
        port(static_cast<uint16_t>(this->readInteger(config, key::PORT, AppConfig::DEFAULT_PORT, 1, 65535))) {}

  AppConfig::~AppConfig() = default;

  AppConfig AppConfig::loadFromFile(const std::filesystem::path &configFile) {
    const std::ifstream stream{configFile};

    if (!stream) {
      throw qdns::error::ConfigError::unableToOpenFile(configFile.string());
    }

    std::stringstream buffer{};
    buffer << stream.rdbuf();

    try {
      const boost::json::value document = boost::json::parse(buffer.str());
      const boost::json::object &root = document.as_object();

      return AppConfig(root);
    } catch (const boost::system::system_error &e) {
      throw qdns::error::ConfigError::invalidFileFormat(e.code().message());
    }
  }

  // Generic methods for reading properties from the JSON object with default values and error handling

  std::string AppConfig::readString(const boost::json::object &root,
                                    const std::string_view &propertyName,
                                    const std::string &defaultValue) const {
    if (const auto *value = root.if_contains(propertyName)) {
      if (value->is_string()) {
        return boost::json::value_to<std::string>(*value);
      }

      return this->defaultFromTypeError(propertyName, "string", defaultValue);
    }

    return this->defaultFromMissingProperty(propertyName, defaultValue);
  }

  bool AppConfig::readBoolean(const boost::json::object &root,
                              const std::string_view &propertyName,
                              const bool defaultValue) const {
    if (const auto *value = root.if_contains(propertyName)) {
      if (value->is_bool()) {
        return boost::json::value_to<bool>(*value);
      }

      return this->defaultFromTypeError(propertyName, "boolean", defaultValue);
    }

    return this->defaultFromMissingProperty(propertyName, defaultValue);
  }

  double AppConfig::readDouble(const boost::json::object &root,
                               const std::string_view &propertyName,
                               const double defaultValue,
                               const double minValue,
                               const double maxValue) const {
    if (const auto *value = root.if_contains(propertyName)) {
      if (value->is_double()) {
        const auto result = boost::json::value_to<double>(*value);

        if (result >= minValue && result <= maxValue) {
          return result;
        }

        return this->defaultFromOutOfRangeError(propertyName,
                                                std::to_string(minValue) + " to " + std::to_string(maxValue),
                                                defaultValue);
      }

      return this->defaultFromTypeError(propertyName, "double", defaultValue);
    }

    return this->defaultFromMissingProperty(propertyName, defaultValue);
  }

  int64_t AppConfig::readInteger(const boost::json::object &root,
                                 const std::string_view &propertyName,
                                 const int64_t defaultValue,
                                 const int64_t minValue,
                                 const int64_t maxValue) const {
    if (const auto *value = root.if_contains(propertyName)) {
      if (value->is_number() && !value->is_double()) {
        const auto result = boost::json::value_to<int64_t>(*value);

        if (result >= minValue && result <= maxValue) {
          return result;
        }

        return this->defaultFromOutOfRangeError(propertyName,
                                                std::to_string(minValue) + " to " + std::to_string(maxValue),
                                                defaultValue);
      }

      return this->defaultFromTypeError(propertyName, "integer", defaultValue);
    }

    return this->defaultFromMissingProperty(propertyName, defaultValue);
  }

  // Specific methods for reading properties with custom validation logic

  Version AppConfig::readVersion(const boost::json::object &root, const std::string_view &propertyName) const {
    if (const auto *version = root.if_contains(propertyName)) {
      const std::string errorMessage = "Version must be an integer between " +
                                       std::to_string(AS_UNDERLYING(Version::V1)) + " and " +
                                       std::to_string(AS_UNDERLYING(Version::Count) - 1);

      if (version->is_number() && !version->is_double()) {
        const auto result = boost::json::value_to<int64_t>(*version);

        if (result >= AS_UNDERLYING(Version::V1) && result < AS_UNDERLYING(Version::Count)) {
          return static_cast<Version>(result);
        }

        throw qdns::error::ConfigError::invalidFileVersion(errorMessage);
      }

      throw qdns::error::ConfigError::invalidFileVersion(errorMessage);
    }

    throw qdns::error::ConfigError::missingRequiredProperty(std::string(propertyName));
  }

  spdlog::level::level_enum AppConfig::readLogLevel(const boost::json::object &root,
                                                    const std::string_view &propertyName,
                                                    const spdlog::level::level_enum defaultValue) const {
    const std::string logLevelStr = this->readString(root, propertyName, std::string(DEFAULT_LOG_LEVEL_STR));

    const spdlog::level::level_enum logLevel = spdlog::level::from_str(logLevelStr);

    if (logLevel == spdlog::level::off && logLevelStr != "off") {
      this->logger->warn("Property '{}' has an invalid log level '{}', using default value '{}'",
                         propertyName,
                         logLevelStr,
                         DEFAULT_LOG_LEVEL_STR);
      return defaultValue;
    }

    return logLevel;
  }

} // namespace qdns::config
