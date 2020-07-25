#include "Toy_Shared.h"




//uncomment this when you begin working on the server

bool Server::StaticInit()
{
	printf("서버 재기동\n");
	sInstance.reset(new Server());
	return true;
}

Server::Server()
{
	printf("서버 초기화\n");
	GameObjectRegistry::sInstance->RegisterCreationFunction('TSOL', ToySoldier::StaticCreate);
	//GameObjectRegistry::sInstance->RegisterCreationFunction('MOUS', MouseServer::StaticCreate);
	//GameObjectRegistry::sInstance->RegisterCreationFunction('YARN', YarnServer::StaticCreate);

	InitNetworkManager();

	//NetworkManagerServer::sInstance->SetDropPacketChance( 0.8f );
	//NetworkManagerServer::sInstance->SetSimulatedLatency( 0.25f );
	//NetworkManagerServer::sInstance->SetSimulatedLatency( 0.5f );
	//NetworkManagerServer::sInstance->SetSimulatedLatency( 0.1f );

}


int Server::Run()
{
	//SetupWorld();
	return Engine::Run();

	//return 0;
}

bool Server::InitNetworkManager()
{
	//string portString = StringUtils::GetCommandLineArg(1);
	//uint16_t port = stoi(portString);
	uint16_t port = SERVERPORT;
	return NetworkManagerServer::StaticInit(port);
}


namespace
{

	void CreateRandomMice(int inMouseCount)
	{
		Vector3 mouseMin(-5.f, -3.f, 0.f);
		Vector3 mouseMax(5.f, 3.f, 0.f);
		GameObjectPtr go;

		//make a mouse somewhere- where will these come from?
		for (int i = 0; i < inMouseCount; ++i)
		{
			go = GameObjectRegistry::sInstance->CreateGameObject('MOUS');
			Vector3 mouseLocation = ToyMath::GetRandomVector(mouseMin, mouseMax);
			go->SetLocation(mouseLocation);
		}
	}


}


void Server::SetupWorld()
{
	//spawn some random mice
	//CreateRandomMice(10);

	//spawn more random mice!
	//CreateRandomMice(10);
}

void Server::DoFrame()
{
	//NetworkManagerServer::sInstance->ProcessIncomingPackets();

	//NetworkManagerServer::sInstance->CheckForDisconnects();

	NetworkManagerServer::sInstance->RespawnCats();

	Engine::DoFrame();

	NetworkManagerServer::sInstance->SendOutgoingPackets();

}

void Server::HandleNewClient(ClientProxyPtr inClientProxy)
{

	int clientid = inClientProxy->GetPlayerId();

	//ScoreBoardManager::sInstance->AddEntry(playerId, inClientProxy->GetName());
	SpawnUnitForPlayer(clientid);
}

void Server::SpawnUnitForPlayer(int inPlayerId)
{
	ToySoldierPtr sol = std::static_pointer_cast<ToySoldier>(GameObjectRegistry::sInstance->CreateGameObject('TSOL'));
	//sol->SetColor(ScoreBoardManager::sInstance->GetEntry(inPlayerId)->GetColor());
	sol->SetPlayerId(inPlayerId);
	//gotta pick a better spawn location than this...
	sol->SetLocation(Vector3(1.f - static_cast<float>(inPlayerId), 0.f, 0.f));

	printf("플레이어 %d의 장난감병사 생성 \n", inPlayerId);

}

void Server::HandleLostClient(ClientProxyPtr inClientProxy)
{
	//kill client's cat
	//remove client from scoreboard
	int playerId = inClientProxy->GetPlayerId();

	ScoreBoardManager::sInstance->RemoveEntry(playerId);
	//RoboCatPtr cat = GetCatForPlayer(playerId);
	//if (cat)
	//{
	//	cat->SetDoesWantToDie(true);
	//}
}



//RoboCatPtr Server::GetCatForPlayer(int inPlayerId)
//{
//	//run through the objects till we find the cat...
//	//it would be nice if we kept a pointer to the cat on the clientproxy
//	//but then we'd have to clean it up when the cat died, etc.
//	//this will work for now until it's a perf issue
//	const auto& gameObjects = World::sInstance->GetGameObjects();
//	for (int i = 0, c = gameObjects.size(); i < c; ++i)
//	{
//		GameObjectPtr go = gameObjects[i];
//		RoboCat* cat = go->GetAsCat();
//		if (cat && cat->GetPlayerId() == inPlayerId)
//		{
//			return std::static_pointer_cast<RoboCat>(go);
//		}
//	}
//
//	return nullptr;
//
//}