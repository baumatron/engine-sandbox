#include <winsock2.h>

#include "threads_main.h"
#include "net_main.h"
#include "con_main.h"
//#include "bn_main.h"


#include <iostream>
#include <string>
#include <queue>


using namespace std;

//int clientCount(0);
string username;

//bool serverThreadQuit = false;
//bool netquit = false;
//bool connected = false;

net_UpdateThread net_updateThread;

void NET_Disconnect();
void NET_ClientConnect(string address);

// to protect data from being overwritten
// expample: if a sendData function is called and the data is being pumped out
//			 the monitor is aquired. This way, when something else wants to 
//			 send data, when it trys to aquire the monitor, it will wait
//			 until it is released, which happens when the data has all been
//			 pumped out.
struct uidPair
{
	unsigned long localUid;
	unsigned long remoteUid;
};

class Monitor_2	// will not skip the acquire wait loop if this thread already has called aquire(), Monitor 1 will do this with mutexOn()
{
public:
	Monitor_2():inUse(false){}
	~Monitor_2(){}

	void acquire()
	{
		while(inUse)
			;
		inUse = true;
	}

	void release()
	{
		inUse = false;
	}
private:
	volatile bool inUse;
};


enum ConnectionStates {cs_disconnected = 0, cs_disconnecting, cs_connected, cs_connecting};

class net_ConnectionToServer
{
public:
	net_ConnectionToServer(string address);
	~net_ConnectionToServer();

	void update();
	void startThreads();
	void stopThreads();
	void sendData(const char dataType, const char* data, const short size);
	void getData(char& dataType, char* data); // data must be a 128 char array
	bool isNewDataAvailable();
	
		// RFSThread RFS = recieve from server
		class RFSThread: public Thread
		{
		public:
			RFSThread();
			virtual ~RFSThread();

			virtual DWORD threadProc();

			friend class net_ConnectionToServer;
		protected:
			queue<char *> data;
			//char dataRecieved[128];
			//bool dataIsNew;
			net_ConnectionToServer* owner;
		};

		// STS = SendToServer
		class STSThread: public Thread
		{
		public:
			STSThread();
			virtual ~STSThread();

			virtual DWORD threadProc();

			friend class net_ConnectionToServer;
		protected:
			queue<char *> data;
			//char dataToSend[128];
			net_ConnectionToServer* owner;
		};
private:
	vector<uidPair> uidCrossReference;

	ConnectionStates networkState;
	ConnectionStates gameNetworkState;
/*	Monitor_2 recieveMonitor;
	Monitor_2 sendMonitor;	// mutex is on when data is being sent*/

	RFSThread recieveThread;
	STSThread sendThread;

	SOCKET sock;
};

class net_Server
{
public:
	net_Server();
	~net_Server();

	int getNumClients();
	void update();
	//void send( int client );
	//void send(  );

		class SLThread: public Thread
		{
		public:
			SLThread();
			virtual ~SLThread();

			virtual unsigned long threadProc();

			friend class net_Server;
		protected:
			net_Server* owner;
		};

	friend class SLThread;

	/////////////////////////////////////////////////////////
		class net_Client
		{
		public:
			net_Client(net_Server* owner);
			~net_Client();

			void startThreads();
			void stopThreads();

			void update();

			void sendData(const char dataType, const char* data, const short size);
			void getData(char& dataType, char* data);
			bool isNewDataAvailable();

			void setSocket(SOCKET _socket);
			void setSockaddr(sockaddr _sockaddr);

				// RFCThread RFC = recieve from client
				class RFCThread: public Thread
				{
				public:
					RFCThread();
					virtual ~RFCThread();

					virtual unsigned long threadProc();

					friend class net_Client;
				protected:
					queue<char *> data;
					//char dataRecieved[128];
					//bool dataIsNew;
					net_Client* owner;
				};

				// STC = SendToClient
				class STCThread: public Thread
				{
				public:
					STCThread();
					virtual ~STCThread();

					virtual unsigned long threadProc();

					friend class net_Client;
				protected:
					queue<char *> data;
					//char dataToSend[128];
					net_Client* owner;
				};

			friend class RFCThread;
			friend class STCThread;
			friend class net_Server;

		private:
			vector<uidPair> uidCrossReference;

			ConnectionStates networkState;
			ConnectionStates gameNetworkState;

		//	Monitor_2 sendMonitor;	// acquired when data is being sent
		//	Monitor_2 recieveMonitor;   // acquired when data has been recieved

//			bn_Player* player;

			SOCKET sock;
			sockaddr sockAddr;

			RFCThread recieveThread;
			STCThread sendThread;

			net_Server* owner;
		};
	/////////////////////////////////////////////////////////
		class net_ClientList
		{
		public:
			net_ClientList();
			~net_ClientList();

