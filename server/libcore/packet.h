//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午2:57
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__packet__
#define __hive__packet__

#include "buffer.h"

NS_HIVE_BEGIN

// 16777216 - 8 - 1
#define PACKET_DATA_MAX 16777207
#define PACKET_MAX_LENGTH 16777215

typedef struct DestinationHandle {
	union{
		struct{
			unsigned int type   : 16;   // 目标服务类型
			unsigned int index  : 16;   // 目标服务下标
		};
		unsigned int handle     : 32;	// 句柄
	};
public:
	DestinationHandle(unsigned int t, unsigned int i) : type(t), index(i){}
	DestinationHandle(unsigned int h) : handle(h) {}
	DestinationHandle(void) : handle(0) {}
	~DestinationHandle(void){}

	inline unsigned int getType(void) const { return this->type; }
	inline unsigned int getIndex(void) const { return this->index; }
	inline unsigned int getHandle(void) const { return this->handle; }
	inline void setType(unsigned int t){ this->type = t; }
	inline void setIndex(unsigned int index){ this->index = index; }
	inline void setHandle(unsigned int handle){ this->handle = handle; }
	inline bool operator==(unsigned int h) const { return (this->handle == h); }
	inline bool operator==(const DestinationHandle& h) const{ return (this->handle == h.handle); }
	inline bool operator<(unsigned int h) const { return (this->handle < h); }
	inline bool operator<(const DestinationHandle& h) const { return (this->handle < h.handle); }
	inline DestinationHandle& operator=(unsigned int h){ this->handle = h; return *this; }
	inline DestinationHandle& operator=(const DestinationHandle& h){ this->handle = h.handle; return *this; }
}DestinationHandle;

// 服务间传递消息的头部数据结构
typedef struct PacketHead {
	int length;                     // 4 包的长度，传递的数据在8M以内
	unsigned int command;           // 4 消息的类型
	unsigned int callback;          // 4 返回的回调ID
	DestinationHandle destination;  // 4 目标服务
	DestinationHandle source;       // 4 源头服务
	unsigned int message;           // 4 当前携带的命令字段
	unsigned int uid;               // 4 玩家的uid
} PacketHead;

#define PACKET_HEAD_LENGTH_PROTECT 16
#define PACKET_HEAD_LENGTH sizeof(PacketHead)

#define WRITE_VALUE(p, v)\
	p->write(&v, sizeof(v))

#define READ_VALUE(p,v)\
	p->read(&v, sizeof(v))

class Packet : public RefObject
{
protected:
	Buffer* m_pBuffer;		// 数据指针
	int m_cursor;			// 数据读取的偏移
public:
    explicit Packet(int bufferSize);
    explicit Packet(Buffer* pBuffer);
    virtual ~Packet(void);

	void copyFrom(Packet* pPacket);
    void convertHead(void);
    void reverseHead(void);

	inline void setCursorToEnd(void){ m_cursor = getLength(); }
	inline void setCursor(int cur){ m_cursor = cur; }
	inline void moveCursor(int length){ m_cursor += length; }
	inline int getCursor(void) const { return m_cursor; }
	inline void resetCursor(void){ m_cursor = 0; }
	inline Buffer* getBuffer(void){ return m_pBuffer; }
	inline PacketHead* getHead(void){ return (PacketHead*)(getDataPtr()); }	// 该指针在write调用后可能会变化
	inline char* getOffsetPtr(int offset){ return m_pBuffer->data() + offset; }
	inline char* getCursorPtr(void){ return m_pBuffer->data() + m_cursor; }
	inline char* getDataPtr(void){ return m_pBuffer->data(); }
	inline int getLength(void) const { return (int)m_pBuffer->size(); }
	inline int getLengthInHead(void) { return getHead()->length; }
	inline bool isCursorEnd(void) const { return getCursor() >= getLength(); }
	inline bool isReceiveEnd(void) { return getCursor() >= getHead()->length; }
	inline char* getBody(void){ return getOffsetPtr(PACKET_HEAD_LENGTH); }
	inline int getBodyLength(void) const { return (getLength() - PACKET_HEAD_LENGTH); }

	inline void readBegin(void){
		setCursor((int)PACKET_HEAD_LENGTH);
	}
	inline void readEnd(void){
		resetCursor();
	}

	inline void writeBegin(unsigned int command){
		m_pBuffer->clear();
		m_pBuffer->resize(PACKET_HEAD_LENGTH, 0);
		setCursor((int)PACKET_HEAD_LENGTH);
        setCommand(command);
	}
	inline void writeEnd(void){
		recordLength();
		m_pBuffer->clearEncryptFlag();
	}
	template<typename NUMBER_T>
	inline int beginArray(void){
		int arrayOffset = getCursor();
		NUMBER_T zero = 0;
		write(&zero, sizeof(NUMBER_T));
		return arrayOffset;
	}
	template<typename NUMBER_T>
	inline int writeArray(const void* ptr, int length, int arrayOffset){
		int n = write(ptr, length);
		if( n > 0 ){
			(*((NUMBER_T*)(getOffsetPtr(arrayOffset))))++;
		}
		return n;
	}
	inline int write(const void* ptr, int length){
		int n = m_pBuffer->write(ptr, length, getCursor());
//		if( n > 0 ){
			moveCursor(n);
//		}
		return n;
	}
	inline int read(void* ptr, int length){
		int n = m_pBuffer->read(ptr, length, getCursor());
//		if( n > 0 ){
			moveCursor(n);
//		}
		return n;
	}
	inline int appendWrite(const void* ptr, int length){
		setCursor(getLength());
		return write(ptr, length);
	}
	inline int offsetRead(void* ptr, int length, int offset){
		if(offset < 0){
			setCursor(getLength() + offset);
		}else{
			setCursor(offset);
		}
		return read(ptr, length);
	}
	inline int popEnd(int popSize){
		int length = getLength();
		if(popSize <= 0 || popSize + (int)PACKET_HEAD_LENGTH > length){
			return 0;
		}
		int newLength = length - popSize;
		m_pBuffer->resize(newLength);
		recordLength();
		return popSize;
	}
	inline void recordLength(void){ getHead()->length = getLength(); }
	inline void setCommand(unsigned int cmd){ getHead()->command = cmd; }
	inline void setDestination(unsigned int handle){ getHead()->destination = handle; }
	inline void setDestination(unsigned int moduleType, unsigned int moduleIndex){
		DestinationHandle handle(moduleType, moduleIndex);
		setDestination(handle.handle);
	}
	inline void setSource(unsigned int source) { getHead()->source = source; }
	inline void setSource(unsigned int moduleType, unsigned int moduleIndex){
		DestinationHandle handle(moduleType, moduleIndex);
		setSource(handle.handle);
	}
	inline void setMessage(unsigned int message) { getHead()->message = message; }
	inline void setUID(unsigned int uid) { getHead()->uid = uid; }
	inline void setCallback(unsigned int callback) { getHead()->callback = callback; }
	inline unsigned int getCommand(void) { return getHead()->command; }
	inline unsigned int getDestination(void) { return getHead()->destination.handle; }
	inline unsigned int getSource(void) { return getHead()->source.handle; }
	inline unsigned int getMessage(void) { return getHead()->message; }
	inline unsigned int getUID(void) { return getHead()->uid; }
	inline unsigned int getCallback(void) { return getHead()->callback; }
};// end class Packet

NS_HIVE_END

#endif
