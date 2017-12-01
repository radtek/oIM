#include <time.h>
#include <objbase.h>

inline int eIMMByte2WByte( const char* pszMBytes, eIMStringW& szWBytes )
{
	try
	{
		szWBytes.clear();
		int i32Size = ::MultiByteToWideChar(CP_ACP, 0, pszMBytes, -1, NULL, 0); 
		if ( i32Size == 0 ) 
			return 0;

		WCHAR* pszWideBytes = new WCHAR[ i32Size ];
		i32Size = ::MultiByteToWideChar(CP_ACP, 0, pszMBytes, -1, pszWideBytes , i32Size ); 
		if ( i32Size > 0 )
			szWBytes = pszWideBytes;

		delete [] pszWideBytes;
		return szWBytes.size();
	}
	catch(...)
	{
		return 0;
	}
}

inline int eIMWByte2MByte( const WCHAR* pszWBytes, eIMStringA& szMBytes )
{
	try
	{
		szMBytes.clear();
		int i32Size = ::WideCharToMultiByte( CP_ACP, 0, pszWBytes, -1, NULL, 0, NULL, NULL ); 
		if ( i32Size == 0 ) 
			return 0;

		char* pszMultiBytes = new char[ i32Size ];
		i32Size = ::WideCharToMultiByte( CP_ACP, 0, pszWBytes, -1, pszMultiBytes, i32Size, NULL, NULL );  
		if ( i32Size > 0 )
			szMBytes = pszMultiBytes;

		delete [] pszMultiBytes;
		return szMBytes.size();
	}
	catch(...)
	{
		return 0;
	}
}

inline int eIMMByte2UTF8 ( const char* pszMBytes, eIMStringA& szBytes )
{
	try
	{
		szBytes.clear();
		eIMStringW szWBytes;
		int i32Size = eIMMByte2WByte( pszMBytes, szWBytes );	// To Unicode
		if ( i32Size == 0 )
			return i32Size;

		return eIMWByte2UTF8( szWBytes.c_str(), szBytes );
	}
	catch(...)
	{
		return 0;
	}
}

inline int eIMWByte2UTF8 ( const WCHAR* pszWBytes, eIMStringA& szBytes )
{
	try
	{
		szBytes.clear();
		int i32Size = ::WideCharToMultiByte(CP_UTF8, 0, pszWBytes, -1, NULL, 0, NULL, NULL);   
		if ( i32Size == 0 ) 
			return 0;

		char* pszBytes = new char[ i32Size ];
		i32Size = ::WideCharToMultiByte(CP_UTF8, 0, pszWBytes, -1, pszBytes, i32Size, NULL, NULL);  
		if ( i32Size > 0 )
			szBytes = pszBytes;

		delete [] pszBytes;
		return szBytes.size();
	}
	catch(...)
	{
		return 0;
	}
}

inline int eIMUTF8toMByte( const char* pszBytes, eIMStringA& szMBytes )
{
	try
	{
		szMBytes.clear();
		eIMStringW szWBytes;
		int i32Size = eIMUTF8toWByte( pszBytes, szWBytes );	// To Unicode
		if ( i32Size == 0 )
			return i32Size;

		return eIMWByte2MByte( szWBytes.c_str(), szMBytes );
	}
	catch(...)
	{
		return 0;
	}
}

inline int eIMUTF8toWByte( const char* pszBytes, eIMStringW& szWBytes )
{
	try
	{
		szWBytes.clear();
		int i32Size = ::MultiByteToWideChar(CP_UTF8, 0, pszBytes, -1, NULL, 0);  
		if ( i32Size == 0 ) 
			return 0;

		WCHAR* pszMultiBytes = new WCHAR[ i32Size ];
		i32Size = ::MultiByteToWideChar(CP_UTF8, 0, pszBytes, -1, pszMultiBytes, i32Size); 
		if ( i32Size > 0 )
			szWBytes = pszMultiBytes;

		delete [] pszMultiBytes;
		return szWBytes.size();
	}
	catch(...)
	{
		return 0;
	}
}

inline int eIMWByte2TChar( const WCHAR* pszWBytes, eIMString& tszBytes )
{
#if defined(UNICODE) || defined(_UNICODE)
	tszBytes = pszWBytes;
	return tszBytes.size();
#else
	return eIMWByte2MByte( pszWBytes, tszBytes );
#endif
}

inline int eIMMByte2TChar( const char* pszBytes, eIMString& tszBytes )
{
#if defined(UNICODE) || defined(_UNICODE)
	return eIMMByte2WByte( pszBytes, tszBytes );
#else
	tszBytes = pszBytes;
	return tszBytes.size();
#endif
}

inline int eIMTChar2MByte( const TCHAR* ptszBytes, eIMStringA& szMBytes )
{
#if defined(UNICODE) || defined(_UNICODE)
	return eIMWByte2MByte( ptszBytes, szMBytes );
#else
	szMBytes = ptszBytes;
	return szMBytes.size();
#endif
}

inline int eIMTChar2WByte( const TCHAR* ptszBytes, eIMStringW& szWBytes )
{
#if defined(UNICODE) || defined(_UNICODE)
	szWBytes = ptszBytes;
	return szWBytes.size();
#else
	return eIMMByte2WByte(ptszBytes, szWBytes);
#endif
}

