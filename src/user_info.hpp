/**
 *  OTUS Graduate Work
 *  User info
 *  (c) 2025
 */
#pragma once

#include <string>

namespace otus::chat_server {
/**
 * @brief Structure to store user information
 */
struct user_info {
	std::string nickname; ///< User's nickname
	std::string id;       ///< Unique user identifier @TODO change to int
	std::string token;
};
} // namespace otus::chat_server
