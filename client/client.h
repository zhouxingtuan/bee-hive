//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/5
// Time: 下午3:59
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__client__
#define __hive__client__

//#define NS_HIVE_BEGIN namespace hivenet {
//#define NS_HIVE_END };
//#define USING_NS_HIVE using namespace hivenet;
#define NS_HIVE_BEGIN
#define NS_HIVE_END
#define USING_NS_HIVE

//c libs
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>  //atof函数
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>

//std libs
#include <atomic>
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>

using namespace std;

//socket & net libs
#include <errno.h>
#ifdef WIN32

#include<winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET HSocket;

#else

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>		/* basic system data types */
#include <sys/socket.h>		/* basic socket definitions */
#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>		/* inet(3) functions */
#include <fcntl.h>			/* nonblocking */

typedef int HSocket;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET 0

#endif

//thread libs
#include <thread>
#include <mutex>

#define SAFE_DELETE(ptr) if(ptr != NULL){ delete ptr; ptr = NULL; }
#define SAFE_DELETE_ARRAY(ptr) if(ptr != NULL){ delete [] ptr; ptr = NULL; }
#define SAFE_RETAIN(ptr) if(ptr != NULL){ ptr->retain(); }
#define SAFE_RELEASE(ptr) if(ptr != NULL){ ptr->release(); ptr = NULL; }

NS_HIVE_BEGIN

/*--------------------------------------------------------------------*/
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif
//#ifndef uint64_t
//typedef unsigned long long int uint64_t;
//#endif
//#define BINARY_HASH_SEED 5381
//uint64_t binary_murmur_hash64A( const void * key, int len, unsigned int seed=BINARY_HASH_SEED );	// 64-bit hash for 64-bit platforms
//#define binary_hash64 binary_murmur_hash64A
uint32_t binary_djb_hash(const char* cstr, unsigned int length);

/*--------------------------------------------------------------------*/
// 拥有同步互斥量的类

class Sync
{
public:
	Sync(void){
		
	}
	virtual ~Sync(void){
		
	}
	inline void lock(void){
		m_mutex.lock();//加锁
	}
	inline void rlock(void){
		m_mutex.lock();//加锁
	}
	inline void unlock(void){
		m_mutex.unlock();
	}
protected:
	std::mutex m_mutex;
};
/*--------------------------------------------------------------------*/
// 引用计数基类
class RefObject
{
public:
	RefObject(void) : m_referenceCount(0){}
	virtual ~RefObject(void){}
	inline void release(void){
		if( std::atomic_fetch_sub_explicit(&m_referenceCount, 1, std::memory_order_relaxed) == 1 ){
			delete this;
		}
	}
	inline void retain(void){
		std::atomic_fetch_add_explicit(&m_referenceCount, 1, std::memory_order_relaxed);
	}
	inline int getRefCount(void){ return (int)m_referenceCount; }
protected:
	std::atomic<int> m_referenceCount;
};
/*--------------------------------------------------------------------*/
// 线程基类
class Thread
{
public:
	typedef void*(*ThreadCallback)(void* pData);
public:
	Thread(void) {}
	virtual ~Thread(void){}
	bool startThread(void);
	void cancelThread(void);
	virtual int threadFunction(void) = 0;
private:
	static void staticThreadFunction(void* pData);
protected:
	
};
/*--------------------------------------------------------------------*/
typedef std::vector<char> CharVector;

class Buffer : public RefObject, public CharVector
{
protected:
	std::atomic_flag m_encryptFlag;
public:
	explicit Buffer(int length);
	virtual ~Buffer(void);

