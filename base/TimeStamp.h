#ifndef _TIMESTAMP_
#define _TIMESTAMP_

//help 中定义了一些boost 但是标准库中没有的 但是自己实现的内容
#include"help.h"
#include"copyable.h" 
#include"types.h"


namespace Cmuduo {
	struct ll{};
	class TimeStamp :
					public Copyable,
					public HELP::less_than_compare<TimeStamp> 		//用来通过< 设置 其他三个符号
	{
	public:
		TimeStamp(int64_t i = 0) :microSecTimes(i) {}
		void swap(TimeStamp& t);			//
		
		
		std::string toString()const; 			 //
		//格式化输出toFormattedString
		std::string toFormattedString(bool includeMicroSec) const;
		
		
		//根据时间获取Unix的时间
		bool valid() const { return microSecTimes > 0; }
		int64_t getMicroSecTimes() const  
		{
			return microSecTimes;
		}
		//get SecTimes
		int getSecTimes() const
		{
			return static_cast<time_t>( this->microSecTimes / perSecond );
		}

	public STATIC:
		static TimeStamp now();
		static TimeStamp invalid()
		{
			return TimeStamp();
		}
		static TimeStamp getFromUnixTime(time_t t)
		{
			return getFromUnixTime(t, 0);
		}
		static TimeStamp getFromUnixTime(time_t t , int microSec)
		{
			return TimeStamp (static_cast<int64_t>(t)*perSecond);
		}
		static const int perSecond = 1000 * 1000;
	private:
		int64_t microSecTimes = 0;		
	};
	
	//附带函数
	inline bool operator<(const TimeStamp &h , const TimeStamp& low )
	{
		return h.getMicroSecTimes() < low.getMicroSecTimes();
	}
	inline bool operator==(const TimeStamp &h , const TimeStamp& low )
	{
		return h.getMicroSecTimes() == low.getMicroSecTimes();
	}
	inline double timeDifference(const TimeStamp &h , const TimeStamp& low )
	{
		double result = static_cast<double> ( h.getMicroSecTimes() - low.getMicroSecTimes());
		return (result > 0? result : -result) / TimeStamp::perSecond;
	}
	inline TimeStamp addTime(const TimeStamp& Time ,int s  )
	{
		return TimeStamp(Time.getMicroSecTimes() + static_cast<int64_t>( s*TimeStamp::perSecond ));
	}
}

#endif