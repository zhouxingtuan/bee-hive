//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/7/29
// Time: 下午4:52
// To change this template use File | Settings | File Templates.
//

#include "hivehandler.h"

NS_HIVE_BEGIN

DEFINE_INSTANCE_CPP(HiveHandler)

HiveHandler::HiveHandler(void) : Handler(), TimerObject(),
    m_destID(0), m_destIP(""), m_destPort(0), m_destEncrypt(false), m_destDecrypt(false),
    m_localIP(""), m_localPort(0), m_localEncrypt(false), m_localDecrypt(false),
    m_globalIP(""), m_globalPort(0), m_globalEncrypt(false), m_globalDecrypt(false) {

}
HiveHandler::~HiveHandler(void){

}

void HiveHandler::onReceiveHttp(Http* pHttp, Packet* pPacket){

}
bool HiveHandler::onReceiveAccept(Accept* pAccept, Packet* pPacket){
	return false;
}
void HiveHandler::onReceiveClient(Client* pClient){
	identifyHive(pClient);
}
void HiveHandler::onCurlResponse(Buffer* pBuffer, uint32 nodeID, uint32 callbackID, bool isRequestOK){

}
void HiveHandler::onCloseAccept(Accept* pAccept){
	HandleToID::iterator itCur = m_handleToDestination.find(pAccept->getHandle());
	if(itCur != m_handleToDestination.end()){
		DestinationHandle destination = itCur->second;
		resetDestination(destination);
		m_handleToDestination.erase(itCur);
		notifyBeeOffline(destination);
	}
	itCur = m_acceptHandleToNode.find(pAccept->getHandle());
	if(itCur != m_acceptHandleToNode.end()){
		setNodeAccept(itCur->second, NULL);
		m_acceptHandleToNode.erase(itCur);
	}
}
void HiveHandler::onCloseClient(Client* pClient){
	uint32 connectHandle = pClient->getHandle();
	HandleToID::iterator itCur = m_handleToNode.find(connectHandle);
    if(itCur != m_handleToNode.end()){
        setNodeClient(itCur->second, NULL);
        m_handleToNode.erase(itCur);
    }
}
void HiveHandler::onCloseHttp(Http* pHttp){

}
void HiveHandler::onCloseListener(Listener* pListener){
	LOG_ERROR("open listener failed");
	sleep(1);
	exit(0);
}
bool HiveHandler::dispatchMessage(Packet* pPacket){
	DestinationHandle destination = pPacket->getDestination();
	return sendToDestination(destination, pPacket);
}
int64 HiveHandler::timerCallback(void){
	updateCheckNodeConnect();
	return CHECK_NODE_CONNECT_TIME;
}