			net_Client* createNewClient(SOCKET socket, sockaddr sockaddr);
			void deleteClient(int index);
			net_Client& operator[] (int index);

			class net_ClientNode
			{
			public:
				net_ClientNode(net_Server* owner):next(0), client(owner){}
				~net_ClientNode(){}

				net_ClientNode* next;
				net_Client client;
			};

			int clientCount;
			net_ClientNode* head;

			net_Server* owner;
		};
	/////////////////////////////////////////////////////////

	void relay(const char dataType, const char* data, const short dataSize, net_Client* source);
	void broadcast(const char dataType, const char* data, const short dataSize);
	void broadcast(const char dataType);


private:
	net_ClientList clientList;	// make this private later
	bool endThreads;
	SLThread listenThread;
	SOCKET sock;
};






////////////////////////////////////////////////////////////////////////
net_ConnectionToServer* connectionToServer(0);
net_Server* server;
////////////////////////////////////////////////////////////////////////

//////////////////// CLASS IMPLEMENTATIONS ///////////////////////////////

net_Server::net_Client::RFCThread::RFCThread():
Thread()
{
}

net_Server::net_Client::RFCThread::~RFCThread()
{
}

DWORD net_Server::net_Client::RFCThread::threadProc()
{
	while(running)
	{
		char* packedData = new char[128];
		if(owner->networkState == cs_disconnected)
		{
			ccout << "net_Server::net_Client::RFCThread::threadProc(), no connection, shouldn't be here\n";
			break;
		}
		if(recv(owner->sock, packedData, 128, 0) == SOCKET_ERROR)
		{
			owner->networkState = cs_disconnected;
			break;
		}

//		owner->recieveMonitor.acquire();
		monitor.mutexOn();
		data.push(packedData);
//		memcpy(dataRecieved, temp, 128);
//		dataIsNew = true;
		monitor.mutexOff();
	}
	ccout << "Client disconnected from server.\n";
	// TODO
	// should do something here like kill the client object or whatever
	return TRUE;
}

net_Server::net_Client::STCThread::STCThread():
Thread()
{
}

net_Server::net_Client::STCThread::~STCThread()
{
}

DWORD net_Server::net_Client::STCThread::threadProc()
{
	while(running)
	{
		monitor.mutexOn();
		if(!data.empty())
		{
			char* packedData = data.front();
			data.pop();
			monitor.mutexOff();
			// should change this to send less than 128 bytes each time...
			if(send(owner->sock, packedData, 128, 0) == SOCKET_ERROR)
			{
				owner->networkState = cs_disconnected;
				ccout << "send(owner->sock, dataToSend, sizeof(dataToSend), 0) = SOCKET_ERROR\n";
			}
			delete [] packedData;
		}
		else
			monitor.mutexOff();
	}
	//owner->sendMonitor.release();

	// TODO
	// should do something here like kill the client object or whatever
	return TRUE;
}

//

net_Server::SLThread::SLThread():
Thread()
{
}

net_Server::SLThread::~SLThread()
{
}


DWORD net_Server::SLThread::threadProc()
{
	while (running) 
	{
		sockaddr tempSockaddr;
		SOCKET temp = accept (owner->sock, &tempSockaddr, 0); 
		if (temp == INVALID_SOCKET)
		{ // error accepting connection
			//WSACleanup ();
			ccout << "Error: clientSocket is invalid. Probably because client disconnected.\n";
			return TRUE;
		}
		else // client connected successfully
		{
			net_Server::net_Client* newClient = owner->clientList.createNewClient(temp, tempSockaddr);
			// start up the required threads
			newClient->startThreads();

			ccout << "Client is connected.\n";
			ccout << "Number of clients: " << owner->getNumClients() << newl;
		}
	}

	return TRUE;
}



net_ConnectionToServer::RFSThread::RFSThread():
Thread()
{
}

net_ConnectionToServer::RFSThread::~RFSThread()
{
}

DWORD net_ConnectionToServer::RFSThread::threadProc()
{
	while(running)
	{
		char* packedData = new char[128];
		if(owner->networkState == cs_disconnected)
		{
			ccout << "net_ConnectionToServer::RFSThread::threadProc(), no connection, shouldn't be here\n";
			break;
		}
		if(recv(owner->sock, packedData, 128, 0) == SOCKET_ERROR)
		{
			owner->networkState = cs_disconnected;
			break;
		}

	//	owner->recieveMonitor.acquire();
		monitor.mutexOn();
		data.push(packedData);
		//memcpy(dataRecieved, temp, 128);
		//dataIsNew = true;
		monitor.mutexOff();
	}
	ccout << "Client disconnected from server.\n";

	// TODO
	// should do something here like kill the client object or whatever
	return TRUE;
}

net_ConnectionToServer::STSThread::STSThread():
Thread()
{
}

