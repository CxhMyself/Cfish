#include"FieldParsing.h"
#include"../base/StrUtil.h"
#include"ValidUtil.h"
#include<json/json.h>
#include <mysql/mysql.h>
#include<string.h>
#include"../net/SocketOps.h"
#include<time.h>


using namespace std;
using namespace Cfish;
#define DATABASE 			"webServerDatabase"
#define USE					"root"
#define PASSWORD			"woshicxh666"
#define VALIDTABLE		"User"

//讲道理居然还能多次分段 Emmm 这样的话 回头再考虑吧
RangeParsing::RangeParsing(const std::string &str , size_t allSize)
{
	string start;
	string end;
	allSize_ = StrNumberToString( static_cast<int>(allSize) );
	StrSplitTwo(str , start , end , '-');
	if( start == "" )
	{
		end_ = static_cast<off_t>(StrToPlusNumber(end) );
		//最后那么多的字节
		start_ = allSize - end_;
		end_ = allSize;
	}
	else
	if( end == "" )
	{
		start_ = static_cast<off_t>(StrToPlusNumber(start) );
		end_ = allSize;
	}
	else{
		start_ = static_cast<off_t>(StrToPlusNumber(start) );
		end_ = static_cast<off_t>(StrToPlusNumber(end) );
	}
	if( end_ > allSize || start_ > end_ || start_ < 0 )
		badRange_ = true;
	else
		badRange_ = false;
}

//无法处理范围内请求
bool RangeParsing::badRange() const
{
	return badRange_;
}

size_t RangeParsing::getReadSize() const
{
	return static_cast<size_t>(end_ - start_ + 1);
}

//获取正缺时应当回复的回复头
std::string RangeParsing::getResponse() const
{
	string start = StrNumberToString(static_cast<int>(start_));
	string end = StrNumberToString(static_cast<int>(end_));
	return string("bytes ") + start + "-" + end + "/" + allSize_;
}
off_t RangeParsing::getStart() const 
{
	return start_;
}
off_t RangeParsing::getEnd() const 
{
	return end_;
}





//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************


namespace Cfish{
bool judgeCookie(const string & cookie , string & tk)
{
	tk.clear();
	vector<string> vec; //根据空格做分离
	StrSplit(cookie , vec , ' ');
	string token = "";
	for(auto x : vec)
	{ 
		//进行前五位的对比 如果为真 
		if( !strncmp(x.c_str() , "Token" , 5) )
		{
			token = x;
			break;
		}
	}
	if( token == "" )
	{
		return false;
	}
	else{
		tk = token.substr(6 , token.size());
		if(tk.back() == ';' )
			tk.pop_back();
		return true;
	}
}
}

//下来是Cookie的解析部分 防止数据不按规则 这里只查Token对应的元素 如果没有 则不允许访问那些 发现其实不用管理 传入可以靠getHeader获得
CookieTokenParing::CookieTokenParing( const string &token )
{
	//首先进行分组 按.
	vector<string > vec;
	StrSplit(token , vec , '.');
	head_ = vec[0];
	payload_ = vec[1];
	signature_ = vec[2];
	/*std::cout << head_ << std::endl;
	std::cout << payload_ << std::endl;
	std::cout << signature_ << std::endl;*/
}

/*
	这里的head 是固定的
	所以只需要解析payload
	采用的JSON格式是
	{
		"AccountJWT": ,			//用户名
		"IpJWT",					//登录时间
		"DateJWT",					//日期
	}
*/

bool CookieTokenParing::valid(const Key &key , std::map<std::string ,std::string> & mapping)
{
	//这里根据Key进行开锁 哈哈哈 如果不成功则返回 false
	if( key(head_ + '.' + payload_ , Key::DECODE) == signature_)
	{
		//解码体
		string payloadJsonStr = Base64Decode(payload_);

		//将数据解析出来 并加入到目标的mapping 没有数组直接映射就好
		Json::Value root;
		Json::Reader jsonRead;
		//这里因为是我传过去的是一定有效的 User must get this
		if( !jsonRead.parse( payloadJsonStr, root) )
			return false;
		mapping[ "AccountJWT" ] =  root["AccountJWT"].asString();
		mapping[ "IpJWT" ] =  root["IpJWT"].asString();
		mapping[ "DateJWT" ] =  root["DateJWT"].asString();
		//std::cout << mapping["AccountJWT"] << std::endl; 
		return true;
	}
	//cout << "qi ma cuo wu shi youde " << std::endl;
	return false;
}


//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************

SetCookieJWT::SetCookieJWT(){ initHead(); }

void SetCookieJWT::setIp(const string &ip)
{
	payload_["IpJWT"] = ip;
}
//this is login time
void SetCookieJWT::setDate(const string &date)
{
	payload_["DateJWT"] = date;
}
void SetCookieJWT::setAccount(const string &account)
{
	payload_["AccountJWT"] = account;
}

/*
	"typ":"JWT",
    "alg":"HS256"
*/
void SetCookieJWT::initHead()
{
	head_["typ"] = "JWT";
	head_["alg"] = "HS256";
}

string SetCookieJWT::toAfterEncodeStr( const Key & key )const
{
	//这里需要的是加密 密钥已经有了
	//第一步将head_转化成字符串并使用base64加密
	Json::FastWriter jsonWrite;
	string head = jsonWrite.write(head_);
	head.pop_back();
	string payload = jsonWrite.write(payload_);
	payload.pop_back();
	//将两个放在一起进行加密 并将加密的结果放在 第三部分就是签名
	head = Base64Encode(head);
	payload = Base64Encode(payload);
	string signature = key(head+"."+payload , Key::ENCODE);
	//将三个合起来形成Token
	return head+"."+payload+"."+signature;
}

namespace Cfish{
bool judgeSetCookie(const std::map<std::string , std::string > &map , int fd , const Key &key , string & set_cookie )
{
	set_cookie.clear();
	auto x = map.find("account");
	auto y = map.find("password");
	//std::cout << "I am coming 222" << std::endl;
	if( x != map.end() && y != map.end())
	{
			string account = x->second;
			string password = x->second;
			//check SQL and judge vaild 
			MYSQL *mysql = mysql_init(nullptr);
			if(!mysql_real_connect(mysql ,"localhost" ,USE , PASSWORD , DATABASE , 0 , nullptr , 0 ))
			{
				std::cout << mysql_error(mysql) << std::endl;
				//exit
				exit(-1); 
			}
			string query = "select * from ";
			query.append(VALIDTABLE);
			query.append(";");
			mysql_query(mysql , query.c_str());
			MYSQL_RES * res= mysql_store_result(mysql);
			MYSQL_ROW row;
			if( mysql_num_rows(res) )
			{
				while((row = mysql_fetch_row(res)))
				{
					//data table only have account password identify
					if( account == row[0] && password == row[1] )
					{
						std::cout << row[0] << std::endl;

						//here use time stamp is batter but I want to use time() because it is simple 
						//set a Set-Cookie
						SetCookieJWT jwt;
						auto _time = ::time(nullptr);
						char buffer[1024] = "\0";
						ctime_r(&_time , buffer);
						buffer[strlen(buffer)-1] = '\0';
						jwt.setDate(string( buffer) );

						jwt.setIp( SocketOps::getSocketIp(fd) );

						jwt.setAccount(account);

						set_cookie = jwt.toAfterEncodeStr(key);
						return true;
					}
				}
			}
		
	}
	return false;
}
}