void HiveHandler::onInitialize(void){
	openLocalListener();
	openGlobalListener();
	uint32 moduleHandle = getModuleHandle();
	registerNode(moduleHandle, m_globalIP.c_str(), m_globalPort, m_globalEncrypt, m_globalDecrypt);
	// register discovery node
	registerNode(m_destID, m_destIP.c_str(), m_destPort, m_destEncrypt, m_destDecrypt);
	// set timer
	setTimer(CHECK_NODE_CONNECT_TIME);
}
void HiveHandler::identifyHive(Accept* pAccept){
	char temp[256] = {0};
	uint32 connectHandle = pAccept->getHandle();
	uint32 moduleHandle = getModuleHandle();
	uint32 t = time(NULL);
	const std::string& password = MainWorker::getInstance()->getPassword();
	sprintf(temp, "%04d-%d-%s", moduleHandle, t, password.c_str());
	uint64 magic = binary_djb_hash(temp, strlen(temp));
	LOG_DEBUG("identifyHive to connectHandle=%d moduleHandle=%d str=%s magic=%llu", connectHandle, moduleHandle, temp, magic);
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + 16);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_REGISTER);
	pPacket->setDestination(connectHandle);
	pPacket->write(&moduleHandle, sizeof(uint32));
	pPacket->write(&t, sizeof(uint32));
	pPacket->write(&magic, sizeof(uint64));
	pPacket->writeEnd();
	bool result = pAccept->sendPacket(pPacket);
	LOG_DEBUG("identifyHive to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
	pPacket->release();
}
void HiveHandler::notifyBeeOffline(DestinationHandle destination){
	Packet* pOffline = new Packet(PACKET_HEAD_LENGTH);
    pOffline->retain();
    pOffline->writeBegin(COMMAND_BEE_OFFLINE);
    pOffline->setDestination(destination.handle);
    pOffline->writeEnd();
    broadcastHive(pOffline);
    pOffline->release();
}
void HiveHandler::broadcastHive(Packet* pPacket){
	for(auto pClient : m_hiveClients){
		if(NULL != pClient){
			pClient->sendPacket(pPacket);
		}
	}
}
void HiveHandler::broadcastDestination(uint32 moduleType, Packet* pPacket, bool shouldBroadcastHive){
	DestinationRouteVector* pRouteVec = getDestinationRouteVector(moduleType);
	if(NULL == pRouteVec){
		LOG_ERROR("can not fin route vector for moduleType=%d", moduleType);
		return;
	}
	uint32 moduleHandle = getModuleHandle();
	for(auto &route : (*pRouteVec)){
		// 可以判定该节点在线
		if(route.nodeID == moduleHandle){
            sendToDestinationByRoute(&route, pPacket);
		}
	}
	if(shouldBroadcastHive){
		broadcastHive(pPacket);
	}
}
bool HiveHandler::sendToDestination(DestinationHandle destination, Packet* pPacket){
	DestinationRoute* pRoute = getDestinationRouteByRefIndex(destination);
	if(NULL == pRoute){
		LOG_ERROR("NULL == pRoute for type=%d index=%d", destination.type, destination.index);
		return false;
	}
	return sendToDestinationByRoute(pRoute, pPacket);
}
bool HiveHandler::sendToDestinationByRoute(DestinationRoute* pRoute, Packet* pPacket){
	if(NULL == pRoute){
		return false;
	}
	if(pRoute->nodeID == 0){
		LOG_ERROR("pRoute->nodeID == 0 type=%d index=%d", pRoute->destination.type, pRoute->destination.index);
		return false;
	}
	if(pRoute->nodeID == getModuleHandle()){
		Accept* pAccept = pRoute->pAccept;
		if(NULL == pAccept){
			LOG_ERROR("NULL == pAccept for moduleType=%d moduleIndex=%d", pRoute->destination.type, pRoute->destination.index);
			return false;
		}
		return pAccept->sendPacket(pPacket);
	}
//	Client* pClient = getNodeClient(pRoute->nodeID);
	Accept* pAccept = getNodeAccept(pRoute->nodeID);
	if(NULL == pAccept){
		LOG_ERROR("can not find client for node=%d", pRoute->nodeID);
		return false;
	}
	return pAccept->sendPacket(pPacket);
}

void HiveHandler::recordHiveInfo(Packet* pPacket){
	int32 arraySize = 0;
	pPacket->read(&arraySize, sizeof(int32));
	for(auto i=0; i<arraySize; ++i){
		HiveInformation info;
		pPacket->read(&info, sizeof(HiveInformation));
		LOG_DEBUG("recordHiveInfo with id=%d ip=%s port=%d encrypt=%d decrypt=%d", info.id, info.ip, info.port, info.encrypt, info.decrypt);
		registerNode(info);
	}
}
void HiveHandler::writeHiveInfo(Packet* pPacket){
	int arrayOffset = pPacket->beginArray<int32>();
	for(auto &info : m_hiveNodes){
		if(info.id > 0){
			LOG_DEBUG("writeHiveInfo with id=%d ip=%s port=%d encrypt=%d decrypt=%d", info.id, info.ip, info.port, info.encrypt, info.decrypt);
			pPacket->writeArray<int32>(info.get(), sizeof(HiveInformation), arrayOffset);
		}
	}
}
void HiveHandler::writeModuleInfo(Packet* pPacket){
	std::vector<DestinationHandle> myDesVec;
	uint32 moduleHandle = getModuleHandle();
	for(auto &routeVec : m_destinations){
		for(auto &route : routeVec){
			if(route.nodeID == moduleHandle){
				myDesVec.push_back(route.destination);
			}
		}
	}
	pPacket->write(&moduleHandle, sizeof(moduleHandle));
	int arrayOffset = pPacket->beginArray<int32>();
	for(auto &destination : myDesVec){
		pPacket->writeArray<int32>(&destination, sizeof(destination), arrayOffset);
	}
}
void HiveHandler::recordModuleInfo(Packet* pPacket){
	uint32 moduleHandle = 0;
	int arraySize = 0;
	pPacket->read(&moduleHandle, sizeof(moduleHandle));
	pPacket->read(&arraySize, sizeof(arraySize));
	for(int i=0; i<arraySize; ++i){
		DestinationHandle destination;
		pPacket->read(&destination, sizeof(destination));
		registerDestination(destination, moduleHandle, NULL);
	}
}
void HiveHandler::writeRefIndex(Packet* pPacket){
	uint32 moduleHandle = getModuleHandle();
	int arrayOffset = pPacket->beginArray<int32>();
	for(uint32 moduleType=0; moduleType<(uint32)m_typeRefIndex.size(); ++moduleType){
		RefIndexVector& refVec = m_typeRefIndex[moduleType];
		for(uint32 refIndex=0; refIndex<(uint32)refVec.size(); ++refIndex){
			uint32 moduleIndex = refVec[refIndex];
			if( refVec[refIndex] > 0 ){
				DestinationHandle des(moduleType, moduleIndex);
                // 只同步连接到当前节点的refIndex数据
                DestinationRoute* pRoute = getDestinationRoute(des);
				if(NULL != pRoute && pRoute->nodeID == moduleHandle){
					RefIndexInfo info;
                    info.moduleType = moduleType;
                    info.moduleIndex = moduleIndex;
                    info.refIndex = refIndex;
                    pPacket->writeArray<int32>(&info, sizeof(info), arrayOffset);
				}
			}
		}
	}
}
void HiveHandler::recordRefIndex(Packet* pPacket){
	int arraySize = 0;
	pPacket->read(&arraySize, sizeof(arraySize));
	for(int i=0; i<arraySize; ++i){
		RefIndexInfo info;
		info.moduleType = 0;
		info.moduleIndex = 0;
		info.refIndex = 0;
		pPacket->read(&info, sizeof(info));
		DestinationHandle destination(info.moduleType, info.moduleIndex);
		registerRefIndex(destination, info.refIndex);
	}
}

