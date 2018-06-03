//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/22
// Time: 上午8:39
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__log__
#define __hive__log__

#include "common.h"

NS_HIVE_BEGIN

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_ERROR 2

const int FMT_BUFFER_SIZE = 1024*1024;

extern void setMaxLogSize(int logSize);
extern int getMaxLogSize(void);
extern void setLogLevel(int level);
extern int getLogLevel(void);
extern const char* getTimeString(void);
extern const char* getTimeStringUS(void);
extern char* getFmtBuffer(void);
extern bool openCppLog(const std::string& fileName);
extern const std::string& getCppLogFileName(void);
extern void writeCppLog(const char* str);
extern void writeLog(const char* fileName, const char* str);

#define LOG_DEBUG(fmt, args...)\
do{\
	if( getLogLevel() > LOG_LEVEL_DEBUG ){\
		break;\
	}\
	char* ptr = getFmtBuffer();\
	snprintf(ptr, FMT_BUFFER_SIZE, "[DEBUG %s][%s:%s():%d] " fmt "\n", getTimeStringUS(), __FILE__, __FUNCTION__, __LINE__, ##args);\
	writeCppLog(ptr);\
}while(0);

#define LOG_INFO(fmt, args...)\
do{\
	if( getLogLevel() > LOG_LEVEL_INFO ){\
		break;\
	}\
	char* ptr = getFmtBuffer();\
	snprintf(ptr, FMT_BUFFER_SIZE, "[INFO  %s][%s:%s():%d] " fmt "\n", getTimeStringUS(), __FILE__, __FUNCTION__, __LINE__, ##args);\
	writeCppLog(ptr);\
}while(0);

#define LOG_ERROR(fmt, ...)\
do{\
	if( getLogLevel() > LOG_LEVEL_ERROR ){\
		break;\
	}\
	char* ptr = getFmtBuffer();\
	snprintf(ptr, FMT_BUFFER_SIZE, "[ERROR %s][%s:%s():%d] " fmt "\n", getTimeStringUS(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	writeCppLog(ptr);\
}while(0);

NS_HIVE_END

#endif
