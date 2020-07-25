#pragma once


class ClientProxy
{
public:

	ClientProxy(const TCPSocketPtr InSock ,const SocketAddress& inSocketAddress, const string& inName, int inPlayerId);

	const	SocketAddress&	GetSocketAddress()	const { return mSocketAddress; }
	int				GetPlayerId()		const { return mPlayerId; }
	void				SetPlayerId(const int & id) { mPlayerId = id; }
	const	string&		GetName()			const { return mName; }
	void				SetName(const string & name) {  mName = name; }
	const	TCPSocketPtr&	GetTCPSocket() { return mSock; }

	void			SetInputState(const InputState& inInputState) { mInputState = inInputState; }
	const	InputState&		GetInputState()		const { return mInputState; }

	void			UpdateLastPacketTime();
	float			GetLastPacketFromClientTime()	const { return mLastPacketFromClientTime; }

	//DeliveryNotificationManager&	GetDeliveryNotificationManager() { return mDeliveryNotificationManager; }
	ReplicationManagerServer&		GetReplicationManagerServer() { return mReplicationManagerServer; }

	const	MoveList&				GetUnprocessedMoveList() const { return mUnprocessedMoveList; }
	MoveList&				GetUnprocessedMoveList() { return mUnprocessedMoveList; }

	void	SetIsLastMoveTimestampDirty(bool inIsDirty) { mIsLastMoveTimestampDirty = inIsDirty; }
	bool	IsLastMoveTimestampDirty()						const { return mIsLastMoveTimestampDirty; }

	void	HandleCatDied();
	void	RespawnCatIfNecessary();

private:

	//DeliveryNotificationManager	mDeliveryNotificationManager;
	ReplicationManagerServer	mReplicationManagerServer;

	SocketAddress	mSocketAddress;
	string			mName;
	int				mPlayerId;

	//going away!
	InputState		mInputState;

	float			mLastPacketFromClientTime;
	float			mTimeToRespawn;

	MoveList		mUnprocessedMoveList;
	bool			mIsLastMoveTimestampDirty;
	TCPSocketPtr	mSock;

	OutputMemoryBitStream OutGoingPacket;


};

