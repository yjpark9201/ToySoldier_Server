#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#pragma comment(lib, "ws2_32")

#include <stdlib.h>
#include <stdio.h>
#include "Toy_Shared.h"

// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg);
HANDLE CreateThreadEvent;

int main(int argc, char *argv[])
{

	//if (Server::StaticInit())
	//{
	//	return Server::sInstance->Run();
	//}
	//else
	//{
	//	//error
	//	return 1;
	//}
	SocketUtil::StaticInit(); 	// 윈속 초기화

	// 입출력 완료 포트 객체 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("CPU 개수 : %d\n\n", si.dwNumberOfProcessors);


	// (CPU 개수 * 2)개의 작업자 스레드 생성
	HANDLE hThread;
	CreateThreadEvent = CreateEvent(NULL, false, true, NULL);

	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++) {

		WaitForSingleObject(CreateThreadEvent, INFINITE);
		printf("%d번 워커쓰레드 생성\n", i);
		hThread = CreateThread(NULL, 0, WorkerThread, (LPVOID)&(TCP_Theadinfo(i, hcp)), 0, NULL);
		if (hThread == NULL) return 1;
		CloseHandle(hThread);
	}

	// socket()
	TCPSocketPtr listen_sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	SocketAddress ownAddress(INADDR_ANY, SERVERPORT);

	listen_sock->Bind(ownAddress);
	LOG("Initializing NetworkManager at port %d", SERVERPORT);

	listen_sock->Listen(SOMAXCONN);

	// 데이터 통신에 사용할 변수
	DWORD recvbytes, flags = 0;
	SocketAddress clientadrr;
	TCPSocketPtr client_sock;
	while (1) {
		// accept()

		client_sock = listen_sock->Accept(clientadrr);

		printf("[TCP 서버] 클라이언트 접속 : %s\n", clientadrr.ToString().c_str());

		client_sock->ConnectIOCP(hcp);
		// 소켓 정보 구조체 할당
		SOCKETINFO * ptr = client_sock->MakeSocketInfo();

		// 비동기 입출력 시작 
		client_sock->IOCPRecv(*ptr, flags, recvbytes);



	}

	// 윈속 종료
	WSACleanup();

	printf("서버 종료\n");
	return 0;
}

// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg)
{

	SetEvent(CreateThreadEvent);
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
			(LPDWORD)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);
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

