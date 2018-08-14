#include"Epoll.h"
#include"Error.h"
#include<sys/epoll.h>
#include<iostream>
#include<errno.h>
using namespace Cfish;

//获取类型
int Event::getType() const {
	return type_;
}
//设置类型
void Event::setType(int type){
	type_ = type;
}
//设置事件
void Event::setEvent(__uint32_t events){
	event_->events = events;
}
//获取事件
__uint32_t Event::getEvent() const{
	return event_->events;
}
//设置用户数据 之后可以将这个void* 进行个中转换
void Event::setUserDataWithFd( int fd ){
	event_->data.fd = fd;
}
//获取用户数据
int Event::getUserDataWithFd() const{
	return event_->data.fd;
}
//获取事件结构体
std::shared_ptr<struct epoll_event> Event::getEventStruct() const {
	return event_;
}

Epoll::Epoll(int size ,int timeout )
: epollFd_( ::epoll_create(size) ),
timeout_(timeout)
{
	if( epollFd_ < 0 )
		ERR("Epoll::Epoll epoll_create");
}

//Emmmm 是否开启ET模式呢暂时不支持此功能 还是默认采用ET模式吧 之后改 可以通过重响应的机制再次设置事件
void Epoll::funEvent(const Event & e){
	auto x = ::epoll_ctl( epollFd_ , e.getType() , e.getUserDataWithFd() ,  e.getEventStruct().get() );
	if( x < 0 ) {
		std::cout << strerror(errno) << std::endl;
		ERR("::epoll_ctl");
	}
}

void Epoll::addFd(int fd)
{
	//还是得先封装一个Event
	Event e;
	e.setType(EPOLL_CTL_ADD);
	e.setEvent(Epoll::defaultEvents_);
	e.setUserDataWithFd(fd);
	
	funEvent(e);
}

void Epoll::resetFd(int fd)
{
	Event e;
	e.setType(EPOLL_CTL_MOD);
	e.setEvent(Epoll::defaultEvents_);
	e.setUserDataWithFd(fd);
	
	funEvent(e);
}
void Epoll::removeFd(int fd)
{
	Event e;
	e.setType(EPOLL_CTL_DEL);
	e.setUserDataWithFd(fd);

	funEvent(e);
}
Event Epoll::getNext(){
	if(already_.size() == 0)
	{
		//调用epoll_wait函数获取新一轮的并加入
		struct epoll_event events[MAX_EVENT_NUMBER];
		//这种模式下 timeout_应该一致保持阻塞
		int ret = ::epoll_wait(epollFd_ , events , MAX_EVENT_NUMBER, timeout_);
		for(int i = 0 ; i < ret ; i++){
			Event e;
			e.setUserDataWithFd(events[i].data.fd);
			e.setEvent(events[i].events);
			already_.push(e);
		}
	}
	Event e = already_.front();
	already_.pop();
	return e;
}


