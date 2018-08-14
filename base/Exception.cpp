#include"Exception.h"

const char* Cmuduo::Exception::what() const throw()
{
	return message_.c_str();
}

const char * Cmuduo::Exception::stackTrace() const throw(){
	return stack_.c_str();
}

void Cmuduo::Exception::fillStackTrace()
{
	//使用backtrace函数
	//申请一个数组
	const int len = 200;
	void *buffer[len];
	auto ret = ::backtrace(buffer , len);
	
	char ** strings = ::backtrace_symbols(buffer , ret);
	
	if(strings)
	{
		for(int i = 0; i < ret ; ++i)
		{
			stack_.append(strings[i]);
			stack_.push_back('\n');
		}
	}
	free(strings);
}