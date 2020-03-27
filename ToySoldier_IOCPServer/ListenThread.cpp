#include "Toy_Shared.h"
DWORD WINAPI ListenThread(LPVOID id) {

	printf("listen 실행 \n");
	TCPSocketPtr listen_sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	//스마트포인터라 쓰레드닫힐때 소켓이 자동으로 종료됨.

	SocketAddress ownAddress(INADDR_ANY, SERVERPORT);

	listen_sock->Bind(ownAddress);
	LOG("Initializing NetworkManager at port %d", SERVERPORT);

	//did we bind okay?
	//if (listen_sock == nullptr)
	//{
	//	return false;
	//}

	//if (listen_sock->SetNonBlockingMode(true) != NO_ERROR)
	//{
	//	return false;
	//}


	listen_sock->Listen(SOMAXCONN);


	DWORD flags = 0;
	SocketAddress clientadrr;
	TCPSocketPtr client_sock;

	while (1) {
		// accept()

		client_sock = listen_sock->Accept(clientadrr);

		printf("[TCP 서버] 클라이언트 접속 : %s\n", clientadrr.ToString().c_str());

		client_sock->ConnectIOCP(NetworkManager::HCP, client_sock);

		// 소켓 정보 구조체 할당 및 새로운 클라이언트 포록시 생성
		SOCKETINFO * ptr = NetworkManagerServer::sInstance->MakeSocketInfo(client_sock, clientadrr);

		// 비동기 입출력 시작 
		client_sock->IOCPRecv(*ptr, flags);



	}
}