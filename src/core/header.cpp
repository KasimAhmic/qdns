#include "header.hpp"

#include <format>

#include "byte_util.hpp"

namespace q::dns {
  namespace constants {
    constexpr size_t HEADER_SIZE = 12;
  } // namespace constants

  namespace mask {
    constexpr uint16_t QR     = 0x8000U;
    constexpr uint16_t OPCODE = 0x7800U;
    constexpr uint16_t AA     = 0x0400U;
    constexpr uint16_t TC     = 0x0200U;
    constexpr uint16_t RD     = 0x0100U;
    constexpr uint16_t RA     = 0x0080U;
    constexpr uint16_t Z      = 0x0040U;
    constexpr uint16_t AD     = 0x0020U;
    constexpr uint16_t CD     = 0x0010U;
    constexpr uint16_t RCODE  = 0x000FU;
  } // namespace mask

  Header::Header(const uint16_t id,
                 const uint16_t flags,
                 const uint16_t qd_count,
                 const uint16_t an_count,
                 const uint16_t ns_count,
                 const uint16_t ar_count)
      : id(id), flags(flags), qd_count(qd_count), an_count(an_count), ns_count(ns_count), ar_count(ar_count) {}

  std::expected<Header, error::ParseError> Header::parse(const std::span<const std::byte> data) {
    if (data.size() < constants::HEADER_SIZE) {
      return std::unexpected<error::ParseError>{error::TruncatedDataError{
          .section        = error::ParseSection::Header,
          .offset         = 0,
          .requiredBytes  = constants::HEADER_SIZE,
          .availableBytes = data.size(),
      }};
    }

    return Header(q::util::read_16(data, 0),
                  q::util::read_16(data, 2),
                  q::util::read_16(data, 4),
                  q::util::read_16(data, 6),
                  q::util::read_16(data, 8),
                  q::util::read_16(data, 10));
  }

  bool Header::is_query() const noexcept { return (this->flags & mask::QR) == 0; }

  bool Header::is_response() const noexcept { return !this->is_query(); }

  OpCode Header::get_op_code() const noexcept { return static_cast<OpCode>((this->flags & mask::OPCODE) >> 11U); }

  bool Header::is_authoritative_answer() const noexcept { return (this->flags & mask::AA) != 0; }

  bool Header::is_truncated() const noexcept { return (this->flags & mask::TC) != 0; }

  bool Header::is_recursion_requested() const noexcept { return (this->flags & mask::RD) != 0; }

  bool Header::is_recursion_available() const noexcept { return (this->flags & mask::RA) != 0; }

  bool Header::is_reserved_set() const noexcept { return (this->flags & mask::Z) != 0; }

  bool Header::is_authentic_data() const noexcept { return (this->flags & mask::AD) != 0; }

  bool Header::is_checking_disabled() const noexcept { return (this->flags & mask::CD) != 0; }

  ResponseCode Header::get_response_code() const noexcept {
    return static_cast<ResponseCode>(this->flags & mask::RCODE);
  }

  std::string Header::to_string() const {
    const std::string
        flags_str = std::format("qr={}, opcode={}, aa={}, tc={}, rd={}, ra={}, z={}, ad={}, cd={}, rcode={}",
                                this->is_query(),
                                static_cast<int>(this->get_op_code()),
                                this->is_authoritative_answer(),
                                this->is_truncated(),
                                this->is_recursion_requested(),
                                this->is_recursion_available(),
                                this->is_reserved_set(),
                                this->is_authentic_data(),
                                this->is_checking_disabled(),
                                static_cast<int>(this->get_response_code()));

    return std::format("Header[id={}, flags=[{}], qd_count={}, an_count={}, ns_count={}, ar_count={}]",
                       this->id,
                       flags_str,
                       this->qd_count,
                       this->an_count,
                       this->ns_count,
                       this->ar_count);
  }
} // namespace q::dns
