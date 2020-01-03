#pragma once
class PacketMgr
{
public:
	PacketMgr();
	~PacketMgr();

	static void ProcessRecvPacket(SOCKETINFO& ptr, SOCKADDR_IN clientaddr, int retval);
};

