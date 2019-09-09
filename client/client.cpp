﻿//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/5
// Time: 下午3:59
// To change this template use File | Settings | File Templates.
//

#include "client.h"

NS_HIVE_BEGIN
/*--------------------------------------------------------------------*/
// 64-bit hash for 64-bit platforms
//uint64_t binary_murmur_hash64A ( const void * key, int len, unsigned int seed )
//{
//    const uint64_t m = 0xc6a4a7935bd1e995;
//    const int r = 47;
//
//    uint64_t h = seed ^ (len * m);
//
//    const uint64_t * data = (const uint64_t *)key;
//    const uint64_t * end = data + (len/8);
//
//    while(data != end)
//    {
//        uint64_t k = *data++;
//
//        k *= m;
//        k ^= k >> r;
//        k *= m;
//
//        h ^= k;
//        h *= m;
//    }
//
//    const unsigned char * data2 = (const unsigned char*)data;
//
//    switch(len & 7)
//    {
//        case 7: h ^= uint64_t(data2[6]) << 48;
//        case 6: h ^= uint64_t(data2[5]) << 40;
//        case 5: h ^= uint64_t(data2[4]) << 32;
//        case 4: h ^= uint64_t(data2[3]) << 24;
//        case 3: h ^= uint64_t(data2[2]) << 16;
//        case 2: h ^= uint64_t(data2[1]) << 8;
//        case 1: h ^= uint64_t(data2[0]);
//            h *= m;
//    };
//
//    h ^= h >> r;
//    h *= m;
//    h ^= h >> r;
//
//    return h;
//}
unsigned int binary_djb_hash(const char* cstr, unsigned int length){
    char* str = const_cast<char*>(cstr);
    uint32_t hash = 5381;
    while (length){
        hash += (hash << 5) + (*str++);
        --length;
    }
    return (hash & 0x7FFFFFFF);
}
// tiny加密
void tiny_encrypt(uint32_t* v, uint32_t* k, uint32_t round){
	uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
	uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
	uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
	for (i=0; i < round; i++) {                       /* basic cycle start */
		sum += delta;
		v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
	}                                              /* end cycle */
	v[0]=v0; v[1]=v1;
}
// tiny解密
void tiny_decrypt(uint32_t* v, uint32_t* k, uint32_t round, uint32_t move){
	uint32_t v0=v[0], v1=v[1], sum/*=0xC6EF3720*/, i;  /* set up */
	uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
	sum = delta << move;
	uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
	for (i=0; i<round; i++) {                         /* basic cycle start */
		v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
		v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		sum -= delta;
	}                                              /* end cycle */
	v[0]=v0; v[1]=v1;
}
// 普通加密，速度和加密程度居中
void binary_encrypt(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_encrypt( (uint32_t*)ptr, u_key, 32 );
    	ptr += 8;
    };
}
void binary_decrypt(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_decrypt( (uint32_t*)ptr, u_key, 32, 5 );
    	ptr += 8;
    };
}
/*--------------------------------------------------------------------*/
bool Thread::startThread(void){
	cancelThread();
	std::thread t(Thread::staticThreadFunction, this);
	t.detach();
	return true;
}
void Thread::staticThreadFunction(void* pData){
	Thread* ret = (Thread*)pData;
	ret->threadFunction();
}
void Thread::cancelThread(void){
	
}
/*--------------------------------------------------------------------*/
Buffer::Buffer(int length) : RefObject(), CharVector(length, 0) {
	this->clearEncryptFlag();
	this->clear();
}
Buffer::~Buffer(void){

}
/*--------------------------------------------------------------------*/
Packet::Packet(int bufferSize) : RefObject(),m_pBuffer(NULL),m_cursor(0) {
	assert(bufferSize > 0 && "Packet new bufferSize should be > 0");
	m_pBuffer = new Buffer(bufferSize);	// 是否处理分配内存失败？
	m_pBuffer->retain();
}
Packet::Packet(Buffer* pBuffer) : RefObject(),m_pBuffer(pBuffer),m_cursor(0) {
	assert(pBuffer != NULL && "Packet new pBuffer should not be NULL");
	pBuffer->retain();
}
Packet::~Packet(void){
	SAFE_RELEASE(m_pBuffer)
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
/*--------------------------------------------------------------------*/
int GetLastSocketError()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}
void CloseSocket(HSocket fd)
{
#ifdef WIN32
	closesocket(fd);
#else
	close(fd);
#endif // WIN32
}

