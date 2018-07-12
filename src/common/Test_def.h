#ifndef TEST_DEF
#define TEST_DEF

#include "ace/message_block.h"

#define PROACTOR_PORT	4321
#define TIMEOUT 3000

typedef struct MsgHead
{
	u_short type;//request,repley
	u_short length;
	u_long identifier;
	u_long req_type;
	u_long retry_times;
}MsgHead;

typedef struct ReplyLogin
{
	MsgHead hd;
	bool success;
	short int PID;
}ReplyLogin;

typedef struct ReqLogin
{
	MsgHead hd;
	TCHAR name[16];
	TCHAR key[16];
}ReqLogin;


typedef struct {
	MsgHead hd;
	short int PID;
	short int kind;
	short int tx;
	short int ty;
	short int azimuth;
}PERSONAE;

typedef struct RTChat
{
	MsgHead hd;
	TCHAR message[256];
}ReqChat;

#define MSG_REQUEST	0x00000001
#define MSG_REPLY	0x00000002
#define REQ_LOGIN	0x00002000
#define REPLY_LOGIN	0x00004000
#define REPLY_GETOBJ	0x00006000
#define REQ_DIRECT	0x00008000
#define REPLY_DIRECT	0x00010000
#define REPLY_DELOBJ	0x00001000
#define REQ_CHAT	0x00003000
#define REPLY_CHAT	0x00005000

#endif