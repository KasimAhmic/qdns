#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace qdns::error {
  enum class ErrorCode : size_t {
    UnableToOpenConfigFile = 0,
    InvalidConfigFileFormat = 1,
    InvalidConfigFileVersion = 2,
    MissingRequiredConfigProperty = 3,
  };

  class ConfigError : public std::runtime_error {
  public:
    explicit ConfigError(ErrorCode errorCode, const std::string &message);
    ~ConfigError();

    static ConfigError unableToOpenFile(const std::string &filePath);
    static ConfigError invalidFileFormat(const std::string &details);
    static ConfigError invalidFileVersion(const std::string &details);
    static ConfigError missingRequiredProperty(const std::string &propertyName);

    [[nodiscard]] ErrorCode getErrorCode() const { return this->errorCode; }

  private:
    ErrorCode errorCode;
  };
}; // namespace qdns::error
