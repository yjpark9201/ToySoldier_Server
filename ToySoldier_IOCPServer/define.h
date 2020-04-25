#pragma once

#define BUFSIZE    100
#define HEADERSIZE 2
#define HEADERMIN 2 // 헤더 최소 유효값
#define HEADERMAX 1000 // 헤더 최대 유효값
#define CPU_NUM 8
#define TEMPCLIENTID -100//임시 클라이언트 아이디

class ClientProxy;
class TCPSocket;

typedef shared_ptr< TCPSocket > TCPSocketPtr;
typedef shared_ptr< ClientProxy >	ClientProxyPtr;

#define Client_Mgr  ClientMgr::Instance()