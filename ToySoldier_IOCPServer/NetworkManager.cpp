#include "Toy_Shared.h"

HANDLE NetworkManager::HCP = NULL;
HANDLE NetworkManager::CreateWorkerThreadEvent = NULL;

NetworkManager::NetworkManager() :
	mBytesSentThisFrame(0),
	mDropPacketChance(0.f),
	mSimulatedLatency(0.f)
{
}

NetworkManager::~NetworkManager()
{
}


bool NetworkManager::CreateWorkerThread() {
	int i = 0;
	CreateWorkerThreadEvent = CreateEvent(NULL, false, true, NULL);

	for (;i < (int)CPU_NUM * 2; i++) {

		WaitForSingleObject(CreateWorkerThreadEvent, INFINITE);
		printf("%d锅 况目静饭靛 积己\n", i);
		mWorkerThreads[i] = CreateThread(NULL, 0, WorkerThread, (LPVOID)&(TCP_Theadinfo(i, HCP)), 0, NULL);
		if (mWorkerThreads[i] == NULL) return false;
		CloseHandle(mWorkerThreads[i]);

	}
	WaitForSingleObject(CreateWorkerThreadEvent, INFINITE);
	return true;
}

bool NetworkManager::CreateListenThread() {

		mListenThread = CreateThread(NULL, 0, ListenThread, NULL, 0, NULL);
		if (mListenThread == NULL) return false;
		CloseHandle(mListenThread);
		return true;

}

bool NetworkManager::Init(uint16_t inPort)
{
		 HCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (HCP == NULL) return false;

	//if (!(CreateWorkerThread() && CreateListenThread())) return false;
	CreateListenThread();

	system("pause");
	CreateWorkerThread();

	mBytesReceivedPerSecond = WeightedTimedMovingAverage(1.f);
	mBytesSentPerSecond = WeightedTimedMovingAverage(1.f);

	return true;
}



void NetworkManager::ProcessIncomingPackets(SOCKETINFO& ptr, DWORD & cbTransferred)
{


	ProcessQueuedPackets(ptr,cbTransferred);

	//UpdateBytesSentLastFrame();

}

void NetworkManager::ReadIncomingPackets(SOCKETINFO& ptr, DWORD &retval, DWORD &transferred, int &thread_id)
{


	int totalReadByteCount = 0;
	


	totalReadByteCount += transferred;
		//now, should we drop the packet?
	if (ToyMath::GetRandomFloat() >= mDropPacketChance)
	{
			InputMemoryBitStream* streamptr =  mPacketMgr.RecvPacketFromBuffer(ptr, retval, transferred, thread_id);
			//we made it
			//shove the packet into the queue and we'll handle it as soon as we should...
			//we'll pretend it wasn't received until simulated latency from now
			//this doesn't sim jitter, for that we would need to.....

	//	float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
		//mPacketQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
	}
	else
	{
			LOG("Dropped packet!", 0);
	}


	if (totalReadByteCount > 0)
	{
		mBytesReceivedPerSecond.UpdatePerSecond(static_cast<float>(totalReadByteCount));
	}
}

void NetworkManager::ProcessQueuedPackets(SOCKETINFO& ptr, DWORD & cbTransferred)
{
	////look at the front packet...
	//while (!mPacketQueue.empty())
	//{
	//	ReceivedPacket& nextPacket = mPacketQueue.front();
	//	if (Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime())
	//	{
	//	//	ProcessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
	//		mPacketQueue.pop();
	//	}
	//	else
	//	{
	//		break;
	//	}

	//}

}



void NetworkManager::SentByteCount(int sentbyte)
{
	
	int sentByteCount = sentbyte;
	if (sentByteCount > 0)
	{
		mBytesSentThisFrame += sentByteCount;
	}
}

void NetworkManager::UpdateBytesSentLastFrame()
{
	if (mBytesSentThisFrame > 0)
	{
		mBytesSentPerSecond.UpdatePerSecond(static_cast<float>(mBytesSentThisFrame));

		mBytesSentThisFrame = 0;
	}

}





void NetworkManager::AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
	mNetworkIdToGameObjectMap[inGameObject->GetNetworkId()] = inGameObject;
}

void NetworkManager::RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
	mNetworkIdToGameObjectMap.erase(inGameObject->GetNetworkId());
}