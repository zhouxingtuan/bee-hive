//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/22
// Time: 上午8:39
// To change this template use File | Settings | File Templates.
//

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

//thread libs
#include <pthread.h>
#include <semaphore.h>

#include <sys/io.h>
#include <fcntl.h>

#include "log.h"

NS_HIVE_BEGIN

class LogFile
{
public:
	std::string m_fileName;		// 文件名
	int64 m_fileLength;			// 文件长度
	int m_fileHandle;			// linux下文件句柄
public:
	LogFile(const std::string& name, const std::string& ext) : m_fileName(name+ext), m_fileLength(0), m_fileHandle(0) {}
	virtual ~LogFile(void){
		closeReadWrite();
	}
public:
    inline bool openLog(void){
        if( !openReadWrite("ab+") ){
            return false;
        }
        if( !openReadWrite("rb+") ){
            return false;
        }
        fileSeek(0, SEEK_END);
        return true;
    }
	inline bool renameLog(const char* newName){
	    if( rename(m_fileName.c_str(), newName) < 0 ){
	        return false;
	    }
	    return true;
	}
    inline bool writeLog(const char* str, int64 length){
//        int64 length = strlen(str);
        if(length != fileWrite(str, 1, length)){
            return false;
        }
        m_fileLength += length;
        return true;
    }
    inline int64 getLogLength(void) const {
        return m_fileLength;
    }
    inline const std::string& getFileName(void) const{
        return m_fileName;
    }
    // -------------------------
	bool openReadWrite(const char* mode){
		closeReadWrite();
		if(*mode == 'a'){
//		O_RDONLY:以只读方式打开文件
//        O_WRONLY:以只写方式打开文件
//        O_RDWR:以读写方式打开文
//          0666 = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
			m_fileHandle = open(m_fileName.c_str(), O_APPEND|O_CREAT, S_IRUSR|S_IWUSR|S_IROTH);   // O_APPEND
		}else{
			m_fileHandle = open(m_fileName.c_str(), O_RDWR);
		}
		if(-1 == m_fileHandle){
			m_fileHandle = 0;
			fprintf(stderr, "openReadWrite io failed file=%s mode=%s\n", m_fileName.c_str(), mode);
			return false;
		}
        m_fileLength = writeTell();
//		fprintf(stderr, "openReadWrite ok file=%s\n", m_fileName.c_str());
		return true;
	}
	void closeReadWrite(void){
	    m_fileLength = 0;
		if(0 != m_fileHandle){
			close(m_fileHandle);
			m_fileHandle = 0;
		}
	}
	inline void setFileName(const char* fileName){
		m_fileName = fileName;
	}
	inline int64 seekRead(void * ptr, int64 size, int64 n, int64 offset, int seek){
		fileSeek(offset, seek);
		return fileRead(ptr, size, n);
	}
	inline int64 seekWrite(const void * ptr, int64 size, int64 n, int64 offset, int seek){
		fileSeek(offset, seek);
		return fileWrite(ptr, size, n);
	}
	// block 文件读写操作
	inline int64 fileRead(void * ptr, int64 size, int64 n){
		return read(m_fileHandle, ptr, size*n);
	}
	inline int64 fileWrite(const void * ptr, int64 size, int64 n){
		return write(m_fileHandle, ptr, size*n);
	}
	inline int64 fileSeek(int64 offset, int seek){
		return lseek(m_fileHandle, offset, seek);
	}
	inline int64 readTell(void){
		return fileSeek(0, SEEK_END);
	}
	inline int64 writeTell(void){
		return fileSeek(0, SEEK_END);
	}
	inline int flush(void){
		return 0;
	}
	inline bool isOpen(void){
		return (0 != m_fileHandle);
	}
};

void* LogThreadHandleFunction(void* pData);

static int g_logLevel = 0;
static int g_currentTime = 0;
static int g_maxLogSize = 100*1024*1024;
static char g_timeString[128] = {0};
static char g_timeStringUS[128] = {0};
static char g_fmtBuffer[FMT_BUFFER_SIZE] = {0};

typedef std::unordered_map<std::string, LogFile*> LogFileMap;
static std::string m_logFileName;
static LogFileMap m_logFileMap;
typedef std::vector<std::pair<std::string, std::string>> LogDataQueue;
static LogDataQueue m_inQueue;
static LogDataQueue m_outQueue;
static pthread_mutex_t m_mutex;
static pthread_cond_t m_cond;
static pthread_t m_thread;


