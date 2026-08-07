#pragma once

#include <cstddef>
#include <cstdint>
#include <variant>

namespace q::dns::error {
  enum class ParseSection : uint8_t {
    Header,
    Question,
    Answer,
    Authority,
    Additional,
  };

  struct TruncatedDataError {
    ParseSection section;
    size_t offset;
    size_t requiredBytes;
    size_t availableBytes;
  };

  struct UnsupportedOpcodeError {
    uint8_t opcode;
  };

  struct ReservedHeaderBitSetError {};

  struct InvalidLabelLengthError {
    size_t offset;
    uint8_t length;
  };

  struct InvalidCompressionPointerError {
    size_t offset;
    uint16_t target;
  };

  struct CompressionLoopError {
    size_t offset;
  };

  using ParseError = std::variant<TruncatedDataError,
                                  UnsupportedOpcodeError,
                                  ReservedHeaderBitSetError,
                                  InvalidLabelLengthError,
                                  InvalidCompressionPointerError,
                                  CompressionLoopError>;
} // namespace q::dns::error
