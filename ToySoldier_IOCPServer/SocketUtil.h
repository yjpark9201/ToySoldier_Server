#pragma once

#define SERVERPORT 8052



enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};


// 오류 출력 함수
void err_quit(const char *msg);
void err_display(const char *msg);





struct TCP_Theadinfo {
	int id = 0; HANDLE hcp;

	TCP_Theadinfo(int th_id, HANDLE th_hcp) { id = th_id, hcp = th_hcp; }
};
class SocketUtil
{
public:

	static bool			StaticInit();
	static void			CleanUp();

	static void			ReportError(const char* inOperationDesc);
	static int			GetLastError();

	static int			Select(const vector< TCPSocketPtr >* inReadSet,
		vector< TCPSocketPtr >* outReadSet,
		const vector< TCPSocketPtr >* inWriteSet,
		vector< TCPSocketPtr >* outWriteSet,
		const vector< TCPSocketPtr >* inExceptSet,
		vector< TCPSocketPtr >* outExceptSet);

	static UDPSocketPtr	CreateUDPSocket(SocketAddressFamily inFamily);
	static TCPSocketPtr	CreateTCPSocket(SocketAddressFamily inFamily);

	static void PrintClientAddr();

private:

	inline static fd_set* FillSetFromVector(fd_set& outSet, const vector< TCPSocketPtr >* inSockets, int& ioNaxNfds);
	inline static void FillVectorFromSet(vector< TCPSocketPtr >* outSockets, const vector< TCPSocketPtr >* inSockets, const fd_set& inSet);
};