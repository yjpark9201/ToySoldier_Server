#include "Toy_Shared.h"


DWORD WINAPI WorkerThread(LPVOID arg) {

	SetEvent(NetworkManager::CreateWorkerThreadEvent);

	TCP_Theadinfo ThreadInfo = *((TCP_Theadinfo*)arg);
	HANDLE hcp = (HANDLE)ThreadInfo.hcp;

	DWORD  retval, cbTransferred = 0;
	SOCKETINFO *ptr = nullptr;
	TCPSocketPtr client_sock;
	DWORD dummy = 0;

	while (1) {


		// 비동기 입출력 완료 기다리기
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(PULONG_PTR)&dummy, (LPOVERLAPPED *)&ptr, INFINITE);

		client_sock = ptr->client->GetTCPSocket();


		// 비동기 입출력 결과 및 접속종료 확인
		if (NetworkManagerServer::sInstance->CheckForDisconnects(*ptr, retval, cbTransferred))
			continue;



		// 데이터 전송량 갱신
		
		DWORD flags = 0;
		if (ptr->IOOperation == IO_OPERATION::Read) {

			NetworkManagerServer::sInstance->ReadIncomingPackets(*ptr, retval, cbTransferred, ThreadInfo.id);
			
			client_sock->IOCPRecv(*ptr, flags);

		}
		else {
			NetworkManagerServer::sInstance->ProcessSentPacket(*ptr, retval, cbTransferred, ThreadInfo.id);
		
		}
	}

	return 0;

}