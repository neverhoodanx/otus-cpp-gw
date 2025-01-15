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

typedef void (*call_parser)(const std::string &);
void pars_message(uint32_t tag, std::string &data) {
	static const std::unordered_map<uint32_t, call_parser> map_hendler({
	    {chat_proto::Type_Auth, &pars_message<chat_proto::Auth>},
	    {chat_proto::Type_IM, &pars_message<chat_proto::IM>},
	    {chat_proto::Type_ServiceIM, &pars_message<chat_proto::ServiceIM>},
	});
	auto it = map_hendler.find(tag);
	if (it != map_hendler.end()) {
		(*it->second)(data);
	} else {
		std::cout << "There is no handler for:" << tag << std::endl;
	}
}

} // namespace otus::chat_server
