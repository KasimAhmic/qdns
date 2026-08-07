#include <array>
#include <functional>
#include <memory>
#include <ostream>
#include <string_view>
#include <utility>

#include "app_config.hpp"
#include "testing.hpp"

class AppConfigTest : public FileTest {
protected:
  static void expectError(const std::filesystem::path &path, const q::config::ErrorCode expectedError) {

    try {
      q::config::AppConfig::loadFromFile(path);
      FAIL() << "Expected ConfigException";
    } catch (const q::config::ConfigException &exception) {
      EXPECT_EQ(exception.getErrorCode(), expectedError);
    } catch (...) {
      FAIL() << "Expected ConfigException";
    }
  }
};

TEST_F(AppConfigTest, LoadsValidConfig) {
  const auto path = writeFile("config.json", R"({
        "version": 1,
        "logLevel": "debug",
        "port": 12345
    })");

  const auto config = q::config::AppConfig::loadFromFile(path);

  EXPECT_EQ(config.getVersion(), q::config::Version::V1);
  EXPECT_EQ(config.getLogLevel(), spdlog::level::debug);
  EXPECT_EQ(config.getPort(), 12345);
}

TEST_F(AppConfigTest, ThrowsOnMissingConfigFile) {
  const auto path = temporaryDirectory.getPath() / "nonexistent_config.json";

  expectError(path, q::config::ErrorCode::UnableToOpenConfigFile);
}

TEST_F(AppConfigTest, ThrowsOnMissingVersion) {
  const auto path = writeFile("config.json", R"({
        "logLevel": "debug",
        "port": 12345
    })");

  expectError(path, q::config::ErrorCode::MissingRequiredConfigProperty);
}

struct ConfigCase {
  std::string_view name;
  std::string_view contents;
};

class AppConfigMalformedJsonTest : public AppConfigTest, public testing::WithParamInterface<ConfigCase> {};

TEST_P(AppConfigMalformedJsonTest, ThrowsOnMalformedJson) {
  const auto &testCase = GetParam();
  const auto path      = writeFile("config.json", testCase.contents);

  expectError(path, q::config::ErrorCode::InvalidConfigFileFormat);
}

INSTANTIATE_TEST_SUITE_P(MalformedJson,
                         AppConfigMalformedJsonTest,
                         testing::Values(ConfigCase{"EmptyFile", R"()"},
                                         ConfigCase{"InvalidJson", R"({)"},
                                         ConfigCase{"ArrayInsteadOfObject", R"([])"},
                                         ConfigCase{"UnquotedKeys", R"({version: 1, logLevel: "debug", port: 12345})"},
                                         ConfigCase{"MissingQuote",
                                                    R"({"version: 1, "logLevel": "debug", "port": 12345})"},
                                         ConfigCase{"InvalidNumberFormat",
                                                    R"({"version": 1L, "logLevel": "debug", "port": 12345})"},
                                         ConfigCase{"MissingClosingBrace",
                                                    R"({"version": 1, "logLevel": "debug", "port": 12345)"},
                                         ConfigCase{"TrailingComma",
                                                    R"({"version": 1, "logLevel": "debug", "port": 12345,})"}),
                         [](const testing::TestParamInfo<ConfigCase> &info) { return std::string{info.param.name}; });

class AppConfigInvalidVersionTest : public AppConfigTest, public testing::WithParamInterface<ConfigCase> {};

TEST_P(AppConfigInvalidVersionTest, ThrowsOnInvalidVersion) {
  const auto &testCase = GetParam();
  const auto path      = writeFile("config.json", testCase.contents);

  expectError(path, q::config::ErrorCode::InvalidConfigFileVersion);
}

INSTANTIATE_TEST_SUITE_P(InvalidVersion,
                         AppConfigInvalidVersionTest,
                         testing::Values(ConfigCase{"UnsupportedVersion",
                                                    R"({"version": 9999999, "logLevel": "debug", "port": 12345})"},
                                         ConfigCase{"VersionAsString",
                                                    R"({"version": "1", "logLevel": "debug", "port": 12345})"},
                                         ConfigCase{"VersionAsFloat",
                                                    R"({"version": 1.5, "logLevel": "debug", "port": 12345})"},
                                         ConfigCase{"VersionAsNull",
                                                    R"({"version": null, "logLevel": "debug", "port": 12345})"},
                                         ConfigCase{"VersionAsBoolean",
                                                    R"({"version": true, "logLevel": "debug", "port": 12345})"}),
                         [](const testing::TestParamInfo<ConfigCase> &info) { return std::string{info.param.name}; });

class AppConfigDefaultPropertiesTest : public FileTest, public testing::WithParamInterface<ConfigCase> {};

TEST_P(AppConfigDefaultPropertiesTest, UsesDefaultProperties) {
  const auto &testCase = GetParam();
  const auto path      = writeFile("config.json", testCase.contents);

  const auto config = q::config::AppConfig::loadFromFile(path);

  EXPECT_EQ(config.getLogLevel(), q::config::defaults::LOG_LEVEL);
  EXPECT_EQ(config.getPort(), q::config::defaults::PORT);
}

INSTANTIATE_TEST_SUITE_P(DefaultProperties,
                         AppConfigDefaultPropertiesTest,
                         testing::Values(ConfigCase{"MissingLogLevelAndPort", R"({"version": 1})"},
                                         ConfigCase{"LogLevelAsNumber", R"({"version": 1, "logLevel": 1})"},
                                         ConfigCase{"PortAsString", R"({"version": 1, "port": "12345"})"}),
                         [](const testing::TestParamInfo<ConfigCase> &info) { return std::string{info.param.name}; });
