#include "StdAfx.h"
#include <string.h>
#include <stdio.h>


#define F_(x,y,z) ((x & y) | (~x & z))  
#define G_(x,y,z) ((x & z) | (y & ~z))  
#define H_(x,y,z) (x^y^z)  
#define I_(x,y,z) (y ^ (x | ~z))  
#define ROTATE_LEFT_(x,n) ((x << n) | (x >> (32-n)))  

#define FF_(a,b,c,d,x,s,ac) \
{ \
	a += F_(b,c,d) + x + ac; \
	a = ROTATE_LEFT_(a,s); \
	a += b; \
}  
#define GG_(a,b,c,d,x,s,ac) \
{ \
	a += G_(b,c,d) + x + ac; \
	a = ROTATE_LEFT_(a,s); \
	a += b; \
}  
#define HH_(a,b,c,d,x,s,ac) \
{ \
	a += H_(b,c,d) + x + ac; \
	a = ROTATE_LEFT_(a,s); \
	a += b; \
}
#define II_(a,b,c,d,x,s,ac) \
{ \
	a += I_(b,c,d) + x + ac; \
	a = ROTATE_LEFT_(a,s); \
	a += b; \
}

  
static unsigned char PADDINGS[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
inline void MD5Init(MD5_CTX *context)  
{  
    context->count[0] = 0;  
    context->count[1] = 0;  
    context->state[0] = 0x67452301;  
    context->state[1] = 0xEFCDAB89;  
    context->state[2] = 0x98BADCFE;  
    context->state[3] = 0x10325476;  
}  

inline void MD5Update(MD5_CTX *context, unsigned char *input,unsigned int inputlen)  
{  
    unsigned int i = 0,index = 0,partlen = 0;  
    index = (context->count[0] >> 3) & 0x3F;  
    partlen = 64 - index;  
    context->count[0] += inputlen << 3;  
    if(context->count[0] < (inputlen << 3))  
        context->count[1]++;  
    context->count[1] += inputlen >> 29;  
  
    if(inputlen >= partlen)  
    {  
        memcpy(&context->buffer[index],input,partlen);  
        MD5Transform(context->state,context->buffer);  
        for(i = partlen;i+64 <= inputlen;i+=64)  
            MD5Transform(context->state,&input[i]);  
        index = 0;          
    }    
    else  
    {  
        i = 0;  
    }  
    memcpy(&context->buffer[index],&input[i],inputlen-i);  
}  

inline void MD5Final(MD5_CTX *context,unsigned char digest[16])  
{  
    unsigned int index = 0,padlen = 0;  
    unsigned char bits[8];  
    index = (context->count[0] >> 3) & 0x3F;  
    padlen = (index < 56)?(56-index):(120-index);  
    MD5Encode(bits,context->count,8);  
    MD5Update(context,PADDINGS,padlen);  
    MD5Update(context,bits,8);  
    MD5Encode(digest,context->state,16);  
}  
inline void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
    {  
        output[j] = input[i] & 0xFF;    
        output[j+1] = (input[i] >> 8) & 0xFF;  
        output[j+2] = (input[i] >> 16) & 0xFF;  
        output[j+3] = (input[i] >> 24) & 0xFF;  
        i++;  
        j+=4;  
    }  
}

inline void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)
{
	unsigned int i = 0,j = 0;
	while(j < len)
	{
		output[i] = (input[j]) |
			(input[j+1] << 8) |
			(input[j+2] << 16) |
			(input[j+3] << 24);
		i++;
		j+=4;
	}
}

