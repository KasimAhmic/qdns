#include "logging.hpp"

#include <string>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace logging {
  void initialize() {
    if (auto existing = spdlog::get("app")) {
      spdlog::set_default_logger(existing);
      return;
    }

    auto logger = spdlog::stdout_color_mt("app");

    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] "
                        "[%^%l%$] "
                        "[%n] "
                        "%v");

    logger->set_level(spdlog::level::info);

    spdlog::set_default_logger(logger);
  }

  std::shared_ptr<spdlog::logger> getLogger(const std::string_view component) {
    const std::string name{component};

    if (auto existing = spdlog::get(name)) {
      return existing;
    }

    auto logger = spdlog::default_logger()->clone(name);
    spdlog::register_logger(logger);

    return logger;
  }
} // namespace logging
