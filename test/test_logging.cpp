#include "logging.hpp"
#include "testing.hpp"

TEST(LoggingTest, InitializesLoggerOnce) {
  q::logging::initialize();

  const auto logger1 = q::logging::getLogger(q::logging::Component::App);
  const auto logger2 = q::logging::getLogger(q::logging::Component::App);

  EXPECT_EQ(logger1, logger2);
}

TEST(LoggingTest, InitializesNewLoggerForDifferentComponent) {
  q::logging::initialize();

  const auto logger1 = q::logging::getLogger(q::logging::Component::App);
  const auto logger2 = q::logging::getLogger(q::logging::Component::Config);

  EXPECT_NE(logger1, logger2);
}
