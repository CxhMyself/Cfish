#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include"help.h"
#include<exception>

namespace Cmuduo{
	//此类用于给其他的继承实现功能
	class Exception : public std::exception{
		public:
		Exception(const char * msg) : message_(msg) { fillStackTrace(); }
		Exception(const std::string& str) : message_(str){ fillStackTrace(); }
		virtual ~Exception() throw() {}
		virtual const char* what() const throw();
		//获取所有的经过的函数
		const char * stackTrace() const throw();
		private:
		void fillStackTrace();
		
		std::string message_;
		std::string stack_;
	};
}

#endif