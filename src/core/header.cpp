#include "header.hpp"

#include <format>

namespace q::dns {
  Header::Header(const uint16_t id,
                 const uint16_t flags,
                 const uint16_t qd_count,
                 const uint16_t an_count,
                 const uint16_t ns_count,
                 const uint16_t ar_count)
      : id(id), flags(flags), qd_count(qd_count), an_count(an_count), ns_count(ns_count), ar_count(ar_count) {}

  Header::~Header() = default;

  std::expected<Header, error::ParseError> Header::parse(const std::span<const std::byte> data) {
    if (data.size() < 12) {
      return std::unexpected<error::ParseError>{error::TruncatedDataError{
          .section        = error::ParseSection::Header,
          .offset         = 0,
          .requiredBytes  = 12,
          .availableBytes = data.size(),
      }};
    }

    return Header(read16(data, 0),
                  read16(data, 2),
                  read16(data, 4),
                  read16(data, 6),
                  read16(data, 8),
                  read16(data, 10));
  }

  std::string Header::toString() const {
    const std::string
        flags = std::format("qr={}, opCode={}, aa={}, tc={}, rd={}, ra={}, reservedZ={}, ad={}, cd={}, rCode={}",
                            this->qr(),
                            static_cast<int>(this->opCode()),
                            this->aa(),
                            this->tc(),
                            this->rd(),
                            this->ra(),
                            this->reservedZ(),
                            this->ad(),
                            this->cd(),
                            static_cast<int>(this->rCode()));

    return std::format("Header{{id={}, flags=[{}], qd_count={}, an_count={}, ns_count={}, ar_count={}}}",
                       this->id,
                       flags,
                       this->qd_count,
                       this->an_count,
                       this->ns_count,
                       this->ar_count);
  }

  bool Header::qr() const noexcept { return (this->flags & 0x8000U) != 0; }
  OpCode Header::opCode() const noexcept { return static_cast<OpCode>((this->flags >> 11U) & 0x0FU); }
  bool Header::aa() const noexcept { return (this->flags & 0x0400U) != 0; }
  bool Header::tc() const noexcept { return (this->flags & 0x0200U) != 0; }
  bool Header::rd() const noexcept { return (this->flags & 0x0100U) != 0; }
  bool Header::ra() const noexcept { return (this->flags & 0x0080U) != 0; }
  bool Header::reservedZ() const noexcept { return (this->flags & 0x0040U) != 0; }
  bool Header::ad() const noexcept { return (this->flags & 0x0020U) != 0; }
  bool Header::cd() const noexcept { return (this->flags & 0x0010U) != 0; }
  RCode Header::rCode() const noexcept { return static_cast<RCode>(this->flags & 0x000FU); }
} // namespace q::dns
