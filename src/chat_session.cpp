/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */

#include "chat_session.hpp"

#include <asio.hpp>

namespace otus::chat_server {
chat_session::chat_session(asio::ip::tcp::socket socket, chat_room &room, chat_server &server,
                           const user_info &user_info)
    : socket_(std::move(socket)), timer_(socket_.get_executor()), timer2_(socket_.get_executor()),
      room_(room), server_(server), user_info_(user_info) {
	timer_.expires_at(std::chrono::steady_clock::time_point::max());
	timer2_.expires_at(std::chrono::steady_clock::time_point::max());
}

void chat_session::start() {

	asio::co_spawn(
	    socket_.get_executor(), [self = shared_from_this()] { return self->reader(); },
	    asio::detached);

	asio::co_spawn(
	    socket_.get_executor(), [self = shared_from_this()] { return self->writer(); },
	    asio::detached);

	asio::co_spawn(
	    socket_.get_executor(), [self = shared_from_this()] { return self->process_messages(); },
	    asio::detached);

	std::cout << "Session started for user_info: " << user_info_.nickname << std::endl;
}

void chat_session::deliver(const std::string &msg) {
	std::cout << "Session deliver for user_info: " << user_info_.nickname << std::endl;
	write_msgs_.push_back(msg);
	timer_.cancel_one();
}

asio::awaitable<void> chat_session::reader() {
	try {
		for (std::string read_msg;;) {
			packet p;
			static const auto size_h = sizeof(header);
			std::size_t n = co_await asio::async_read(socket_, asio::buffer(&p.header_, size_h),
			                                          asio::transfer_exactly(size_h));
			if (n != size_h) {
				//@TODO some error
				continue;
			}
			std::size_t n_h =
			    co_await asio::async_read(socket_, asio::dynamic_buffer(p.data_, 2048),
			                              asio::transfer_exactly(p.header_.length_));
			std::cout << "RAW message size: " << n_h << std::endl;
			auto b = messages_.empty();
			messages_.push_back(p);
			if (b) {
				timer2_.cancel_one();
			}
		}
	} catch (std::exception &) {
		stop();
	}
}

asio::awaitable<void> chat_session::writer() {
	try {
		while (socket_.is_open()) {
			if (write_msgs_.empty()) {
				asio::error_code ec;
				co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
			} else {
				co_await asio::async_write(socket_, asio::buffer(write_msgs_.front()),
				                           asio::use_awaitable);
				write_msgs_.pop_front();
			}
		}
	} catch (std::exception &) {
		stop();
	}
}

asio::awaitable<void> chat_session::process_messages() {
	try {
		while (socket_.is_open()) {
			if (messages_.empty()) {
				asio::error_code ec;
				co_await timer2_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
			} else {
				auto pkg = messages_.front();
				pars_message(pkg.header_.tag_, pkg.data_);
				messages_.pop_front();
			}
		}
	} catch (std::exception &) {
		stop();
	}
}

void chat_session::stop() {
	room_.leave(shared_from_this(), user_info_);
	socket_.close();
	timer_.cancel();
	timer2_.cancel();

	std::cout << "Session stopped for user_info: " << user_info_.nickname << std::endl;
}

template <> void chat_session::process_message(const chat_proto::Auth &msg) {
	std::cout << "Session auth token: " << msg.token() << std::endl;
	if (msg.token() == "super_secretny_token") {
		user_info_.nickname = msg.nick();
		room_.join(shared_from_this(), user_info_);
		chat_proto::Auth ans;
		ans.CopyFrom(msg);
		ans.set_id(std::stoi(user_info_.id));
		room_.deliver(serialize_packet(chat_proto::Type_Auth, ans.SerializeAsString()));
		//@TODO send system Message user join to room
	}
}

template <> void chat_session::process_message(const chat_proto::IM &msg) {
	room_.deliver(serialize_packet(chat_proto::Type_IM, msg.SerializeAsString()));
}

template <> void chat_session::process_message(const chat_proto::ServiceIM &msg) {
	std::cout << "Session action: " << msg.action() << std::endl;
	chat_proto::ServiceIM im;
	im.set_id(std::stoi(user_info_.id));
	im.set_nick(user_info_.nickname);
	im.set_action(msg.action());
	switch (msg.action()) {
	case chat_proto::ServiceIM_Actions_room_list: {
		auto rooms = server_.get_avalible_room();
		std::string buf;
		for (const auto &val : rooms) {
			buf.append(" - ");
			buf.append(val);
			buf.append("\n");
		}
		im.set_data(buf);
		break;
	}
	case chat_proto::ServiceIM_Actions_user_list: {
		auto users = room_.get_users_online();
		std::string buf;
		for (const auto &val : users) {
			buf.append(" - ");
			buf.append(val);
			buf.append("\n");
		}
		im.set_data(buf);
		break;
	}
	case chat_proto::ServiceIM_Actions_join: {
		std::cout << "Session action data: " << msg.data() << std::endl;
		room_.leave(shared_from_this(), user_info_);
		auto r = server_.get_room(msg.data());
		room_ = *r;
		room_.join(shared_from_this(), user_info_);
		im.set_data(msg.data()); //@TODO set room name from current room; it can bi default
		break;
	}
	case chat_proto::ServiceIM_Actions_leave: {
		room_.leave(shared_from_this(), user_info_);
		auto r = server_.get_room("default");
		room_ = *r;
		room_.join(shared_from_this(), user_info_);
		im.set_data(msg.data()); //@TODO set room name from current room; it can bi default
		break;
	}
	default:
		break;
	}
	deliver(serialize_packet(chat_proto::Type_ServiceIM, im.SerializeAsString()));
}

void chat_session::pars_message(uint32_t tag, std::string &data) {
	typedef void (chat_session::*call_parser)(const std::string &);
	static const std::unordered_map<uint32_t, call_parser> map_hendler({
	    {chat_proto::Type_Auth, &chat_session::pars_message<chat_proto::Auth>},
	    {chat_proto::Type_IM, &chat_session::pars_message<chat_proto::IM>},
	    {chat_proto::Type_ServiceIM, &chat_session::pars_message<chat_proto::ServiceIM>},
	});
	auto it = map_hendler.find(tag);
	if (it != map_hendler.end()) {
		(this->*it->second)(data);
	} else {
		std::cout << "There is no handler for:" << tag << std::endl;
	}
}
} // namespace otus::chat_server