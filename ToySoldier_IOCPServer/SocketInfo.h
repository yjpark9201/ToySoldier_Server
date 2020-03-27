#pragma once




typedef enum _IO_OPERATION
{
	Read, // 읽기 작업 진행 중
	Write  // 쓰기 작업 진행 중
} IO_OPERATION, *PIO_OPERATION;

// 소켓 정보 저장을 위한 구조체



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

	//	printf("생성자 호출\n");
}

		~SOCKETINFO() {
			// printf("소멸자 호출\n");
		}

		void SetIO_Operation(IO_OPERATION io) {
			IOOperation = io;
		}

	
};


//typedef shared_ptr< SOCKETINFO > SockInfoPtr;

//typedef shared_ptr< SOCKETINFO > SockInfoPtr; <- 이렇게 하면 IOcompletion Queue에 들어가는순간 Socketinfo를 참조하는
// 포인터가 사라져서 SocketInfo의 메모리가 해제됨. 따라서 이렇게 하면 안됨.