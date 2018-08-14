#include"Logging.h"


//对内部类Impl的定义
namespace Cmuduo{

//设置线程独有的变量
__thread char *errorBuffer[128];
const char * strerror_t1(int errorCode)
{
	return strerror_r(errorCode , errorBuffer ,sizeof errorBuffer);
}

//初始化日志等级 用于给全局日志等级的初始化
Logger::LogLevel initLogLevel()
{
  if (::getenv("CMUDUO_LOG_TRACE"))
    return Logger::TRACE;
  else if (::getenv("CMUDUO_LOG_DEBUG"))
    return Logger::DEBUG;
  else
    return Logger::INFO;
}
Logger::LogLevel g_logLevel = initLogLevel();

//根据日志等级来确定日志名
const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

TimeStamp time_;
		LogLevel lv_;
		LogStream stream_;
		int line_;
		SourceFile basefile_;

Logging::Impl::Impl(LogLevel level, int old_errno, const SourceFile& file, int line)
:time_(TimeStamp::now()),lv_(level),line_(line),stream_(),basefile_(file)
{
	
}

}