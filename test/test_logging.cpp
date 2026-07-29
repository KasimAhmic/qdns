#include "logging.hpp"
#include "testing.hpp"

BOOST_AUTO_TEST_SUITE(logging_tests)

BOOST_AUTO_TEST_CASE(initializes_logger_once) {
  qdns::logging::initialize();

  const auto logger1 = qdns::logging::getLogger(qdns::logging::Component::App);
  const auto logger2 = qdns::logging::getLogger(qdns::logging::Component::App);

  BOOST_TEST(logger1 == logger2);
}

BOOST_AUTO_TEST_SUITE_END()
