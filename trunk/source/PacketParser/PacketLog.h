#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

class PacketLog {
public:
	PacketLog(const std::string& file);
	~PacketLog() = default;

	bool TransformPackets();

	std::string filename;
	uint32_t logVersion;
	std::unordered_map<uint16_t, std::vector<std::string> > packets;
	std::unordered_map<std::string, uint16_t> opcodeLookup;

private:
	void AddPacket(const std::ostringstream& ss, bool bServerPacket);
	uint32_t ReadLoginByNumRequest(const unsigned char* data, uint32_t size);
	uint32_t ReadLoginRequest(const unsigned char* data, uint32_t size);
	void SortClientCommands();
};