#include <array>
#include <ostream>
#include <string_view>
#include <utility>

#include "app_config.hpp"
#include "error.hpp"
#include "testing.hpp"

namespace qdns::config {
  std::ostream &operator<<(std::ostream &stream, const Version version) {
    return stream << std::to_underlying(version);
  }
} // namespace qdns::config

BOOST_AUTO_TEST_SUITE(app_config_tests)

BOOST_FIXTURE_TEST_CASE(loads_valid_config, FileFixture) {
  const auto path = writeFile("config.json", R"({
    "version": 1,
    "logLevel": "debug",
    "port": 12345
  })");

  const auto config = qdns::config::AppConfig::loadFromFile(path);

  BOOST_TEST(config.getVersion() == qdns::config::Version::V1);
  BOOST_TEST(config.getLogLevel() == spdlog::level::level_enum::debug);
  BOOST_TEST(config.getPort() == 12345);
}

std::function<bool(const qdns::error::ConfigError &)> isErrorCode(const qdns::error::ErrorCode expected) {
  return [expected](const qdns::error::ConfigError &e) { return e.getErrorCode() == expected; };
}

BOOST_FIXTURE_TEST_CASE(throws_on_missing_config_file, FileFixture) {
  const auto path = this->temporaryDirectory.getPath() / "nonexistent_config.json";

  BOOST_CHECK_EXCEPTION(qdns::config::AppConfig::loadFromFile(path),
                        qdns::error::ConfigError,
                        isErrorCode(qdns::error::ErrorCode::UnableToOpenConfigFile));
}

BOOST_DATA_TEST_CASE_F(FileFixture,
                       throws_on_malformed_json,
                       boost::unit_test::data::make({
                           R"()",   // Empty file
                           R"({)",  // Missing closing brace
                           R"([])", // Invalid JSON type (array instead of object)
                           R"({version: 1, logLevel: "debug", port: 12345})",        // Missing quotes around keys
                           R"({"version: 1, "logLevel": "debug", "port": 12345})",   // Missing closing quote
                           R"({"version": 1L, "logLevel": "debug", "port": 12345})", // Invalid number format
                           R"({"version": 1, "logLevel": "debug", "port": 12345)",   // Missing closing brace
                           R"({"version": 1, "logLevel": "debug", "port": 12345,})", // Trailing comma
                           R"({"version": 1, "logLevel": "debug", "port": 12345,)",  // Missing closing brace and
                                                                                     // trailing comma
                       }),
                       contents) {
  const auto path = writeFile("config.json", contents);

  BOOST_CHECK_EXCEPTION(qdns::config::AppConfig::loadFromFile(path),
                        qdns::error::ConfigError,
                        isErrorCode(qdns::error::ErrorCode::InvalidConfigFileFormat));
}

BOOST_DATA_TEST_CASE_F(FileFixture,
                       throws_on_invalid_version,
                       boost::unit_test::data::make({
                           // Unsupported version
                           R"({"version": 2, "logLevel": "debug", "port": 12345})",
                           // Version as string instead of integer
                           R"({"version": "1", "logLevel": "debug", "port": 12345})",
                           // Version as float instead of integer
                           R"({"version": 1.5, "logLevel": "debug", "port": 12345})",
                           // Version as null instead of integer
                           R"({"version": null, "logLevel": "debug", "port": 12345})",
                           // Version as boolean instead of integer
                           R"({"version": true, "logLevel": "debug", "port": 12345})",
                       }),
                       contents) {
  const auto path = writeFile("config.json", contents);

  BOOST_CHECK_EXCEPTION(qdns::config::AppConfig::loadFromFile(path),
                        qdns::error::ConfigError,
                        isErrorCode(qdns::error::ErrorCode::InvalidConfigFileVersion));
}

BOOST_FIXTURE_TEST_CASE(throws_on_missing_version, FileFixture) {
  const auto path = writeFile("config.json", R"({
    "logLevel": "debug",
    "port": 12345
  })");

  BOOST_CHECK_EXCEPTION(qdns::config::AppConfig::loadFromFile(path),
                        qdns::error::ConfigError,
                        isErrorCode(qdns::error::ErrorCode::MissingRequiredConfigProperty));
}

BOOST_DATA_TEST_CASE_F(FileFixture,
                       loads_default_values_for_missing_properties,
                       boost::unit_test::data::make({
                           R"({"version": 1})",
                           R"({"version": 1, "logLevel": 1})",
                           R"({"version": 1, "port": "12345"})",
                       }),
                       contents) {
  const auto path = writeFile("config.json", contents);

  const auto config = qdns::config::AppConfig::loadFromFile(path);

  BOOST_TEST(config.getLogLevel() == qdns::config::defaults::LOG_LEVEL);
  BOOST_TEST(config.getPort() == qdns::config::defaults::PORT);
}

BOOST_AUTO_TEST_SUITE_END()
