//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/7/9
// Time: 上午11:39
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__listener__
#define __hive__listener__

#include "epoll.h"
#include "objectpool.h"

NS_HIVE_BEGIN


#define MAX_LISTEN_SIZE 100000

class Listener;

typedef void (*AcceptSocketFunction)(int fd, const char* ip, uint16 port, Listener* pListener);

class Listener : public EpollConnectObject, public Object1616
{
public:
	Listener(void);
	virtual ~Listener(void);

	static Listener* createObject(void){
		return new Listener();
	}
	static void releaseObject(Listener* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from EpollObject
	virtual bool epollActive(uint32 events);
	virtual bool epollIn(void);
	virtual bool epollOut(void);
	virtual void epollRemove(void);
	virtual void epollCheck(void);

	bool openSocket(void);
	void setAcceptSocketFunction(AcceptSocketFunction pFunc){ m_acceptSocketFunction = pFunc; }
	AcceptSocketFunction getAcceptSocketFunction(void){ return m_acceptSocketFunction; }
	void resetData(void);
	inline bool isNeedEncrypt(void) const { return m_isNeedEncrypt; }
	inline void setIsNeedEncrypt(bool need) { m_isNeedEncrypt = need; }
	inline bool isNeedDecrypt(void) const { return m_isNeedDecrypt; }
	inline void setIsNeedDecrypt(bool need) { m_isNeedDecrypt = need; }
protected:
	AcceptSocketFunction m_acceptSocketFunction;
	bool m_isNeedEncrypt;			// 是否需要解密
	bool m_isNeedDecrypt;			// 是否需要加密
};

typedef ObjectPool<Listener> ListenerPool;

NS_HIVE_END

#endif
