#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#pragma comment(lib, "ws2_32")

#include <stdlib.h>
#include <stdio.h>
#include "Toy_Shared.h"

// 작업자 스레드 함수




int main(int argc, char *args[])
{


	if (Server::StaticInit())
	{
		return Server::sInstance->Run();
	}
	else
	{
		//error
		printf("서버 생성 에러로 프로그램 종료\n");
		return 1;
	}


	printf("서버 종료\n");
	return 100;
}