void removeLogFile(const std::string& fileName){
    LogFileMap::iterator itCur = m_logFileMap.find(fileName);
    if(itCur != m_logFileMap.end()){
        delete itCur->second;
        m_logFileMap.erase(itCur);
    }
}
LogFile* getOrReviveLogFile(const std::string& fileName){
    LogFileMap::iterator itCur = m_logFileMap.find(fileName);
    if(itCur == m_logFileMap.end()){
        LogFile* f = new LogFile(fileName, "");
        if( f->openLog() ){
            m_logFileMap.insert(std::make_pair(fileName, f));
            return f;
        }else{
            delete f;
            fprintf(stderr, "open log file failed = %s\n", fileName.c_str());
            return NULL;
        }
    }
    return itCur->second;
}

bool openCppLog(const std::string& fileName){
    if(!m_logFileName.empty()){
        return false;
    }
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
    pthread_create(&m_thread, NULL, LogThreadHandleFunction, NULL);
    pthread_detach(m_thread);
    m_logFileName = fileName;
    LogFile* f = getOrReviveLogFile(fileName);

    return (NULL != f);
}
const std::string& getCppLogFileName(void){
    return m_logFileName;
}
void setMaxLogSize(int logSize){
    g_maxLogSize = logSize;
}
int getMaxLogSize(void){
    return g_maxLogSize;
}
void setLogLevel(int level){
	g_logLevel = level;
}
int getLogLevel(void){
	return g_logLevel;
}
const char* getTimeString(void){
	time_t t = time(NULL);
	if(g_currentTime == t){
		return g_timeString;
	}
	g_currentTime = t;
	struct tm* ptm = localtime(&t);
	sprintf(g_timeString, "%4d-%02d-%02d %02d:%02d:%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return g_timeString;
}
const char* getTimeStringUS(void){
	timeval t;
	gettimeofday( &t, NULL );
	struct tm* ptm = localtime(&t.tv_sec);
	sprintf(g_timeStringUS, "%4d-%02d-%02d %02d:%02d:%02d %06d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)t.tv_usec);
	return g_timeStringUS;
}
const char* getTimeStringUSFile(void){
	timeval t;
	gettimeofday( &t, NULL );
	struct tm* ptm = localtime(&t.tv_sec);
	sprintf(g_timeStringUS, "%4d-%02d-%02d_%02d-%02d-%02d_%06d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)t.tv_usec);
	return g_timeStringUS;
}
char* getFmtBuffer(void){
    return g_fmtBuffer;
}
void writeCppLog(const char* str){
    writeLog(m_logFileName.c_str(), str);
}
void writeLog(const char* fileName, const char* str){
    pthread_mutex_lock(&m_mutex);
    m_inQueue.push_back(std::make_pair(std::string(fileName), std::string(str)));
    pthread_mutex_unlock(&m_mutex);
    pthread_cond_signal(&m_cond);
}
void writeLogInThread(const std::string& fileName, const std::string& str){
    LogFile* f = getOrReviveLogFile(fileName);
    if(NULL == f){
        return;
    }
    if( !f->writeLog(str.c_str(), (int64)str.length()) ){
        fprintf(stderr, "write log to file=%s failed log=%s", fileName.c_str(), str.c_str());
        removeLogFile(fileName);
        return;
    }
    if(f->getLogLength() >= g_maxLogSize){
        char tempBuffer[256];
        sprintf(tempBuffer, "%s.%s", f->getFileName().c_str(), getTimeStringUSFile());
        f->renameLog(tempBuffer);
        removeLogFile(fileName);
//        if( !f->openLog() ){
//            fprintf(stderr, "openLog file=%s failed", fileName.c_str());
//            removeLogFile(fileName);
//            return;
//        }
    }
}
void* LogThreadHandleFunction(void* arg){
    while(true){
        pthread_mutex_lock(&m_mutex);
        m_inQueue.swap(m_outQueue);
        while( m_outQueue.empty() ){
            pthread_cond_wait(&m_cond, &m_mutex);
            m_inQueue.swap(m_outQueue);
        }
        pthread_mutex_unlock(&m_mutex);
        for(auto &p : m_outQueue){
            writeLogInThread(p.first, p.second);
        }
        m_outQueue.clear();
    }
    return NULL;
}

NS_HIVE_END
