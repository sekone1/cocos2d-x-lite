// SocketHelper.h

#ifndef __SOCKETHELPER__CLASS

#define __SOCKETHELPER__CLASS

#include "cocos2d.h"
using namespace cocos2d;

namespace cocos2d {
	class SocketHelper
	{
	public:
		static void sendMsgToCocosThread(const std::function<void()>& cb);
		static uint16_t getUint16(int offset, char* netBuf);
		static uint32_t getUint32(int offset, char* netBuf);
		static void uint32ToNetBuf(char* dst, uint32_t num);
		static void encryptData(char* data, int dataLen,char* key, int keyLen, uint8_t keyOffset, uint8_t bitMove);
		static void descryptData(char* data, int dataLen, char* key, int keyLen, uint8_t keyOffset, uint8_t bitMove, int* curDataIndex);
	};
} 

#endif // __SOCKETHELPER__CLASS