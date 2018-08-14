#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_


//请求类这里主要是保存内容
#include"../base/TimeStamp.h"
#include"../net/Socket.h"
#include<set>
#include<map>
#include<string>
#include<vector>

using std::string;
using Cmuduo::TimeStamp;
using std::vector;

namespace Cfish{
namespace WEB{
class HttpRequest 
{

	public:
	static const string NONESTRING;
	//这里提供一些从网络到一个完整报文的接口 从网络读取一个报文到一个请求对象中
	friend bool operator >> (Socket & , HttpRequest& );
	//或者采用
	bool getEntireMessage(Socket &);

	public:
	enum Query{
		INVALID,GET,POST,HEAD,PUT,DELETE,OPTIONS
	};
	HttpRequest() {setAllHeaders(); }
	~HttpRequest() { }
	inline const string& getUrl() const
	{
		return url_;
	}
	inline const int getQuery() const
	{
		return query_;
	}
	inline const string & getVersion() const
	{
		return version_;
	}
	//设置内容
	inline const string & setHeader(const string& key , const string & value){
		if(headers_.find(key) == headers_.end())
			return NONESTRING;
		headersAndContent_[key] = value;
		return value;
	}
	//查看内容
	inline const string& getHeader(const string & key) const{
		auto p_ = headersAndContent_.find(key);
		if( p_ == headersAndContent_.end() )
			return NONESTRING;
		return p_->second;
	}
	
	/*inline const string& getContent() const
	{
		return content_;
	}
	void setContent(const char * ct)
	{
		content_ = strdup(ct);
	}
	void clearContent()
	{
		content_.clear();
	}*/

	bool badRequest() const {
		return badRequest_;
	}

	inline const std::map<string, string>& getKeyAndvalue() const
	{
		
		return keyAndvalue_;
	}
	inline std::map<string, string>& getKeyAndvalue()
	{
		
		return keyAndvalue_;
	}
	private:

	//分析得到url和键值对
	void analysisUrl(const string &);
	//这个函数会自动分析字段的键值对然后存储起来
	void analysisLineContent(const string & line);

	//提供行检测 如果行检测整确则自动加入到映射中 并返回true
	bool judgeAndJoin(const string & str , const string& line);

	//这个比较特殊 所以单独写出 只用于判断和得出长度 如果出错则返回0否则返回要读取的字节数
	size_t judgeContentLength(const string & line);
	//读取脏数据
	void clearMessage(Socket & sock);

	//这个读取完如果发生错误 下来读脏数据 并且 这个会自动的设置响应状态
	bool getAndJudgeQueryUrlVersion(const string& str);
	int setQuery( const string & str );

	void setAllHeaders();

	string url_ ;
	int query_ ;
	string version_;
	TimeStamp receiveTime_;
	std::map<string ,string > headersAndContent_;
	std::set<string> headers_;
	//所有的数据均为键值对
	std::map<string, string> keyAndvalue_; //存储url中的键值对

	//状态码 如果是命令行错误 直接设置成400 并读取脏数据 默认200OK
	bool badRequest_ = false;
	//内容 这样肯定是不安全的额
	//std::string content_;
	//用来保存 分段内容 先不实现
	vector<string> contents_;
	
	//Cookie 这里是用来保存Cookie的 字段解析见工具文件
};

}

}
#endif
