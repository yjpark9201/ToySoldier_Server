#pragma once


//typedef void (SOCKETINFO::*SOCKETINFO)(SOCKET sock);

class TCPSocket
{
public:
	~TCPSocket();
	int								Connect(const SocketAddress& inAddress);
	void                            ConnectIOCP(HANDLE hcp);
	int								Bind(const SocketAddress& inToAddress);
	int								Listen(int inBackLog = 32);
	shared_ptr< TCPSocket >			Accept(SocketAddress& inFromAddress);
	int32_t							Send(const void* inData, size_t inLen);
	int32_t							Receive(void* inBuffer, size_t inLen);
	int32_t							IOCPRecv(SOCKETINFO& ptr, DWORD& flags, DWORD& recvbytes);
	int                             SetNonBlockingMode(bool inShouldBeNonBlocking);
	SOCKETINFO*						MakeSocketInfo();
	int32_t							GetStatusIOCP(HANDLE hcp, SOCKETINFO& ptr, DWORD& cbTransferred);
private:
	friend class SocketUtil;
	friend class SOCKETINFO;

	TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET		mSocket;
};
typedef shared_ptr< TCPSocket > TCPSocketPtr;
