#include "Toy_Shared.h"
ClientMgr::ClientMgr()
{

}


ClientMgr::~ClientMgr()
{
}

ClientMgr* ClientMgr::Instance()
{
	static ClientMgr instance;

	return &instance;
}

void ClientMgr::AddClient(string& name, SOCKETINFO& info, const SocketAddress& inFromAddress)
{
	info.client->SetName(name);
	mAddressToClientMap[inFromAddress] = info.client;
	mPlayerIdToClientMap[info.client->GetPlayerId()] = info.client;
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

