//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/2
// Time: 下午4:57
// To change this template use File | Settings | File Templates.
//

#include "configloader.h"
#include "commandfunction.h"
#include "hivehandler.h"

NS_HIVE_BEGIN

void parseIPAndPort(const std::string& ip_port_str, std::string& ip, uint16& port){
	Token::TokenMap ip_port_map;
	Token::split(ip_port_str, ":", ip_port_map);
	for(auto &kv : ip_port_map){
		ip = kv.first;
		port = atoi(kv.second.c_str());
		LOG_DEBUG("ip=%s port=%d", ip.c_str(), port);
		return;
	}
}

void parseConfig(const char* fileName, Token::TokenMap& config){
    Token::readConfig(fileName, config);
}
void loadConfig(Token::TokenMap& config){
	// # the discovery node to join
	int 				des_id 				= atoi(config["des_id"].c_str());
	const std::string&	des_addr 			= config["des_addr"];
	int 				des_encrypt 		= atoi(config["des_encrypt"].c_str());
	int 				des_decrypt 		= atoi(config["des_decrypt"].c_str());

	// # the node id of current service (1-255)
	int 				hive_id 			= atoi(config["hive_id"].c_str());
//	const std::string& 	netkey 				= config["netkey"];
//	const std::string& 	password 			= config["password"];
	int             	log_level 			= atoi(config["log_level"].c_str());
	int             	log_max_size 		= atoi(config["log_max_size"].c_str());
	const std::string& 	log_file 			= config["log_file"];

	// # local ip and port to connect for bee
	const std::string& 	local_addr 			= config["local_addr"];
	int 				local_encrypt 		= atoi(config["local_encrypt"].c_str());
	int 				local_decrypt 		= atoi(config["local_decrypt"].c_str());

	// # listen global ip and port for hive; start with 'global'
	const std::string& 	global_addr 		= config["global_addr"];
	int 				global_encrypt 		= atoi(config["global_encrypt"].c_str());
	int 				global_decrypt 		= atoi(config["global_decrypt"].c_str());

	// init the core
	setLogLevel(log_level);
	setMaxLogSize(log_max_size);
	HiveHandler* pHiveHandler = HiveHandler::getInstance();
	pHiveHandler->setModuleHandle(hive_id);
	pHiveHandler->setLogFile(log_file);
	MainWorker::getInstance()->setKey("d27796b6bcfb584ee678c7f74782d461");
	MainWorker::getInstance()->setPassword("775b26b9c439ae11d878d7b637e83a18");

	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BROADCAST, onCommandBroadcast);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BROADCAST_BEGIN, onCommandBroadcastBegin);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_NODE_ERROR, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_NODE_REQUEST, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_NODE_RESPONSE, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_CLIENT_REQUEST, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_REQUEST_TIMEOUT, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_HTTP_REQUEST, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_PING, onCommandPing);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_PONG, onCommandPong);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_REGISTER, onCommandRegister);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_RESPONSE, onCommandResponse);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_HIVE_REGISTER, onCommandHiveRegister);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_HIVE_RESPONSE, onCommandHiveResponse);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BEE_REGISTER, onCommandBeeRegister);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BEE_RESPONSE, onCommandBeeResponse);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BEE_ONLINE, onCommandBeeOnline);
//	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BEE_OFFLINE, onCommandBeeOffline);

	MainWorker::getInstance()->initialize(pHiveHandler);

	// record main init data
	pHiveHandler->m_destID = des_id;
	parseIPAndPort(des_addr, pHiveHandler->m_destIP, pHiveHandler->m_destPort);
	pHiveHandler->m_destEncrypt = (des_encrypt > 0);
	pHiveHandler->m_destDecrypt = (des_decrypt > 0);

	parseIPAndPort(local_addr, pHiveHandler->m_localIP, pHiveHandler->m_localPort);
	pHiveHandler->m_localEncrypt = (local_encrypt > 0);
	pHiveHandler->m_localDecrypt = (local_decrypt > 0);

	parseIPAndPort(global_addr, pHiveHandler->m_globalIP, pHiveHandler->m_globalPort);
	pHiveHandler->m_globalEncrypt = (global_encrypt > 0);
	pHiveHandler->m_globalDecrypt = (global_decrypt > 0);

	// 初始化
	pHiveHandler->onInitialize();
}

NS_HIVE_END
