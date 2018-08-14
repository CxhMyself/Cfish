#ifndef _FD_ROOT_H_
#define _FD_ROOT_H_

//这是一个纯虚函数 是所有统一事件源的根 Emmmm 还是可以保留下来作为一个TcpConnect的根 用这个可以判断此连接属于什么连接
class FdRoot{
	virtual getFd() = 0;
	virtual ~FdRoot();
};

#endif