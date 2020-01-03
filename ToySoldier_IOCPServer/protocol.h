

#pragma once
#define Object_MAX_NUM 50
#define NAME_LEN 20


enum ObjectType
{
	obj_none,
	obj_Rabbit,
	obj_Boss,
	obj_HoodGirl,
	obj_Torch,
	obj_Stump,
	obj_Tile,
	obj_Dark

};
enum ObjectState {
	obj_dead,
	obj_idle,
	obj_attack

};


enum Packet_Type_Send {
	Bool_GameWaiting,
	Bool_GameBegin,
	Bool_GameTitle,
	Bool_GameWaving,
	Bool_GameLoading,
	Bool_GameClear,
	Bool_GameOver,
	Int_User_State,
	Int_Obj_State,
	Int_User_ID,
	Int_Score,
	Int_Stump_Time,
	Int_WaveLevel,
	String_User_name,
	none

};
enum Packet_Type_Recv {
	Opening_End,
	Opening_Start,
	Game_Ready

};
enum Object_type
{
	rabbit,
	boss,
	user,
	torch



};

struct Header {

	BYTE size;
	BYTE Packet_type;

};
struct Object_info {
	ObjectType obj_type;
	char id;
	int pX, pY;
	int Velocity;


};


// #pragma pack(1)// 왠지모르겠는데 이거 쓰면 [WSARecv()] 호출에 대한 포인터 인수를 사용하려는 동안 시스템에서 잘못된 포인터 주소를 감지했습니다. 라는 에러가뜸
struct TCP_SendIntPacket : public Header {
	BYTE Eventint;
};

// #pragma pack(1)
struct TCP_SendBoolPacket : public Header {
	bool Eventflag;
};


//#pragma pack(1)
//struct TCP_SendStrPacket : public Header {
//	char* EventStr;
//};
//
//#pragma pack(1)
//struct TCP_RecvBoolPacket : public Header {
//	bool Eventflag;
//};
//struct TCP_RecvNamePacket : public Header {
//	char name[NAME_LEN];
//};
//
//#pragma pack(1)
//struct UDP_RecvPacket {
//	int id;
//	UINT iMessage;
//	WPARAM KeyInputBuffer;
//};
//#pragma pack(1)
//struct UDP_SendPacket {
//	int User_Position_x;
//	int User_Position_y;
//	int Velocity;
//	Object_info obj[50];
//};
//
//
//
//
