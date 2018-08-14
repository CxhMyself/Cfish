#include"TimeStamp.h"

using namespace Cmuduo;
static_assert(sizeof(Cmuduo::TimeStamp) == sizeof(int64_t),"size error");

//static function
TimeStamp TimeStamp::now()
{
	//获取当前的时间并且设置
	struct timeval tv;
	gettimeofday(&tv , nullptr );
	return TimeStamp( static_cast<int64_t>(tv.tv_sec*perSecond) + tv.tv_usec );
}
//function
std::string TimeStamp::toString() const
{
	//将秒数和微秒数得到
	char buffer[32];
	sprintf(buffer ,"%d:%06" PRId64 "", getSecTimes() , getMicroSecTimes() );
	return buffer;
}

std::string TimeStamp::toFormattedString(bool includeMicroSec) const
{
		struct tm t;
	std::string format = "%4d%02d%02d %02d:%02d:%02d";

	if( includeMicroSec )
		format += ".%06d";

	time_t sec = static_cast<time_t>(microSecTimes / TimeStamp::perSecond);

	gmtime_r(&sec , &t);
	char buffer[128] = "0";
	int ret = sprintf(buffer , format.c_str() , 
		t.tm_year,t.tm_mon, t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec
	);
	if(includeMicroSec)
		sprintf(buffer + ret, ".%06d" ,static_cast<int>(microSecTimes - sec * perSecond ) );
	return buffer;	
}