net_ConnectionToServer::STSThread::~STSThread()
{
}

DWORD net_ConnectionToServer::STSThread::threadProc()
{
	while(running)
	{
		monitor.mutexOn();
		if(!data.empty())
		{
			// pop a pointer off the front of the queue
			char* packedData = data.front();
			data.pop();
			monitor.mutexOff();

			// send it off
			if(send(owner->sock, packedData, 128, 0) == SOCKET_ERROR)
			{
				owner->networkState = cs_disconnected;
				ccout << "send(owner->sock, dataToSend, sizeof(dataToSend), 0) = SOCKET_ERROR\n";
			}
			// free memory
			delete [] packedData;
		}
		else
			monitor.mutexOff();
	}

	//owner->sendMonitor.release();


	// TODO
	// should do something here like kill the client object or whatever
	return TRUE;
}


net_ConnectionToServer::net_ConnectionToServer(string address):
gameNetworkState(cs_disconnected),
networkState(cs_disconnected)
{
	recieveThread.owner = this;
	sendThread.owner = this;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET; // address family Internet 
	addr.sin_port = htons (5001); // set server’s port number 
	addr.sin_addr.s_addr = inet_addr(address.c_str());  
	if (addr.sin_addr.s_addr == INADDR_NONE) 
	{
		// Host isn't an IP address, try using DNS
		hostent* HE = gethostbyname(address.c_str());
		if (HE == 0) 
		{
			ccout << "Error resolving host name.\n";
			return;
		}
		addr.sin_addr.s_addr = *((unsigned long*)HE->h_addr_list[0]);
	}

	if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) 
	{ // an error connecting has occurred! 
		//WSACleanup (); 
		ccout << "Socket error occured when connecting.\n";
		return; 
	} 

	networkState = cs_connected;
	ccout << "You are connected to " << address << ".\n";
	
	startThreads();
//	connected = true;
}

net_ConnectionToServer::~net_ConnectionToServer()
{
	// end threads
	// close socket
	networkState = cs_disconnected;

	shutdown (sock, SD_SEND);  // clientSocket can't send anymore
	closesocket (sock);
//	connected = false;

	recieveThread.stop();
	sendThread.stop();
}

