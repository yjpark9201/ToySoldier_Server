#include "Toy_Shared.h"


NetworkManagerServer*	NetworkManagerServer::sInstance;


NetworkManagerServer::NetworkManagerServer() :
	mNewNetworkId(1),
	mTimeBetweenStatePackets(0.033f),
	mClientDisconnectTimeout(3.f)
{
}

bool NetworkManagerServer::StaticInit(uint16_t inPort)
{
	
	sInstance = new NetworkManagerServer();
	return sInstance->Init(inPort);
}

void NetworkManagerServer::HandleConnectionReset(const SocketAddress& inFromAddress)
{
	//just dc the client right away...
	ClientProxyPtr client = Client_Mgr->FindClientFromAddress(inFromAddress);
	if (client == nullptr)
	{
		HandleClientDisconnected(client);
	}
}

void NetworkManagerServer::ProcessPacket(InputMemoryBitStream& inInputStream,
			SOCKETINFO& ptr, DWORD & cbTransferred, const SocketAddress& inFromAddress)
{
	//try to get the client proxy for this address
	//pass this to the client proxy to process
	ClientProxyPtr client = Client_Mgr->FindClientFromAddress(inFromAddress);

	if (client == nullptr)
	{
		//didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
		HandlePacketFromNewClient(ptr, inInputStream, inFromAddress);
	}
	else
	{
		mPacketMgr.ProcessPacket(client, inInputStream);
	}
}






void NetworkManagerServer::HandlePacketFromNewClient(SOCKETINFO& info, InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	//read the beginning- is it a hello?
	short size;
	uint32_t	packetType;

	inInputStream.Read(size);
	inInputStream.Read(packetType);
	if (packetType == PacketMgr::kHelloCC)
	{
		//read the name
		string name;
		inInputStream.Read(name);

		Client_Mgr->AddClient(name, info, inFromAddress);

		//tell the server about this client, spawn a cat, etc...
		//if we had a generic message system, this would be a good use for it...
		//instead we'll just tell the server directly
		static_cast<Server*> (Engine::sInstance.get())->HandleNewClient(info.client);

		//and welcome the client...
		mPacketMgr.SendWelcomePacket(info.client);

		//and now init the replication manager with everything we know about!
		for (const auto& pair : mNetworkIdToGameObjectMap)
		{
			info.client->GetReplicationManagerServer().ReplicateCreate(pair.first, pair.second->GetAllStateMask());
		}
	}
	else
	{
		//bad incoming packet from unknown client- we're under attack!!
		LOG("Bad incoming packet from unknown client at socket %s", inFromAddress.ToString().c_str());
	}
}


void NetworkManagerServer::RespawnCats()
{
	/*for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
	{
		ClientProxyPtr clientProxy = it->second;

		clientProxy->RespawnCatIfNecessary();
	}*/
}

void NetworkManagerServer::SendOutgoingPackets()
{
	float time = Timing::sInstance.GetTimef();

	//let's send a client a state packet whenever their move has come in...
	//for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
	//{
	//	ClientProxyPtr clientProxy = it->second;
	//	//process any timed out packets while we're going through the list
	//	//clientProxy->GetDeliveryNotificationManager().ProcessTimedOutPackets();

	//	if (clientProxy->IsLastMoveTimestampDirty())
	//	{
	//		SendStatePacketToClient(clientProxy);
	//	}
	//}
}

void NetworkManagerServer::UpdateAllClients()
{
	//for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
	//{
	//	//process any timed out packets while we're going through the list
	//	//it->second->GetDeliveryNotificationManager().ProcessTimedOutPackets();

	//	SendStatePacketToClient(it->second);
	//}
}

