/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */

#include "chat_server.hpp"

namespace otus::chat_server {
chat_server::chat_server() {
	// Create and add predefined rooms
	chat_rooms_["default"] = std::make_shared<chat_room>();
	chat_rooms_["room1"] = std::make_shared<chat_room>();
	chat_rooms_["room2"] = std::make_shared<chat_room>();
}

std::shared_ptr<chat_room> chat_server::get_room(const std::string &name) {
	if (auto it = chat_rooms_.find(name); it != chat_rooms_.end()) {
		return it->second;
	}
	return chat_rooms_["default"];
}
} // namespace otus::chat_server