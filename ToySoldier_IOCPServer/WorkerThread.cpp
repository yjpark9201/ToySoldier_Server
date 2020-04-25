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


		// �񵿱� ����� �Ϸ� ��ٸ���
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(PULONG_PTR)&dummy, (LPOVERLAPPED *)&ptr, INFINITE);

		client_sock = ptr->client->GetTCPSocket();


		// �񵿱� ����� ��� �� �������� Ȯ��
		if (NetworkManagerServer::sInstance->CheckForDisconnects(*ptr, retval, cbTransferred))
			continue;



		// ������ ���۷� ����
		
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