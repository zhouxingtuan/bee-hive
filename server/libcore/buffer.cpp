//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午3:09
// To change this template use File | Settings | File Templates.
//

#include "buffer.h"

NS_HIVE_BEGIN

Buffer::Buffer(int length) : RefObject(), CharVector(length, 0), m_isEncrypt(false) {
	clearEncryptFlag();
	this->clear();
}
Buffer::~Buffer(void){

}

void Buffer::copyFrom(Buffer* pBuffer){
//	m_encryptFlag = pBuffer->m_encryptFlag; // 无法复制加密标记
	if(pBuffer->m_isEncrypt){
		checkEncryptFlag();
	}
	this->resize(pBuffer->size(), 0);
	memcpy(this->data(), pBuffer->data(), pBuffer->size());
}

NS_HIVE_END
