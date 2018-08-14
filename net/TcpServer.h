#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include"../base/LogThread.h"
#include"../base/help.h"
#include"../base/ThreadPool.h"
#include"ServerAndClientSocket.h"
#include"ThreadWorkTable.h"
#include<memory>
#include<map>

namespace Cfish{
/*
	要不然全部的元素都改成智能指针
	最终决定采用统一管理Socket 将文件描述符fd 直接传入子线程 并且对于子线程来说 只有一个计数器 用来统计目前轮询的数量进行最小堆的组建 干脆专门封装一个EpollThreadPool

	思考都需要什么 因为这个是全局管控的
	所以需要一个线程池 这个线程池每个线程为工作线程 和两个单独线程 分别为 日志线程和主线程 主线程不做管理 这是两个对象

	这里采用的是 主线程监听消息 如果到来分配给各个子线程（通过最小堆的方式进行给哪个线程的选取）然后子线程通过轮询这些受自己监听的已连接套接字或者信号等（统一事件源）处理任务
	上述过程中所有的日志先输出到一个日志的队列中 然后在日志线程中进行日志的输出
	综上所述 我需要维护的数据有一个日志队列 一个线程和套接字数组的对应关系
	网络方面
	只考虑主线程 因为其他的线程功能都在线程函数中实现 且所用的数据均可为局部变量
*/
//讲道理 一般的网络程序工作线程应该进行的工作都是一样的所以这里就只设置相同工作模式的ThreadPool
class TcpServer{
	NONCOPYABLE(TcpServer);
	
	using Task = std::function<void()>;
	//一个TcpServer可能代表 我需要获取的本地Tcp信息
	using MessageFunction = std::function< bool (void *, Socket&) >;
	public:


	/*
	 * 一些辅助功能的函数
	 * */
	//返回本地的监听套接字
	inline int getFd() const {
		return localSocket_.getFd();
	}
	inline int getPort() const{
		return localSocket_.getPort();
	}
	inline const std::string getIp() const {
		return localSocket_.getIp();
	}
	inline void * getHelpPtr() const
	{
		return helpPtr_;
	}
	//构造一个TcpServer 想想 是不是只需要设置一个端口号呢 因为暂时没有日志功能
	TcpServer( int port /*端口号*/, bool LogOpen = false /*是否开启日志*/);
	//主线程开始运行 监听 并返回TcpConnect 一个Tcp连接 其中包含一个TcpClient TcpClient 用于传送给子线程然后子线程通过连接的fd 获取信息 处理信息(也可以直接关闭连接)
	void start();

	//未开始都可以set
	void setMessageCallBack( const MessageFunction & t ,void *ptr){ MessageCallBack_ = std::make_shared<MessageFunction>(t); helpPtr_ = ptr; }
	void setConnectCallBack( const MessageFunction & t ,void *ptr){ ConnectCallBack_ = std::make_shared<MessageFunction>(t); helpPtr_ = ptr; }
	void setCloseCallBack( const MessageFunction & t ,void *ptr)  { CloseCallBack_	 = std::make_shared<MessageFunction>(t); helpPtr_ = ptr; };
	private:
	//负责监听的
	ServerSocket localSocket_;

	//这里设置一个多线程服务器程序所需要的那些数据
	//一个线程安全的日志队列 巧了 我在ThreadPool中写了 但是日志队列的内容该维护成什么样子呢 这是个问题 字符串队列吧 反正到了最后也是输出字符串
	threadsafe_queue<std::string > logQueue_;
	//一个map 维护线程和统一源的套接字 tid 和 一个链表这样子加入和删除更加的快 FdRoot是一个抽象基类 因为要做成统一事件源 叫工作安排表再合适不过了 里面全是智能指针 其实可以用句柄 只不过感觉很麻烦的样子
	std::shared_ptr< ThreadWorkTable> workTable_;

	private:
	//这里表示日志线程所需要运行的函数 因为可有可无 所以采用智能指针的方式进行存储 日志线程根据队列进行操作
	std::shared_ptr<LogThread> logThread_;

	//其内置实际上是Epoll轮询 记得初始化的时候传入一个Task 用来操控每个线程中执行的任务 但是线程池内也有一个默认程序 是epoll 循环 循环里面存在这MessageCallBack()
	Cfish::ThreadPool workerThreads_;
	//文件描述符和套接字
	std::map<int , Socket> fdAndSocket_;
	//回调函数之 MessageCallBack 子线程调用
	std::shared_ptr<MessageFunction> MessageCallBack_ = nullptr;

	//回调函数之 ConnectCallBack
	std::shared_ptr<MessageFunction> ConnectCallBack_ = nullptr;

	//回调函数之 CloseCallback
	std::shared_ptr<MessageFunction> CloseCallBack_ = nullptr;
	
	private:
	//这里表示主线程所需要的运行函数
	void mainThreadFunc();

	//辅助指针用来存储外界的东西
	void *helpPtr_;
	private:
	//存储文件描述符和套接字的信息 新加入后则在这里进行添加 这个的存在主要是为了 未来时钟检测
	std::map<int, Socket> mapping_;
	//用来负责对mapping_的增删操控
	std::shared_ptr< std::mutex> mutex_;

	public:
	//所有的外调需要的 全部返回智能指针 写成模板的样子吧
	enum {
		MUTEX,
		MAPPING,
		WORKTABLE,
		MESSAGE
	};
	std::shared_ptr<std::mutex> getMutex(){
		return mutex_;
	}
	std::shared_ptr<std::map<int, Socket> > getMapping()
	{
		return std::make_shared<std::map<int ,Socket> >(mapping_);
	}
	std::shared_ptr<ThreadWorkTable> getTable()
	{
		return workTable_;
	}
	std::shared_ptr<MessageFunction> getMessage()
	{
		return MessageCallBack_;
	}
	std::shared_ptr<MessageFunction> getConnect()
	{
		return ConnectCallBack_;
	}
	std::shared_ptr<MessageFunction> getClose()
	{
		return CloseCallBack_;
	}
};
//子线程需要运行的程序 必须传入一个TcpServer实例

//还不能使用这个引用因为非法拷贝 必须把所有的调用改成一个完整的调用 所有的操作变成方法
//void subThreadFunc(TcpServer * tcpServer);
}
#endif