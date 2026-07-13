#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <string_view>

#include <boost/json/value_to.hpp>
#include <spdlog/spdlog.h>

class AppConfig {
public:
  explicit AppConfig(const std::filesystem::path &configFile, std::shared_ptr<spdlog::logger> logger);
  ~AppConfig();

  [[nodiscard]] uint16_t getPort() const { return this->port; }
  [[nodiscard]] spdlog::level::level_enum getLogLevel() const { return this->logLevel; }

private:
  std::shared_ptr<spdlog::logger> logger;

  uint16_t port;
  spdlog::level::level_enum logLevel;

  static constexpr uint16_t DEFAULT_PORT = 55555;
  static constexpr spdlog::level::level_enum DEFAULT_LOG_LEVEL = spdlog::level::info;
  static constexpr std::string_view DEFAULT_LOG_LEVEL_STR = "info";

  // Helper functions to read configuration properties with default values and error handling

  template <typename T>
  [[nodiscard]] T defaultFromMissingProperty(const std::string_view &propertyName, const T defaultValue) const {
    this->logger->warn("Property '{}' is missing, using default value", propertyName);
    return defaultValue;
  }

  template <typename T>
  [[nodiscard]] T defaultFromTypeError(const std::string_view &propertyName,
                                       const std::string_view &expectedType,
                                       const T defaultValue) const {
    this->logger->warn("Property '{}' is not of type '{}', using default value", propertyName, expectedType);
    return defaultValue;
  }

  template <typename T>
  [[nodiscard]] T defaultFromOutOfRangeError(const std::string_view &propertyName,
                                             const std::string_view &expectedRange,
                                             const T defaultValue) const {
    this->logger->warn("Property '{}' is out of range '{}', using default value", propertyName, expectedRange);
    return defaultValue;
  }

  [[nodiscard]] std::string readString(const boost::json::object &root,
                                       const std::string_view &propertyName,
                                       const std::string &defaultValue) const;

  [[nodiscard]] bool readBoolean(const boost::json::object &root,
                                 const std::string_view &propertyName,
                                 const bool defaultValue) const;

  [[nodiscard]] double readDouble(const boost::json::object &root,
                                  const std::string_view &propertyName,
                                  const double defaultValue,
                                  const double minValue = std::numeric_limits<double>::lowest(),
                                  const double maxValue = std::numeric_limits<double>::max()) const;

  [[nodiscard]] int64_t readInteger(const boost::json::object &root,
                                    const std::string_view &propertyName,
                                    const int64_t defaultValue,
                                    const int64_t minValue = std::numeric_limits<int64_t>::min(),
                                    const int64_t maxValue = std::numeric_limits<int64_t>::max()) const;

  // Config option specific read functions

  [[nodiscard]] spdlog::level::level_enum readLogLevel(const boost::json::object &root,
                                                       const std::string_view &propertyName,
                                                       const spdlog::level::level_enum defaultValue) const;
};
