#include"FileUtil.h"
#include"Logging.h" //strerror_t1
#include<fcntl.h>
#include<error.h>
#include<sys/stat.h>
#include<sys/types.h>
using namespace Cmuduo;


//先进行 AppendFile 类的实现
//追加模式打开文件自动偏移至文件尾
FileUtil::AppendFile(const string &filename)
: fp_( fopen(filename.c_str() , "ae"))/*e 表示O_CLOEXEC*/
, writtenBytes_(0)
{
	//不为nullptr则一切正常
	assert(fp_);
	::setbuffer(fp_ , buffer_ , sizeof( buffer_ ));
}

FileUtil::AppendFile::flush()
{
	//刷新这个文件指针所指向的文件的文件缓冲区
	::fflush(fp_);
}

FileUtil::AppendFile::~AppendFile()
{
	//首先进行刷新到缓冲区
	flush();
	::fclose(fp_);
}

//这之后采用队列的方式将数据发送到缓冲区 所以 线程不安全也OK
FileUtil::AppendFile::append(const char* logline , const size_t len)
{
	auto ss = write(logline , len);
	auto intermediate = len - ss;
	while( intermediate > 0 ){
		auto x = write(logline , intermediate);
		if( x == 0 )
		{
			auto eror = ferror(fp_);
			if(eror)
				fprintf(stderr , "AppendFile::append() failed : %s\n" ,strerror_t1(eror));
			break;
		}
		ss += x;
		intermediate = len - ss; //更新
	}
	writttenBytes_ += len;
}

size_t FileUtil::AppendFile::write(const char * logline , size_t len)
{
	//这个 有问题																																							@@@@
	return ::write_unlock(logline , len , 1 , fp_ );
}

//这里直接将文件打开
FileUtil::ReadSmallFile::ReadSmallFile(const std::string & filename)
:fd_(::open(filename.c_str() , O_RDONLY | O_CLOEXEC) /*设置自动关闭*/ ),
err_(0)
{
	//将缓冲区字符设置成零
	buf_[0] = '\0';
	if( fd_ < 0 )
		err_ = errno;
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
	if(fd_ > 0)
		::close(fd_);
}


/*
	ReadSmallFile 读文件工具类的封装
*/

FileUtil::ReadSmallFile::ReadSmallFile(const std::string &str)
: fd_(::open(str.c_str() , O_RDONLY | O_CLOEXEC)), err_(0)
{
	buf_[0] = '\0';
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
	if(fd_ > 0)
	::close(fd_);
}

//这个函数的功能是 获取文件的内容 及一些额外信息 申请的最大长度
template<class String>
FileUtil::ReadSmallFile::readFile(int MaxSize,String* content, int64_t *fileSize , int64_t *modifyTime, int64_t *createTime)
{
	//如果content为NULL则发生错误
	assert(content);
	struct stat st;
	content->clear();

	int err = err_;
	if( fileSize ){
		//如果成功则读取尺寸并提前定义长度
	if( ::fstat( fd_ ,&st) == 0 )
	{
		//判断是否为文件 这样子
		if( S_ISREG(st.st_size) )
		{
			if( fileSize )
				*fileSize = st.st_size;
			//设置内容的大小提前一次性申请空间
			content->reserve( std::min( MaxSize , st.st_size ) );
			
			if( modifyTime )
				*modifyTime = st.st_mtim;
			if( createTime )
				*createTime = st.st_ctim;
		}
	}
	else
	{
		err = errno;
	}
	}
	
	
	while( content->size() < (size_t)MaxSize )
	{
		//总体就是读取 如果没达到要求或者读完 就停止 si为应该读取的数量
		auto si = std::min( (size_t)MaxSize - content->size() , sizeof(buf_) );
		auto ret = ::read(fd_ , buf_ , si );
		if(ret > 0)
		{
			content->append(buf_ , ret);
		}
		else{
			//==0 也就是读完的意思
			if( ret < 0 )
				err = error;
			break;
		}
	}
	return err;
}
int 
FileUtil::ReadSmallFile::readToBuffer(int *size)
{
	int err = err_;
	//我就很纳闷 为啥用pread而不直接用read呢
	if(fd_ > 0){
		auto ret = ::pread(fd_ , buf_ , sizeof buf_ , 0);
		//这个函数会通过size返回正确读取的字节数
		if( ret >= 0 )
		{
			//不为空才能执行
			if(size)
			*size = static_cast<int>( ret );
			buf_[ret] = '\0';
		}
		else{
			err = errno;
		}
	}
	return err;
}


//最后显示的定义std::string

template int readFile( string& filename, int MaxSize,std::string* content, int64_t *fileSize , int64_t *modifyTime, int64_t *createTime);
template int readFile( string& filename, int MaxSize,std::string* content, int64_t *fileSize , int64_t *modifyTime, int64_t *createTime);






































