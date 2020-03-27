#pragma once

#define BUFSIZE    8
#define HEADERSIZE 2
#define HEADERMIN 2 // ��� �ּ� ��ȿ��
#define HEADERMAX 1000 // ��� �ִ� ��ȿ��
#define CPU_NUM 8

class ClientProxy;
class TCPSocket;

typedef shared_ptr< TCPSocket > TCPSocketPtr;
typedef shared_ptr< ClientProxy >	ClientProxyPtr;

#define Client_Mgr  ClientMgr::Instance()