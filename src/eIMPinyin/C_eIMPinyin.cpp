#include "StdAfx.h"
#include "C_eIMPinyin.h"
#include "Public\Utils.h"

//extern const char* GBK2Pinyin(int i32Char);
extern const char* Uni2Pinyin(int i32Char);
int eIMPinyinCreateInterface(const TCHAR* pctszIID, void** ppvIObject)
{
	if( pctszIID == NULL || ppvIObject == NULL )
		return E_POINTER;

	if( _tcsnicmp(pctszIID, INAME_EIMUI_PINYIN, _tcslen(INAME_EIMUI_PINYIN)) == 0 )
	{
		static C_eIMPinyin obj;
		*ppvIObject = &obj;

		return EIMERR_NO_ERROR;
	}

	return EIMERR_NOT_IMPL;
}

IMPLEMENT_PLUGIN_SINGLETON_(C_eIMPinyin, INAME_EIMUI_PINYIN);
C_eIMPinyin::C_eIMPinyin(void)
	: m_i32Flag( 0 )
{
}

C_eIMPinyin::~C_eIMPinyin(void)
{
}

int	C_eIMPinyin::Vect2Str(vector<eIMStringA>& vectPinyin, eIMStringA& szPinyin)
{
	int i32count = 0;
	for ( vector<eIMStringA>::iterator itor = vectPinyin.begin(); itor != vectPinyin.end(); itor++ )
	{
		if ( szPinyin.npos == szPinyin.find( *itor ))
		{
			szPinyin += *itor + ";";
			i32count++;
		}	
	}
	return i32count;
}

int C_eIMPinyin::GetUniPinyins( const WCHAR* pszText, eIMStringA& szPinyin, eIMStringA& szSearch )
{
	szPinyin.clear();
	szSearch.clear();

	CHECK_NULL_RET_(pszText, 0);
	const int	 i32MPyMax = 96;	// Polyphone group max count(2^6)
	const int	 i32PyMax  = 240;	// Eatch pinyin max char
	int			 i32MPyCount = 0;	// Polyphone group count
	int			 i32PyCount  = 0;	// Pinyin count
	int			 i32MPyIndex = 0;	// Polyphone index
	char		 aaachPinyin[2][i32MPyMax][i32PyMax] = { 0 };
	const WCHAR* pszHz       = pszText;
	const char*  pszHzPinyin = NULL;
	const char*  pszHzPinyinFull = NULL;
	
	while( *pszHz )
	{
		pszHzPinyin = Uni2Pinyin( *pszHz++ );
		pszHzPinyinFull = pszHzPinyin;
		if ( *pszHzPinyin == '\0' )
			continue;
		
		szPinyin   += pszHzPinyin;	// Firstly pinyin 
		if ( i32MPyCount == 0 )
		{	// Initial Multi-Pinyin
			aaachPinyin[0][0][0] = tolower(*pszHzPinyin);
		}
		else
		{	// Add the pinyin's first-letter
			for ( i32MPyIndex = 0; i32MPyIndex < i32MPyCount; i32MPyIndex++ )
			{
				aaachPinyin[0][i32MPyIndex][i32PyCount] = tolower(*pszHzPinyin);
			}
		}

		// For multi-pinyin
		int i32CurMPyIndex = i32MPyCount ? i32MPyCount : 1;
		int i32CurMPyCount = i32CurMPyIndex;		// Current Polyphone count
		int i32CurMPyNum   = 0;
		for ( pszHzPinyin += strlen( pszHzPinyin ) + 1;
			 *pszHzPinyin != '\0' && i32CurMPyIndex < i32MPyMax;
			  pszHzPinyin += strlen( pszHzPinyin ) + 1)
		{
			i32CurMPyNum++;
			if ( i32MPyCount == 0 )
			{	// Initial the first letter
				aaachPinyin[0][i32CurMPyIndex][0] = tolower(*pszHzPinyin);
				i32CurMPyCount = ++i32CurMPyIndex;
				continue;
			}

			// Copy-asign a new group Polyphone
			for ( i32MPyIndex = 0, i32CurMPyIndex = i32CurMPyCount; 
				  i32MPyIndex < i32MPyCount && i32CurMPyIndex < i32MPyMax ;
				  i32MPyIndex++, i32CurMPyIndex++ )
			{
				memcpy(aaachPinyin[0][i32CurMPyIndex], aaachPinyin[0][i32MPyIndex], i32PyCount);
				aaachPinyin[0][i32CurMPyIndex][i32PyCount] = tolower(*pszHzPinyin);
			}

			i32CurMPyCount = i32CurMPyIndex;
		}

		if ( m_i32Flag == 0 )
		{	// For multi-pinyin of all combination of full pinyin
			int i32To = 0;
			int i32Len= 0;
			if ( i32CurMPyNum > 0 )
			{
				for ( i32MPyIndex = 0; i32MPyIndex < i32CurMPyNum; i32MPyIndex++ )
				{
					for ( int i32Index = 0; i32Index < i32MPyCount; i32Index++ )
					{
						i32To = (i32MPyIndex + 1) * i32MPyCount + i32Index;
						ASSERT_(i32To < i32MPyMax);
						if ( i32To >= i32MPyMax )
						{
							TRACE_(_T("Exceed the max of Text:%s"), pszText);
							break;
						}

						strncpy(aaachPinyin[1][i32To], aaachPinyin[1][i32Index], i32PyMax - 1);
					}
				}

				for ( i32MPyIndex = 0; i32MPyIndex <= i32CurMPyNum && i32MPyIndex < i32MPyMax; i32MPyIndex++, pszHzPinyinFull += strlen( pszHzPinyinFull ) + 1  )
				{
					if ( i32MPyCount == 0 )
					{
						i32Len = strlen(aaachPinyin[1][i32MPyIndex]);
						strncat(aaachPinyin[1][i32MPyIndex], pszHzPinyinFull, i32PyMax - i32Len - 1);
						continue;
					}

					for ( int i32Index = 0; i32Index < i32MPyCount; i32Index++ )
					{
						i32To = i32MPyIndex * i32MPyCount + i32Index;
						ASSERT_(i32To < i32MPyMax);
						if ( i32To >= i32MPyMax )
							break;

						i32Len= strlen(aaachPinyin[1][i32To]);
						strncat(aaachPinyin[1][i32To], pszHzPinyinFull, i32PyMax - i32Len - 1);
					}
				}
			}
			else
			{
				if ( i32MPyCount == 0 )
				{	// Initial Multi-Pinyin
					i32Len = strlen(aaachPinyin[1][0]);
					strncat(aaachPinyin[1][0], pszHzPinyinFull, i32PyMax - i32Len - 1);
				}
				else
				{	// Add the pinyin
					for ( i32MPyIndex = 0; i32MPyIndex < i32MPyCount; i32MPyIndex++ )
					{
						i32Len = strlen(aaachPinyin[1][i32MPyIndex]);
						strncat(aaachPinyin[1][i32MPyIndex], pszHzPinyinFull, i32PyMax - i32Len - 1);
					}
				}
			}
		}	// if ( m_i32Flag == 0 )

		// Adjust the count
		i32PyCount++;
		i32MPyCount = i32CurMPyCount;
		if ( i32CurMPyIndex >= i32MPyMax || i32PyCount >= i32PyMax - 1 )
			break;
	}

	for (i32MPyIndex = 0; i32MPyIndex < i32MPyCount; i32MPyIndex++)
	{	// Make search 
		szSearch += aaachPinyin[0][i32MPyIndex];
		szSearch += ";";
	}

	if ( m_i32Flag == 0 )
	{
		for (i32MPyIndex = 0; i32MPyIndex < i32MPyCount; i32MPyIndex++)
		{	// Make search 
			szSearch += aaachPinyin[1][i32MPyIndex];
			szSearch += ";";
		}
	}
	else
		szSearch += szPinyin;	// Append full pinyin

	return i32MPyCount;
}

