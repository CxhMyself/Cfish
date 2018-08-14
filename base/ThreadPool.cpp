#include"ThreadPool.h"

#include<iostream>
using namespace Cfish;

ThreadPool::ThreadPool(int thread_num  ,const std::string & nameArg) : name_(nameArg) , poolMode_(DIFFERENCEWORKER) {
	maxThreadsNum = thread_num <= 0 ? maxThreadsNum : thread_num;

	running_ = false;
}
ThreadPool::ThreadPool(const std::shared_ptr<Task> task,int thread_num ,const std::string & nameArg ) : name_(nameArg), taskFun_(task) , poolMode_(IDENTICALWORER) {
	maxThreadsNum = thread_num <= 0 ? maxThreadsNum : thread_num;
	std::cout << "[max threads num]" << std::thread::hardware_concurrency() << std::endl;
	running_ = false;
}
void ThreadPool::start()
{
	//开始只是代表创建了线程池 并不代表线程池开始行进
	try{
		if( poolMode_ == IDENTICALWORER )
			for(int i = 0; i < maxThreadsNum ; ++i)
				threadPool_.push_back(std::thread( &ThreadPool::run_identical , this ) );
		else // 不然就默认走
			for(int i = 0; i < maxThreadsNum ; ++i)
				threadPool_.push_back(std::thread( &ThreadPool::run_difference , this ) );
	}
	catch(...)
	{
		this->~ThreadPool();
		throw;
	}
}
//这个单指真正开始运行
void ThreadPool::run()
{
	running_ = true;
}
void ThreadPool::close()
{
	running_ = false;
}

//进行任务的加入
void ThreadPool::addTask(const Task& t)
{
	//队列是线程安全的所以直接添加就好
	this->tasks_.push(t);
}

//如果定义了 C++11 则可以使用右值引用
#ifdef __GXX_EXPERIMENTAL_CXX0X__
void ThreadPool::addTask(Task&& t)
{
	//这里其实也是直接加入就好了
	tasks_.push(std::move(t));
}
#endif

#include<iostream>

//真实运行的场所
void ThreadPool::run_difference()
{
	std::cout << "我到过这了？？" << std::endl;
	tids_.insert( std::this_thread::get_id() ) ;
	while(running_)
	{
		auto item = tasks_.try_pop();
            if( item.get() != nullptr )
            {
                /*执行任务 直接执行就好*/
				(*item)();
				
            } else {
				//如果没任务则让步
                std::this_thread::yield();
            }
	}
}
void ThreadPool::run_identical()
{
	fun();
	if( static_cast<bool>(taskFun_) )
		(*taskFun_)(); 
}

ThreadPool::~ThreadPool()
{
	//std::cout << running_ << std::endl;
	if( running_ ){
	running_ = false;
	for(int i = 0 ; i < maxThreadsNum ; ++i)
		threadPool_[i].join();
	}
}

