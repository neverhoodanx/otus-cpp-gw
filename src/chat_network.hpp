/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */
#pragma once

#include "chat_packet.hpp"

#define ASIO_HAS_CO_AWAIT

#include <asio.hpp>
#include <deque>
#include <functional>
#include <memory>

namespace otus::chat_server {

class chat_network : public std::enable_shared_from_this<chat_network> {
  public:
	using message_handler = std::function<void(uint32_t tag, std::string data)>;

	chat_network(asio::ip::tcp::socket socket);
	void set_handler(message_handler handler);
	void start();
	void send(const std::string &msg);
	void stop();

  private:
	asio::awaitable<void> reader();
	asio::awaitable<void> writer();
	asio::awaitable<void> process_messages();

	asio::ip::tcp::socket socket_;
	asio::steady_timer timer_;
	asio::steady_timer timer2_;
	std::deque<std::string> write_msgs_;
	std::deque<packet> messages_;
	message_handler message_handler_;
};

} // namespace otus::chat_server