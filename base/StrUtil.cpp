#include"StrUtil.h"

#include<algorithm>
#include<iostream>
#include<sstream>
#include<fstream>
//有问题 为啥加姓名空间就会出错

using std::string;
using std::vector;

	//分离操作
	void StrSplit(const std::string & str,std::vector<std::string> & vec ,char ch)
	{
		if(vec.size() != 0) vec.clear();
		//使用迭代器
		auto start = str.begin();
		auto tail = start;
		while( tail != str.end() ){
			tail = std::find_if(start , str.end()
			, [ch](char c){ return ch == c; } );
			if(tail != start)
			{
				vec.push_back(string( start , tail ));
				//添加成功
				start = tail;
				start++;
			}
		}
	}
	//组合操作
	string StrMerge(const vector<string>& vec,const string & intermediate )
	{
		string str;
		
		int i;
		for( i = 0 ; i < vec.size() ; ++i )
		{
			str+= vec[i];
			if( i != vec.size() - 1 ) str += intermediate;
		}
		return str;
	}
	void StrSplitTwo( const std::string & str , std::string & str1 , std::string & str2 , char ch )
	{
		//将源分离成两个分别设置为str1 和str2
		auto tail = std::find_if(str.begin() , str.end()
			, [ch](char c){ return ch == c; } );
		str1 = string( str.begin() , tail );
		if( tail == str.end() )
			str2 = "";
		else
			str2 = string(++tail , str.end());
	}
	size_t StrToPlusNumber( const std::string & str){
		std::stringstream ss(str);
		size_t size;
		
		try{
			ss >> size;
			return size;
		}
		catch( ... )
		{
			return 0;
		}
	}
	//获取一个字符前的字符串
	string StrGetFrontStr(const string & str , char ch)
	{
		auto tail = std::find_if(str.begin() , str.end()
			, [ch](char c){ return ch == c; } );
		return string(str.begin() , tail);
	}
	//获取从后往前的一个字符后的字符串
	string StrGetAfterStr(const string & str ,char ch)
	{
		auto x = str.rfind(ch);
		return str.substr(x+1 , str.size());
	}
	
	const std::string StrNumberToString(int number)
	{
		try{
		std::ostringstream os;
		os << number;
		return os.str();
		}
		catch(...)
		{
			return string("");
		}
	}