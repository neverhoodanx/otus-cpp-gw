/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */
#pragma once

#include "chat_packet.hpp"
#include "chat_participant.hpp"
#include "chat_server.hpp"

#define ASIO_HAS_CO_AWAIT

#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/redirect_error.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

#include <map>

namespace otus::chat_server {
/**
 * @brief Class representing a user session.
 */
class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session> {
  public:
	/**
	 * @brief Ctor
	 */
	chat_session(asio::ip::tcp::socket socket, std::shared_ptr<chat_room> room, chat_server &server,
	             const user_info &user_info);
	/**
	 * @brief Start a chat session for the user.
	 */
	void start();
	/**
	 * @brief override chat_participant::deliver
	 */
	void deliver(const std::string &msg) final;

	static unsigned int user_id_counter; // user_info ID counter, @TODO move to server

  private:
	/**
	 * @brief Handle reader.
	 */
	asio::awaitable<void> reader();
	/**
	 * @brief Handle writer.
	 */
	asio::awaitable<void> writer();
	/**
	 * @brief Process proto message
	 */
	asio::awaitable<void> process_messages();
	/**
	 * @brief Stop a chat session for the user.
	 */
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

	void pars_message(uint32_t tag, std::string &data);

	asio::ip::tcp::socket socket_;
	asio::steady_timer timer_;
	asio::steady_timer timer2_;
	std::shared_ptr<chat_room> room_;
	chat_server &server_;
	user_info user_info_;
	std::deque<std::string> write_msgs_;
	std::deque<packet> messages_;
};
} // namespace otus::chat_server