void HiveHandler::registerRefIndex(DestinationHandle destination, uint32 refIndex){
    if(destination.type > MAX_TYPE_INDEX){
        LOG_ERROR("destination.type=%d > MAX_TYPE_INDEX=%d", destination.type, MAX_TYPE_INDEX);
        return;
    }
    if(refIndex > MAX_NODE_INDEX){
        LOG_ERROR("refIndex=%d > MAX_NODE_INDEX=%d", refIndex, MAX_NODE_INDEX);
        return;
    }
	if(destination.type >= m_typeRefIndex.size()){
		m_typeRefIndex.resize(destination.type + 1);
	}
	RefIndexVector& refVec = m_typeRefIndex[destination.type];
	if(refIndex >= refVec.size()){
		refVec.resize(refIndex + 1, 0);
	}
	LOG_INFO("moduleType=%d moduleIndex=%d refIndex=%d", destination.type, destination.index, refIndex);
	refVec[refIndex] = destination.index;
}
DestinationRoute* HiveHandler::getDestinationRouteByRefIndex(DestinationHandle destination){
	if(destination.type >= m_typeRefIndex.size()){
	    LOG_ERROR("destination.type=%d >= m_typeRefIndex.size()=%d", destination.type, (int)m_typeRefIndex.size());
		return NULL;
	}
	RefIndexVector& refVec = m_typeRefIndex[destination.type];
	if(destination.index >= refVec.size())
	{
        LOG_ERROR("destination.type=%d destination.index=%d >= refVec.size()=%d", destination.type, destination.index, (int)refVec.size());
	    return NULL;
	}
	uint32 moduleIndex = refVec[destination.index];
	if(moduleIndex > 0){
		DestinationHandle des(destination.type, moduleIndex);
		return getDestinationRoute(des);
	}
    LOG_ERROR("can not find DestinationRoute type=%d index=%d moduleIndex=%d", destination.type, destination.index, moduleIndex);
	return NULL;
}
RefIndexVector* HiveHandler::getRefIndexVector(uint32 moduleType){
	if(moduleType >= m_typeRefIndex.size()){
		return NULL;
	}
	return &(m_typeRefIndex[moduleType]);
}

