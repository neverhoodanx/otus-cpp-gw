#include "chat_server.hpp"

#include <gtest/gtest.h>

namespace otus::chat_server {

class ChatServerTest : public ::testing::Test {
  protected:
	void SetUp() override { server = std::make_unique<chat_server>(); }

	std::unique_ptr<chat_server> server;
};

TEST_F(ChatServerTest, GetRoom) {
	auto room = server->get_room("default");
	ASSERT_NE(room, nullptr);
}

TEST_F(ChatServerTest, GetAvailableRooms) {
	auto rooms = server->get_avalible_room();
	ASSERT_FALSE(rooms.empty());
	EXPECT_NE(std::find(rooms.begin(), rooms.end(), "default"), rooms.end());
}

TEST_F(ChatServerTest, AuthValidUser) {
	user_info user = server->auth("test_user", "123", "valid_token");
	ASSERT_FALSE(user.id.empty());
}

} // namespace otus::chat_server