/**
 *  OTUS Graduate Work
 *  Network packet
 *  (c) 2025
 */

#include "chat_packet.hpp"

#include <unordered_map>

namespace otus::chat_server {

std::string serialize_packet(uint32_t tag, std::string data) {
	static const uint32_t header_size = 8; // @TODO need sizeof header
	uint32_t packet_size = data.size() + header_size;
	uint32_t data_size = data.size();
	std::string buf;
	buf.resize(packet_size);
	std::memcpy(buf.data(), &tag, sizeof(tag));
	std::memcpy(buf.data() + 4, &data_size, sizeof(data_size));
	std::memcpy(buf.data() + header_size, data.data(), data.size());
	return buf;
}

} // namespace otus::chat_server
