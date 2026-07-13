#pragma once

#include <memory>
#include <string_view>

#include <spdlog/logger.h>

namespace logging {
  void initialize();

  [[nodiscard]] std::shared_ptr<spdlog::logger> getLogger(std::string_view component);
} // namespace logging
