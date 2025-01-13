/**
 *  OTUS Graduate Work
 *  Chat session
 *  (c) 2025
 */

#include "chat_session.hpp"

namespace otus::chat_server {
  chat_session::chat_session(asio::ip::tcp::socket socket, chat_room& room, const user_info& user_info)
    : socket_(std::move(socket)),
      timer_(socket_.get_executor()),
      room_(room),
      user_info_(user_info)
  {
    timer_.expires_at(std::chrono::steady_clock::time_point::max());
  }

  void chat_session::start() {
    room_.join(shared_from_this(), user_info_);

    asio::co_spawn(socket_.get_executor(),
        [self = shared_from_this()]{ return self->reader(); },
        asio::detached);

    asio::co_spawn(socket_.get_executor(),
        [self = shared_from_this()]{ return self->writer(); },
        asio::detached);

    std::cout << "Session started for user_info: " << user_info_.nickname << "\n";
  }

  void chat_session::deliver(const std::string& msg) {
    write_msgs_.push_back(msg);
    timer_.cancel_one();
  }

  asio::awaitable<void> chat_session::reader() {
    try {
      for (std::string read_msg;;) {
        std::size_t n = co_await asio::async_read_until(socket_,
            asio::dynamic_buffer(read_msg, 1024), "\n", asio::use_awaitable);

        std::string msg = read_msg.substr(0, n - 1);
        read_msg.erase(0, n);
       
        std::cout << "RAW message: " << msg << "\n";
        std::cout << "RAW message: " << msg.size() << "\n";
        // Command processing

        if (msg == "/list_rooms") {
          handle_list_rooms();
        }
        else if (msg == "/list_user_infos") {
          handle_list_user_infos();
        }
        else {
          room_.deliver(user_info_.nickname + ": " + msg);
        }
      }
    }
    catch (std::exception&) {
      stop();
    }
  }

  asio::awaitable<void> chat_session::writer() {
    try {
      while (socket_.is_open()) {
        if (write_msgs_.empty()) {
          asio::error_code ec;
          co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
        }
        else {
          co_await asio::async_write(socket_,
              asio::buffer(write_msgs_.front()), asio::use_awaitable);
          write_msgs_.pop_front();
        }
      }
    }
    catch (std::exception&) {
      stop();
    }
  }

  void chat_session::handle_list_rooms() {
    std::string response = "Available rooms: ";
    for (const auto& [room_name, _] : chat_rooms) {
      response += room_name + ", ";
    }
    deliver(response + "\n");
  }

  void chat_session::handle_list_user_infos() {
    auto user_infos = room_.get_users_online();
    std::string response = "user_infos online: ";
    for (const auto& user_info : user_infos) {
      response += user_info + ", ";
    }
    deliver(response + "\n");
  }

  void chat_session::stop() {
    room_.leave(shared_from_this(), user_info_);
    socket_.close();
    timer_.cancel();
    
    std::cout << "Session stopped for user_info: " << user_info_.nickname << "\n";
  }
} // namespace otus::chat_server