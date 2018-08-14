#include<iostream>
#include"base/types.h"
#include"base/Exception.h"
#include<stdio.h>

//#pragram warning(disable: 4786)
#include"net/TcpServer.h"
#include"net/Socket.h"
#include<string.h>
#include"base/StrUtil.h"
#include"http/HttpServer.h"
#include"http/HttpRequest.h"
#include"http/HttpResponse.h"
#include<functional>
#include<errno.h>
#include"http/HotUpdata.h"
#include<sys/signal.h>
#include"http/FieldParsing.h"

using namespace Cfish;
using namespace std;

void funHttpResponse(WEB::HttpServer* httpServer, WEB::HttpResponse& httpResponse , WEB::HttpRequest & httpRequest ){

	httpResponse.setHeader("Server:",httpServer->getName());

	//解析URL得到 url可以和root组成一个 本地的文件位置
	if(httpResponse.getUrl().back() == '/')
	{
		//是否存在默认界面
		//这里可以直接跳到处理文件的部分并发送一个重定向 去掉/
		string newStr = string("http://") + httpServer->getIp() + ":" + StrNumberToString( httpServer->getPort() ) + httpResponse.getUrl();
		newStr.pop_back();
		httpResponse.setHeader("Location:" , newStr.c_str());
		httpResponse.setCode( "307" ,_307_STR );
		return ;
	}
	string cgi = StrGetAfterStr( httpResponse.getUrl() , '/' );
	string localAddr = httpServer->getRoot() + httpResponse.getUrl();        //指针嘛因为存在继承关系
	//获取到函数指针如果不存在则自动
	auto func = httpServer->getHotUpdata().getCgiInterface(cgi);

	if( func == nullptr )
	{
		sendFile:
		struct stat st;
		auto ret = ::stat(localAddr.c_str(), &st);
		if (ret < 0)
		{
			if( errno == 2 )//2号是文件不存在
			{
				httpResponse.setCode("404", _404_STR);
			}else{  //文件访问的权限不足
				httpResponse.setCode("403",_403_STR);
			}
		}
		else
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
			//一个回复 并重定向到目录下的index.html 注意此时的应该为 目录则到这个子目录下 这里应该用域名 不知道为啥ip出了问题
			string newStr = string("http://") + httpServer->getIp() + ":" + StrNumberToString( httpServer->getPort() ) + httpResponse.getUrl() + "/index.html";
			//这里应该进行URL的转码？
			httpResponse.setHeader("Location:" , newStr.c_str());
			httpResponse.setCode( "307" ,_307_STR );
		} else { 
			//文件的读取长度
			size_t size = st.st_size;
			//文件偏移量
			off_t off = 0; 
			httpResponse.setCode( "200" ,_200_STR );
			//到这里代表是文件没毛病了 查看是否存在断点续传 请求中的 Range
			if( httpRequest.getHeader("Range:") != "" )
			{
				//进行Range 的解析 分析函数功能 解析得到起始位置 和 终止位置 并且
				//根据Range 的解析 得到Content-Range: 字段的内容 起始位置-终止位置
				RangeParsing parsing(httpRequest.getHeader("Range:") , size );
				//如果不符合标准就发送全部和 200 OK 如果符合就发送 206
				if( !parsing.badRange())
				{
					size = parsing.getReadSize();
					off = parsing.getStart();
					httpResponse.setHeader("Content-Range:" , parsing.getResponse());
					httpResponse.setCode("206" , _206_STR);
				}
			}
			if(httpRequest.getQuery() != WEB::HttpRequest::HEAD )
			{
				int fd = ::open(localAddr.c_str() , O_RDONLY);
				//这个之后要进行释放 一个封装一个报文 所以我想在报文中释放
				struct iovec iov{nullptr , 0};
				iov.iov_base = ::mmap(nullptr , size_t(st.st_size) , PROT_READ , MAP_PRIVATE , fd , off);
				iov.iov_len = st.st_size; //其实将这个封装之后更加的漂亮
				httpResponse.setBodyData(&iov);//HttpResponse 会自动释放这个申请的空间
				::close(fd);
			}
		}
	}else{
		std::string result;
		std::string type = (*func)(httpRequest.getKeyAndvalue(),result); // 传入字符映射
		//这个结果代表是一个文件资源 则此时发送这个资源
		if(type == "*")
		{
			//此时的result是资源的本地位置
			localAddr = result;
			goto sendFile;
		}
		httpResponse.setBodyData(result);
		httpResponse.setHeader("Content-Type:" , httpResponse.getTypeBySuffix(type) );
	}
}

/*
 * 思考怎么样才能传参
 * */

//
bool closeCallBack(void * , Cfish::Socket &sock)
{
}
 
//这个是连接时的函数 //写个日志吧
bool callConnectback(void *v,Cfish::Socket &sock)
{
	//std::cout << "欢迎ip为：" << sock.getPeerIp() << " 的用户连接" << sock.getFd() << std::endl;//为什么会多次连接？？？
}

//一个用于接收处理发送报文的函数 可读事件了都 全程没有Tcp的事情
bool callMessageBack(void* v,Cfish::Socket &sock)
{
	auto httpServer = (WEB::HttpServer* )v;
	//获取报文
	WEB::HttpRequest httpRequest;
	sock >> httpRequest;
	WEB::HttpResponse httpResponse(httpRequest);
	//设置处理
	if( !httpRequest.badRequest() ){
		//it is must easy version
		string token = "";
		if( httpRequest.getHeader("Cookie:") != "" )
		{
			if( judgeCookie(httpRequest.getHeader("Cookie:") , token) )
			{
				CookieTokenParing cookieTokenParing( token );
				cookieTokenParing.valid(httpServer->getKey() , httpRequest.getKeyAndvalue() );
			}
		}
		//check account password if it is here and I can Set-Cookie
		string set_cookie = "";
		// if false I do nothing about KeyAndValue
		if( judgeSetCookie(httpRequest.getKeyAndvalue() , sock.getFd() , httpServer->getKey() , set_cookie ) )
		{
			httpResponse.setHeader("Set-Cookie:" , string("Token=") + set_cookie);
		}

		funHttpResponse(httpServer, httpResponse ,httpRequest);
	}
	//发送报文
	sock << httpResponse;
	//返回是否继续
	return httpResponse.keepAlive();
}


int main( int argc , char *argv[])
{
	//屏蔽掉SIGPIPE信号
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGPIPE);
	int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
	if (rc != 0) {
		printf("block sigpipe error\n");
	}

	WEB::HttpServer httpServer("Cfish/1.1" ,StrToPlusNumber(argv[1]));
	httpServer.setConnectCallBack(callConnectback );
	httpServer.setMessageCallBack(callMessageBack );
	httpServer.setCloseCallBack(closeCallBack);
	httpServer.startRun();

	return 0;
}
