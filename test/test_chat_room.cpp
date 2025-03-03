#include "chat_participant.hpp"
#include "chat_room.hpp"
#include "user_info.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <set>
#include <string>

class MockParticipant : public otus::chat_server::chat_participant {
  public:
	void deliver(const std::string &msg) override { delivered_msgs.push_back(msg); }

	std::vector<std::string> delivered_msgs;
};

class ChatRoomTest : public ::testing::Test {
  public:
	ChatRoomTest() : room(otus::chat_server::chat_room("")) {};

  protected:
	otus::chat_server::chat_room room;
	std::shared_ptr<MockParticipant> participant;

	void SetUp() override {
		participant = std::make_shared<MockParticipant>();
		user = otus::chat_server::user_info{"TestUser", "1"};
	}

	otus::chat_server::user_info user;
};

TEST_F(ChatRoomTest, UserCanJoinRoom) {
	room.join(participant, user);
	std::set<std::string> users = room.get_users_online();
	EXPECT_EQ(users.size(), 1);
	EXPECT_NE(users.find("TestUser"), users.end());
}

TEST_F(ChatRoomTest, UserCanLeaveRoom) {
	room.join(participant, user);
	room.leave(participant, user);
	std::set<std::string> users = room.get_users_online();
	EXPECT_EQ(users.size(), 0);
}

TEST_F(ChatRoomTest, MessageDeliveryToParticipants) {
	room.join(participant, user);
	std::string message = "Hello, World!";
	room.deliver(message);
	EXPECT_EQ(participant->delivered_msgs.size(), 1);
	EXPECT_EQ(participant->delivered_msgs[0], message);
}

TEST_F(ChatRoomTest, RecentMessagesLimit) {
	for (int i = 0; i < 10005; ++i) {
		room.deliver("Message " + std::to_string(i));
	}
	room.join(participant, user);
	EXPECT_EQ(participant->delivered_msgs.size(), 10000);
	EXPECT_EQ(participant->delivered_msgs.front(), "Message 5");
	EXPECT_EQ(participant->delivered_msgs.back(), "Message 10004");
}