#ifndef _INET_ADDRESS_H_
#define _INET_ADDRESS_H_

//#include"../base/copyable.h"
#include<netinet/in.h> 				//这里是存放ipv4结构体的地方
#include<arpa/inet.h>				//提供字节转化
#include<sys/socket.h>


namespace Cfish{

//对不起 我垃圾 所以只考虑 ipv4 
//作用有二 存放 以及 查看所有的信息 因为只是地址 所以并不包含过多的信息
class InetAddress
{
	public:
	//因为有指针所以 直接使用另一个构造函数就好
	InetAddress(const InetAddress & i) : InetAddress( i.getSockaddr() ) {  }
	//这个的话得完全复制一份返回
	InetAddress operator=(InetAddress & i) {
		if ( addr_ ){
			delete [] addr_;
		}
		addr_ = new struct sockaddr_in[1];
		(*addr_) = (*i.getSockaddr());
		len = i.getLen();
	}
	//赋值符重载
	InetAddress() : addr_(nullptr) , len(0) {}
	~InetAddress() { if( addr_ ) delete [] addr_; }
	//两种方式构建
	explicit InetAddress(const struct sockaddr_in * addr);
	//协议组默认的 ip地址有两种默认选项 下来最重要的就是端口号了
	explicit InetAddress( uint16_t port , const char * ip = "0.0.0.0");
	//获取
	inline struct sockaddr_in * getSockaddr() const { return addr_; }
	//getPort
	inline int getPort(){
		return static_cast<uint16_t>( ntohs(addr_->sin_port) );
	}
	//这个因为调用比如 accept时需要长度可能会改变这个长度
	inline socklen_t getLen() const
	{
		return len;
	}
	inline void setLen(socklen_t l)
	{
		len = l;
	}
	//getIp
	inline const char * getIp(){
		return inet_ntoa(addr_->sin_addr);
	}

	private:
	struct sockaddr_in *addr_;
	socklen_t len;
};

}
#endif