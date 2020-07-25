
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
	case PacketType::Hello: // Ŭ���̾�Ʈ�� �ð�ٰ� �������� ��� or UDP���� ���� ��Ŷ�� �ι� ���� ���
		//need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
		//otherwise something weird is going on...
		SendWelcomePacket(inClientProxy); 
		printf("send WelcomePacekt");
		break;
	case  PacketType::Input:
		//if (inClientProxy->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
		//{
		printf("�̵���Ŷ ����\n");
		HandleInputPacket(inClientProxy, inInputStream);
		//	}
		break;
	default:

		LOG("Unknown packet type received from %s", inClientProxy->GetSocketAddress().ToString().c_str());
		break;
	}


}


InputMemoryBitStream  PacketMgr::RecvPacketFromBuffer(SOCKETINFO& ptr, int retval, DWORD &transferred, int &thread_id)
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
		
			 return inputStream;
		 }
		 else {
			 InputMemoryBitStream inputStream(nullptr, 0);
			 return  inputStream;
		 }
}

bool PacketMgr::ProcessSentPacket(SOCKETINFO& ptr, int retval, DWORD &transferred, int &thread_id)
 {
	 ptr.ringbuffer->update_head(transferred);
	 size_t totalsent = 0;
	 if (transferred == 0) return false;
	
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

			 delete & ptr;

			 continue;
		



		 }
		 else break; 	 

	 }

	 if (totalsent == 0) {//����� ��ϵ� �͸�ŭ �����͸� �������� ���ߴٸ� ������

		 ptr.wsabuf.buf = ptr.ringbuffer->GetRecordablePoint();
		 ptr.wsabuf.len = ptr.ringbuffer->GetRecordableSize();
		 ptr.client->GetTCPSocket()->IOCPSend(ptr);
	 }
 }


 void PacketMgr::SendPacketCP(const OutputMemoryBitStream& inOutputStream, const ClientProxyPtr& client, SOCKETINFO * ptr) {

	 if (ptr == nullptr) {
		 ptr = 	new SOCKETINFO(client, (CHAR*)inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength());
	 }

	 ptr->client = client;
	 ptr->SetIO_Operation(IO_OPERATION::Write);
	 client->GetTCPSocket()->IOCPSend(*ptr);
 
 }

 void PacketMgr::SendWelcomePacket(ClientProxyPtr inClientProxy)
 {
	 OutputMemoryBitStream welcomePacket; // �����Ҵ�������� �Ƹ���
	 short header = 0;
	 welcomePacket.Write(header);
	 welcomePacket.Write(PacketType::Welcome); 
	 welcomePacket.Write(inClientProxy->GetPlayerId()); 
	 welcomePacket.UpdateStreamHeader();
	 LOG("Server Welcoming, new client '%s' as player %d", inClientProxy->GetName().c_str(), inClientProxy->GetPlayerId());

	 SendPacketCP(welcomePacket, inClientProxy, nullptr);
 }

 void PacketMgr::HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream)
 {
//	 uint32_t moveCount = 0;
	 Move move;
//	 inInputStream.Read(moveCount, 2);

	 //for (; moveCount > 0; --moveCount)
	 //{
		 if (move.Read(inInputStream))
		 {
			 if (inClientProxy->GetUnprocessedMoveList().AddMoveIfNew(move))
			 {
				 inClientProxy->SetIsLastMoveTimestampDirty(true);
			 }
		 }
//	 }
 }