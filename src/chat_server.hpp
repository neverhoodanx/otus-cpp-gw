/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */
#pragma once

#include "chat_room.hpp"
#include "user_service.hpp"

#include <map>
#include <vector>

namespace otus::chat_server {

/**
 * @class chat_server
 * @brief Class representing the chat server and managing rooms
 *
 * The chat_server class handles the management of chat rooms, user authentication,
 * and server shutdown
 */
class chat_server {
  public:
	/**
	 * @brief Constructs a chat_server instance
	 *
	 * Initializes predefined chat rooms
	 */
	chat_server();

	/**
	 * @brief Retrieves a chat room by name
	 *
	 * If the specified room does not exist, the default chat room is returned
	 *
	 * @param name The name of the chat room
	 * @return Pointer to the requested chat room
	 */
	std::shared_ptr<chat_room> get_room(const std::string &name);

	/**
	 * @brief Retrieves a list of available chat rooms
	 *
	 * @return A vector containing the names of available chat rooms
	 */
	std::vector<std::string> get_avalible_room() const;

	/**
	 * @brief Stops the chat server
	 *
	 * If graceful shutdown is requested, sends a maintenance message to all rooms
	 *
	 * @param graceful Indicates whether the shutdown should be graceful
	 */
	void stop(bool graceful);

	/**
	 * @brief Authenticates a user
	 *
	 * Validates user credentials and retrieves user information from cache
	 *
	 * @param nick The user's nickname
	 * @param id The user's unique identifier
	 * @param token The authentication token
	 * @return The authenticated user information
	 */
	user_info auth(const std::string &nick, const std::string &id, const std::string &token);

  private:
	/**
	 * @brief Chat rooms managed by the server
	 */
	std::map<std::string, std::shared_ptr<chat_room>> chat_rooms_;

	/**
	 * @brief Active chat users and their associated data
	 */
	std::map<std::string, uint32_t> chat_users_;

	/**
	 * @brief User service instance for handling authentication
	 */
	user_service user_service_;
};

} // namespace otus::chat_server