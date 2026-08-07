#pragma once

#include <concepts>
#include <cstdint>
#include <limits>
#include <string>

namespace q::util {
  /**
   * @brief A concept that checks if a type is a valid JSON type (string, number, boolean).
   *
   * @tparam T The type to check.
   * @return true if T is a valid JSON type, false otherwise.
   */
  template <typename T>
  concept JsonType = std::same_as<T, std::string> || std::integral<T> || std::floating_point<T> ||
                     std::same_as<T, bool>;

  /**
   * @brief A concept that checks if a type is a valid JSON number type (int64_t, double, float).
   *
   * @tparam T The type to check.
   * @return true if T is a valid JSON number type, false otherwise.
   */
  template <typename T>
  concept JsonNumber = (std::integral<T> && !std::same_as<T, bool>) || std::floating_point<T>;

  /**
   * @brief A struct that provides the minimum and maximum values for a given JSON number type.
   *
   * @tparam T The JSON number type (int64_t, double, float) for which to provide the limits.
   */
  template <typename T> struct JsonNumberLimits {};

  /**
   * @brief A specialization of JsonNumberLimits for integral types (excluding bool) that provides the minimum and
   * maximum values for the type.
   *
   * @tparam T The integral type (excluding bool) for which to provide the limits.
   */
  template <typename T>
    requires std::integral<T> && (!std::same_as<T, bool>)
  struct JsonNumberLimits<T> {
    static constexpr T min() { return std::numeric_limits<T>::min(); }
    static constexpr T max() { return std::numeric_limits<T>::max(); }
  };

  /**
   * @brief A specialization of JsonNumberLimits for floating-point types that provides the minimum and maximum values
   * for the type.
   *
   * @tparam T The floating-point type for which to provide the limits.
   */
  template <typename T>
    requires std::floating_point<T>
  struct JsonNumberLimits<T> {
    static constexpr T min() { return std::numeric_limits<T>::lowest(); }
    static constexpr T max() { return std::numeric_limits<T>::max(); }
  };

} // namespace q::util