	inline int write(const void* ptr, int length, int offset){
		size_t needSize = offset + length;
		if( this->size() < needSize ){
			this->resize(needSize, 0);
		}
		char* writeTo = (char*)(this->data()) + offset;
		memcpy(writeTo, ptr, length);
		return length;
	}
	inline int read(void* ptr, int length, int offset){
		int needSize = offset + length;
		if( (int)this->size() < needSize){
			return 0;
		}
		char* readFrom = (char*)(this->data()) + offset;
		memcpy(ptr, readFrom, length);
		return length;
	}
	template<typename T>
	inline T to(int offset) const {
		return *((T*)(this->data()+offset));
	}
	inline bool checkEncryptFlag(void){ return !m_encryptFlag.test_and_set(); }
	inline void clearEncryptFlag(void){ m_encryptFlag.clear(); }
};// end class Buffer
/*--------------------------------------------------------------------*/
typedef struct DestinationHandle {
	union{
		struct{
			unsigned int type   : 16;   // 目标服务类型
			unsigned int index  : 16;   // 目标服务下标
		};
		unsigned int handle     : 32;	// 句柄
	};
public:
	DestinationHandle(unsigned int t, unsigned int i) : type(t), index(i){}
	DestinationHandle(unsigned int h) : handle(h) {}
	DestinationHandle(void) : handle(0) {}
	~DestinationHandle(void){}

	inline unsigned int getType(void) const { return this->type; }
	inline unsigned int getIndex(void) const { return this->index; }
	inline unsigned int getHandle(void) const { return this->handle; }
	inline void setType(unsigned int t){ this->type = t; }
	inline void setIndex(unsigned int index){ this->index = index; }
	inline void setHandle(unsigned int handle){ this->handle = handle; }
	inline bool operator==(unsigned int h) const { return (this->handle == h); }
	inline bool operator==(const DestinationHandle& h) const{ return (this->handle == h.handle); }
	inline bool operator<(unsigned int h) const { return (this->handle < h); }
	inline bool operator<(const DestinationHandle& h) const { return (this->handle < h.handle); }
	inline DestinationHandle& operator=(unsigned int h){ this->handle = h; return *this; }
	inline DestinationHandle& operator=(const DestinationHandle& h){ this->handle = h.handle; return *this; }
}DestinationHandle;
// 服务间传递消息的头部数据结构
typedef struct PacketHead {
	int length;                     // 4 包的长度，传递的数据在8M以内
	unsigned int command;           // 4 消息的类型
	unsigned int callback;          // 4 返回的回调ID
	DestinationHandle destination;  // 4 目标服务
	DestinationHandle source;       // 4 源头服务
	unsigned int message;           // 4 当前携带的命令字段
	unsigned int uid;               // 4 玩家的uid
} PacketHead;

#define PACKET_HEAD_LENGTH_PROTECT 16
#define PACKET_HEAD_LENGTH sizeof(PacketHead)

class Packet : public RefObject
{
public:
    explicit Packet(int bufferSize);
    explicit Packet(Buffer* pBuffer);
    virtual ~Packet(void);

    void convertHead(void);
    void reverseHead(void);

	inline void setCursorToEnd(void){ m_cursor = getLength(); }
	inline void setCursor(int cur){ m_cursor = cur; }
	inline void moveCursor(int length){ m_cursor += length; }
	inline int getCursor(void) const { return m_cursor; }
	inline void resetCursor(void){ m_cursor = 0; }
	inline Buffer* getBuffer(void){ return m_pBuffer; }
	inline PacketHead* getHead(void){ return (PacketHead*)(getDataPtr()); }	// 该指针在write调用后可能会变化
	inline char* getOffsetPtr(int offset){ return m_pBuffer->data() + offset; }
	inline char* getCursorPtr(void){ return m_pBuffer->data() + m_cursor; }
	inline char* getDataPtr(void){ return m_pBuffer->data(); }
	inline int getLength(void) const { return (int)m_pBuffer->size(); }
	inline int getLengthInHead(void) { return getHead()->length; }
	inline bool isCursorEnd(void) const { return getCursor() >= getLength(); }
	inline bool isReceiveEnd(void) { return getCursor() >= getHead()->length; }
	inline char* getBody(void){ return getOffsetPtr(PACKET_HEAD_LENGTH); }
	inline int getBodyLength(void) const { return (getLength() - PACKET_HEAD_LENGTH); }

