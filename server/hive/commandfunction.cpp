//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/26
// Time: 下午4:49
// To change this template use File | Settings | File Templates.
//

#include "commandfunction.h"
#include "hivehandler.h"

NS_HIVE_BEGIN

void onCommandPing(Accept* pAccept, Packet* pPacket, uint32 command){
//	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	pAccept->setOnline(true);
	pPacket->setCommand(COMMAND_PONG);
	pAccept->sendPacket(pPacket);
}
void onCommandPong(Accept* pAccept, Packet* pPacket, uint32 command){
//	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	pAccept->setOnline(true);
}
void onCommandRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	char temp[256] = {0};
	uint32 nodeID = 0;
	uint32 t = 0;
	uint32 magic = 0;
	pPacket->readBegin();
	pPacket->read(&nodeID, sizeof(uint32));
	pPacket->read(&t, sizeof(uint32));
	pPacket->read(&magic, sizeof(uint32));
	pPacket->readEnd();
    nodeID = ntohl(nodeID);
    t = ntohl(t);
    magic = ntohl(magic);
	const std::string& password = MainWorker::getInstance()->getPassword();
	sprintf(temp, "%04d-%d-%s", nodeID, t, password.c_str());
	uint32 magicHere = binary_djb_hash(temp, strlen(temp));
	LOG_DEBUG("onCommandRegister nodeID=%d str=%s magic=%d magicHere=%d", nodeID, temp, magic, magicHere);
	if(magic != magicHere){
		// response error message
		Packet* pResponse = new Packet(PACKET_HEAD_LENGTH);
        pResponse->retain();
        pResponse->writeBegin(COMMAND_RESPONSE);
        pResponse->writeEnd();
        bool result = pAccept->sendPacket(pResponse);
        uint32 connectHandle = pPacket->getDestination();
        LOG_DEBUG("onCommandRegister failed to remote connectHandle=%d result=%d acceptHandle=%d", connectHandle, result, pAccept->getHandle());
        pResponse->release();
		// close the connection
		pAccept->epollRemove();
	}else{
		// response success message
		uint32 acceptHandle = pAccept->getHandle();
		Packet* pResponse = new Packet(PACKET_HEAD_LENGTH);
        pResponse->retain();
        pResponse->writeBegin(COMMAND_RESPONSE);
        pResponse->setDestination(acceptHandle);
        pResponse->writeEnd();
        bool result = pAccept->sendPacket(pResponse);
        uint32 connectHandle = pPacket->getDestination();
	    LOG_DEBUG("onCommandRegister OK to remote connectHandle=%d result=%d acceptHandle=%d", connectHandle, result, acceptHandle);
        pResponse->release();
		// set Accept to identify state
		pAccept->setConnectionState(CS_IDENTIFY_OK);
	}
}
void onCommandResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	uint32 remoteHandle = pPacket->getDestination();
	LOG_DEBUG("onCommandResponse remoteHandle=%d", remoteHandle);
	if(remoteHandle > 0){
		// set Client to identify state
        pAccept->setConnectionState(CS_IDENTIFY_OK);
		// schedule current client to keep online
		Timer* pTimer = (MainWorker::getInstance()->getTimer());
		pAccept->setTimeout(CONNECT_KEEP_ONLINE_TIME, pTimer, MainWorker::keepConnectOnline);
		// identify OK, tell the MainHandler to register Hive
		uint32 connectHandle = pAccept->getHandle();
		uint32 moduleHandle = HiveHandler::getInstance()->getModuleHandle();
        LOG_DEBUG("start register hive connectHandle=%d moduleHandle=%d", connectHandle, moduleHandle);
        pPacket->writeBegin(COMMAND_HIVE_REGISTER);
        pPacket->setDestination(moduleHandle);
        pPacket->setMessage(0x666666);
        HiveHandler::getInstance()->writeHiveInfo(pPacket);
        HiveHandler::getInstance()->writeModuleInfo(pPacket);
        HiveHandler::getInstance()->writeRefIndex(pPacket);
        pPacket->writeEnd();
        bool result = pAccept->sendPacket(pPacket);
        LOG_DEBUG("registerHive to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
	}
}
void onCommandHiveRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	uint32 sourceID = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	if(sourceID != 0 || message != 0x666666){
        LOG_ERROR("COMMAND_HIVE_REGISTER sourceID=%d message=0x%x", sourceID, message);
	    return;
	}

	uint32 connectHandle = pAccept->getHandle();
	LOG_DEBUG("COMMAND_HIVE_REGISTER get connectHandle=%d", connectHandle);
	uint32 moduleHandle = pPacket->getDestination();
	HiveHandler::getInstance()->setNodeAccept(moduleHandle, pAccept);
	pPacket->readBegin();
	// get all the node info and record
	HiveHandler::getInstance()->recordHiveInfo(pPacket);
	HiveHandler::getInstance()->recordModuleInfo(pPacket);
	HiveHandler::getInstance()->recordRefIndex(pPacket);
	pPacket->readEnd();
	// response the remote node, current node info
	pPacket->writeBegin(COMMAND_HIVE_RESPONSE);
	pPacket->setDestination(connectHandle);
	pPacket->setMessage(0x666666);
	HiveHandler::getInstance()->writeHiveInfo(pPacket);
	HiveHandler::getInstance()->writeModuleInfo(pPacket);
	HiveHandler::getInstance()->writeRefIndex(pPacket);
	pPacket->writeEnd();
	bool result = pAccept->sendPacket(pPacket);
	LOG_DEBUG("COMMAND_HIVE_RESPONSE to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
}
void onCommandHiveResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	LOG_DEBUG("COMMAND_HIVE_RESPONSE get connectHandle=%d", pAccept->getHandle());
	uint32 sourceID = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	if(sourceID != 0 || message != 0x666666){
        LOG_ERROR("COMMAND_HIVE_RESPONSE sourceID=%d message=0x%x", sourceID, message);
	    return;
	}
	pPacket->readBegin();
	HiveHandler::getInstance()->recordHiveInfo(pPacket);
    HiveHandler::getInstance()->recordModuleInfo(pPacket);
    HiveHandler::getInstance()->recordRefIndex(pPacket);
}
void onCommandBroadcast(Accept* pAccept, Packet* pPacket, uint32 command){
	DestinationHandle destination = pPacket->getDestination();
	DestinationHandle source = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	uint32 uid = pPacket->getUID();
	LOG_DEBUG("handle=%d packet length=%d command=%d desType=%d desID=%d resType=%d resID=%d message=0x%x uid=%d",
		pAccept->getHandle(), pPacket->getLength(), command, destination.type, destination.index, source.type, source.index, message, uid);
	HiveHandler::getInstance()->broadcastDestination(destination.type, pPacket, false);
}
void onCommandBroadcastBegin(Accept* pAccept, Packet* pPacket, uint32 command){
	DestinationHandle destination = pPacket->getDestination();
	DestinationHandle source = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	uint32 uid = pPacket->getUID();
	LOG_DEBUG("handle=%d packet length=%d command=%d desType=%d desID=%d resType=%d resID=%d message=0x%x uid=%d",
		pAccept->getHandle(), pPacket->getLength(), command, destination.type, destination.index, source.type, source.index, message, uid);
	pPacket->setCommand(COMMAND_BROADCAST);
	HiveHandler::getInstance()->broadcastDestination(destination.type, pPacket, true);
}
void onCommandDispatchByHandle(Accept* pAccept, Packet* pPacket, uint32 command){
	DestinationHandle destination = pPacket->getDestination();
	DestinationHandle source = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	LOG_INFO("handle=%d packet length=%d command=%d desType=%d desID=%d resType=%d resID=%d message=0x%x uid=%d callbackID=%d",
		pAccept->getHandle(), pPacket->getLength(), command, destination.type, destination.index, source.type, source.index, message, pPacket->getUID(), pPacket->getCallback());
	bool result = HiveHandler::getInstance()->sendToDestination(destination, pPacket);
	if(!result){
		LOG_ERROR("sendToDestination failed moduleType=%d moduleIndex=%d command=%d", destination.type, destination.index, command);
	}
}
void onCommandBeeRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	uint32 sourceID = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	if(sourceID != 0 || message != 0x666666){
        LOG_ERROR("COMMAND_BEE_REGISTER sourceID=%d message=0x%x", sourceID, message);
	    return;
	}
	uint32 moduleType = 0;
	uint32 moduleIndex = 0;
	uint32 moduleHandle = HiveHandler::getInstance()->getModuleHandle();
	uint32 connectHandle = pAccept->getHandle();
	pPacket->readBegin();
	pPacket->read(&moduleType, sizeof(moduleType));
	pPacket->read(&moduleIndex, sizeof(moduleIndex));
	RefVector refIndexVector;
	int arraySize = 0;
    pPacket->read(&arraySize, sizeof(arraySize));
    for(int i=0; i<arraySize; ++i){
        uint32 refIndex = 0;
        pPacket->read(&refIndex, sizeof(refIndex));
        refIndexVector.push_back(refIndex);
    }
	pPacket->readEnd();
	DestinationHandle destination(moduleType, moduleIndex);
	HiveHandler::getInstance()->registerDestination(destination, moduleHandle, pAccept);
	for(auto refIndex : refIndexVector){
		HiveHandler::getInstance()->registerRefIndex(destination, refIndex);
	}
	// 返回 COMMAND_BEE_RESPONSE 给对方
	pPacket->writeBegin(COMMAND_BEE_RESPONSE);
	pPacket->setDestination(destination.handle);
	pPacket->write(&moduleHandle, sizeof(moduleHandle));
	pPacket->write(&connectHandle, sizeof(connectHandle));
	pPacket->writeEnd();
	pAccept->sendPacket(pPacket);

	// 广播通知所有节点，这个进程进入 COMMAND_BEE_ONLINE
	Packet* pOnline = new Packet(PACKET_HEAD_LENGTH);
	pOnline->retain();
	pOnline->writeBegin(COMMAND_BEE_ONLINE);
	pOnline->setDestination(connectHandle);
	pOnline->setMessage(0x666666);
	pOnline->write(&destination, sizeof(destination));
	pOnline->write(&moduleHandle, sizeof(moduleHandle));
	int arrayOffset = pOnline->beginArray<int32>();
    for(auto refIndex : refIndexVector){
        pOnline->writeArray<int32>(&refIndex, sizeof(uint32), arrayOffset);
    }
	pOnline->writeEnd();
	HiveHandler::getInstance()->broadcastHive(pOnline);
	pOnline->release();
}
void onCommandBeeResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}
void onCommandBeeOnline(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	uint32 sourceID = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	if(sourceID != 0 || message != 0x666666){
        LOG_ERROR("COMMAND_HIVE_RESPONSE sourceID=%d message=0x%x", sourceID, message);
	    return;
	}
	DestinationHandle destination;
	uint32 moduleHandle = 0;
	pPacket->readBegin();
	pPacket->read(&destination, sizeof(destination));
	pPacket->read(&moduleHandle, sizeof(moduleHandle));
	RefVector refIndexVector;
	int arraySize = 0;
    pPacket->read(&arraySize, sizeof(arraySize));
    for(int i=0; i<arraySize; ++i){
        uint32 refIndex = 0;
        pPacket->read(&refIndex, sizeof(refIndex));
        refIndexVector.push_back(refIndex);
    }
	pPacket->readEnd();
	HiveHandler::getInstance()->unregisterDestination(destination, true); // 先清理老的数据连接，下线该进程
	HiveHandler::getInstance()->registerDestination(destination, moduleHandle, NULL);
	for(auto refIndex : refIndexVector){
		HiveHandler::getInstance()->registerRefIndex(destination, refIndex);
	}
}
void onCommandBeeOffline(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	DestinationHandle destination = pPacket->getDestination();
	HiveHandler::getInstance()->unregisterDestination(destination, false);
}

NS_HIVE_END
