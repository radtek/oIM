#pragma once

#define DES_ENCRYPT 1
#define DES_DECRYPT 0

class CDes
{
public:
	CDes(void);
	~CDes(void);

	void Setkey(const char *pczKey);  //��������Կ
	int Desrun(char czOut[], const char *cszIn, int itype);
protected:
	int ByteToBit(char ch,char bit[8]);  
	int BitToByte(char bit[8],char *ch);  
	int Char8ToBit64(char ch[8],char bit[64]);  
	int Bit64ToChar8(char bit[64],char ch[8]);  
	int DES_MakeSubKeys(char key[64],char subKeys[16][48]);  
	int DES_PC1_Transform(char key[64], char tempbts[56]);  
	int DES_PC2_Transform(char key[56], char tempbts[48]);  
	int DES_ROL(char data[56], int time);  
	int DES_IP_Transform(char data[64]);  
	int DES_IP_1_Transform(char data[64]);  
	int DES_E_Transform(char data[48]);  
	int DES_P_Transform(char data[32]);  
	int DES_SBOX(char data[48]);  
	int DES_XOR(char R[48], char L[48],int count);  
	int DES_Swap(char left[32],char right[32]);  

	int DES_EncryptBlock(char plainBlock[8], char subKeys[16][48], char cipherBlock[8]);  
	int DES_DecryptBlock(char cipherBlock[8], char subKeys[16][48], char plainBlock[8]);  
private:
	// 16λ����Կ
	char SubKey[16][48];
};

