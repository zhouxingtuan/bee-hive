//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/8/4
// Time: 上午7:11
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__node__
#define __hive__node__

#include "packet.h"
#include "util.h"
#include "client.h"
#include "https.h"
#include "multicurl.h"

NS_HIVE_BEGIN

class Node : public RefObject
{
public:
	uint32 m_nodeID;
	std::string m_logFile;
public:
	Node(void);
	virtual ~Node(void);

	virtual void onReceiveAccept(Accept* pAccept, Packet* pPacket) = 0;
	virtual void onCloseAccept(Accept* pAccept) = 0;
	virtual void onReceiveHttp(Http* pHttp, Packet* pPacket) = 0;
	virtual void onCloseHttp(Http* pHttp) = 0;
	virtual void onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK) = 0;

	virtual int64 onTimerUpdate(uint32 callbackID) = 0;						// 更新定时器
    uint32 startTimer(uint32 callbackID, int64 timeCount);			        // 开始一个计时器
    void removeTimer(uint32 handle);								        // 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				        // 更改计时器的时间
    int64 getTimerLeft(uint32 handle);									    // 获取计时器剩余时间

	bool sendPacket(Packet* pPacket);

	bool pushUser(uint32 desID, uint32 message, uint32 uid, const char* ptr, uint32 length);
	bool broadcastOnline(uint32 message, uint32 uid, const char* ptr, uint32 length);
	bool broadcast(uint32 desType, uint32 message, uint32 uid, const char* ptr, uint32 length);
	bool sendRequest(uint32 desType, uint32 desID, uint32 message, uint32 callbackID, uint32 uid, const char* ptr, uint32 length);
	bool sendResponse(uint32 desType, uint32 desID, uint32 message, uint32 callbackID, uint32 uid, const char* ptr, uint32 length);
	bool responseConnect(uint32 desType, uint32 desID, uint32 message, uint32 callbackID, uint32 uid, const char* ptr, uint32 length, uint32 connHandle);
	bool curlRequest(RequestData* pRequest);
	const char* getConnectIP(uint32 connHandle);
	bool closeConnect(uint32 connHandle);

	uint32 hash(const char* str, uint32 length);
	void encrypt(const char* str, uint32 length, std::string& result);
	void decrypt(const char* str, uint32 length, std::string& result);

	uint32 getNodeID(void){ return m_nodeID; }
	void setNodeID(uint32 nodeID){ m_nodeID = nodeID; }
	uint32 getModuleType(void) const;
	uint32 getModuleIndex(void) const;
	uint32 getModuleHandle(void) const;

	void log(const char* str);
	inline void setLogFile(const std::string& logFile){ m_logFile = logFile; }
	inline const std::string& getLogFile(void) const { return m_logFile; }
};

NS_HIVE_END

#endif
