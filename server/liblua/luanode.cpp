//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:14
// To change this template use File | Settings | File Templates.
//

#include "luanode.h"

NS_HIVE_BEGIN

LuaNode::LuaNode(void) : Node(), m_pScript(NULL) {

}
LuaNode::~LuaNode(void){
	destroyHandler();
}

void LuaNode::onReceiveAccept(Accept* pAccept, Packet* pPacket){
	uint32 command = pPacket->getCommand();
	uint32 callbackID = pPacket->getCallback();
	uint32 uid = pPacket->getUID();
	DestinationHandle des = pPacket->getDestination();
	DestinationHandle res = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	LOG_DEBUG("packet length=%d handle=%d node=%d command=%d callbackID=%d uid=%d desType=%d desIndex=%d resType=%d resIndex=%d message=0x%x",
		pPacket->getLength(), pAccept->getHandle(), getNodeID(), command, callbackID, uid,
		des.type, des.index, res.type, res.index, message);
	m_pScript->pushFunction("onReceiveAccept");
	m_pScript->pushUserData(pAccept, "Accept");
	m_pScript->pushString(pPacket->getBody(), pPacket->getBodyLength());
	m_pScript->pushNumber(command);
	m_pScript->pushNumber(callbackID);
	m_pScript->pushNumber(des.type);
	m_pScript->pushNumber(des.index);
	m_pScript->pushNumber(res.type);
	m_pScript->pushNumber(res.index);
	m_pScript->pushNumber(message);
	m_pScript->pushNumber(uid);
	m_pScript->executeFunction(0);
}
void LuaNode::onCloseAccept(Accept* pAccept){
	LOG_DEBUG("packet accept=%d node=%d", pAccept->getHandle(), getNodeID());
	m_pScript->pushFunction("onCloseAccept");
	m_pScript->pushUserData(pAccept, "Accept");
	m_pScript->executeFunction(0);
}
void LuaNode::onReceiveHttp(Http* pHttp, Packet* pPacket){
	uint32 command = pPacket->getCommand();
	uint32 callbackID = pPacket->getCallback();
	uint32 uid = pPacket->getUID();
	DestinationHandle des = pPacket->getDestination();
	DestinationHandle res = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	LOG_DEBUG("packet length=%d handle=%d node=%d command=%d callbackID=%d uid=%d desType=%d desIndex=%d resType=%d resIndex=%d message=0x%x",
		pPacket->getLength(), pHttp->getHandle(), getNodeID(), command, callbackID, uid,
		des.type, des.index, res.type, res.index, message);
	m_pScript->pushFunction("onReceiveHttp");
	m_pScript->pushUserData(pHttp, "Http");
	m_pScript->pushString(pPacket->getBody(), pPacket->getBodyLength());
	m_pScript->pushNumber(command);
	m_pScript->pushNumber(callbackID);
	m_pScript->pushNumber(des.type);
	m_pScript->pushNumber(des.index);
	m_pScript->pushNumber(res.type);
	m_pScript->pushNumber(res.index);
	m_pScript->pushNumber(message);
	m_pScript->pushNumber(uid);
	m_pScript->executeFunction(0);
}
void LuaNode::onCloseHttp(Http* pHttp){
	LOG_DEBUG("packet accept=%d node=%d", pHttp->getHandle(), getNodeID());
	m_pScript->pushFunction("onCloseHttp");
	m_pScript->pushUserData(pHttp, "Http");
	m_pScript->executeFunction(0);
}
void LuaNode::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	m_pScript->pushFunction("onCurlResponse");
	m_pScript->pushBool(isRequestOK);
	m_pScript->pushNumber(callbackID);
	m_pScript->pushString(pBuffer->data(), pBuffer->size());
	m_pScript->executeFunction(0);
}
int64 LuaNode::onTimerUpdate(uint32 callbackID){
	m_pScript->pushFunction("onTimerUpdate");
	m_pScript->pushNumber(callbackID);
	m_pScript->executeFunction(0);
	return -1;
}

void LuaNode::onInitialize(const std::string& param){
	LOG_DEBUG("onInitialize called node=%d param=%s", getNodeID(), param.c_str());
	initializeHandler();
	m_pScript->requireFile(param);
	m_pScript->pushFunction("onInitialize");
	m_pScript->pushUserData(this, "LuaNode");
	m_pScript->pushString(param);
	m_pScript->executeFunction(0);
}
void LuaNode::onDestroy(void){
	LOG_DEBUG("onDestroy called node=%d", getNodeID());
	m_pScript->pushFunction("onDestroy");
	m_pScript->executeFunction(0);
	destroyHandler();
}
void LuaNode::initializeHandler(void){
	if(NULL == m_pScript){
		m_pScript = new Script();
		m_pScript->retain();
    	m_pScript->setState(NULL);
        m_pScript->pushUserData(this, "LuaNode");
	    lua_setglobal(m_pScript->getState(), "g_pNode");
	}
}
void LuaNode::destroyHandler(void){
	SAFE_RELEASE(m_pScript);
}

NS_HIVE_END
