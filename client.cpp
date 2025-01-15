#define ASIO_HAS_CO_AWAIT

#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/read_until.hpp>
#include <asio/use_awaitable.hpp>
#include <iostream>
#include <string>

#include "src/chat_packet.hpp"

#include <memory>

std::shared_ptr<asio::ip::tcp::socket> sk;

void read_console_input(asio::io_context &io_context, const std::string &nickname) {
	try {
		for (std::string line; std::getline(std::cin, line);) {
			if (line.empty()) {
				continue;
			}
			asio::post(io_context, [buf = line, nickname]() mutable {
				std::cout << nickname << "(me): " << buf << std::endl;
				std::cout << std::flush;
			});
			std::string buf;
			if (line == "/room_list") {
				chat_proto::ServiceIM sim;
				sim.set_action(chat_proto::ServiceIM_Actions_room_list);
				sim.set_nick(nickname);
				buf = otus::chat_server::serialize_packet(chat_proto::Type_ServiceIM,
				                                          sim.SerializeAsString());
			} else if (line == "/user_list") {
				chat_proto::ServiceIM sim;
				sim.set_action(chat_proto::ServiceIM_Actions_user_list);
				sim.set_nick(nickname);
				buf = otus::chat_server::serialize_packet(chat_proto::Type_ServiceIM,
				                                          sim.SerializeAsString());
			} else if (line == "/join") {
				chat_proto::ServiceIM sim;
				sim.set_action(chat_proto::ServiceIM_Actions_join);
				sim.set_nick(nickname);
				buf = otus::chat_server::serialize_packet(chat_proto::Type_ServiceIM,
				                                          sim.SerializeAsString());
			} else if (line == "/leave") {
				chat_proto::ServiceIM sim;
				sim.set_action(chat_proto::ServiceIM_Actions_leave);
				sim.set_nick(nickname);
				buf = otus::chat_server::serialize_packet(chat_proto::Type_ServiceIM,
				                                          sim.SerializeAsString());
			} else {
				chat_proto::IM msg;
				msg.set_nick(nickname);
				msg.set_message(line);
				buf = otus::chat_server::serialize_packet(chat_proto::Type_IM,
				                                          msg.SerializeAsString());
			}
			asio::post(io_context, [buf = std::move(buf)]() mutable {
				asio::async_write(*sk, asio::buffer(buf), [](std::error_code ec, std::size_t) {
					if (ec) {
						std::cerr << "Error writing message: " << ec.message() << "\n";
					}
				});
			});
		}
		// Close the socket when done:
		// asio::post(io_context, []() { socket.close(); });
	} catch (const std::exception &e) {
		std::cerr << "Error in read_console_input: " << e.what() << "\n";
	}
}

/**
 * @brief Function to asynchronously read messages from the server.
 * @param socket The socket connected to the server.
 */
asio::awaitable<void> read_messages(asio::ip::tcp::socket &socket, const std::string &nickname) {
	try {
		for (std::string read_msg;;) {
			otus::chat_server::packet p;
			static const auto size_h = sizeof(otus::chat_server::header);
			std::size_t n = co_await asio::async_read(socket, asio::buffer(&p.header_, size_h),
			                                          asio::transfer_exactly(size_h));
			if (n != size_h) {
				//@TODO some error
				continue;
			}
			std::size_t n_h = asio::read(socket, asio::dynamic_buffer(p.data_, 2048),
			                             asio::transfer_exactly(p.header_.length_));
			if (p.header_.tag_ == chat_proto::Type_IM) {
				chat_proto::IM msg;
				msg.ParseFromString(p.data_);
				if (msg.nick() == nickname) {
					continue;
				}
				std::cout << msg.nick() << ": " << msg.message() << std::endl;
				std::cout << std::flush;
			}
			if (p.header_.tag_ == chat_proto::Type_Auth) {
				chat_proto::Auth msg;
				msg.ParseFromString(p.data_);
				if (msg.nick() != nickname) {
					continue;
				}
				std::cout << "Welcome to the room: Default, your ID: " << msg.id() << std::endl;
				std::cout << std::flush;
			}
			if (p.header_.tag_ == chat_proto::Type_ServiceIM) {
				chat_proto::ServiceIM msg;
				msg.ParseFromString(p.data_);
				if (msg.nick() != nickname) {
					continue;
				}
				switch (msg.action()) {
				case chat_proto::ServiceIM_Actions_room_list: {
					std::cout << "[server] /room_list: \n" << msg.data() << std::endl;
					std::cout << std::flush;
					break;
				}
				case chat_proto::ServiceIM_Actions_user_list: {
					std::cout << "[server] /user_list: \n" << msg.data() << std::endl;
					std::cout << std::flush;
					break;
				}
				default:
					break;
				}
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "Error in read_messages: " << e.what() << "\n";
	}
}

/**
 * @brief Function to asynchronously write messages to the server.
 * @param socket The socket connected to the server.
 * @param nickname The user's nickname.
 */
asio::awaitable<void> write_messages(asio::ip::tcp::socket &socket, const std::string &nickname) {
	try {

		// for (std::string line; std::getline(std::cin, line);) {
		// 	if (line.empty()) {
		// 		continue;
		// 	}

		// chat_proto::IM msg;
		// msg.set_nick(nickname);
		// msg.set_message(line);
		// auto buf =
		//     otus::chat_server::serialize_packet(chat_proto::Type_IM, msg.SerializeAsString());
		// co_await asio::async_write(socket, asio::buffer(buf), asio::use_awaitable);
		// }
		// socket.close();
		chat_proto::Auth msg;
		msg.set_nick(nickname);
		msg.set_token("super_secretny_token");
		auto buf =
		    otus::chat_server::serialize_packet(chat_proto::Type_Auth, msg.SerializeAsString());
		co_await asio::async_write(socket, asio::buffer(buf), asio::use_awaitable);

	} catch (const std::exception &e) {
		std::cerr << "Error in write_messages: " << e.what() << "\n";
	}
}

/**
 * @brief Main coroutine function for running the client.
 * @param nickname The user's nickname.
 * @param host Server's host IP.
 * @param port Server's host port.
 */
asio::awaitable<void> client(const std::string &nickname, const std::string &host,
                             const std::string &port) {
	asio::ip::tcp::resolver resolver(co_await asio::this_coro::executor);
	auto endpoints = co_await resolver.async_resolve(host, port, asio::use_awaitable);
	sk = std::make_shared<asio::ip::tcp::socket>(co_await asio::this_coro::executor);
	// asio::ip::tcp::socket socket(co_await asio::this_coro::executor);
	co_await asio::async_connect(*sk.get(), endpoints, asio::use_awaitable);

	std::cout << "Connected to chat server as " << nickname
	          << ". You can start typing messages...\n";

	asio::co_spawn((*sk.get()).get_executor(), read_messages(*sk.get(), nickname), asio::detached);
	// asio::co_spawn((*sk.get()).get_executor(), write_messages(*sk.get(), nickname),
	// asio::detached);
	co_await write_messages(*sk.get(), nickname);
}

int main(int argc, char *argv[]) {
	try {
		if (argc != 4) {
			std::cerr << "Usage: chat_client <nickname> <host> <port>\n";
			return 1;
		}

		asio::io_context io_context;

		std::string nickname = argv[1];
		std::string host = argv[2];
		std::string port = argv[3];

		asio::co_spawn(io_context, client(nickname, host, port), asio::detached);
		std::thread input_thread(
		    [&io_context, nickname]() { read_console_input(io_context, nickname); });
		io_context.run();
		input_thread.join();
	} catch (const std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}