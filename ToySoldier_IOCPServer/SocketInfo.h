#pragma once




typedef enum _IO_OPERATION
{
	Read, // �б� �۾� ���� ��
	Write  // ���� �۾� ���� ��
} IO_OPERATION, *PIO_OPERATION;

// ���� ���� ������ ���� ����ü



template <class T>
using  RingBufferPtr = shared_ptr< Circular_buffer<T>>;


struct SOCKETINFO
{

	OVERLAPPED overlapped;

	RingBufferPtr<char> ringbuffer;
	WSABUF wsabuf;
	IO_OPERATION IOOperation;
	ClientProxyPtr client;


SOCKETINFO(ClientProxyPtr inclient ) {

	ZeroMemory(&overlapped, sizeof(overlapped));
	client = inclient;
	ringbuffer = std::make_shared<Circular_buffer<char>>(BUFSIZE);
	wsabuf.buf = ringbuffer->GetBegin();
	//	memset(buf, 0, BUFSIZE);
	wsabuf.len = BUFSIZE;
	IOOperation = _IO_OPERATION::Read;

	//	printf("������ ȣ��\n");
}

		~SOCKETINFO() {
			// printf("�Ҹ��� ȣ��\n");
		}

		void SetIO_Operation(IO_OPERATION io) {
			IOOperation = io;
		}

	
};


//typedef shared_ptr< SOCKETINFO > SockInfoPtr;

//typedef shared_ptr< SOCKETINFO > SockInfoPtr; <- �̷��� �ϸ� IOcompletion Queue�� ���¼��� Socketinfo�� �����ϴ�
// �����Ͱ� ������� SocketInfo�� �޸𸮰� ������. ���� �̷��� �ϸ� �ȵ�.