inline void MD5Transform(unsigned int state[4],unsigned char block[64])
{
	unsigned int a = state[0];
	unsigned int b = state[1];
	unsigned int c = state[2];
	unsigned int d = state[3];
	unsigned int x[64];
	MD5Decode(x,block,64);
	FF_(a, b, c, d, x[ 0], 7, 0xd76aa478);
	FF_(d, a, b, c, x[ 1], 12, 0xe8c7b756);
	FF_(c, d, a, b, x[ 2], 17, 0x242070db);
	FF_(b, c, d, a, x[ 3], 22, 0xc1bdceee);
	FF_(a, b, c, d, x[ 4], 7, 0xf57c0faf);
	FF_(d, a, b, c, x[ 5], 12, 0x4787c62a);
	FF_(c, d, a, b, x[ 6], 17, 0xa8304613);
	FF_(b, c, d, a, x[ 7], 22, 0xfd469501);
	FF_(a, b, c, d, x[ 8], 7, 0x698098d8);
	FF_(d, a, b, c, x[ 9], 12, 0x8b44f7af);
	FF_(c, d, a, b, x[10], 17, 0xffff5bb1);
	FF_(b, c, d, a, x[11], 22, 0x895cd7be);
	FF_(a, b, c, d, x[12], 7, 0x6b901122);
	FF_(d, a, b, c, x[13], 12, 0xfd987193);
	FF_(c, d, a, b, x[14], 17, 0xa679438e);
	FF_(b, c, d, a, x[15], 22, 0x49b40821);

	GG_(a, b, c, d, x[ 1], 5, 0xf61e2562);
	GG_(d, a, b, c, x[ 6], 9, 0xc040b340);
	GG_(c, d, a, b, x[11], 14, 0x265e5a51);
	GG_(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
	GG_(a, b, c, d, x[ 5], 5, 0xd62f105d);
	GG_(d, a, b, c, x[10], 9,  0x2441453);
	GG_(c, d, a, b, x[15], 14, 0xd8a1e681);
	GG_(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
	GG_(a, b, c, d, x[ 9], 5, 0x21e1cde6);
	GG_(d, a, b, c, x[14], 9, 0xc33707d6);
	GG_(c, d, a, b, x[ 3], 14, 0xf4d50d87);
	GG_(b, c, d, a, x[ 8], 20, 0x455a14ed);
	GG_(a, b, c, d, x[13], 5, 0xa9e3e905);
	GG_(d, a, b, c, x[ 2], 9, 0xfcefa3f8);
	GG_(c, d, a, b, x[ 7], 14, 0x676f02d9);
	GG_(b, c, d, a, x[12], 20, 0x8d2a4c8a);

	HH_(a, b, c, d, x[ 5], 4, 0xfffa3942);
	HH_(d, a, b, c, x[ 8], 11, 0x8771f681);
	HH_(c, d, a, b, x[11], 16, 0x6d9d6122);
	HH_(b, c, d, a, x[14], 23, 0xfde5380c);
	HH_(a, b, c, d, x[ 1], 4, 0xa4beea44);
	HH_(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
	HH_(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
	HH_(b, c, d, a, x[10], 23, 0xbebfbc70);
	HH_(a, b, c, d, x[13], 4, 0x289b7ec6);
	HH_(d, a, b, c, x[ 0], 11, 0xeaa127fa);
	HH_(c, d, a, b, x[ 3], 16, 0xd4ef3085);
	HH_(b, c, d, a, x[ 6], 23,  0x4881d05);
	HH_(a, b, c, d, x[ 9], 4, 0xd9d4d039);
	HH_(d, a, b, c, x[12], 11, 0xe6db99e5);
	HH_(c, d, a, b, x[15], 16, 0x1fa27cf8);
	HH_(b, c, d, a, x[ 2], 23, 0xc4ac5665);

	II_(a, b, c, d, x[ 0], 6, 0xf4292244);
	II_(d, a, b, c, x[ 7], 10, 0x432aff97);
	II_(c, d, a, b, x[14], 15, 0xab9423a7);
	II_(b, c, d, a, x[ 5], 21, 0xfc93a039);
	II_(a, b, c, d, x[12], 6, 0x655b59c3);
	II_(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
	II_(c, d, a, b, x[10], 15, 0xffeff47d);
	II_(b, c, d, a, x[ 1], 21, 0x85845dd1);
	II_(a, b, c, d, x[ 8], 6, 0x6fa87e4f);
	II_(d, a, b, c, x[15], 10, 0xfe2ce6e0);
	II_(c, d, a, b, x[ 6], 15, 0xa3014314);
	II_(b, c, d, a, x[13], 21, 0x4e0811a1);
	II_(a, b, c, d, x[ 4], 6, 0xf7537e82);
	II_(d, a, b, c, x[11], 10, 0xbd3af235);
	II_(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
	II_(b, c, d, a, x[ 9], 21, 0xeb86d391);
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

inline void Create32Md5(unsigned char *pSrcData, int iSrcLen, unsigned char *pOut)
{
	unsigned char decrypt[16];
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5,pSrcData,iSrcLen);
	MD5Final(&md5,decrypt);
	unsigned int i32index;
	for (i32index = 0; i32index < 16; i32index++)
	{
		sprintf((char*)pOut, "%02x", decrypt[i32index]);
		pOut += 2;
	}
}
