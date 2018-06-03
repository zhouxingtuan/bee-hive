//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/7/28
// Time: 上午9:14
// To change this template use File | Settings | File Templates.
//

#include "mainworker.h"

NS_HIVE_BEGIN

DEFINE_INSTANCE_CPP(MainWorker)

MainWorker::MainWorker(void) : RefObject(),
    m_pEpoll(NULL), m_pTimer(NULL), m_pHandler(NULL), m_pListenerPool(NULL), m_pAcceptPool(NULL),
    m_pClientPool(NULL), m_pHttpPool(NULL), m_pHttpsPool(NULL), m_pMultiCurl(NULL),
    m_pPingPacket(NULL), m_pSSLCTX(NULL) {

}
MainWorker::~MainWorker(void){

}
void MainWorker::onAcceptSocket(int fd, const char* ip, uint16 port, Listener* pListener){
	MainWorker::getInstance()->openAccept(fd, ip, port, pListener->isNeedEncrypt(), pListener->isNeedDecrypt());
}
void MainWorker::onAcceptHttp(int fd, const char* ip, uint16 port, Listener* pListener){
	MainWorker::getInstance()->openHttp(fd, ip, port);
}
void MainWorker::onAcceptHttps(int fd, const char* ip, uint16 port, Listener* pListener){
	MainWorker::getInstance()->openHttps(fd, ip, port);
}
int64 MainWorker::checkConnectIdentify(Accept* pAccept){
	if(pAccept->getConnectionState() < CS_IDENTIFY_OK){
		LOG_ERROR("Accept is not CS_IDENTIFY_OK and remove");
		pAccept->epollRemove();
	}else{
		pAccept->setOnline(true);
		Timer* pTimer = (MainWorker::getInstance()->getTimer());
		pAccept->setTimeout(CONNECT_ONLINE_TIME, pTimer, MainWorker::checkConnectOnline);
	}
	return -1;
}
int64 MainWorker::checkConnectOnline(Accept* pAccept){
	if(!pAccept->isOnline()){
		LOG_ERROR("Accept is not online and remove handle=%d", pAccept->getHandle());
		pAccept->epollRemove();
		return -1;
	}
	pAccept->setOnline(false);
	return CONNECT_ONLINE_TIME;
}
int64 MainWorker::keepConnectOnline(Accept* pAccept){
	Packet* pPing = new Packet(MainWorker::getInstance()->m_pPingPacket->getBuffer());
	pPing->retain();
	pAccept->sendPacket(pPing);
	pPing->release();
	return CONNECT_KEEP_ONLINE_TIME;
}

