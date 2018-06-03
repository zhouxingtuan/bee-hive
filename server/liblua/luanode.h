//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:13
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__luanode__
#define __hive__luanode__

#include "core.h"
#include "script.h"
#include "dbmysql.h"
#include "dbredis.h"
#include "base.h"

NS_HIVE_BEGIN

class LuaNode : public Node
{
public:
	Script* m_pScript;
	DBRedisPool* m_pDBRedisPool;
	DBMysqlPool* m_pDBMysqlPool;
	MD5Pool* m_pMD5Pool;
	HiveKVPool* m_pHiveKVPool;
	HiveBlockPool* m_pHiveBlockPool;
	HiveArrayPool* m_pHiveArrayPool;
	HiveMapPool* m_pHiveMapPool;
public:
	LuaNode(void);
	virtual ~LuaNode(void);

	// from Node
	virtual void onReceiveAccept(Accept* pAccept, Packet* pPacket);
	virtual void onCloseAccept(Accept* pAccept);
	virtual void onReceiveHttp(Http* pHttp, Packet* pPacket);
	virtual void onCloseHttp(Http* pHttp);
	virtual void onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK);
	virtual int64 onTimerUpdate(uint32 callbackID);

	void onInitialize(const std::string& param);
	void onDestroy(void);

	DEFINE_CREATE_REMOVE_REDIS()
	DEFINE_CREATE_REMOVE_MYSQL()

	DEFINE_CREATE_REMOVE_MD5()
	DEFINE_CREATE_REMOVE(HiveKVPool, HiveKV)
	DEFINE_CREATE_REMOVE(HiveBlockPool, HiveBlock)
	DEFINE_CREATE_REMOVE(HiveArrayPool, HiveArray)
	DEFINE_CREATE_REMOVE(HiveMapPool, HiveMap)

	void initializeHandler(void);
	void destroyHandler(void);
};

NS_HIVE_END

#endif
