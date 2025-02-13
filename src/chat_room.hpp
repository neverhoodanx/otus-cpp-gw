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
 * @brief Class representing a chat room.
 */
class chat_room {
  public:
	/**
	 * @brief Ctor
	 */
	explicit chat_room(std::string name);
	/**
	 * @brief Add participant to the room.
	 * @param participant Pointer to the participant.
	 * @param user_info The user_info structure.
	 */
	void join(std::shared_ptr<chat_participant> participant, const user_info &user_info);

	/**
	 * @brief Remove participant from the room.
	 * @param participant Pointer to the participant.
	 * @param user_info The user_info structure.
	 */
	void leave(std::shared_ptr<chat_participant> participant, const user_info &user_info);

	/**
	 * @brief Deliver a message to all participants in the room.
	 * @param msg Message to deliver.
	 */
	void deliver(const std::string &msg);

	/**
	 * @brief Get a list of user_infos currently online in the room.
	 * @return Set of user_info nicknames.
	 */
	std::set<std::string> get_users_online() const;

	/**
	 * @brief Get a room name
	 * @return room name
	 */
	const std::string &get_name() const;

  private:
	std::set<std::shared_ptr<chat_participant>> participants_;
	std::set<std::string> user_infos_;
	enum { max_recent_msgs = 100 };
	std::deque<std::string> recent_msgs_;
	std::string name_;
};
} // namespace otus::chat_server