#include"ValidUtil.h"
#include <randpool.h> 
#include <rsa.h> 
#include <hex.h> 
#include <files.h>
#include <cryptlib.h>
#include <rng.h>
#include <osrng.h>
#include <string>
#include <json/json.h>
#include <iostream>
using std::string ;
using namespace CryptoPP; 
//定义一个Json串 是JWT的头部 

namespace Cfish{
//这个文件要专注于实现这个

//Base64Encode加密
unsigned char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
string Base64Encode(const string & source)
{
	//得到存在多少的 这里应该可以优化 获取需要补多少的位
	int un = source.size() % 3;
	string result;

	for( int i = 2 ; i < source.size() ;)
	{
		//三八二十四 三个字节生成四个对应的字节
		unsigned char ch1 = static_cast<unsigned char>( source[i-2] );
		unsigned char ch2 = static_cast<unsigned char>( source[i-1] );
		unsigned char ch3 = static_cast<unsigned char>( source[i] );

		//右移两位获取前六位
		result.push_back( table[ ch1 >> 2   ]);
		//对于第二个要获取 第一个的低两位(11)和第二个的高四位
		result.push_back( table[ ((ch1 & 0x3) << 4) | (ch2 >> 4) ] );
		//对于第三个 获取第二个的低四位和第三个的高两位
		result.push_back( table[ ((ch2 & 0xf) << 2) | (ch3 >> 6) ]);
		//对于第四个 获取第三个的低六位
		result.push_back( table[ ch3 & 0x3f ] );
		i += 3;
	}
	//现在剩了一位 则 需要补齐一位
	if( un == 1 )
	{
		unsigned char ch1 = static_cast<unsigned char>( source[source.size() - 1 ] );
		unsigned char ch2 = static_cast<unsigned char>( 0 );
		result.push_back( table[ ch1 >> 2] );
		result.push_back( table[ ((ch1 & 0x3) << 4) | (ch2 >> 4) ] ) ;
		result.push_back('=');
		result.push_back('=');
	}
	else if(un == 2)
	{
		unsigned char ch1 = static_cast<unsigned char>( source[source.size() - 2 ] );
		unsigned char ch2 = static_cast<unsigned char>( source[source.size() - 1 ] );
		unsigned char ch3 = static_cast<unsigned char>( 0 );
		//右移两位获取前六位
		result.push_back( table[ ch1 >> 2] );
		result.push_back( table[ ((ch1 & 0x3) << 4) | (ch2 >> 4) ] ) ;
		result.push_back( table[ ((ch2 & 0xf) << 2) | (ch3 >> 6) ]);
		result.push_back('=');
	}
	return result;
}
//Base64Encode解密 一个解密表 用来根据字符判断值 等号传入时判断
unsigned char getValueByChar( char ch )
{
	if( ch >= 65 && ch <= 90 )
		return static_cast<unsigned char>(ch-65);
	if( ch >= 97 && ch <= 122 )
		return static_cast<unsigned char>(ch-71);
	if( ch >= 48 && ch <= 57)
		return static_cast<unsigned char>(ch+4);
	if( ch == '+' )
		return static_cast<unsigned char>(62);
	if( ch == '/' )
		return static_cast<unsigned char>(63);
	return -1;
}

string Base64Decode( string source)
{
	//失败返回空串
	if( source.size() % 4 )
		return "";
	while( source.back() == '=' )
		source.pop_back();
	int un = source.size() % 4;
	string result;
	for(int i = 3 ; i < source.size() ; )
	{
		//四个
		unsigned char ch1 = getValueByChar(source[i-3]);
		unsigned char ch2 = getValueByChar(source[i-2]);
		unsigned char ch3 = getValueByChar(source[i-1]);
		unsigned char ch4 = getValueByChar(source[i]);
		//三个待解
		char re1 = (ch1 << 2) | (ch2 >> 4);
		char re2 = (ch2 << 4) | (ch3 >> 2);
		char re3 = (ch3 << 6) | ch4;
		result.push_back(re1);
		result.push_back(re2);
		result.push_back(re3);
		i+=4;
	}
	if( un == 3 )
	{
		unsigned char ch1 = getValueByChar(source[source.size() - 3]);
		unsigned char ch2 = getValueByChar(source[source.size() - 2]);
		unsigned char ch3 = getValueByChar(source[source.size() - 1]);
		char re1 = (ch1 << 2) | (ch2 >> 4);
		char re2 = (ch2 << 4) | (ch3 >> 2);
		result.push_back(re1);
		result.push_back(re2);
	}else if( un == 2 )
	{
		unsigned char ch1 = getValueByChar(source[source.size() - 2]);
		unsigned char ch2 = getValueByChar(source[source.size() - 1]);
		char re1 = (ch1 << 2) | (ch2 >> 4);
		result.push_back(re1);
	}
	return result;
}
}


