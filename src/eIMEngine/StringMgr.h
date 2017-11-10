#pragma once
#include "stdafx.h"

#define IME_STRING_BUF_BLOCK_SIZE		(4 * 1024 * 1024 )	// 4MB
#define IME_STRING_BUF_COUNT			( 200 )				// Buffer count

#define eIMAddString_(P)	C_StringMgr::GetObject().AddString(P)

class C_StringMgr
{
private:
	C_StringMgr(void)
	{
		ZERO_STRUCT_(m_sStrBuf);
		m_sStrBuf.u32BlockSize = IME_STRING_BUF_BLOCK_SIZE;
		AddString(L"");	// the Empty string at the begin
	}

	// The contacts Conflict ratio about: 6 / 231921 = 0.00002587087844567762298368841114
	inline UINT  _Hash(const BYTE* pu8Str, UINT u32Bytes)
	{
		if (  u32Bytes == 0 || pu8Str == NULL) 
			return 0u; 

		register UINT u32Index = 0u;
		register UINT u32Hash  = 0u;  
		do
		{
			u32Hash = u32Hash * 131u + pu8Str[u32Index] + 1;
		}while(++u32Index < u32Bytes);

		return u32Hash;
	}
	
	inline BYTE* _Add2StringBuf( const BYTE* pszBuf, UINT u32Bytes)
	{
		if ( pszBuf == NULL )
			return NULL;

		BOOL bNewBuf = FALSE;
		if ( m_sStrBuf.aszBuf[m_sStrBuf.u32Index] == NULL ||
			( bNewBuf = (m_sStrBuf.pszPos - m_sStrBuf.aszBuf[m_sStrBuf.u32Index] + u32Bytes > m_sStrBuf.u32BlockSize)) )
		{	// Not allocate memory, do it
			if ( bNewBuf )
			{	// New item
				m_sStrBuf.u32Index++;
				if ( m_sStrBuf.u32Index >= IME_STRING_BUF_COUNT )
				{
					m_sStrBuf.u32Index = IME_STRING_BUF_COUNT - 1;
					TRACE_(_T("String buffer is not enough"));
					return NULL;	// Out of pinyin cache buffer
				}
			}

			m_sStrBuf.aszBuf[m_sStrBuf.u32Index] = new BYTE[m_sStrBuf.u32BlockSize];
			if ( m_sStrBuf.aszBuf[m_sStrBuf.u32Index] == NULL )
			{
				TRACE_(_T("String buffer is not enough"));
				return NULL;
			}

			memset(m_sStrBuf.aszBuf[m_sStrBuf.u32Index], 0, m_sStrBuf.u32BlockSize);
			m_sStrBuf.pszPos = m_sStrBuf.aszBuf[m_sStrBuf.u32Index];	
		}

		// Cache string 
		memcpy(m_sStrBuf.pszPos, pszBuf, u32Bytes);
		m_sStrBuf.pszPos += u32Bytes;

		return m_sStrBuf.pszPos - u32Bytes;
	}

	typedef struct tagStringBuf
	{
		UINT	u32BlockSize;					// Block size
		UINT	u32Index;						// String buffer index
		BYTE*	pszPos;							// Append new string start position
		BYTE*	aszBuf[IME_STRING_BUF_COUNT];	// String buffer
	}S_StringBuf, *PS_StringBuf;
	
	typedef std::map<UINT, void* >	MapStr;		// String map
	typedef MapStr::iterator		MapStrIt;	// Iterator of string map, 
	typedef std::pair<UINT, void* >	PairStr;	// Pair of MapStr

	S_StringBuf	m_sStrBuf;
	MapStr		m_mapString;					// Memory string table

public:
	~C_StringMgr(void)
	{
		for ( int i32Index = 0; i32Index < IME_STRING_BUF_COUNT; i32Index++ )
		{
			SAFE_DELETE_PTR_(m_sStrBuf.aszBuf[i32Index]);
		}
	}

	static C_StringMgr& GetObject()
	{
		static C_StringMgr obj;
		return obj;
	}

	const char* AddString(const char* pszString)
	{
		if ( pszString == NULL )
			pszString = "";

		UINT u32Size = strlen(pszString);
		UINT u32Hash = _Hash((const BYTE*)pszString, u32Size);	// Calc hash key
		for (MapStrIt it = m_mapString.find(u32Hash); it != m_mapString.end();)
		{
			if ( strcmp(pszString, (char*)it->second) == 0 )
				return (char*)it->second;

			//	TRACE_(_T("Conflict resolution of hash[0x%08X]: %S <==> (0x%)%S"), u32Hash, pszString, it->second, (char*)it->second);
			u32Hash++;
			it = m_mapString.find(u32Hash);	// conflict resolution
		}

		char* pszValue = (char*)_Add2StringBuf((BYTE*)pszString, u32Size + 1);
		CHECK_NULL_RET_(pszValue, NULL);
		m_mapString.insert(PairStr(u32Hash, pszValue));
		return pszValue;
	}

	const WCHAR* AddString(const WCHAR* pszString)
	{
		if ( pszString == NULL )
			pszString = L"";

		UINT u32Size = wcslen(pszString);
		UINT u32Hash = _Hash((const BYTE*)pszString, u32Size * sizeof(WCHAR));	// Calc hash key
		for (MapStrIt it = m_mapString.find(u32Hash); it != m_mapString.end();)
		{
			if ( _wcsicmp(pszString, (WCHAR*)it->second) == 0 )
				return (WCHAR*)it->second;

			//	TRACE_(_T("Conflict resolution of hash[0x%08X]: %s <==> (0x%p)%s"), u32Hash, pszString, it->second, (WCHAR*)it->second);
			u32Hash++;
			it = m_mapString.find(u32Hash);	// conflict resolution
		}

		WCHAR* pszValue = (WCHAR*)_Add2StringBuf((BYTE*)pszString, (u32Size + 1) * sizeof(WCHAR));
		CHECK_NULL_RET_(pszValue, NULL);
		m_mapString.insert(PairStr(u32Hash, pszValue));
		return pszValue;
	}
};