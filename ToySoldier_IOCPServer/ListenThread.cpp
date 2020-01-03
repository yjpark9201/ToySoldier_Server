#include "Toy_Shared.h"
DWORD WINAPI ListenThread(LPVOID id) {

	printf("listen 실행 \n");
	TCPSocketPtr listen_sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	SocketAddress ownAddress(INADDR_ANY, SERVERPORT);

	listen_sock->Bind(ownAddress);
	LOG("Initializing NetworkManager at port %d", SERVERPORT);


	listen_sock->Listen(SOMAXCONN);


	DWORD recvbytes, flags = 0;
	SocketAddress clientadrr;
	TCPSocketPtr client_sock;

	while (1) {
		// accept()

		client_sock = listen_sock->Accept(clientadrr);

		printf("[TCP 서버] 클라이언트 접속 : %s\n", clientadrr.ToString().c_str());

		client_sock->ConnectIOCP(NetworkManager::HCP);
		// 소켓 정보 구조체 할당
		SOCKETINFO * ptr = client_sock->MakeSocketInfo();

		// 비동기 입출력 시작 
		client_sock->IOCPRecv(*ptr, flags, recvbytes);



	}
}