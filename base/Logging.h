#ifndef _LOGGINT_H_
#define _LOGGINT_H_

//先包含这些头文件之后再加

#include"LogStream.h"
#include"TimeStamp.h"
#include"help.h"

namespace Cmuduo{

class Logging{
	public:
	
	//设置日志等级
	
	enum LogLevel
	{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS, //haha  还有这种操作
	};

	//一个用来表示文件名的类
	class SourceFile																								//class SourceFile start
	{
		public:
		explicit SourceFile(const char * filename)
		:data_(filename)
		{
			const char * realName = strrchr(filename , '/');
			if( realName ) /*不为NULL 便代表找到了*/
			{
				data_ = realName+1; /*因为找到的位置是/ 所在的位置*/
			}
			size_ = static_cast<int>(strlen(data_));
		}
		
		//这个构造出的长度不一致
		template<int N>
		inline SourceFile(const char (&arr)[N])
		:data_(arr),size_(N)
		{
			const char * realName = strrchr(data_ , '/');
			if( realName ) /*不为NULL 便代表找到了*/
			{
				data_ = realName + 1; /*因为找到的位置是/ 所在的位置*/
			}
			size_ -= static_cast<int>(data_-arr);
		}
		const char * data_;
		int size_;
	};																												//class SourceFile ending
	
	//定义静态函数用于控制全局的日志等级
	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	//几个构造函数和析构函数
	Logging(SourceFile file,int line );

	private:
	class Impl{
		public:
		using LogLevel = Logging::LogLevel;
		Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
		//俩函数待定
		void formatTime();
		void finish();
		
		private:

		TimeStamp time_;
		LogLevel lv_;
		LogStream stream_;
		int line_;
		SourceFile basefile_;

	} impl_ ;
};

//全局的日志等级的查看和设置
extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}
inline void Logging::setLogLevel(LogLevel level)
{
	g_logLevel = level;
}

//这个函数用来设置error和其提示
const char * strerror_t1(int errorCode);

#endif