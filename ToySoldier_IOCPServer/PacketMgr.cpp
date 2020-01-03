
#include "Toy_Shared.h"




PacketMgr::PacketMgr()
{

}


PacketMgr::~PacketMgr()
{
}

 void PacketMgr::ProcessRecvPacket(SOCKETINFO& ptr, SOCKADDR_IN clientaddr,int retval)
{

	 printf("[TCP 서버] 클라이언트 : IP 주소=%s, 포트 번호=%d\n %d바이트 %d번째 패킷을 받았습니다.\n",
		 inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retval, ptr.buf[0]);
	//if (Packet_Type_Send::Bool_GameBegin == ptr.buf[1]) {

	//	TCP_SendIntPacket packet;
	//	memcpy(reinterpret_cast<BYTE*>(&packet), ptr.buf, *(ptr.buf));

	//
	//}

}