#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include <spdlog/logger.h>

namespace qdns::logging {
  enum class Component : uint8_t {
    App = 0,
    Config = 1,
  };

  namespace detail {
    constexpr std::string_view getLoggerName(const Component component) {
      switch (component) {
      case Component::App:
        return "qdns";
      case Component::Config:
        return "config";
      }
    }
  } // namespace detail

  void initialize();

  [[nodiscard]] std::shared_ptr<spdlog::logger> getLogger(const Component component);
} // namespace qdns::logging
