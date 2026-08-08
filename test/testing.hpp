#pragma once

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#include <gtest/gtest.h>

class TempDir {
public:
  TempDir() : path{createUniqueDirectory()} {}

  TempDir(const TempDir &)            = delete;
  TempDir &operator=(const TempDir &) = delete;

  ~TempDir() noexcept {
    if (shouldKeepArtifacts()) {
      return;
    }

    std::error_code error;
    std::filesystem::remove_all(this->getPath(), error);
  }

  [[nodiscard]] const std::filesystem::path &getPath() const noexcept { return this->path; }

  [[nodiscard]] std::filesystem::path writeFile(const std::filesystem::path &relativePath,
                                                const std::string_view contents) const {

    if (relativePath.empty()) {
      throw std::invalid_argument{"Relative path must not be empty"};
    }

    if (!relativePath.has_filename()) {
      throw std::invalid_argument{"Relative path must have a filename"};
    }

    if (relativePath.is_absolute()) {
      throw std::invalid_argument{"Relative path must be relative"};
    }

    for (const auto &component : relativePath) {
      if (component == "..") {
        throw std::invalid_argument{"Relative path must not contain '..'"};
      }
    }

    const auto filePath = this->getPath() / relativePath;

    if (const auto parent = filePath.parent_path(); !parent.empty()) {
      std::filesystem::create_directories(parent);
    }

    std::ofstream file{filePath, std::ios::binary};

    if (!file) {
      throw std::runtime_error{"Unable to create test file: " + filePath.string()};
    }

    file.write(contents.data(), static_cast<std::streamsize>(contents.size()));

    if (!file) {
      throw std::runtime_error{"Unable to write test file: " + filePath.string()};
    }

    return filePath;
  }

private:
  static std::filesystem::path createUniqueDirectory() {
    const auto temporaryRoot = std::filesystem::temp_directory_path();

    std::random_device randomDevice;
    std::mt19937_64 generator{randomDevice()};

    for (std::size_t attempt = 0; attempt < 100; ++attempt) {
      std::ostringstream name;
      name << "qdns-test-" << std::hex << std::setw(16) << std::setfill('0') << generator();

      const auto candidate = temporaryRoot / name.str();

      std::error_code error;
      const bool created = std::filesystem::create_directory(candidate, error);

      if (created) {
        return candidate;
      }

      if (error) {
        throw std::filesystem::filesystem_error{"Unable to create temporary test directory", candidate, error};
      }
    }

    throw std::runtime_error{"Unable to generate a unique temporary test directory"};
  }

  static bool shouldKeepArtifacts() noexcept {
    const char *value = std::getenv("QDNS_KEEP_TEST_ARTIFACTS");
    return value != nullptr && std::string_view{value} == "1";
  }

  const std::filesystem::path path;
};

class FileTest : public testing::Test {
public:
  TempDir temporaryDirectory;

protected:
  /**
   * @brief Writes a file with the given contents to the system's temp directory.
   *
   * @param name The relative path of the file to write.
   * @param contents The contents to write to the file.
   * @return The full path to the written file.
   */
  [[nodiscard]] std::filesystem::path writeFile(const std::filesystem::path &name,
                                                const std::string_view contents) const {
    return this->temporaryDirectory.writeFile(name, contents);
  }
};
