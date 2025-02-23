/**
 *  OTUS Graduate Work
 *  Сhat server
 *  (c) 2025
 */

#include "user_service.hpp"

namespace otus::chat_server {

user_service::user_service() {
}

user_info user_service::get_cached_user(const std::string &nick, const std::string &id,
                                        const std::string &token) {
	if (auto it = users_.find(nick); it != users_.end()) {
		if (it->second.token == token) {
			return it->second;
		} else {
			return {};
		}
	}
	users_.emplace(nick, user_info{nick, id, token});
	return user_info{nick, id, token};
}
} // namespace otus::chat_server