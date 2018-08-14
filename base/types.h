#ifndef _TYPES_H_
#define _TYPES_H_


//如果不是调试状态
#ifdef NDEBUG
#include<assert.h>
#endif


namespace Cmuduo{
	
//简化 直接使用string
/*
	如果  dynamic_cast<To>(f) 转换失败则应当提示错误是失败的
*/
template<class To , class From>
inline To down_cast(From * f){
#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
	assert(f == nullptr || dynamic_cast<To>(f) != nullptr);
#endif
	return static_cast<To>(f);
}

}

#endif