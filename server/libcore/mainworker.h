//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/7/28
// Time: 上午9:14
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__mainworker__
#define __hive__mainworker__

#include "common.h"
#include "epoll.h"
#include "timer.h"
#include "log.h"
#include "packet.h"
#include "accept.h"
#include "client.h"
#include "http.h"
#include "https.h"
#include "listener.h"
#include "multicurl.h"
#include "handler.h"

NS_HIVE_BEGIN

#define CONNECT_IDENTIFY_TIME 5000
#define CONNECT_ONLINE_TIME 120000
#define CONNECT_KEEP_ONLINE_TIME 10000
#define HTTP_HANDLE_TIMEOUT 30000

#define COMMAND_CLIENT_REQUEST 1
#define COMMAND_HTTP_REQUEST 2
#define COMMAND_NODE_REQUEST 3
#define COMMAND_NODE_RESPONSE 4
#define COMMAND_NODE_ERROR 5
#define COMMAND_REQUEST_TIMEOUT 6
#define COMMAND_BROADCAST 7
#define COMMAND_BROADCAST_BEGIN 8

#define COMMAND_PING 255
#define COMMAND_PONG 254
#define COMMAND_REGISTER 253
#define COMMAND_RESPONSE 252
#define COMMAND_HIVE_REGISTER 251
#define COMMAND_HIVE_RESPONSE 250
#define COMMAND_BEE_REGISTER 249
#define COMMAND_BEE_RESPONSE 248
#define COMMAND_BEE_ONLINE 247
#define COMMAND_BEE_OFFLINE 246
#define COMMAND_BEE_KICKOFF 245

#define NET_KEY_LENGTH 16
#define COMMAND_NUMBER 256
#define INVALID_COMMAND (uint32)(-1)
typedef void (*AcceptCommandFunction)(Accept* pAccept, Packet* pPacket, uint32 command);

typedef struct PacketQueueInfo{
    Accept* pAccept;
    Packet* pPacket;
}PacketQueueInfo;

class MainWorker : public RefObject
{
public:
    typedef std::deque<PacketQueueInfo> PacketQueue;
public:
	char m_pReadBuffer[EPOLL_READ_BUFFER_SIZE+PACKET_HEAD_LENGTH];
	Epoll* m_pEpoll;
	Timer* m_pTimer;
	Handler* m_pHandler;
	ListenerPool* m_pListenerPool;
	AcceptPool* m_pAcceptPool;
	ClientPool* m_pClientPool;
	HttpPool* m_pHttpPool;
	HttpsPool* m_pHttpsPool;
	MultiCurl* m_pMultiCurl;
	SSL_CTX* m_pSSLCTX;
    http_parser_settings m_settings;
	AcceptCommandFunction m_commandArr[COMMAND_NUMBER];
	char m_key[NET_KEY_LENGTH];					// 密钥
	std::string m_password;						// 服务间连接验证密码
	std::string m_publicKey;					// 公共密钥
	std::string m_privateKey;					// 私钥
    PacketQueue m_packetQueue;
public:
	MainWorker(void);
	virtual ~MainWorker(void);

	DEFINE_INSTANCE_H(MainWorker)

	static void onAcceptSocket(int fd, const char* ip, uint16 port, Listener* pListener);
	static void onAcceptHttp(int fd, const char* ip, uint16 port, Listener* pListener);
	static void onAcceptHttps(int fd, const char* ip, uint16 port, Listener* pListener);
	static int64 checkConnectIdentify(Accept* pAccept);
	static int64 checkConnectOnline(Accept* pAccept);
	static int64 keepConnectOnline(Accept* pAccept);

	void onReceiveHttp(Http* pHttp, Packet* pPacket);
	Http* openHttp(int fd, const char* ip, uint16 port);
	Http* getHttp(uint32 handle);
	bool closeHttp(Http* pHttp);
	Https* openHttps(int fd, const char* ip, uint16 port);
	Https* getHttps(uint32 handle);
	bool closeHttps(Https* pHttps);

    void pushPacketToQueue(Accept* pAccept, Packet* pPacket);
    void dispatchPacket(void);

	bool onReceiveAccept(Accept* pAccept, Packet* pPacket);
	Client* openClient(const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt);
	Client* getClient(uint32 handle);
	void receiveClient(Client* pClient);
	bool closeClient(Client* pClient);
	Accept* openAccept(int fd, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt);
	Accept* getAccept(uint32 handle);
	bool closeAccept(Accept* pAccept);

	Listener* openHttpListener(const char* ip, uint16 port);
	Listener* openHttpsListener(const char* ip, uint16 port);
	Listener* openSocketListener(const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt);
	Listener* openListener(const char* ip, uint16 port, AcceptSocketFunction pFunc, bool isNeedEncrypt, bool isNeedDecrypt);
	Listener* getListener(uint32 handle){
		return (Listener*)m_pListenerPool->get(handle);
	}
	bool closeListener(Listener* pListener);

	bool sendCurlRequest(RequestData* pRequest);
	void onCurlResponse(Buffer* pBuffer, uint32 nodeID, uint32 callbackID, bool isRequestOK);

	bool dispatchMessage(Packet* pPacket);

    uint32 startTimer(uint32 callbackID, int64 timeCount, uint32 nodeID); // 开始一个计时器
    void removeTimer(uint32 handle);								// 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				// 更改计时器的时间
    int64 getTimerLeft(uint32 handle);								// 获取计时器剩余时间

	void update(void);
	void initialize(Handler* pHandler);
	void destroy(void);
	void initializeHttp(void);
	void initializeHttps(void);
	bool initHttpsCertificate(const char* publicKey, const char* privateKey);
	inline void setHandler(Handler* pHandler){ m_pHandler = pHandler; }
	inline Handler* getHandler(void){ return m_pHandler; }
	inline http_parser_settings* getSettings(void) { return &m_settings; }
	inline MultiCurl* getMultiCurl(void){ return m_pMultiCurl; }
	inline char* getReadBuffer(void){ return m_pReadBuffer; }
	inline Epoll* getEpoll(void){ return m_pEpoll; }
	inline Timer* getTimer(void){ return m_pTimer; }
	inline void setAcceptCommandFunction(uint32 command, AcceptCommandFunction func){
		m_commandArr[command] = func;
	}
	inline AcceptCommandFunction getAcceptCommandFunction(uint32 command){
		return m_commandArr[command];
	}
	inline bool setKey(const char* key){
		if(strlen(key) < NET_KEY_LENGTH){
			return false;
		}
		memcpy(m_key, key, NET_KEY_LENGTH);
		return true;
	}
	inline const char* getKey(void) const { return m_key; }
	inline void setPassword(const std::string& pwd){ m_password = pwd; }
	inline const std::string& getPassword(void) const { return m_password; }
	inline void setPublicKey(const std::string& key){ m_publicKey = key; }
	inline const std::string& getPublicKey(void) const { return m_publicKey; }
	inline void setPrivateKey(const std::string& key){ m_privateKey = key; }
	inline const std::string& getPrivateKey(void) const { return m_privateKey; }
};

NS_HIVE_END

#endif
