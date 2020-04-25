#pragma once
class ClientMgr
{
private:

	CRITICAL_SECTION mCS;
	typedef unordered_map< int, ClientProxyPtr >	IntToClientMap;
	typedef unordered_map< SocketAddress, ClientProxyPtr >	AddressToClientMap;

	AddressToClientMap		mAddressToClientMap;
	IntToClientMap			mPlayerIdToClientMap;

	int mAliveClientsNum = 0;
	int	mNewClientId = 0; // client id
public:
	ClientMgr();
	~ClientMgr();
	void Reset();
	//void ChangeStatusAll(Client_State state);
	int GetClientNum() { return mAliveClientsNum; }
	ClientProxyPtr GetClientProxy(int inPlayerId);
	SOCKETINFO*  MakeClientSockInfo(TCPSocketPtr clientsock, SocketAddress & clientaddr);
	void AddClient(string& name, SOCKETINFO& info, const SocketAddress& inFromAddress);
	void DeleteClient(ClientProxyPtr inClientProxy);
	bool Empty() { return mAddressToClientMap.empty(); }
	ClientProxyPtr FindClientFromAddress(const SocketAddress& inFromAddress);
	static ClientMgr* Instance();
};