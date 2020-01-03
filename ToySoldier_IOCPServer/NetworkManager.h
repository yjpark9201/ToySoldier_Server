#pragma once

typedef unordered_map< int, GameObjectPtr > IntToGameObjectMap;
#define CPU_NUM 8

class NetworkManager
{
public:
	static const uint32_t	kHelloCC = 'HELO';
	static const uint32_t	kWelcomeCC = 'WLCM';
	static const uint32_t	kStateCC = 'STAT';
	static const uint32_t	kInputCC = 'INPT';
	static const int		kMaxPacketsPerFrameCount = 10;

    static HANDLE			HCP; // 입출력포트 핸들
	static HANDLE			CreateWorkerThreadEvent;// 워커쓰레드 생성이벤트

	NetworkManager();
	virtual ~NetworkManager();

	bool	Init(uint16_t inPort);
	void	ProcessIncomingPackets();

	bool	CreateWorkerThread();
	bool	CreateListenThread();

	virtual void	ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) = 0;
	virtual void	HandleConnectionReset(const SocketAddress& inFromAddress) { (void)inFromAddress; }

	void	SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);

	const WeightedTimedMovingAverage& GetBytesReceivedPerSecond()	const { return mBytesReceivedPerSecond; }
	const WeightedTimedMovingAverage& GetBytesSentPerSecond()		const { return mBytesSentPerSecond; }

	void	SetDropPacketChance(float inChance) { mDropPacketChance = inChance; }
	void	SetSimulatedLatency(float inLatency) { mSimulatedLatency = inLatency; }

	inline	GameObjectPtr	GetGameObject(int inNetworkId) const;
	void	AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject);
	void	RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject);

protected:

	IntToGameObjectMap		mNetworkIdToGameObjectMap;

private:

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

	void	UpdateBytesSentLastFrame();
	void	ReadIncomingPacketsIntoQueue();
	void	ProcessQueuedPackets();

	queue< ReceivedPacket, list< ReceivedPacket > >	mPacketQueue;

	TCPSocketPtr	mSocket;

	WeightedTimedMovingAverage	mBytesReceivedPerSecond;
	WeightedTimedMovingAverage	mBytesSentPerSecond;

	int							mBytesSentThisFrame;

	float						mDropPacketChance;
	float						mSimulatedLatency;


	HANDLE					mWorkerThreads[CPU_NUM * 2];
	HANDLE					mListenThread;


};



inline	GameObjectPtr NetworkManager::GetGameObject(int inNetworkId) const
{
	auto gameObjectIt = mNetworkIdToGameObjectMap.find(inNetworkId);
	if (gameObjectIt != mNetworkIdToGameObjectMap.end())
	{
		return gameObjectIt->second;
	}
	else
	{
		return GameObjectPtr();
	}
}
