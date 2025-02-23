#include "chat_room.hpp"
#include "chat_server.hpp"
#include "chat_session.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

namespace otus::chat_server {
class MockChatNetwork : public i_chat_network {
  public:
	MOCK_METHOD(void, set_handler, (message_handler handler), (override));
	MOCK_METHOD(void, start, (), (override));
	MOCK_METHOD(void, send, (const std::string &msg), (override));
	MOCK_METHOD(void, stop, (), (override));
};

class ChatSessionTest : public ::testing::Test {
  protected:
	void SetUp() override {
		mock_network = std::make_shared<MockChatNetwork>();
		mock_room = std::make_shared<chat_room>("default");
		mock_server = std::make_shared<chat_server>();

		user_info info{"user1", "id1"};
		session = std::make_shared<chat_session>(mock_network, mock_room, mock_server, info);
	}

	std::shared_ptr<MockChatNetwork> mock_network;
	std::shared_ptr<chat_room> mock_room;
	std::shared_ptr<chat_server> mock_server;
	std::shared_ptr<chat_session> session;
};

TEST_F(ChatSessionTest, StartSession) {
	EXPECT_CALL(*mock_network, start()).Times(1);
	// EXPECT_CALL(*mock_room, join(_, _)).Times(1);

	session->start();
}

TEST_F(ChatSessionTest, DeliverMessage) {
	std::string message = "Hello, World!";
	EXPECT_CALL(*mock_network, send(message)).Times(1);

	session->deliver(message);
}

TEST_F(ChatSessionTest, ProcessAuthMessage) {
	chat_proto::Auth auth_msg;
	auth_msg.set_token("token123");
	auth_msg.mutable_user()->set_nick("user1");

	user_info expected_info{"user1", "id1"};
	// EXPECT_CALL(*mock_server, auth("user1", _, "token123")).WillOnce(Return(expected_info));
	// EXPECT_CALL(*mock_room, join(_, expected_info)).Times(1);
	EXPECT_CALL(*mock_network, send(_)).Times(1); // Verify a response is sent

	std::string serialized_msg = auth_msg.SerializeAsString();
	session->process_message(chat_proto::Type_Auth, serialized_msg);
}

// TEST_F(ChatSessionTest, ProcessIMMessage) {
// 	chat_proto::IM im_msg;
// 	im_msg.set_content("Hello, World!");

// 	EXPECT_CALL(*mock_room, deliver(_)).Times(1);

// 	std::string serialized_msg = im_msg.SerializeAsString();
// 	session->process_message(chat_proto::Type_IM, serialized_msg);
// }

// TEST_F(ChatSessionTest, ProcessRoomJoinMessage) {
// 	chat_proto::RoomJoin room_join_msg;
// 	room_join_msg.set_room_name("new_room");

// 	EXPECT_CALL(*mock_server, get_room("new_room")).Times(1);
// 	EXPECT_CALL(*mock_room, leave(_, _)).Times(1);
// 	EXPECT_CALL(*mock_room, join(_, _)).Times(1);
// 	EXPECT_CALL(*mock_room, deliver(_)).Times(1);

// 	std::string serialized_msg = room_join_msg.SerializeAsString();
// 	session->process_message(chat_proto::Type_RoomJoin, serialized_msg);
// }
} // namespace otus::chat_server