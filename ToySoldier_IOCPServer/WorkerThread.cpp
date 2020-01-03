#include "Toy_Shared.h"

DWORD WINAPI WorkerThread(LPVOID arg){
	SetEvent(NetworkManager::CreateWorkerThreadEvent);
	int retval;
	TCP_Theadinfo* ThreadInfo = (TCP_Theadinfo*)arg;
	HANDLE hcp = (HANDLE)ThreadInfo->hcp;
	int thread_id = ThreadInfo->id;
	printf("thread id : %d \n", thread_id);
	while (1) {

		// �񵿱� ����� �Ϸ� ��ٸ���
		DWORD cbTransferred = 0;
		SOCKETINFO *ptr = nullptr;
		TCPSocketPtr client_sock;

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(PULONG_PTR)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);
		//printf(" ���� ���� ����ü �ּ� : %d", ptr);


		// Ŭ���̾�Ʈ ����
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

		// �񵿱� ����� ��� Ȯ��
		if (retval == 0 || cbTransferred == 0) {
			if (retval == 0) {
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
					&temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult()");
			}
			closesocket(ptr->sock);
			printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			delete ptr;
			continue;
		}

		// ������ ���۷� ����
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// ���� ������ ���


			PacketMgr::ProcessRecvPacket(*ptr, clientaddr, cbTransferred);

			//ptr->buf[ptr->recvbytes] = '\0';
			//printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			//	ntohs(clientaddr.sin_port), ptr->buf);
		}
		else {
			ptr->sendbytes += cbTransferred;
		}

		ptr->recvbytes = 0;

		// ������ �ޱ�
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;

		DWORD recvbytes;
		DWORD flags = 0;

		client_sock->IOCPRecv(*ptr, flags, recvbytes);



		if (ptr->recvbytes > ptr->sendbytes) { /// ������ �����ѵ����Ͱ� �ȿ��� ��į??/
			// ������ ������1
			//ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			//ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			//ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			//DWORD sendbytes;
			//retval = WSASend(ptr->sock, &ptr->wsabuf, 1,
			//	&sendbytes, 0, &ptr->overlapped, NULL);
			//if (retval == SOCKET_ERROR) {
			//	if (WSAGetLastError() != WSA_IO_PENDING) {
			//		err_display("WSASend()");
			//	}
			//	continue;
			//}
		}
		else {

		}
	}

	return 0;

}