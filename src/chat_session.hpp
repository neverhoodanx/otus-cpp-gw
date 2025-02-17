/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */
#pragma once

#include "chat_network.hpp"
#include "chat_participant.hpp"
#include "chat_server.hpp"
#include <memory>

namespace otus::chat_server {

class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session> {
  public:
	chat_session(std::shared_ptr<chat_network> network, std::shared_ptr<chat_room> room,
	             std::shared_ptr<chat_server> server, const user_info &user_info);
	void start();
	void deliver(const std::string &msg) override;

  private:
	void process_message(uint32_t tag, std::string data);
	void stop();

	template <typename T> void process_message(const T &msg) {
		std::cout << "unwrapped process message: " << msg.GetTypeName() << std::endl;
	}

	template <typename T> void pars_message(const std::string &data) {
		T msg;
		msg.ParseFromString(data);
		std::cout << "process_message: " << msg.GetTypeName() << std::endl;
		process_message<T>(msg);
	}

	std::shared_ptr<chat_network> network_session_;
	std::shared_ptr<chat_room> room_;
	std::shared_ptr<chat_server> server_;
	user_info user_info_;
};

} // namespace otus::chat_server