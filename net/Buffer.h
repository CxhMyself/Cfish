#ifndef _BUFFER_H_
#define _BUFFER_H_

#include<sys/uio.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string>

namespace Cfish{

using std::string;


//Buffer 定义一些接口吧 思考下身为一个缓冲区的自觉性
class Buffer{
	public:
	virtual ~Buffer() = default;
	//通过这个接口获取内容 这两个下来选择一个就行了
	//virtual const string& getContentToString() const {return "";}
	virtual const char * getContentToCstring() const {return nullptr ;}
	//通过这个接口获取内容长度
	virtual size_t getSize() const = 0;
};
//Emmmm 报文都是字符串的形式所以不打紧 不会存在\0的字符的
class SizeBuffer : public Buffer{
	public:
	virtual ~SizeBuffer() = default;
	SizeBuffer() = default;
	//设置读取的长度Emmm 这个就是通过string 的reserve来重置string的大小
	inline void setSize( size_t size ){
		clear();
		data_.reserve(size);
	}
	inline void clear(){
		data_.clear();
	}
	
	virtual inline size_t getSize() const override{
		return data_.capacity();
	}
	size_t getRealSize() const
	{
		return data_.size();
	}
	//cpp中定义
	//virtual const string& getContentToString();
	//virtual const char * getContentToCstring();
	virtual const string& getContentToString() const {
		return data_;
	}
	virtual const char * getContentToCstring() const override{
		return data_.c_str();
	}
	
	//两种添加
	virtual void append(const char * buf);
	virtual void append(const char * begin ,const char *end);
	virtual void append(const string & str){
		data_.append(str);
	}
	//添加数据
	private:
	//数据用append添加就好了
	std::string data_;
};

//因为这个往往都需要使用iovec程序块和报文整体发送
//这里直接负责了本地的读取
class FileBuffer : public Buffer{
	public:
	
	//思考下会出现什么情况 比如权限不足(文件只读之类的) 这不是普通文件
	static const int FB_NOPOWER = 0;
	static const int FB_NOTFILE = 1;
	static const int FB_OK = 2;
	//构造的思路就是通过文件描述符或者通过路径来构造文件 构造的时候自动mmap去映射 析构做结尾处理
	//FileBuffer(int fd);
	FileBuffer();

	//FileBuffer(const string& filename);																		这个方案被我排除
	//此函数返回errno值
	int setFileByName(const string & filename);
	virtual ~FileBuffer();
	//这个用来装文件所以方法选用mmap ummap
	virtual const char * getContentToCstring() const override {return static_cast<char *>( fileContent_->iov_base );}
	//通过这个接口获取内容长度
	virtual size_t getSize() const{
		if( fileContent_ )
			return fileContent_->iov_len;
		return 0;
	}

	private:
	struct iovec * fileContent_;
};

/*
//我觉得这样就挺好的
class LineBuffer : public SizeBuffer{
	public:
	
	LineBuffer() : SizeBuffer() {}
	virtual ~LineBuffer() = default;

	//这里是从爷爷那里得到的
	virtual inline size_t getSize() const override{
		return SizeBuffer::getSize();
	}
	virtual inline const string& getContentToString() const override{
		return SizeBuffer::getConntentToString();
	}
	virtual const char * getContentToCstring() const  override{
		return SizeBuffer::getConntentToCstring();
	}

	//从爸爸那里得到的
	virtual inline void append(const char * buf) override {
		SizeBuffer::append(buf);
	}
	virtual inline void append(const char * begin , const char * end)override {
		SizeBuffer::append(begin , end);
	}
	//自我的方法 去除\n\r
	inline void removeCrlf(){
		while( data_.back() == '\n' || data_.back() == '\r' )
			data_.pop_back();
	}
	inline void 
};
*/


}

#endif