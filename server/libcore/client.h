//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:40
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__client__
#define __hive__client__

#include "accept.h"

NS_HIVE_BEGIN

class Client;

class ClientHandler
{
public:
	ClientHandler(void){}
	virtual ~ClientHandler(void){}

	virtual void onReceiveClient(Client* pClient) = 0;
	virtual bool onReceivePacket(Client* pClient, Packet* pPacket) = 0;
};

class Client : public Accept
{
public:
	ClientHandler* m_pClientHandler;
public:
	Client(void);
	virtual ~Client(void);

	static Client* createObject(void){
		return new Client();
	}
	static void releaseObject(Client* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from EpollObject
	virtual bool epollActive(uint32 events);
	virtual void epollRemove(void);

	// from Accept
	virtual void resetData(void);

	bool connectServer(void);
	void setClientHandler(ClientHandler* pClientHandler){ m_pClientHandler = pClientHandler; }
	ClientHandler* getClientHandler(void){ return m_pClientHandler; }
};

typedef ObjectPool<Client> ClientPool;

NS_HIVE_END

#endif
