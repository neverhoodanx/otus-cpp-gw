/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */
#pragma once

#include "user_info.hpp"

#include <map>
#include <vector>

namespace otus::chat_server {

/**
 * @class user_service
 * @brief Manages user-related operations within the chat server.
 *
 * The user_service class is responsible for maintaining user information
 * and retrieving user details from cache
 */
class user_service {
  public:
	/**
	 * @brief Constructs a new user_service object
	 */
	explicit user_service();

	/**
	 * @brief Retrieves user information from cache
	 *
	 * This function fetches user details based on the provided nickname,
	 * ID, and authentication token
	 *
	 * @param nick The nickname of the user
	 * @param id The unique identifier of the user
	 * @param token The authentication token of the user
	 * @return user_info The retrieved user information
	 */
	user_info get_cached_user(const std::string &nick, const std::string &id,
	                          const std::string &token);

  private:
	/**
	 * @brief A map storing cached user information
	 *
	 * The key is a string representing the user's unique identifier,
	 * and the value is the corresponding user_info object
	 */
	std::map<std::string, user_info> users_;
};

} // namespace otus::chat_server