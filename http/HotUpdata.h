#ifndef _HOT_UPDATA_H_
#define _HOT_UPDATA_H_


#include<string>
#include<cstdlib>
#include<dlfcn.h>
#include"../base/StrUtil.h"
#include<iostream>



namespace Cfish
{
	
template<class FUNC>
struct CgiInfo{
	FUNC func; // 一个函数指针
	time_t m_time; //上一次的修改时间 如果两次修改时间不一致 则代表需要重新编译
	void *hander;
};

template<class FUNC>
class HotUpdata{
	public:
	//传入一个扫描的地址 绝对路径 以及函数的类型
	explicit HotUpdata(const std::string & address , const char * form) : address_(address) , funcForm_(form){
		if( address_.back() != '/' )
			address_.push_back('/');
		
	}
	//进行操作
	FUNC getCgiInterface(const std::string& url);
	private:

	string getFuncName(const std::string &);
	//获取函数指针
	std::string address_;
	//函数格式
	std::string funcForm_;
	//cgi和 函数之间的映射通过CGI找到函数
	std::map<std::string , CgiInfo<FUNC> > function_;
};

template<class FUNC >
string HotUpdata<FUNC>::getFuncName(const  std::string &str)
{
	//首先将两者分离 以@为分隔符
	std::string one , two;
	StrSplitTwo(funcForm_ , one , two ,'@');
	//组装
	return one+StrNumberToString(str.size())+str+two;
}

template<class FUNC >
FUNC HotUpdata<FUNC>::getCgiInterface(const std::string &cgi)
{
	if(cgi == "")
		return nullptr;
	
	auto x = function_.find(cgi);
	
	/*for(auto ret : function_)
	{
		std::cout << "dsad" <<  ret.first << std::endl;
	}*/
	
	//std::cout << function_.size() << std::endl;
	std::string cppName = cgi+".cpp";
	std::string entireName = address_;

	struct stat st;
	
	if( ::stat((entireName+cppName).c_str() , &st) < 0 )
	{
		//文件不存在或者权限不足 删除x 然后返回就好了
		if( x != function_.end() ){
			dlclose( (x->second).hander );
			function_.erase(x); 
		}
		return nullptr;
	}
	std::string libName = "lib";
	libName = libName + cgi + ".so";
	std::string query = "g++ ";
	query += entireName;
	query += cppName;
	//编译时便发现错误
	query += " -Wall -Werror -shared -fPIC -std=c++11 -o ";
	query.append(address_ + libName);
	//添加重定向 使得不输出在终端 
	query.append(string (" 2> /dev/null")); 

	if( x == function_.end() )
	{
		auto status = ::system(query.c_str());
		if( status != 0 )
			return nullptr;
		auto handle = dlopen( (address_ + libName).c_str() ,  RTLD_LAZY );
		if( !handle )
			//这里可以用日志未来
			fprintf(stderr , "%s\n" , dlerror());
		//清除错误
		dlerror();
		//进行函数的绑定
		FUNC func;
		*(void **)(&func) = dlsym(handle ,  getFuncName(cgi).c_str());
		if( dlerror() != nullptr )
			return nullptr;
		//讲这次的更新时间保存
		//std::cout <<  st.st_mtime << std::endl;
		function_.insert( std::make_pair(cgi ,CgiInfo<FUNC>{ func, st.st_mtime , handle} ) );
		//将动态链接库删除
		::remove( (address_ + libName).c_str());
		return func;
	}
	else
	{
		if( st.st_mtime != (x->second).m_time )
		{
			dlclose( function_[cgi].hander);
			//修改过了不就得重新编译
			auto status = ::system(query.c_str());
			//无效
			//std::cout << query << std::endl;
			if( status != 0 )
				return nullptr;
			auto handle = dlopen( (address_ + libName).c_str() ,  RTLD_LAZY );
			if( !handle )
				fprintf(stderr , "%s\n" , dlerror());
			dlerror();
			FUNC func;
			*(void **)(&func) = dlsym(handle ,  getFuncName(cgi).c_str());
			if( dlerror() != nullptr )
				return nullptr;
			function_[cgi] = CgiInfo<FUNC>{ func, st.st_mtime , handle};
			//::remove( (address_ + libName).c_str());
			return func;
		}
		else
			return (x->second).func;
	}
}
}







#endif