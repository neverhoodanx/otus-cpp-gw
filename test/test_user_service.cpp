#include "user_service.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace otus::chat_server {

class UserServiceTest : public ::testing::Test {
  protected:
	void SetUp() override { service = std::make_unique<user_service>(); }
	std::unique_ptr<user_service> service;
};

TEST_F(UserServiceTest, ReturnsIdTokenMatches) {
	std::string nick = "testuser";
	std::string id = "12345";
	std::string token = "validtoken";
	user_info user = {nick, id, token};
	service->get_cached_user(nick, id, token);
	user_info result = service->get_cached_user(nick, id, token);
	EXPECT_EQ(result.id, id);
}

TEST_F(UserServiceTest, ReturnsEmptyUserTokenDoesNotMatch) {
	std::string nick = "testuser";
	std::string id = "12345";
	std::string token = "validtoken";
	service->get_cached_user(nick, id, token);
	user_info result = service->get_cached_user(nick, id, "wrongtoken");
	EXPECT_EQ(result.id, "");
}

TEST_F(UserServiceTest, AddsUserWhenNotFound) {
	std::string nick = "newuser";
	std::string id = "67890";
	std::string token = "newtoken";
	user_info result = service->get_cached_user(nick, id, token);
	EXPECT_EQ(result.nickname, nick);
	EXPECT_EQ(result.id, id);
	EXPECT_EQ(result.token, token);
}
} // namespace otus::chat_server