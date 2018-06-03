//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/10
// Time: 上午8:30
// To change this template use File | Settings | File Templates.
//

#include "luamodule.h"
#include "luanode.h"

NS_HIVE_BEGIN

void SOInitialize(void){
	LOG_DEBUG("lua SOInitialize called");
}
void SODestroy(void){
	LOG_DEBUG("lua SODestroy called");
}

Node* NodeCreate(uint32 nodeID, const std::string& param, const std::string& logFile){
	LuaNode* pNode = new LuaNode();
	pNode->retain();
	pNode->setNodeID(nodeID);
	pNode->setLogFile(logFile);
	LOG_DEBUG("NodeCreate nodeID=%d param=%s", nodeID, param.c_str());
	pNode->onInitialize(param);
	return pNode;
}
void NodeDestroy(Node* pNode){
	if(NULL != pNode){
		LOG_DEBUG("NodeDestroy nodeID=%d", pNode->getNodeID());
		((LuaNode*)pNode)->onDestroy();
		pNode->release();
	}
}

NS_HIVE_END
