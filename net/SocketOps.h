#ifndef _SOCKET_OPS_H_
#define _SOCKET_OPS_H_

#include<errno.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/types.h>
#include<netinet/in.h>
#include"Error.h"
#include<string>

namespace Cfish{

	namespace SocketOps{
		//关于Socket的一些操作
		/*
			包含 监听 绑定 接受连接 去连接 设置非阻塞 关闭套接字 获取本地的套接字详细信息 获取对端套接字详细信息 获取套接字错误
		*/
		
		//请求连接
		int connectOps( int fd , const struct sockaddr_in * addr ,socklen_t len);
		//清除文件描述符的TIME_WAIT 状态
		void clearTimeWait(int fd);
		//定义获取套接字的操作  返回一个套接字 协议类型唯一
		int socketOps(sa_family_t family);
		//监听的数量设置为默认值 5
		void listenOps(int sock);
		//绑定将套接字和端口号绑定起来
		void bindOps(int listenfd ,const struct sockaddr_in* addr , socklen_t len);
		//接受一个连接请求 并返回对应的套接字
		int acceptOps(int listenfd );
		//设置非阻塞 返回之前的状态
		int setNonBlock(int fd);
		//close
		void closefd(int fd);
		//shutdownWrite
		void shutdownWrite(int fd);
		//shutdownRead
		void shutdownRead(int fd);
		//getSocketError
		//getSocketInfo // 这里只考虑 ipv4所以直接进行转化换为sockaddr_in *的类型 本端的sockaddr 错误返回-1
		int getSocketInfo(int fd , struct sockaddr_in* sin , socklen_t* len);
		//getPeerInfo 对端的sockaddr accept的结果的对端
		int getPeerInfo(int fd , struct sockaddr_in*sin , socklen_t* len);
		//get ip address
		std::string getSocketIp(int fd); 
		int getSocketError(int fd);
	}
}

#endif