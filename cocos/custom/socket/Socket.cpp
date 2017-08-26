// Socket.cpp
#include "Socket.h"
//#include "ScriptingCore.h"
//#include "jsapi.h"
#include "SocketHelper.h"
#include <time.h> 
#if defined(_WINDOWS)
#else
#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#endif

#define KEY_LEN 32
#define BUFFER_LEN 1024*1024
//心跳包发送时间 10秒
#define HEARTBEAT_TIME 10 
//收到消息回复的超时时间 5秒
#define MSG_OUT_RECIVE_TIME 5
#define WAIT_ALL_THREAD_ABORT while (true){if (recvThreadRuning == false && sendThreadRuning == false && waitThradRuning == false){break;}}
USING_NS_CC;

static string F_KEY = "!@#$!sqwS80UIOs%^kldae0$%sreRER:wg";

Socket::Socket(){
	recvThreadRuning = false;
	sendThreadRuning = false;
	waitThradRuning = false;
	_delegate = nullptr;
	buffer = new char[BUFFER_LEN];
	key =  new char[KEY_LEN];
	_connectStatus = ConnectStatus::ConnectLost;
	serialNum = 0;
	//设置随机种子
	srand((int)time(0));
	host = 0;
	port = 0;
	hadKey = false;
	log("=====in Socket::Socket()=====");
}

Socket::~Socket(){
	if (key != nullptr)
		delete[] key;
	if (buffer != nullptr)
		delete[] buffer;
    if(_delegate != nullptr){
        _delegate->release();
        log("====_delegate==deleted==");
    }
	clearSendMsg();
}

void Socket::setDelegate(Delegate* delegate){
	//_delegate = const_cast<Delegate*>(&delegate);
    if(_delegate)
        _delegate->release();
    _delegate = delegate;
    _delegate->retain();
}

void Socket::setAddr(const char* h, u_short p){
	host = inet_addr(h);
	port = p;
	clearSendMsg();
}

void Socket::clearSendMsg(){
	//清空队列
	while (msgWaitToSend.empty() == false)
	{
		Data d = msgWaitToSend.front();
		msgWaitToSend.pop();
		delete[] d.bytes;
	}
}


void Socket::waitThreadAbort(){
	while (true){if (recvThreadRuning == false && sendThreadRuning == false && waitThradRuning == false){break;}}
}

void Socket::close(){
	setConnectStatus(ConnectStatus::ConnectLost);
	SocketBase::closeSocket();
	clearSendMsg();
	msgWaitReciveMutex.lock();
	msgWaitRecive.clear();
	msgWaitReciveMutex.unlock();
}
void Socket::destroy(){
	netStatMutex.lock();
	if (_connectStatus != ConnectStatus::ConnectLost){
		_connectStatus = ConnectStatus::ConnectLost;
		SocketBase::closeSocket();
	}
	netStatMutex.unlock();
	WAIT_ALL_THREAD_ABORT
	delete this;
}
void Socket::onError(ErrorCode errCode){
	auto instance = this;
	SocketHelper::sendMsgToCocosThread([instance, errCode](){
		// In UI thread
		instance->_delegate->onError(instance,errCode);
	});
}
bool Socket::readFull(char* buf, int len){
	int readCount = readBuf(buf, len);
	if (readCount < 0) {
		return false;
	}
	else if (readCount == len){
		return true;
	}
	char* temp = buf + readCount;
	while (readCount<len)
	{
		if (_connectStatus != ConnectStatus::Connected)
		{
			return false;
		}
		int r = readBuf(temp, (len - readCount));
		if (r < 0) {
			return false;
		}
		else if (r == 0){
			continue;
		}
		else {
			readCount += r;
			temp = buf + readCount;
		}
	}
	return true;
}



void Socket::connect(){
	netStatMutex.lock();
	if (_connectStatus != ConnectStatus::ConnectLost){
		netStatMutex.unlock();
		return;
	}
	_connectStatus = ConnectStatus::GoConnecting;
	netStatMutex.unlock();

	std::thread tRecive = std::thread(&Socket::onSubThreadReciveLoop, this, false);
	tRecive.detach();
}

