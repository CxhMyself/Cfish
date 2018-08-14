#ifndef _FIELD_PARSING_H_
#define _FIELD_PARSING_H_

#include<string>
#include<json/json.h>
//有效性验证所需要的工具 因为包含Cookie字段
#include"ValidUtil.h"
//这个文件主要是产生 一系列字段解析工具
namespace Cfish{
	class RangeParsing{
		public:
		RangeParsing(const std::string & ,size_t allSize);
		~RangeParsing() = default;
		std::string getResponse() const ;
		off_t getStart() const ;
		off_t getEnd() const ;
		size_t getReadSize() const;
		bool badRange() const;

		private:
		//起始位置
		off_t start_;
		//终止位置
		off_t end_;
		//总长度
		std::string allSize_;
		//是否正确
		bool badRange_;
	};

	//这里将Cookie进行解析 并得到其中的token数据 可以用数据再进行研恒如果正确 也就是说这个还进行解密加密的操作 如果有效便可以从中分析出用户并加到内容表里
	//如果无效 根据返回的数值u判断是哪种无效
	bool judgeCookie(const std::string & cookie , std::string & tk);
	class CookieTokenParing{
		public:
		//一串来初始化 抛弃所有的除了Token之外的数据
		CookieTokenParing(const std::string &str);
		~CookieTokenParing() = default;
		//这个未来可以设置成多种情况
		bool valid(const Key &key , std::map<std::string ,std::string> & mapping) ;
		
		private:
		//这里 我选择直接解析出需要设置的和解析的三个字段 分别是head Payload Signature
		std::string head_;
		std::string payload_;
		std::string signature_;
	};

	//这里进行Set-Cookie: 的设置 这里只有在 得到的数据里面有
	class SetCookieJWT{
		public:
		SetCookieJWT();
		~SetCookieJWT() = default;

		//设置payload 的内容
		void setIp(const std::string &ip);
		void setDate(const std::string &date);
		void setAccount(const std::string &account); 
		//获取Json的字符串格式
		std::string toAfterEncodeStr(const Key & key )const ;
		private:
		//初始化head
		void initHead();
		Json::Value head_;
		Json::Value payload_;
	};
	//judge Set-Cookie can or not set
	bool judgeSetCookie(const std::map<std::string , std::string > &map , int fd , const Key& , std::string & );
}

#endif