//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************
//**************************************************

//加密算法 这个用开源库吧 这个是直接调用的开源库的内容可不是我自己的  因为自己是真的不会
/*
ANONYMOUS_NAMESPACE_BEGIN
#if (CRYPTOPP_USE_AES_GENERATOR)
OFB_Mode<AES>::Encryption s_globalRNG;
#else
NonblockingRng s_globalRNG;
#endif
NAMESPACE_END


RandomNumberGenerator & GlobalRNG()
{
	return dynamic_cast<RandomNumberGenerator&>(s_globalRNG);
}

void GenerateRSAKey(unsigned int keyLength, string &privString, string &pubString, const char *seed)
{
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
	HexEncoder privStr(new StringSink(privString));
	priv.AccessMaterial().Save(privStr);
	privStr.MessageEnd();

	RSAES_OAEP_SHA_Encryptor pub(priv);
	HexEncoder pubStr(new StringSink(pubString));
	pub.AccessMaterial().Save(pubStr);
	pubStr.MessageEnd();
}

std::string RSAEncryptString(const string & pubString, const char *seed, const char *message)
{
	StringSource pubStr(pubString, true, new HexDecoder);
	RSAES_OAEP_SHA_Encryptor pub(pubStr);
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	std::string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}

std::string RSADecryptString(const string & privString, const char *ciphertext)
{
	StringSource privStr(privString, true, new HexDecoder);
	RSAES_OAEP_SHA_Decryptor priv(privStr);

	std::string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
	return result;
}
}

*/
ANONYMOUS_NAMESPACE_BEGIN
#if (CRYPTOPP_USE_AES_GENERATOR)
OFB_Mode<AES>::Encryption s_globalRNG;
#else
NonblockingRng s_globalRNG;
#endif
NAMESPACE_END


RandomNumberGenerator & GlobalRNG()
{
	return dynamic_cast<RandomNumberGenerator&>(s_globalRNG);
}

void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed)
{
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
	HexEncoder privFile(new FileSink(privFilename));
	priv.AccessMaterial().Save(privFile);
	privFile.MessageEnd();

	RSAES_OAEP_SHA_Encryptor pub(priv);
	HexEncoder pubFile(new FileSink(pubFilename));
	pub.AccessMaterial().Save(pubFile);
	pubFile.MessageEnd();
}

std::string RSAEncryptString(const char *pubFilename, const char *seed, const char *message)
{
	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Encryptor pub(pubFile);

	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	std::string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}

std::string RSADecryptString(const char *privFilename, const char *ciphertext)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Decryptor priv(privFile);

	std::string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
	return result;
}

//use segmentation analysis
namespace Cfish{
	Key::Key(){
		//获取当前时间
		time_t time_ = ::time(nullptr);
		::ctime_r(&time_,seed_);
		seed_[ strlen(seed_) - 1 ] = '\0';
		CreateKey();
	}

	void Key::CreateKey(){
		//获取得到密钥
		GenerateRSAKey( 1024, privateKey_ , publicKey_ , seed_ );
		std::cout << publicKey_ << std::endl; 
	}
	//这个是解密
	string Key::operator()(const string & content , int mode) const{
		if( mode == ENCODE )
			return encode(content);
		else if( mode == DECODE )
			return decode(content);
		else
			return "";
	}
	//加密
	string Key::encode(const string & content) const
	{
		string result;
		size_t ret = ( content.size() / segmentationSize_ );
		size_t index = 0;
		for(int i = 0 ; i <= ret ; ++i )
		{
			string middle = "";
			if( index + segmentationSize_ < content.size() )
			{
				middle = content.substr(index ,segmentationSize_ );
			}
			else
			{
				middle = content.substr(index ,content.size() );
			}
			result.append(RSAEncryptString(publicKey_ , seed_ ,middle.c_str()));
			index += segmentationSize_;
		}
		
		return result;
	}
	string Key::decode(const string & content) const
	{
		//256
		if( content.size() % 256 != 0 )
			return "";
		auto ret = content.size() / 256;
		string result = "";
		for( int i = 0 ; i < ret ; ++i )
		{
			string middle = content.substr(i*256 , (i+1)*256);
			result.append(RSADecryptString(privateKey_ , middle.c_str()));
		}
		return result;
	}
}
