#pragma once

#define BUFSIZE    1200

typedef enum _IO_OPERATION
{
	ClientIoRead, // 읽기 작업 진행 중
	ClientIoWrite  // 쓰기 작업 진행 중
} IO_OPERATION, *PIO_OPERATION;

// 소켓 정보 저장을 위한 구조체
class SOCKETINFO
{


public : SOCKETINFO(SOCKET& sock) {
		ZeroMemory(&overlapped, sizeof(overlapped));
		this->sock = sock;
		recvbytes = sendbytes = 0;
		wsabuf.buf = buf;
	//	memset(buf, 0, BUFSIZE);
		wsabuf.len = BUFSIZE;

	//	printf("생성자 호출\n");
	}

		 ~SOCKETINFO() {
			// printf("소멸자 호출\n");
		 }

		 void SetIO_Operation(IO_OPERATION io) {
			 IOOperation = io;
		 }

		 OVERLAPPED overlapped;

		 SOCKET sock;
		 char buf[BUFSIZE + 1];
		 int recvbytes;
		 int sendbytes;
		 WSABUF wsabuf;
		 IO_OPERATION IOOperation;

};

//typedef shared_ptr< SOCKETINFO > SocketInfoPtr; <- 이렇게 하면 IOcompletion Queue에 들어가는순간 Socketinfo를 참조하는
// 포인터가 사라져서 SocketInfo의 메모리가 해제됨. 따라서 이렇게 하면 안됨.