#pragma once

#define BUFSIZE    100
#define HEADERSIZE 2
#define HEADERMIN 2 // ��� �ּ� ��ȿ��
#define HEADERMAX 1000 // ��� �ִ� ��ȿ��
#define CPU_NUM 8
#define TEMPCLIENTID -100//�ӽ� Ŭ���̾�Ʈ ���̵�

class ClientProxy;
class TCPSocket;

typedef shared_ptr< TCPSocket > TCPSocketPtr;
typedef shared_ptr< ClientProxy >	ClientProxyPtr;

#define Client_Mgr  ClientMgr::Instance()