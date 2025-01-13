#define ASIO_HAS_CO_AWAIT

#include <iostream>
#include <string>
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/detached.hpp>
#include <asio/co_spawn.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/read_until.hpp>

/**
 * @brief Function to asynchronously read messages from the server.
 * @param socket The socket connected to the server.
 */
asio::awaitable<void> read_messages(asio::ip::tcp::socket& socket) {
  try {
    for (std::string read_msg;;) {
      std::size_t n = co_await asio::async_read_until(socket, asio::dynamic_buffer(read_msg, 1024), "\n", asio::use_awaitable);
      std::string msg = read_msg.substr(0, n - 1);
      read_msg.erase(0, n);
      std::cout << read_msg;
      std::cout << std::flush;
    }
  }
  catch (const std::exception& e) {
    std::cerr << "Error in read_messages: " << e.what() << "\n";
  }
}

/**
 * @brief Function to asynchronously write messages to the server.
 * @param socket The socket connected to the server.
 * @param nickname The user's nickname.
 */
asio::awaitable<void> write_messages(asio::ip::tcp::socket& socket, const std::string& nickname) {
  try {
    for (std::string line; std::getline(std::cin, line);) {
      line = nickname + ": " + line + "\n";
      co_await asio::async_write(socket, asio::buffer(line), asio::use_awaitable);
    }
    socket.close();
  }
  catch (const std::exception& e) {
    std::cerr << "Error in write_messages: " << e.what() << "\n";
  }
}

/**
 * @brief Main coroutine function for running the client.
 * @param nickname The user's nickname.
 * @param host Server's host IP.
 * @param port Server's host port.
 */
asio::awaitable<void> client(const std::string& nickname, const std::string& host, const std::string& port) {
  asio::ip::tcp::resolver resolver(co_await asio::this_coro::executor);
  auto endpoints = co_await resolver.async_resolve(host, port, asio::use_awaitable);

  asio::ip::tcp::socket socket(co_await asio::this_coro::executor);
  co_await asio::async_connect(socket, endpoints, asio::use_awaitable);

  std::cout << "Connected to chat server as " << nickname << ". You can start typing messages...\n";

  asio::co_spawn(socket.get_executor(), read_messages(socket), asio::detached);
  co_await write_messages(socket, nickname);
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: chat_client <nickname> <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    std::string nickname = argv[1];
    std::string host = argv[2];
    std::string port = argv[3];

    asio::co_spawn(io_context,
             client(nickname, host, port),
             asio::detached);

    io_context.run();
  }
  catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}