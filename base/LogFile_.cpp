#include"LogFile.h"
#include"FileUtil.h"
#include<regex>

using namespace Cmuduo;
using std::string;

LogFile::LogFile(const std::string & basename, const std::string & rootDir ,bool threadSafe )
:mutex_( threadSafe ? new std::mutex : nullptr ), 
basename_(basename),
rootDir_(rootDir)
{
	//根必须满足正则表达式
	//只能由字母或者数字构成
	std::regex re("[a-zA-Z0-9_]");
	std::match m_;
	assert ( regex_match(basename ,m_ ,re ) );
	
	rollFile();
}

//日志名的包含基础名(一般设置为服务器程序的名称) 年月日时间 最后更新的时间
string LogFile::getLogFileNmae()
{
	string logname(basename_);
	time_t now = ::time(nullptr);
	struct tm time_struct;
	gmtime_r( &now, &tm );

	char buffer[32];
	strftime(buffer , 32 , "%Y%m%d-%H%M%S" , &time_struct);
	logname += "-XX-";
	logname += buffer;

	logname += ".log";
	return logname;
}

void LogFile::append_unlocked(const char * logline, int len)
{
	file_->append(logline , len);
	if( file_->writtenBytes() > MaxSizeLimit ){
		//发生滚动也就是新创建一个文件 然后继续写入
		rollFile();
	}
	else{
		//获取当前时间
		time_t now = ::time(nullptr);
		time_t now_t = (now / SeconedPerDay) * SeconedPerDay;
		if( now_t != today_ )
			rollFile();
		else if( now - lastFlush_ > static_cast<time_t> (FlushTime) ) /*正常写入 其实已经在缓冲区了 这时只要刷新就行了*/
		{
			lastFlush_ = now;
			file_->flush();
		}
	}
}

//此函数用于更新日志系统的文件
/*
	函数的经历
	首先获取一个名字 和设置的根目录搭配变成完整的名字
*/
void LogFile::rollFile()
{
	string name = getLogFileNmae();
	string entireName = rootDir_ + ( rootDir_.back() != '/' ? string("/")+name : name );

	time_t now = ::time(nullptr);
	today_ = (now / SeconedPerDay) * SeconedPerDay;
	//更新私有数据
	lastFlush_ = now;
	file_->reset(new FileUtil::AppendFile(entireName) );
}
void LogFile::append(const char * logline , int len)
{
	if( static_cast<bool>( mutex_ ) )
	{
		std::lock_guard<std::mutex> lk(*mutex_);
		append_unlocked(logline , len);
	}else{
		append_unlocked(logline , len);
	}
}












