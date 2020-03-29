
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
	case kHelloCC: // 클라이언트가 팅겼다가 재접속한 경우
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

		 if (HEADERSIZE > remain)  break; // 헤더를 읽어오지 못했다면 헤더부터 다시 읽어옴.



		 short header = 0;
		 ptr.ringbuffer->Ref((char*)&header, HEADERSIZE);  // 헤더만 읽어옴.
		 if (header >= HEADERMAX || header <= HEADERMIN) {//헤더에 쓰레기값이 넘어올 경우 (악성데이터 처리)
			 printf("[TCP 서버] 클라이언트 : %s에게 악성데이터를 받았습니다 %d \n", ptr.client->GetSocketAddress().ToString().c_str(), header);
		 }


		 

		 if (remain >= header) { // 헤더에 기록된 것만큼 충분히 읽어 올 수 있는 경우 

		
			 ptr.ringbuffer->Copy(mRecvBuffer[thread_id]+ totalread, header);//movememory
			 totalread += header;
			 printf("[TCP 서버] 클라이언트 : %s \n %d바이트 %d번째 패킷을 받았습니다.\n",
				 ptr.client->GetSocketAddress().ToString().c_str(), header, mRecvBuffer[thread_id][6]);
		
			 continue;
			//헤더에 기록된 것만큼만 데이터를 읽어와서 저장 후 루프를 돌며 혹시 다음 헤더가 있는지 체크
		
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

			// printf("[TCP 서버] 클라이언트 : %s에 %d byte를 보냈어요\n", ptr.client->GetSocketAddress().ToString().c_str(), header);
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

		 if (HEADERSIZE > remain)  return false; // 헤더만큼 전송하지 못했다면 재전송.

		 short header = 0;
		 ptr.ringbuffer->Ref((char*)&header, HEADERSIZE);  // 헤더만 읽어옴.


		 if (remain >= header) { // 하나 이상의 패킷을 전송에 성공한 경우


			 ptr.ringbuffer->update_tail( header);
			 totalsent += header;
			 printf("[TCP 서버] 클라이언트 : %s \n %d 바이트 패킷을 보냈습니다.\n",
				 ptr.client->GetSocketAddress().ToString().c_str(), header);

			 continue;
		



		 }
		 else break; 	 //헤더에 기록된 것만큼 데이터를 전송하지 못했다면 재전송

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