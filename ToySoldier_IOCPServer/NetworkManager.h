#pragma once

typedef unordered_map< int, GameObjectPtr > IntToGameObjectMap;


class NetworkManager
{
public:

	static const int		kMaxPacketsPerFrameCount = 10;

    static HANDLE			HCP; // 입출력포트 핸들
	static HANDLE			CreateWorkerThreadEvent;// 워커쓰레드 생성이벤트

	NetworkManager();
	virtual ~NetworkManager();

	bool	Init(uint16_t inPort);
	void	ProcessIncomingPackets(SOCKETINFO& ptr, DWORD & cbTransferred);

	bool	CreateWorkerThread();
	bool	CreateListenThread();

	virtual void	ProcessPacket(InputMemoryBitStream& inInputStream, SOCKETINFO& ptr, DWORD & cbTransferred, const SocketAddress& inFromAddress) = 0;
	virtual void	HandleConnectionReset(const SocketAddress& inFromAddress) { (void)inFromAddress; }

	void	SentByteCount(int sentbyte);

	const WeightedTimedMovingAverage& GetBytesReceivedPerSecond()	const { return mBytesReceivedPerSecond; }
	const WeightedTimedMovingAverage& GetBytesSentPerSecond()		const { return mBytesSentPerSecond; }

	void	SetDropPacketChance(float inChance) { mDropPacketChance = inChance; }
	void	SetSimulatedLatency(float inLatency) { mSimulatedLatency = inLatency; }

	inline	GameObjectPtr	GetGameObject(int inNetworkId) const;
	void	AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject);
	void	RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject);

	void	ReadIncomingPackets(SOCKETINFO& ptr, DWORD &retval, DWORD &transferred, int &thread_id);

protected:

	IntToGameObjectMap		mNetworkIdToGameObjectMap;
	PacketMgr		mPacketMgr;

private:


	void	UpdateBytesSentLastFrame();

	void	ProcessQueuedPackets(SOCKETINFO& ptr, DWORD & cbTransferred);



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
