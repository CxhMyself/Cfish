#ifndef _VALID_UTIL_H_
#define _VALID_UTIL_H_

#include<string>

namespace Cfish{
	//Base64 code
	std::string Base64Encode(const std::string & source);
	std::string Base64Decode( std::string source);
	//Key 解密的类
	//设置密钥类 用于生成密钥 这个设置成全局的一个对象 程序运行期间用
class Key{
	public:
	//加密和解密的模式
	static const int ENCODE = 0;
	static const int DECODE = 1;	
	Key();
	inline const char * getPrivateKey()const
	{
		return privateKey_;
	}
	inline const char* getPublicKey()const
	{
		return publicKey_;
	}
	
	std::string operator()(const std::string & content , int ) const ;
	
	private:
	std::string encode(const std::string & content) const ;
	std::string decode(const std::string & content) const ;
	void CreateKey();
	//保存私钥的文件名 在想如果一直保存在程序内部不就好了
	//所用种子 这个设置成时间转化成字符串后的形式
	char seed_[1024] = "seed";
	//获取的私钥
	char privateKey_[1024] = "private";
	//获取的公钥
	char publicKey_[1024] = "public";
	//segmentation  
	static const int segmentationSize_ = 80 ;
};
}

#endif
