/**
 *  OTUS Graduate Work
 *  Global chat server
 *  (c) 2025
 */

#include "chat_service.hpp"
#include "chat_server.hpp"
#include "chat_session.hpp"
#include "user_info.hpp"

#include <asio/signal_set.hpp>
#include <asio/steady_timer.hpp>

namespace otus::chat_server {

unsigned int chat_session::user_id_counter = 0;

/**
 * @brief Asynchronous listener for accepting incoming connections.
 * @param acceptor The socket acceptor.
 * @param server The chat server.
 */
asio::awaitable<void> listener(asio::ip::tcp::acceptor acceptor, chat_server &server) {
	for (;;) {
		auto socket = co_await acceptor.async_accept(asio::use_awaitable);

		// Retrieve user data
		std::string nickname = "User" + std::to_string(chat_session::user_id_counter);
		std::string id = std::to_string(chat_session::user_id_counter++);
		user_info user{nickname, id};

		std::cout << "New user connected: " << nickname << " with ID: " << id << "\n";

		// Select room
		auto room = server.get_room("default");

		std::make_shared<chat_session>(std::move(socket), *room, server, user)->start();
	}
}

void start(uint16_t port) {
	asio::io_context io_context(1);
	chat_server server;
	co_spawn(io_context,
	         listener(asio::ip::tcp::acceptor(io_context, {asio::ip::tcp::v4(), port}), server),
	         asio::detached);

	asio::signal_set signals(io_context, SIGINT, SIGTERM);
	signals.async_wait([&](auto, auto) {
		stop();
		io_context.stop();
	});

	std::cout << "Chat server is running..." << std::endl;
	io_context.run();
}

void stop() {
	std::cout << "Chat server is stopped..." << std::endl;
}

} // namespace otus::chat_server