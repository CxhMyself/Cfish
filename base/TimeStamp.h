#ifndef _TIMESTAMP_
#define _TIMESTAMP_

//help �ж�����һЩboost ���Ǳ�׼����û�е� �����Լ�ʵ�ֵ�����
#include"help.h"
#include"copyable.h" 
#include"types.h"


namespace Cmuduo {
	struct ll{};
	class TimeStamp :
					public Copyable,
					public HELP::less_than_compare<TimeStamp> 		//����ͨ��< ���� ������������
	{
	public:
		TimeStamp(int64_t i = 0) :microSecTimes(i) {}
		void swap(TimeStamp& t);			//
		
		
		std::string toString()const; 			 //
		//��ʽ�����toFormattedString
		std::string toFormattedString(bool includeMicroSec) const;
		
		
		//����ʱ���ȡUnix��ʱ��
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
	
	//��������
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