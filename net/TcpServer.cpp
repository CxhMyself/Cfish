#include"TcpServer.h"
#include<iostream>
#include<sys/socket.h>
using namespace Cfish;

static void subThreadFunc(TcpServer * tcpServer)
{
	//这里进行本线程tid 的
	auto tid = std::this_thread::get_id();
	//自己负责添加tid
	tcpServer->getTable()->addTid(tid);
	//获取自动生成的Epoll
	auto epoll_run = tcpServer->getTable()->getEpollPerThreadByTid(tid);
	while(true){
		//获取了一个事件
		Event event = epoll_run->getNext();
		//Event event = (*epoll_run)[tid].getNext();
		//开始事件的读取 Emmm 这里我只设置了可读事件 不过也要处理可写事件 未来再说吧
		//下面两个事件 标识对端连接关闭 此时需要执行的操作是删除本端的对端信息 关闭套接字
		if( event.getEvent() & ( EPOLLRDHUP | EPOLLHUP ) )
		{
			//主要来讲还是需要完成对自己设置的处理 自己上锁 我不管了 这里可以在Socket类中添加一些属性 用来判断状态
			if( static_cast<bool>(tcpServer->getClose()) )
				(* (tcpServer->getClose()) )( tcpServer->getHelpPtr(), (*tcpServer->getMapping())[event.getUserDataWithFd()]);
			std::unique_lock<std::mutex> lock( *( tcpServer->getMutex() ) );
			//这里完成mapping_的清除工作
			auto x = tcpServer->getMapping()->find(event.getUserDataWithFd());
			//要关关闭套接字
			//不再重复触发从Epoll 中删除
			epoll_run->removeFd(event.getUserDataWithFd());
			//shudown( (x->second).getFd(), SHUT_RDWR);
			//(x->second).shudownWrite();
			//(x->second).shutdownRead();
			//std::cout << (x->second).getFd() << std::endl;
			(x->second).closefd();
			tcpServer->getMapping()->erase(x);
			//此时应带计数器减一
			tcpServer->getTable()->minusOneCnt(tid);
			lock.unlock();
		}
		if( event.getEvent() & EPOLLERR )
		{
			//因为这里要给客户端返回一个服务器错误的响应 对于web服务器来说 没想到之前无意的设计在这里会起到作用 根据TcpServer 来获取自己的数据结构getHelpPtr
			auto outClass = tcpServer->getHelpPtr();
			//一定要外部无关 所以这里采用的方案是 增加回调函数的设置 将关闭时的函数设置成一个可配置的回调
			//下来执行对应的外部类操作 想想关闭的原因 这个存在的作用是什么呢

			if( static_cast<bool>(tcpServer->getClose()) )
				(* (tcpServer->getClose()) )( tcpServer->getHelpPtr(), (*tcpServer->getMapping())[event.getUserDataWithFd()]);
			exit(-1);
		}
		else if( event.getEvent() & EPOLLIN )
		{
			bool status = false; //状态码 从全局设置 默认是一次操作后关闭 防止资源浪费
			//这里进行报文读取
			if( static_cast<bool>(tcpServer->getMessage()) )
				status = (* (tcpServer->getMessage()) )( tcpServer->getHelpPtr(), (*tcpServer->getMapping())[event.getUserDataWithFd()]);
			//目前状态设置就是要么继续要么关闭

			//这里主要是对mapping_加的锁
			if( status == false )
			{
				//这里也是先清除掉来自自定义数据结构的数据的保存
				if( static_cast<bool>(tcpServer->getClose()) )
					(* (tcpServer->getClose()) )( tcpServer->getHelpPtr(), (*tcpServer->getMapping())[event.getUserDataWithFd()]);
				std::unique_lock<std::mutex> lock( *( tcpServer->getMutex() ) );
				//这里完成mapping_的清除工作
				auto x = tcpServer->getMapping()->find(event.getUserDataWithFd());
				//要关关闭套接字
				//不再重复触发从Epoll 中删除
				epoll_run->removeFd(event.getUserDataWithFd());

				//::shudown( (x->second).getFd(), SHUT_RDWR);
				//std::cout << (x->second).getFd() << std::endl;
				//(x->second).shutdownRead();
				//(x->second).shudownWrite();
				(x->second).closefd();
				tcpServer->getMapping()->erase(x);
				//此时应带计数器减一
				tcpServer->getTable()->minusOneCnt(tid);
				lock.unlock();
			}else {
				//Epoll 采用ET模式触发 所以这里需要重新触发 并重新设置oneshut 连接不关闭连接继续 重置epoll 事件
				epoll_run->resetFd(event.getUserDataWithFd());
			}
		}
	}
}






TcpServer::TcpServer(int port , bool LogOpen) 
: localSocket_(port),
//工作线程组的的创建 先是不做任何事情 直接绑定
workerThreads_( nullptr ),
//日志队列的创建
logQueue_(),
//日志线程是否创建 日志线程在base里面 暂时的处理方式是先存在但是不实现功能
logThread_( LogOpen ? std::make_shared<LogThread>(): nullptr ),
//初始化一个用表
workTable_(std::make_shared<ThreadWorkTable>() ),
mutex_(std::make_shared<std::mutex>())
{
	workerThreads_.setAllTask(std::bind(subThreadFunc , this));
}






void TcpServer::start()
{
	//子线程开始运行前显然不能是默认的处理函数
	/*
		子线程自己添加tid吧
		流程是日志线程先开启
		子线程开启
		主线程开启
	*/
	if( static_cast<bool>(logThread_) )
	{
		(*logThread_).start();
	}
	//if( static_cast<bool>( MessageCallBack_ )  )
	//{
		//子线程开启 因为提前设置过了
		workerThreads_.start();

		//主线程开启真实运行 当然必须在子线程开启的前提下
		mainThreadFunc();
	//}
}



//此函数主要用来监听套接字然后得到结果并分配给各个子线程
void TcpServer::mainThreadFunc()
{
	while(true){
		Socket client = localSocket_.acceptNewClient();
		//std::cout << client.getFd() << std::endl;
		if(client.getFd() != -1){
			//std::cout << "一个新的连接到来了" << std::endl;
			//一个新的连接到来了 需要运行以下函数 使用bind能方便好多 如果不是nullptr代表有函数可调用
			if( static_cast<bool>(ConnectCallBack_) )
				(*ConnectCallBack_)( helpPtr_, client );
			//将这个加入到子线程的轮询中 就没主线程啥事了
			workTable_->addSocket(client);
			//主线程还需要对文件描述表进行添加操作
			std::unique_lock<std::mutex> lock(*mutex_);
			mapping_[client.getFd()] = client;
			lock.unlock();
		}
	}
}


//setMessageCallBack setConnectCallBack 这两个函数已经给出
