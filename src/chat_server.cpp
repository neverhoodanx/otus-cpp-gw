/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */

#include "chat_server.hpp"

namespace otus::chat_server {
chat_server::chat_server() {
	// Create and add predefined rooms
	chat_rooms_["default"] = std::make_shared<chat_room>("default");
	chat_rooms_["room1"] = std::make_shared<chat_room>("room1");
	chat_rooms_["room2"] = std::make_shared<chat_room>("room2");
}

std::shared_ptr<chat_room> chat_server::get_room(const std::string &name) {
	if (auto it = chat_rooms_.find(name); it != chat_rooms_.end()) {
		return it->second;
	}
	return chat_rooms_["default"];
}

std::vector<std::string> chat_server::get_avalible_room() const {
	auto ret = std::vector<std::string>();
	for (const auto &[key, val] : chat_rooms_) {
		ret.push_back(key);
	}
	return ret;
}
} // namespace otus::chat_server