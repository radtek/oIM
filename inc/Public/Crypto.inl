///////////////////////////////////////////////////////////////////////////////
// �㷨�ο���http://www.bubuko.com/infodetail-1128539.html
// ������䷽ʽ PKCS5Padding���£�
//   KCS5Padding������ԭ���ǣ������������16���ֽڣ���Ҫ����16���ֽڣ���(16-len)��(16-len)���磺
//   huguPozhen����ڷ�����9���ֽڣ�16-9= 7,�������磺huguozhen+7��ʮ���Ƶ�7
//   ����ַ�������������16�ֽڣ�����Ҫ�ٲ�16���ֽڵ�ʮ���Ƶ�16��
// ע�⣺
//   0. AesCbcEncrypt ����������OpenSSL �� Base64 ��Դ��
//   1.�������������������ֻ֧�ּ����ַ�������'\0'Ϊ��β�����Ҫ����Buffer������Ҫ���볤��
//   2.�����KeyҲ��Ҫ��16���룬Ĭ�ϲ�0����
//   3.�����תΪBASE64
//   4.iv Ҳ��Ҫ��16���룬������0����
// ʾ����
//	char* pszOut = NULL;
//	char* pszIn  = "yexy,t=231413";
//	char* pszKey = "1234567891234567";
//
//	if ( AesCbcEncrypt(pszIn, pszKey, &pszOut) )
//	{
//		printf("%s", pszOut);	//������������FFllNAgFNJ45xaHmAUhkog==
//		free(pszOut);
//	}
inline BOOL AesCbcEncrypt(const char* pszIn, const char* pszKey, eIMString& szOut, const char* pszIv)
{
	if ( pszIn == NULL || pszKey == NULL)
		return FALSE;

	BOOL bRet = FALSE;
	AES_KEY aesKey = { 0 };
	int i32InLen	   = strlen(pszIn);					// �õ����볤��
	int i32Block	   = i32InLen / AES_BLOCK_SIZE + 1;	// ����Block�����ο�PKCS5Padding
	int i32AlignedLen  = i32Block * AES_BLOCK_SIZE;		// ��������ĳ���
	int i32Md5Len	   = (int)(i32AlignedLen* 1.33 + 10);
	int i32Padding     = (i32InLen % 16 > 0) ? (i32AlignedLen - i32InLen) : 16;
	char* pszInAlgined = NULL;
	char* pszEncrypted = NULL;
	char* pszMd5Out    = NULL;
	char aszKey[AES_BLOCK_SIZE+1] = { 0 };	// +1 ��Ϊ��ʹ��strncpy
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
