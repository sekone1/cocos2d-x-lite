// Socket.h

#ifndef __SOCKET__CLASS

#define __SOCKET__CLASS

#include "cocos2d.h"
#include "SocketBase.h"
#include <queue>
using namespace cocos2d;
using namespace std;

namespace cocos2d {
	class Socket : public SocketBase
	{
	public:
		struct Data
		{
			Data() :bytes(nullptr), len(0){}
			ssize_t len;
			char* bytes;
		};
		struct MsgInfo
		{
			uint16_t protoID;
			uint32_t serialNum;
			time_t sendTime;
		};
		enum class ErrorCode
		{
			ConnectFailed,
			InvalidKey,
			LoseConnect,
			LoseConnectAndRetry,
			DecodeErr,
		};
		enum class ConnectStatus
		{
			GoConnecting,//正要连接
			Connecting,//正在连接
			Connected,//已经连接
			ConnectLost,//未连接或者连接已经断开
			SendErr,//发送失败
		};
        class Delegate : public cocos2d::Ref {
		public:
			/** Destructor of Delegate. */
			virtual ~Delegate() {}
			virtual void onMessage(Socket* socket, const Data& data) = 0;
			virtual void onTimeOut(Socket* socket, uint16_t protoID, uint32_t serialNum) = 0;
			virtual void onError(Socket* socket, ErrorCode errCode) = 0;
			virtual void onConnected(Socket* socket) = 0;
		};
	private:
		uint32_t serialNum;
		char* key;//密钥
		bool hadKey;
		uint64_t host;//ip
		u_short port;//端口
		Delegate *_delegate;
		queue<Data> msgWaitToSend;
		ConnectStatus _connectStatus;
		mutex netStatMutex;
		char* buffer;
		//mutex netConnectMutex;
		//time_t lastReciveTime;
		time_t lastSendTime;
		bool recvThreadRuning;
		bool sendThreadRuning;
		bool waitThradRuning;
		vector<MsgInfo> msgWaitRecive;
		mutex msgWaitReciveMutex;
		/*
		inline ConnectStatus getConnectStatus(){
			netStatMutex.lock();
			ConnectStatus r = _connectStatus;
			netStatMutex.unlock();
			return r;
		}
		*/
		inline void setConnectStatus(ConnectStatus connectStatus){
			netStatMutex.lock();
			_connectStatus = connectStatus;
			netStatMutex.unlock();
		}
	public:

		Socket();
		~Socket();
		void connect();
		void onSubThreadReciveLoop(bool isRetry);
		void onSubThreadSendLoop();
		void onSubThreadCheckTimeOutLoop();
		void setDelegate(Delegate* delegate);
		bool send(vector<Data> datas, int totalLen);
		void setAddr(const char* host, u_short port);
		void close();
		void destroy();

	private:
		void onError(ErrorCode errCode);
		void clearSendMsg();
		bool readFull(char* buf, int len);
		void waitThreadAbort();
		///void recvLoop(bool isRetry);
	};
} 

#endif // __SOCKET__CLASS
