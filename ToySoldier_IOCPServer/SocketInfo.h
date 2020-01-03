#pragma once

#define BUFSIZE    1200

typedef enum _IO_OPERATION
{
	ClientIoRead, // �б� �۾� ���� ��
	ClientIoWrite  // ���� �۾� ���� ��
} IO_OPERATION, *PIO_OPERATION;

// ���� ���� ������ ���� ����ü
class SOCKETINFO
{


public : SOCKETINFO(SOCKET& sock) {
		ZeroMemory(&overlapped, sizeof(overlapped));
		this->sock = sock;
		recvbytes = sendbytes = 0;
		wsabuf.buf = buf;
	//	memset(buf, 0, BUFSIZE);
		wsabuf.len = BUFSIZE;

	//	printf("������ ȣ��\n");
	}

		 ~SOCKETINFO() {
			// printf("�Ҹ��� ȣ��\n");
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

//typedef shared_ptr< SOCKETINFO > SocketInfoPtr; <- �̷��� �ϸ� IOcompletion Queue�� ���¼��� Socketinfo�� �����ϴ�
// �����Ͱ� ������� SocketInfo�� �޸𸮰� ������. ���� �̷��� �ϸ� �ȵ�.