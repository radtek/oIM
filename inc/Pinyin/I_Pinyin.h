// Description: I_Pinyin is the convert pinyin, support
//  to Unicode(UTF16) and UTF8(GBK) encoding chinese
//
// Ôø³¤ö¦: Ôø(zeng/ceng)  ³¤(chang/zhang) ö¦(liang/jing)
// UTF16:  D4 F8 B3 A4 F6 A6
// UTF8:   E6 9B BE E9 95 BF E9 9D 93
// eg.: GetUniPinyins/GetUtf8Pinyins
//   pszText:  Ôø³¤ö¦
//	 szPinyin: ZengChangLiang
//	 szSearch: ccj;ccl;czj;czl;zcj;zcl;zzj;zzl;cengchangjing;cengchangliang;cengzhangjing;cengzhangliang;zengchangjing;zengchangliang;zengzhangjing;zengzhangliang;
//	 Return: 16
//
// eg.:GetPinyin
//	i32Code: Ôø(D4 F8)
//  bUnicode: true
//  Return: zeng
//
//	i32Code: Ôø(E6 9B BE)
//  bUnicode: false
//  Return: zeng

// Auth: yfgz
// Date: 2013/12/5 9:28:00 
// 
// History: 
//   2014/10/18 Optimize and szSearch only simple and szPinyin
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IOIMUI_CONVERT_2_PINYIN_HEADER_2013_12_0_YFGZ__
#define __IOIMUI_CONVERT_2_PINYIN_HEADER_2013_12_0_YFGZ__

#include <Windows.h>
#include <tchar.h>
#include "unknown\obj-ref-i.h"
#include <string\tstring.h>
#include "trace.h"

#define INAME_PINYIN	_T("SXIT.Pinyin")	// I_Pinyin interface name

#ifdef USE_PINYIN_LIB
#  if defined(_DEBUG) || defined(DEBUG)
#    pragma comment(lib, "Pinyind.lib")
#  else
#    pragma comment(lib, "Pinyin.lib")
#  endif
#endif

extern int PinyinCreateInterface(const TCHAR* pctszIID, void** ppvIObject);

class __declspec(novtable) I_Pinyin: public IObjRef
{
public:
	//=============================================================================
	//Function:     GetUniPinyins
	//Description:	Get unicode chinese charaters's pinyin, and any different combination of
	//              multi-pinyins
	//
	//Parameter:
	//	pszText     - Chinese charaters string encding by Unicode(UTF16)
	//	szPinyin    - Return the pinyin of chinese charaters, Capitalizes the first letter of 
	//	              each chinese.
	//  szSearch	- Return any different combination of multi-pinyins
	//
	//Return:
	//		>0	The count of combination of multi-pinyins 
	//		=0	No convert to pinyins
	//=============================================================================
	virtual int GetUniPinyins( const WCHAR* pszText, SOUI::SStringA& szPinyin, SOUI::SStringA& szSearch ) = 0;

	//=============================================================================
	//Function:     GetUtf8Pinyins
	//Description:	Get UTF8 chinese charaters's pinyin, and any different combination of
	//              multi-pinyins
	//
	//Parameter:
	//	pszText     - Chinese charaters string encoding by UTF8
	//	szPinyin    - Return the pinyin of chinese charaters, Capitalizes the first letter of 
	//	              each chinese.
	//  szSearch	- Return any different combination of multi-pinyins
	//
	//Return:
	//		>0	The count of combination of multi-pinyins
	//		=0  No convert to pinyin
	//=============================================================================
	virtual int GetUtf8Pinyins( const char* pszText, SOUI::SStringA& szPinyin, SOUI::SStringA& szSearch ) = 0;

	//=============================================================================
	//Function:     GetPinyin
	//Description:	Get a chinese charater's pinyin
	//
	//Parameter:
	//	i32Code     - Chinese charater's code
	//	i32Encoding - 0: Unicode(UTF16), 1: UTF8(Ôø:0xE69BBE), 2: UTF8 Bytes stream(Ôø:{ 0xe6, 0x9b, 0xbe, 0x00 })
	//  i32Bytes	- Return current Code's size in byte
	//
	//Return:
	//		Non-NULL	Return the pinyin if succeed
	//		NULL		Convert failed
	//=============================================================================
	virtual const char* GetPinyin( int i32Code, int i32Encoding, int* i32Bytes ) = 0;

	//=============================================================================
	//Function:     SetFlag
	//Description:	Set flag
	//Parameter:
	//	i32Flag   - Flag
	// 
	//Return:
	// 	TRUE  - succeeded
	//	FALSE - failed
	//=============================================================================
	virtual BOOL SetFlag(int i32Flag = 0) = 0;
	
	//=============================================================================
	//Function:     GetFlag
	//Description:	Get flag
	//
	//Return:
	//	The flag
	//=============================================================================
	virtual  int  GetFlag() const = 0 ;

};

#endif // __IOIMUI_CONVERT_2_PINYIN_HEADER_2013_12_0_YFGZ__