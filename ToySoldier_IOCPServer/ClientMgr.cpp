#include "Toy_Shared.h"
ClientMgr::ClientMgr() : mNewClientId(1) 
{

}


ClientMgr::~ClientMgr()
{
}

void ClientMgr::Reset() { mNewClientId = 1; }

ClientMgr* ClientMgr::Instance()
{
	static ClientMgr instance;

	return &instance;
}

void ClientMgr::AddClient(string& name, SOCKETINFO& info, const SocketAddress& inFromAddress)
{
	info.client->SetName(name);
	info.client->SetPlayerId(++mNewClientId);
	mAddressToClientMap[inFromAddress] = info.client;
	mPlayerIdToClientMap[info.client->GetPlayerId()] = info.client;
}

void ClientMgr::DeleteClient(ClientProxyPtr inClientProxy) {
	mPlayerIdToClientMap.erase(inClientProxy->GetPlayerId());
	mAddressToClientMap.erase(inClientProxy->GetSocketAddress());
}

SOCKETINFO* ClientMgr::MakeClientSockInfo(TCPSocketPtr clientsock, SocketAddress & clientaddr) {

	ClientProxyPtr client = std::make_shared<ClientProxy>(clientsock, clientaddr, "", mNewClientId++);
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


ClientProxyPtr ClientMgr::GetClientProxy(int inPlayerId) const
{
	auto it = mPlayerIdToClientMap.find(inPlayerId);
	if (it != mPlayerIdToClientMap.end())
	{
		return it->second;
	}

	return nullptr;
}

