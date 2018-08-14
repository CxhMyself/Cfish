#ifndef _LOG_FILE_
#define _LOG_FILE_

#include<mutex>
#include<memory>

#include"help.h"


namespace Cmuduo{
/*
	用来控制输入到文件
	只有一个并且一个控制一个根目录
	分析需求
	首先在程序执行期间如果开启写日志文件的功能 则这个起到作用 只不过控制不在自己这里
	当使用我的时候 我要进行的工作是自动生成一个文件并在流刷新的时候

	private:
	方法
	获取下一个文件的basename的算法

	属性
	一个根目录代表日志的存储位置 一般设置为当前工作目录下的 logs目录如果不存在则创建一个目录 默认为当前目录
	一个用来保证线程安全的锁
	一个日志名的基础名 用于自动构建文件名
	当前使用的文件描述符 或者文件指针
	当前文件大小
	static:
	限制条件 大小限制
	限制条件 时间限制
*/

namespace FileUtil{
	class AppendFile; //先声明
}

class LogFile{
	NONCOPYABLE(LogFile);

	LogFile(const std::string & basename, const std::string & rootDir = "." ,bool threadSafe = true );
	~LogFile();

	inline void setFlushTime(int ft){
		LogFile::FlushTime = ft;
	}
	inline void SetMaxSizeLimit(int msl){
		LogFile::MaxSizeLimit = msl;
	}
	inline void setCountWrite(int mcw){
		LogFile::MaxCountWrite = mcw;
	}
	void rollFile();

	void append(const char * logline , int len);

	private:
	//线程不安全的写入文件
	void append_unlocked(const char*str , int len);
	static std::string getLogFileNmae(const std::string& basename , int len );

	//配置文件可见
	std::string rootDir_ = ".";
	std::string basename_;

	time_t lastFlush_;
	time_t today_;
	std::shared_ptr<std::mutex> mutex_;
	//这个用作向文件中写数据
	std::shared_ptr<FileUtil::AppendFile> file_;
	//文件的最大长度限制  这里说一个程序只能有一个日志系统
	//这三个可以采用 配置文件的方式进行配置
	static int FlushTime = 100;
	static int MaxSizeLimit = 1000 * 1024 * 1024;
	static int MaxCountWrite = 1000;
	//零点更新
	static const int SeconedPerDay = 60*60*60 * 24;
};

}
#endif