void Socket::onSubThreadReciveLoop(bool isRetry){
	if (isRetry == false){
		WAIT_ALL_THREAD_ABORT
	}

	netStatMutex.lock();
	if (_connectStatus == ConnectStatus::ConnectLost){
		netStatMutex.unlock();
		recvThreadRuning = false;
		return;
	}
	_connectStatus = ConnectStatus::Connecting;
	netStatMutex.unlock();

	hadKey = false;
	auto instance = this;


	bool ok = false;
	int tryCount = 5;//如果连接失败重试5次
	for (size_t i = 0; i <= tryCount; i++)
	{
		if (_connectStatus == ConnectStatus::ConnectLost)
		{
			recvThreadRuning = false;
			return;
		}
		ok = instance->connectTo(host, port);
		if (ok){
			break;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(((i + 1)*(i + 1) * 100)));
		}
	}

	netStatMutex.lock();
	if (_connectStatus == ConnectStatus::ConnectLost){
		//已经被强制关闭
		if (ok){
			SocketBase::closeSocket();
		}
		netStatMutex.unlock();
		recvThreadRuning = false;
		return;
	}
	else
	{
		if (!ok)
		{
			_connectStatus = ConnectStatus::ConnectLost;
			instance->onError(ErrorCode::ConnectFailed);
			netStatMutex.unlock();
			recvThreadRuning = false;
			return;
		}
	}
	_connectStatus = ConnectStatus::Connected;
	netStatMutex.unlock();


	recvThreadRuning = true;

	ConnectStatus tempNetStatus = ConnectStatus::Connected;
	ok = instance->readFull(instance->key, KEY_LEN);
	if (!ok){
		//read key error
		//instance->onError(ErrorCode::InvalidKey);
		//setConnectStatus(ConnectStatus::ConnectLost);
		//return;
		//丢失连接了 重连
		tempNetStatus = ConnectStatus::Connecting;
	}
	else
	{
		int curDataIndex = 0;
		SocketHelper::descryptData(instance->key, KEY_LEN, (char*)F_KEY.c_str(), F_KEY.length(), 0, 0, &curDataIndex);
		hadKey = true;
		/*
		log("===key===");
		for (size_t i = 0; i < KEY_LEN; i++)
		{
		log("%d=%d", i, (uint8_t)instance->key[i]);
		}
		*/


		//启动发送线程
		std::thread tSend(&Socket::onSubThreadSendLoop, this);
		tSend.detach();
		//检查线程
		std::thread t(&Socket::onSubThreadCheckTimeOutLoop, this);
		t.detach();

		SocketHelper::sendMsgToCocosThread([instance](){
			instance->_delegate->onConnected(instance);
		});

		while (true)
		{
			ok = instance->readFull(buffer, 1);
			if (!ok) {
				tempNetStatus = ConnectStatus::Connecting;
				break;
			}
			//instance->lastReciveTime = time(0);
			if (buffer[0] == 0) {
				//心跳包
				continue;
			}
			int readCount = 1;
			int packageCount = buffer[0];//子包个数
			ok = instance->readFull(buffer, 2 + 4);//2:keyInfo 4:序号
			if (!ok) {
				tempNetStatus = ConnectStatus::Connecting;
				break;
			}
			uint8_t keyOffset = buffer[0];
			uint8_t bitMove = buffer[1];
			int curDataIndex = 0;

			char* temp = buffer + 2;// 序号解密
			SocketHelper::descryptData(temp, 4, key, KEY_LEN, keyOffset, bitMove, &curDataIndex);
			log("------------keyOffset:%d, bitMove:%d", keyOffset, bitMove);

			readCount = 6;
			for (int i = 0; i < packageCount; i++)
			{
				int readLen = 2 + 4;//2:协议ID 4:包体长
				if (readCount + readLen > BUFFER_LEN)
				{
					instance->onError(ErrorCode::DecodeErr);
					tempNetStatus = ConnectStatus::ConnectLost;
					break;
				}
				temp = buffer + readCount;
				//读取包头
				ok = instance->readFull(temp, readLen);
				if (!ok) {
					tempNetStatus = ConnectStatus::Connecting;
					break;
				}
				log("0------------protoID:%d, %d", (uint8_t)temp[0], (uint8_t)temp[1]);
				log("0-------------bodylen:%d,%d,%d,%d", (uint8_t)temp[2], (uint8_t)temp[3], (uint8_t)temp[4], (uint8_t)temp[5]);
				SocketHelper::descryptData(temp, readLen, key, KEY_LEN, keyOffset, bitMove, &curDataIndex);
				readCount += readLen;
				readLen = SocketHelper::getUint32(2, temp); //temp[2-5] 包体长
				log("------------protoID:%d, %d", (uint8_t)temp[0], (uint8_t)temp[1]);
				log("-------------bodylen:%d,%d,%d,%d,%d", readLen, (uint8_t)temp[2], (uint8_t)temp[3], (uint8_t)temp[4], (uint8_t)temp[5]);
				if (readLen > 0){
					if (readCount + readLen > BUFFER_LEN)
					{
						instance->onError(ErrorCode::DecodeErr);
						tempNetStatus = ConnectStatus::ConnectLost;
						break;
					}

					temp = buffer + readCount;
					//读取包体
					ok = instance->readFull(temp, readLen);
					if (!ok) {
						tempNetStatus = ConnectStatus::Connecting;
						break;
					}
					SocketHelper::descryptData(temp, readLen, key, KEY_LEN, keyOffset, bitMove, &curDataIndex);
					readCount += readLen;
				}
			}

			if (tempNetStatus != ConnectStatus::Connected){
				break;
			}

			temp = buffer + 2;
			int dataLen = readCount - 2;//包含 序号|协议ID+包长+包体|协议ID+包长+包体|...

			/*
			log("===recive msg ====");
			for (size_t i = 0; i < dataLen; i++)
			{
			log("%d=%d", i, (uint8_t)temp[i]);
			}
			*/
			log("serialNum: %d=%d=%d=%d", (uint8_t)temp[0], (uint8_t)temp[1], (uint8_t)temp[2], (uint8_t)temp[3]);
			bool needHandler = false;
			uint32_t serialNum = SocketHelper::getUint32(0, temp);
			if (serialNum > 0){
				msgWaitReciveMutex.lock();
				for (vector<MsgInfo>::iterator it = msgWaitRecive.begin(); it != msgWaitRecive.end(); it++){
					if ((*it).serialNum == serialNum)
					{
						msgWaitRecive.erase(it);
						needHandler = true;//有在等待队列里才处理
						break;
					}
				}
				msgWaitReciveMutex.unlock();
			}
			else{
				//0为NTF 推送消息 必须处理
				needHandler = true;
			}
			if (needHandler)
			{
				Data d;
				d.len = dataLen;
				d.bytes = new char[dataLen];
				memcpy(d.bytes, temp, dataLen);
				SocketHelper::sendMsgToCocosThread([instance, d](){
					// In UI thread
					log("Notify data to Cocos thread.\n");
					instance->_delegate->onMessage(instance, d);
					delete[] d.bytes;
				});
			}
		}
	}

	hadKey = false;

	//网络断开后 重新连接
	bool reConnect = false;
	netStatMutex.lock();
	if (_connectStatus == ConnectStatus::Connected || _connectStatus == ConnectStatus::SendErr){
		if (tempNetStatus == ConnectStatus::Connecting){
			//重连
			instance->onError(ErrorCode::LoseConnectAndRetry);
			_connectStatus = ConnectStatus::Connecting;
			reConnect = true;
		}
		else {
			_connectStatus = ConnectStatus::ConnectLost;
		}
	}
	netStatMutex.unlock();

	if (reConnect)
		onSubThreadReciveLoop(true);
	else
	{
		recvThreadRuning = false;
	}
}

