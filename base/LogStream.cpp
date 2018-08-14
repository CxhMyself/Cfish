#include"LogStream.h"
#include"LogFile_.h"

//日志流的实现

using namespace Cmuduo;

//通过这里进行日志文件的设置	
namespace Cmuduo{//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~未来鉴定
namespace LOG{
/*
	LogFile_ * logfile = nullptr;// 默认为nullptr
	//这里进行的操作是
	void startFile(){
		logfile = new LogFile_();
	}
	void closeFile(){
		delete logfile;
		logfile = 0;
	}
	*/
/*
	std::string g_logFileName = "";
	void setLogFile(const char *filename){
		//保存的是日志文件的绝对路径
		if( filename[0] == '.' )
			g_logFileName = filename;
		else{
			char buffer[PATH_MAX+NAME_MAX];
			getcwd(buffer , sizeof(buffer) );
			g_logFileName = std::string(buffer);
			g_logFileName += "/" ;
			g_logFileName += filename;
		}
		//首先获取当前目录
	}
	void setLogFile(const std::string &filename){
		char buffer[PATH_MAX+NAME_MAX];
		//保存的是日志文件的绝对路径
		if( filename.at(0) == '.' )
			g_logFileName = filename;
		else{
			char buffer[PATH_MAX+NAME_MAX];
			getcwd(buffer , sizeof(buffer) );
			g_logFileName = std::string(buffer);
			g_logFileName += "/" ;
			g_logFileName += filename;
		}
	}
	void reSet()
	{
		g_logFileName = "";
	}
*/
}



//detail 专门用来定义内部的东西
//namespace detail{

//为啥要从9-0-9 直接0-9 不是更好
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20 , "digits error");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17 , "digits error");

	//将数字转换为字符串的形式
template<class T>
size_t convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0)
  {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}
//这里是将地址值转换成整数表示 并且用十六进制输出
size_t convertHex(char buf[], uintptr_t value)
{
  uintptr_t i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

//}
template<class T>
void LogStream::formatInteger(T value)
{
	//如果空间足够
	if(buffer_.avail() > kMaxNumericSize )
	{
		size_t ret = convert( buffer_.current(), value);
		buffer_.addLen(ret );
	}
}

LogStream& LogStream::operator<<(short v)
{
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(double v)
{
	//将double重定向
	if (buffer_.avail() >= kMaxNumericSize){
		size_t ret = sprintf( buffer_.current() , "%.12lf" , v);
		buffer_.addLen(ret);
	}
	return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
//将指针数据转换成整数的形式
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
 
  if (buffer_.avail() >= kMaxNumericSize)
  {
    char* buf = buffer_.current();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = convertHex(buf+2, v);
    buffer_.addLen(len+2);
  }
  return *this;
}

void LogStream::flush()
{
	//如果不为空
	if( !LOG::g_logFileName.empty() )
	{
		//将流的内容输出到文件
		
	}
	
}