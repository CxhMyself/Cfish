#ifndef _EPOLL_H_
#define _EPOLL_H_

#include<sys/epoll.h>
#include<memory>
#include<fcntl.h>
#include<unistd.h>
#include"../base/ThreadPool.h"

#define MAX_EVENT_NUMBER 1024

/*
	一个事件表
	功能分析 分别有添加注册事件  删除注册事件  修改注册事件
	所以 我想先封装一个Event 这个呢 可以使用户使用Epoll时 只用添加 自己设置自己的属性
	之后封装一个Epoll
	最后封装一个Loop

*/

namespace Cfish{

class Event{
	public:

	//默认模式是添加
	Event() : event_(std::make_shared<struct epoll_event>() ), type_(EPOLL_CTL_ADD){  }

	//获取类型
	int getType() const;
	//设置类型
	void setType(int);
	//设置事件
	void setEvent(__uint32_t );
	//获取事件
	__uint32_t getEvent() const;
	//设置用户数据 之后可以将这个void* 进行个中转换
	void setUserDataWithFd( int fd );
	//获取用户数据
	int getUserDataWithFd() const;

	std::shared_ptr<struct epoll_event> getEventStruct() const;

	private:
	//被操作的对象
	int type_;
	std::shared_ptr<struct epoll_event> event_;
};

//这个类调用 Event 事件类来完成自动添加 因为我想一次读取一个报文 所以这里用默认模式 会再次触发
class Epoll{
	
	public:

	//size 只是长个样子 timeout 默认是阻塞的
	explicit Epoll(int size = 5,int timeout = -1);

	~Epoll(){ ::close(epollFd_); }
	
	//对一个事件进行操作
	void funEvent(const Event & );

	//返回轮询的结果依次返回 为0 的时候再次轮询 自动轮询
	Event getNext();

	//一个文件描述符 并采用默认的模式进行处理
	void addFd(int fd);
	void removeFd(int fd);
	void resetFd(int fd);
	//getEpollFd
	inline int getEpollFd() const{
		return epollFd_;
	}
	private:
	
	static const __uint32_t defaultEvents_ = EPOLLIN | EPOLLET | EPOLLONESHOT;
	
	int timeout_;
	//用来装就绪事件的 这个队列应该线程安全才对 不不不 对于一个epoll 来说只会存在于一个线程
	std::queue<Event> already_;
	//标识事件表
	int epollFd_;
};

}

#endif