// /**
//  *  OTUS Graduate Work
//  *  Chat session
//  *  (c) 2025
//  */
// #pragma once

// #include "chat_packet.hpp"

// #define ASIO_HAS_CO_AWAIT

// #include <asio/awaitable.hpp>
// #include <asio/co_spawn.hpp>
// #include <asio/detached.hpp>
// #include <asio/io_context.hpp>
// #include <asio/ip/tcp.hpp>
// #include <asio/read_until.hpp>
// #include <asio/redirect_error.hpp>
// #include <asio/use_awaitable.hpp>
// #include <asio/write.hpp>

// #include <map>

// namespace otus::chat_server {
// /**
//  * @brief Class representing a user session.
//  */
// class chat_network : public std::enable_shared_from_this<chat_network> {
//   public:
// 	/**
// 	 * @brief Ctor
// 	 */
// 	chat_network(asio::ip::tcp::socket socket);
// 	/**
// 	 * @brief Start a chat session for the user.
// 	 */
// 	void start();

// 	/**
// 	 * @brief
// 	 */
// 	virtual void on_packet(uint32_t tag, const std::string &data);

// 	/**
// 	 * @brief
// 	 */
// 	virtual void on_stoped();

// 	void deliver(const std::string &msg);

// 	static unsigned int user_id_counter; // user_info ID counter, @TODO move to server

//   private:
// 	/**
// 	 * @brief Handle reader.
// 	 */
// 	asio::awaitable<void> reader();
// 	/**
// 	 * @brief Handle writer.
// 	 */
// 	asio::awaitable<void> writer();
// 	/**
// 	 * @brief Handle writer.
// 	 */
// 	asio::awaitable<void> process_messages();

// 	/**
// 	 * @brief Stop a chat session for the user.
// 	 */
// 	void stop();

// 	asio::ip::tcp::socket socket_;
// 	asio::steady_timer timer_;
// 	asio::steady_timer timer2_;
// 	std::deque<std::string> write_msgs_;
// 	std::deque<packet> messages_;
// };
// } // namespace otus::chat_server