//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/8/4
// Time: 上午7:11
// To change this template use File | Settings | File Templates.
//

#include "node.h"
#include "mainworker.h"

NS_HIVE_BEGIN

static unsigned char encryptMap[256] = {
	0xca,0xf3,0xa5,0xdf,0xcd,0x34,0x02,0x2b,0x1b,0xbb,0x95,0xe9,0x7b,0x87,0xde,0xfe,
	0x44,0x8a,0x1c,0x82,0x6d,0x29,0x03,0x74,0xc5,0x5d,0x4c,0x2d,0x24,0x2a,0x58,0xd1,
	0xf2,0x09,0x38,0x9f,0xc8,0x80,0xef,0x12,0xcb,0x01,0x08,0x2e,0x5b,0xa0,0xdd,0x0d,
	0xa1,0x64,0x3e,0x93,0xd2,0xb2,0x45,0xd8,0xd9,0x78,0x47,0xa4,0x6a,0x43,0x10,0x92,
	0x4b,0x4a,0x1a,0x5f,0xb5,0x71,0x6c,0xaa,0xd4,0x42,0x3a,0x81,0x4f,0x14,0x36,0xa8,
	0x51,0x3c,0xa7,0xf4,0x7a,0xe0,0x0c,0x62,0x6f,0x8d,0x11,0x1e,0xe2,0x00,0x3f,0xb7,
	0x59,0x30,0x17,0x61,0x16,0xb6,0xab,0x40,0xb1,0x83,0xcc,0x99,0x91,0xac,0x7f,0xcf,
	0xf6,0x7d,0x4d,0xa2,0x89,0xaf,0xfb,0xf0,0x1d,0x2c,0x9d,0xd0,0x5e,0xfd,0xec,0x56,
	0x53,0x9c,0x15,0x0b,0x8c,0xba,0x9a,0x4e,0x22,0x65,0x52,0x25,0x8b,0x68,0xc4,0xe1,
	0xe6,0xfa,0xc6,0x96,0xdb,0x60,0x0e,0xb8,0x18,0x75,0xf7,0x0a,0xc1,0xf8,0x8f,0xd7,
	0x84,0xe3,0xf9,0xe8,0x88,0xd3,0x86,0xa3,0x41,0xae,0x73,0xad,0xa9,0xd6,0x77,0x90,
	0xa6,0xc0,0xe5,0x7e,0xb9,0x2f,0x85,0x9b,0xbf,0x33,0x49,0xff,0xdc,0x98,0x69,0x7c,
	0x76,0xc3,0xed,0x97,0x72,0x66,0x19,0x57,0x3d,0x9e,0xc9,0x3b,0x1f,0x21,0x26,0xb0,
	0x39,0x06,0x32,0xee,0x20,0x5a,0xbc,0x35,0x04,0xce,0xbe,0x79,0x6b,0xe7,0xb4,0x28,
	0x67,0x27,0xfc,0x8e,0xf1,0xc7,0x37,0xeb,0x94,0x23,0x07,0xd5,0x63,0xe4,0x5c,0xc2,
	0x55,0xda,0xea,0xf5,0xbd,0x6e,0x05,0xb3,0x54,0x48,0x0f,0x31,0x46,0x50,0x70,0x13,
};
static unsigned char decryptMap[256] = {
	0x5d,0x29,0x06,0x16,0xd8,0xf6,0xd1,0xea,0x2a,0x21,0x9b,0x83,0x56,0x2f,0x96,0xfa,
	0x3e,0x5a,0x27,0xff,0x4d,0x82,0x64,0x62,0x98,0xc6,0x42,0x08,0x12,0x78,0x5b,0xcc,
	0xd4,0xcd,0x88,0xe9,0x1c,0x8b,0xce,0xe1,0xdf,0x15,0x1d,0x07,0x79,0x1b,0x2b,0xb5,
	0x61,0xfb,0xd2,0xb9,0x05,0xd7,0x4e,0xe6,0x22,0xd0,0x4a,0xcb,0x51,0xc8,0x32,0x5e,
	0x67,0xa8,0x49,0x3d,0x10,0x36,0xfc,0x3a,0xf9,0xba,0x41,0x40,0x1a,0x72,0x87,0x4c,
	0xfd,0x50,0x8a,0x80,0xf8,0xf0,0x7f,0xc7,0x1e,0x60,0xd5,0x2c,0xee,0x19,0x7c,0x43,
	0x95,0x63,0x57,0xec,0x31,0x89,0xc5,0xe0,0x8d,0xbe,0x3c,0xdc,0x46,0x14,0xf5,0x58,
	0xfe,0x45,0xc4,0xaa,0x17,0x99,0xc0,0xae,0x39,0xdb,0x54,0x0c,0xbf,0x71,0xb3,0x6e,
	0x25,0x4b,0x13,0x69,0xa0,0xb6,0xa6,0x0d,0xa4,0x74,0x11,0x8c,0x84,0x59,0xe3,0x9e,
	0xaf,0x6c,0x3f,0x33,0xe8,0x0a,0x93,0xc3,0xbd,0x6b,0x86,0xb7,0x81,0x7a,0xc9,0x23,
	0x2d,0x30,0x73,0xa7,0x3b,0x02,0xb0,0x52,0x4f,0xac,0x47,0x66,0x6d,0xab,0xa9,0x75,
	0xcf,0x68,0x35,0xf7,0xde,0x44,0x65,0x5f,0x97,0xb4,0x85,0x09,0xd6,0xf4,0xda,0xb8,
	0xb1,0x9c,0xef,0xc1,0x8e,0x18,0x92,0xe5,0x24,0xca,0x00,0x28,0x6a,0x04,0xd9,0x6f,
	0x7b,0x1f,0x34,0xa5,0x48,0xeb,0xad,0x9f,0x37,0x38,0xf1,0x94,0xbc,0x2e,0x0e,0x03,
	0x55,0x8f,0x5c,0xa1,0xed,0xb2,0x90,0xdd,0xa3,0x0b,0xf2,0xe7,0x7e,0xc2,0xd3,0x26,
	0x77,0xe4,0x20,0x01,0x53,0xf3,0x70,0x9a,0x9d,0xa2,0x91,0x76,0xe2,0x7d,0x0f,0xbb,
};
void replace_encrypt(char* ptr, unsigned int length){
	for(unsigned int i=0; i<length; ++i){
		unsigned char index = (unsigned char)ptr[i];
		ptr[i] = (char)encryptMap[index];
	}
}
void replace_decrypt(char* ptr, unsigned int length){
	for(unsigned int i=0; i<length; ++i){
		unsigned char index = (unsigned char)ptr[i];
		ptr[i] = (char)decryptMap[index];
	}
}

