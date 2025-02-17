/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */

#include "chat_server.hpp"
#include "chat_messages.pb.h"
#include "chat_packet.hpp"

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

void chat_server::stop(bool graceful) {
	if (!graceful) {
		return;
	}
	for (const auto &[key, val] : chat_rooms_) {
		auto msg = chat_proto::ServerMaintenance();
		msg.set_error_code(chat_proto::ServerMaintenance_error_codes_ec_shutdown);
		msg.set_message("The server is temporarily suspended, please contact the administrator.");
		val->deliver(serialize_packet(chat_proto::Type_ServerMaintenance, msg.SerializeAsString()));
	}
}

user_info chat_server::auth(const std::string &nick, const std::string &id,
                            const std::string &token) {
	return user_service_.get_cached_user(nick, id, token);
}

} // namespace otus::chat_server