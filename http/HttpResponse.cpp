#include"HttpResponse.h"
#include"../base/StrUtil.h"
#include "HttpStatueCode.h"
#include<iostream>
#include<string.h>
namespace Cfish{
	
namespace WEB{
	const string HttpResponse::NONESTRING = "";
void HttpResponse::setAllHeaders()
{
	//之后翻阅 P85     吧                                                        77 7                                 888888888888888888888888888
	//通用首部字段
	headers_.insert("Cache-Control:");
	headers_.insert("Connection:");
	headers_.insert("Date:");
	headers_.insert("Transfer-Encoding:");	//默认当然不分块了 															*******************之后考虑实现
	headers_.insert("Upgrade:");			
	headers_.insert("Via:");
	headers_.insert("Warning:");
	//响应首部字段 用于补充响应 服务器信息 

	/*
		bytes 	正常被处理
		none	不能处理的范围
	*/
	headers_.insert("Accept-Ranges:");
	headers_.insert("Age:");
	
	/*
		这个得在服务器上实现把资源统一设置一个标识 没时间了 暂时没想法也不实现
	*/
	headers_.insert("ETag:");
	/*
		指定一个新的URL 客户端会重定向到那里去看
	*/
	headers_.insert("Location:");
	
	headers_.insert("proxy-Authenticate:"); //代理服务器相关 不考虑
	/*
		秒数  表示 提示客户端过多久之后再来访问 Emmmm 一般用不上吧 除非自己设置
	*/
	headers_.insert("Retry-After:");
	/*
		就服务器的信息  Emmmm没了
	*/
	headers_.insert("Server:");
	/*
		需要有默认设置 如果有代理服务器呢 
		value是某个字段 类似于设置条件 默认设置为 Accept-Encoding吧
	*/

	headers_.insert("Vary:");
	headers_.insert("WWW-Authenticate:");		//用于认证

	//总结 需要实现 Accept-Ranges: Server 默认设置 Vary

	//实体首部字段 允许的方法
	headers_.insert("Allow:");
	headers_.insert("Content-Encoding:");	//压缩方式
	headers_.insert("Content-Language:");	//内容的语言
	headers_.insert("Content-Length:");		//资源的大小 这很重要
	headers_.insert("Content-Location:");	//Emmmm 毛用？？
	headers_.insert("Content-MD5:");		//这个就跟主要是用来判断数据的内容正确性的Emmm 可以实现不过等未来 吧  哈哈 原来时神的MD5是这个
	headers_.insert("Content-Range:");		//一部分内容 放在这吧用来给提示
	headers_.insert("Content-Type:");		//实体的类型
	headers_.insert("Expires:");			//这个靠用户自己设置
	headers_.insert("Last-Modified:");		//资源的最后修改时间																					如果目标是资源的话则设置时间***
	
	//一些莫名奇妙的字段
	headers_.insert("Set-Cookie:");			//用户自己设置					Emmmm 也先不实现凉了呀兄弟
	/*
		总结 
	*/
}

void HttpResponse::setDefaultContent()
{
	setHeader( "Vary:" , "Accept-Encoding");
	setHeader( "Cache-Control:" , "private, max-age=0, no-cache");
	//获取本地时间
	auto times = ::time(nullptr);
	char buffer[128];
	::ctime_r( &times , buffer );
	buffer[strlen(buffer)-1] = '\0';

	setHeader( "Date:" , buffer);
}


/*设置默认的命令时产生的信息*/
void HttpResponse::setByInvalid(const HttpRequest &hr)
{
	version_ = hr.getVersion();
	codeAndStr_ = string("400") + _400_STR;
}
/*
	get 和 post 并没有啥服务器方面的区别 心累
	一个的数据存在于体
	一个存在于 url 所以区别在于数据的获取
	按逻辑来讲 这里应该再处理键值对或者请求的内容
*/

//get 还是 post 在请求中决定吧 
void HttpResponse::setByGet(const HttpRequest &hr)
{
	//因为两者就内容位置不同而已所以 在请求中已经处理了
	setByPost(hr);
}
void HttpResponse::setByPost(const HttpRequest &hr)
{
	//头设置完了
	setDefaultContent();
	//身体交给服务器运行吧
}

void HttpResponse::setByPut(const HttpRequest &hr)
{
	
}
void HttpResponse::setByDelete(const HttpRequest &hr)
{
	
}
//默认只产生本服务器所支持的方法
void HttpResponse::setByOptions(const HttpRequest &hr)
{
	setHeader("Allow:" , "GET,POST,HEAD,OPTIONS,PUT,DELETE");
}
Socket& operator<<(Socket & sock,const HttpResponse &hr)
{
	hr.sendToNet(sock);
}


void HttpResponse::sendToNet(Socket& sock) const
{
	//这个有两个部分 分别为 报文头和内容
	//首先是行
	std::string sizeBuffer;
	
	sizeBuffer.append(version_ + " " + codeAndStr_ + "\r\n");

	if(keepAlive_)
	{
		sizeBuffer.append("Connection: keep-alive\r\n");
	}
	//添加字段
	for(auto x :headersAndContent_ ){
		sizeBuffer.append(x.first + " " + x.second + "\r\n");
	}
	sizeBuffer.append("\r\n");
	if( content_ == "" ){
		sock.sendToNetWithMessage(sizeBuffer , &fileIov_); //发送了
	}
	else{
		sizeBuffer.append(content_);
		sock.sendStr(sizeBuffer);
	}
}

HttpResponse::HttpResponse(const HttpRequest& hr) : codeAndStr_(string("200 ") + _200_STR)/* fileBuffer_()*/ {
//	//清空内部的程序块
	fileIov_.iov_base = nullptr;
	fileIov_.iov_len = 0;
	setAllHeaders();
    url_ = hr.getUrl();
	analysisAccept(url_);
    //是否有keep-alive
    if ( hr.getHeader("Connection:") != "keep-alive" ){
        keepAlive_ = false;
	}
    else
        keepAlive_ = true;
  	//std::cout << "query = " << hr.getQuery() << std::endl;
	//正确的话才会进行进行下来的默认配置之类的
	if( !hr.badRequest() ){
		auto x = hr.getQuery();
		switch( x )
		{
		case	hr.GET:
			setByGet(hr);break;
		case	hr.POST:
			setByPost(hr); break;
		case	hr.PUT:
			setByPut(hr); break;
		case	hr.DELETE:
			setByDelete(hr);break;
		case	hr.OPTIONS:
			setByOptions(hr); break;
		};
		version_ = hr.getVersion();
        //连接要关闭
	}
	else{
		keepAlive_ = false;
		version_ = hr.getVersion();
		codeAndStr_ = string("400 ") + _400_STR;
	}
}

//这里分析可获取的类型和Url进行比对如果两者符合规则则进行Content-Type 的设置 否则则设置返回错误并自动设置code_ 默认客户端支持所有的文件格式
void HttpResponse::analysisAccept(const string &url)
{
	string s = StrGetAfterStr(url , '.');
	//无后缀默认为CGI程序
	if(s != "")
		setHeader( "Content-Type:", getTypeBySuffix(s) );
}

//全根据后缀判断 这是自动的当然后面也能由程序员进行修改
string HttpResponse::getTypeBySuffix(const string &str)
{
	if( str == "html" ){
		return string("text/")+str + ";charset=utf-8" ;																				//*******这里之后要显式的处理字符集 默认为utf-8
	}
	if(str == "avi" || str=="mp4")
		return string("video/")+str;
	if( str == "css")
		return string("text/css");
	if( str == "jpeg" || str == "gif" || str == "png" || str=="jpg" )
		return string("image/") + str;
	if( str == "js")
		return string("application/x-javascript");																					//*****视频文件暂时不考虑
	if( str == "json")
		return string("application/json");
	return string("text/html;charset=utf-8");//默认格式？？？
}


}
}
