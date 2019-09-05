//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/1/15
// Time: 下午10:46
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__md5__
#define __hive__md5__

#include <string>
#include <cstring>
#include "core.h"

NS_HIVE_BEGIN

/* Parameters of MD5. */
#define s11 7
#define s12 12
#define s13 17
#define s14 22
#define s21 5
#define s22 9
#define s23 14
#define s24 20
#define s31 4
#define s32 11
#define s33 16
#define s34 23
#define s41 6
#define s42 10
#define s43 15
#define s44 21

/**
 * @Basic MD5 functions.
 *
 * @param there bit32.
 *
 * @return one bit32.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/**
 * @Rotate Left.
 *
 * @param {num} the raw number.
 *
 * @param {n} rotate left n.
 *
 * @return the number after rotated left.
 */
#define ROTATELEFT(num, n) (((num) << (n)) | ((num) >> (32-(n))))

/**
 * @Transformations for rounds 1, 2, 3, and 4.
 */
#define FF(a, b, c, d, x, s, ac) { \
  (a) += F ((b), (c), (d)) + (x) + ac; \
  (a) = ROTATELEFT ((a), (s)); \
  (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
  (a) += G ((b), (c), (d)) + (x) + ac; \
  (a) = ROTATELEFT ((a), (s)); \
  (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
  (a) += H ((b), (c), (d)) + (x) + ac; \
  (a) = ROTATELEFT ((a), (s)); \
  (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
  (a) += I ((b), (c), (d)) + (x) + ac; \
  (a) = ROTATELEFT ((a), (s)); \
  (a) += (b); \
}

/* Define of btye.*/
typedef unsigned char byte;
/* Define of byte. */
typedef unsigned int bit32;

class MD5 : public Object0824
{
public:
	/* Construct a MD5 object with a string. */
	MD5(void);

	static MD5* createObject(void){
		return new MD5();
	}
	static void releaseObject(MD5* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	/* update with the string */
	void update(const char* ptr, unsigned int length);

	/* Generate md5 digest. */
	const byte* getDigest(void);

	/* get c type string */
	const char* getString(void);

	/* Convert digest to string value */
	const std::string& toString(void);

private:
	/* Initialization the md5 object, processing another message block,
	* and updating the context.*/
	void init(const byte* input, size_t len);

	/* MD5 basic transformation. Transforms state based on block. */
	void transform(const byte block[64]);

	/* Encodes input (usigned long) into output (byte). */
	void encode(const bit32* input, byte* output, size_t length);

	/* Decodes input (byte) into output (usigned long). */
	void decode(const byte* input, bit32* output, size_t length);

private:
	/* Flag for mark whether calculate finished. */
	bool finished;

	/* state (ABCD). */
	bit32 state[4];

	/* number of bits, low-order word first. */
	bit32 count[2];

	/* input buffer. */
	byte buffer[64];

	/* message digest. */
	byte digest[16];

	/* result */
	std::string result;

	/* padding for calculate. */
	static const byte PADDING[64];

	/* Hex numbers. */
	static const char HEX_NUMBERS[16];
};

typedef ObjectPool<MD5> MD5Pool;

#define DEFINE_CREATE_REMOVE_MD5()\
public:\
	MD5Pool m_MD5Pool;\
	MD5* createMD5(void){ return m_MD5Pool.create(); }\
	bool removeMD5(MD5* pMD5){ return this->removeMD5(pMD5->getHandle()); }\
	bool removeMD5(uint32 handle){\
		MD5* pMD5;\
		pMD5 = m_MD5Pool.get(handle);\
		if(NULL != pMD5){\
			m_MD5Pool.idle(handle);\
			return true;\
		}\
		return false;\
	}\


NS_HIVE_END

#endif
