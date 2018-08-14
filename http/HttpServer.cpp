#include"HttpServer.h"
#include"HttpResponse.h"
#include"HttpRequest.h"
#include"../base/StrUtil.h"
#include<string>
#include<iostream>
#include"../net/SocketOps.h"
#include<sys/types.h>
#include<netinet/in.h>
#include<iostream>
using std::string;


#define CGIPATH "/media/cxhmyself/新加卷/summer_vacation/Cfish/webServer1.0/cgi"

namespace Cfish{
    namespace WEB {
        //这个的作用主要构建服务器
        HttpServer::HttpServer(const string &name, int port) : serverName_(name), tcpServer(port) //,key_(),
		, hotUpdate_(CGIPATH,"_Z@RKSt3mapINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEES5_St4lessIS5_ESaISt4pairIKS5_S5_EEERS5_"){
        }
        string HttpServer::getIp() const {
            int fd = tcpServer.getFd();
            struct sockaddr_in address;
            socklen_t len = sizeof(address);
            SocketOps::getPeerInfo(fd,&address , &len);
            char * ip= ::inet_ntoa(address.sin_addr);
            return string(ip);
        }
        int HttpServer::getPort() const{
            return tcpServer.getPort();
        }
    }
}
