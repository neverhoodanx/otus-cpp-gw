/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */
#pragma once

#include "chat_packet.hpp"
#include "i_chat_network.hpp"

#define ASIO_HAS_CO_AWAIT
#include <asio.hpp>
#include <deque>
#include <functional>
#include <memory>

namespace otus::chat_server {

/**
 * @class chat_network
 * @brief Implementation of chat network communication.
 *
 * Class handles receiving and sending messages using ASIO (Asynchronous I/O).
 */
class chat_network : public i_chat_network, public std::enable_shared_from_this<chat_network> {
  public:
	using message_handler = std::function<void(uint32_t tag, std::string data)>;
	/**
	 * @brief Constructs a chat_network object
	 * @param socket The TCP socket used for the connection
	 */
	explicit chat_network(asio::ip::tcp::socket socket);
	/**
	 * @brief Sets the handler for incoming messages
	 * @param handler The callback function to handle incoming messages
	 */
	void set_handler(message_handler handler);
	/**
	 * @brief Starts the network connection processing
	 *
	 * Initiates the network operations, such as reading and writing messages
	 */
	void start();
	/**
	 * @brief Sends a message to the client.
	 * @param msg The message to send, as a string
	 */
	void send(const std::string &msg);
	/**
	 * @brief Stops the network connection
	 *
	 * Gracefully shuts down the network operations and releases resources
	 */
	void stop();

  private:
	/**
	 * @brief Asynchronously reads incoming messages
	 *
	 * Coroutine continuously reads data from the socket and processes it
	 */
	asio::awaitable<void> reader();
	/**
	 * @brief Asynchronously sends messages from the queue
	 *
	 * Coroutine sends messages stored in the write queue to the client
	 */
	asio::awaitable<void> writer();
	/**
	 * @brief Processes received messages.
	 *
	 * Coroutine handles the logic for processing incoming messages
	 * such as parsing and forwarding them to the message handler
	 */
	asio::awaitable<void> process_messages();

	asio::ip::tcp::socket socket_;       ///< The TCP socket for communication
	asio::steady_timer timer_;           ///< Timer for asynchronous operations
	asio::steady_timer timer2_;          ///< Timer for asynchronous operations
	std::deque<std::string> write_msgs_; ///< Queue of messages to be sent
	std::deque<packet> messages_;        ///< Queue of received messages to be processed
	message_handler message_handler_;    ///< Callback function for handling incoming messages
};

} // namespace otus::chat_server