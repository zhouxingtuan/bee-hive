//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午2:58
// To change this template use File | Settings | File Templates.
//

#include "packet.h"

NS_HIVE_BEGIN

Packet::Packet(int bufferSize) : RefObject(),m_pBuffer(NULL),m_cursor(0) {
//	assert(bufferSize >= (int)sizeof(PacketHead) && "Packet new bufferSize should be > sizeof(PacketHead) 8");
	if(bufferSize < (int)PACKET_HEAD_LENGTH){
		bufferSize = (int)PACKET_HEAD_LENGTH;
	}
	m_pBuffer = new Buffer(bufferSize);	// 是否处理分配内存失败？
	m_pBuffer->retain();
}
Packet::Packet(Buffer* pBuffer) : RefObject(),m_pBuffer(pBuffer),m_cursor(0) {
//	assert(pBuffer != NULL && "Packet new pBuffer should not be NULL");
	pBuffer->retain();
}
Packet::~Packet(void){
	SAFE_RELEASE(m_pBuffer)
}

void Packet::copyFrom(Packet* pPacket){
	m_cursor = pPacket->m_cursor;
	m_pBuffer->copyFrom(pPacket->m_pBuffer);
}
void Packet::convertHead(void){
    PacketHead* pHead = getHead();
    pHead->length = htonl(pHead->length);
    pHead->command = htonl(pHead->command);
    pHead->callback = htonl(pHead->callback);
    pHead->destination = htonl(pHead->destination.getHandle());
    pHead->source = htonl(pHead->source.getHandle());
    pHead->message = htonl(pHead->message);
    pHead->uid = htonl(pHead->uid);
}
void Packet::reverseHead(void){
    PacketHead* pHead = getHead();
    pHead->length = ntohl(pHead->length);
    pHead->command = ntohl(pHead->command);
    pHead->callback = ntohl(pHead->callback);
    pHead->destination = ntohl(pHead->destination.getHandle());
    pHead->source = ntohl(pHead->source.getHandle());
    pHead->message = ntohl(pHead->message);
    pHead->uid = ntohl(pHead->uid);
}

NS_HIVE_END
