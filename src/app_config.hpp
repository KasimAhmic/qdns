#pragma once

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <string_view>

#include <boost/json/value_to.hpp>
#include <spdlog/spdlog.h>

namespace qdns::config {
  namespace key {
    constexpr std::string_view VERSION = "version";
    constexpr std::string_view LOG_LEVEL = "logLevel";
    constexpr std::string_view PORT = "port";
  }; // namespace key

  namespace defaults {
    constexpr spdlog::level::level_enum LOG_LEVEL = spdlog::level::info;
    constexpr uint16_t PORT = 55555;
  } // namespace defaults

  enum class Version : int64_t {
    V1 = 1,
    Count,
  };

  class AppConfig {
  public:
    ~AppConfig();

    static AppConfig loadFromFile(const std::filesystem::path &configFile);

    [[nodiscard]] qdns::config::Version getVersion() const { return this->version; }
    [[nodiscard]] spdlog::level::level_enum getLogLevel() const { return this->logLevel; }
    [[nodiscard]] uint16_t getPort() const { return this->port; }

  private:
    std::shared_ptr<spdlog::logger> logger;
    qdns::config::Version version;
    spdlog::level::level_enum logLevel;
    uint16_t port;

    explicit AppConfig(const boost::json::object &config);

    // Helper functions to read configuration properties with default values and error handling

    template <typename T>
    [[nodiscard]] T defaultFromMissingProperty(const std::string_view &propertyName, const T defaultValue) const {
      this->logger->warn("Property '{}' is missing, using default value: '{}'", propertyName, defaultValue);
      return defaultValue;
    }

    template <typename T>
    [[nodiscard]] T defaultFromTypeError(const std::string_view &propertyName,
                                         const std::string_view &expectedType,
                                         const T defaultValue) const {
      this->logger->warn("Property '{}' is not of type '{}', using default value: '{}'",
                         propertyName,
                         expectedType,
                         defaultValue);
      return defaultValue;
    }

    template <typename T>
    [[nodiscard]] T defaultFromOutOfRangeError(const std::string_view &propertyName,
                                               const std::string_view &expectedRange,
                                               const T defaultValue) const {
      this->logger->warn("Property '{}' is out of range '{}', using default value: '{}'",
                         propertyName,
                         expectedRange,
                         defaultValue);
      return defaultValue;
    }

    template <typename T>
    [[nodiscard]] T defaultFromValueError(const std::string_view &propertyName,
                                          const std::string_view &expectedValues,
                                          const T defaultValue) const {
      this->logger->warn("Property '{}' has an invalid value, expected one of '{}', using default value: '{}'",
                         propertyName,
                         expectedValues,
                         defaultValue);
      return defaultValue;
    }

    template <typename T>
    [[nodiscard]] T defaultFromValueError(const std::string_view &propertyName,
                                          const std::string_view &expectedValues,
                                          const T defaultValue,
                                          const std::string_view &defaultValueStr) const {
      this->logger->warn("Property '{}' has an invalid value, expected one of '{}', using default value: '{}'",
                         propertyName,
                         expectedValues,
                         defaultValueStr);
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

    [[nodiscard]] qdns::config::Version readVersion(const boost::json::object &root,
                                                    const std::string_view &propertyName) const;

    [[nodiscard]] spdlog::level::level_enum readLogLevel(const boost::json::object &root,
                                                         const std::string_view &propertyName,
                                                         const spdlog::level::level_enum defaultValue) const;
  };
} // namespace qdns::config