void NetworkManagerServer::SendStatePacketToClient(ClientProxyPtr inClientProxy)
{
	//build state packet
	OutputMemoryBitStream	statePacket;

	//it's state!
	statePacket.Write(PacketMgr::kStateCC);

	//InFlightPacket* ifp = inClientProxy->GetDeliveryNotificationManager().WriteState(statePacket);

	WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);

	AddScoreBoardStateToPacket(statePacket);

	ReplicationManagerTransmissionData* rmtd = new ReplicationManagerTransmissionData(&inClientProxy->GetReplicationManagerServer());
	inClientProxy->GetReplicationManagerServer().Write(statePacket, rmtd);
	//ifp->SetTransmissionData('RPLM', TransmissionDataPtr(rmtd));

	mPacketMgr.SendPacketCP(statePacket, inClientProxy,nullptr);

}


void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxyPtr inClientProxy)
{
	//first, dirty?
	bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
	inOutputStream.Write(isTimestampDirty);
	if (isTimestampDirty)
	{
		inOutputStream.Write(inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
		inClientProxy->SetIsLastMoveTimestampDirty(false);
	}
}

//should we ask the server for this? or run through the world ourselves?
void NetworkManagerServer::AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream)
{
	const auto& gameObjects = World::sInstance->GetGameObjects();

	//now start writing objects- do we need to remember how many there are? we can check first...
	inOutputStream.Write(gameObjects.size());

	for (GameObjectPtr gameObject : gameObjects)
	{
		inOutputStream.Write(gameObject->GetNetworkId());
		inOutputStream.Write(gameObject->GetClassId());
		gameObject->Write(inOutputStream, 0xffffffff);
	}
}

void NetworkManagerServer::AddScoreBoardStateToPacket(OutputMemoryBitStream& inOutputStream)
{
	ScoreBoardManager::sInstance->Write(inOutputStream);
}


int NetworkManagerServer::GetNewNetworkId()
{
	int toRet = mNewNetworkId++;
	if (mNewNetworkId < toRet)
	{
		LOG("Network ID Wrap Around!!! You've been playing way too long...", 0);
	}

	return toRet;

}





bool NetworkManagerServer::CheckForDisconnects(SOCKETINFO& ptr, DWORD &retval, DWORD & cbTransferred)
{

	if (retval == 0 || cbTransferred == 0) {
		if (retval == 0) {
			ptr.client->GetTCPSocket()->WSAGetOverlapptedResult(&ptr.overlapped);
		}
		HandleClientDisconnected(ptr.client);
		delete &ptr; 
		return true;
	}

	return false;

}

void NetworkManagerServer::HandleClientDisconnected(ClientProxyPtr inClientProxy)
{
	Client_Mgr->DeleteClient(inClientProxy);
	static_cast<Server*> (Engine::sInstance.get())->HandleLostClient(inClientProxy);

	//was that the last client? if so, bye!
	if (Client_Mgr->Empty())
	{
	//	Engine::sInstance->SetShouldKeepRunning(false);
	}

	printf("[TCP 서버] 클라이언트 종료: %s \n",
		inClientProxy->GetSocketAddress().ToString().c_str());
}

void NetworkManagerServer::RegisterGameObject(GameObjectPtr inGameObject)
{
	//assign network id
	int newNetworkId = GetNewNetworkId();
	inGameObject->SetNetworkId(newNetworkId);

	//add mapping from network id to game object
	mNetworkIdToGameObjectMap[newNetworkId] = inGameObject;

	//tell all client proxies this is new...
	//for (const auto& pair : mAddressToClientMap)
	//{
	//	pair.second->GetReplicationManagerServer().ReplicateCreate(newNetworkId, inGameObject->GetAllStateMask());
	//}
}


void NetworkManagerServer::UnregisterGameObject(GameObject* inGameObject)
{
	int networkId = inGameObject->GetNetworkId();
	mNetworkIdToGameObjectMap.erase(networkId);

	//tell all client proxies to STOP replicating!
	//tell all client proxies this is new...
	//for (const auto& pair : mAddressToClientMap)
	//{
	//	pair.second->GetReplicationManagerServer().ReplicateDestroy(networkId);
	//}
}

void NetworkManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
	//tell everybody this is dirty
	//for (const auto& pair : mAddressToClientMap)
	//{
	//	pair.second->GetReplicationManagerServer().SetStateDirty(inNetworkId, inDirtyState);
	//}
}

