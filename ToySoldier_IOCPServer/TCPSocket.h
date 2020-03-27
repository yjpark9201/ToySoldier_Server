#pragma once


//typedef void (SOCKETINFO::*SOCKETINFO)(SOCKET sock);

class TCPSocket
{
public:
	~TCPSocket();
	int								Connect(const SocketAddress& inAddress);
	void                            ConnectIOCP(HANDLE hcp, TCPSocketPtr clientsock);
	int								Bind(const SocketAddress& inToAddress);
	int								Listen(int inBackLog = 32);
	shared_ptr< TCPSocket >			Accept(SocketAddress& inFromAddress);
	int32_t							Send(const void* inData, size_t inLen);
	int32_t							Receive(void* inBuffer, size_t inLen);
	int32_t							IOCPRecv(SOCKETINFO& ptr, DWORD& flags);
	int32_t							IOCPSend(SOCKETINFO& ptr);
	int                             SetNonBlockingMode(bool inShouldBeNonBlocking);
	void							WSAGetOverlapptedResult(LPWSAOVERLAPPED lpoverlapped);
private:
	friend class SocketUtil;
	friend struct SOCKETINFO;

	TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET		mSocket;
};

