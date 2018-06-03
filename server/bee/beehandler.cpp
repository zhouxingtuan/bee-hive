//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/7/29
// Time: 下午4:52
// To change this template use File | Settings | File Templates.
//

#include "beehandler.h"
#include <dlfcn.h>

NS_HIVE_BEGIN

int64 TimerHandle::timerCallback(void){
	int64 timeCount = -1;
	Node* pNode = BeeHandler::getInstance()->findNodeByID(m_nodeID);
	if(NULL != pNode){
		timeCount = pNode->onTimerUpdate(m_callbackID);
	}
	if( -1 == timeCount ){
		BeeHandler::getInstance()->removeTimerHandle(this);
	}
	return timeCount;
}

DEFINE_INSTANCE_CPP(BeeHandler)

BeeHandler::BeeHandler(void) : Handler(), TimerObject(),
	m_hiveIP(""), m_hivePort(0), m_hiveEncrypt(false), m_hiveDecrypt(false),
	m_globalIP(""), m_globalPort(0), m_globalEncrypt(false), m_globalDecrypt(false),
	m_httpIP(""), m_httpPort(0), m_httpsIP(""), m_httpsPort(0), m_moduleName(""), m_moduleParam(""),
	m_hiveHandle(0), m_pHiveClient(NULL), m_pTimerHandlePool(NULL),
	m_pHandle(NULL) {

}
BeeHandler::~BeeHandler(void){
	SAFE_RELEASE(m_pTimerHandlePool)
}

void BeeHandler::onReceiveHttp(Http* pHttp, Packet* pPacket){
	DestinationHandle destination = pPacket->getDestination();
	Node* pNode = findNode(destination);
	if(NULL == pNode){
		pNode = randomNode();
		LOG_DEBUG("NULL == pNode handle by random moduleType=%d moduleIndex=%d node=%d",
			destination.type, destination.index, pNode->getNodeID());
	}else{
		LOG_DEBUG("onReceiveHttp handle by moduleType=%d moduleIndex=%d", destination.type, destination.index);
	}
	pHttp->setBindNodeID(pNode->getNodeID());   // 这里直接绑定
	pNode->onReceiveHttp(pHttp, pPacket);
}
bool BeeHandler::onReceiveAccept(Accept* pAccept, Packet* pPacket){
	DestinationHandle destination = pPacket->getDestination();
	Node* pNode = findNode(destination);
	if(NULL == pNode){
		LOG_ERROR("NULL == pNode moduleType=%d moduleIndex=%d", destination.type, destination.index);
		return false;
	}
//	pAccept->setBindNodeID(pNode->getNodeID());   // 这里由SO中的Node节点来绑定
	pNode->onReceiveAccept(pAccept, pPacket);
	return true;
}
void BeeHandler::onReceivePacketFromClient(Accept* pAccept, Packet* pPacket){
	Node* pNode = randomNode();
	pAccept->setBindNodeID(pNode->getNodeID());   // 这里由SO中的Node节点来绑定
	pNode->onReceiveAccept(pAccept, pPacket);
}
void BeeHandler::onReceiveClient(Client* pClient){
	if(pClient == m_pHiveClient){
		identifyHive(pClient);
	}
}
void BeeHandler::onCurlResponse(Buffer* pBuffer, uint32 nodeID, uint32 callbackID, bool isRequestOK){
	Node* pNode = findNodeByID(nodeID);
	if(NULL == pNode){
		LOG_ERROR("NULL == pNode nodeID=%d", nodeID);
		return;
	}
	pNode->onCurlResponse(pBuffer, callbackID, isRequestOK);
}
void BeeHandler::onCloseAccept(Accept* pAccept){
	uint32 bindNodeID = pAccept->getBindNodeID();
	Node* pNode = findNodeByID(bindNodeID);
	if(NULL == pNode){
		LOG_ERROR("NULL == pNode bindNodeID=%d", bindNodeID);
		return;
	}
	pNode->onCloseAccept(pAccept);
}
void BeeHandler::onCloseClient(Client* pClient){
	if(pClient == m_pHiveClient){
		// 激发所有node节点的退出操作 onCloseAccept
		for(auto &it : m_nodeMap){
			it.second->onCloseAccept(pClient);
        }
        LOG_ERROR("onCloseClient in bee");
		m_pHiveClient = NULL;
	}
}
void BeeHandler::onCloseHttp(Http* pHttp){
	uint32 bindNodeID = pHttp->getBindNodeID();
	Node* pNode = findNodeByID(bindNodeID);
	if(NULL == pNode){
		LOG_ERROR("NULL == pNode bindNodeID=%d", bindNodeID);
		return;
	}
	pNode->onCloseHttp(pHttp);
}
void BeeHandler::onCloseListener(Listener* pListener){
	LOG_ERROR("open listener failed");
	sleep(1);
	exit(0);
}
bool BeeHandler::dispatchMessage(Packet* pPacket){
	// 检查是不是当前的类型和index
	DestinationHandle destination = pPacket->getDestination();
	if(destination.type == getModuleType()){
		NodeMap::iterator itCur = m_nodeMap.find(destination.index);
		if(itCur != m_nodeMap.end()){
			pPacket->retain();
			m_packets.push_back(pPacket);
			if( !TimerObject::isActive() || TimerObject::getTimerLeft() > 0 ){
				setTimer(0);
			}
			return true;
		}
	}
	if(NULL == m_pHiveClient){
	    LOG_ERROR("NULL == m_pHiveClient");
		return false;
	}
	return m_pHiveClient->sendPacket(pPacket);
}
int64 BeeHandler::timerCallback(void){
	updateCheckNodeConnect();
	updateCheckPackets();
	return CHECK_NODE_CONNECT_TIME;
}

