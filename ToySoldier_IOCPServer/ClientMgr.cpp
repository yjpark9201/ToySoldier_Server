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

	//공유변수 mNewNetworkId에 write를 하는 스레드는 listen스레드 뿐이며, write를 하는도중 타 스레드가
	// read를 해도 서버동작에는 아무 문제 없기에 criticalsection에 대한 별도처리는 따로 하지 않음.

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

