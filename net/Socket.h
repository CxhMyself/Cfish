#ifndef _SOCKET_H_
#define _SOCKET_H_

#include"../base/copyable.h"
#include"../base/help.h"
#include"Buffer.h"
#include"SocketOps.h"
#include<unistd.h>
#include"InetAddress.h"
namespace Cfish{

class InetAddress;

//一个套接字都需要什么 其实根据套接字可以获取一切 存指针就不用复制了 之后分为了 ServerSocket和 ClientSocket
class Socket{
	public:
	
	void reset();
	
	virtual ~Socket() = default;
	//对于一个ServerSocket 需要自动的设置fd
	Socket();
	//ClientSocket需要调用connect 由内核分配
	Socket(int fd);

	//关闭写端
	void shudownWrite();
	inline void shutdownRead(){
		SocketOps::shutdownRead(sockfd_);
	}
	//close
	void closefd();

	
	//获取套接字地址
	InetAddress getSockaddr() const;
	//获取文件描述符
	inline int getFd() const {
		return sockfd_;
	}
	inline string getPeerIp()const {
		struct sockaddr_in sock;
		socklen_t len = sizeof(sock);
		
		auto ret = SocketOps::getPeerInfo(sockfd_ , &sock,&len );
		if( ret < 0 )
			return string("");
		else
		{
			InetAddress addr(&sock);
			return addr.getIp();
		}
	}

	//两读一写
	//注意所有的SizeBuffer都存在问题
	//读取 一行 读取一行数据到Buffer中 读取之后自动消除\r\n
	size_t readLineData(string& );
	size_t readSizeData(SizeBuffer &); // 这个东西呀用来读取
	size_t readSizeData(size_t size , std::string & str);
	size_t readFileData(struct iovec*); // 一个内存块 这里会动态申请一块空间 后面记得释放呀
	size_t readRubbishData(size_t size); //这个用于读取脏数据 用于加快程序的运行
	
	//将Buffer中的数据写入 Net 也就是套接字里面 所有的都能用这一个输入 这里干脆用一个工具就好了第二个参数默认为null 也就是无文件
	size_t sendStr(const char * , size_t siz); // 直接发送
	ssize_t sendToNetWithMessage(const std::string&, const struct iovec *iov);
	inline size_t sendStr(const std::string &str){
		return sendStr(str.c_str()  ,str.size());
	}

	//私有的是bind accept listen connect 这些函数可以由子类进行组合调用 如果存在返回值也是 Socket的返回值
	void bindSocket(const InetAddress &);
	void listenSocket();
	int acceptSocket();
	int connectSocket(const InetAddress & inet );
	
	private:

	//文件描述符
	int sockfd_;
};

}
#endif