void net_ConnectionToServer::update()
{
	if(networkState != cs_connected)
	{
		stopThreads();
		return;
	}
	// process data
	// SEND ALL DATA
	//uidReq uidReqData;
	/*bn_Ent::bn_EntNetData netData;
	
	if(gameNetworkState == cs_connected)
	{
		bn_Ent::bn_EntNetData netData;

		Video.clientSceneGraph.monitor.mutexOn();
		Video.serverSceneGraph.monitor.mutexOn();
		
		for(unsigned int i = 0; i < Video.serverSceneGraph.getRoot()->childlist.size(); i++)
//		for(vid_SceneObject* i = Video.serverSceneGraph.getHead()->childlist; i != NULL; i = i->next)
		{
			if(Video.serverSceneGraph.getRoot()->childlist[i]->sotype == vid_SceneObject::entity)
			{
				if( ((bn_Ent*)Video.serverSceneGraph.getRoot()->childlist[i])->enttype == bn_Ent::player)
				{
					netData = ((bn_Player*)Video.serverSceneGraph.getRoot()->childlist[i])->getNetData();
					sendData(ND_CLIENT_OBJECT_DATA, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));
				}
			}
		}
		if(theplayer)
			netData = theplayer->getNetData();

		Video.clientSceneGraph.monitor.mutexOff();
		Video.serverSceneGraph.monitor.mutexOff();

		sendData(ND_CLIENT_OBJECT_DATA, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));
	}
*/
	// RECIEVE ALL DATA
	if(isNewDataAvailable())
	{
		char dataType;
		char data[127];
		
		getData(dataType, data);

//		bn_Ent* object(0);


		switch(dataType)
		{
		case ND_CHAT:
			ccout << (data) << newl;
			break;
		case ND_SERVER_CONNECT_ACK:
			{
				ccout << "ND_SERVER_CONNECT_ACK\n";
				gameNetworkState = cs_connected;

	/*		//	bn_Ent::bn_EntNetData netData;

				Video.clientSceneGraph.monitor.mutexOn();
				Video.serverSceneGraph.monitor.mutexOn();

				if(theplayer)
					netData = theplayer->getNetData();

				Video.clientSceneGraph.monitor.mutexOff();
				Video.serverSceneGraph.monitor.mutexOff();

				ccout << "sending net data:\n" << netData << newl;
				sendData(ND_CLIENT_CREATE_OBJECT, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));*/

				break;
			}
		case ND_SERVER_CREATE_OBJECT:
			{
				ccout << "ND_SERVER_CREATE_OBJECT\n";

			/*	memcpy(&netData, data, sizeof(bn_Ent::bn_EntNetData));

				ccout << "recieved net data:\n" << netData << newl;

				switch((bn_Ent::ent_EntType)netData.enttype)
				{
				case bn_Ent::player:
					ccout << "object type is player" << newl;
					object = new bn_Player;
					break;
				case bn_Ent::pickup:
					switch((bn_Pickup::pup_PupType)netData.puptype)
					{
					case bn_Pickup::health:
						object = new bn_Health;
						break;
					default:
						// massive error!!!
						ccout << "Bad network error!\n";
						return;
						break;
					}
				//case bn_Ent::monster:
				//	object = new bn_Monster;
				//	break;
				case bn_Ent::projectile:
					object = new bn_Rocket; // this is not a general solution, should be changed
					// todo: change this
					break;
				default:
					ccout << "object type is unkown" << newl;
					return;
					break;
				}
				
				object->transform.mtfIdentity();
				matrix4x4 matrix;
				object->transform *= matrix.mtfTranslate(v3d(netData.x, netData.y, netData.z)) * matrix.mtfXRot(netData.ax) * matrix.mtfYRot(netData.ay) * matrix.mtfZRot(netData.az);
				object->creatorAliasUid = netData.creatorAliasUid;
				object->setDeaths(netData.deaths);
				object->setHp(netData.hp);
				object->kill = netData.kill;
				object->setPoints(netData.points);
				object->setState((bn_Ent::states)netData.state);
				object->setSuicides(netData.suicides);
				object->visible = netData.visible;
//				object->velocity = v3d(netData.vx, netData.vy, netData.vz);
				uidPair temp;
				temp.localUid = aliases.getUid(object);
				temp.remoteUid = netData.uid;
				uidCrossReference.push_back(temp);

				Video.clientSceneGraph.addObject(object);
*/
				break;
			}
	
		case ND_SERVER_OBJECT_DATA:
			{
			/*	memcpy(&netData, data, sizeof(bn_Ent::bn_EntNetData));

				unsigned long localUid(0);
				for(vector<uidPair>::iterator i = uidCrossReference.begin(); i != uidCrossReference.end(); i = i++)
				{
					if(netData.uid == i->remoteUid) // found the right cross reference
					{
						localUid = i->localUid;
						break;
					}
				}

				Video.clientSceneGraph.monitor.mutexOn();
				Video.serverSceneGraph.monitor.mutexOn();

				object = (bn_Ent*)aliases.getObject(localUid);

				if(!object)
				{
			//		ccout << "Received an invalid UID!\n";
					Video.clientSceneGraph.monitor.mutexOff();
					Video.serverSceneGraph.monitor.mutexOff();
					break;
				}
				if(object == theplayer)
				{
					object->creatorAliasUid = netData.creatorAliasUid;
					object->setDeaths(netData.deaths);
					object->setHp(netData.hp);
					object->kill = netData.kill;
					object->setPoints(netData.points);
					object->setState((bn_Ent::states)netData.state);
					object->setSuicides(netData.suicides);
					object->visible = netData.visible;
					Video.clientSceneGraph.monitor.mutexOff();
					Video.serverSceneGraph.monitor.mutexOff();
					break;
				}

				object->transform.mtfIdentity();
				matrix4x4 matrix;
				object->transform *= matrix.mtfTranslate(v3d(netData.x, netData.y, netData.z)) * matrix.mtfXRot(netData.ax) * matrix.mtfYRot(netData.ay) * matrix.mtfZRot(netData.az);
				object->creatorAliasUid = netData.creatorAliasUid;
				object->setDeaths(netData.deaths);
				object->setHp(netData.hp);
				object->kill = netData.kill;
				object->setPoints(netData.points);
				object->setState((bn_Ent::states)netData.state);
				object->setSuicides(netData.suicides);
				object->visible = netData.visible;
//				object->velocity = v3d(netData.vx, netData.vy, netData.vz);

				Video.clientSceneGraph.monitor.mutexOff();
				Video.serverSceneGraph.monitor.mutexOff();
*/
				break;
			}
		default:
			break;
		}
	}
}

void net_ConnectionToServer::startThreads()
{
	if(networkState == cs_connected)
	{
		recieveThread.start();
		sendThread.start();
	}
}

void net_ConnectionToServer::stopThreads()
{
	recieveThread.stop();
	sendThread.stop();
}

void net_ConnectionToServer::sendData(const char dataType, const char* data, const short size)
{
	if(networkState != cs_connected)
	{
		ccout << "Error: no connection.\n";
		return;
	}
	if(size > 127)
	{
		ccout << "Error: sendData size parameter is too large!\n";
		return;
	}
	// allocate memory and copy data
	char* packedData = new char[128];
	packedData[0] = dataType;
	memcpy(&(packedData[1]), data, size);

	// queue it up
	sendThread.monitor.mutexOn();
	sendThread.data.push(packedData);
	sendThread.monitor.mutexOff();
}

