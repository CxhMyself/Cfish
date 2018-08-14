#include"ServerAndClientSocket.h"
#include"InetAddress.h"
#include"SocketOps.h"
#include<sys/types.h>
#include<iostream>
namespace Cfish{
	//ServerSocket
	ServerSocket::ServerSocket(): Socket(){
		//默认情况直接调用 listen 内核会自动分配
		Socket::listenSocket();
	}
	ServerSocket::ServerSocket(const InetAddress & address)
	{
		//bind 和 listen
		SocketOps::clearTimeWait(Socket::getFd());
		Socket::bindSocket(address);
		Socket::listenSocket();
	}

	ServerSocket::ServerSocket(      uint16_t port          )
	{
		Socket::bindSocket( InetAddress(port) );
		Socket::listenSocket();
	}
	Socket ServerSocket::acceptNewClient()
	{
		int fd = Socket::acceptSocket();
		std::cout << "get a new socket " << fd << std::endl;
		return Socket(fd);
	}
	//ClientSocket 你总得指定对端的地址和端口号吧
	ClientSocket::ClientSocket() : Socket() {}
	ClientSocket::ClientSocket(const InetAddress& addr)
	{
		SocketOps::clearTimeWait(Socket::getFd());
		//只完成绑定动作 指定本端的端口号主要是 用于与外界相交流
		Socket::bindSocket(addr);
	}

	Socket ClientSocket::connectNewServer( const InetAddress& addr )
	{
		//连接绑定的 服务器
		return Socket( Socket::connectSocket(addr) ) ;
	}
}