inline int eIMTChar2UTF8 ( const TCHAR* ptszBytes, eIMStringA& szBytes )
{
#if defined(UNICODE) || defined(_UNICODE)
	return eIMWByte2UTF8( ptszBytes, szBytes );
#else
	return eIMMByte2UTF8( ptszBytes, szBytes );
#endif
}

inline int eIMUTF8ToTChar( const char* pszBytes, eIMString& tszBytes )
{
#if defined(UNICODE) || defined(_UNICODE)
	return eIMUTF8toWByte( pszBytes, tszBytes );
#else
	return eIMUTF8toMByte( pszBytes, tszBytes );
#endif
}


///////////////////////////////////////////////////////////////////////////////
inline int Str2Int( const TCHAR* pszValue, int i32Default )
{
	if ( pszValue )
	{
		if ( pszValue[0] == _T('0') && 
			(pszValue[1] == _T('x') || pszValue[1] == _T('X')) )
			return _tcstoul( pszValue, NULL, 16 );	    // hexadecimal
        else if(pszValue[0] == _T('#'))
			return _tcstoul( &pszValue[1], NULL, 16 );	// hexadecimal, for UiLib Color
		else
			return _tcstoul( pszValue, NULL, 10 );	    // decimal
	}
	
	return i32Default;
}

inline __int64 Str2Int64( const TCHAR* pszValue, __int64 i64Default )
{
	if ( pszValue )
	{
		if ( pszValue[0] == _T('0') && 
			(pszValue[1] == _T('x') || pszValue[1] == _T('X')) )
			return _tcstoui64( pszValue, NULL, 16 );	    // hexadecimal
        else if(pszValue[0] == _T('#'))
			return _tcstoui64( &pszValue[1], NULL, 16 );	// hexadecimal, for UiLib Color
		else
			return _tcstoui64( pszValue, NULL, 10 );	    // decimal
	}
	
	return i64Default;
}

inline TCHAR* IntToStr(int i32Value, TCHAR* pszBuf, BOOL bHex, BOOL bSign )	// Convert int/uint to String(hex/decimal) 
{
	if ( pszBuf == NULL )
	{
		return NULL;
	}

	if ( bSign )
		return _itot( i32Value, pszBuf, bHex ? 16 : 10 );
	else
		return _ultot( (unsigned long)i32Value, pszBuf, bHex ? 16 : 10 );
}

inline TCHAR* Int64ToStr(__int64 i32Value, TCHAR* pszBuf, BOOL bHex, BOOL bSign )	// Convert int64/uint64 to String(hex/decimal)
{
	if ( pszBuf == NULL )
	{
		return NULL;
	}

	if ( bSign )
		return _i64tot( i32Value, pszBuf, bHex ? 16 : 10 );
	else
		return _ui64tot( (unsigned __int64)i32Value, pszBuf, bHex ? 16 : 10 );
}

inline eIMString eIMGetGuidString()
{
	GUID guid = { 0 };
	TCHAR szGuid[40] = { 0 };
	if ( SUCCEEDED(CoCreateGuid(&guid)) )
	{
		_sntprintf(szGuid, _countof(szGuid), _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			guid.Data1, guid.Data2, guid.Data3,  
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],  
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	else
	{
		_sntprintf(szGuid, _countof(szGuid), _T("{%08X-0000-0000-0000-000000000000}"), time(NULL));
	}

	return szGuid;
}

//=============================================================================
//Function:     eIMRegexSearch
//Description:	Regular Expression search
//
//Parameter:
//	pszValue	- The string value to be search
//	pszPatten	- The patten to be used
//	pu32Start	- Return the searched start position when not NULL
//	pu32Length	- Return the searched length when not NULL
//
//Return:
//		TRUE	- Searched
//		FALSE	- Not searched
//=============================================================================
inline BOOL eIMRegexSearch(const TCHAR* pszValue, const TCHAR* pszPatten, UINT* pu32Start, UINT* pu32Length, std::regex_constants::syntax_option_type flag)
{
	if ( pu32Start )  *pu32Start  = 0;	// Initial
	if ( pu32Length ) *pu32Length = 0;	
	if ( pszValue == NULL || pszPatten == NULL )
	{
		return FALSE;
	}

	std::match_results<const TCHAR *>  mr;	// match result
	std::basic_regex<TCHAR> rx(pszPatten, flag);	// patten
	if ( std::regex_search(pszValue, mr, rx, std::regex_constants::format_first_only) )
	{
		if ( pu32Start )
			*pu32Start = mr.position(0);

		if ( pu32Length )
			*pu32Length = mr.length(0);

		return TRUE;
	}

	return FALSE;
}

inline TCHAR* eIMStrReplace(TCHAR* pszSrc, TCHAR chFrom, TCHAR chTo)
{
    if (pszSrc == NULL )
        return pszSrc;
        
    for (TCHAR* pszCh = pszSrc; *pszCh; pszCh++)
    {
        if (*pszCh ==chFrom)
            *pszCh=chTo;
    }
    
    return pszSrc;
}
