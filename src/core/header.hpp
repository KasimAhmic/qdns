#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>

#include "error.hpp"

namespace q::dns {
  enum class OpCode : uint8_t {
    Query  = 0,
    IQuery = 1,
    Status = 2,
    Notify = 4,
    Update = 5,
  };

  enum class RCode : uint8_t {
    NoError        = 0,
    FormatError    = 1,
    ServerFailure  = 2,
    NameError      = 3,
    NotImplemented = 4,
    Refused        = 5,
  };

  /**
   * @brief Represents the DNS header section.
   * @code
   * DNS Header Layout
   *
   *  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * |                      ID                       |
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * |QR|   Opcode  |AA|TC|RD|RA|    Z   |   RCODE   |
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * |                    QDCOUNT                    |
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * |                    ANCOUNT                    |
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * |                    NSCOUNT                    |
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * |                    ARCOUNT                    |
   * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   * @endcode
   */
  class Header {
  public:
    explicit Header();

    explicit Header(uint16_t id,
                    uint16_t flags,
                    uint16_t qd_count,
                    uint16_t an_count,
                    uint16_t ns_count,
                    uint16_t ar_count);
    ~Header();

    static std::expected<Header, error::ParseError> parse(const std::span<const std::byte> data);

    [[nodiscard]] uint16_t getId() const noexcept { return this->id; }
    [[nodiscard]] uint16_t getFlags() const noexcept { return this->flags; }
    [[nodiscard]] uint16_t getQdCount() const noexcept { return this->qd_count; }
    [[nodiscard]] uint16_t getAnCount() const noexcept { return this->an_count; }
    [[nodiscard]] uint16_t getNsCount() const noexcept { return this->ns_count; }
    [[nodiscard]] uint16_t getArCount() const noexcept { return this->ar_count; }

    [[nodiscard]] std::string toString() const;

    // TODO: Replace these with getters
    [[nodiscard]] bool qr() const noexcept;
    [[nodiscard]] OpCode opCode() const noexcept;
    [[nodiscard]] bool aa() const noexcept;
    [[nodiscard]] bool tc() const noexcept;
    [[nodiscard]] bool rd() const noexcept;
    [[nodiscard]] bool ra() const noexcept;
    [[nodiscard]] bool reservedZ() const noexcept;
    [[nodiscard]] bool ad() const noexcept;
    [[nodiscard]] bool cd() const noexcept;
    [[nodiscard]] RCode rCode() const noexcept;

  private:
    uint16_t id;
    uint16_t flags;
    uint16_t qd_count;
    uint16_t an_count;
    uint16_t ns_count;
    uint16_t ar_count;

    [[nodiscard]] static constexpr std::uint16_t read16(const std::span<const std::byte> data,
                                                        const std::size_t offset) noexcept {

      const auto high = std::to_integer<std::uint16_t>(data[offset]);
      const auto low  = std::to_integer<std::uint16_t>(data[offset + 1]);

      return static_cast<std::uint16_t>((high << 8U) | low);
    }
  };
} // namespace q::dns
