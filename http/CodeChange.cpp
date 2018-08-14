
#include<string>
#include<string.h>
#include<stdio.h>
#include"CodeChange.h"
static unsigned char char_to_hex( unsigned char x ) 
{ 
return (unsigned char)(x > 9 ? x + 55: x + 48); 
} 
 
static int is_alpha_number_char( unsigned char c ) 
{ 
if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ) 
  return 1; 
return 0; 
} 
 
//url编码实现 
 
void urlencode( unsigned char * src, int  src_len, unsigned char * dest, int  dest_len ) 
{ 
unsigned char ch; 
int  len = 0; 
 
while (len < (dest_len - 4) && *src) 
{ 
  ch = (unsigned char)*src; 
  if (*src == ' ') 
  { 
   *dest++ = '+'; 
  } 
  else if (is_alpha_number_char(ch) || strchr("=!~*'()", ch)) 
  { 
   *dest++ = *src; 
  } 
  else 
  { 
   *dest++ = '%'; 
   *dest++ = char_to_hex( (unsigned char)(ch >> 4) ); 
   *dest++ = char_to_hex( (unsigned char)(ch % 16) ); 
  }  
  ++src; 
  ++len; 
} 
*dest = 0; 
return ; 
} 
 
 
 
//解url编码实现 
 
unsigned char* urldecode(unsigned char* encd,unsigned char* decd) 
{ 
    int j,i; 
    char *cd =(char*) encd; 
    char p[2]; 
    unsigned int num; 
	j=0; 
 
    for( i = 0; i < strlen(cd); i++ ) 
    { 
        memset( p, '\0', 2 ); 
        if( cd[i] != '%' ) 
        { 
            decd[j++] = cd[i]; 
            continue; 
        } 
  
  p[0] = cd[++i]; 
        p[1] = cd[++i]; 
 
        p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0); 
        p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0); 
        decd[j++] = (unsigned char)(p[0] * 16 + p[1]); 
  
    } 
    decd[j] = '\0'; 
 
    return decd; 
}