void Socket::onSubThreadSendLoop(){
	sendThreadRuning = true;
	lastSendTime = time(0);
	//lastReciveTime = time(0);
	bool ok = true;
	int c = 0;
	char heartbeat[1];
	heartbeat[0] = 0;
	while (true)
	{
		netStatMutex.lock();
		if (_connectStatus == ConnectStatus::ConnectLost || _connectStatus == ConnectStatus::GoConnecting){
			netStatMutex.unlock();
			break;
		}
		else if (_connectStatus != ConnectStatus::Connected || hadKey == false)
		{
			netStatMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}
		netStatMutex.unlock();
		if (msgWaitToSend.empty() == false){
			Data d = msgWaitToSend.front();

			//加密
			//密钥信息
			uint8_t keyOffset = rand() % KEY_LEN;
			uint8_t bitMove = rand() % 8;
			d.bytes[1] = keyOffset;
			d.bytes[2] = bitMove;
			char* temp = d.bytes + 3;
			//前面3个字节不加密
			SocketHelper::encryptData(temp, d.len - 3, key, KEY_LEN, keyOffset, bitMove);

			ok = sendMsg(d.bytes, d.len);
			if (ok == false){
				netStatMutex.lock();
				if (_connectStatus == ConnectStatus::Connected){
					_connectStatus = ConnectStatus::SendErr;//需要重新连接
				}
				netStatMutex.unlock();
				//setConnectStatus(ConnectStatus::SendErr);
			}else{
				//发送成功才移除
				msgWaitToSend.pop();
				delete[] d.bytes;
			}
			lastSendTime = time(0);
		} else{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		c++;
		if (c >= 1000){
			c = 0;
			//大概每隔1秒钟 检查心跳包时间
			if (time(0) - lastSendTime >= HEARTBEAT_TIME){
				//发送心跳包
				ok = sendMsg(heartbeat, 1);
				if (ok == false){
					netStatMutex.lock();
					if (_connectStatus == ConnectStatus::Connected){
						_connectStatus = ConnectStatus::SendErr;//需要重新连接
					}
					netStatMutex.unlock();
				}
				lastSendTime = time(0);
			}
			/*
			if (time(0) - lastReciveTime > HEARTBEAT_TIME*20)
			{
				//超时了 客户端与服务端失去连接
				//close();
				break;
			}
			*/
		}
	}
	sendThreadRuning = false;
	/*
	//清空队列
	while (msgWaitToSend.empty() == false)
	{
		Data d = msgWaitToSend.front();
		msgWaitToSend.pop();
		delete[] d.bytes;
	}
	*/
}

void Socket::onSubThreadCheckTimeOutLoop(){
	waitThradRuning = true;
	//检查消息是否超时未收到
	auto instance = this;
	while (true)
	{
		//netStatMutex.lock();
		if (_connectStatus == ConnectStatus::ConnectLost || _connectStatus == ConnectStatus::GoConnecting){
			//netStatMutex.unlock();
			break;
		}
		//netStatMutex.unlock();
		bool sleep = true;
		if (msgWaitRecive.size() > 0)
		{
			msgWaitReciveMutex.lock();
			time_t timeNow = time(0);
			for (vector<MsgInfo>::iterator it = msgWaitRecive.begin(); it != msgWaitRecive.end(); it++){
				if (timeNow - (*it).sendTime > MSG_OUT_RECIVE_TIME)
				{
					MsgInfo info = (*it);
					SocketHelper::sendMsgToCocosThread([instance, info](){
						instance->_delegate->onTimeOut(instance, info.protoID, info.serialNum);
					});
					msgWaitRecive.erase(it);
					sleep = false;
					break;
				}
			}
			msgWaitReciveMutex.unlock();
		}
		if (sleep)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	waitThradRuning = false;
}

bool Socket::send(vector<Data> datas, int totalLen){
	//log("====Socket::send recive:%d,%d", binaryMsg[0], len);
	if (_connectStatus != ConnectStatus::Connected) {
		//未连接
		return false;
	}
	
	Data d;
	int headLen = 7;//包个数+密钥信息+序号 （1+2+4）
	d.len = totalLen + headLen;//加上 包个数+密钥信息+序号 （1+2+4）
	d.bytes = new char[d.len];
	int hadCopyLen = 0;
	for (int i = 0; i < datas.size(); i++)
	{
		char* temp = d.bytes + hadCopyLen + headLen;
		memcpy(temp, datas[i].bytes, datas[i].len);
		hadCopyLen += datas[i].len;
	}
	/*
	for (int i = 0; i < totalLen; i++){
		log("%d",(UINT8)d.bytes[i]);
	}
	*/
	//加上 包个数+密钥信息+序号 （1+2+4）
	//包个数
	d.bytes[0] = datas.size();
	/*
	发送线程里加密
	//密钥信息
	uint8_t keyOffset = rand() % KEY_LEN;
	uint8_t bitMove = rand() % 8;
	d.bytes[1] = keyOffset;
	d.bytes[2] = bitMove;
	*/
	char* temp = d.bytes + 3;
	//序号

	uint32_t sn = 0;

	msgWaitReciveMutex.lock();
	serialNum++;
	sn = serialNum;
	MsgInfo m;
	m.serialNum = sn;
	m.sendTime = time(0);
	log("d.bytes[7]:%d,%d", (uint8_t)d.bytes[7], (uint8_t)d.bytes[8]);
	m.protoID = SocketHelper::getUint16(headLen, d.bytes);
	msgWaitRecive.push_back(m);
	msgWaitReciveMutex.unlock();

	SocketHelper::uint32ToNetBuf(temp, sn);
	//
	/*
	log("===1====");
	for (size_t i = 0; i < totalLen; i++)
	{
		log("%d=%d", i, (UINT8)d.bytes[i]);
	}
	*/
	//加密
	//SocketHelper::encryptData(temp, totalLen + 4, key, KEY_LEN, keyOffset, bitMove);
	/*
	log("===2====");
	for (size_t i = 0; i < totalLen; i++)
	{
		log("%d=%d", i, (UINT8)d.bytes[i]);
	}
	char* tt = new char[totalLen + 4];
	memcpy(tt, temp, totalLen + 4);
	SocketHelper::descryptData(tt, totalLen + 4, key, KEY_LEN, keyOffset, bitMove);
	log("===3====");
	for (size_t i = 0; i < totalLen + 4; i++)
	{
		log("%d=%d", i, (UINT8)tt[i]);
	}
	*/
	/*
	Data d;
	d.len = len;
	d.bytes = (char*)const_cast<unsigned char*>(binaryMsg);
	d.bytes = new char[len];
	memcpy((void*)d.bytes, (void*)binaryMsg, len);
	*/
	msgWaitToSend.push(d);
	/*
	u_long a = 2;
	u_long b = htonl(a);
	u_long c = ntohl(b);
	*/
	/*
	auto instance = this;
	bool ok = instance->connectTo("127.0.0.1", 8084);
	if (ok){
		char buffer[35];
		ok = instance->readFull(buffer, 35);
		bool a = ok;
	}
	*/
	//connect();
	//_delegate->onMessage(this, d);
	return true;
}
