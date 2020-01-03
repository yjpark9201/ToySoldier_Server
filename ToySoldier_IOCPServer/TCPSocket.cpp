#include "Toy_Shared.h"



int TCPSocket::Connect(const SocketAddress& inAddress)
{
	int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Connect");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

int TCPSocket::Listen(int inBackLog)
{
	int err = listen(mSocket, inBackLog);
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Listen");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

void TCPSocket::ConnectIOCP(HANDLE hcp) {
	CreateIoCompletionPort((HANDLE)mSocket, hcp, (ULONG_PTR)this, 0);

}
SOCKETINFO*	TCPSocket::MakeSocketInfo() {
	return 	new SOCKETINFO(mSocket);

}

int32_t TCPSocket::IOCPRecv(SOCKETINFO& ptr, DWORD& flags, DWORD& recvbytes)
{
	// 소켓과 입출력 완료 포트 연결

	int retval = 0;
	retval = WSARecv(mSocket, &(ptr.wsabuf), 1, &recvbytes, //이부분 없으면 작업자 스레드들이 깨어나지 않아서 데이터 자체를 못받음. 
		&flags, &ptr.overlapped, NULL);

	if (retval < 0)
	{

		if (WSAGetLastError() != ERROR_IO_PENDING) {
			SocketUtil::ReportError("WSARecv()");
			return -SocketUtil::GetLastError();
		}
	
	}
	return retval;
}

int32_t TCPSocket::GetStatusIOCP(HANDLE hcp, SOCKETINFO& ptr, DWORD& cbTransferred)
{
	/*int retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
		(LPDWORD)&mSocket, (LPOVERLAPPED *)&ptr, INFINITE);*/


	//DWORD cbTransferred;
	SOCKET client_sock;
	//SOCKETINFO *ptr = nullptr;

	int retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
		(PULONG_PTR)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);


	if (retval < 0)
	{

		if (WSAGetLastError() != ERROR_IO_PENDING) {
			SocketUtil::ReportError("WSARecv()");
			return -SocketUtil::GetLastError();
		}

	}

	return retval;
}

TCPSocketPtr TCPSocket::Accept(SocketAddress& inFromAddress)
{
	socklen_t length = inFromAddress.GetSize();
	SOCKET newSocket = accept(mSocket, &inFromAddress.mSockAddr, &length);

	if (newSocket != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(newSocket));
	}
	else
	{
		SocketUtil::ReportError("TCPSocket::Accept");
		return nullptr;
	}
}

int32_t	TCPSocket::Send(const void* inData, size_t inLen)
{
	int bytesSentCount = send(mSocket, static_cast< const char* >(inData), inLen, 0);
	if (bytesSentCount < 0)
	{
		SocketUtil::ReportError("TCPSocket::Send");
		return -SocketUtil::GetLastError();
	}
	return bytesSentCount;
}

int32_t	TCPSocket::Receive(void* inData, size_t inLen)
{
	int bytesReceivedCount = recv(mSocket, static_cast< char* >(inData), inLen, 0);
	if (bytesReceivedCount < 0)
	{
		SocketUtil::ReportError("TCPSocket::Receive");
		return -SocketUtil::GetLastError();
	}
	return bytesReceivedCount;
}

int TCPSocket::Bind(const SocketAddress& inBindAddress)
{
	int error = bind(mSocket, &inBindAddress.mSockAddr, inBindAddress.GetSize());
	if (error != 0)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return SocketUtil::GetLastError();
	}

	return NO_ERROR;
}


int TCPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
#if _WIN32
	u_long arg = inShouldBeNonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);
#else
	int flags = fcntl(mSocket, F_GETFL, 0);
	flags = inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
	int result = fcntl(mSocket, F_SETFL, flags);
#endif

	if (result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::SetNonBlockingMode");
		return SocketUtil::GetLastError();
	}
	else
	{
		return NO_ERROR;
	}
}

TCPSocket::~TCPSocket()
{
#if _WIN32
	printf("TCP소켓종료");
	closesocket(mSocket);
#else
	close(mSocket);
#endif
}
