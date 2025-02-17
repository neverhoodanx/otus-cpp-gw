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
 * @struct header
 * @brief Represents the header of a network packet
 *
 * The header contains a tag and the length of the packet data
 */
struct header {
	uint32_t tag_;    ///< Tag identifying the packet type
	uint32_t length_; ///< Length of the data in the packet
};

/**
 * @struct packet
 * @brief Represents a complete network packet.
 *
 * A packet consists of a header and a data payload
 */
struct packet {
	header header_;    ///< Packet header containing metadata
	std::string data_; ///< Packet data payload
};

/**
 * @brief Serializes a packet into a string format
 *
 * This function constructs a packet with the given tag and data, then serializes it
 * into a string buffer for transmission
 *
 * @param tag The tag identifying the packet type
 * @param data The payload data
 * @return std::string The serialized packet data
 */
std::string serialize_packet(uint32_t tag, std::string data);

} // namespace otus::chat_server
