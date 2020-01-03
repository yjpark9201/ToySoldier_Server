#include "Toy_Shared.h"
DWORD WINAPI ListenThread(LPVOID id) {

	printf("listen ���� \n");
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

		printf("[TCP ����] Ŭ���̾�Ʈ ���� : %s\n", clientadrr.ToString().c_str());

		client_sock->ConnectIOCP(NetworkManager::HCP);
		// ���� ���� ����ü �Ҵ�
		SOCKETINFO * ptr = client_sock->MakeSocketInfo();

		// �񵿱� ����� ���� 
		client_sock->IOCPRecv(*ptr, flags, recvbytes);



	}
}