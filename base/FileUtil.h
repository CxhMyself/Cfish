#ifndef _FILE_UTIL_
#define _FILE_UTIL_

#include"help.h"
#include"type.h"


using std::string;
//知道你不想写这个 但是为了未来请你加油！
namespace Cmuduo{
	namespace FileUtil{
		//用以获取文件的信息 
		class ReadSmallFile{
			NONCOPYABLE(ReadSmallFile);
			public:
			ReadSmallFile(const std::string &);
			~ReadSmallFile();

			template<class String>
			int readToString(int MaxSize,String* content, int64_t *fileSize = nullptr , int64_t *modifyTime = nullptr, int64_t *createTime = nullptr);

			int readToBuffer(int *size);
			const char *buffer() { return buf_; }


			private:
			static const int BufferSize = 64*1024;
			int fd_;
			int err_;
			char buf_[BufferSize];

		};

		template<class String>
		int readFile( string& filename, int MaxSize,String* content, int64_t *fileSize , int64_t *modifyTime, int64_t *createTime)
		{
			ReadSmallFile file(filename);
			return file.readToString(MaxSize,content, fileSize , modifyTime, createTime);
		}

		//添加到日志文件所用的类
		class AppendFile
		{
			NONCOPYABLE(AppendFile);
			public:
			explicit AppendFile(const string& filename);
			~AppendFile();
			void append(const char* logline , const size_t len);

			//将文件缓冲区的内容刷新到文件中
			void flush();
			
			off_t writtenBytes() const {return writttenBytes_; }
			
			private:
			
			size_t write(const char * logline , size_t len);
			//文件指针 用于函数内的文件操作
			FILE *fp_;
			//这个的作用是待绑定的一个文件缓冲区
			char buffer_[64*1024];
			//已经写入的字节数
			off_t writttenBytes_;
		}
	}
}

#endif