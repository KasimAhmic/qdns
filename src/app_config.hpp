#pragma once

#include <filesystem>
#include <string_view>

#include <boost/json/value_to.hpp>

class AppConfig {
  public:
    AppConfig();
    explicit AppConfig(const std::filesystem::path &configFile);

    ~AppConfig();

  private:
    uint16_t port;
    uint8_t logLevel;

    template <typename T>
    static T readProperty(const boost::json::object &root, const std::string_view &propertyName, T defaultValue) {
        if (const auto *value = root.if_contains(propertyName)) {
            return boost::json::value_to<T>(*value);
        }

        return defaultValue;
    }
};
