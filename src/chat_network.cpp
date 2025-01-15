// /**
//  *  OTUS Graduate Work
//  *  Chat network session
//  *  (c) 2025
//  */

// #include "chat_network.hpp"
// #include <asio.hpp>

// namespace otus::chat_server {
// chat_network::chat_network(asio::ip::tcp::socket socket)
//     : socket_(std::move(socket)), timer_(socket_.get_executor()), timer2_(socket_.get_executor())
//     {
// 	timer_.expires_at(std::chrono::steady_clock::time_point::max());
// 	timer2_.expires_at(std::chrono::steady_clock::time_point::max());
// }

// void chat_network::start() {
// 	asio::co_spawn(
// 	    socket_.get_executor(), [self = shared_from_this()] { return self->reader(); },
// 	    asio::detached);

// 	asio::co_spawn(
// 	    socket_.get_executor(), [self = shared_from_this()] { return self->writer(); },
// 	    asio::detached);

// 	asio::co_spawn(
// 	    socket_.get_executor(), [self = shared_from_this()] { return self->process_messages(); },
// 	    asio::detached);

// 	std::cout << "Network session started" << std::endl;
// }

// void chat_network::on_packet(uint32_t tag, const std::string &data) {
// 	std::cout << "on_packet: " << tag << "; size: " << data.size() << std::endl;
// }

// void chat_network::on_stoped() {
// }

// void chat_network::deliver(const std::string &msg) {
// 	write_msgs_.push_back(msg);
// 	timer_.cancel_one();
// }

// asio::awaitable<void> chat_network::reader() {
// 	try {
// 		for (std::string read_msg;;) {
// 			packet p;
// 			static const auto size_h = sizeof(header);
// 			std::size_t n = co_await asio::async_read(socket_, asio::buffer(&p.header_, size_h),
// 			                                          asio::transfer_exactly(size_h));
// 			if (n != size_h) {
// 				//@TODO some error
// 				continue;
// 			}
// 			std::size_t n_h =
// 			    co_await asio::async_read(socket_, asio::dynamic_buffer(p.data_, 2048),
// 			                              asio::transfer_exactly(p.header_.length_));
// 			std::cout << "RAW message size: " << n_h << std::endl;
// 			auto b = messages_.empty();
// 			messages_.push_back(p);
// 			if (b) {
// 				timer2_.cancel_one();
// 			}
// 		}
// 	} catch (std::exception &) {
// 		stop();
// 	}
// }

// asio::awaitable<void> chat_network::writer() {
// 	try {
// 		while (socket_.is_open()) {
// 			if (write_msgs_.empty()) {
// 				asio::error_code ec;
// 				co_await timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
// 			} else {
// 				co_await asio::async_write(socket_, asio::buffer(write_msgs_.front()),
// 				                           asio::use_awaitable);
// 				write_msgs_.pop_front();
// 			}
// 		}
// 	} catch (std::exception &) {
// 		stop();
// 	}
// }

// asio::awaitable<void> chat_network::process_messages() {
// 	try {
// 		while (socket_.is_open()) {
// 			if (messages_.empty()) {
// 				asio::error_code ec;
// 				co_await timer2_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
// 			} else {
// 				auto pkg = messages_.front();
// 				pars_message(pkg.header_.tag_, pkg.data_);
// 				messages_.pop_front();
// 			}
// 		}
// 	} catch (std::exception &) {
// 		stop();
// 	}
// }

// void chat_network::stop() {
// 	socket_.close();
// 	timer_.cancel();
// 	timer2_.cancel();
// 	on_stoped();
// 	std::cout << "Session stopped" << std::endl;
// }
// } // namespace otus::chat_server