#include "chat_packet.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace otus::chat_server {

class PacketTest : public ::testing::Test {
  protected:
	void SetUp() override {}
};

TEST_F(PacketTest, SerializePacketCreatesValidBuffer) {
	uint32_t tag = 42;
	std::string data = "Hello, world!";
	std::string serialized = serialize_packet(tag, data);
	EXPECT_EQ(serialized.size(), data.size() + 8);
	uint32_t extracted_tag;
	uint32_t extracted_size;
	std::memcpy(&extracted_tag, serialized.data(), sizeof(extracted_tag));
	std::memcpy(&extracted_size, serialized.data() + 4, sizeof(extracted_size));
	EXPECT_EQ(extracted_tag, tag);
	EXPECT_EQ(extracted_size, data.size());
}

TEST_F(PacketTest, SerializePacketHandlesEmptyData) {
	uint32_t tag = 99;
	std::string data = "";
	std::string serialized = serialize_packet(tag, data);
	EXPECT_EQ(serialized.size(), 8);
	uint32_t extracted_tag;
	uint32_t extracted_size;
	std::memcpy(&extracted_tag, serialized.data(), sizeof(extracted_tag));
	std::memcpy(&extracted_size, serialized.data() + 4, sizeof(extracted_size));
	EXPECT_EQ(extracted_tag, tag);
	EXPECT_EQ(extracted_size, 0);
}

} // namespace otus::chat_server
