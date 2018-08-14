#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include"help.h"
#include<string.h>
using std::string;
//先定义一个流的缓冲区类
namespace Cmuduo{

	namespace LOG{
		typedef struct {} End;
	}
	typedef LOG::End End;
	End LOGEND;
	namespace detail{
		
		//两款大小的缓冲区
		const int kSmallBuffer = 4000;
		const int kLargeBuffer = 4000*1000;
		
		template<size_t SIZE>
		class FixBuffer{
			NONCOPYABLE(FixBuffer)
			
			public:
			//重置缓冲区
			void reset() { cur_ = buffer_; }
			//缓冲区清零
			void bezero() { ::bzero(buffer_ , sizeof(buffer_)); }
			//字符串显示
			string toString() const { return string(buffer_, length()); }
			//返回缓冲区剩余字节容量
			int avail() const { return static_cast<int>(end() - cur_); }
			//缓冲区当前数据量
			int length() const { return static_cast<int>(cur_ - buffer_); }
			//缓冲区地址
			const char* data() const { return buffer_; }
			void addLen(int len) { cur_+=len; }
			//当前地址
			char* current() { return cur_; }
			//缓冲区添加数据
			void append(const char* /*restrict*/ buf, size_t len)
			{
			// 为啥要用implicit_cast????? 因为上升的类型转换最好用implicit_cast
				if ( (size_t)(avail()) > len)
				{
					memcpy(cur_, buf, len);
					cur_ += len;
				}
			}

			private:
			
			const char * end() const { return buffer_ + sizeof( buffer_ ); }
			
			char buffer_[SIZE];
			char *cur_;
		};
	}

	//流不做传入
	class LogStream{
		NONCOPYABLE(LogStream);
		
		using self = LogStream;
		using Buffer = detail::FixBuffer<detail::kSmallBuffer>;
		
		public:
			self& operator<<(short);
			self& operator<<(unsigned short);
			self& operator<<(int);
			self& operator<<(unsigned int);
			self& operator<<(long);
			self& operator<<(unsigned long);
			self& operator<<(long long);
			self& operator<<(unsigned long long);
			self& operator<<(double );
			self& operator<<(const void* );
			self& operator<<(const string & str)
			{
				buffer_.append(str.c_str() , str.size());
				return *this;
			}
			self& operator<<(float f)
			{
				*this << static_cast<double>(f);
				return *this;
			}
			
			self& operator<<(const char* str)
			{
				if (str)
				{
					buffer_.append(str, strlen(str));
				}
				else
				{
					buffer_.append("(null)", 6);
				}
				return *this;
			}
			
			self& operator<<(const unsigned char* str)
			{
				//进行指针和值的强制转换
				return operator<<(reinterpret_cast<const char*>(str));
			}
			self& operator<<(const Buffer& v)
			{
				*this << v.toString();
				return *this;
			}
			
			void append(const char* data, int len) { buffer_.append(data, len); }
			const Buffer& buffer() const { return buffer_; }
			void resetBuffer() { buffer_.reset(); }
			
			void flush();//刷新流
		private:
			//一个内部切换的函数模板
			template<class T>
			void formatInteger(T);
			
			//最大的整数的长度设置成32 用于 流的输入有效性检查
			static const int kMaxNumericSize = 32;
			Buffer buffer_;
	};

	inline LogStream& operator<<(LogStream& ls , End e)
	{
		//进行刷新操作 检查全局的
		ls.append("\n" , sizeof("\n"));
		ls.flush();
	}
	
}

#endif