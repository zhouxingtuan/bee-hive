//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/26
// Time: 下午4:49
// To change this template use File | Settings | File Templates.
//

#include "commandfunction.h"
#include "beehandler.h"

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
        LOG_DEBUG("start register hive connectHandle=%d", connectHandle);
        // 注册bee信息
        uint32 moduleType = BeeHandler::getInstance()->getModuleType();
        uint32 moduleIndex = BeeHandler::getInstance()->getModuleIndex();
        pPacket->writeBegin(COMMAND_BEE_REGISTER);
        pPacket->setDestination(connectHandle);
        pPacket->setMessage(0x666666);
		pPacket->write(&moduleType, sizeof(moduleType));
		pPacket->write(&moduleIndex, sizeof(moduleIndex));
		BeeHandler::getInstance()->writeRefIndex(pPacket);
        pPacket->writeEnd();
        bool result = pAccept->sendPacket(pPacket);
        LOG_DEBUG("registerHive to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
	}
}
void onCommandBroadcast(Accept* pAccept, Packet* pPacket, uint32 command){
	DestinationHandle destination = pPacket->getDestination();
    DestinationHandle source = pPacket->getSource();
    uint32 message = pPacket->getMessage();
    uint32 uid = pPacket->getUID();
    LOG_DEBUG("handle=%d packet length=%d command=%d desType=%d desID=%d resType=%d resID=%d message=0x%x uid=%d",
        pAccept->getHandle(), pPacket->getLength(), command, destination.type, destination.index, source.type, source.index, message, uid);
	BeeHandler::getInstance()->broadcastPacket(pAccept, pPacket);
}
void onCommandDispatchByClient(Accept* pAccept, Packet* pPacket, uint32 command){
    uint32 callbackID = pPacket->getCallback();
    uint32 message = pPacket->getMessage();
    uint32 uid = pPacket->getUID();
    LOG_DEBUG("message=0x%x uid=%d callbackID=%d", message, uid, callbackID);
    BeeHandler::getInstance()->onReceivePacketFromClient(pAccept, pPacket);
}
void onCommandDispatchByHandle(Accept* pAccept, Packet* pPacket, uint32 command){
	DestinationHandle destination = pPacket->getDestination();
	DestinationHandle source = pPacket->getSource();
	uint32 message = pPacket->getMessage();
	LOG_INFO("handle=%d packet length=%d command=%d desType=%d desID=%d resType=%d resID=%d message=0x%x uid=%d callbackID=%d",
		pAccept->getHandle(), pPacket->getLength(), command, destination.type, destination.index, source.type, source.index, message, pPacket->getUID(), pPacket->getCallback());
	bool result = BeeHandler::getInstance()->onReceiveAccept(pAccept, pPacket);
	if(!result){
        LOG_ERROR("sendToDestination failed moduleType=%d moduleIndex=%d command=%d", destination.type, destination.index, command);
	}
}
void onCommandBeeResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	uint32 moduleHandle = 0;
	uint32 connectHandle = 0;
	pPacket->readBegin();
	pPacket->read(&moduleHandle, sizeof(moduleHandle));
	pPacket->read(&connectHandle, sizeof(connectHandle));
	pPacket->readEnd();
	LOG_INFO("register OK to moduleHandle=%d connectHandle=%d", moduleHandle, connectHandle);
	BeeHandler::getInstance()->setHiveHandle(moduleHandle);
}
void onCommandBeeKickoff(Accept* pAccept, Packet* pPacket, uint32 command){
	uint32 destination = pPacket->getDestination();
	uint32 moduleHandle = BeeHandler::getInstance()->getModuleHandle();
	if(destination == moduleHandle){
		LOG_ERROR("kickoff bee");
		BeeHandler::getInstance()->kickoffAllNode();
		exit(0);
	}else{
		LOG_ERROR("kickoff handle not equal destination=%d moduleHandle=%d", destination, moduleHandle);
	}
}

NS_HIVE_END
