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

  enum class ResponseCode : uint8_t {
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
   * |QR|   Opcode  |AA|TC|RD|RA| Z|AD|CD|   RCODE   |
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
    explicit Header(uint16_t id,
                    uint16_t flags,
                    uint16_t qd_count,
                    uint16_t an_count,
                    uint16_t ns_count,
                    uint16_t ar_count);

    static std::expected<Header, error::ParseError> parse(const std::span<const std::byte> data);

    [[nodiscard]] uint16_t get_id() const noexcept { return this->id; }
    [[nodiscard]] uint16_t get_flags() const noexcept { return this->flags; }
    [[nodiscard]] uint16_t get_question_count() const noexcept { return this->qd_count; }
    [[nodiscard]] uint16_t get_answer_count() const noexcept { return this->an_count; }
    [[nodiscard]] uint16_t get_authority_count() const noexcept { return this->ns_count; }
    [[nodiscard]] uint16_t get_additional_records() const noexcept { return this->ar_count; }

    [[nodiscard]] bool is_query() const noexcept;
    [[nodiscard]] bool is_response() const noexcept;
    [[nodiscard]] OpCode get_op_code() const noexcept;
    [[nodiscard]] bool is_authoritative_answer() const noexcept;
    [[nodiscard]] bool is_truncated() const noexcept;
    [[nodiscard]] bool is_recursion_requested() const noexcept;
    [[nodiscard]] bool is_recursion_available() const noexcept;
    [[nodiscard]] bool is_reserved_set() const noexcept;
    [[nodiscard]] bool is_authentic_data() const noexcept;
    [[nodiscard]] bool is_checking_disabled() const noexcept;
    [[nodiscard]] ResponseCode get_response_code() const noexcept;

    [[nodiscard]] std::string to_string() const;

  private:
    uint16_t id;
    uint16_t flags;
    uint16_t qd_count;
    uint16_t an_count;
    uint16_t ns_count;
    uint16_t ar_count;
  };
} // namespace q::dns
