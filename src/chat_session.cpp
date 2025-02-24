/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */

#include "chat_session.hpp"

namespace otus::chat_server {

chat_session::chat_session(std::shared_ptr<i_chat_network> network, std::shared_ptr<chat_room> room,
                           std::shared_ptr<chat_server> server, const user_info &user_info)
    : network_session_(std::move(network)), room_(std::move(room)), server_(server),
      user_info_(user_info) {
	network_session_->set_handler(
	    [this](uint32_t tag, std::string data) { process_message(tag, std::move(data)); });
	chat_participant::id_ = user_info_.nickname;
}

void chat_session::start() {
	network_session_->start();
	room_->join(shared_from_this(), user_info_, true);
}
void chat_session::stop() {
	room_->leave(shared_from_this(), user_info_);
	network_session_->stop();
}
void chat_session::deliver(const std::string &msg) {
	network_session_->send(msg);
}

template <> void chat_session::process_message(const chat_proto::Auth &msg) {
	std::cout << "Session auth token: " << msg.token() << std::endl;
	auto info = server_->auth(msg.user().nick(), user_info_.id, msg.token());
	if (info.id.empty()) {
		chat_proto::ServiceIM ret;
		ret.set_error_code(chat_proto::ServiceIM_error_codes_ec_auth_falied);
		chat_session::deliver(
		    serialize_packet(chat_proto::Type_ServiceIM, ret.SerializeAsString()));
		// need remove; self
		return;
	}
	user_info_ = info;
	chat_participant::id_ = msg.user().nick();
	chat_proto::Auth ans;
	ans.CopyFrom(msg);
	ans.mutable_user()->set_id(user_info_.id);
	chat_session::deliver(serialize_packet(chat_proto::Type_Auth, ans.SerializeAsString()));
	room_->join(shared_from_this(), user_info_);
	chat_proto::RoomJoin im;
	auto user = im.mutable_user();
	user->set_nick(user_info_.nickname);
	user->set_id(user_info_.id);
	im.set_room_name("default");
	room_->deliver(serialize_packet(chat_proto::Type_RoomJoin, im.SerializeAsString()));
}

template <> void chat_session::process_message(const chat_proto::IM &msg) {
	room_->deliver(serialize_packet(chat_proto::Type_IM, msg.SerializeAsString()));
}

template <>
void chat_session::process_message([[maybe_unused]] const chat_proto::GetRoomList &msg) {
	auto rooms = server_->get_avalible_room();
	chat_proto::RoomList im;
	for (const auto &val : rooms) {
		im.add_room_names(val);
	}
	chat_session::deliver(serialize_packet(chat_proto::Type_RoomList, im.SerializeAsString()));
}

template <>
void chat_session::process_message([[maybe_unused]] const chat_proto::GetUserList &msg) {
	auto users = room_->get_users_online();
	chat_proto::UserList im;

	for (const auto &val : users) {
		auto user = im.add_users();
		user->set_id("");
		user->set_nick(val);
	}
	chat_session::deliver(serialize_packet(chat_proto::Type_UserList, im.SerializeAsString()));
}

template <> void chat_session::process_message([[maybe_unused]] const chat_proto::RoomLeft &msg) {
	auto session = shared_from_this();
	chat_proto::RoomLeft im;
	auto user = im.mutable_user();
	user->set_nick(user_info_.nickname);
	user->set_id(user_info_.id);
	im.set_room_name("default");
	auto room_name = room_->get_name();
	auto room = server_->get_room("default");
	if (room->get_name() != room_name) {
		room->join(session, user_info_);
		room_->leave(shared_from_this(), user_info_);
		room_->deliver(serialize_packet(chat_proto::Type_RoomLeft, im.SerializeAsString()));
		room_ = room;
		room_->deliver(serialize_packet(chat_proto::Type_RoomJoin, im.SerializeAsString()));
	} else {
		chat_session::deliver(serialize_packet(chat_proto::Type_RoomLeft, im.SerializeAsString()));
	}
}

template <> void chat_session::process_message([[maybe_unused]] const chat_proto::ServiceIM &msg) {
}

template <> void chat_session::process_message(const chat_proto::RoomJoin &msg) {
	auto session = shared_from_this();
	chat_proto::RoomJoin im;
	auto user = im.mutable_user();
	user->set_nick(user_info_.nickname);
	user->set_id(user_info_.id);

	auto room = server_->get_room(msg.room_name());
	room->join(shared_from_this(), user_info_);
	room_->leave(shared_from_this(), user_info_);
	room_->deliver(serialize_packet(chat_proto::Type_RoomLeft, im.SerializeAsString()));
	room_ = room;
	im.set_room_name(msg.room_name()); //@TODO set room name from current room; it can bi default
	room_->deliver(serialize_packet(chat_proto::Type_RoomJoin, im.SerializeAsString()));
}

template <> void chat_session::process_message(const chat_proto::WhisperIM &msg) {
	room_->deliver_to(msg.user_to().nick(),
	                  serialize_packet(chat_proto::Type_WhisperIM, msg.SerializeAsString()));
}

void chat_session::process_message(uint32_t tag, std::string data) {
	typedef void (chat_session::*call_parser)(const std::string &);
	static const std::unordered_map<uint32_t, call_parser> map_hendler({
	    {chat_proto::Type_Auth, &chat_session::pars_message<chat_proto::Auth>},
	    {chat_proto::Type_IM, &chat_session::pars_message<chat_proto::IM>},
	    {chat_proto::Type_ServiceIM, &chat_session::pars_message<chat_proto::ServiceIM>},
	    {chat_proto::Type_GetRoomList, &chat_session::pars_message<chat_proto::GetRoomList>},
	    {chat_proto::Type_GetUserList, &chat_session::pars_message<chat_proto::GetUserList>},
	    {chat_proto::Type_RoomLeft, &chat_session::pars_message<chat_proto::RoomLeft>},
	    {chat_proto::Type_RoomJoin, &chat_session::pars_message<chat_proto::RoomJoin>},
	    {chat_proto::Type_WhisperIM, &chat_session::pars_message<chat_proto::WhisperIM>},
	});
	auto it = map_hendler.find(tag);
	if (it != map_hendler.end()) {
		(this->*it->second)(data);
	} else {
		std::cout << "There is no handler for:" << tag << std::endl;
	}
}
} // namespace otus::chat_server