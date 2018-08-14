#ifndef _CODE_CHANGE_H_
#define _CODE_CHANGE_H_
//url编码实现 

void urlencode( unsigned char * src, int  src_len, unsigned char * dest, int  dest_len ); 

//解url编码实现 

unsigned char* urldecode(unsigned char* encd,unsigned char* decd);

#endif