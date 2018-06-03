//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/12/6
// Time: 下午10:52
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__dbredis__
#define __hive__dbredis__

#include "script.h"
#include "hiredis.h"

NS_HIVE_BEGIN

/*--------------------------------------------------------------------*/

typedef const char* ConstCharPtr;

class DBRedis : public Object0824
{
public:
	friend class DBRedisManager;
	typedef std::vector<size_t> SizeTVector;
	typedef std::vector<ConstCharPtr> ConstCharPtrVector;
public:
    DBRedis(void);
    virtual ~DBRedis(void);

	static DBRedis* createObject(void){
		return new DBRedis();
	}
	static void releaseObject(DBRedis* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

    bool isAvailable(void);
	bool connectRedis(const char* ip, uint16 port);
	void closeRedis(void);
	bool identifyRedis(const char* pwd);
	int command(lua_State* tolua_S, bool skipResult);
	void appendCommand(lua_State* tolua_S);
	int getReply(lua_State* tolua_S);

    int executeCommandArgv(lua_State* tolua_S, int argc, const char **argv, const size_t *argvlen);
    bool executeCommandArgvWithoutReturn(int argc, const char **argv, const size_t *argvlen);
    inline bool checkReplyStatus(redisReply* reply);
    void checkRedisReplyData(lua_State* tolua_S, redisReply* reply);
private:
	redisContext* m_pContext;
	SizeTVector m_argvlen;
	ConstCharPtrVector m_argv;
};

typedef ObjectPool<DBRedis> DBRedisPool;

#define DEFINE_CREATE_REMOVE_REDIS()\
public:\
	DBRedisPool m_DBRedisPool;\
	DBRedis* createRedis(void){ return m_DBRedisPool.create(); }\
	bool removeRedis(DBRedis* pDBRedis){ return this->removeRedis(pDBRedis->getHandle()); }\
	bool removeRedis(uint32 handle){\
		DBRedis* pDBRedis;\
		pDBRedis = m_DBRedisPool.get(handle);\
		if(NULL != pDBRedis){\
			pDBRedis->closeRedis();\
			m_DBRedisPool.idle(handle);\
			return true;\
		}\
		return false;\
	}\

NS_HIVE_END

#endif
