#include "Toy_Shared.h"

DWORD WINAPI WorkerThread(LPVOID arg){
	SetEvent(NetworkManager::CreateWorkerThreadEvent);
	int retval;
	TCP_Theadinfo* ThreadInfo = (TCP_Theadinfo*)arg;
	HANDLE hcp = (HANDLE)ThreadInfo->hcp;
	int thread_id = ThreadInfo->id;
	printf("thread id : %d \n", thread_id);
	while (1) {

		// 비동기 입출력 완료 기다리기
		DWORD cbTransferred = 0;
		SOCKETINFO *ptr = nullptr;
		TCPSocketPtr client_sock;

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(PULONG_PTR)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);
		//printf(" 소켓 정보 구조체 주소 : %d", ptr);


		// 클라이언트 정보
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

		// 비동기 입출력 결과 확인
		if (retval == 0 || cbTransferred == 0) {
			if (retval == 0) {
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
					&temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult()");
			}
			closesocket(ptr->sock);
			printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			delete ptr;
			continue;
		}

		// 데이터 전송량 갱신
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// 받은 데이터 출력


			PacketMgr::ProcessRecvPacket(*ptr, clientaddr, cbTransferred);

			//ptr->buf[ptr->recvbytes] = '\0';
			//printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			//	ntohs(clientaddr.sin_port), ptr->buf);
		}
		else {
			ptr->sendbytes += cbTransferred;
		}

		ptr->recvbytes = 0;

		// 데이터 받기
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;

		DWORD recvbytes;
		DWORD flags = 0;

		client_sock->IOCPRecv(*ptr, flags, recvbytes);



		if (ptr->recvbytes > ptr->sendbytes) { /// 힌번에 전송한데이터가 안오면 어캄??/
			// 데이터 보내기1
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