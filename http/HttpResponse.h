#ifndef _HTTP_RESPONSE_
#define _HTTP_RESPONSE_

#include"HttpRequest.h"
#include"../net/Socket.h"
#include"../base/StrUtil.h"
#include<sys/uio.h>
#include<map>
#include<set>
#include "HttpStatueCode.h"
#include<iostream>
#include<sys/mman.h>
namespace Cfish{
namespace WEB{

//只能依赖于请求头进行构造
class HttpResponse{
	public:

	friend Socket& operator<< (Socket & ,const HttpResponse &);

	void sendToNet(Socket& ) const;
	static const string NONESTRING;

	~HttpResponse(){
		if( fileIov_.iov_base != nullptr )
			::munmap(fileIov_.iov_base , fileIov_.iov_len);
	}

	HttpResponse() = delete;
	explicit HttpResponse(const HttpRequest& hr);
	//根据后缀获取content type
	string getTypeBySuffix(const string & sub);
	//friend Socket& operator << (Socket& ,const HttpResponse& );
	//显式的发送报文
	void sendToNet(Socket& );

	//设置内容
	inline const string & setHeader(const string& key , const string & value){
		if(headers_.find(key) == headers_.end())
			return NONESTRING;
		headersAndContent_[key] = value;
		return value;
	}
	//查看内容
	inline const string& getHeader(const string & key){
		auto p_ = headersAndContent_.find(key);
		if( p_ == headersAndContent_.end() )
			return NONESTRING;
		return p_->second;
	}

	inline const string & getUrl()const{
		return url_;
	}
	inline bool keepAlive()const {
		return keepAlive_;
	}
	inline void setCode(const string & str1, const string &str2 ){
		codeAndStr_ = str1+ " " + str2;
	}
//	进行内容的设置 如果未来经过CGI修改过后 长度必然不一样
	inline void setBodyData(const struct iovec* iov){
		if( fileIov_.iov_base != nullptr )
		{
			::munmap(fileIov_.iov_base,fileIov_.iov_len);
			fileIov_.iov_base = nullptr;
			fileIov_.iov_len = 0;
		}
		//进行内容的设置和长度的设置 iov 是通过对应的程序得到文件内容或者处理结果
		fileIov_.iov_base = iov->iov_base;
		fileIov_.iov_len  = iov->iov_len;
		setHeader("Content-Length:" ,StrNumberToString(static_cast<int> (iov->iov_len) ) );
	}
	inline void setBodyData(const string & str)
	{
		content_ = str;
	}
	private:

		void setByInvalid(const HttpRequest &);
		void setByGet(const HttpRequest &);
		void setByPost(const HttpRequest &);
		void setByPut(const HttpRequest &);
		void setByDelete(const HttpRequest &);
		void setByOptions(const HttpRequest &);

	void setAllHeaders();
	void setDefaultContent();

	
	/*
		根据request 进行设计相应的结果
	*/
	void analysisAccept(const std::string &);
	
	//存一个文件
	struct iovec fileIov_;
//	FileBuffer fileBuffer_; //这个东西会自己走自己释放
	bool keepAlive_ = false;
	//用来存储Url
	string url_;
	string codeAndStr_;
	string version_;
	std::map<string ,string > headersAndContent_;
	std::set<string> headers_;
	//存body信息
	std::string content_ = "";
};
}

}
#endif