void net_ConnectionToServer::getData(char& dataType, char* data) // data must be a 128 char array
{
	recieveThread.monitor.mutexOn();
	char* packedData = recieveThread.data.front();
	recieveThread.data.pop();
	recieveThread.monitor.mutexOff();
	dataType = packedData[0];
	memcpy(data, &(packedData[1]), 127);
	delete [] packedData;
//	recieveThread.dataIsNew = false;
//	recieveMonitor.release();
}

bool net_ConnectionToServer::isNewDataAvailable()
{
	return !recieveThread.data.empty();
	//return recieveThread.dataIsNew;
}


////////////////////////////////////////////////////////////////////////


net_Server::net_Client::net_Client(net_Server* _owner):
owner(_owner),
networkState(cs_connected),
gameNetworkState(cs_disconnected)
{
	recieveThread.owner = this;
	sendThread.owner = this;
}

net_Server::net_Client::~net_Client()
{
	shutdown (sock, SD_SEND);  // clientSocket can't send anymore
	closesocket (sock);

	stopThreads();
}

void net_Server::net_Client::startThreads()
{
	if(networkState == cs_connected)
	{
		recieveThread.start();
		sendThread.start();
	}
}

void net_Server::net_Client::stopThreads()
{
	recieveThread.stop();
	sendThread.stop();
}

