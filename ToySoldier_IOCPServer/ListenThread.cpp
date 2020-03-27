#include "Toy_Shared.h"
DWORD WINAPI ListenThread(LPVOID id) {

	printf("listen ���� \n");
	TCPSocketPtr listen_sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	//����Ʈ�����Ͷ� ����������� ������ �ڵ����� �����.

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

		printf("[TCP ����] Ŭ���̾�Ʈ ���� : %s\n", clientadrr.ToString().c_str());

		client_sock->ConnectIOCP(NetworkManager::HCP, client_sock);

		// ���� ���� ����ü �Ҵ� �� ���ο� Ŭ���̾�Ʈ ���Ͻ� ����
		SOCKETINFO * ptr = NetworkManagerServer::sInstance->MakeSocketInfo(client_sock, clientadrr);

		// �񵿱� ����� ���� 
		client_sock->IOCPRecv(*ptr, flags);



	}
}