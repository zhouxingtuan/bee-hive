//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/7/9
// Time: 上午11:39
// To change this template use File | Settings | File Templates.
//

#include "listener.h"
#include "mainworker.h"

NS_HIVE_BEGIN

Listener::Listener(void) : EpollConnectObject(), Object1616(),
	m_acceptSocketFunction(NULL), m_isNeedEncrypt(false), m_isNeedDecrypt(false) {

}
Listener::~Listener(void){

}
bool Listener::epollActive(uint32 events){
	struct sockaddr_in cliaddr;
	socklen_t socklen = sizeof(struct sockaddr_in);
	int fd;
	while( (fd = accept(getSocketFD(), (struct sockaddr*)&cliaddr, &socklen)) > 0 ){
		// 这里接收到新的连接，处理新的连接事件
		m_acceptSocketFunction(fd, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port, this);
	};
	if( fd == -1 ){
		switch(errno){
			case EAGAIN:
			case EINTR:
			case ECONNABORTED:
			case EPROTO:
				epollCheck();
				break;
			default:
				// 这里说明socket出Bug了，需要进行处理；移除这个Listener
				epollRemove();
				break;
		}
	}
	return true;
}
bool Listener::epollIn(void){
    return true;
}
bool Listener::epollOut(void){
    return true;
}
void Listener::epollRemove(void){
	LOG_DEBUG("handle=%d\n", this->getHandle());
	MainWorker::getInstance()->closeListener(this);
}
void Listener::epollCheck(void){
//	fprintf(stderr, "--Listener::epollCheck\n");
	getEpoll()->objectChange(this, EPOLLIN);
}
void Listener::resetData(void){
	closeSocket();
	m_isNeedEncrypt = false;
	m_isNeedDecrypt = false;
}
bool Listener::openSocket(void){
    int opt = 1;    // 是否复用标记 0不复用 1复用
    int fd;
    struct sockaddr_in servaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    bzero(&servaddr, socklen);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr( this->getIP() );//htonl(INADDR_ANY);
    servaddr.sin_port = htons( this->getPort() );
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
		LOG_ERROR("failed ip(%s) port(%d)", this->getIP(), this->getPort());
		return false;
    }
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1){
		LOG_ERROR("setNonBlocking to listen socket failed ip(%s) port(%d)", this->getIP(), this->getPort());
		goto LISTEN_FAILED;
    }
    if(bind(fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1){
		LOG_ERROR("bind listen socket error ip(%s) port(%d)", this->getIP(), this->getPort());
		goto LISTEN_FAILED;
    }
    if(listen(fd, MAX_LISTEN_SIZE) == -1){
  		LOG_ERROR("socket listen error ip(%s) port(%d)", this->getIP(), this->getPort());
		goto LISTEN_FAILED;
    }
    LOG_INFO("OK ip=%s port=%d", this->getIP(), this->getPort());
    this->setSocketFD(fd);
    return true;
LISTEN_FAILED:
    close(fd);
    return false;
}

NS_HIVE_END
