#include"SocketOps.h"


#include<errno.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/types.h>
#include<netinet/in.h>
#include"Error.h"
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
namespace Cfish{
	namespace SocketOps{
		//关于Socket的一些操作
		/*
			包含 监听 绑定 接受连接 去连接 设置非阻塞 关闭套接字 获取本地的套接字详细信息 获取对端套接字详细信息 获取套接字错误
		*/
		
		//请求连接
		int connectOps( int fd , const struct sockaddr_in * addr ,socklen_t len){
			int f = ::connect(fd , (const struct sockaddr*)addr , len );
			if( f < 0){
				ERR("connectOps failed");
				return -1;
			}
			else
				return f;
		}
		
		//清除文件描述符的TIME_WAIT 状态
		void clearTimeWait(int fd){
			int r = 1;
			::setsockopt(fd , SOL_SOCKET , 0, &r , sizeof(r));
		}
		//定义获取套接字的操作  返回一个套接字 协议类型唯一
		int socketOps(sa_family_t family){
			int sock = ::socket(family , SOCK_STREAM | SOCK_CLOEXEC | SOCK_STREAM , 0);
			if(sock < 0)
			{
				ERR("::socket failed");
			}
			return sock;
		}
		//监听的数量设置为默认值 5
		void listenOps(int sock){
			auto x = ::listen(sock , 5);
			if( x < 0 )
				ERR("listen failed");
		}
		//绑定将套接字和端口号绑定起来
		void bindOps(int listenfd ,const struct sockaddr_in* addr , socklen_t len){
			auto x = ::bind(listenfd , (const struct sockaddr*) addr , len);
			if( x < 0 )
				ERR("bind failed");
		}
		//接受一个连接请求 并返回对应的套接字
		int acceptOps(int listenfd ){
			//临时构建一个地址用来存储 未来可以通过套接字获取对端的信息
			struct ::sockaddr_in addr;
			socklen_t len = sizeof(addr);//接收当然是阻塞的了
			int clientfd = ::accept4(listenfd , (struct sockaddr *)(&addr) , &len , SOCK_NONBLOCK | SOCK_CLOEXEC); //新创建的文件描述符自动拥有这些属性
			if( clientfd < 0 )
			{
				ERR(strerror(errno) ); // 输出错误原因 并停止程序
			}
			return clientfd;
		}
		//设置非阻塞 返回之前的状态
		int setNonBlock(int fd)
		{
			//读取文件的当前状态
			int flags = fcntl(fd , F_GETFL ,0);
			//将当前的状态加上非阻塞
			int ret = flags;
			flags |= O_NONBLOCK;
			fcntl(fd , F_SETFL , flags , 0);
			return ret;
		}
		//close
		void closefd(int fd){
			::close(fd);
		}
		//shutdownWrite
		void shutdownWrite(int fd)
		{
			::shutdown(fd , SHUT_WR);
		}
		//shutdownRead
		void shutdownRead(int fd)
		{
			::shutdown(fd , SHUT_RD);
		}
		//getSocketError
		//getSocketInfo // 这里只考虑 ipv4所以直接进行转化换为sockaddr_in *的类型 本端的sockaddr 错误返回-1
		int getSocketInfo(int fd , struct sockaddr_in* sin , socklen_t* len){
			auto x = ::getsockname(fd , (struct sockaddr*) sin , len );
			return x;
		}
		//getPeerInfo 对端的sockaddr accept的结果的对端
		int getPeerInfo(int fd , struct sockaddr_in*sin , socklen_t* len)
		{
			auto x = ::getpeername(fd , (struct sockaddr*) sin , len );
			return x;
		}
		int getSocketError(int fd)
		{
			int optval;
			socklen_t len = static_cast<socklen_t>(sizeof(optval));
			if( ::getsockopt( fd , SOL_SOCKET , SO_ERROR , &optval , &len ) < 0 )
			{
				//返回的是这个函数的调用错误
				return errno;
			}
			//返回的是由函数调用得到的错误
			return optval;
		}
		std::string getSocketIp(int sock)
		{
			struct sockaddr_in addr;
			socklen_t len = sizeof(addr);
			if ( getPeerInfo(sock , &addr , &len) < 0 )
				return "";
			else
			{
				//address to string
				char buffer[1024] = "\0";
				::inet_ntop( AF_INET, &(addr.sin_addr) , buffer , sizeof(buffer));
				return std::string(buffer);
			}
		}
	}
}