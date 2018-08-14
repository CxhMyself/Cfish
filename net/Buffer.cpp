#include"Buffer.h"
#include<unistd.h>
#include<string>
#include<string.h>
#include<iostream>
#include<sys/mman.h>
using std::string;

namespace Cfish{
	namespace Util{
		//对文件加锁 这样的话 保证文件的线程安全
		int lock_file(int fd , int type) {
			int ret;
			struct flock the_lock;
			memset(&the_lock, 0, sizeof(the_lock) );
			the_lock.l_type = type;
			the_lock.l_whence = SEEK_SET;
			the_lock.l_start = 0;
			the_lock.l_len = 0;
			//防止被信号中断
			do {
				ret = fcntl(fd, F_SETLKW, &the_lock);
			} while (ret < 0 && errno == EINTR);
			if ( ret == 0 )
				return 0;
			return -1;
		}
		int lock_read_file(int fd){
			return lock_file(fd ,F_RDLCK );
		}
		int unlock_file(int fd){
			return lock_file(fd , F_UNLCK);
		}
	}
/*	const string& SizeBuffer::getContentToString() const override {
		return data_;
	}
	const char * SizeBuffer::getContentToCstring() const override{
		return data_.c_str();
	}*/
	void SizeBuffer::append(const char * buf)
	{
		data_.append(buf);
	}
	void SizeBuffer::append(const char * begin ,const char *end)
	{
		data_.append(begin , end);
	}

	
	
	//FileBuffer 因为这样子的话会有比如权限之类的问题 因为程序整体都没有用到异常处理 所以如果这里存在异常的话显得比较突兀 所以设置成其他样子
	FileBuffer::FileBuffer() : fileContent_(nullptr) {
	}

	FileBuffer::~FileBuffer() {
		if( fileContent_ )
		{
			if( fileContent_->iov_base != nullptr ) {
				//说明存在东西需要释放
				munmap(fileContent_->iov_base, fileContent_->iov_len);
				delete fileContent_;
				fileContent_ = nullptr;
			}
		}
	}
	//仅提供这一种方式 其中的返回值分别进行定义 比如权限不足是目录等等 分别写成宏
	int FileBuffer::setFileByName(const string & filename){
		if( fileContent_ )
		{
			if( fileContent_->iov_base != nullptr )
				//说明存在东西需要释放
				munmap(fileContent_->iov_base, fileContent_->iov_len);
			delete fileContent_;
			fileContent_ = nullptr;
		}

		std::cout << "[filename] " << filename  << std::endl;
		int fd = ::open( filename.c_str() , O_RDONLY );
		fileContent_ = new struct iovec;
		Util::lock_read_file(fd);
		struct stat st;
		auto x = ::fstat( fd , &st);
//		if( x < 0 )
//			return FB_NOPOWER;
 		std::cout << "[fd] " << fd << std::endl;
		fileContent_->iov_base = ::mmap(nullptr ,st.st_size ,  PROT_READ , MAP_PRIVATE  ,fd , 0 );
		fileContent_->iov_len = st.st_size;

		std::cout << "[][][文件地址][][] = " << (void*)fileContent_->iov_base << std::endl;
		//mmunp
		Util::unlock_file(fd);
		close(fd);
		return FB_OK;
	}
}