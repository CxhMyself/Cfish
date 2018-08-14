#ifndef _ATOMICLNTEGER_H_
#define _ATOMICLNTEGER_H_

#include"help.h"

namespace Cmuduo{
namespace detail{
	

template<class T>
class AtomiclntegerT{
	
	//声明此类不可拷贝
	NONCOPYABLE(AtomiclntegerT<T>)

	public:	
	AtomiclntegerT() = default;

	T get()
	{
		return __sync_val_compare_and_swap(&value_t_ , 0 , 0);
	}

	//先获取再加
	T getAndAdd( const T x )
	{
		return __sync_fetch_and_add( &value , x);
	}
	//先加再获取
	T addAndGet( const T x )
	{
		return getAndAdd(x) + x;
	}
	T incrementAndGet()
	{
		return addAndGet(1);
	}

	T decrementAndGet()
	{
		return addAndGet(-1);
	}
	void add(T x)
	{
		getAndAdd(x);
	}
	void increment()
	{
		incrementAndGet();
	}
	void decrement()
	{
		decrementAndGet();
	}
	
	T getAndSet(T newval)
	{
		__sync_lock_test_and_set(&value_t_ , newval);
	}
	private:
	volatile T value_t_; // 可变的
};
}

using detail::AtomicInt32 = AtomiclntegerT<int32_t>;
using detail::AtomicInt64 = AtomiclntegerT<int64_t>;

}

#endif