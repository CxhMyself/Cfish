#include"Socket.h"
#include"SocketOps.h"
#include"InetAddress.h"
#include<fcntl.h>
#include<string>
#include<iostream>
using std::string;
namespace Cfish{
	
	Socket::Socket() : sockfd_(0){
		sockfd_ = SocketOps::socketOps(AF_INET);
	}
	void Socket::reset()
	{
		if( sockfd_ )
			closefd();
		sockfd_ = SocketOps::socketOps(AF_INET);
	}
	Socket::Socket(int fd) : sockfd_(0) {
		sockfd_ = fd;
	}
	InetAddress Socket::getSockaddr() const{
		//通过信息获取的方式获取地址
		struct sockaddr_in sockaddr;
		socklen_t len = static_cast<socklen_t>(sizeof sockaddr );
		
		auto x = SocketOps::getSocketInfo( sockfd_, &sockaddr , &len);
		if(x >= 0)
		{
			return InetAddress(&sockaddr);
		}
		return InetAddress();//默认为nullptr
	}
	void Socket::bindSocket(const InetAddress &inet)
	{
		SocketOps::bindOps( sockfd_ , inet.getSockaddr() , inet.getLen());
	}
	void Socket::listenSocket()
	{
		SocketOps::listenOps(sockfd_);
	}
	int  Socket::acceptSocket()
	{
		SocketOps::acceptOps(sockfd_);
	}
	int  Socket::connectSocket(const InetAddress & inet )
	{
		return SocketOps::connectOps(sockfd_ , inet.getSockaddr() , inet.getLen());
	}

	//关闭写端
	void Socket::shudownWrite()
	{
		SocketOps::shutdownWrite(sockfd_);
	}
	//close
	void Socket::closefd()
	{
		
		::shutdown(sockfd_ , SHUT_RDWR);
		::close(sockfd_);
		std::cout << " close -> " << sockfd_ << std::endl;
		sockfd_ = 0;
	}
	//三读 一个指针
	size_t Socket::readLineData(string& line)
	{
		line.clear();
		//直接从读取 读取一行内容
		char ch;
		label:
			auto ret = ::read( sockfd_ , &ch ,1 );
			if( ret == 1 ){
				//std::cout << "[ch] " << ch << std::endl;
				line += ch;
				if(ch == '\r' || ch == '\n' ){
					//读取到了一行
					goto end;
				}else
				{
					goto label;
				}
			}
			else if( ret == 0 )
					return line.size();
			else{
				if( errno == EINTR )
					goto label;
				line.clear();
				return -1;
			}
		end:
	
		ret = recv(sockfd_  ,&ch , 1 , MSG_PEEK);
        if( ret == 1 && ( ch == '\n' || ch == '\r') )
			ret = recv(sockfd_  ,&ch , 1 , 0);

		//去除\r\n
		while( line.back() == '\r' || line.back() == '\n' )
			line.pop_back();
		return line.size();
	}
	static size_t readn(char *buf ,size_t size , int fd)
	{
		auto ptr = buf;
		auto allSize = size;
		decltype(allSize) index = 0 ;

		while( allSize < 0 )
		{
			if( ( index = ::recv(fd , ptr , allSize,0) ) <0 )
			{
				//信号中断
				if( errno == EINTR )
					continue;
				else{
					//进行返回值的设置 容量和真实的内容长度不一样则出错
					return -1;
				}
			}
			else if( index == 0 ) //读完了
				break;
			allSize -= index;
			ptr += index;
		}
		return size;
	}
	size_t Socket::readSizeData(SizeBuffer &sb) // 这个东西呀用来读取
	{
		//清除原先的内容
		sb.clear();
		auto allSize = sb.getSize();
		char * space = new char[allSize+1];
		auto x = readn(space , allSize , sockfd_);
		if(x == -1){ delete [] space ; return -1; }
		sb.append(space , space+allSize);
		delete [] space;
		//设置实际长度为容量
		sb.setSize(sb.getRealSize());
		return x;
	}

	size_t Socket::readFileData(struct iovec* io) // 一个内存块 这里会动态申请一块空间 后面记得释放呀
	{
		//根据内部的len的大小 申请
		//这个是内部动态申请的空间 可以通过传入的参数更改内部数据
		io->iov_base = new char[io->iov_len];
		//将文件读取到这个缓冲区
		return readn(static_cast<char*>(io->iov_base) , io->iov_len , sockfd_);
	}
	ssize_t Socket::sendToNetWithMessage(const std::string& sizebuf, const struct iovec *iv) {
		if( iv == nullptr )
		{
			return sendStr(sizebuf.c_str() ,sizebuf.size() );
		}
		else{
			struct iovec iov[2];
			iov[0].iov_base = const_cast<void *>( ( const void*)(sizebuf.c_str() ) ) ;
			iov[0].iov_len  = sizebuf.size();
			iov[1].iov_base = iv->iov_base;
			iov[1].iov_len 	= iv->iov_len;
		//调用writev 将报文写入
			return ::writev(sockfd_ , iov , 2);
		}
	}
	//返回发送的字节数
	size_t Socket::sendStr(const char * buf , size_t siz){
		const char * ptr = buf;
		size_t writenSize = 0;
		ssize_t allSize = siz;
		while( allSize > 0 ) //还有数据未读
		{
			if( (writenSize = ::write(sockfd_ ,ptr , allSize)) <= 0 ){
				if( writenSize < 0 && errno == EINTR )
					writenSize = 0;
				else
					return -1;
			}
			allSize -= writenSize;
			ptr += writenSize;
		}
		return siz;
	}
	size_t Socket::readRubbishData(size_t size)
	{
		//设置一个缓冲区 每次读取一个缓冲区的
		char buffer[4096];
		while( size > 0 )  {
			auto ret = readn(buffer , size ,sockfd_ );
			//如果还没读完 就继续读
			size -= ret;
		}
	}
	//读取一定长度的
	size_t Socket::readSizeData(size_t size , std::string & str)
	{
		char buffer[4097] = "\0";
		auto allSize = size;
		int index = 0 ;
		//std::cout << " I ININININININIBN " << std::endl;
		while( allSize > 0 )
		{
			if( ( index = ::recv(sockfd_ , buffer , allSize, 0) ) <0 )
			{
				//std::cout  << buffer << std::endl;
				//信号中断
				if( index == -1 && errno == EINTR  )
					continue;
				else{
					//进行返回值的设置 容量和真实的内容长度不一样则出错
					return -1;
				}
			}
			else if( index == 0 ) //读完了
				break;
			buffer[index] = '\0';
			allSize -= index;
			//std::cout << allSize << " and " << index << " and " << buffer << std::endl;
			str.append(buffer);
		}
		return size;
	}
}
