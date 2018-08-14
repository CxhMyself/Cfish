#include"InetAddress.h"
#include"string.h"

using namespace Cfish;

InetAddress::InetAddress(const struct sockaddr_in * addr)
{
	addr_ = new struct sockaddr_in[1];
	*addr_ = *addr;
	len = sizeof(*addr);
}

//默认是连接所有的网卡 INADDR_ANY
InetAddress::InetAddress( uint16_t port , const char * ip )
{
	addr_ = new struct sockaddr_in[1];
	len = 0;
	
	addr_->sin_family = AF_INET;
	if(strcmp("0.0.0.0" , ip) == 0)
		addr_->sin_addr.s_addr = INADDR_ANY;
	else
		::inet_aton(ip , &( addr_->sin_addr) );
	addr_->sin_port = htons(port);
	len = sizeof(*addr_);
}
