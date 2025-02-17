/**
 *  OTUS Graduate Work
 *  Chat room
 *  (c) 2025
 */

#include "chat_room.hpp"

namespace otus::chat_server {
chat_room::chat_room(std::string name) : name_(name) {
}

void chat_room::join(std::shared_ptr<chat_participant> participant, const user_info &user_info) {
	participants_.insert(participant);
	user_infos_.insert(user_info.nickname);
	for (auto msg : recent_msgs_)
		participant->deliver(msg);

	std::cout << "[" << name_ << "]: " << "user_info " << participant->id_ << " joined the room."
	          << std::endl;
}

void chat_room::leave(std::shared_ptr<chat_participant> participant, const user_info &user_info) {
	participants_.erase(participant);
	user_infos_.erase(user_info.nickname);

	std::cout << "[" << name_ << "]: " << "user_info " << user_info.nickname << " left the room."
	          << std::endl;
}

void chat_room::deliver(const std::string &msg) {
	recent_msgs_.push_back(msg);
	while (recent_msgs_.size() > max_recent_msgs) {
		recent_msgs_.pop_front();
	}
	for (auto participant : participants_) {
		participant->deliver(msg);
	}
	std::cout << "[" << name_ << "]: " << "Delivered message: " << msg << std::endl;
}

void chat_room::deliver_to(const std::string &id, const std::string &msg) {
	auto it = std::find_if(participants_.begin(), participants_.end(),
	                       [&id](const std::shared_ptr<chat_participant> &participant) {
		                       return participant->id_ == id;
	                       });
	if (it != participants_.end()) {
		(*it)->deliver(msg);
	}
}

std::set<std::string> chat_room::get_users_online() const {
	return user_infos_;
}

const std::string &chat_room::get_name() const {
	return name_;
}

} // namespace otus::chat_server