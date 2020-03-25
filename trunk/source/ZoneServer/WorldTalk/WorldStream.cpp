#include "stdafx.h"

#include "WorldStream.h"
#include "../../common/EmuPacket.h"
#include "../../common/util.h"
#include "../../common/Packets/EmuPackets/Emu_RegisterZoneServer_Packet.h"

#include "../ZoneServer/ZoneOperator.h"

extern ZoneOperator z;

WorldStream::WorldStream(unsigned int ip, unsigned int port) : EmuStream(ip, port) {
	authentication = Authentication::ENonAuth;
}

void WorldStream::Process() {
	auto incoming = PopIncoming();

	for (auto& itr : incoming) {
		LogDebug(LOG_PACKET, 0, "WorldStream::incoming dump");
		DumpBytes(itr->buffer, itr->Size);
		itr->HandlePacket(shared_from_this());
	}

	if (authentication == Authentication::ENonAuth) {
		SetAuthentication(Authentication::EAuthenticating);

		Emu_RegisterZoneServer_Packet* p = new Emu_RegisterZoneServer_Packet();	
		p->ip = z.GetHostString();
		p->port = z.GetPort();

		QueuePacket(p);
	}

	auto outgoing = PopOutgoing();

	for (auto& itr : outgoing) {
		WritePacket(Sock, itr->buffer, itr->Size);
	}
}