#include "Toy_Shared.h"
ClientMgr::ClientMgr() : mNewClientId(0) 
{
	InitializeCriticalSection(&mCS);

}


ClientMgr::~ClientMgr()
{
	DeleteCriticalSection(&mCS);
}

void ClientMgr::Reset() { mNewClientId = 0; }

ClientMgr* ClientMgr::Instance()
{
	static ClientMgr instance;

	return &instance;
}

void ClientMgr::AddClient(string& name, SOCKETINFO& info, const SocketAddress& inFromAddress)
{
	EnterCriticalSection(&mCS);
	info.client->SetName(name);
	info.client->SetPlayerId(++mNewClientId);
	mAddressToClientMap[inFromAddress] = info.client;
	mPlayerIdToClientMap[info.client->GetPlayerId()] = info.client;
	LeaveCriticalSection(&mCS);

	printf("Client %s, ID : %d, Address : %s �߰�", info.client->GetName().c_str(),
		info.client->GetPlayerId(), info.client->GetSocketAddress().ToString().c_str());
}

void ClientMgr::DeleteClient(ClientProxyPtr inClientProxy) {
	EnterCriticalSection(&mCS);
	mPlayerIdToClientMap.erase(inClientProxy->GetPlayerId());
	mAddressToClientMap.erase(inClientProxy->GetSocketAddress());
	LeaveCriticalSection(&mCS);


	printf("Client %s, ID : %d, Address : %s ����", inClientProxy->GetName(), 
		inClientProxy->GetPlayerId(), inClientProxy->GetSocketAddress());

}

SOCKETINFO* ClientMgr::MakeClientSockInfo(TCPSocketPtr clientsock, SocketAddress & clientaddr) {

	ClientProxyPtr client = std::make_shared<ClientProxy>(clientsock, clientaddr, "", TEMPCLIENTID);
	return 	new SOCKETINFO(client);

	//�������� mNewNetworkId�� write�� �ϴ� ������� listen������ ���̸�, write�� �ϴµ��� Ÿ �����尡
	// read�� �ص� �������ۿ��� �ƹ� ���� ���⿡ criticalsection�� ���� ����ó���� ���� ���� ����.

}
//void ClientMgr::ChangeStatusAll(Client_State state)
//{
//
//	for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
//	{
//		//process any timed out packets while we're going throug hthe list
//		//it->second->GetDeliveryNotificationManager().ProcessTimedOutPackets();
//
//	//	it->mClientState = state;
//	}
//
//}





ClientProxyPtr ClientMgr::GetClientProxy(int inPlayerId)
{
	auto it = mPlayerIdToClientMap.find(inPlayerId);
	if (it != mPlayerIdToClientMap.end())
	{
		return it->second;
	}

	return nullptr;
}

ClientProxyPtr ClientMgr::FindClientFromAddress(const SocketAddress& inFromAddress) {
	auto it = mAddressToClientMap.find(inFromAddress);
	if (it != mAddressToClientMap.end())
	{
		return it->second;
	}

	return nullptr;	
}
