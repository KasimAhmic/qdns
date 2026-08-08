#include "core/header.hpp"

#include <array>
#include <cstdint>

#include "../testing.hpp"

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
  EXPECT_EQ(header.get_id(), 0x1234);
  EXPECT_EQ(header.get_flags(), 0x5678);
  EXPECT_EQ(header.get_question_count(), 1);
  EXPECT_EQ(header.get_answer_count(), 2);
  EXPECT_EQ(header.get_authority_count(), 3);
  EXPECT_EQ(header.get_additional_records(), 4);
}

class HeaderOpCodeTest : public testing::TestWithParam<std::pair<uint16_t, q::dns::OpCode>> {};

TEST_P(HeaderOpCodeTest, ExtractsOpCodeFromFlags) {
  const auto &[flags, expectedOpCode] = GetParam();

  const q::dns::Header header(0, flags, 0, 0, 0, 0);
  EXPECT_EQ(header.get_op_code(), expectedOpCode);
}

INSTANTIATE_TEST_SUITE_P(HeaderOpCodeTests,
                         HeaderOpCodeTest,
                         testing::Values(std::make_pair(0x0000, q::dns::OpCode::Query),
                                         std::make_pair(0x0800, q::dns::OpCode::IQuery),
                                         std::make_pair(0x1000, q::dns::OpCode::Status),
                                         std::make_pair(0x2000, q::dns::OpCode::Notify),
                                         std::make_pair(0x2800, q::dns::OpCode::Update)));

class HeaderResponseCodeTest : public testing::TestWithParam<std::pair<uint16_t, q::dns::ResponseCode>> {};

TEST_P(HeaderResponseCodeTest, ExtractsResponseCodeFromFlags) {
  const auto &[flags, expectedResponseCode] = GetParam();

  const q::dns::Header header(0, flags, 0, 0, 0, 0);
  EXPECT_EQ(header.get_response_code(), expectedResponseCode);
}

INSTANTIATE_TEST_SUITE_P(HeaderResponseCodeTests,
                         HeaderResponseCodeTest,
                         testing::Values(std::make_pair(0x0000, q::dns::ResponseCode::NoError),
                                         std::make_pair(0x0001, q::dns::ResponseCode::FormatError),
                                         std::make_pair(0x0002, q::dns::ResponseCode::ServerFailure),
                                         std::make_pair(0x0003, q::dns::ResponseCode::NameError),
                                         std::make_pair(0x0004, q::dns::ResponseCode::NotImplemented),
                                         std::make_pair(0x0005, q::dns::ResponseCode::Refused)));