Node::Node(void) : RefObject() {

}
Node::~Node(void){

}

// 开始一个计时器
uint32 Node::startTimer(uint32 callbackID, int64 timeCount){
	return MainWorker::getInstance()->startTimer(callbackID, timeCount, getNodeID());
}
// 移除计时器
void Node::removeTimer(uint32 handle){
	MainWorker::getInstance()->removeTimer(handle);
}
// 更改计时器的时间
void Node::changeTimer(uint32 handle, int64 timeCount){
	MainWorker::getInstance()->changeTimer(handle, timeCount);
}
// 获取计时器剩余时间
int64 Node::getTimerLeft(uint32 handle){
	return MainWorker::getInstance()->getTimerLeft(handle);
}
bool Node::sendPacket(Packet* pPacket){
	return MainWorker::getInstance()->dispatchMessage(pPacket);
}
bool Node::broadcast(uint32 desType, uint32 message, uint32 uid, const char* ptr, uint32 length){
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + length);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_BROADCAST_BEGIN);
	pPacket->setCallback(0);
	pPacket->setDestination(desType, 0);
	pPacket->setSource(getModuleType(), getNodeID());
	pPacket->setMessage(message);
	pPacket->setUID(uid);
	pPacket->write(ptr, length);
	pPacket->writeEnd();
	bool result = MainWorker::getInstance()->dispatchMessage(pPacket);
	pPacket->release();
	return result;
}
bool Node::sendRequest(uint32 desType, uint32 desID, uint32 message, uint32 callbackID, uint32 uid, const char* ptr, uint32 length){
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + length);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_NODE_REQUEST);
	pPacket->setCallback(callbackID);
	pPacket->setUID(uid);
	pPacket->setDestination(desType, desID);
	pPacket->setSource(getModuleType(), getNodeID());
	pPacket->setMessage(message);
	pPacket->write(ptr, length);
	pPacket->writeEnd();
	bool result = MainWorker::getInstance()->dispatchMessage(pPacket);
	pPacket->release();
	return result;
}
bool Node::sendResponse(uint32 desType, uint32 desID, uint32 message, uint32 callbackID, uint32 uid, const char* ptr, uint32 length){
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + length);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_NODE_RESPONSE);
	pPacket->setCallback(callbackID);
	pPacket->setUID(uid);
	pPacket->setDestination(desType, desID);
	pPacket->setSource(getModuleType(), getNodeID());
	pPacket->setMessage(message);
	pPacket->write(ptr, length);
	pPacket->writeEnd();
	bool result = MainWorker::getInstance()->dispatchMessage(pPacket);
	pPacket->release();
	return result;
}
bool Node::responseConnect(uint32 desType, uint32 desID, uint32 message, uint32 callbackID, uint32 uid, const char* ptr, uint32 length,
	uint32 connType, uint32 connHandle){
	bool result = false;
	switch(connType){
		case CONN_TYPE_CLIENT:{
			Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + length);
        	pPacket->retain();
        	pPacket->writeBegin(COMMAND_NODE_RESPONSE);
        	pPacket->setCallback(callbackID);
        	pPacket->setUID(uid);
        	pPacket->setDestination(desType, desID);
        	pPacket->setSource(getModuleType(), getNodeID());
        	pPacket->setMessage(message);
        	pPacket->write(ptr, length);
        	pPacket->writeEnd();
			Client* pClient = MainWorker::getInstance()->getClient(connHandle);
			if(NULL != pClient){
				result = pClient->sendPacket(pPacket);
			}
			pPacket->release();
			break;
		}
		case CONN_TYPE_ACCEPT:{
			Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + length);
            pPacket->retain();
            pPacket->writeBegin(COMMAND_NODE_RESPONSE);
            pPacket->setCallback(callbackID);
            pPacket->setUID(uid);
            pPacket->setDestination(desType, desID);
            pPacket->setSource(getModuleType(), getNodeID());
            pPacket->setMessage(message);
            pPacket->write(ptr, length);
            pPacket->writeEnd();
			Accept* pAccept = MainWorker::getInstance()->getAccept(connHandle);
			if(NULL != pAccept){
				result = pAccept->sendPacket(pPacket);
			}
			pPacket->release();
			break;
		}
		case CONN_TYPE_HTTP:{
			Http* pHttp = MainWorker::getInstance()->getHttp(connHandle);
			if(NULL != pHttp){
				result = pHttp->responseBody(ptr, length);
			}
			break;
		}
		case CONN_TYPE_HTTPS:{
			Https* pHttps = MainWorker::getInstance()->getHttps(connHandle);
			if(NULL != pHttps){
				result = pHttps->responseBody(ptr, length);
			}
			break;
		}
		default:{
			LOG_ERROR("unknown connection type =%d", connType);
			break;
		}
	}
	if( !result ){
		LOG_ERROR("responseConnect failed desType=%d desID=%d message=0x%x callbackID=%d uid=%d connType=%d connHandle=%d",
			desType, desID, message, callbackID, uid, connType, connHandle);
	}
	return result;
}
const char* Node::getConnectIP(uint32 connType, uint32 connHandle) {
	switch(connType){
		case CONN_TYPE_CLIENT:{
			Client* pClient = MainWorker::getInstance()->getClient(connHandle);
			if(NULL != pClient){
				return pClient->getIP();
			}
			break;
		}
		case CONN_TYPE_ACCEPT:{
			Accept* pAccept = MainWorker::getInstance()->getAccept(connHandle);
			if(NULL != pAccept){
				return pAccept->getIP();
			}
			break;
		}
		case CONN_TYPE_HTTP:{
			Http* pHttp = MainWorker::getInstance()->getHttp(connHandle);
			if(NULL != pHttp){
				return pHttp->getIP();
			}
			break;
		}
		case CONN_TYPE_HTTPS:{
			Https* pHttps = MainWorker::getInstance()->getHttps(connHandle);
			if(NULL != pHttps){
				return pHttps->getIP();
			}
			break;
		}
		default:{
			LOG_ERROR("unknown connection type =%d", connType);
			break;
		}
	}
	LOG_ERROR("getConnectIP failed connHandle=%d", connHandle);
	return "";
}
bool Node::closeConnect(uint32 connType, uint32 connHandle){
	switch(connType){
		case CONN_TYPE_CLIENT:{
			Client* pClient = MainWorker::getInstance()->getClient(connHandle);
			if(NULL != pClient){
				pClient->epollRemove();
				return true;
			}
			break;
		}
		case CONN_TYPE_ACCEPT:{
			Accept* pAccept = MainWorker::getInstance()->getAccept(connHandle);
			if(NULL != pAccept){
				pAccept->epollRemove();
				return true;
			}
			break;
		}
		case CONN_TYPE_HTTP:{
			Http* pHttp = MainWorker::getInstance()->getHttp(connHandle);
			if(NULL != pHttp){
				pHttp->epollRemove();
				return true;
			}
			break;
		}
		case CONN_TYPE_HTTPS:{
			Https* pHttps = MainWorker::getInstance()->getHttps(connHandle);
			if(NULL != pHttps){
				pHttps->epollRemove();
				return true;
			}
			break;
		}
		default:{
			LOG_ERROR("unknown connection type =%d", connType);
			break;
		}
	}
	LOG_ERROR("closeConnect failed connHandle=%d", connHandle);
	return false;
}
bool Node::curlRequest(RequestData* pRequest){
	pRequest->m_originNodeID = getNodeID();
	return MainWorker::getInstance()->sendCurlRequest(pRequest);
}

uint32 Node::hash(const char* str, uint32 length){
	return binary_djb_hash(str, length);
}
void Node::encrypt(const char* str, uint32 length, std::string& result){
	std::vector<char> temp(length);
	memcpy(temp.data(), str, length);
	replace_encrypt(temp.data(), length);
	result = std::string(temp.data(), length);
}
void Node::decrypt(const char* str, uint32 length, std::string& result){
	std::vector<char> temp(length);
	memcpy(temp.data(), str, length);
	replace_decrypt(temp.data(), length);
	result = std::string(temp.data(), length);
}
uint32 Node::getModuleType(void) const{
	return MainWorker::getInstance()->getHandler()->getModuleType();
}
uint32 Node::getModuleIndex(void) const{
	return MainWorker::getInstance()->getHandler()->getModuleIndex();
}
uint32 Node::getModuleHandle(void) const{
	return MainWorker::getInstance()->getHandler()->getModuleHandle();
}
void Node::log(const char* str){
    writeLog(getLogFile().c_str(), str);
}

NS_HIVE_END
