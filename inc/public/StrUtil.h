// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: String operate helper function
// 
// 
// 
// Auth: LongWQ
// Date: 2013/12/21 19:11:49
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_STRING_UTIL_API_2013_12_2_BY_YFGZ__
#define __EIM_STRING_UTIL_API_2013_12_2_BY_YFGZ__

#include <tchar.h>
#include <stdlib.h>
#include <regex>
//#include "Debugs.h"
#include "typedefs.h"



#define CONVERT_TO_STRING_SIZE	( 65 )	// Max size in TCHAR of convert to string buffer


//
int eIMMByte2WByte( const char*  pszMBytes, eIMStringW& szWBytes );	// Convert Multi-Bytes to Wide-Bytes
int eIMWByte2MByte( const WCHAR* pszWBytes, eIMStringA& szMBytes );	// Convert Wide-Bytes to Multi-Bytes

int eIMMByte2UTF8 ( const char*  pszMBytes, eIMStringA& szBytes );	// Convert Multi-Bytes to UTF8s
int eIMWByte2UTF8 ( const WCHAR* pszWBytes, eIMStringA& szBytes );	// Convert Wide-Bytes to UTF8s
int eIMUTF8toMByte( const char*  pszBytes,  eIMStringA& szMBytes );	// Convert UTF8s to Multi-Bytes
int eIMUTF8toWByte( const char*  pszBytes,  eIMStringW& szWBytes );	// Convert UTF8s to Wide-Bytes

int eIMWByte2TChar( const WCHAR* pszWBytes, eIMString&  tszBytes );	// Convert Wide-Bytes to TCHAR*
int eIMMByte2TChar( const char*  pszBytes,  eIMString&  tszBytes );	// Convert Multi-Byte to TCHAR*
int eIMTChar2MByte( const TCHAR* ptszBytes, eIMStringA& szMBytes );	// Convert TCHAR* to Multi-Bytes
int eIMTChar2WByte( const TCHAR* ptszBytes, eIMStringW& szWBytes );	// Convert TCHAR* to Wide-Bytes
int eIMTChar2UTF8 ( const TCHAR* ptszBytes, eIMStringA& szBytes );	// Convert TCHAR* to UTF8 ( Merged eIMMByte2UTF8 & eIMWByte2UTF8 )
int eIMUTF8ToTChar( const char*  pszBytes,  eIMString&  tszBytes );	// Convert UTF8s to TCHAR* ( Merged eIMUTF8toMByte & eIMUTF8toWByte )
BOOL eIMRegexSearch(const TCHAR* pszValue, const TCHAR* pszPatten, UINT* pu32Start = NULL, UINT* pu32Length = NULL, std::regex_constants::syntax_option_type=std::regex_constants::ECMAScript);	// Regular Expression search

///////////////////////////////////////////////////////////////////////////////
int		Str2Int   ( const TCHAR* pszValue, int     i32Default );	// Convert String(hex/decimal) to int/uint
__int64 Str2Int64 ( const TCHAR* pszValue, __int64 i64Default );	// Convert String(hex/decimal) to int64/uint64

TCHAR* IntToStr   ( int     i32Value, TCHAR* pszBuf, BOOL bHex = TRUE, BOOL bSign = TRUE );	// Convert int/uint to String(hex/decimal) 
TCHAR* Int64ToStr ( __int64 i32Value, TCHAR* pszBuf, BOOL bHex = TRUE, BOOL bSign = TRUE );	// Convert int64/uint64 to String(hex/decimal)
eIMString GetGuidString();
TCHAR* eIMStrReplace(TCHAR* pszSrc, TCHAR chFrom, TCHAR chTo);

#include "StrUtil.inl"

#endif	// __EIM_STRING_UTIL_API_2013_12_2_BY_YFGZ__

