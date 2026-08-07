#include <array>

#include "../testing.hpp"
#include "core/header.hpp"

TEST(HeaderTest, FailsToParseTruncatedData) {
  const std::array<std::byte, 11> data{};
  const auto result = q::dns::Header::parse(data);

  EXPECT_FALSE(result.has_value());

  const auto *error = std::get_if<q::dns::error::TruncatedDataError>(&result.error());
  ASSERT_NE(error, nullptr);

  EXPECT_EQ(error->section, q::dns::error::ParseSection::Header);
  EXPECT_EQ(error->offset, 0);
  EXPECT_EQ(error->requiredBytes, 12);
  EXPECT_EQ(error->availableBytes, data.size());
}

TEST(HeaderTest, ParsesValidData) {
  const std::array<std::byte, 12> data{
      std::byte{0x12},
      std::byte{0x34}, // ID
      std::byte{0x56},
      std::byte{0x78}, // Flags
      std::byte{0x00},
      std::byte{0x01}, // QDCount
      std::byte{0x00},
      std::byte{0x02}, // ANCount
      std::byte{0x00},
      std::byte{0x03}, // NSCount
      std::byte{0x00},
      std::byte{0x04} // ARCount
  };

  const auto result = q::dns::Header::parse(data);

  ASSERT_TRUE(result.has_value());

  const auto &header = result.value();
  EXPECT_EQ(header.getId(), 0x1234);
  EXPECT_EQ(header.getFlags(), 0x5678);
  EXPECT_EQ(header.getQdCount(), 1);
  EXPECT_EQ(header.getAnCount(), 2);
  EXPECT_EQ(header.getNsCount(), 3);
  EXPECT_EQ(header.getArCount(), 4);
}
