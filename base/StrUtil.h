#ifndef _STRUTIL_H_
#define _STRUTIL_H_

#include<string>
#include<vector>

	//为啥加姓名空间就会出错
	void StrSplit(const std::string & str,std::vector<std::string> & vec , char ch);
	std::string StrMerge(const std::vector<std::string>& vec,const std::string & intermediate );
	//只分析出 第一个由字母ch 隔开的内容
	void StrSplitTwo( const std::string & source , std::string & str1 , std::string & str2 , char ch );
	size_t StrToPlusNumber( const std::string & str);
	std::string StrGetFrontStr(const std::string & str , char ch);
	std::string StrGetAfterStr(const std::string & str ,char ch);
	const std::string StrNumberToString(int number);
	
#endif