void net_Server::net_Client::update()
{
	if(networkState != cs_connected)
	{
		stopThreads();
		return;
	}
	// process data
	// SEND ALL DATA
	
	// replace this with a loop that send ALL player and entity data

/*	Video.clientSceneGraph.monitor.mutexOn();
	Video.serverSceneGraph.monitor.mutexOn();

	bn_Ent::bn_EntNetData netData;
	for(unsigned int i = 0; i < Video.serverSceneGraph.getRoot()->childlist.size(); i++)
	//for(vid_SceneObject* i = Video.serverSceneGraph.getHead()->childlist; i != NULL; i = i->next)
	{
		if(Video.serverSceneGraph.getRoot()->childlist[i]->sotype == vid_SceneObject::entity)
		{
			if( ((bn_Ent*)Video.serverSceneGraph.getRoot()->childlist[i])->enttype == bn_Ent::player)
			{
				netData = ((bn_Player*)Video.serverSceneGraph.getRoot()->childlist[i])->getNetData();
				owner->broadcast(ND_CLIENT_OBJECT_DATA, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));
			}
		}
	}

	if(theplayer)
		netData = theplayer->getNetData();

	Video.clientSceneGraph.monitor.mutexOff();
	Video.serverSceneGraph.monitor.mutexOff();

	owner->broadcast(ND_SERVER_OBJECT_DATA, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));
*/

	// RECIEVE ALL DATA
	if(isNewDataAvailable())
	{
		char dataType;
		char data[127];
		
		getData(dataType, data);

		switch(dataType)
		{
		case ND_CHAT:
			{
				ccout << (data) << newl;
				// now repeat the data to all other clients
				owner->relay(dataType, data, 127, this);
				break;
			}
		case ND_CLIENT_CONNECT:
			{
				ccout << "ND_CLIENT_CONNECT\n";
/*
	//			for(int i = 0; i < NET_GetNumClients(); i++)
				{
					bn_Ent::bn_EntNetData netData;
					// Video.serverSceneGraph.monitor.mutexOn();
		//			for(vid_SceneObject* index = Video.serverSceneGraph.getHead()->childlist; index != 0; index = index->next)
		//			{
		//				// loop through all objects in the server scene graph and send their data to the server, then remove
		//				netData = ((bn_Ent*)index)->getNetData();
		//				if(index->sotype == vid_SceneObject::entity)
		//					server->broadcast(NET_SERVER_CREATE_OBJECT, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));
		//					//NET_ServerSendData(ND_SERVER_CREATE_OBJECT, (char*)&netData, sizeof(bn_Ent::bn_EntNetData), i);
		//				// else, don't care, only send ent's
		//			}

					Video.clientSceneGraph.monitor.mutexOn();
					Video.serverSceneGraph.monitor.mutexOn();
					netData = theplayer->getNetData();
					Video.clientSceneGraph.monitor.mutexOff();
					Video.serverSceneGraph.monitor.mutexOff();

					ccout << "sending net data:\n" << netData << newl;
					ccout << "sending ND_SERVER_CREATE_OBJECT\n";

					owner->broadcast(ND_SERVER_CREATE_OBJECT, (char*)&netData, sizeof(bn_Ent::bn_EntNetData));

					ccout << "after sending ND_SERVER_CREATE_OBJECT\n";
					//NET_ServerSendData(ND_SERVER_OBJECT_DATA, (char*)&netData, sizeof(bn_Ent::bn_EntNetData), i);
				}

				ccout << "sending ND_SERVER_CONNECT_ACK\n";
				owner->broadcast(ND_SERVER_CONNECT_ACK);
				ccout << "after sending ND_SERVER_CONNECT_ACK\n";
*/
				gameNetworkState = cs_connected;
				break;
			}
		case ND_CLIENT_CREATE_OBJECT:
			{
				ccout << "ND_CLIENT_CREATE_OBJECT\n";
/*
				bn_Ent::bn_EntNetData netData;
				memcpy(&netData, data, sizeof(bn_Ent::bn_EntNetData));

				ccout << "recieved net data:\n" << netData << newl;

				bn_Ent* object(0);

				switch((bn_Ent::ent_EntType)netData.enttype)
				{
				case bn_Ent::player:
					ccout << "object type is player" << newl;
					object = new bn_Player;
					break;
				case bn_Ent::pickup:
					switch((bn_Pickup::pup_PupType)netData.puptype)
					{
					case bn_Pickup::health:
						object = new bn_Health;
						break;
					default:
						// massive error!!!
						ccout << "Bad network error!\n";
						return;
						break;
					}
				//case bn_Ent::monster:
				//	object = new bn_Monster;
				//	break;
				case bn_Ent::projectile:
					object = new bn_Rocket; // this is not a general solution, should be changed
					// todo: change this
					break;
				default:
					ccout << "object type is unkown" << newl;
					return;
					break;
				}
				
				object->transform = TranslationMatrix(v3d(netData.ax, netData.ay, netData.az)) * RotationMatrix(v3d(netData.x, netData.y, netData.z));
				object->creatorAliasUid = netData.creatorAliasUid;
				object->setDeaths(netData.deaths);
				object->setHp(netData.hp);
				object->kill = netData.kill;
				object->setPoints(netData.points);
				object->setState((bn_Ent::states)netData.state);
				object->setSuicides(netData.suicides);
				object->visible = netData.visible;
//				object->velocity = v3d(netData.vx, netData.vy, netData.vz);
				uidPair temp;
				temp.localUid = aliases.getUid(object);
				temp.remoteUid = netData.uid;
				uidCrossReference.push_back(temp);				//aliases.setUid(object, netData.uid);			// should have been negotiated
				
				Video.serverSceneGraph.addObject(object);
*/
				break;
			}
		case ND_CLIENT_OBJECT_DATA:
			{
			/*	bn_Ent::bn_EntNetData netData;
				memcpy(&netData, data, sizeof(bn_Ent::bn_EntNetData));

				unsigned long localUid(0);
				for(vector<uidPair>::iterator i = uidCrossReference.begin(); i != uidCrossReference.end(); i = i++)
				{
					if(netData.uid == i->remoteUid) // found the right cross reference
					{
						localUid = i->localUid;
						break;
					}
				}

				Video.clientSceneGraph.monitor.mutexOn();
				Video.serverSceneGraph.monitor.mutexOn();

				bn_Ent* object = (bn_Ent*)aliases.getObject(localUid);

				if(!object)
				{
					Video.clientSceneGraph.monitor.mutexOff();
					Video.serverSceneGraph.monitor.mutexOff();
				//	ccout << "Received an invalid UID!\n";
					break;
				}

				if(object == theplayer)
				{
					Video.clientSceneGraph.monitor.mutexOff();
					Video.serverSceneGraph.monitor.mutexOff();
					break;
				}

				object->transform = TranslationMatrix(v3d(netData.ax, netData.ay, netData.az)) * RotationMatrix(v3d(netData.x, netData.y, netData.z));
				/*object->creatorAliasUid = netData.creatorAliasUid;
				object->setDeaths(netData.deaths);
				object->setHp(netData.hp);
				object->kill = netData.kill;
				object->setPoints(netData.points);
				object->setState((bn_Ent::states)netData.state);
				object->setSuicides(netData.suicides);
				object->visible = netData.visible;*/
//				object->velocity = v3d(netData.vx, netData.vy, netData.vz);
			
/*
				Video.clientSceneGraph.monitor.mutexOff();
				Video.serverSceneGraph.monitor.mutexOff();*/

				break;
			}
		default:
			break;
		}
	}
}

void net_Server::net_Client::sendData(const char dataType, const char* data, const short size)
{
	if(networkState != cs_connected)
	{
		ccout << "Error: no connection.\n";
		return;
	}
	if(size > 127)
	{
		ccout << "Error: sendData size parameter is too large!\n";
		return;
	}
//	sendMonitor.acquire();
	char* packedData = new char[128];
	packedData[0] = dataType;
	memcpy(&(packedData[1]), data, size);
	sendThread.monitor.mutexOn();
	sendThread.data.push(packedData);
	sendThread.monitor.mutexOff();
}

