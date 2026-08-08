#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace q::util {
  // TODO: Handle out-of-bounds read
  [[nodiscard]] constexpr uint16_t read_16(const std::span<const std::byte> data, const size_t offset) noexcept {
    const auto high = std::to_integer<uint16_t>(data[offset]);
    const auto low  = std::to_integer<uint16_t>(data[offset + 1]);

    return static_cast<uint16_t>((high << 8U) | low);
  }
} // namespace q::util
