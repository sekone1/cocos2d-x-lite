// SocketBase.h

#ifndef __SOCKETBASE__CLASS

#define __SOCKETBASE__CLASS

#include "cocos2d.h"
using namespace cocos2d;

namespace cocos2d {
	class SocketBase : public cocos2d::Ref
	{
#if defined(_WINDOWS)
		SOCKET _sock;
#else
		int _sock;
#endif
	protected:
		SocketBase();
		~SocketBase();
		bool connectTo(uint64_t host, u_short port);
		int readBuf(char* buf, int len);
		//读满len个字节
		bool readMsg(char* buf, int len);
		int sendBuf(char* buf, int len);
		//发满len个节
		bool sendMsg(char* buf, int len);
		void closeSocket();
	private:
	};
} 

#endif // __SOCKETBASE__CLASS
