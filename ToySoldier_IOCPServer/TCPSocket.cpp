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

void TCPSocket::ConnectIOCP(HANDLE hcp, TCPSocketPtr clientsock) {
	CreateIoCompletionPort((HANDLE)mSocket, hcp, 0, 0);

	// 첫번째 인자 IOCP와 연결할 device의 핸들, File IO라면 Fileobject, 네트워크니까 Socket.
	// TCPSOcketPtr 던져주면 연결 못해줌.
	// 두번째 인자 Handle은 이미 만들어진 Completion 객체에 대한 핸들
	// 세번째 인자는 완료된 
}

void	TCPSocket::WSAGetOverlapptedResult(LPWSAOVERLAPPED lpoverlapped) {
	DWORD temp1, temp2;
	WSAGetOverlappedResult(mSocket, lpoverlapped,
		&temp1, FALSE, &temp2);
	err_display("WSAGetOverlappedResult()");

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

int32_t TCPSocket::IOCPRecv(SOCKETINFO& ptr, DWORD& flags)
{
	// 소켓과 입출력 완료 포트 연결

	int retval = 0;
	DWORD recvbytes = 0;

	ZeroMemory(&ptr.overlapped, sizeof(ptr.overlapped));
	ptr.wsabuf.buf = ptr.ringbuffer->GetRecordablePoint();
	ptr.wsabuf.len = ptr.ringbuffer->GetRecordableSize();

	retval = WSARecv(mSocket, &(ptr.wsabuf), 1, &recvbytes, &flags, &ptr.overlapped, NULL);

	if (retval < 0)
	{

		if (WSAGetLastError() != ERROR_IO_PENDING) {
			SocketUtil::ReportError("WSARecv()");
			return -SocketUtil::GetLastError();
		}

	}
	return retval;
}

int32_t TCPSocket::IOCPSend(SOCKETINFO& ptr)
{
	// 소켓과 입출력 완료 포트 연결

	int retval = 0;
	DWORD sendbytes;

	ZeroMemory(&ptr.overlapped, sizeof(ptr.overlapped));
	//ptr.wsabuf.buf = ptr.ringbuffer->GetRecordablePoint();
	//ptr.wsabuf.len = ptr.ringbuffer->GetRecordableSize();

	retval = WSASend(mSocket, &(ptr.wsabuf), 1,&sendbytes, 0, &(ptr.overlapped), NULL);
	if (retval < 0)
	{

		if (WSAGetLastError() != ERROR_IO_PENDING) {
			SocketUtil::ReportError("WSARecv()");
			return -SocketUtil::GetLastError();
		}

	}
	return retval;

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
	closesocket(mSocket);
#else
	close(mSocket);
#endif
}
