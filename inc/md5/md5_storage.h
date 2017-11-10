#ifndef MD5___H
#define MD5___H


typedef struct
{
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
}MD5_CTX;


void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16]);
void MD5Transform(unsigned int state[4],unsigned char block[64]);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);

//函数说明：获得md5后32个小写16进制串
//pOut 长度33个，最后0结尾
//pSrcData 待MD5的原始数据
//iSrcLen 原始数据串长度
void  Create32Md5(unsigned char *pSrcData, int iSrcLen, unsigned char *pOut);
#include "md5_storage.inl"
#endif