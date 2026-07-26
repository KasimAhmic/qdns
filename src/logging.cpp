#include "logging.hpp"

#include <string>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace qdns::logging {
  void initialize() {
    if (auto existing = spdlog::get(std::string(detail::getLoggerName(Component::App)))) {
      spdlog::set_default_logger(existing);
      return;
    }

    auto logger = spdlog::stdout_color_mt(std::string(detail::getLoggerName(Component::App)));

    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] "
                        "[%^%l%$] "
                        "[%n] "
                        "%v");

    logger->set_level(spdlog::level::info);

    spdlog::set_default_logger(logger);
  }

  std::shared_ptr<spdlog::logger> getLogger(const Component component) {
    const std::string name{detail::getLoggerName(component)};

    if (auto existing = spdlog::get(name)) {
      return existing;
    }

    auto logger = spdlog::default_logger()->clone(name);
    spdlog::register_logger(logger);

    return logger;
  }
} // namespace qdns::logging
