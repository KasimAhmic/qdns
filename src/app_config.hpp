#pragma once

#include <filesystem>
#include <string_view>

#include <boost/json/value_to.hpp>
#include <spdlog/spdlog.h>

class AppConfig {
  public:
    explicit AppConfig(const std::filesystem::path &configFile);
    ~AppConfig();

    [[nodiscard]] uint16_t getPort() const { return this->port; }
    [[nodiscard]] spdlog::level::level_enum getLogLevel() const { return this->logLevel; }

  private:
    uint16_t port;
    spdlog::level::level_enum logLevel;

    static constexpr uint16_t DEFAULT_PORT = 55555;
    static constexpr spdlog::level::level_enum DEFAULT_LOG_LEVEL = spdlog::level::info;

    static std::string readString(const boost::json::object &root,
                                  const std::string_view &propertyName,
                                  const std::string &defaultValue) {
        if (const auto *value = root.if_contains(propertyName)) {
            if (value->is_string()) {
                return boost::json::value_to<std::string>(*value);
            }

            return defaultValue;
        }

        return defaultValue;
    }

    static bool readBoolean(const boost::json::object &root,
                            const std::string_view &propertyName,
                            const bool defaultValue) {
        if (const auto *value = root.if_contains(propertyName)) {
            if (value->is_bool()) {
                return boost::json::value_to<bool>(*value);
            }

            return defaultValue;
        }

        return defaultValue;
    }

    static double readDouble(const boost::json::object &root,
                             const std::string_view &propertyName,
                             const double defaultValue) {
        if (const auto *value = root.if_contains(propertyName)) {
            if (value->is_double()) {
                return boost::json::value_to<double>(*value);
            }

            return defaultValue;
        }

        return defaultValue;
    }

    static int64_t readInt(const boost::json::object &root,
                           const std::string_view &propertyName,
                           const int64_t defaultValue) {
        if (const auto *value = root.if_contains(propertyName)) {
            if (value->is_int64()) {
                return boost::json::value_to<int64_t>(*value);
            }

            return defaultValue;
        }

        return defaultValue;
    }

    static uint64_t readUInt(const boost::json::object &root,
                             const std::string_view &propertyName,
                             const uint64_t defaultValue) {
        if (const auto *value = root.if_contains(propertyName)) {
            if (value->is_uint64()) {
                return boost::json::value_to<uint64_t>(*value);
            }

            return defaultValue;
        }

        return defaultValue;
    }

    static uint64_t readUInt(const boost::json::object &root,
                             const std::string_view &propertyName,
                             const uint64_t defaultValue,
                             const uint64_t minValue,
                             const uint64_t maxValue) {
        const uint64_t value = readUInt(root, propertyName, defaultValue);

        if (value < minValue || value > maxValue) {
            return defaultValue;
        }

        return value;
    }
};