void net_Server::net_Client::getData(char& dataType, char* data) // data must be a 127 char array
{
	recieveThread.monitor.mutexOn();
	char* packedData = recieveThread.data.front();
	recieveThread.data.pop();
	recieveThread.monitor.mutexOff();
	dataType = packedData[0];
	memcpy(data, &(packedData[1]), 127);
	delete [] packedData;
//	recieveThread.dataIsNew = false;
//	recieveMonitor.release();
}

bool net_Server::net_Client::isNewDataAvailable()
{
	return !recieveThread.data.empty();
//	return recieveThread.dataIsNew;
}

void net_Server::net_Client::setSocket(SOCKET _socket)
{
	sock = _socket;
}
void net_Server::net_Client::setSockaddr(sockaddr _sockaddr)
{
	sockAddr = _sockaddr;
}



////////////////////////////////////////////////////////////////////////

net_Server::net_ClientList::net_ClientList():
head(0), 
clientCount(0)
{
}

net_Server::net_ClientList::~net_ClientList()
{
	for(net_ClientNode* i = head; i != 0; )
	{
		net_ClientNode* temp = i;
		i = i->next;
		delete temp;
	}
}

net_Server::net_Client* net_Server::net_ClientList::createNewClient(SOCKET socket, sockaddr sockaddr)
{
	clientCount++;

	net_ClientNode* newNode = new net_ClientNode(owner);
	newNode->client.setSocket(socket);
	newNode->client.setSockaddr(sockaddr);
	//newNode->client.clientSocket = socket;
	//newNode->client.clientSockaddr = sockaddr;

	if(!head)
	{
		head = newNode;
		return &(head->client);
	}
	else
	{
		for(net_ClientNode* i = head; i != 0; i = i->next )
		{
			if(i->next == 0)
			{
				i->next = newNode;
				return &(i->next->client);
			}
		}
	}
	ccout << "Error in createNewClient()\n";
	return 0; // should never reach this point
}

void net_Server::net_ClientList::deleteClient(int index)
{
	if(head == NULL)
		return;

	clientCount--;

	if(index == 0)								// remove the head
	{
		net_ClientNode* temp = head->next;
		delete head;
		head = temp;
		return;
	}
	else
	{
		int count = 1;
		net_ClientNode* prev(head);				// head is valid
		for(net_ClientNode* i = head->next; i != 0; i = i->next)
		{
			if(count == index)
			{
				prev->next = i->next;
				delete i;
				return;
			}
			else
				count++;
		}
	}
}

net_Server::net_Client& net_Server::net_ClientList::operator[] (int index)
{
	int count = 0;
	for(net_ClientNode* i = head; i != 0; i = i->next)
	{
		if(count == index)
			return i->client;
		else
			count++;
	}
	return head->client;
}

////////////////////////////////////////////////////////////////////////

net_Server::net_Server()
{
	clientList.owner = this;
	listenThread.owner = this;

	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) 
	{ 
		//WSACleanup (); 
		ccout << "Error making socket.\n";
		return; 
	} 
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(5001);
	addr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind(sock, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) 
	{ 
		//WSACleanup (); 
		ccout << "Error binding socket to addres.\n";
		return; 
	} 

	if (listen(sock,5)==SOCKET_ERROR)
	{ 
		//WSACleanup ();
		ccout << "Error listening with socket.\n";
		return;
	}

	listenThread.start();
}

net_Server::~net_Server()
{
	shutdown (sock, SD_SEND);  // clientSocket can't send anymore
	// read in remaining data here
	closesocket (sock);	

	listenThread.stop();

	while(clientList.clientCount)
		clientList.deleteClient(0);
}

int net_Server::getNumClients()
{
	return clientList.clientCount;
}

void net_Server::update()
{
	for(int i = 0; i < clientList.clientCount; i++)
	{
		if(clientList[i].networkState == cs_disconnected)
		{
			clientList.deleteClient(i);
			i = 0;				// for this to work, clientList.clientCount must be re-evaluated
		}
	}

	for(int i = 0; i < clientList.clientCount; i++)
	{
		clientList[i].update();
	}
}

void net_Server::relay( const char dataType, const char* data, const short dataSize, net_Client* source)
{
	for(int i = 0; i < clientList.clientCount; i++)
	{
		if(&clientList[i] != source)
			clientList[i].sendData(dataType, data, dataSize);
	}
}

void net_Server::broadcast(const char dataType, const char* data, const short dataSize)
{
	for(int i = 0; i < clientList.clientCount; i++)
	{
		clientList[i].sendData(dataType, data, dataSize);
	}
}

