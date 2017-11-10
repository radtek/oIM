///////////////////////////////////////////////////////////////////////////////
// 算法参考：http://www.bubuko.com/infodetail-1128539.html
// 对齐填充方式 PKCS5Padding如下：
//   KCS5Padding：填充的原则是，如果长度少于16个字节，需要补满16个字节，补(16-len)个(16-len)例如：
//   huguPozhen这个节符串是9个字节，16-9= 7,补满后如：huguozhen+7个十进制的7
//   如果字符串长度正好是16字节，则需要再补16个字节的十进制的16。
// 注意：
//   0. AesCbcEncrypt 函数会依赖OpenSSL 和 Base64 开源库
//   1.本函数所有输入参数，只支持加密字符串，即'\0'为结尾。如果要加密Buffer，就需要传入长度
//   2.输入的Key也需要按16对齐，默认补0对齐
//   3.输出会转为BASE64
//   4.iv 也需要以16对齐，不足以0补齐
// 示例：
//	char* pszOut = NULL;
//	char* pszIn  = "yexy,t=231413";
//	char* pszKey = "1234567891234567";
//
//	if ( AesCbcEncrypt(pszIn, pszKey, &pszOut) )
//	{
//		printf("%s", pszOut);	//加密输出结果：FFllNAgFNJ45xaHmAUhkog==
//		free(pszOut);
//	}
inline BOOL AesCbcEncrypt(const char* pszIn, const char* pszKey, eIMString& szOut, const char* pszIv)
{
	if ( pszIn == NULL || pszKey == NULL)
		return FALSE;

	BOOL bRet = FALSE;
	AES_KEY aesKey = { 0 };
	int i32InLen	   = strlen(pszIn);					// 得到输入长度
	int i32Block	   = i32InLen / AES_BLOCK_SIZE + 1;	// 计算Block数，参考PKCS5Padding
	int i32AlignedLen  = i32Block * AES_BLOCK_SIZE;		// 计算对齐后的长度
	int i32Md5Len	   = (int)(i32AlignedLen* 1.33 + 10);
	int i32Padding     = (i32InLen % 16 > 0) ? (i32AlignedLen - i32InLen) : 16;
	char* pszInAlgined = NULL;
	char* pszEncrypted = NULL;
	char* pszMd5Out    = NULL;
	char aszKey[AES_BLOCK_SIZE+1] = { 0 };	// +1 是为了使用strncpy
	char aszIV[AES_BLOCK_SIZE+1] = { 0 };	// ditto

	do
	{
		pszInAlgined = (char*)malloc(i32AlignedLen);
		pszEncrypted = (char*)malloc(i32AlignedLen);
		pszMd5Out    = (char*)malloc(i32Md5Len);
		if ( pszInAlgined == NULL || pszEncrypted == NULL || pszMd5Out == NULL )
			break;

		memset(pszInAlgined, i32Padding, i32AlignedLen);
		memcpy(pszInAlgined, pszIn, i32InLen);

		memset(pszEncrypted, 0, i32AlignedLen);
		memset(pszMd5Out, 0, i32Md5Len);

		strncpy(aszKey, pszKey, AES_BLOCK_SIZE);
		strncpy(aszIV,  pszIv,  AES_BLOCK_SIZE);

		if (AES_set_encrypt_key((unsigned char*)aszKey, 128, &aesKey) < 0) 
			break;

		AES_cbc_encrypt((unsigned char *)pszInAlgined, (unsigned char*)pszEncrypted, i32Block * AES_BLOCK_SIZE, &aesKey, (unsigned char*)aszIV, AES_ENCRYPT);
		b64_encode(pszMd5Out, pszEncrypted, i32Block * AES_BLOCK_SIZE);
		eIMUTF8ToTChar(pszMd5Out, szOut);

		bRet = TRUE;
	}while(0);

	SAFE_FREE_PTR_( pszInAlgined );
	SAFE_FREE_PTR_( pszEncrypted );
	SAFE_FREE_PTR_( pszMd5Out );

	return bRet;
}
