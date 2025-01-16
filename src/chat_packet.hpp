/**
 *  OTUS Graduate Work
 *  Network packet
 *  (c) 2025
 */
#pragma once

#include "chat_messages.pb.h"

#include <unordered_map>

namespace otus::chat_server {

/**
 * @brief
 */
struct header {
	uint32_t tag_;
	uint32_t length_;
};

/**
 * @brief
 */
struct packet {
	header header_;
	std::string data_;
};

/**
 * @brief
 */
std::string serialize_packet(uint32_t tag, std::string data);

} // namespace otus::chat_server
