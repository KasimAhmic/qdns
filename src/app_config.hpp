#pragma once

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include <boost/json/value_to.hpp>
#include <spdlog/spdlog.h>

#include "number_util.hpp"

namespace q::config {

  namespace key {
    constexpr std::string_view VERSION   = "version";
    constexpr std::string_view LOG_LEVEL = "logLevel";
    constexpr std::string_view PORT      = "port";
  }; // namespace key

  namespace defaults {
    constexpr spdlog::level::level_enum LOG_LEVEL = spdlog::level::info;
    constexpr uint16_t PORT                       = 55555;
  } // namespace defaults

  enum class Version : int64_t {
    V1 = 1,
    Count,
  };

  class AppConfig {
  public:
    ~AppConfig();

    static AppConfig loadFromFile(const std::filesystem::path &configFile);

    [[nodiscard]] q::config::Version getVersion() const { return this->version; }
    [[nodiscard]] spdlog::level::level_enum getLogLevel() const { return this->logLevel; }
    [[nodiscard]] uint16_t getPort() const { return this->port; }

  private:
    std::shared_ptr<spdlog::logger> logger;
    q::config::Version version;
    spdlog::level::level_enum logLevel;
    uint16_t port;

    explicit AppConfig(const boost::json::object &config);

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

    template <typename T>
      requires util::JsonType<T>
    [[nodiscard]] T readValue(const boost::json::object &root,
                              const std::string_view &propertyName,
                              const T defaultValue) const {

      const auto *value = root.if_contains(propertyName);

      if (!value) {
        return this->defaultFromMissingProperty(propertyName, defaultValue);
      }

      if constexpr (std::same_as<std::remove_cvref_t<T>, std::string>) {
        if (!value->is_string()) {
          return this->defaultFromTypeError(propertyName, "string", defaultValue);
        }
      } else if constexpr (std::same_as<std::remove_cvref_t<T>, bool>) {
        if (!value->is_bool()) {
          return this->defaultFromTypeError(propertyName, "bool", defaultValue);
        }
      } else if constexpr (std::same_as<std::remove_cvref_t<T>, double>) {
        if (!value->is_double()) {
          return this->defaultFromTypeError(propertyName, "double", defaultValue);
        }
      } else if constexpr (std::same_as<std::remove_cvref_t<T>, int64_t>) {
        if (!value->is_int64()) {
          return this->defaultFromTypeError(propertyName, "int64", defaultValue);
        }
      } else {
        static_assert(std::is_same_v<T, void>, "Unsupported type for readValue");
      }

      return boost::json::value_to<T>(*value);
    }

    template <typename T>
      requires util::JsonNumber<T>
    [[nodiscard]] T readValueBetween(const boost::json::object &root,
                                     const std::string_view &propertyName,
                                     const T defaultValue,
                                     const T min = util::JsonNumberLimits<T>::min(),
                                     const T max = util::JsonNumberLimits<T>::max()) {
      const T value = this->readValue<T>(root, propertyName, defaultValue);

      if (value < min || value > max) {
        return this->defaultFromOutOfRangeError(propertyName,
                                                std::to_string(min) + " to " + std::to_string(max),
                                                defaultValue);
      }

      return value;
    };

    [[nodiscard]] q::config::Version readVersion(const boost::json::object &root,
                                                 const std::string_view &propertyName) const;

    [[nodiscard]] spdlog::level::level_enum readLogLevel(const boost::json::object &root,
                                                         const std::string_view &propertyName,
                                                         const spdlog::level::level_enum defaultValue) const;
  };

  enum class ErrorCode : size_t {
    UnableToOpenConfigFile        = 0,
    InvalidConfigFileFormat       = 1,
    InvalidConfigFileVersion      = 2,
    MissingRequiredConfigProperty = 3,
  };

  class ConfigException : public std::runtime_error {
  public:
    explicit ConfigException(ErrorCode errorCode, const std::string &message);
    ~ConfigException();

    static ConfigException unableToOpenFile(const std::string &filePath);
    static ConfigException invalidFileFormat(const std::string &details);
    static ConfigException invalidFileVersion(const std::string &details);
    static ConfigException missingRequiredProperty(const std::string &propertyName);

    [[nodiscard]] ErrorCode getErrorCode() const { return this->errorCode; }

  private:
    ErrorCode errorCode;
  };
} // namespace q::config
