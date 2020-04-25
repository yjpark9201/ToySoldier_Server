#pragma once

#define Packet_Size 1500
class PacketMgr
{
public:

	static const uint32_t	kHelloCC = 'HELO';
	static const uint32_t	kWelcomeCC = 'WLCM';
	static const uint32_t	kStateCC = 'STAT';
	static const uint32_t	kInputCC = 'INPT';




	PacketMgr();
	~PacketMgr();


	void SendPacketCP(const OutputMemoryBitStream& inOutputStream, const ClientProxyPtr& client, SOCKETINFO * ptr);

	InputMemoryBitStream RecvPacketFromBuffer(SOCKETINFO& ptr, int retval, DWORD &transferred, int &thread_id);
	bool ProcessSentPacket(SOCKETINFO& ptr, int retval, DWORD &transferred, int &thread_id);

	 void	ProcessPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream);
	void	SendWelcomePacket(ClientProxyPtr inClientProxy);
	private : 

		class ReceivedPacket
		{
		public:
			ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);

			const	SocketAddress&			GetFromAddress()	const { return mFromAddress; }
			float					GetReceivedTime()	const { return mReceivedTime; }
			InputMemoryBitStream&	GetPacketBuffer() { return mPacketBuffer; }

		private:

			float					mReceivedTime;
			InputMemoryBitStream	mPacketBuffer;
			SocketAddress			mFromAddress;

		};
		queue< ReceivedPacket, list< ReceivedPacket > >	mPacketQueuePerThread;


	char mRecvBuffer[CPU_NUM * 2][Packet_Size];
	char mSendrBuffe[CPU_NUM * 2][Packet_Size];

	void	HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream);
};

