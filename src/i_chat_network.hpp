/**
 *  OTUS Graduate Work
 *  Chat network session
 *  (c) 2025
 */

#pragma once

#include <functional>

/**
 * @interface i_chat_network
 * @brief Interface for chat network communication
 *
 * This interface abstracts the network layer to simplify testing
 */
class i_chat_network {
  public:
	using message_handler = std::function<void(uint32_t tag, std::string data)>;

	virtual ~i_chat_network() = default;

	/**
	 * @brief Sets the handler for incoming messages.
	 * @param handler The callback function to handle incoming messages.
	 */
	virtual void set_handler(message_handler handler) = 0;

	/**
	 * @brief Starts the network connection processing.
	 */
	virtual void start() = 0;

	/**
	 * @brief Sends a message to the client.
	 * @param msg The message to send, as a string.
	 */
	virtual void send(const std::string &msg) = 0;

	/**
	 * @brief Stops the network connection.
	 */
	virtual void stop() = 0;
};
