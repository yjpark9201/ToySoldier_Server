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

	if (!(CreateWorkerThread() && CreateListenThread())) return false;


	mBytesReceivedPerSecond = WeightedTimedMovingAverage(1.f);
	mBytesSentPerSecond = WeightedTimedMovingAverage(1.f);

	//did we bind okay?
	if (mSocket == nullptr)
	{
		return false;
	}

	if (mSocket->SetNonBlockingMode(true) != NO_ERROR)
	{
		return false;
	}

	return true;
}


void NetworkManager::ProcessIncomingPackets()
{
	ReadIncomingPacketsIntoQueue();

	ProcessQueuedPackets();

	UpdateBytesSentLastFrame();

}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
	////should we just keep a static one?
	////should we just keep a static one?
	//char packetMem[1500];
	//int packetSize = sizeof(packetMem);
	//InputMemoryBitStream inputStream(packetMem, packetSize * 8);
	//SocketAddress fromAddress;

	////keep reading until we don't have anything to read ( or we hit a max number that we'll process per frame )
	//int receivedPackedCount = 0;
	//int totalReadByteCount = 0;

	//while (receivedPackedCount < kMaxPacketsPerFrameCount)
	//{
	//	int readByteCount = mSocket->ReceiveFrom(packetMem, packetSize, fromAddress);
	//	if (readByteCount == 0)
	//	{
	//		//nothing to read
	//		break;
	//	}
	//	else if (readByteCount == -WSAECONNRESET)
	//	{
	//		//port closed on other end, so DC this person immediately
	//		HandleConnectionReset(fromAddress);
	//	}
	//	else if (readByteCount > 0)
	//	{
	//		inputStream.ResetToCapacity(readByteCount);
	//		++receivedPackedCount;
	//		totalReadByteCount += readByteCount;

	//		//now, should we drop the packet?
	//		if (RoboMath::GetRandomFloat() >= mDropPacketChance)
	//		{
	//			//we made it
	//			//shove the packet into the queue and we'll handle it as soon as we should...
	//			//we'll pretend it wasn't received until simulated latency from now
	//			//this doesn't sim jitter, for that we would need to.....

	//			float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
	//			mPacketQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
	//		}
	//		else
	//		{
	//			LOG("Dropped packet!", 0);
	//			//dropped!
	//		}
	//	}
	//	else
	//	{
	//		//uhoh, error? exit or just keep going?
	//	}
	//}

	//if (totalReadByteCount > 0)
	//{
	//	mBytesReceivedPerSecond.UpdatePerSecond(static_cast<float>(totalReadByteCount));
	//}
}

void NetworkManager::ProcessQueuedPackets()
{
	//look at the front packet...
	while (!mPacketQueue.empty())
	{
		ReceivedPacket& nextPacket = mPacketQueue.front();
		if (Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime())
		{
			ProcessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
			mPacketQueue.pop();
		}
		else
		{
			break;
		}

	}

}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress)
{
	//int sentByteCount = mSocket->SendTo(inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), inFromAddress);
	//if (sentByteCount > 0)
	//{
	//	mBytesSentThisFrame += sentByteCount;
	//}
}

void NetworkManager::UpdateBytesSentLastFrame()
{
	if (mBytesSentThisFrame > 0)
	{
		mBytesSentPerSecond.UpdatePerSecond(static_cast<float>(mBytesSentThisFrame));

		mBytesSentThisFrame = 0;
	}

}


NetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress) :
	mReceivedTime(inReceivedTime),
	mFromAddress(inFromAddress),
	mPacketBuffer(ioInputMemoryBitStream)
{
}


void NetworkManager::AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
	mNetworkIdToGameObjectMap[inGameObject->GetNetworkId()] = inGameObject;
}

void NetworkManager::RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
	mNetworkIdToGameObjectMap.erase(inGameObject->GetNetworkId());
}