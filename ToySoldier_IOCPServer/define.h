#pragma once

#define BUFSIZE    8
#define HEADERSIZE 2
#define HEADERMIN 2 // 헤더 최소 유효값
#define HEADERMAX 1000 // 헤더 최대 유효값
#define CPU_NUM 8

class ClientProxy;
class TCPSocket;

typedef shared_ptr< TCPSocket > TCPSocketPtr;
typedef shared_ptr< ClientProxy >	ClientProxyPtr;

#define Client_Mgr  ClientMgr::Instance()