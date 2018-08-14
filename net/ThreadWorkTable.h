#ifndef _THREAD_WORK_TABLE_H_
#define _THREAD_WORK_TABLE_H_

#include<list>
#include<vector>
#include<algorithm>
#include<memory>
#include<mutex>

#include"Socket.h"
#include"Epoll.h"
#include<map>
#include<iostream>
//这里之后要改 改成线程安全的																									**********************************************************
namespace Cfish{
//这里类里面表示是一个表 对应的信息有一个线程的tid 因为唯一 和 此线程中管理的 FdRoot 同一源的个数
//线程的选择在于make_heap
/*class ThreadWorkTableItem{
	public:
	
	using SocketList = std::list< Socket >;

	ThreadWorkTableItem(int tid): tid_(tid) , fds_() {}
	
	//用于子线程轮询
	inline const SocketList & getList() const { return fds_; }

	inline int getTid() const { return tid_; }

	inline void addSocket(const Socket& sock){ 
		fds_.push_back( sock );
	}//添加一个同一源
	
	inline void deleteSocket(const Socket & sock){
	
		//auto x = std::find_if(fds_.begin() , fds_end() , [fd]( const std::shared_ptr< FdRoot > ff ){ ff->getFd() == fd.getFd() }); 
		//if( x != fds_.end() )
		for(auto x = fds_.begin() ; x != fds_.end() ; x++ )
			if( x->getFd() == sock.getFd()  )
				fds_.erase(x);
	}

	inline size_t size() const { return fds_.size(); }

	inline bool operator < ( const ThreadWorkTableItem &t1) { return size() < t1.size(); }

	private:
	int tid_;
	//这里的list是动态变化的
	std::list< Socket > fds_;
};
*/
//工作表要改 要可以直接加Socket而 不用tid
/*class ThreadWorkTable{
	public:
	ThreadWorkTable() = default;
	
	inline const std::list< Socket >&  getFdRootListByTid(int tid) {
		int i;
		//auto x = find_if( items_.begin() , items_.end() ,[tid](const ThreadWorkTable & tw){ return tw.getTid() == tid; } );
		for(i = 0 ; i < items_.size() ; i++)
			if(items_[i].getTid() == tid)
				break;
		if( i != items_.size() ){
		//if( x != items_.end() )
			return items_[i].getList();
		}
	}
	//用来初始化
	inline bool addTid(int tid)
	{
		std::lock_guard<std::mutex> lk(mutex_);
		//auto x = find_if(items_.begin() , items_.end() ,[](const ThreadWorkTable & tw) { return tw.getTid() == tid; }  )
		//if( x == items_.end() ){
		int i;
		for(int i = 0 ; i < items_.size() ; i++)
			if(items_[i].getTid() == tid)
				break;
		if( i != items_.size() ){
			items_.push_back(ThreadWorkTableItem(tid));
			return true;
		}
		return false;
	}
	inline void addSocket(const Socket& fr)
	{
		std::lock_guard<std::mutex> lk(mutex_);
		//对于栈顶元素进行添加
		items_[0].addSocket(fr);
		//重新整顿堆
		std::make_heap(items_.begin() , items_.end());
	}
	inline bool deleteSocketByTid( int tid , const Socket &fr )
	{
		std::lock_guard<std::mutex> lk(mutex_);
		//auto x = find_if( items_.begin() , items_.end() ,[tid](const ThreadWorkTable & tw){ return tw.getTid() == tid; } );
		
		int i;
		for(i = 0 ; i < items_.size() ; i++)
			if(items_[i].getTid() == tid)
				break;
		if( i != items_.size() ){
			items_[i].deleteSocket(fr);
			std::make_heap(items_.begin() , items_.end() );
			return true;
		}
		return false;
	}

	private:
	std::vector<ThreadWorkTableItem> items_;
	std::mutex mutex_;
};
*/

class ThreadAndCnt{
	public:
	explicit ThreadAndCnt(std::thread::id tid) : tid_(tid),cnt_(0) {}
	void addOne(){ ++cnt_; }
	void minusOne(){ if( cnt_ - 1 < 0 ) return; --cnt_; }
	std::thread::id getTid() const { return tid_; }
	int getCnt() const { return cnt_; }
	inline bool operator < ( const ThreadAndCnt& t )
	{
		return cnt_ < t.getCnt();
	}
	private:
	std::thread::id tid_;
	int cnt_;
};

class ThreadWorkTable{
	public:
	
	ThreadWorkTable() = default;
	
	
	//默认只有可读事件 和开启ET 模式 并设置oneshot
	inline void addSocket(const Socket& fr)
	{
		std::lock_guard<std::mutex> lk(c_mutex_);
		cntPerThread_[0].addOne();
		//将此事件加入到对应的Epoll里面
		epollPerThread_[cntPerThread_[0].getTid()]->addFd(fr.getFd());

		std::make_heap(cntPerThread_.begin() , cntPerThread_.end());
	}
	inline bool minusOneCnt(std::thread::id tid)
	{
		//检测下是否存在
		std::lock_guard<std::mutex> lk(c_mutex_);
		int i;
		for( i = 0 ; i < cntPerThread_.size() ; ++i)
			if( cntPerThread_[i].getTid() == tid )
				break;
		if( i == cntPerThread_.size() )
			return false;
		cntPerThread_[i].minusOne();
		std::make_heap(cntPerThread_.begin() , cntPerThread_.end());
		return true;
	}
	
	inline bool addTid(std::thread::id tid)
	{
		for(int i = 0 ; i < cntPerThread_.size() ; ++i)
			if( cntPerThread_[i].getTid() == tid )
				return false;
		cntPerThread_.push_back(ThreadAndCnt(tid));
		
		if( epollPerThread_.find(tid) == epollPerThread_.end() ){
			epollPerThread_.insert(std::make_pair(tid , std::make_shared<Epoll>()) );
			return true;
		}
		return false;
	}
	std::shared_ptr<Epoll> getEpollPerThreadByTid(std::thread::id tid)
	{
		return epollPerThread_[tid];
	}
	private:
	std::mutex c_mutex_;
	//每个线程的计数 用tid 引用 这个是线程安全的
	std::vector<ThreadAndCnt> cntPerThread_;
	//每个线程对应的一个Epoll 这个本身来讲就是线程安全的
	std::map<std::thread::id , std::shared_ptr<Epoll> > epollPerThread_;
};

}
#endif