#include <array>

#include "../testing.hpp"
#include "core/byte_util.hpp"

TEST(ByteUtilTest, Read16) {
  const std::array<std::byte, 4> data{
      std::byte{0x12},
      std::byte{0x34},
      std::byte{0x56},
      std::byte{0x78},
  };

  EXPECT_EQ(q::util::read_16(data, 0), 0x1234);
  EXPECT_EQ(q::util::read_16(data, 2), 0x5678);
}
