
#include "Toy_Shared.h"




PacketMgr::PacketMgr()
{

}

PacketMgr::~PacketMgr()
{
}
PacketMgr::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress) :
	mReceivedTime(inReceivedTime),
	mFromAddress(inFromAddress),
	mPacketBuffer(ioInputMemoryBitStream)
{
}

void PacketMgr::ProcessPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream)
{
	//remember we got a packet so we know not to disconnect for a bit
	inClientProxy->UpdateLastPacketTime();
	short size;
	uint32_t	packetType;

	inInputStream.Read(size);
	inInputStream.Read(packetType);
	switch (packetType)
	{
	case kHelloCC: // Ŭ���̾�Ʈ�� �ð�ٰ� �������� ���
		//need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
		//otherwise something weird is going on...
		SendWelcomePacket(inClientProxy); 
		printf("send WelcomePacekt");
		break;
	case kInputCC:
		//if (inClientProxy->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
		//{
		HandleInputPacket(inClientProxy, inInputStream);
		//	}
		break;
	default:

		LOG("Unknown packet type received from %s", inClientProxy->GetSocketAddress().ToString().c_str());
		break;
	}


}


InputMemoryBitStream*  PacketMgr::RecvPacketFromBuffer(SOCKETINFO& ptr, int retval, DWORD &transferred, int &thread_id)
{
	 ptr.ringbuffer->update_head(transferred);
	 short totalread = 0;


	 while (true) {

		 size_t remain = ptr.ringbuffer->remain();

		 if (HEADERSIZE > remain)  break; // ����� �о���� ���ߴٸ� ������� �ٽ� �о��.



		 short header = 0;
		 ptr.ringbuffer->Ref((char*)&header, HEADERSIZE);  // ����� �о��.
		 if (header >= HEADERMAX || header <= HEADERMIN) {//����� �����Ⱚ�� �Ѿ�� ��� (�Ǽ������� ó��)
			 printf("[TCP ����] Ŭ���̾�Ʈ : %s���� �Ǽ������͸� �޾ҽ��ϴ� %d \n", ptr.client->GetSocketAddress().ToString().c_str(), header);
		 }


		 

		 if (remain >= header) { // ����� ��ϵ� �͸�ŭ ����� �о� �� �� �ִ� ��� 

		
			 ptr.ringbuffer->Copy(mRecvBuffer[thread_id]+ totalread, header);//movememory
			 totalread += header;
			 printf("[TCP ����] Ŭ���̾�Ʈ : %s \n %d����Ʈ %d��° ��Ŷ�� �޾ҽ��ϴ�.\n",
				 ptr.client->GetSocketAddress().ToString().c_str(), header, mRecvBuffer[thread_id][6]);
		
			 continue;
			//����� ��ϵ� �͸�ŭ�� �����͸� �о�ͼ� ���� �� ������ ���� Ȥ�� ���� ����� �ִ��� üũ
		
		 }
		 else break;

	 }
	

		 if (totalread != 0) {
			
			 InputMemoryBitStream inputStream(mRecvBuffer[thread_id], totalread * 8);
		/*	 OutputMemoryBitStream ostream;
			 short header = 0;
			 uint32_t type;
			 char a = 0;

			 inputStream.Read(header);
			 inputStream.Read(type);
			 inputStream.Read(a);
			 ostream.Write(header);
			 ostream.Write(type);
			 ostream.Write(a);
			 SendPacketCP(ostream, ptr.client, nullptr);*/

			// printf("[TCP ����] Ŭ���̾�Ʈ : %s�� %d byte�� ���¾��\n", ptr.client->GetSocketAddress().ToString().c_str(), header);
			 ProcessPacket(ptr.client, inputStream);
			 return &inputStream;
		 }
		 else {
			 return nullptr;
		 }
}

bool PacketMgr::ProcessSentPacket(SOCKETINFO& ptr, int retval, DWORD &transferred, int &thread_id)
 {
	 ptr.ringbuffer->update_head(transferred);
	 size_t totalsent = 0;
	 
	 ptr.wsabuf.buf = ptr.wsabuf.buf + transferred;
	 ptr.wsabuf.len = ptr.wsabuf.len - transferred;
	 while (true) {

		 size_t remain = ptr.ringbuffer->remain();

		 if (HEADERSIZE > remain)  return false; // �����ŭ �������� ���ߴٸ� ������.

		 short header = 0;
		 ptr.ringbuffer->Ref((char*)&header, HEADERSIZE);  // ����� �о��.


		 if (remain >= header) { // �ϳ� �̻��� ��Ŷ�� ���ۿ� ������ ���


			 ptr.ringbuffer->update_tail( header);
			 totalsent += header;
			 printf("[TCP ����] Ŭ���̾�Ʈ : %s \n %d ����Ʈ ��Ŷ�� ���½��ϴ�.\n",
				 ptr.client->GetSocketAddress().ToString().c_str(), header);

			 continue;
		



		 }
		 else break; 	 //����� ��ϵ� �͸�ŭ �����͸� �������� ���ߴٸ� ������

	 }


	// ptr.client->GetTCPSocket.IOCPSend(ptr);
 }


 void PacketMgr::SendPacketCP(const OutputMemoryBitStream& inOutputStream, const ClientProxyPtr& client, SOCKETINFO * ptr) {

	 if (ptr == nullptr) {
		 ptr = 	new SOCKETINFO(client);
	 }

	 ptr->client = client;
	 ptr->SetIO_Operation(IO_OPERATION::Write);
	 ptr->wsabuf.buf = (CHAR*)inOutputStream.GetBufferPtr();
	 ptr->wsabuf.len = inOutputStream.GetByteLength();
	 client->GetTCPSocket()->IOCPSend(*ptr);
 
 }

 void PacketMgr::SendWelcomePacket(ClientProxyPtr inClientProxy)
 {
	 OutputMemoryBitStream welcomePacket;
	 short packetsize = 0;
	 welcomePacket.Write(packetsize);
	 welcomePacket.Write(kWelcomeCC);
	 welcomePacket.Write(inClientProxy->GetPlayerId());
	 welcomePacket.UpdateStreamHeader();

	 LOG("Server Welcoming, new client '%s' as player %d", inClientProxy->GetName().c_str(), inClientProxy->GetPlayerId());

	 SendPacketCP(welcomePacket, inClientProxy, nullptr);
 }

 void PacketMgr::HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream)
 {
	 uint32_t moveCount = 0;
	 Move move;
	 inInputStream.Read(moveCount, 2);

	 for (; moveCount > 0; --moveCount)
	 {
		 if (move.Read(inInputStream))
		 {
			 if (inClientProxy->GetUnprocessedMoveList().AddMoveIfNew(move))
			 {
				 inClientProxy->SetIsLastMoveTimestampDirty(true);
			 }
		 }
	 }
 }