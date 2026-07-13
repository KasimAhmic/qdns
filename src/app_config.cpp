#include "app_config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/json/parse.hpp>

AppConfig::AppConfig(const std::filesystem::path &configFile, std::shared_ptr<spdlog::logger> logger)
    : logger(std::move(logger)) {
  std::ifstream stream{configFile};

  if (!stream) {
    throw std::runtime_error("Failed to open configuration file: " + configFile.string());
  }

  std::stringstream buffer;
  buffer << stream.rdbuf();

  const boost::json::value document = boost::json::parse(buffer.str());
  const boost::json::object &root = document.as_object();

  this->port = static_cast<uint16_t>(this->readInteger(root, "port", DEFAULT_PORT, 1, 65535));
  this->logLevel = this->readLogLevel(root, "logLevel", spdlog::level::info);
}

AppConfig::~AppConfig() = default;

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

spdlog::level::level_enum AppConfig::readLogLevel(const boost::json::object &root,
                                                  const std::string_view &propertyName,
                                                  const spdlog::level::level_enum defaultValue) const {
  const std::string logLevelStr = this->readString(root, propertyName, std::string(DEFAULT_LOG_LEVEL_STR));

  const spdlog::level::level_enum logLevel = spdlog::level::from_str(logLevelStr);

  if (logLevel == spdlog::level::off && logLevelStr != "off") {
    this->logger->warn("Property '{}' has an invalid log level '{}', using default value", propertyName, logLevelStr);
    return defaultValue;
  }

  return logLevel;
}
