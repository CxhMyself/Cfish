
#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include"../net/TcpServer.h"
#include"../http/HttpRequest.h"
#include"../http/HttpResponse.h"
#include"HotUpdata.h"
//Key
#include"ValidUtil.h"
#include<string>
//基础库都是命名空间 Cmuduo
//个人无借鉴的都是命名空间 Cfish
//目标是一个框架

#define WWWPATH "/media/cxhmyself/新加卷/summer_vacation/Cfish/webServer1.0/web/Object"

//I need a key to run encode and decode

namespace Cfish{

    namespace WEB {

		typedef std::string (* FUNC) (const std::map< std::string , std::string > &,std::string&);
		
        class HttpServer {
        public:
            using CallBack = std::function<bool(void * , Socket &)>;

            //构造函数主要用于构造 TcpServer对象
            HttpServer(const std::string &name , int);

            inline const string & getName() const{
                return serverName_;
            }
            const string &getRoot() const {
                return root_;
            }
            inline int getFd() const{
                tcpServer.getFd();
            }
            //服务器的名字
            std::string serverName_;
            //一个TcpServer 中含有日志 以及线程的自动管理
            TcpServer tcpServer;

            //一个用于连接时的函数 主要用于数据的读写 用tcpServer 设置 还有关闭连接时的日志输出 不漂亮
            inline void setConnectCallBack(const CallBack &connectCallBack) {
                tcpServer.setConnectCallBack(connectCallBack , this);
            }

            inline void setMessageCallBack(const CallBack &messageCallBack) {
                tcpServer.setMessageCallBack(messageCallBack,this);
            }
			inline void setCloseCallBack(const CallBack &closeCallBack) {
				tcpServer.setCloseCallBack(closeCallBack , this);
			}
			
            inline void startRun()
            {
                tcpServer.start();
			}

            string getIp() const;
            int getPort() const ;

			inline HotUpdata<FUNC> &  getHotUpdata(){
				return hotUpdate_;
			}
			inline const Key& getKey(){
				return key_;
			}
        private:
            string root_ = WWWPATH;
				Key key_;
        private:
			//一个热更新的实例 内置有管理的模块
			HotUpdata<FUNC> hotUpdate_;
        };
    }
}

#endif
