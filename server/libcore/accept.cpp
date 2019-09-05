//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:40
// To change this template use File | Settings | File Templates.
//

#include "accept.h"
#include "epoll.h"
#include "mainworker.h"

NS_HIVE_BEGIN

void onAcceptRead(Accept* pAccept, char* recvBuffer, int nread){
    Packet* pPacket;
    pPacket = pAccept->getTempReadPacket();
    //check stick message
    if( NULL != pPacket ){
    	int needLength = pPacket->getLengthInHead() - pPacket->getCursor();
    	if( needLength > nread ){
			// 数据不够，仍然继续等待后续数据
			pPacket->write(recvBuffer, nread);
			return;
    	}else{
			// 数据包已经完整，发送这个消息
			pPacket->write(recvBuffer, needLength);
			// 派发消息给对应的消息处理器
			pAccept->dispatchPacket(pPacket, pPacket->getCommand());
			pPacket->release();		// 对应Packet创建时的retain
			pPacket = NULL;
			// 使用完临时数据之后，需要直接清除
			pAccept->setTempReadPacket(NULL);
			// 剩下的消息需要继续解析
			if(nread == needLength){
				return;
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
            pAccept->m_tempLength = nread;
	        memcpy(pAccept->m_tempHead, recvBuffer, nread);
	    }
		return;
	}
	//这里读取的信息很可能包含多条信息，这时候需要解析出来；这几条信息因为太短，在发送时被底层socket合并了
	recvBufferPtr = recvBuffer;
	do{
		// 对头部数据进行解密
		if( pAccept->isNeedDecrypt() ){
			LOG_DEBUG("Accept decrypt head");
			binary_decrypt(recvBufferPtr, PACKET_HEAD_LENGTH_PROTECT, MainWorker::getInstance()->getKey());
		}
		packetLength = ntohl( ((PacketHead*)(recvBufferPtr))->length );
		if( packetLength < (int)PACKET_HEAD_LENGTH || packetLength > pAccept->getMaxLength() ){
			LOG_ERROR("head length is invalid packetLength=%d left=%d", packetLength, (int)(nread-(recvBufferPtr-recvBuffer)));
			pAccept->setTempReadPacket(NULL);
			isSuccessParsePacket = false;
			pAccept->epollRemove();
			return;
			break;	// 这里直接将数据丢弃
		}
		writeLength = std::min( (int)(nread-(recvBufferPtr-recvBuffer)), packetLength );
		// 创建Packet对象，并将数据写入
		pPacket = new Packet(packetLength);
		pPacket->retain();	// 如果数据没有全部收到，那么m_tempReadPacket会保持这个retain状态
		pPacket->write( recvBufferPtr, writeLength );
		pPacket->reverseHead();
		recvBufferPtr += writeLength;
		if( pPacket->isReceiveEnd() ){
			// 派发消息给对应的消息处理器
			pAccept->dispatchPacket(pPacket, pPacket->getCommand());
			pPacket->release();
			pPacket = NULL;
		}
		// 如果消息没有全部接收，那么将会放到临时包中等待下一次读数据操作
	}while(nread-(recvBufferPtr-recvBuffer) >= (int)PACKET_HEAD_LENGTH);
	if(NULL == pPacket && isSuccessParsePacket){
	    pAccept->m_tempLength = nread-(recvBufferPtr-recvBuffer);
        if(pAccept->m_tempLength > 0){
            memcpy(pAccept->m_tempHead, recvBufferPtr, pAccept->m_tempLength);
        }
	}
    pAccept->setTempReadPacket(pPacket);
}

Accept::Accept(void) : EpollConnectObject(), TimerObject(), Object0824(),
 	m_timerCallback(NULL), m_tempReadPacket(NULL),
 	m_connectionState(CS_DISCONNECT), m_isOnline(false),
	m_isNeedEncrypt(false), m_isNeedDecrypt(false), m_connType(CONN_TYPE_ACCEPT), m_bindNodeID(0), m_tempLength(0)
{
	m_connType = CONN_TYPE_ACCEPT;
}
Accept::~Accept(void){
	releasePacket();
	SAFE_RELEASE(m_tempReadPacket)
}
bool Accept::epollActive(uint32 events){
	return false;
}
bool Accept::epollIn(void){
	int result;
	do{
		result = readSocket();
	}while(result == 0);
	if( result < 0 ){
		epollRemove();
		return false;
	}
	return true;
}
bool Accept::epollOut(void){
	Packet* pPacket;
	int result;
	do{
		pPacket = NULL;
		if( !m_packetQueue.empty() ){
			pPacket = m_packetQueue.front();
			m_packetQueue.pop_front();
		}
		if( NULL == pPacket ){
			//getEpoll()->objectChange(this, EPOLLIN);
			return true;
		}
		result = writeSocket(pPacket);
		if( result < 0 ){
		    LOG_ERROR("writeSocket result < 0 remove client");
			pPacket->release();		// 释放
			epollRemove();
			return false;
		}
		// result == 0 成功写 || result > 0 需要重新尝试写
		if(pPacket->isCursorEnd()){
			pPacket->release();		// 释放
		}else{
			// 没写完就说明写入buffer已经满了，等待下一次写操作
			m_packetQueue.push_front(pPacket);
			return true;
		}
	}while(1);
	return true;
}
void Accept::epollRemove(void){
	if(getSocketFD() > 0){
    	MainWorker::getInstance()->closeAccept(this);
	}
}
void Accept::epollCheck(void){
	if( m_packetQueue.empty() ){
		getEpoll()->objectChange(this, EPOLLIN);
	}else{
		getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
	}
}
int64 Accept::timerCallback(void){
	if(NULL != m_timerCallback){
		return m_timerCallback(this);
	}
	return -1;
}
bool Accept::setTimeout(int64 timeCount, Timer* pTimer, ConnectTimeoutCallback callback){
	m_timerCallback = callback;
	return setTimer(timeCount, pTimer);
}

void Accept::releasePacket(void){
	for( auto pPacket : m_packetQueue ){
		pPacket->release();
	}
	m_packetQueue.clear();
}
bool Accept::sendPacket(Packet* pPacket){
//	LOG_INFO("send package, packet length=%d commond=%u des=%u res=%u message=0x%x uid=%d callbackID=%u",
//				pPacket->getLength(), pPacket->getCommand(), pPacket->getDestination(), pPacket->getSource(),
//				pPacket->getMessage(), pPacket->getUID(), pPacket->getCallback());
	pPacket->resetCursor();		// 后面的写操作需要重置
	pPacket->retain();			// 进入队列前引用
	if( !m_packetQueue.empty() ){
		// 已经在epoll中等待out事件
		m_packetQueue.push_back(pPacket);
	}else{
		// 先执行写操作，如果出现重试，那么再将Packet入队
		int result = writeSocket(pPacket);
		if( result < 0 ){
		    LOG_ERROR("writeSocket result < 0 remove client");
			pPacket->release();		// 释放
			epollRemove();
		}else{
			// result == 0 成功写 || result > 0 需要重新尝试写
			if(pPacket->isCursorEnd()){
				pPacket->release();		// 释放
			}else{
				// 没有写完的消息进入队列
				m_packetQueue.push_front(pPacket);
				// 进入epoll等待
				getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
			}
		}
	}

	return true;
}
bool Accept::sendPacket(const char* ptr, int length){
	if( !m_packetQueue.empty() ){
		Packet* pPacket = new Packet(length);
		pPacket->write(ptr, length);
		pPacket->resetCursor();		// 后面的写操作需要重置
		pPacket->retain();			// 进入队列前引用
		pPacket->recordLength();
		// 已经在epoll中等待out事件
		m_packetQueue.push_back(pPacket);
	}else{
		// 先执行写操作，如果出现重试，那么再将Packet入队
		int writeLength = 0;
		int result = writeSocket(ptr, length, &writeLength);
		if( result < 0 ){
		    LOG_ERROR("writeSocket result < 0 remove client");
			epollRemove();
		}else{
			// result == 0 成功写 || result > 0 需要重新尝试写
			if(length == writeLength){
				// write success end, do nothing
			}else{
				int leftLength;
				if(result == 0){
					leftLength = length - writeLength;
				}else{
					leftLength = length;
				}
				Packet* pPacket = new Packet(leftLength);
				pPacket->write(ptr + writeLength, leftLength);
				pPacket->resetCursor();		// 后面的写操作需要重置
				pPacket->retain();			// 进入队列前引用
				pPacket->recordLength();
				// 没有写完的消息进入队列
				m_packetQueue.push_front(pPacket);
				// 进入epoll等待
				getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
			}
		}
	}

	return true;
}
void Accept::resetData(void){
	setConnectionState(CS_DISCONNECT);
//	setPingTime(0);
	SAFE_RELEASE(m_tempReadPacket)
	clearTimer();		// 停止计时器
	closeSocket();		// 关闭套接字
	releasePacket();	// 取消所有数据包的发送
	m_isNeedEncrypt = false;
	m_isNeedDecrypt = false;
	m_bindNodeID = 0;
	m_tempLength = 0;
}
void Accept::dispatchPacket(Packet* pPacket, uint32 command){
//	LOG_DEBUG("Accept handle=%d dispatchPacket command=%d", getHandle(), command);
	// 对收到的消息进行解密处理：从body开始解密；头部已经在判断长度的时候解密
	if( this->isNeedDecrypt() ){
		LOG_DEBUG("Accept handle=%d dispatchPacket command=%d need decrypt", getHandle(), command);
		if( pPacket->getLength() >= (int)PACKET_HEAD_LENGTH ){
			binary_decrypt(pPacket->getBody(), pPacket->getBodyLength(), MainWorker::getInstance()->getKey());
		}
	}
	// 判断当前的连接是否已经验证
	if(getConnectionState() < CS_IDENTIFY_OK && getType() == CONN_TYPE_ACCEPT && command != COMMAND_REGISTER){
		LOG_ERROR("Accept is not identify yet. close the connection handle=%d command=%d", getHandle(), command);
		epollRemove();
		return;
	}
	MainWorker::getInstance()->pushPacketToQueue(this, pPacket);
}
int Accept::readSocket(void){
    if( this->getSocketFD() <= 0 ){
        return -1;
    }
	char* recvBuffer = MainWorker::getInstance()->getReadBuffer();
	int nread;
	if(m_tempLength > 0){
        memcpy(recvBuffer, m_tempHead, m_tempLength);
	}else{
	    m_tempLength = 0;
	}
	nread = read(this->getSocketFD(), recvBuffer + m_tempLength, EPOLL_READ_BUFFER_SIZE);
//	nread = recv(this->getSocketFD(), recvBuffer + m_tempLength, EPOLL_READ_BUFFER_SIZE, 0);
    if(nread < 0){
        switch(errno){
        case EINTR: return 1; 	// 读数据失败，处理信号中断
        case EAGAIN: return 2;	// 可以下次重新调用
        default: return -1;
        }
        return -1;
    }else if(nread == 0){
        return -1;
    }
    if(m_tempLength > 0){
        nread += m_tempLength;
        m_tempLength = 0;
    }
    onAcceptRead(this, recvBuffer, nread);
    this->setOnline(true);
    return 0;
}
int Accept::writeSocket(Packet* pPacket){
	// 检查是否已经经过加密操作
	if( pPacket->getBuffer()->checkEncryptFlag() ){
	    pPacket->recordLength();
	    pPacket->convertHead();
		if( this->isNeedEncrypt() ){
			binary_encrypt(pPacket->getDataPtr(), PACKET_HEAD_LENGTH_PROTECT, MainWorker::getInstance()->getKey());
			binary_encrypt(pPacket->getBody(), pPacket->getBodyLength(), MainWorker::getInstance()->getKey());
		}
	}
	int nwrite;
	nwrite = write(this->getSocketFD(), pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor());
//	LOG_INFO("write socket, fd=%d, packet length=%d commond=%u des=%u res=%u message=0x%x uid=%d callbackID=%u, send length:%d",
//			this->getSocketFD(), pPacket->getLength(), pPacket->getCommand(), pPacket->getDestination(), pPacket->getSource(),
//			pPacket->getMessage(), pPacket->getUID(), pPacket->getCallback(), nwrite);
//    nwrite = send(this->getSocketFD(), pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor(), 0);
    if(nwrite < 0){
        switch(errno){
        case EINTR: return 1; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
            LOG_INFO("write EAGAIN capture\n");
            return 2;
        default:
            LOG_INFO("write errno=%d\n", errno);
            return -1;
        }
        return -1;
    }
    pPacket->moveCursor( nwrite );// used
    return 0;
}
int Accept::writeSocket(const char* ptr, int length, int* writeLength){
    int nwrite;
	nwrite = write(this->getSocketFD(), ptr, length);
	LOG_INFO("write socket, fd=%d packet length=%d send length:%d", 
			this->getSocketFD(), length, nwrite);
//    nwrite = send(this->getSocketFD(), ptr, length, 0);
    if(nwrite < 0){
        switch(errno){
        case EINTR: return 1; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
            LOG_INFO("write EAGAIN capture\n");
            return 2;
        default:
            LOG_INFO("write errno=%d\n", errno);
            return -1;
        }
        return -1;
    }
    *writeLength = nwrite;
    return 0;
}

NS_HIVE_END
