/**
 *  OTUS Graduate Work
 *  Global chat server
 *  (c) 2025
 */

#include "chat_service.hpp"
#include "chat_network.hpp"
#include "chat_server.hpp"
#include "chat_session.hpp"
#include "user_info.hpp"

#include <asio/signal_set.hpp>
#include <asio/steady_timer.hpp>

namespace otus::chat_server {
/**
 * @brief Counter for generating unique user IDs.
 */
static unsigned int user_id_counter = 0;
/**
 * @brief Pointer to the chat server instance.
 */
std::shared_ptr<chat_server> server_;

/**
 * @brief Asynchronous listener for accepting incoming client connections.
 *
 * This function runs in a loop, continuously accepting new client connections.
 * Each accepted connection is assigned a unique user ID and added to the default chat room.
 *
 * @param acceptor The socket acceptor used to listen for new connections.
 * @param server Chat server instance managing the connections.
 */
asio::awaitable<void> listener(asio::ip::tcp::acceptor acceptor,
                               std::shared_ptr<chat_server> server) {
	for (;;) {
		auto socket = co_await acceptor.async_accept(asio::use_awaitable);

		// Retrieve user data
		std::string nickname = "";
		std::string id = std::to_string(user_id_counter++);
		user_info user{nickname, id, ""};

		std::cout << "New user is connecting..." << std::endl;

		// Select defaut room
		auto room = server->get_room("default");
		std::shared_ptr<i_chat_network> net = std::make_shared<chat_network>(std::move(socket));
		std::make_shared<chat_session>(net, room, server, user)->start();
	}
}

void start(uint16_t port) {
	asio::io_context io_context(1);
	server_ = std::make_shared<chat_server>();
	co_spawn(io_context,
	         listener(asio::ip::tcp::acceptor(io_context, {asio::ip::tcp::v4(), port}), server_),
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
	server_->stop(true);
	std::cout << "Chat server is stopped..." << std::endl;
}

} // namespace otus::chat_server