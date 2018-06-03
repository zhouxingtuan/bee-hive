//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/7/29
// Time: 下午4:50
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__beehandler__
#define __hive__beehandler__

#include "core.h"

NS_HIVE_BEGIN

class TimerHandle : public TimerObject, public Object0824
{
public:
	TimerHandle(void) : TimerObject(), Object0824(), m_nodeID(0), m_callbackID(0){}
	virtual ~TimerHandle(void){}

	static TimerHandle* createObject(void){
		TimerHandle* pHandle = new TimerHandle();
		return pHandle;
	}
	static void releaseObject(TimerHandle* pHandle){
		if(NULL != pHandle){
			delete pHandle;
		}
	}

	// from TimerObject
	virtual int64 timerCallback(void);

	inline uint32 getCallbackID(void) const { return m_callbackID; }
	inline void setCallbackID(uint32 id){ m_callbackID = id; }
	void setActiveObject(uint32 handlerID){ m_nodeID = handlerID; }
public:
	uint32 m_nodeID;                    // 返回的节点ID
	uint32 m_callbackID;				// 返回的callback ID
};

typedef ObjectPool<TimerHandle> TimerHandlePool;

typedef Node* (*NodeCreateFunction)(uint32 nodeID, const std::string& param, const std::string& logFile);
typedef void (*NodeDestroyFunction)(Node* pNode);
typedef void(*SOInitializeFunction)(void);
typedef void(*SODestroyFunction)(void);

#define CHECK_NODE_CONNECT_TIME 1000

typedef std::vector<uint32> RefVector;
typedef std::unordered_map<uint32, Node*> NodeMap;
typedef std::vector<Packet*> PacketVector;

class BeeHandler : public Handler, public TimerObject
{
public:
	// discovery node information
	std::string m_hiveIP;
	uint16 m_hivePort;
	bool m_hiveEncrypt;
	bool m_hiveDecrypt;

	// global address
	std::string m_globalIP;
	uint16 m_globalPort;
	bool m_globalEncrypt;
	bool m_globalDecrypt;

	// http address
	std::string m_httpIP;
	uint16 m_httpPort;

	// https address
	std::string m_httpsIP;
	uint16 m_httpsPort;

	// so name
	std::string m_moduleName;
	std::string m_moduleParam;

	uint32 m_hiveHandle;        // 连接的节点的ID
	Client* m_pHiveClient;      // 连接hive的socket连接

	// Timer池
	TimerHandlePool* m_pTimerHandlePool;			// 计时器对象池

	// SO指针
	void* m_pHandle;
	SOInitializeFunction SOInitialize;
	SODestroyFunction SODestroy;
	NodeCreateFunction NodeCreate;
	NodeDestroyFunction NodeDestroy;

	RefVector m_refIndex;       // 引用下标数组
	NodeMap m_nodeMap;
	PacketVector m_packets;
public:
	BeeHandler(void);
	virtual ~BeeHandler(void);

	DEFINE_INSTANCE_H(BeeHandler)

	// from Handler
	virtual void onReceiveHttp(Http* pHttp, Packet* pPacket);
	virtual bool onReceiveAccept(Accept* pAccept, Packet* pPacket);
	virtual void onReceiveClient(Client* pClient);
	virtual void onCurlResponse(Buffer* pBuffer, uint32 nodeID, uint32 callbackID, bool isRequestOK);
	virtual void onCloseAccept(Accept* pAccept);
	virtual void onCloseClient(Client* pClient);
	virtual void onCloseHttp(Http* pHttp);
	virtual void onCloseListener(Listener* pListener);
	virtual bool dispatchMessage(Packet* pPacket);
    virtual uint32 startTimer(uint32 callbackID, int64 timeCount, uint32 nodeID); // 开始一个计时器
    virtual void removeTimer(uint32 handle);								// 移除计时器
    virtual void changeTimer(uint32 handle, int64 timeCount);				// 更改计时器的时间
    virtual int64 getTimerLeft(uint32 handle);								// 获取计时器剩余时间

	// from TimerObject
	virtual int64 timerCallback(void);

    void onReceivePacketFromClient(Accept* pAccept, Packet* pPacket);
	void broadcastPacket(Accept* pAccept, Packet* pPacket);
	void onInitialize(void);
	void identifyHive(Accept* pAccept);
	void setHiveHandle(uint32 hiveHandle){ m_hiveHandle = hiveHandle; }
	void setModuleName(const std::string& moduleName){ m_moduleName = moduleName; }
	void setModuleParam(const std::string& moduleParam){ m_moduleParam = moduleParam; }

	void writeRefIndex(Packet* pPacket);
	void kickoffAllNode(void);
	bool dispatchPacket(Packet* pPacket);
	Node* findNode(DestinationHandle destination);
	Node* findNodeByID(uint32 nodeID);
	Node* randomNode(void);


	TimerHandle* createTimerHandle(void){
		return m_pTimerHandlePool->create();
	}
	TimerHandle* getTimerHandle(uint32 handle){
		return m_pTimerHandlePool->get(handle);
	}
	void removeTimerHandle(TimerHandle* pHandle){
		pHandle->clearTimer();
    	pHandle->setActiveObject(0);	// 释放占用
		m_pTimerHandlePool->idle(pHandle);
	}
protected:
	void updateCheckPackets(void);
	void updateCheckNodeConnect(void);
	void openGlobalListener(void);
	void openHttpListener(void);
	void openHttpsListener(void);
	bool initializeNode(void);
};

NS_HIVE_END

#endif
