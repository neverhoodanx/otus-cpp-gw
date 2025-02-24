/**
 *  OTUS Graduate Work
 *  Chat room
 *  (c) 2025
 */
#pragma once

#include "chat_participant.hpp"
#include "user_info.hpp"

#include <deque>
#include <iostream>
#include <memory>
#include <set>

namespace otus::chat_server {

/**
 * @class chat_room
 * @brief Class representing a chat room
 *
 * The chat_room class manages participants and message delivery within
 * a chat environment
 */
class chat_room {
  public:
	/**
	 * @brief Constructs a chat_room with the given name
	 *
	 * @param name The name of the chat room
	 */
	explicit chat_room(std::string name);

	/**
	 * @brief Adds a participant to the chat room
	 *
	 * This function registers a participant in the room and associates them with
	 * their user information
	 *
	 * @param participant Pointer to the participant
	 * @param user_info The user_info structure
	 * @param silence Very trigger age case
	 */
	void join(std::shared_ptr<chat_participant> participant, const user_info &user_info,
	          bool silence = false);

	/**
	 * @brief Removes a participant from the chat room
	 *
	 * This function unregisters a participant from the room based on their user information
	 *
	 * @param participant Pointer to the participant
	 * @param user_info The user_info structure
	 */
	void leave(std::shared_ptr<chat_participant> participant, const user_info &user_info);

	/**
	 * @brief Delivers a message to all participants in the chat room
	 *
	 * @param msg The message to be delivered
	 */
	void deliver(const std::string &msg);

	/**
	 * @brief Delivers a message to a specific participant in the chat room
	 *
	 * @param id The unique identifier of the recipient
	 * @param msg The message to be delivered
	 */
	void deliver_to(const std::string &id, const std::string &msg);

	/**
	 * @brief Retrieves a list of users currently online in the chat room
	 *
	 * @return A set containing the nicknames of active participants
	 */
	std::set<std::string> get_users_online() const;

	/**
	 * @brief Retrieves the name of the chat room
	 *
	 * @return A reference to the chat room name
	 */
	const std::string &get_name() const;

  private:
	/**
	 * @brief Set of active participants in the chat room
	 */
	std::set<std::shared_ptr<chat_participant>> participants_;

	/**
	 * @brief Set of user nicknames currently in the chat room
	 */
	std::set<std::string> user_infos_;

	/**
	 * @brief Maximum number of recent messages stored in the room
	 */
	enum { max_recent_msgs = 10000 };

	/**
	 * @brief Queue storing the most recent messages
	 */
	std::deque<std::string> recent_msgs_;

	/**
	 * @brief Name of the chat room
	 */
	std::string name_;
};

} // namespace otus::chat_server