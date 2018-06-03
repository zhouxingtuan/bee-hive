//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/7
// Time: 下午9:30
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__handler__
#define __hive__handler__

#include "packet.h"
#include "util.h"
#include "client.h"
#include "https.h"
#include "listener.h"

NS_HIVE_BEGIN

class Handler : public RefObject
{
public:
	DestinationHandle m_moduleHandle;
    std::string m_logFile;
public:
	Handler(void);
	virtual ~Handler(void);

	// 连接相关的接口
	virtual void onReceiveHttp(Http* pHttp, Packet* pPacket) = 0;
	virtual bool onReceiveAccept(Accept* pAccept, Packet* pPacket) = 0;
	virtual void onReceiveClient(Client* pClient) = 0;
	virtual void onCurlResponse(Buffer* pBuffer, uint32 nodeID, uint32 callbackID, bool isRequestOK) = 0;
	virtual void onCloseAccept(Accept* pAccept) = 0;
	virtual void onCloseClient(Client* pClient) = 0;
	virtual void onCloseHttp(Http* pHttp) = 0;
	virtual void onCloseListener(Listener* pListener) = 0;
	virtual bool dispatchMessage(Packet* pPacket) = 0;
    virtual uint32 startTimer(uint32 callbackID, int64 timeCount, uint32 nodeID) = 0;
    virtual void removeTimer(uint32 handle) = 0;
    virtual void changeTimer(uint32 handle, int64 timeCount) = 0;
    virtual int64 getTimerLeft(uint32 handle) = 0;

	inline void setModuleType(uint32 moduleType){ m_moduleHandle.type = moduleType; }
	inline void setModuleIndex(uint32 moduleIndex){ m_moduleHandle.index = moduleIndex; }
	inline uint32 getModuleType(void) const { return m_moduleHandle.type; }
	inline uint32 getModuleIndex(void) const { return m_moduleHandle.index; }
	inline void setModuleHandle(uint32 handle) { m_moduleHandle.handle = handle; }
	inline uint32 getModuleHandle(void) const { return m_moduleHandle.handle; }
	inline void setLogFile(const std::string& logFile){ m_logFile = logFile; }
	inline const std::string& getLogFile(void) const { return m_logFile; }
};// end class Handler

NS_HIVE_END

#endif