void MainWorker::onReceiveHttp(Http* pHttp, Packet* pPacket){
	m_pHandler->onReceiveHttp(pHttp, pPacket);
}
bool MainWorker::onReceiveAccept(Accept* pAccept, Packet* pPacket){
	return m_pHandler->onReceiveAccept(pAccept, pPacket);
}
Http* MainWorker::openHttp(int fd, const char* ip, uint16 port){
	Http* pHttp = m_pHttpPool->create();
	if(NULL == pHttp){
		LOG_ERROR("create http NULL == pHttp");
		return 0;
	}
	pHttp->initialize();
	pHttp->setEpoll(m_pEpoll);
	pHttp->setSocketFD(fd);
	pHttp->setSocket(ip, port);
	pHttp->setState(HTTP_STATE_READ);
	pHttp->setTimer(HTTP_HANDLE_TIMEOUT, getTimer());
	LOG_DEBUG("handle=%d", pHttp->getHandle());
	// 读取数据
	pHttp->epollIn();
	return pHttp;
}
Http* MainWorker::getHttp(uint32 handle){
	return m_pHttpPool->get(handle);
}
bool MainWorker::closeHttp(Http* pHttp){
	LOG_DEBUG("handle=%d", pHttp->getHandle());
	m_pHandler->onCloseHttp(pHttp);
	pHttp->resetData();
    return m_pHttpPool->idle(pHttp);
}
Https* MainWorker::openHttps(int fd, const char* ip, uint16 port){
	Https* pHttps = m_pHttpsPool->create();
	if(NULL == pHttps){
		LOG_ERROR("create https NULL == pHttps");
		return 0;
	}
	pHttps->initialize();
	pHttps->setEpoll(m_pEpoll);
	pHttps->setSocketFD(fd);
	pHttps->setSocket(ip, port);
	pHttps->setState(HTTP_STATE_READ);
	pHttps->setTimer(HTTP_HANDLE_TIMEOUT, getTimer());
	if( !pHttps->bindSSL(m_pSSLCTX) ){
		closeHttps(pHttps);
		LOG_ERROR("bind ssl failed fd=%d", fd);
		return 0;
	}
	LOG_DEBUG("handle=%d", pHttps->getHandle());
	// 读取数据
	pHttps->epollIn();
	return pHttps;
}
Https* MainWorker::getHttps(uint32 handle){
	return m_pHttpsPool->get(handle);
}
bool MainWorker::closeHttps(Https* pHttps){
	LOG_DEBUG("handle=%d", pHttps->getHandle());
	m_pHandler->onCloseHttp(pHttps);
	pHttps->resetData();
    return m_pHttpsPool->idle(pHttps);
}
Client* MainWorker::openClient(const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt){
	Client* pClient = m_pClientPool->create();
	if(NULL == pClient){
		LOG_ERROR("create client NULL == pClient");
		return 0;
	}
	pClient->setEpoll(m_pEpoll);
	pClient->setSocket(ip, port);
	pClient->setConnectionState(CS_CONNECT_START);
	if( !pClient->connectServer() ){
		LOG_ERROR("Client::connectServer failed");
		closeClient(pClient);
		return 0;
	}
	// 添加当前的socket到epoll中进行监听
	if( !m_pEpoll->objectAdd(pClient, EPOLLIN | EPOLLOUT) ){
		LOG_ERROR("Epoll::objectAdd failed");
		closeClient(pClient);
		return 0;
	}
	pClient->setIsNeedEncrypt(isNeedEncrypt);
	pClient->setIsNeedDecrypt(isNeedDecrypt);
	LOG_DEBUG("openClient handle=%d ip=%s port=%d", pClient->getHandle(), ip, port);
	return pClient;
}
Client* MainWorker::getClient(uint32 handle){
	return m_pClientPool->get(handle);
}
void MainWorker::receiveClient(Client* pClient){
	ClientHandler* pClientHandler = pClient->getClientHandler();
	if(NULL != pClientHandler){
		pClientHandler->onReceiveClient(pClient);
		return;
	}
	m_pHandler->onReceiveClient(pClient);
}
bool MainWorker::closeClient(Client* pClient){
	LOG_DEBUG("handle=%d", pClient->getHandle());
	m_pHandler->onCloseClient(pClient);
	m_pEpoll->objectRemove(pClient);
	pClient->resetData();
	return m_pClientPool->idle(pClient);
}
Accept* MainWorker::openAccept(int fd, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt){
	LOG_DEBUG("fd=%d ip=%s port=%d", fd, ip, port);
	// 获取一个连接对象Accept，将对象一并加入到epoll中
	Accept* pAccept = m_pAcceptPool->create();
	if(NULL == pAccept){
		close(fd);
		LOG_ERROR("create accept NULL == pAccept");
		return 0;
	}
	pAccept->setEpoll(m_pEpoll);
	pAccept->setSocketFD( fd );
	pAccept->setSocket( ip, port );
	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1){
		pAccept->closeSocket();
		closeAccept(pAccept);
		return 0;
	}
