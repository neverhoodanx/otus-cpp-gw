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
/**
 * @class chat_session
 * @brief Manages an individual user session in the chat server
 *
 * The chat_session class handles user authentication, message parsing, and room transitions
 */
class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session> {
  public:
	/**
	 * @brief Constructs a chat_session instance
	 *
	 * @param Chat network interface
	 * @param Chat room the user joins
	 * @param Chat server
	 * @param user_info User authentication information
	 */
	chat_session(std::shared_ptr<chat_network> network, std::shared_ptr<chat_room> room,
	             std::shared_ptr<chat_server> server, const user_info &user_info);
	/**
	 * @brief Starts the session by joining the chat room and listening for messages
	 */
	void start();
	/**
	 * @brief Delivers a message to the chat network
	 *
	 * @param msg The message to send
	 */
	void deliver(const std::string &msg) final;

  private:
	/**
	 * @brief Processes an incoming message with a specific tag
	 *
	 * @param tag Message type identifier
	 * @param data The raw message data
	 */
	void process_message(uint32_t tag, std::string data);
	/**
	 * @brief Stops the session by leaving the chat room and disconnecting from the network
	 */
	void stop();
	/**
	 * @brief Processes messages of various types
	 */
	template <typename T> void process_message(const T &msg) {
		std::cout << "unwrapped process message: " << msg.GetTypeName() << std::endl;
	}
	/**
	 * @brief Parses messages of various types
	 */
	template <typename T> void pars_message(const std::string &data) {
		T msg;
		msg.ParseFromString(data);
		std::cout << "process_message: " << msg.GetTypeName() << std::endl;
		process_message<T>(msg);
	}

	std::shared_ptr<chat_network> network_session_; ///< Chat network interface
	std::shared_ptr<chat_room> room_;               ///< The chat room the session is part of
	std::shared_ptr<chat_server> server_;           ///< Chat server reference
	user_info user_info_;                           ///< User authentication information
};
} // namespace otus::chat_server