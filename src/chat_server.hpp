/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */
#pragma once

#include "chat_room.hpp"

#include <map>

namespace otus::chat_server {

/**
 * @brief Class representing the chat server and managing rooms.
 */
class chat_server {
  public:
	chat_server();

	/**
	 * @brief Get a room by name.
	 * @param name Room name.
	 * @return Pointer to the room.
	 */
	std::shared_ptr<chat_room> get_room(const std::string &name);
	/**
	 *
	 */
	std::vector<std::string> get_avalible_room() const;

  private:
	std::map<std::string, std::shared_ptr<chat_room>> chat_rooms_;
	std::map<std::string, uint32_t> chat_users_;
};

} // namespace otus::chat_server