
#include "SocketBase.h"
#include <stdio.h>
#include <stdlib.h>


#if defined(_WINDOWS)
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#endif
USING_NS_CC;

SocketBase::SocketBase(){

}

SocketBase::~SocketBase(){

}

bool SocketBase::connectTo(uint64_t host, u_short port){
#if defined(_WINDOWS)
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建套接字
	_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
	_sock = socket(AF_INET, SOCK_STREAM, 0);
#endif


	//向服务器发起请求
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = host;//inet_addr(host);
	sockAddr.sin_port = htons(port);
#if defined(_WINDOWS)
	int result = connect(_sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
#else
	int result = connect(_sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
#endif
	return result == 0;
}

int SocketBase::readBuf(char* buf, int len){
#if defined(_WINDOWS)
	return recv(_sock, buf, len, NULL);
#else
	return read(_sock, buf, len);
#endif
}

bool SocketBase::readMsg(char* buf, int len){
	int readCount = readBuf(buf, len);
	if (readCount < 0) {
		return false;
	}
	else if (readCount == len){
		return true;
	}
	char* temp = buf + readCount;
	while(readCount<len)
	{
		int r = readBuf(temp, (len - readCount));
		if (r < 0) {
			return false;
		}
		else if(r == 0){
			continue;
		}
		else {
			readCount += r;
			temp = buf + readCount;
		}
	}
	return true;
}

int SocketBase::sendBuf(char* buf, int len){
#if defined(_WINDOWS)
	return send(_sock, buf, len, NULL);
#else
	return write(_sock, buf, len);
#endif
}

bool SocketBase::sendMsg(char* buf, int len){
	int sendLen = sendBuf(buf, len);
	if (sendLen < 0) {
		return false;
	}
	else if (sendLen == len){
		return true;
	}
	char* temp = buf + sendLen;
	while (sendLen < len){
		int r = sendBuf(buf, len);
		if (r < 0) {
			return false;
		}
		else if (r == 0){
			continue;
		}
		else {
			sendLen += r;
			temp = buf + sendLen;
		}
	}
	return true;
}

void SocketBase::closeSocket(){
#if defined(_WINDOWS)
	closesocket(_sock);
	WSACleanup();
#else
	close(_sock);
#endif
}
