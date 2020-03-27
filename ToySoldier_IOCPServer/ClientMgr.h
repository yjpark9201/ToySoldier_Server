#pragma once
class ClientMgr
{
private:
	typedef unordered_map< int, ClientProxyPtr >	IntToClientMap;
	typedef unordered_map< SocketAddress, ClientProxyPtr >	AddressToClientMap;

	AddressToClientMap		mAddressToClientMap;
	IntToClientMap			mPlayerIdToClientMap;

	int mAliveClientsNum = 0;

public:
	int mClientnum = 0;
	ClientMgr();
	~ClientMgr();
	//void ChangeStatusAll(Client_State state);
	int GetClientNum() { return mAliveClientsNum; }
	ClientProxyPtr GetClientProxy(int inPlayerId) const;
	void AddClient(string& name, SOCKETINFO& info, const SocketAddress& inFromAddress);
	static ClientMgr* Instance();
};