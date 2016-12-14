#ifndef NET_MAIN_H
#define NET_MAIN_H

#include <string>

#include "threads_main.h"

using namespace std;

#define ND_CHAT								0x01
#define ND_SERVER_OBJECT_DATA				0x02 // one object at a time
#define ND_CLIENT_OBJECT_DATA				0x03
#define ND_CLIENT_REGISTER_PLAYER			0x04
#define ND_SERVER_REGISTER_ACK				0x05
//#define ND_SERVER_CHANGE_UID_AND_RESEND		0x06
//#define ND_CLIENT_CHANGE_UID				0x08
#define ND_SERVER_CREATE_OBJECT				0x0A // for anything, players, rockets, all of graph etc
#define ND_CLIENT_CREATE_OBJECT				0x0B // for rockets, player on connect, etc
#define ND_CLIENT_CONNECT					0x0C
#define ND_SERVER_CONNECT_ACK				0x0D

int NET_GetNumClients();

//void NET_Init();
void NET_Shutdown();

void NET_ServerListen();
void NET_ClientConnect(string address);
void NET_Disconnect();

/*void NET_ServerSendData(const char dataType, const int clientNumber);
void NET_ServerSendData(const char dataType, const char* data, const short dataSize, const int clientNumber);
void NET_ClientSendData(const char dataType);
void NET_ClientSendData(const char dataType, const char* data, const short dataSize);*/

class net_UpdateThread: public Thread
{
public:
	virtual unsigned long threadProc();
};

extern net_UpdateThread net_updateThread;
#endif