#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include<set>

//这个加一个如果是C++0x版本 则存在右值引用
#include"help.h"

#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>

//线程队列
#include<queue>

namespace Cfish{

template<class T>
class threadsafe_queue
{
public:
    threadsafe_queue() = default;
    threadsafe_queue(const threadsafe_queue &Q)
    {
        std::lock_guard<std::mutex> lk(mut);
        dq = Q.dq;
    }
    threadsafe_queue& operator=(const threadsafe_queue & ) = delete;
 

    void push(const T data)
    {
        std::lock_guard<std::mutex> lk(mut);
        dq.push(data);
        cond.notify_one();
    }
	
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if( dq.empty() )
            return false;
        value = dq.front();
        dq.pop();
        return true;
    }
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if( dq.empty() )
            return std::shared_ptr<T> ();
        std::shared_ptr<T> res( std::make_shared<T>(dq.front()) );
        dq.pop();
        return res;
    }
    void wait_and_pop(T& value) // 用来得到顶部数据以及线程安全
    {
        std::unique_lock<std::mutex> lk(mut);
        cond.wait(lk, [this] {return !(this->dq).empty() ; } );
        value = dq.front();
        dq.pop();
    }
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        cond.wait(lk, [this] {return !(this->dq).empty() ; } );
        std::shared_ptr<T> result{ std::make_shared<T>(dq.front()) };
        dq.pop();
        return result;
    }
    bool empty()
    {
        std::lock_guard<std::mutex> lk(mut);
        return dq.empty();
    }


	
private:
    //分别是互斥量 条件变量 和队列本身
    std::queue<T> dq;
    std::mutex mut;
    std::condition_variable cond;
};

//定义两种模式的线程池 一种是工作线程池 工作不一样 另一种是所有线程做一样的工作的构造的时候给出构建哪种模式的线程
#define DIFFERENCEWORKER 	1
#define IDENTICALWORER		2

class ThreadPool{

	NONCOPYABLE(ThreadPool);

	//每个线程真正运行的地方 两种模式
	void run_difference();
	void run_identical();
	//默认的所有线程行为方式

	public:
	using Task = std::function<void()>;

	explicit ThreadPool(int thread_num = 0 ,const std::string & nameArg = std::string("ThreadPool"));
	explicit ThreadPool( const std::shared_ptr<Task> task ,int thread_num = 0 ,const std::string & nameArg = "ThreadPool" );

	//真正开启线程池中每个线程的行进
	void run();
	//自动关闭线程所以在析构这里一一加入
	~ThreadPool();
	//显式的开启 这里只做线程池的创建
	void start();
	//显式的关闭
	void close();
	//添加任务到任务队列中
	void addTask(const Task& );
	//设置所有的任务的执行线程函数 这个未来可以使用bind 函数
	inline void setAllTask(const Task& task)
	{
		taskFun_ = std::make_shared<Task>( task ) ;
	}
	inline void setAllTask(const std::shared_ptr<Task> t )
	{
		taskFun_ = t;
	}
	const std::set<std::thread::id > &getTids()const 
	{
		return tids_;
	}
#ifdef __GXX_EXPERIMENTAL_CXX0X__
void addTask(Task&& t);
#endif
	private:
	inline void fun()
	{
		std::lock_guard<std::mutex> lock(tidsMutex_);
		tids_.insert( std::this_thread::get_id() );
	}
	private:
	//设置一个线程池
	std::vector<std::thread> threadPool_;
	//如果直接存对象的话 要考虑存指针(因为是同过继承的方式进行) 这里还不如存一个函数对象
	threadsafe_queue<Task> tasks_;
	//线程池的名字
	std::string name_;
	//是否正在运行
	bool running_;
	//线程数 除去主线程 和可能存在的日志线程
	size_t maxThreadsNum = std::thread::hardware_concurrency()*2 - 2;
	//poolmode
	int poolMode_;
	//默认的线程工作模式 干脆一智能指针啥都解决了
	std::shared_ptr<Task> taskFun_ ;
	//记录所有线程的线程tid 加锁吧
	std::mutex tidsMutex_;
	std::set<std::thread::id> tids_;
};

}
#endif