#ifndef _SERVER_AND_CLIENT_SOCKET_
#define _SERVER_AND_CLIENT_SOCKET_

#include"Socket.h"
#include"InetAddress.h"

namespace Cfish{
	//身为一个服务端的Socket 我需要的功能是等待并返回一个Socket 
	class ServerSocket : public Socket{
		public:
		ServerSocket();
		ServerSocket(uint16_t );
		ServerSocket(const InetAddress & address);
		//需要实现的是 获取本端
		//和接收的组合实现并封装成一个Socket 因为socket的动作都挺一致的
		Socket acceptNewClient();
		inline const string getIp() const{
			InetAddress ia = Socket::getSockaddr();
			return ia.getIp();
		}
		inline const int getPort() const{
			InetAddress ia = Socket::getSockaddr();
			return ia.getPort();
		}
	};
	
	//这个先不用实现那么多 主要先把服务器端做出来
	class ClientSocket : public Socket{
		public:
		ClientSocket();
		ClientSocket(const InetAddress& addr);
		//获取对端的地址信息
		//接收并返回一个ServerSocket
		Socket connectNewServer(const InetAddress &);
	};
}

#endif