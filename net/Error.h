#ifndef _ERROR_H_
#define _ERROR_H_

#include<iostream>
#include<cstdlib>

#define ERR(x) \
{ std::cerr << (x) << std::endl;  \
	exit(-1);\
}

#endif