Client::Client(void) : RefObject(), Sync(), Thread(),
	m_fd(0), m_port(0), m_isNeedEncrypt(false), m_isNeedDecrypt(false), m_isNeedReconnect(false), m_pingTime(0), m_currentConnectState(0),
	m_tempReadPacket(NULL), m_pInterface(NULL), m_tempLength(0) {
	//memset(m_ip, 0, sizeof(m_ip));
#ifdef WIN32
	WSADATA Ws;
	//Init Windows Socket  
	if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
	{
		fprintf(stderr, "WSAStartup failed\n");
	}
#endif
}
Client::~Client(void){
	releasePacket();
	SAFE_RELEASE(m_tempReadPacket)
#ifdef WIN32
	WSACleanup();
#endif
}
void Client::releasePacket(void){
	this->lock();
	for( auto pPacket : m_packetQueue ){
		pPacket->release();
	}
	m_packetQueue.clear();
	this->unlock();
}
void Client::identifyHive(void){
	char temp[256] = {0};
	unsigned int t = (unsigned int)time(NULL);
	srand(t);
	uint32 nodeID = rand();
	const std::string& password = getPassword();
	sprintf(temp, "%04d-%d-%s", nodeID, t, password.c_str());
	uint32 magic = binary_djb_hash(temp, (int)strlen(temp));
	fprintf(stderr, "identifyHive nodeID=%d str=%s magic=%d\n", nodeID, temp, magic);
    nodeID = htonl(nodeID);
    t = htonl(t);
    magic = htonl(magic);
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + 16);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_REGISTER);
	pPacket->write(&nodeID, sizeof(uint32));
	pPacket->write(&t, sizeof(uint32));
	pPacket->write(&magic, sizeof(uint32));
	pPacket->writeEnd();
	bool result = receivePacket(pPacket);
	fprintf(stderr, "identifyHive to server command=%d result=%d\n", pPacket->getCommand(), result);
	pPacket->release();
	m_pingTime = t + 5;
}
void Client::checkAndPingServer(void){
	int currentTime = (int)time(NULL);
	if( currentTime > m_pingTime ){
		m_pingTime = currentTime + CONNECT_KEEP_ONLINE_TIME;
		fprintf(stderr, "try to ping server\n");
		if( 0 == m_fd ){
			fprintf(stderr, "client is not connected\n");
			reconnectSocket();
			return;
		}
		Packet* pPacket = new Packet(sizeof(PacketHead));
        pPacket->retain();
        pPacket->writeBegin(COMMAND_PING);
        pPacket->writeEnd();
        receivePacket(pPacket);
        pPacket->release();
	}
}
void Client::reconnectSocket(void){
	if(m_isNeedReconnect){
		fprintf(stderr, "check reconnect server again\n");
		startConnectEvent();
	}else{
		fprintf(stderr, "no need to reconnect server\n");
	}
}
bool Client::sendData(unsigned int message, unsigned int callbackID, unsigned int uid, const char* pData, unsigned int length){
	Packet* pPacket = new Packet(sizeof(PacketHead) + length);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_CLIENT_REQUEST);
	pPacket->setDestination(0);
	pPacket->setSource(0);
	pPacket->setMessage(message);
	pPacket->setUID(uid);
	pPacket->setCallback(callbackID);
	pPacket->write(pData, length);
	pPacket->writeEnd();
	bool ret = receivePacket(pPacket);
	pPacket->release();
	return ret;
}
bool Client::receivePacket(Packet* pPacket){
	if( 0 == m_fd ){
		fprintf(stderr, "receivePacket failed 0 == m_fd\n");
		return false;
	}
	pPacket->resetCursor();		// 后面的写操作需要重置
	pPacket->retain();			// 进入队列前引用
	this->lock();
	m_packetQueue.push_back(pPacket);
	this->unlock();
	return true;
}
int Client::threadFunction(void){
	assert(NULL != m_pInterface && "m_pInterface for Client can not be NULL");
	while(true){
	    dispatchConnectEvent();
	    switch(m_currentConnectState){
	        case 1:{
                if( !trySelectSocket() ){
                    fprintf(stderr, "select failed. client out ...\n");
                    removeSocket();
                    addClientEvent(CLIENT_EVENT_CONN_OUT, NULL);
                }
				break;
	        }
	        default:{
                std::chrono::milliseconds dura(5);
                std::this_thread::sleep_for(dura);
				break;
	        }
	    }
	};
	return 0;
}
void Client::updateEvent(void){
    dispatchConnectEvent();
    switch(m_currentConnectState){
        case 1:{
            if( !trySelectSocket() ){
                fprintf(stderr, "select failed. client out ...\n");
                removeSocket();
                addClientEvent(CLIENT_EVENT_CONN_OUT, NULL);
            }
            break;
        }
        default:{
//            std::chrono::milliseconds dura(5);
//            std::this_thread::sleep_for(dura);
            break;
        }
    }
}
void Client::dispatchPacket(Packet* pPacket){
	if( this->isNeedDecrypt() && pPacket->getLength() >= (int)PACKET_HEAD_LENGTH ){
        binary_decrypt(pPacket->getBody(), pPacket->getBodyLength(), this->getKey().c_str());
    }
	unsigned int command = pPacket->getCommand();
	switch(command){
		case COMMAND_PONG:{
			fprintf(stderr, "receive server pong back\n");
			return;
		}
		case COMMAND_RESPONSE:{
			unsigned int dest = pPacket->getDestination();
			if(dest > 0){
				fprintf(stderr, "receive server identify response OK\n");
				addClientEvent(CLIENT_EVENT_IDENTIFY_SUCCESS, pPacket);
			}else{
				fprintf(stderr, "receive server identify response failed\n");
				addClientEvent(CLIENT_EVENT_IDENTIFY_FAILED, pPacket);
			}
			return;
		}
		default:{
			break;
		}
	}
	addClientEvent(CLIENT_EVENT_PACKET_IN, pPacket);
}
void Client::dispatchEvent(void){
	// 一次性处理完所有的消息
	m_tempEventQueue.clear();
	lock();
	m_tempEventQueue.swap(m_clientEventQueue);
	unlock();
	ClientEvent evt;
	memset(&evt, 0, sizeof(ClientEvent));
	while(!m_tempEventQueue.empty()){
		evt = m_tempEventQueue.front();
		m_tempEventQueue.pop_front();
		switch(evt.event){
    		case CLIENT_EVENT_NONE: break;	// nothing happened
    		case CLIENT_EVENT_PACKET_IN:{
    			m_pInterface->notifyPacketIn(this, evt.pPacket);
    			break;
    		}
    		case CLIENT_EVENT_CONN_FAILED:{
    			m_pInterface->notifyConnectServerFailed(this);	// 通知外部连接失败
    			break;
    		}
    		case CLIENT_EVENT_CONN_SUCCESS:{
    			m_pInterface->notifyConnectServerSuccess(this);	// 通知外部连接成功
    			break;
    		}
    		case CLIENT_EVENT_CONN_OUT:{
    			m_pInterface->notifyConnectOut(this);	// 通知外部连接退出
    			break;
    		}
    		case CLIENT_EVENT_IDENTIFY_FAILED:{
				m_pInterface->notifyIdentifyServerFailed(this, evt.pPacket);	// 通知外部验证失败
    		    break;
    		}
    		case CLIENT_EVENT_IDENTIFY_SUCCESS:{
				m_pInterface->notifyIdentifyServerSuccess(this, evt.pPacket);	// 通知外部验证成功
    		    break;
    		}
    		default:{
    			fprintf(stderr, "unknown event type for client\n");
    			break;
    		}
    	};
    	if( NULL != evt.pPacket ){
    		evt.pPacket->release();
    	}
	};
	// check to ping server
	checkAndPingServer();
}
void Client::addClientEvent(ClientEventType event, Packet* pPacket){
	ClientEvent evt;
	evt.event = event;
	evt.pPacket = pPacket;
	if( NULL != pPacket ){
		pPacket->retain();
	}
	lock();
	m_clientEventQueue.push_back(evt);
	unlock();
}
void Client::startConnectEvent(void){
    lock();
    m_threadEventQueue.push_back(1);
    unlock();
}
void Client::closeConnectEvent(void){
    lock();
    m_threadEventQueue.push_back(2);
    unlock();
}
void Client::dispatchConnectEvent(void){
	m_tempThreadEventQueue.clear();
	lock();
	m_tempThreadEventQueue.swap(m_threadEventQueue);
	unlock();
	int evt;
	while(!m_tempThreadEventQueue.empty()){
		evt = m_tempThreadEventQueue.front();
		m_tempThreadEventQueue.pop_front();
		switch(evt){
		    case 1:{
		        removeSocket();
                m_tempLength = 0;
                if( !connectServer() ){
                    addClientEvent(CLIENT_EVENT_CONN_FAILED, NULL);
                    continue;
                }
                m_currentConnectState = 1;
                addClientEvent(CLIENT_EVENT_CONN_SUCCESS, NULL);
                // identify the client
                identifyHive();
				break;
		    }
		    case 2:{
                fprintf(stderr, "choose close socket. client out ...\n");
                removeSocket();
                addClientEvent(CLIENT_EVENT_CONN_OUT, NULL);
				break;
		    }
		    default:{
		        fprintf(stderr, "unknown event evt=%d\n", evt);
				break;
		    }
		}
    }
}
bool Client::trySelectSocket(void){
	if( !tryReadSocket() ){
		return false;
	}
	if( !tryWriteSocket() ){
		return false;
	}
	return true;
}
bool Client::tryReadSocket(void){
	timeval	seltime;
	int		iResult = 0;
	seltime.tv_sec = 0;
	seltime.tv_usec = CLIENT_SELECT_TIMEOUT;
	fd_set readSet;
	FD_ZERO( &readSet );
	FD_SET( m_fd, &readSet );
	iResult = select(FD_SETSIZE, &readSet, NULL, NULL, &seltime);
	if( iResult < 0){
		fprintf(stderr,"--Client::tryReadSocket select read state error.\n");
		return false;
	}else if( iResult == 0 ){
//		fprintf(stderr, "select iResult == 0\n");
		return true;
	}
	if( FD_ISSET( m_fd, &readSet ) ){	//read data
		if( !readSocket() ){
			return false;
		}
	}
	return true;
}
bool Client::tryWriteSocket(void){
	timeval	seltime;
	int		iResult = 0;
	seltime.tv_sec = 0;
	seltime.tv_usec = CLIENT_SELECT_TIMEOUT;
	fd_set writeSet;
	FD_ZERO( &writeSet );
	FD_SET( m_fd, &writeSet );
	iResult = select(FD_SETSIZE, NULL, &writeSet, NULL, &seltime);
	if( iResult < 0){
		fprintf(stderr,"--Client::tryWriteSocket select write state error.\n");
		return false;
	}else if( iResult == 0 ){
//		fprintf(stderr, "tryWriteSocket select iResult == 0\n");
		return true;
	}
	if( FD_ISSET( m_fd, &writeSet ) ){//write data
		if( !writePacket() ){
			return false;
		}
	}
	return true;
}
void Client::removeSocket(void){
    m_currentConnectState = 0;
	closeSocket();	// 关闭套接字
	SAFE_RELEASE(m_tempReadPacket)
	releasePacket();	// 取消所有数据包的发送
}
bool Client::readSocket(void){
	char recvBuffer[8192+PACKET_HEAD_LENGTH];
    int nread;
    if(m_tempLength > 0){
        memcpy(recvBuffer, m_tempHead, m_tempLength);
    }else{
        m_tempLength = 0;
    }
#ifdef WIN32
    nread = (int)recv(m_fd, recvBuffer+m_tempLength, 8192, 0);
#else
    nread = (int)read(m_fd, recvBuffer+m_tempLength, 8192);
#endif

    if(nread < 0){
		int n = GetLastSocketError();
        switch(n){
        case EINTR: return true; // 读数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
            return true;
        default: return false;
        }
        return false;
    }else if(nread == 0){
        return false;
    }
    if(m_tempLength > 0){
        nread += m_tempLength;
        m_tempLength = 0;
    }
    return onParsePacket(recvBuffer, nread);
}
bool Client::onParsePacket(char* recvBuffer, int nread){
    Packet* pPacket;
    pPacket = m_tempReadPacket;
    if( NULL != pPacket ){
    	int needLength = pPacket->getLengthInHead() - pPacket->getCursor();
    	if( needLength > nread ){
			// 数据不够，仍然继续等待后续数据
			pPacket->write(recvBuffer, nread);
			return true;
    	}else{
			// 数据包已经完整，发送这个消息
			pPacket->write(recvBuffer, needLength);
			// 派发消息给对应的消息处理器
			dispatchPacket(pPacket);
			pPacket->release();		// 对应Packet创建时的retain
			pPacket = NULL;
			// 使用完临时数据之后，需要直接清除
			m_tempReadPacket = NULL;
			// 剩下的消息需要继续解析
			if(nread == needLength){
				return true;
			}
			recvBuffer += needLength;
			nread -= needLength;
    	}
    }
    // 解析剩余的数据包
	char* recvBufferPtr;
	int packetLength;
	int writeLength;
	bool isSuccessParsePacket = true;
	if( nread < (int)PACKET_HEAD_LENGTH ){
	    if(nread > 0){
            m_tempLength = nread;
	        memcpy(m_tempHead, recvBuffer, nread);
	    }
		return true;
	}
	//这里读取的信息很可能包含多条信息，这时候需要解析出来；这几条信息因为太短，在发送时被底层socket合并了
	recvBufferPtr = recvBuffer;
	do{
		// 对头部数据进行解密
		if( isNeedDecrypt() ){
			binary_decrypt(recvBufferPtr, PACKET_HEAD_LENGTH_PROTECT, getKey().c_str());
		}
		packetLength = ntohl(((PacketHead*)(recvBufferPtr))->length);
		if( packetLength < (int)PACKET_HEAD_LENGTH ){
			fprintf(stderr, "head length is invalid packetLength=%d left=%d \n", packetLength, (int)(nread-(recvBufferPtr-recvBuffer)));
			m_tempReadPacket = NULL;
			isSuccessParsePacket = false;
			break;	// 这里直接将数据丢弃
		}
		writeLength = min( (int)(nread-(recvBufferPtr-recvBuffer)), packetLength );
		// 创建Packet对象，并将数据写入
		pPacket = new Packet(packetLength);
		pPacket->retain();	// 如果数据没有全部收到，那么m_tempReadPacket会保持这个retain状态
		pPacket->write( recvBufferPtr, writeLength );
		pPacket->reverseHead();
		recvBufferPtr += writeLength;
		if( pPacket->isReceiveEnd() ){
			// 派发消息给对应的消息处理器
			dispatchPacket(pPacket);
			pPacket->release();
			pPacket = NULL;
		}
		// 如果消息没有全部接收，那么将会放到临时包中等待下一次读数据操作
	}while(nread-(recvBufferPtr-recvBuffer) >= (int)PACKET_HEAD_LENGTH);
	if(NULL == pPacket && isSuccessParsePacket){
	    m_tempLength = nread-(recvBufferPtr-recvBuffer);
        if(m_tempLength > 0){
            memcpy(m_tempHead, recvBufferPtr, m_tempLength);
        }
	}
	m_tempReadPacket = pPacket;
    return true;
}
bool Client::writePacket(void){
	Packet* pPacket = NULL;
	this->lock();
	if( !m_packetQueue.empty() ){
		pPacket = m_packetQueue.front();
		pPacket->retain();		// 引用 1
	}
	this->unlock();
	if( NULL == pPacket ){
		return true;
	}
	if( !writeSocket(pPacket) ){
		pPacket->release();		// 释放 1
		return false;
	}
	if( pPacket->isCursorEnd() ){
		pPacket->release();		// 释放 1
		// 反向检查队列并移除队末尾的对象
		bool releasePacketAtEnd = false;
		this->lock();
		if( !m_packetQueue.empty() && m_packetQueue.front() == pPacket ){
			releasePacketAtEnd = true;
			m_packetQueue.pop_front();
		}
		this->unlock();
		if(releasePacketAtEnd){
			pPacket->release();	// 对应进入队列时的retain
		}
		return true;
	}
	return true;
}
bool Client::writeSocket(Packet* pPacket){
	// 检查是否已经经过加密操作
	if( pPacket->getBuffer()->checkEncryptFlag() ){
	    pPacket->convertHead();
		if( this->isNeedEncrypt() ){
            binary_encrypt(pPacket->getDataPtr(), PACKET_HEAD_LENGTH_PROTECT, this->getKey().c_str());
            binary_encrypt(pPacket->getBody(), pPacket->getBodyLength(), this->getKey().c_str());
		}
	}
    int nwrite;
#ifdef WIN32
	nwrite = send(m_fd, pPacket->getCursorPtr(), pPacket->getLength() - pPacket->getCursor(), 0);
#else
	nwrite = (int)write(m_fd, pPacket->getCursorPtr(), pPacket->getLength() - pPacket->getCursor());
#endif
    if(nwrite < 0){
		int n = GetLastSocketError();
        switch(n){
        case EINTR: return true; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
//            fprintf(stderr, "write EAGAIN capture\n");
            return true;
        default: return false;
        }
        return false;
    }
    pPacket->moveCursor( nwrite );// used
	fprintf(stderr, "writeSocket nwrite=%d\n", nwrite);
    return true;
}
void Client::closeSocket(void){
	if( m_fd != INVALID_SOCKET ){
		CloseSocket(m_fd);
        m_fd = INVALID_SOCKET;
    }
}
#ifdef WIN32
bool Client::connectServer(void){
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *ptr = NULL;
	int fd = -1;
	int result;
	char port_str[8];
	sprintf(port_str, "%d", m_port);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	result = getaddrinfo(m_ip.c_str(), port_str, &hints, &res);
	if (result) {
		freeaddrinfo(res);	// 记得释放
		return false;
	}
	for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0) {
			continue;
		}
		if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
			CloseSocket(fd);
			fd = -1;
			continue;
		}
		break;	// we got one conn
	}
	freeaddrinfo(res);	// 记得释放
	if (fd < 0) {
		return false;
	}
	m_fd = fd;

//	int protocol = 0;
//	HSocket hs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	SOCKADDR_IN serverAddr;
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_addr.S_un.S_addr = inet_addr(m_ip.c_str());
//	serverAddr.sin_port = htons(m_port);
//	memset(serverAddr.sin_zero, 0x0, sizeof(serverAddr.sin_zero));
//	if (connect(hs, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
//		CloseSocket(hs);
//		return false;
//	}
//	m_fd = hs;
    return true;
}
#else
bool Client::connectServer(void) {
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *ptr = NULL;
	int fd = -1;
	int result;
	char port_str[8];
	sprintf(port_str, "%d", m_port);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	result = getaddrinfo(m_ip.c_str(), port_str, &hints, &res);
	if (result) {
//        if(NULL != res){
//            freeaddrinfo(res);	// 记得释放
//        }
		return false;
	}
	for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0) {
			continue;
		}
		if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
			close(fd);
			fd = -1;
			continue;
		}
		break;	// we got one conn
	}
	if(NULL != res){
	    freeaddrinfo(res);	// 记得释放
	}
	if (fd < 0) {
		return false;
	}
	m_fd = fd;
	return true;
}
#endif


/*--------------------------------------------------------------------*/

NS_HIVE_END