uint32 BeeHandler::startTimer(uint32 callbackID, int64 timeCount, uint32 nodeID){
	TimerHandle* pHandle =  this->createTimerHandle();
	if(NULL == pHandle){
		return 0;
	}
	pHandle->setCallbackID(callbackID);
	pHandle->setActiveObject(nodeID);
	pHandle->setTimer(timeCount, NULL);
	return pHandle->getHandle();
}
void BeeHandler::removeTimer(uint32 handle){
	TimerHandle* pHandle = getTimerHandle(handle);
	if(NULL != pHandle){
		this->removeTimerHandle(pHandle);
	}
}
void BeeHandler::changeTimer(uint32 handle, int64 timeCount){
	TimerHandle* pHandle = getTimerHandle(handle);
	if(NULL != pHandle){
		pHandle->setTimer(timeCount, NULL);
	}
}
int64 BeeHandler::getTimerLeft(uint32 handle){
	TimerHandle* pHandle = getTimerHandle(handle);
	if(NULL != pHandle){
		return pHandle->getTimerLeft();
	}
	return 0;
}
Node* BeeHandler::randomNode(void){
	int index = rand() % m_refIndex.size();
	uint32 nodeID = m_refIndex[index];
	return findNodeByID(nodeID);
}
Node* BeeHandler::findNode(DestinationHandle destination){
	if(destination.type == getModuleType()){
		NodeMap::iterator itCur = m_nodeMap.find(destination.index);
		if(itCur != m_nodeMap.end()){
			return itCur->second;
		}
	}
    return NULL;
}
Node* BeeHandler::findNodeByID(uint32 nodeID){
	NodeMap::iterator itCur = m_nodeMap.find(nodeID);
	if(itCur != m_nodeMap.end()){
		return itCur->second;
	}
	return NULL;
}
void BeeHandler::onInitialize(void){
	m_pTimerHandlePool = new TimerHandlePool();
	m_pTimerHandlePool->retain();
	openGlobalListener();
	openHttpListener();
	openHttpsListener();
	// set timer
	setTimer(CHECK_NODE_CONNECT_TIME);
	// 初始化 m_refIndex 对应的SO对象
	if( !initializeNode() ){
		LOG_ERROR("initializeNode error moduleName=%s", m_moduleName.c_str());
		sleep(1);
		exit(0);
	}
}
void BeeHandler::identifyHive(Accept* pAccept){
	char temp[256] = {0};
	uint32 connectHandle = pAccept->getHandle();
	uint32 moduleHandle = getModuleHandle();
	uint32 t = time(NULL);
	const std::string& password = MainWorker::getInstance()->getPassword();
	sprintf(temp, "%04d-%d-%s", moduleHandle, t, password.c_str());
	uint64 magic = binary_hash64(temp, strlen(temp));
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
void BeeHandler::writeRefIndex(Packet* pPacket){
	int arrayOffset = pPacket->beginArray<int32>();
	for(auto refIndex : m_refIndex){
		pPacket->writeArray<int32>(&refIndex, sizeof(uint32), arrayOffset);
	}
}
void BeeHandler::broadcastPacket(Accept* pAccept, Packet* pPacket){
	DestinationHandle destination = pPacket->getDestination();
	if(destination.type == getModuleType()){
		for(auto &it : m_nodeMap){
			it.second->onReceiveAccept(pAccept, pPacket);
	    }
	}
}
bool BeeHandler::dispatchPacket(Packet* pPacket){
	DestinationHandle destination = pPacket->getDestination();
    if(destination.type == getModuleType()){
        NodeMap::iterator itCur = m_nodeMap.find(destination.index);
        if(itCur != m_nodeMap.end()){
            itCur->second->onReceiveAccept(m_pHiveClient, pPacket);
            return true;
        }
    }
    return false;
}
void BeeHandler::updateCheckPackets(void){
	if(m_packets.size() > 0){
		PacketVector temp;
		temp.swap(m_packets);
		for(auto pPacket : temp){
			dispatchPacket(pPacket);
			pPacket->release();
		}
	}
}
void BeeHandler::updateCheckNodeConnect(void){
	if(NULL == m_pHiveClient){
		m_pHiveClient = MainWorker::getInstance()->openClient(m_hiveIP.c_str(), m_hivePort, m_hiveEncrypt, m_hiveDecrypt);
	}
}
void BeeHandler::openGlobalListener(void){
	LOG_DEBUG("ip=%s port=%d", m_globalIP.c_str(), m_globalPort);
	if(m_globalIP.empty()){
		return;
	}
	MainWorker::getInstance()->openSocketListener(m_globalIP.c_str(), m_globalPort, m_globalEncrypt, m_globalDecrypt);
}
void BeeHandler::openHttpListener(void){
	LOG_DEBUG("ip=%s port=%d", m_httpIP.c_str(), m_httpPort);
	if(m_httpIP.empty()){
		return;
	}
	MainWorker::getInstance()->openHttpListener(m_httpIP.c_str(), m_httpPort);
}
void BeeHandler::openHttpsListener(void){
	LOG_DEBUG("ip=%s port=%d", m_httpsIP.c_str(), m_httpsPort);
	if(m_httpsIP.empty()){
		return;
	}
	MainWorker::getInstance()->openHttpsListener(m_httpsIP.c_str(), m_httpsPort);
}
bool BeeHandler::initializeNode(void){
	char* errorStr;

	m_pHandle = dlopen(m_moduleName.c_str(), RTLD_NOW);
	if(NULL == m_pHandle){
		LOG_ERROR("load dll failed error=%s", dlerror());
		return false;
	}
	SOInitialize = (SOInitializeFunction)dlsym(m_pHandle, "SOInitialize");
	errorStr = dlerror();
	if(errorStr != NULL){
	  LOG_ERROR("load dll func failed SOInitialize error=%s", errorStr);
	  return false;
	}
	SODestroy = (SODestroyFunction)dlsym(m_pHandle, "SODestroy");
	errorStr = dlerror();
	if(errorStr != NULL){
	  LOG_ERROR("load dll func failed SODestroy error=%s", errorStr);
	  return false;
	}
	NodeCreate = (NodeCreateFunction)dlsym(m_pHandle, "NodeCreate");
	errorStr = dlerror();
	if(errorStr != NULL){
	  LOG_ERROR("load dll func failed HandlerCreateObject error=%s", errorStr);
	  return false;
	}
	NodeDestroy = (NodeDestroyFunction)dlsym(m_pHandle, "NodeDestroy");
	errorStr = dlerror();
	if(errorStr != NULL){
	  LOG_ERROR("load dll func failed HandlerReleaseObject error=%s", errorStr);
	  return false;
	}

	// 初始化SO内容
	SOInitialize();
	char temp[256];
	for(auto nodeID : m_refIndex){
	    snprintf(temp, 256, "%s_%d_%d_%d.log", getLogFile().c_str(), getModuleType(), getModuleIndex(), nodeID);
		Node* pNode = NodeCreate(nodeID, m_moduleParam, std::string(temp));
		pNode->setNodeID(nodeID);
		m_nodeMap[nodeID] = pNode;
	}
	return true;
}
void BeeHandler::kickoffAllNode(void){
	for(auto &it : m_nodeMap){
		NodeDestroy(it.second);
	}
	m_nodeMap.clear();
	SODestroy();
}

NS_HIVE_END