void HiveHandler::registerDestination(DestinationHandle destination, uint32 nodeID, Accept* pAccept){
    if(destination.type > MAX_TYPE_INDEX){
        LOG_ERROR("destination.type=%d > MAX_TYPE_INDEX=%d", destination.type, MAX_TYPE_INDEX);
        return;
    }
    if(destination.index > MAX_NODE_INDEX){
        LOG_ERROR("destination.index=%d > MAX_NODE_INDEX=%d", destination.index, MAX_NODE_INDEX);
        return;
    }
	if(destination.type >= m_destinations.size()){
		m_destinations.resize(destination.type + 1);
	}
	DestinationRouteVector& routeVec = m_destinations[destination.type];
	if(destination.index >= routeVec.size()){
		routeVec.resize(destination.index + 1);
	}
	DestinationRoute& route = routeVec[destination.index];
	route.destination = destination;
	route.nodeID = nodeID;
	route.pAccept = pAccept;
	LOG_INFO("register bee type=%d index=%d node=%d", destination.type, destination.index, nodeID);
	if(NULL != pAccept){
		m_handleToDestination[pAccept->getHandle()] = destination.handle;
	}
}
void HiveHandler::unregisterDestination(DestinationHandle destination, bool notifyKickOff){
	DestinationRoute* pRoute = getDestinationRoute(destination);
	if(NULL != pRoute){
		if(pRoute->nodeID > 0){
			LOG_INFO("unregister bee type=%d index=%d node=%d", destination.type, destination.index, pRoute->nodeID);
		}
		Accept* pAccept = pRoute->pAccept;
		if(NULL != pAccept){
			if(notifyKickOff){
				// 移除后不再通知 bee offline 命令
				HandleToID::iterator itCur = m_handleToDestination.find(pAccept->getHandle());
				if(itCur != m_handleToDestination.end()){
					m_handleToDestination.erase(itCur);
				}
				// 发送命令通知这个进程退出
//				notifyBeeKickoff(pAccept, destination);
			}else{
				pAccept->epollRemove();
			}
			pRoute->pAccept = NULL;
		}
		LOG_INFO("unregisterDestination type=%d index=%d nodeID=%d", pRoute->destination.type, pRoute->destination.index, pRoute->nodeID);
		pRoute->nodeID = 0;
	}
}
void HiveHandler::notifyBeeKickoff(Accept* pAccept, DestinationHandle destination){
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_BEE_KICKOFF);
	pPacket->setDestination(destination.handle);
	pPacket->writeEnd();
	pAccept->sendPacket(pPacket);
	pPacket->release();
}
void HiveHandler::resetDestination(DestinationHandle destination){
	DestinationRoute* pRoute = getDestinationRoute(destination);
	if(NULL != pRoute){
		Accept* pAccept = pRoute->pAccept;
		if(NULL != pAccept){
			pRoute->pAccept = NULL;
		}
		LOG_INFO("resetDestination type=%d index=%d nodeID=%d", pRoute->destination.type, pRoute->destination.index, pRoute->nodeID);
		pRoute->nodeID = 0;
	}
}
DestinationRoute* HiveHandler::getDestinationRoute(DestinationHandle destination){
	if(destination.type >= m_destinations.size()){
	    LOG_ERROR("destination.type=%d >= m_destinations.size()=%d", destination.type, (int)m_destinations.size());
		return NULL;
	}
	DestinationRouteVector& routeVec = m_destinations[destination.type];
	if(destination.index >= routeVec.size()){
	    LOG_ERROR("destination.index=%d >= routeVec.size()=%d", destination.index, (int)routeVec.size());
		return NULL;
	}
	return &(routeVec[destination.index]);
}
DestinationRouteVector* HiveHandler::getDestinationRouteVector(uint32 moduleType){
	if(moduleType >= m_destinations.size()){
		return NULL;
	}
	return &(m_destinations[moduleType]);
}
bool HiveHandler::registerNode(const char* ptr){
	HiveInformation regInfo;
	regInfo.set(ptr);
	return registerNode(regInfo);
}
bool HiveHandler::registerNode(uint32 id, const char* ip, uint16 port, bool encrypt, bool decrypt){
	HiveInformation regInfo;
	regInfo.set(id, ip, port, encrypt, decrypt);
	return registerNode(regInfo);
}
bool HiveHandler::registerNode(const HiveInformation& regInfo){
    if(regInfo.id > MAX_HIVE_NUMBER){
        LOG_ERROR("regInfo.id=%d > MAX_HIVE_NUMBER=%d", regInfo.id, MAX_HIVE_NUMBER);
        return false;
    }
	if(regInfo.id >= (uint32)m_hiveNodes.size()){
		m_hiveNodes.resize(regInfo.id + 1);
	}
	HiveInformation& info = m_hiveNodes[regInfo.id];
	if(info == regInfo){
		LOG_INFO("current node info equal to register node");
		return true;
	}
	LOG_INFO("registerNode id=%d ip=%s port=%d encrypt=%d decrypt=%d", regInfo.id, regInfo.ip, regInfo.port, regInfo.encrypt, regInfo.decrypt);
	unregisterNode(regInfo.id);
	info.set(regInfo.get());
	checkNodeConnect(info.id);
	return true;
}
bool HiveHandler::unregisterNode(uint32 id){
    if(id > MAX_HIVE_NUMBER){
        LOG_ERROR("id=%d > MAX_HIVE_NUMBER=%d", id, MAX_HIVE_NUMBER);
        return false;
    }
	if(id >= (uint32)m_hiveNodes.size()){
		m_hiveNodes.resize(id + 1);
	}
	HiveInformation& info = m_hiveNodes[id];
	if(info.id > 0){
		info.reset();                           // reset the data
		Client* pClient = getNodeClient(id);
		if(NULL != pClient){
			setNodeClient(id, NULL);            // remove the node connection
			pClient->epollRemove();             // close connection
			return true;
		}
	}
	return false;
}
void HiveHandler::updateCheckNodeConnect(void){
	for(auto &info : m_hiveNodes){
		if(info.id > 0){
			Client* pClient = getNodeClient(info.id);
			if(NULL == pClient){
				checkNodeConnect(info.id);
			}
		}
	}
}
void HiveHandler::checkNodeConnect(uint32 id){
	// try to connect the destination node
	uint32 moduleHandle = getModuleHandle();
	if( id == moduleHandle ){
//		LOG_DEBUG("the current node is the destination node");
	}else{
		// if the connection is already connected, skip
		Client* pClient = getNodeClient(id);
		if(NULL == pClient){
			HiveInformation& info = m_hiveNodes[id];
			LOG_DEBUG("try to open client to node=%d ip=%s port=%d", id, info.ip, info.port);
			pClient = MainWorker::getInstance()->openClient(info.ip, info.port, info.encrypt, info.decrypt);
			if(pClient == NULL){
				LOG_ERROR("openClient failed node=%d ip=%s port=%d", id, info.ip, info.port);
			}else{
				setNodeClient(id, pClient);
			}
		}else{
			// do nothing
            LOG_DEBUG("the current node is already connected id=%d", id);
		}
	}
}
void HiveHandler::openLocalListener(void){
	LOG_DEBUG("ip=%s port=%d", m_localIP.c_str(), m_localPort);
	MainWorker::getInstance()->openSocketListener(m_localIP.c_str(), m_localPort, m_localEncrypt, m_localDecrypt);
}
void HiveHandler::openGlobalListener(void){
	LOG_DEBUG("ip=%s port=%d", m_globalIP.c_str(), m_globalPort);
	MainWorker::getInstance()->openSocketListener(m_globalIP.c_str(), m_globalPort, m_globalEncrypt, m_globalDecrypt);
}
void HiveHandler::setNodeClient(uint32 id, Client* pClient){
    if(id > MAX_HIVE_NUMBER){
        LOG_ERROR("id=%d > MAX_HIVE_NUMBER=%d", id, MAX_HIVE_NUMBER);
        return;
    }
	if(id >= m_hiveClients.size()){
		m_hiveClients.resize(id+1, NULL);
	}
	m_hiveClients[id] = pClient;
	if(NULL != pClient){
		m_handleToNode[pClient->getHandle()] = id;
	}
}
Client* HiveHandler::getNodeClient(uint32 id){
	if(id >= m_hiveClients.size()){
		return NULL;
	}
	return m_hiveClients[id];
}
void HiveHandler::setNodeAccept(uint32 id, Accept* pAccept){
    if(id > MAX_HIVE_NUMBER){
        LOG_ERROR("id=%d > MAX_HIVE_NUMBER=%d", id, MAX_HIVE_NUMBER);
        return;
    }
	if(id >= m_hiveAccepts.size()){
		m_hiveAccepts.resize(id+1, NULL);
	}
	m_hiveAccepts[id] = pAccept;
	if(NULL != pAccept){
		m_acceptHandleToNode[pAccept->getHandle()] = id;
	}
}
Accept* HiveHandler::getNodeAccept(uint32 id){
	if(id >= m_hiveAccepts.size()){
		return NULL;
	}
	return m_hiveAccepts[id];
}
uint32 HiveHandler::getNodeIDByConnectHandle(uint32 connectHandle){
	HandleToID::iterator itCur = m_handleToNode.find(connectHandle);
    if(itCur != m_handleToNode.end()){
        return itCur->second;
    }
    return 0;
}

NS_HIVE_END
