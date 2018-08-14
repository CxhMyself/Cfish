#ifndef _HELP_
#define _HELP_

#include<stdint.h>/*int64_t PRI*/
#include<string>
#include<string.h> /*使用提供的那些函数*/
#include<limits.h>

#define STATIC

#include<sys/time.h>
#include<cinttypes>		
#include<execinfo.h>

#include<algorithm>
/*
	自己的通用工具头文件引用区
*/
#include"copyable.h"

/*
	一个不可拷贝的声明
*/
#define NONCOPYABLE(x) public:\
	x(const x&) = delete;\
	x& operator=(const x&) = delete;\
	private:

	

namespace HELP{
	//提供一个相同之间的比较
	template<class T>
	struct less_than_compare
	{
		friend bool operator<=(const T& t1, const T& t2) { return !(t1 < t2); }
		friend bool operator>=(const T& t1, const T& t2) { return !(t2 < t1); }
		friend bool operator>(const T& t1, const T &t2){ return t2 < t1; }
	};
}

#endif
