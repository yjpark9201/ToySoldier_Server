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
	ringbuffer = std::make_shared<Circular_buffer<char>>(BUFSIZE);
	Init(inclient);
	

}

SOCKETINFO(ClientProxyPtr inclient, CHAR* buffer, size_t size) {
	ringbuffer = std::make_shared<Circular_buffer<char>>(buffer, size);
	Init(inclient);

}



		~SOCKETINFO() {
			 printf("���� ���� �Ҹ��� ȣ�� �޸� �ּ� : %d\n", this);
		}

		void SetIO_Operation(IO_OPERATION io) {
			IOOperation = io;
		}

private: void Init(ClientProxyPtr inclient) {
	ZeroMemory(&overlapped, sizeof(overlapped));
	client = inclient;
	wsabuf.buf = ringbuffer->GetRecordablePoint();
	wsabuf.len = ringbuffer->GetRecordableSize();
	IOOperation = _IO_OPERATION::Read;
	
}

	
};


//typedef shared_ptr< SOCKETINFO > SockInfoPtr;

//typedef shared_ptr< SOCKETINFO > SockInfoPtr; <- �̷��� �ϸ� IOcompletion Queue�� ���¼��� Socketinfo�� �����ϴ�
// �����Ͱ� ������� SocketInfo�� �޸𸮰� ������. ���� �̷��� �ϸ� �ȵ�.