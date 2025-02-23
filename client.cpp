#define ASIO_HAS_CO_AWAIT

#include "src/chat_packet.hpp"
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/read_until.hpp>
#include <asio/use_awaitable.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include <memory>

std::vector<std::string> split_string_words(const std::string &input) {
	std::istringstream stream(input);
	std::vector<std::string> words;
	std::string word;

	while (stream >> word) {
		words.push_back(word);
	}

	return words;
}

std::shared_ptr<asio::ip::tcp::socket> sk;

void read_console_input(asio::io_context &io_context, const std::string &nickname,
                        const std::string &token) {
	try {
		for (std::string rline; std::getline(std::cin, rline);) {
			if (rline.empty()) {
				continue;
			}
			asio::post(io_context, [buf = rline, nickname]() mutable {
				std::cout << nickname << "(me): " << buf << std::endl;
				std::cout << std::flush;
			});
			std::string buf;
			std::vector<std::string> commands;
			if (rline[0] == '/') {
				commands = split_string_words(rline);
			} else {
				commands.push_back("");
			}
			if (commands[0] == "/room_list") {
				chat_proto::GetRoomList msg;
				buf = otus::chat_server::serialize_packet(chat_proto::Type_GetRoomList,
				                                          msg.SerializeAsString());
			} else if (commands[0] == "/user_list") {
				chat_proto::GetUserList msg;
				buf = otus::chat_server::serialize_packet(chat_proto::Type_GetUserList,
				                                          msg.SerializeAsString());
			} else if (commands[0] == "/join") {
				chat_proto::RoomJoin msg;
				msg.set_room_name(commands[1]);
				buf = otus::chat_server::serialize_packet(chat_proto::Type_RoomJoin,
				                                          msg.SerializeAsString());
			} else if (commands[0] == "/leave") {
				chat_proto::RoomLeft msg;
				buf = otus::chat_server::serialize_packet(chat_proto::Type_RoomLeft,
				                                          msg.SerializeAsString());
			} else if (commands[0] == "/whisper") {
				chat_proto::WhisperIM msg;
				auto from = msg.mutable_user_from();
				auto to = msg.mutable_user_to();
				from->set_nick(nickname);
				to->set_nick(commands[1]);
				auto sz = commands[0].size() + commands[1].size();
				msg.set_message(rline.substr(sz));
				buf = otus::chat_server::serialize_packet(chat_proto::Type_WhisperIM,
				                                          msg.SerializeAsString());
			} else if (commands[0] == "/help") {
				std::cout << "Supported command: \n"
				          << "- /room_list\n"
				          << "- /user_list\n"
				          << "- /join <room_name>\n"
				          << "- /leave\n"
				          << "- /whisper <nickname>\n";
			} else {
				chat_proto::IM msg;
				auto user = msg.mutable_user();
				user->set_nick(nickname);
				user->set_id("-1");
				msg.set_message(rline);
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
				if (msg.user().nick() == nickname) {
					continue;
				}
				std::cout << msg.user().nick() << ": " << msg.message() << std::endl;
				std::cout << std::flush;
			}
			if (p.header_.tag_ == chat_proto::Type_Auth) {
				chat_proto::Auth msg;
				msg.ParseFromString(p.data_);
				if (msg.user().nick() != nickname) {
					continue;
				}
				std::cout << "Welcome to the room: Default, your ID: " << msg.user().id()
				          << std::endl;
				std::cout << std::flush;
			}
			if (p.header_.tag_ == chat_proto::Type_ServerMaintenance) {
				chat_proto::ServerMaintenance msg;
				msg.ParseFromString(p.data_);
				std::cout << "[server] ServerMaintenance: " << msg.message() << std::endl;
				sk->close();
				continue;
			}
			if (p.header_.tag_ == chat_proto::Type_RoomList) {
				chat_proto::RoomList msg;
				msg.ParseFromString(p.data_);
				std::cout << "[server] /room_list:" << std::endl;
				for (int i = 0; i < msg.room_names_size(); ++i) {
					std::cout << " - " << msg.room_names().at(i) << std::endl;
				}
				continue;
			}
			if (p.header_.tag_ == chat_proto::Type_UserList) {
				chat_proto::UserList msg;
				msg.ParseFromString(p.data_);
				std::cout << "[server] /user_list:" << std::endl;
				for (int i = 0; i < msg.users_size(); ++i) {
					std::cout << " - " << msg.users().at(i).nick() << std::endl;
				}
				continue;
			}
			if (p.header_.tag_ == chat_proto::Type_RoomLeft) {
				chat_proto::RoomLeft msg;
				msg.ParseFromString(p.data_);
				std::cout << "[server] /leave, and join to default" << std::endl;
				continue;
			}
			if (p.header_.tag_ == chat_proto::Type_RoomJoin) {
				chat_proto::RoomJoin msg;
				msg.ParseFromString(p.data_);
				std::cout << "[server] /join: " << msg.room_name() << std::endl;
				continue;
			}
			if (p.header_.tag_ == chat_proto::Type_WhisperIM) {
				chat_proto::WhisperIM msg;
				msg.ParseFromString(p.data_);
				std::cout << "[Whisper] " << msg.user_from().nick() << ": " << msg.message()
				          << std::endl;
				continue;
			}
			if (p.header_.tag_ == chat_proto::Type_ServiceIM) {
				chat_proto::ServiceIM msg;
				msg.ParseFromString(p.data_);
				auto str = msg.error_code() == chat_proto::ServiceIM_error_codes_ec_auth_falied
				               ? "Auth failed!"
				               : "Unknown errod";
				std::cout << "[Service] " << str << ": " << msg.message() << std::endl;
				continue;
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
asio::awaitable<void> write_messages(asio::ip::tcp::socket &socket, const std::string &nickname,
                                     const std::string &token) {
	try {
		chat_proto::Auth msg;
		auto user = msg.mutable_user();
		user->set_nick(nickname);
		msg.set_token(token);
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
                             const std::string &port, const std::string &token) {
	asio::ip::tcp::resolver resolver(co_await asio::this_coro::executor);
	auto endpoints = co_await resolver.async_resolve(host, port, asio::use_awaitable);
	sk = std::make_shared<asio::ip::tcp::socket>(co_await asio::this_coro::executor);
	// asio::ip::tcp::socket socket(co_await asio::this_coro::executor);
	co_await asio::async_connect(*sk.get(), endpoints, asio::use_awaitable);

	std::cout << "Connected to chat server as " << nickname
	          << ". You can start typing messages...\n";

	asio::co_spawn((*sk.get()).get_executor(), read_messages(*sk.get(), nickname), asio::detached);
	co_await write_messages(*sk.get(), nickname, token);
}

int main(int argc, char *argv[]) {
	try {
		if (argc != 5) {
			std::cerr << "Usage: chat_client <nickname> <password> <host> <port>\n";
			return 1;
		}

		asio::io_context io_context;

		std::string nickname = argv[1];
		std::string token = argv[2];
		std::string host = argv[3];
		std::string port = argv[4];
		asio::co_spawn(io_context, client(nickname, host, port, token), asio::detached);
		std::thread input_thread(
		    [&io_context, nickname, token]() { read_console_input(io_context, nickname, token); });
		io_context.run();
		input_thread.join();
	} catch (const std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}