void net_Server::broadcast(const char dataType)
{
	for(int i = 0; i < clientList.clientCount; i++)
	{
		clientList[i].sendData(dataType, 0, 0);
	}
}


////////////////////////////////////////////////////////////////////////


/*DWORD WINAPI clientRecieve( LPVOID arg1 )
{
	SOCKET sock = (SOCKET) arg1;
	while(!netquit)
	{
		char recvData[256];
		if(recv(sock, recvData, sizeof(recvData), 0) == SOCKET_ERROR)
		{
			netquit = true;
			break;
		}		
		if(connectionToServer)
		{
			if(connectionToServer->recieveMessage)
				ccout << "Recieve error: data already exists...\n";
			else
			{
				connectionToServer->recieveBuffer = recvData;
				connectionToServer->recieveMessage = true;
			}
		}
//		ccout << recvData << newl;
	}	
	ccout << "Recieve connection lost.\n";
	connected = false;
	return TRUE;
}

DWORD WINAPI clientSend( LPVOID arg1 )
{
	SOCKET sock = (SOCKET) arg1;
	while(!netquit)
	{
		if(connectionToServer)
			if(connectionToServer->sendMessage)
			{
				if(send(sock, connectionToServer->sendBuffer, connectionToServer->sendBufferSize, 0) == SOCKET_ERROR)
				{
					netquit = true;
					break;
				}

				delete connectionToServer->sendBuffer;
				connectionToServer->sendBuffer = 0;
				connectionToServer->sendBufferSize = 0;
				connectionToServer->sendMessage = false;
			}
	}
	ccout << "Send connection lost.\n";
	connected = false;
	return TRUE;
}*/


////////////////////////////////////////////////////////////////////////
void NET_ClientConnect(string address)
{
	NET_Disconnect();
	ccout << "Connecting to " << address << "...\n";
	connectionToServer = new net_ConnectionToServer(address);

	if(connectionToServer)
		connectionToServer->sendData(ND_CLIENT_CONNECT, 0, 0);
}

void NET_ServerListen()
{
	NET_Disconnect();
	server = new net_Server;
	ccout << "Server is listening...\n";
}

void NET_Disconnect()
{
	if(connectionToServer)
	{
		delete connectionToServer;
		connectionToServer = 0;
	}
	if(server)
	{
		delete server;
		server = 0;
	}
}
////////////////////////////////////////////////////////////////////////
void NET_ServerSendData(const char dataType, const int clientNumber)
{
//	clientList[clientNumber].sendData(dataType, 0, 0);
}

void NET_ServerSendData(const char dataType, const char* data, const short dataSize, const int clientNumber)
{
//	clientList[clientNumber].sendData(dataType, data, dataSize);
}

void NET_ClientSendData(const char dataType)
{
	if(connectionToServer)
		connectionToServer->sendData(dataType, 0, 0);
}

void NET_ClientSendData(const char dataType, const char* data, const short dataSize)
{
	if(connectionToServer)
		connectionToServer->sendData(dataType, data, dataSize);
}


////////////////////////////////////////////////////////////////////////
CON_CMD(nettest)
{
	for(int i = 0 ; i < 10; i++)
	{
		string text;
		text = "this is the number: " + M_itoa(i);
		CON_Exec("say " + text);
	}
}

CON_CMD(say)
{
	if(!connectionToServer && !server)
	{
		ccout << "You are not connected.\n";
	}
	else
	{
		string data = "chat: " + input.unparsedtext.substr(4);
		if(server)
		{
			server->broadcast(ND_CHAT, data.c_str(), data.length()+1);
		}
		else
		{
			connectionToServer->sendData(ND_CHAT, data.c_str(), data.length()+1);
		}
	}
}



CON_CMD(listen)
{
	NET_ServerListen();
}

////////////////////////////////////////////////////////////////////////
void NET_Init()
{
	WSADATA wsaData;
	WORD wVersionRequested;
	
	int err;
 
	wVersionRequested = 0x0202;

	err = WSAStartup(wVersionRequested, &wsaData);
	if(err)
	{
		ccout << "Couldn't find a useable winsock dll.\n";
		WSACleanup( );
		return;
	}
	if ( wsaData.wVersion != wVersionRequested ) 
	{
		ccout << "Couldn't find a useable winsock dll.\n";
		WSACleanup( );
		return; 
	}

	CON_CMD_REG(nettest);
	CON_CMD_REG(say);
	CON_CMD_REG(listen);
}

DWORD net_UpdateThread::threadProc()
{
	NET_Init();
	while(running)
	{
		if(server)
		{
			server->update();
		}
		else if(connectionToServer)
		{
			connectionToServer->update();
		}
		Sleep(40);
	}
	return 0;
}

void NET_Shutdown()
{
	NET_Disconnect();
	WSACleanup();
}

int NET_GetNumClients()
{
	return server->getNumClients();
}