//	int nRecvBuf=EPOLL_READ_BUFFER_SIZE;
//    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
//    int nSendBuf=EPOLL_READ_BUFFER_SIZE;
//    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
	if( !m_pEpoll->objectAdd(pAccept, EPOLLIN) ){
		pAccept->closeSocket();
		closeAccept(pAccept);
		return 0;
	}
	pAccept->setConnectionState(CS_CONNECT_OK);
	pAccept->setTimeout(CONNECT_IDENTIFY_TIME, m_pTimer, MainWorker::checkConnectIdentify);
	pAccept->setIsNeedEncrypt(isNeedEncrypt);
	pAccept->setIsNeedDecrypt(isNeedDecrypt);
	// 发送消息，通知有连接到来？
	LOG_DEBUG("handle=%d fd=%d ip=%s port=%d", pAccept->getHandle(), fd, ip, port);
	return pAccept;
}
Accept* MainWorker::getAccept(uint32 handle){
	return m_pAcceptPool->get(handle);
}
bool MainWorker::closeAccept(Accept* pAccept){
	LOG_DEBUG("handle=%d", pAccept->getHandle());
	m_pHandler->onCloseAccept(pAccept);
	m_pEpoll->objectRemove(pAccept);
	pAccept->resetData();
	return m_pAcceptPool->idle(pAccept);
}
Listener* MainWorker::openHttpListener(const char* ip, uint16 port){
	return this->openListener(ip, port, MainWorker::onAcceptHttp, false, false);
}
Listener* MainWorker::openHttpsListener(const char* ip, uint16 port){
	return this->openListener(ip, port, MainWorker::onAcceptHttps, false, false);
}
Listener* MainWorker::openSocketListener(const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt){
	return this->openListener(ip, port, MainWorker::onAcceptSocket, isNeedEncrypt, isNeedDecrypt);
}
Listener* MainWorker::openListener(const char* ip, uint16 port, AcceptSocketFunction pFunc, bool isNeedEncrypt, bool isNeedDecrypt){
	LOG_DEBUG("try to open Listener ip=%s port=%d", ip, port);
	Listener* pListener = (Listener*)m_pListenerPool->create();
	if(NULL == pListener){
		LOG_ERROR("NULL == pListener");
		return NULL;
	}
	pListener->setEpoll(m_pEpoll);
	pListener->setSocket(ip, port);
	pListener->setAcceptSocketFunction(pFunc);
	if( !pListener->openSocket() ){
		closeListener(pListener);
		LOG_ERROR("Listener openSocket failed");
		return NULL;
	}
	if( !m_pEpoll->objectAdd(pListener, EPOLLIN) ){
		pListener->closeSocket();
		closeListener(pListener);
		LOG_ERROR("Listener objectAdd to epoll failed");
		return NULL;
	}
	pListener->setIsNeedEncrypt(isNeedEncrypt);
	pListener->setIsNeedDecrypt(isNeedDecrypt);
	LOG_INFO("handle=%d fd=%d ip=%s port=%d", pListener->getHandle(), pListener->getSocketFD(), ip, port);
	return pListener;
}
bool MainWorker::closeListener(Listener* pListener){
	LOG_DEBUG("handle=%d", pListener->getHandle());
	m_pHandler->onCloseListener(pListener);
	m_pEpoll->objectRemove(pListener);
	pListener->resetData();
	return m_pListenerPool->idle(pListener);
}
bool MainWorker::sendCurlRequest(RequestData* pRequest){
	return m_pMultiCurl->acceptRequest(pRequest);
}
void MainWorker::onCurlResponse(Buffer* pBuffer, uint32 nodeID, uint32 callbackID, bool isRequestOK){
	m_pHandler->onCurlResponse(pBuffer, nodeID, callbackID, isRequestOK);
}
bool MainWorker::dispatchMessage(Packet* pPacket){
	return m_pHandler->dispatchMessage(pPacket);
}
uint32 MainWorker::startTimer(uint32 callbackID, int64 timeCount, uint32 nodeID){
	return m_pHandler->startTimer(callbackID, timeCount, nodeID);
}
void MainWorker::removeTimer(uint32 handle){
	m_pHandler->removeTimer(handle);
}
void MainWorker::changeTimer(uint32 handle, int64 timeCount){
	m_pHandler->changeTimer(handle, timeCount);
}
int64 MainWorker::getTimerLeft(uint32 handle){
	return m_pHandler->getTimerLeft(handle);
}

