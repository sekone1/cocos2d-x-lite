
#include "SocketHelper.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(_WINDOWS)
#else
#include <arpa/inet.h>
#endif

USING_NS_CC;

void SocketHelper::sendMsgToCocosThread(const std::function<void()>& cb)
{
	if (Director::getInstance())
		Director::getInstance()->getScheduler()->performFunctionInCocosThread(cb);
}


uint16_t SocketHelper::getUint16(int offset, char* netBuf)
{
	return (uint16_t(netBuf[offset]) << 8) + uint8_t(netBuf[offset + 1]);
}

uint32_t SocketHelper::getUint32(int offset, char* netBuf)
{
	return (uint32_t(netBuf[offset]) << 24) + (uint32_t(netBuf[offset + 1]) << 16) + (uint32_t(netBuf[offset + 2]) << 8) + (uint8_t(netBuf[offset + 3]));
}

void SocketHelper::uint32ToNetBuf(char* dst, uint32_t num){
	//num = htonl(num);
	//dst = (char*)(&num);
	for (int i = 0; i < 4; i++){
		dst[i] = 0xFF & (num >> (3 - i) * 8);
		log("uint32ToNetBuf: %d", (uint8_t)dst[i]);
	}
}

void SocketHelper::encryptData(char* data, int dataLen, char* key, int keyLen, uint8_t keyOffset, uint8_t bitMove)
{
	uint8_t bitOffset = bitMove % 8;
	for (int i = 0; i < dataLen; i++){
		uint8_t d = data[i];
		d = (d << bitOffset) | (d >> (8 - bitOffset));
		d = d ^ (uint8_t)key[(keyOffset + i) % keyLen];
		data[i] = d;
		//data[i] = (data[i] << bitOffset) | (data[i] >> (8 - bitOffset));
		//data[i] = data[i] ^ key[(keyOffset + i) % keyLen];
	}
	/*
	for i : = 0; i < len(data); i++ {
		data[i] = ((data[i] << bitOffset) | (data[i] >> (8 - bitOffset)))
			data[i] = data[i] ^ key[(int(keyOffset) + i) % keyLen]
	}
	*/
}
void SocketHelper::descryptData(char* data, int dataLen, char* key, int keyLen, uint8_t keyOffset, uint8_t bitMove, int* curDataIndex)
{
	uint8_t bitOffset = bitMove % 8;
	//uint8_t keyOffset = keyOffset % keyLen;
	for (int i = 0; i < dataLen; i++){
		uint8_t d = data[i];
		d = d ^ (uint8_t)key[(keyOffset + *curDataIndex) % keyLen];
		d = (d >> bitOffset) | (d << (8 - bitOffset));
		data[i] = d;
		//data[i] = data[i] ^ key[(keyOffset + i) % keyLen];
		//data[i] = (data[i] >> bitOffset) | (data[i] << (8 - bitOffset));
		*curDataIndex += 1;
	}
	/*
	for i : = 0; i < len(data); i++ {
		data[i] = data[i] ^ this.key[(int(this.keyOffset) + this.index) % this.keyLen]
			data[i] = ((data[i] >> this.bitOffset) | (data[i] << (8 - this.bitOffset)))
			this.index++
	}
	*/
}