int C_eIMPinyin::GetUtf8Pinyins( const char* pszText, eIMStringA& szPinyin, eIMStringA& szSearch )
{
	eIMStringW szWText;
	//将多字节(UTF8)转化为宽字节
	if( 0 == eIMUTF8toWByte( pszText, szWText ) )
	{
		return 0;	//转化失败
	}

	//调用GetUniPinyins函数进行拼音获取并返回组合的种数
	return GetUniPinyins( (WCHAR*)szWText.c_str(), szPinyin, szSearch );
}

const char* C_eIMPinyin::GetPinyin( int i32Code, int i32Encoding, int* pi32Bytes  )
{
	if ( 0 == i32Encoding )	//Unicode(UTF16)
	{
		if ( pi32Bytes )
		{
			*pi32Bytes = 2;
		}	
		return Uni2Pinyin( i32Code );
	}
	else if( 1 == i32Encoding )	//UTF8
	{
		char		chTmp[4];
		eIMStringW	szWBytes;

		if ( i32Code <= 0xFFFFFF && i32Code >= 0x00FFFF )
		{
			chTmp[0] = ( i32Code & 0xFF0000 ) >> 16;
			chTmp[1] = ( i32Code & 0x00FF00 ) >> 8;
			chTmp[2] = i32Code & 0x0000FF;
			chTmp[3] = 0;
			if ( pi32Bytes )
			{
				*pi32Bytes = 3;
			}
		} 
		else if ( i32Code <= 0xFFFF && i32Code >= 0x00FF )
		{
			chTmp[0] = ( i32Code & 0xFF00 ) >> 8;
			chTmp[1] = i32Code & 0x00FF;
			chTmp[2] = 0;
			if ( pi32Bytes )
			{
				*pi32Bytes = 2;
			}
		}
		else
		{
			chTmp[0] = i32Code;
			chTmp[1] = 0;
			if ( pi32Bytes )
			{
				*pi32Bytes = 1;
			}
		}
		
		eIMUTF8toWByte( chTmp, szWBytes );
		int i32Rlt = szWBytes[0];
		return Uni2Pinyin( i32Rlt );
	}
	else if ( 2 == i32Encoding )	//UTF8 Bytes stream
	{
		char		chTmp[4];
		eIMStringW	szWBytes;

		chTmp[0] = i32Code & 0x000000FF;
		chTmp[1] = ( i32Code & 0x0000FF00 ) >> 8;
		chTmp[2] = ( i32Code & 0x00FF0000 ) >> 16;
		chTmp[3] = ( i32Code & 0xFF000000 ) >> 24;		
		
		eIMUTF8toWByte( chTmp, szWBytes );
		int i32Rlt = szWBytes[0];

		if ( pi32Bytes )
		{
			if ( i32Rlt <= 0xFFFF && i32Rlt > 0x00FF )
			{
				*pi32Bytes = 3;
			}
			else if ( i32Rlt <= 0xFF )
			{
				*pi32Bytes = 1;
			}
		}
		return Uni2Pinyin( i32Rlt );
	}
	return "";
}

BOOL C_eIMPinyin::SetFlag(int i32Flag)
{
	m_i32Flag = i32Flag & 1;	// 0: All combination; 1: fast
	return TRUE;
}

int  C_eIMPinyin::GetFlag() const
{
	return m_i32Flag;
}
