#include "error.hpp"

namespace qdns::error {
  ConfigError::ConfigError(const ErrorCode errorCode, const std::string &message)
      : std::runtime_error(message), errorCode(errorCode) {}

  ConfigError::~ConfigError() = default;

  ConfigError ConfigError::unableToOpenFile(const std::string &filePath) {
    return ConfigError(ErrorCode::UnableToOpenConfigFile, "Unable to open file: " + filePath);
  }

  ConfigError ConfigError::invalidFileFormat(const std::string &details) {
    return ConfigError(ErrorCode::InvalidConfigFileFormat, "Invalid file format: " + details);
  }

  ConfigError ConfigError::invalidFileVersion(const std::string &details) {
    return ConfigError(ErrorCode::InvalidConfigFileVersion, "Invalid file version: " + details);
  }

  ConfigError ConfigError::missingRequiredProperty(const std::string &propertyName) {
    return ConfigError(ErrorCode::MissingRequiredConfigProperty, "Missing required property: " + propertyName);
  }
} // namespace qdns::error
