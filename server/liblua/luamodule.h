//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/10
// Time: 上午8:30
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__luamodule__
#define __hive__luamodule__

#include "core.h"

NS_HIVE_BEGIN

extern "C"{

void SOInitialize(void);
void SODestroy(void);

Node* NodeCreate(uint32 nodeID, const std::string& param, const std::string& logFile);
void NodeDestroy(Node* pNode);

}

NS_HIVE_END

#endif
