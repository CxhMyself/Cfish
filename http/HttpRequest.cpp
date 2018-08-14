
#include"../base/StrUtil.h"
#include<string>
#include<sstream>
#include"HttpRequest.h"
#include<iostream>
#include<sys/socket.h>
#include"CodeChange.h"
using std::stringstream;

namespace Cfish{

namespace WEB{

    const string HttpRequest::NONESTRING = "";

bool operator >> (Socket & sock, HttpRequest& request)
{
	return request.getEntireMessage(sock);
}
//读取一个完整的报文 对于GET和POST GET方法的话 则将URL存入内容 如果是post
bool HttpRequest::getEntireMessage(Socket &sock)
{
	std::string line;
	sock.readLineData(line);
	if( line == "" )
		return false;
	
	if( !getAndJudgeQueryUrlVersion(line) ){
		//设置状态码
		badRequest_ = true;
		//clearMessage(sock);
		return false;
	}
	else{
		//就不考虑字段的错误了 考虑的话太细了
		while(true){
			sock.readLineData(line);
			if( line == "" )
				break;
			//解析并加入每一种字段
			analysisLineContent(line);
		}
		if( getQuery() == POST ){
			size_t size_ = StrToPlusNumber( getHeader("Content-Length:") );
			string key_value;
			sock.readSizeData(size_ ,key_value);
			vector<string> vec;
			StrSplit(key_value , vec ,'&' );
			for( auto x : vec )
			{
				string str1;
				string str2;
				StrSplitTwo(x , str1 , str2 , '=' );
				keyAndvalue_.insert(std::make_pair( str1 , str2 ));
				//std::cout << str1 << " and " << keyAndvalue_[str1] << std::endl;
			}
		}
		return true;
	}
}

void HttpRequest::setAllHeaders()
{
	//所有的历史遗留字段均抛弃
	//通用首部字段
	/*
		控制缓存 一般的指令有
		no-cache 无参数 表示必须重新向服务器请求资源 对于代理服务器来说
		no-store 无参数 不缓存或者请求任何内容
		Emmm 看了下面好多指令这个主要跟代理服务器有关系  服务器不处理 而在发送报文时 应当注意
	*/
	headers_.insert("Cache-Control:");
	/*
		两个行为 1. 让代理服务器删除指定字段
		2.close Keep-Alive 默认是连接的 如果需要关闭 则明确使用close
	*/
	headers_.insert("Connection:");
	/*
		报文创建的日期 总体来讲我决定使用ctime 获取的 这样子Emmmm 简单点（一个笑哭的表情）Tue Jul 03 04:40:59 2012 响应的时候用
	*/
	headers_.insert("Date:");
	/*
		报文分块发送的时候 记录报文主体之后记录了哪些字段 ？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
		Transfer-Encoding: chunked 这个有啥作用？？？不实现了
		在读取解析的时候用
	*/
	headers_.insert("Trailer:");
	/*
		报文传输时采用的编码方式 HTTP/1.1中仅对值为chunked 也就是分块有用 注意分块传输过程中 没有Content-Size 但是 每一块都有一个开始的数字表示传输的字节数 由请求头发送
		服务器处理并分块处理 此时必须有Transfer-Encoding: chunked 这个在服务器响应的时候分
	*/
	headers_.insert("Transfer-Encoding:");
	/*
		客户端对邻接服务器说 能让爸爸用Upgrade:指定的协议么 临界服务器 说不能滚 并且在Connect:中设置Upgrade: Upgrade:中设置允许的类型
		实现么 ？？？  当然不 毕竟 我。。。就支持HTTP/1.1 难受
	*/
	headers_.insert("Upgrade:");																															//******
	/*
		这个留着吧
		万一之后想用这个做代理服务器呢 对吧 说不准 但是现在不实现																							//******
	*/
	headers_.insert("Via:");
	/*
		我警告你哟  锤子用哟 代理服务器相关 暂时不考虑
	*/
	headers_.insert("Warning:");

	//总结 需要服务器处理或者说真实实现的  Cache-Control    Connection:  Date  Transfer-Encoding: 也就只有四个

	//请求首部字段
	/*
		这个字段是客户端发往服务器的对请求的附加信息客户端信息 和响应的内容优先级的控制
		请求的客户端信息不做处理用来当日志输出 （哈哈哈可惜我的日志系统还没写完噗。。。）
	*/
	
	/*
		请求的文本格式优先级控制
		这个鸡儿居然控制优先级那么麻烦 我还是只用它来做可接受的类型 如果没有的话 设置为* 表示可接受所有的类型 但是有啥用呢 人家要资源你给不给？？
		不给的话返回什么？？Emmmm 返回一个403？？就这样吧
	*/
	headers_.insert("Accept:");
	/*
		这东西还得有内容协商协商也就是说得在内容中查找所用字符集？？我拒绝 所以 我只支持 utf-8
	*/
	headers_.insert("Accept-Charset:");				//其实并没有考虑 和Accept一样 考虑的话太伤了
	/*
		这个呢 就是说指定客户端所接收的服务器发来的内容编码 这个在压缩发送的时候考虑现在先不考虑 保留但是不实现
	*/
	headers_.insert("Accept-Encoding:");
	/*
		这东西 现在一般都中英都可以的 所以这个也默认不做处理
	*/
	headers_.insert("Accept-Language:");		//其实不用处理
	/*
		需要认证信息 这个注意返回是401 可以实现 但是后期实现 因为不知道服务器端的认证信息该如何发放
	*/
	headers_.insert("Authorization:");																							//关于认证还没有学到
	/*
		这东西 ？？？？啥啥啥 期望的扩展？？？
	*/
	headers_.insert("Expect:");																													//***
	headers_.insert("From:"); 			//不管这个
	headers_.insert("Host:");			//主机名 因为虚拟主机的存在 必须指定主机名 因为要根据主机名来选择ip （单台主机分配多个域名怎么做呢）  EMMM不会 就单纯接受吧
	
	/*
		条件请求 这四个均不实现 想实现也得先看ETag
	*/
	headers_.insert("If-Match:");
	headers_.insert("If-Modified-Since:");
	headers_.insert("If-Range:");
	headers_.insert("If-Unmodified-Since:");
	
	/*
		跟代理服务器的中转次数相关 不考虑
	*/
	headers_.insert("Max-Forwards:");
	headers_.insert("Proxy-Authorization:");
	/*
		获取的资源的范围
	*/
	headers_.insert("Range:");																											//****赶工呀兄弟 不做了
	/*
		此中含有原始URL
	*/
	headers_.insert("Referer:");
	/*
		编码方式 P117  还可以指定分块的编码
	*/
	headers_.insert("TE:");
	/*
		显示用户代理等信息
	*/
	headers_.insert("User-Agent:");

	//总结 服务器需要处理的字段 Range   待定  Authorization   Host处理依据
	//默认先设置为* Accept-Language Accept-Encoding Accept-Charset Accept TE: 传输编码 这些都有默认值 如果不为默认值再处理

	//实体首部字段 允许的方法  这个Emm 一
	headers_.insert("Content-Encoding:");
	headers_.insert("Content-Language:");
	headers_.insert("Content-Length:");
	headers_.insert("Content-Location:");
	headers_.insert("Content-Range:");
	headers_.insert("Content-Type:");
	//一些乱七八糟的字段
	headers_.insert("Cookie:");
}



//private function



//分析和加入
void HttpRequest::analysisLineContent(const string & line)
{
	string key , value;
	StrSplitTwo( line , key , value ,' ');
	setHeader(key ,value);
}

//读取脏数据 读脏数据的时候不进行检测 只进行Content-Length 的判断 如果有 则之后读出相应长度的脏数据
void HttpRequest::clearMessage(Socket& sock){
	string line;
	size_t size = 0;
	while(true){
		sock.readLineData(line);
		size = judgeContentLength(line);
		if( line == "" )
			break;
	}
	//如果存在size字段则 读出size长度的脏数据 badRequest
	if(size)
		sock.readRubbishData(size);
}

size_t HttpRequest::judgeContentLength(const string & line)
{
	vector<string> vec;
	StrSplit(line , vec , ' ');
	if( vec[0] == "Content-Length:" )
	{
		//这里应该对数字进行检测 但是真的不想那样做了 实在是太费事了
		size_t result;
		std::istringstream ss(vec[1]);
		ss >> result;
		return result;
	}
	return 0;
}

bool HttpRequest::getAndJudgeQueryUrlVersion(const string& str)
{

	vector<string> vec;
	StrSplit(str , vec , ' ');

	if( vec.size() != 3 )
		return false;
	//对第一个进行判断 如果命令不符合要求要返回失败的
	if( setQuery( vec[0] ) == INVALID )
		return false;
	if( vec[1][0] != '/' )
		if( !(query_ == OPTIONS && vec[1] == "*" ) )
			return false;
	url_ = vec[1];
	//url_保持原样
	analysisUrl(url_);

	if( !( vec[2] == "HTTP/1.1" || vec[2] == "HTTP/1.0" ) )
		return false;
		
	version_ = vec[2];
	return true;
}

//这个放到 response里面
void HttpRequest::analysisUrl(const string & _url){
	//这里进行分析并且且分析出的结果放入key-value 单独的设置为空 和 url_
	//第一步将其由'?'进行分隔
	//第一步解码
	unsigned char ur[2048] = "";

	strcpy(reinterpret_cast<char* >(ur) , _url.c_str());

	unsigned char buffer[2048] = ""; 

	urldecode( ur ,buffer);
	const char* buffer_ = reinterpret_cast<const char*>(buffer);
	string url(buffer_);

	if( query_ == GET )
	{
		string key_value;
		string u;
		StrSplitTwo(url ,u , key_value , '?');
		url_ = u;

		vector<string> vec;
		StrSplit(key_value , vec ,'&' );
	
		//std::cout << "这一行的内容是：：：" << _url << std::endl;
		for( auto x : vec )
		{
			string str1;
			string str2;
			StrSplitTwo(x , str1 , str2 , '=' );
			//std::cout << str1  <<  " and " << str2 << std::endl; 
			keyAndvalue_.insert(std::make_pair( str1 , str2 ));
		}
	}
}


int HttpRequest::setQuery(const string & str){
	if( str == "GET" )
	{
		query_ = GET;
		return GET;
	}else if( str == "POST" )
	{
		query_ = POST;
		return POST;
	}else if( str == "HEAD" )
	{
		query_ = HEAD;
		return HEAD;
	}else if( str == "DELETE" )
	{
		query_ = DELETE;
		return DELETE;
	}else if( str == "PUT" )
	{
		query_ = PUT;
		return PUT;
	}else if( str == "OPTIONS"){
		query_ = OPTIONS;
		return OPTIONS;
	}
	else{
		query_ = INVALID;
		return INVALID;
	}
}
}
}
