//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/7/29
// Time: 下午4:50
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__hivehandler__
#define __hive__hivehandler__

#include "core.h"

NS_HIVE_BEGIN

#define CHECK_NODE_CONNECT_TIME 1000

#define MAX_HIVE_NUMBER 1024
#define MAX_TYPE_INDEX 1024
#define MAX_NODE_INDEX 10240

typedef struct HiveInformation{
    uint32 id;
    char ip[IP_SIZE];//192.168.110.110 --> 16 byte
    uint16 port;
    bool encrypt;
    bool decrypt;
    HiveInformation(void){
        reset();
    }
    ~HiveInformation(void){}
    void set(const char* ptr){
        memcpy((char*)get(), ptr, sizeof(HiveInformation));
    }
    const char* get(void) const {
        return ((char*)(&id));
    }
    void set(uint32 id, const char* ip, uint16 port, bool encrypt, bool decrypt){
        this->id = id;
        memcpy(this->ip, ip, IP_SIZE);
        this->port = port;
        this->encrypt = encrypt;
        this->decrypt = decrypt;
    }
    void reset(void){
        memset((char*)get(), 0, sizeof(HiveInformation));
    }
	inline bool operator==(const HiveInformation& h) const{
		return (memcmp(get(), (h.get()), sizeof(HiveInformation)) == 0);
	}
	inline HiveInformation& operator=(const HiveInformation& h){
		set(h.get());
		return *this;
	}
}HiveInformation;

typedef struct DestinationRoute{
	DestinationHandle destination;  // 目标服务的类型和下标
	uint32 nodeID;                  // 目标服务所在的节点ID
	Accept* pAccept;                // 目标服务的连接，不在当前节点则为NULL；没有建立连接也为NULL
	DestinationRoute(void) : destination(0), nodeID(0), pAccept(NULL){}
	~DestinationRoute(void){}
}DestinationRoute;

typedef std::vector<HiveInformation> HiveInformationVector;
typedef std::vector<Client*> HiveClientVector;
typedef std::vector<Accept*> HiveAcceptVector;
typedef std::map<uint32, uint32> HandleToID;

typedef std::vector<DestinationRoute> DestinationRouteVector;
typedef std::vector<DestinationRouteVector> TypeVector;
typedef std::vector<uint16> RefIndexVector;
typedef std::vector<RefIndexVector> TypeRefIndexVector;
typedef std::vector<uint32> RefVector;

typedef struct RefIndexInfo{
	uint16 moduleType;
	uint16 moduleIndex;
	uint32 refIndex;
}RefIndexInfo;

class HiveHandler : public Handler, public TimerObject
{
public:
	// discovery node information
	uint32 m_destID;
	std::string m_destIP;
	uint16 m_destPort;
	bool m_destEncrypt;
	bool m_destDecrypt;

	// local address
	std::string m_localIP;
	uint16 m_localPort;
	bool m_localEncrypt;
	bool m_localDecrypt;

	// global address
	std::string m_globalIP;
	uint16 m_globalPort;
	bool m_globalEncrypt;
	bool m_globalDecrypt;

	HiveInformationVector m_hiveNodes;
	HandleToID m_handleToNode;
	HiveClientVector m_hiveClients;
	HandleToID m_acceptHandleToNode;
	HiveAcceptVector m_hiveAccepts;

	HandleToID m_handleToDestination;
	TypeVector m_destinations;
	TypeRefIndexVector m_typeRefIndex;
public:
	HiveHandler(void);
	virtual ~HiveHandler(void);

	DEFINE_INSTANCE_H(HiveHandler)

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
    virtual uint32 startTimer(uint32 callbackID, int64 timeCount, uint32 nodeID){ return 0; };
    virtual void removeTimer(uint32 handle){};
    virtual void changeTimer(uint32 handle, int64 timeCount){};
    virtual int64 getTimerLeft(uint32 handle){ return -1; };

	// from TimerObject
	virtual int64 timerCallback(void);

	void onInitialize(void);
	void identifyHive(Accept* pAccept);
	void notifyBeeOffline(DestinationHandle destination);

	// 广播给除了自己之外的所有节点
	void broadcastHive(Packet* pPacket);
	// 广播给所有bee节点
	void broadcastDestination(uint32 moduleType, Packet* pPacket, bool shouldBroadcastHive);
	// 单独发送给某个节点
	bool sendToDestination(DestinationHandle destination, Packet* pPacket);

	void recordHiveInfo(Packet* pPacket);
	void writeHiveInfo(Packet* pPacket);
	void writeModuleInfo(Packet* pPacket);
	void recordModuleInfo(Packet* pPacket);
	void writeRefIndex(Packet* pPacket);
	void recordRefIndex(Packet* pPacket);

	void registerRefIndex(DestinationHandle destination, uint32 refIndex);
	DestinationRoute* getDestinationRouteByRefIndex(DestinationHandle destination);
	RefIndexVector* getRefIndexVector(uint32 moduleType);

	void registerDestination(DestinationHandle destination, uint32 nodeID, Accept* pAccept);
	void unregisterDestination(DestinationHandle destination, bool notifyKickOff);
	void resetDestination(DestinationHandle destination);
	DestinationRoute* getDestinationRoute(DestinationHandle destination);
	DestinationRouteVector* getDestinationRouteVector(uint32 moduleType);

	void notifyBeeKickoff(Accept* pAccept, DestinationHandle destination);

	bool registerNode(const char* ptr);
	bool registerNode(uint32 id, const char* ip, uint16 port, bool encrypt, bool decrypt);
	bool registerNode(const HiveInformation& regInfo);
	bool unregisterNode(uint32 id);
	void setNodeClient(uint32 id, Client* pClient);
	Client* getNodeClient(uint32 id);
	uint32 getNodeIDByConnectHandle(uint32 connectHandle);
	void setNodeAccept(uint32 id, Accept* pAccept);
	Accept* getNodeAccept(uint32 id);
protected:
	bool sendToDestinationByRoute(DestinationRoute* pRoute, Packet* pPacket);
	void updateCheckNodeConnect(void);
	void checkNodeConnect(uint32 id);
	void openLocalListener(void);
	void openGlobalListener(void);
};

NS_HIVE_END

#endif
