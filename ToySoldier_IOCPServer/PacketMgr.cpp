
#include "Toy_Shared.h"




PacketMgr::PacketMgr()
{

}


PacketMgr::~PacketMgr()
{
}

 void PacketMgr::ProcessRecvPacket(SOCKETINFO& ptr, SOCKADDR_IN clientaddr,int retval)
{

	 printf("[TCP ����] Ŭ���̾�Ʈ : IP �ּ�=%s, ��Ʈ ��ȣ=%d\n %d����Ʈ %d��° ��Ŷ�� �޾ҽ��ϴ�.\n",
		 inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), retval, ptr.buf[0]);
	//if (Packet_Type_Send::Bool_GameBegin == ptr.buf[1]) {

	//	TCP_SendIntPacket packet;
	//	memcpy(reinterpret_cast<BYTE*>(&packet), ptr.buf, *(ptr.buf));

	//
	//}

}