	inline int write(const void* ptr, int length){
		int n = m_pBuffer->write(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}
	inline int read(void* ptr, int length){
		int n = m_pBuffer->read(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}

	inline void readBegin(void){
		setCursor((int)PACKET_HEAD_LENGTH);
	}
	inline void readEnd(void){
		resetCursor();
	}

	inline void writeBegin(unsigned int command){
		m_pBuffer->clear();
		m_pBuffer->resize(PACKET_HEAD_LENGTH, 0);
		setCursor((int)PACKET_HEAD_LENGTH);
        setCommand(command);
	}
	inline void writeEnd(void){
		recordLength();
	}
	inline void recordLength(void){ getHead()->length = getLength(); }
	inline void setCommand(unsigned int cmd){ getHead()->command = cmd; }
	inline void setDestination(unsigned int handle){ getHead()->destination = handle; }
	inline void setDestination(unsigned int moduleType, unsigned int moduleIndex){
		DestinationHandle handle(moduleType, moduleIndex);
		setDestination(handle.handle);
	}
	inline void setSource(unsigned int source) { getHead()->source = source; }
	inline void setSource(unsigned int moduleType, unsigned int moduleIndex){
		DestinationHandle handle(moduleType, moduleIndex);
		setSource(handle.handle);
	}
	inline void setMessage(unsigned int message) { getHead()->message = message; }
	inline void setUID(unsigned int uid) { getHead()->uid = uid; }
	inline void setCallback(unsigned int callback) { getHead()->callback = callback; }
	inline unsigned int getCommand(void) { return getHead()->command; }
	inline unsigned int getDestination(void) { return getHead()->destination.handle; }
	inline unsigned int getSource(void) { return getHead()->source.handle; }
	inline unsigned int getMessage(void) { return getHead()->message; }
	inline unsigned int getUID(void) { return getHead()->uid; }
	inline unsigned int getCallback(void) { return getHead()->callback; }
protected:
	Buffer* m_pBuffer;		// 数据指针
	int m_cursor;			// 数据读取的偏移
};// end class Packet
/*--------------------------------------------------------------------*/
#define CLIENT_SELECT_TIMEOUT 30
#define IP_SIZE 18
class Client;
class ClientInterface
{
public:
	ClientInterface(void){}
	virtual ~ClientInterface(void){}

	virtual void notifyConnectServerFailed(Client* pClient) = 0;
	virtual void notifyConnectServerSuccess(Client* pClient) = 0;
	virtual void notifyConnectOut(Client* pClient) = 0;
	virtual void notifyPacketIn(Client* pClient, Packet* pPacket) = 0;
	virtual void notifyIdentifyServerFailed(Client* pClient, Packet* pPacket) = 0;
	virtual void notifyIdentifyServerSuccess(Client* pClient, Packet* pPacket) = 0;
};// end class ClientInterface

enum ClientEventType{
	CLIENT_EVENT_NONE = 0,
	CLIENT_EVENT_CONN_FAILED = 1,
	CLIENT_EVENT_CONN_SUCCESS = 2,
	CLIENT_EVENT_CONN_OUT = 3,
	CLIENT_EVENT_PACKET_IN = 4,
	CLIENT_EVENT_IDENTIFY_FAILED = 5,
	CLIENT_EVENT_IDENTIFY_SUCCESS = 6,
};

typedef struct ClientEvent{
	ClientEventType event;
	Packet* pPacket;
}ClientEvent;

#define CONNECT_KEEP_ONLINE_TIME 5

#define COMMAND_CLIENT_REQUEST 1
#define COMMAND_NODE_REQUEST 3
#define COMMAND_NODE_RESPONSE 4
#define COMMAND_BROADCAST 7

#define COMMAND_PING 255
#define COMMAND_PONG 254
#define COMMAND_REGISTER 253
#define COMMAND_RESPONSE 252

class Client : public RefObject, public Sync, public Thread
{
public:
	typedef std::deque<Packet*> PacketQueue;
	typedef std::deque<ClientEvent> ClientEventQueue;
	typedef std::deque<int> EventQueue;
public:
	Client(void);
	virtual ~Client(void);
	virtual int threadFunction(void);

	virtual void dispatchEvent(void);	// 这个函数需要在主循环中调用，用来分发事件
	virtual void updateEvent(void);     // 单线程时在主线程进行update操作

	// 6 <= command <= 255
	virtual bool sendData(unsigned int message, unsigned int callbackID, unsigned int uid, const char* pData, unsigned int length);
	virtual void reconnectSocket(void);
	virtual bool receivePacket(Packet* pPacket);
	virtual void removeSocket(void);
	virtual void setNotifyInterface(ClientInterface* pInterface){ m_pInterface = pInterface; }
	virtual inline void setSocket(const char* ip, unsigned short port){
		//strcpy(m_ip, ip);
		m_ip = ip;
		m_port = port;
	}
	virtual inline void setSocketFD(HSocket fd){ m_fd = fd; }
	virtual inline HSocket getSocketFD(void) const { return m_fd; }
	inline bool isNeedEncrypt(void) const { return m_isNeedEncrypt; }
	inline void setIsNeedEncrypt(bool need) { m_isNeedEncrypt = need; }
	inline bool isNeedDecrypt(void) const { return m_isNeedDecrypt; }
	inline void setIsNeedDecrypt(bool need) { m_isNeedDecrypt = need; }
	inline void setIsNeedReconnect(bool need) { m_isNeedReconnect = need; }
	inline bool isNeedReconnect(void) { return m_isNeedReconnect; }
	inline void setKey(const std::string& key) { m_key = key; }
	inline const std::string& getKey(void) const { return m_key; }
	inline void setPassword(const std::string& password){ m_password = password; }
	inline const std::string& getPassword(void) const { return m_password; }
	void startConnectEvent(void);
	void closeConnectEvent(void);
	void dispatchConnectEvent(void);
protected:
	virtual bool connectServer(void);
	virtual bool trySelectSocket(void);
	virtual bool tryReadSocket(void);
	virtual bool tryWriteSocket(void);
	virtual bool readSocket(void);
	virtual bool writePacket(void);
	virtual bool writeSocket(Packet* pPacket);
	virtual void closeSocket(void);
	virtual void addClientEvent(ClientEventType event, Packet* pPacket);
	void identifyHive(void);
	void checkAndPingServer(void);
	void dispatchPacket(Packet* pPacket);
	void releasePacket(void);
	bool onParsePacket(char* recvBuffer, int nread);
protected:
    HSocket m_fd;
	std::string m_ip;
    //char m_ip[IP_SIZE];//192.168.110.110
    unsigned short m_port;
	bool m_isNeedEncrypt;			// 是否需要解密
	bool m_isNeedDecrypt;			// 是否需要加密
	bool m_isNeedReconnect;			// 是否需要发起重连
	int m_pingTime;                 // ping服务器的时间
	std::string m_key;				// 网络秘钥
	std::string m_password;         // 连接密码
	PacketQueue m_packetQueue;
	ClientEventQueue m_clientEventQueue;
	ClientEventQueue m_tempEventQueue;
	EventQueue m_threadEventQueue;
	EventQueue m_tempThreadEventQueue;
	int m_currentConnectState;
	Packet* m_tempReadPacket;
	ClientInterface* m_pInterface;
    char m_tempLength;
    char m_tempHead[PACKET_HEAD_LENGTH];
};//end class Client
/*--------------------------------------------------------------------*/

NS_HIVE_END

#endif
