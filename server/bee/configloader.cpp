//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/2
// Time: 下午4:57
// To change this template use File | Settings | File Templates.
//

#include "configloader.h"
#include "commandfunction.h"
#include "beehandler.h"

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
	const std::string&	hive_addr 			= config["hive_addr"];
	int 				hive_encrypt 		= atoi(config["hive_encrypt"].c_str());
	int 				hive_decrypt 		= atoi(config["hive_decrypt"].c_str());

	// # the node id of current service (1-255)
	int 				module_type 		= atoi(config["module_type"].c_str());
	int 				module_index 		= atoi(config["module_index"].c_str());
	const std::string& 	module_ref_index 	= config["module_ref_index"];
	const std::string& 	module_name 	    = config["module_name"];
	const std::string& 	module_param 	    = config["module_param"];
//	const std::string& 	netkey 				= config["netkey"];
//	const std::string& 	password 			= config["password"];
	int             	log_level 			= atoi(config["log_level"].c_str());
	int             	log_max_size 		= atoi(config["log_max_size"].c_str());
	const std::string& 	log_file 			= config["log_file"];

	// # listen global ip and port for hive; start with 'global'
	std::string 	    global_addr 		= "";
	int 				global_encrypt 		= 0;
	int 				global_decrypt 		= 0;

	// # listen http ip and port; start with 'http'
	std::string 	    http_addr 			= "";

	// # listen https ip and port; start with 'https'
	std::string 	    https_addr 			= "";
	std::string 	    https_public 		= "";
	std::string 	    https_private 		= "";

	for(auto &itCur : config){
		if(itCur.first == "global_addr"){
			global_addr = itCur.second;
		}else if(itCur.first == "global_encrypt"){
			global_encrypt = atoi(itCur.second.c_str());
		}else if(itCur.first == "global_decrypt"){
			global_decrypt = atoi(itCur.second.c_str());
		}else if(itCur.first == "http_addr"){
			http_addr = itCur.second;
		}else if(itCur.first == "https_addr"){
			https_addr = itCur.second;
		}else if(itCur.first == "https_public"){
			https_public = itCur.second;
		}else if(itCur.first == "https_private"){
			https_private = itCur.second;
		}
	}

	// 解析 module_ref_index 数组
	Token::TokenVector refStringArray;
	Token::splitArray(module_ref_index, ",", refStringArray);

	// init the core
	setLogLevel(log_level);
	setMaxLogSize(log_max_size);
	BeeHandler* pBeeHandler = BeeHandler::getInstance();
	std::set<uint32> indexSet;
	for(auto &str : refStringArray){
		int refIndex = atoi(str.c_str());
		if(refIndex >= 0){
			indexSet.insert((uint32)refIndex);
		}else{
			LOG_ERROR("error refIndex=%d", refIndex);
		}
	}
	RefVector& m_refIndex = pBeeHandler->m_refIndex;
	m_refIndex.insert(m_refIndex.begin(), indexSet.begin(), indexSet.end());
	pBeeHandler->setModuleType(module_type);
	pBeeHandler->setModuleIndex(module_index);
	pBeeHandler->setModuleName(module_name);
	pBeeHandler->setModuleParam(module_param);
	pBeeHandler->setLogFile(log_file);
	MainWorker::getInstance()->setKey("d27796b6bcfb584ee678c7f74782d461");
	MainWorker::getInstance()->setPassword("775b26b9c439ae11d878d7b637e83a18");

	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BROADCAST, onCommandBroadcast);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_NODE_ERROR, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_NODE_REQUEST, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_NODE_RESPONSE, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_CLIENT_REQUEST, onCommandDispatchByClient);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_REQUEST_TIMEOUT, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_HTTP_REQUEST, onCommandDispatchByHandle);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_PING, onCommandPing);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_PONG, onCommandPong);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_REGISTER, onCommandRegister);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_RESPONSE, onCommandResponse);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BEE_RESPONSE, onCommandBeeResponse);
	MainWorker::getInstance()->setAcceptCommandFunction(COMMAND_BEE_KICKOFF, onCommandBeeKickoff);

	MainWorker::getInstance()->initialize(pBeeHandler);

	// record main init data
	parseIPAndPort(hive_addr, pBeeHandler->m_hiveIP, pBeeHandler->m_hivePort);
	pBeeHandler->m_hiveEncrypt = (hive_encrypt > 0);
	pBeeHandler->m_hiveDecrypt = (hive_decrypt > 0);

	if( !global_addr.empty() ){
		parseIPAndPort(global_addr, pBeeHandler->m_globalIP, pBeeHandler->m_globalPort);
    	pBeeHandler->m_globalEncrypt = (global_encrypt > 0);
    	pBeeHandler->m_globalDecrypt = (global_decrypt > 0);
	}

	if( !http_addr.empty() ){
		parseIPAndPort(http_addr, pBeeHandler->m_httpIP, pBeeHandler->m_httpPort);
		MainWorker::getInstance()->initializeHttp();
	}

	MainWorker::getInstance()->setPublicKey(https_public);
	MainWorker::getInstance()->setPrivateKey(https_private);
	if( !https_addr.empty() ){
		parseIPAndPort(https_addr, pBeeHandler->m_httpsIP, pBeeHandler->m_httpsPort);
		MainWorker::getInstance()->initializeHttps();
	}

	// 初始化
	pBeeHandler->onInitialize();
}

NS_HIVE_END
