#include <memory>

#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
  const auto logger = std::make_shared<spdlog::logger>("test", std::make_shared<spdlog::sinks::null_sink_mt>());

  spdlog::set_default_logger(logger);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