void MainWorker::pushPacketToQueue(Accept* pAccept, Packet* pPacket){
    pPacket->retain();
    PacketQueueInfo info;
    info.pAccept = pAccept;
    info.pPacket = pPacket;
    m_packetQueue.push_back(info);
}
void MainWorker::dispatchPacket(void){
    Client* pClient;
    ClientHandler* pClientHandler;
    Accept* pAccept;
    Packet* pPacket;
    AcceptCommandFunction func;
    uint32 command;
    for(auto &info : m_packetQueue){
        pAccept = info.pAccept;
        pPacket = info.pPacket;
        if( pAccept->getType() == CONN_TYPE_CLIENT ){
			pClient = (Client*)pAccept;
			pClientHandler = pClient->getClientHandler();
			if( NULL != pClientHandler ){
				pClientHandler->onReceivePacket(pClient, pPacket);
				pPacket->release();
				continue;
			}
        }
        command = pPacket->getCommand();
		func = getAcceptCommandFunction(command);
		if(NULL == func){
			LOG_ERROR("Accept handle=%d dispatchPacket command=%d function not found.", pAccept->getHandle(), command);
        }else{
            func(pAccept, pPacket, command);
        }
        pPacket->release();
    }
    m_packetQueue.clear();
}
void MainWorker::update(void){
	LOG_INFO("start main handle=%d", m_pHandler->getModuleHandle());
	int64 timeout;
	while(1){
		timeout = m_pTimer->getWaitTimeout();
		m_pEpoll->update(timeout);
		dispatchPacket();
		m_pTimer->update();
	}
	LOG_INFO("exit main handle=%d", m_pHandler->getModuleHandle());
}
void MainWorker::initialize(Handler* pHandler){
	m_pHandler = pHandler;
	if(NULL == m_pEpoll){
		m_pEpoll = new Epoll();
		m_pEpoll->retain();
		if( !m_pEpoll->createEpoll() ){
			LOG_ERROR("createEpoll failed");
			sleep(1);
			exit(0);
		}
	}
	if(NULL == m_pTimer){
		m_pTimer = new Timer();
		m_pTimer->retain();
		m_pTimer->start(MAX_0824_HASH_SIZE);
	}
	if(NULL == m_pListenerPool){
		m_pListenerPool = new ListenerPool();
		m_pListenerPool->retain();
	}
	if(NULL == m_pAcceptPool){
		m_pAcceptPool = new AcceptPool();
		m_pAcceptPool->retain();
		m_pAcceptPool->setMaxHashNumber(MAX_0824_HASH_SIZE);
	}
	if(NULL == m_pClientPool){
		m_pClientPool = new ClientPool();
		m_pClientPool->retain();
		m_pClientPool->setMaxHashNumber(MAX_0824_HASH_SIZE);
	}
	if(NULL == m_pHttpPool){
		m_pHttpPool = new HttpPool();
		m_pHttpPool->retain();
		m_pHttpPool->setMaxHashNumber(MAX_0824_HASH_SIZE);
	}
	if(NULL == m_pHttpsPool){
		m_pHttpsPool = new HttpsPool();
		m_pHttpsPool->retain();
		m_pHttpsPool->setMaxHashNumber(MAX_0824_HASH_SIZE);
	}
	if(NULL == m_pMultiCurl){
		m_pMultiCurl = new MultiCurl();
		m_pMultiCurl->retain();
		m_pMultiCurl->setEpoll(m_pEpoll);
		m_pMultiCurl->initialize();
	}
	if(NULL == m_pPingPacket){
		m_pPingPacket = new Packet(PACKET_HEAD_LENGTH);
		m_pPingPacket->retain();
		m_pPingPacket->writeBegin(COMMAND_PING);
		m_pPingPacket->writeEnd();
	}
}
void MainWorker::initializeHttp(void){
	http_parser_settings_init(&m_settings);
	m_settings.on_message_begin 	= Http::onMessageBegin;
	m_settings.on_url 				= Http::onUrl;
	m_settings.on_header_field 		= Http::onHeaderField;
	m_settings.on_header_value 		= Http::onHeaderValue;
	m_settings.on_headers_complete 	= Http::onHeadersComplete;
	m_settings.on_body 				= Http::onBody;
	m_settings.on_message_complete 	= Http::onMessageComplete;
}
void MainWorker::initializeHttps(void){
	initializeHttp();
	// 初始化https数据
	/* SSL 库初始化 */
	SSL_library_init();
	/* 载入所有 SSL 算法 */
	OpenSSL_add_all_algorithms();
	/* 载入所有 SSL 错误消息 */
	SSL_load_error_strings();
	/* 以 SSLV2 和 V3 标准兼容方式产生一个 SSL_CTX ,即 SSLContentText*/
	m_pSSLCTX = SSL_CTX_new(SSLv23_server_method());
	/* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
	if (m_pSSLCTX == NULL) {
		ERR_print_errors_fp(stderr);
	}else{
		LOG_INFO("SSL_CTX_new OK");
	}
	const std::string& publicKey = getPublicKey();
	const std::string& privateKey = getPrivateKey();
	if( !initHttpsCertificate(publicKey.c_str(), privateKey.c_str()) ){
		LOG_ERROR("eoll init https key error handle=%d", m_pHandler->getModuleHandle());
	}
}
void MainWorker::destroy(void){
	/* 释放 CTX */
	if(NULL != m_pSSLCTX){
		SSL_CTX_free(m_pSSLCTX);
		m_pSSLCTX = NULL;
	}
	SAFE_RELEASE(m_pAcceptPool)
	SAFE_RELEASE(m_pClientPool)
	SAFE_RELEASE(m_pHttpPool)
	SAFE_RELEASE(m_pHttpsPool)
	SAFE_RELEASE(m_pMultiCurl)
	SAFE_RELEASE(m_pListenerPool)
	SAFE_RELEASE(m_pEpoll)
	SAFE_RELEASE(m_pTimer)
}
bool MainWorker::initHttpsCertificate(const char* publicKey, const char* privateKey){
	if (m_pSSLCTX == NULL) {
		LOG_ERROR("m_pSSLCTX == NULL");
		return false;
	}
	/* 载入用户的数字证书, 此证书用来发送给客户端。 证书里包含有公钥 */
	if (SSL_CTX_use_certificate_file(m_pSSLCTX, publicKey, SSL_FILETYPE_PEM) <= 0) {
    	ERR_print_errors_fp(stderr);
    	return false;
    }
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(m_pSSLCTX, privateKey, SSL_FILETYPE_PEM) <= 0) {
    	ERR_print_errors_fp(stderr);
    	return false;
    }
    /* 检查用户私钥是否正确 */
    if (!SSL_CTX_check_private_key(m_pSSLCTX)) {
    	ERR_print_errors_fp(stderr);
    	return false;
    }
    return true;
}


NS_HIVE_END
