// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
//	This is CmdBase, 
// 
// 
// Auth: LongWQ
// Date:  2014/8/6 11:08:40 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_COMMAND_BASE_HEADER_2014_08_06_YFGZ__
#define __EIMENGINE_COMMAND_BASE_HEADER_2014_08_06_YFGZ__
#pragma once

#include "Public\StrUtil.h"
#include "eIMEngine\IeIMCmd.h"
#include "eIMEngine\eIMProtocol.h"
#include "eIMEngine\CmdReflection.h"

// Check abort param and flag, set the result 
#define CMD_ABORT_RET_(PABT) \
	do{ \
		if( ((PABT) && (*PABT)) || (m_dwFlag & CMD_FLAG_ABORT) ) \
		{ \
			TRACE_(_T("Abort cmd:%d"), m_i32CmdId); \
			m_i32Result = EIMERR_ABORT_CMD; \
			return m_i32Result; \
		} \
	}while(0)

// Helper Macro to define default constructor and DoCmd
#define CMD_DECLARE_DEFAULT_(CLS, CMD, FLAG) \
	public: \
		CLS( const CLS& Obj ): T_eIMCmdBase( Obj ){}; \
		CLS( const PTCMD pObj ): T_eIMCmdBase( pObj, CMD, FLAG ) \
		{ \
			TCHAR* pszXml = (TCHAR*)(pObj); \
			if (pszXml && pszXml[0] == _T('<') &&  \
				CMD_TAG == MAKE4CC(pszXml[0], pszXml[1], pszXml[2], pszXml[3]) ) \
				FromXml(pszXml); \
			else \
			{	\
				if ( pObj ) \
				{ \
					memcpy( &m_sData, pObj, sizeof(m_sData) ); \
					ToXml(); \
				} \
				else \
					memset(&m_sData, 0, sizeof(m_sData)); \
			} \
		}

#define CMD_TAG	MAKE4CC(_T('<'), _T('C'), _T('m'), _T('d'))	// "<Cmd"


template< class TCMD >
class __declspec(novtable) T_eIMCmdBase: public I_EIMCmd
{
protected:
	int			m_i32Result;	// DoCmd Result
	int			m_i32CmdId;		// Psuedo or Cmd Id
	DWORD		m_dwFlag;		// Flag of the Cmd	
	ULONG		m_ulRef;		// Reference count of Cmd
	eIMString	m_szXml;		// XML of cmd
	TCMD		m_sData;		// Cmd data
	DWORD		m_dwBusinessId;	// UI business id
	UINT64		m_u64BusinessTypeId;
	UINT32      m_virtualGroupUpdatetime;

	void ResetData()
	{
		m_u64BusinessTypeId = 0;
		m_virtualGroupUpdatetime = 0;
		m_i32Result = EIMERR_NO_ERROR;
		memset( &m_sData, 0, sizeof(m_sData) );
		m_szXml.clear();
	}

public:
	typedef TCMD* PTCMD;

	IMPLEMENT_PLUGIN_REF_(T_eIMCmdBase, INAME_EIMCMD, m_ulRef);

	T_eIMCmdBase() 
		: m_i32CmdId( PROTOCOL_ID_INVALID )
		, m_ulRef( 1 )
		, m_dwFlag( CMD_FLAG_THREAD_ALL )
		, m_dwBusinessId( 0 )
		, m_u64BusinessTypeId( 0 )
	{
		ResetData();
	}

	T_eIMCmdBase( const T_eIMCmdBase& obj )
		: m_ulRef( 1 )
	{
		ResetData();
		m_i32Result = obj.m_i32Result;
		m_i32CmdId  = obj.m_i32CmdId;
		m_dwFlag	= obj.m_dwFlag;
		m_szXml		= obj.m_szXml;
		m_dwBusinessId = obj.m_dwBusinessId;
		m_u64BusinessTypeId = obj.m_u64BusinessTypeId;

		memcpy( &m_sData, &obj.m_sData, sizeof(m_sData) );
	}

	T_eIMCmdBase( const TCMD* pTCmd, int i32CmdId, DWORD dwFlag = CMD_FLAG_THREAD_ALL ) 
		: m_ulRef( 1 )
		, m_dwFlag( dwFlag )
		, m_i32CmdId( i32CmdId )
		, m_i32Result( 0 )
		, m_dwBusinessId( 0 )
		, m_u64BusinessTypeId( 0 )
	{
		ResetData();

	}
	
	virtual ~T_eIMCmdBase()
	{
	}

	virtual int GetResult()
	{
		return m_i32Result;
	}
		
	virtual const TCHAR* GetErrorDesc(int i32Error=0)
	{
		if ( i32Error == EIMERR_NO_ERROR )
			i32Error = GetResult();

		return hpi.GetErrorLangInfo(i32Error);
	}

	virtual int	 GetCmdId()
	{
		return m_i32CmdId;
	}
	
	virtual const TCMD* GetCmdData()
	{
		return &m_sData;
	}
	
	virtual const void* GetData()
	{
		return (void*)GetCmdData();
	}

	virtual const TCHAR*GetXml()
	{
		return m_szXml.c_str();
	}

	virtual DWORD GetFlag(DWORD dwMask = 0xFFFFFFFF)
	{
		return (m_dwFlag & dwMask);
	}
	
	virtual DWORD SetFlag(DWORD dwFlag, DWORD dwMask = 0xFFFFFFFF)
	{
		if ( dwFlag & dwMask )
		{	// Set flags
			m_dwFlag |= (dwFlag & dwMask);
		}
		else
		{	// Clear flags
			m_dwFlag &= ~dwMask;
		}

		return GetFlag(dwMask);
	}
		

	virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL)
	{
		TRACE_(_T("Cmd[%d] not implementated command operate!"), m_i32CmdId);
		return EIMERR_NOT_IMPL;
	}

	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort = NULL)
	{
		TRACE_(_T("Cmd[%d] not implementated database operate!"), m_i32CmdId);
		return EIMERR_NOT_IMPL;
	}

	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pIBIFile, BOOL* pbAbort = NULL)
	{
		TRACE_(_T("Cmd[%d] not implementated notify operate!"), m_i32CmdId);
		return EIMERR_NOT_IMPL;
	}

	virtual const TCHAR* ToXml()
	{ 
		if (!(m_dwFlag & CMD_FLAG_NO_TOXML))
			TRACE_(_T("Cmd[%d] not implementated ToXml()"), m_i32CmdId); 

		return NULL; 
	} 

	virtual BOOL FromXml( const TCHAR* pszXml ) 
	{ 
		if (!(m_dwFlag & CMD_FLAG_NO_FROMXML))
			TRACE_(_T("Cmd[%d] not implementated FromXml()"), m_i32CmdId); 

		return FALSE;
	}

	virtual TCHAR* GetAttribute(UINT64 u64AttrId)
	{
		TRACE_(_T("Cmd[%d] not implementated GetAttribute(%llu)"), m_i32CmdId, u64AttrId); 
		return NULL;
	}

	virtual BOOL SetAttribute(UINT64 u64AttrId, TCHAR* pszValue)
	{
		TRACE_(_T("Cmd[%d] not implementated SetAttribute(%llu)"), m_i32CmdId, u64AttrId); 
		return FALSE;
	}

	void ConvertToXml(const TiXmlElement& e, eIMString& szXml )
	{
		eIMStringA oStr;

		oStr << e ;
		::eIMUTF8ToTChar(oStr.c_str(), szXml);
	}

	void SetBusinessId(DWORD dwBusinessId)
	{
		m_dwBusinessId = dwBusinessId;
	}

	DWORD GetBusinessId()
	{
		return m_dwBusinessId;
	}

	virtual void SetBusinessTypeId(UINT64 u64BusinessTypeId)
	{
		m_u64BusinessTypeId = u64BusinessTypeId;
	}

	virtual UINT64 GetBusinessTypeId()
	{
		return m_u64BusinessTypeId;
	}

};

// URL Encode/Decode helper class
#define URL_X_SPACE_FLAG	( 0x00000001 )	 // ' ' ==> %20 or %20 ==>' '
template< class T >
class C_UrlHelper
{
private:
	DWORD	m_dwFlag;
	T*		m_pszUrl;

	T _ToHex(T chCode) 
	{
		static const char s_Hex_Chars[] = "0123456789abcdef";
		return s_Hex_Chars[chCode & 15];
	}

	T _ToInt(T chCode) 
	{
		return isdigit(chCode) ? chCode - '0' : tolower(chCode) - 'a' + 10;
	}

public:
	C_UrlHelper(DWORD dwFlag=0)
		: m_dwFlag(dwFlag)
		, m_pszUrl(NULL)
	{
	}
	~C_UrlHelper()
	{
		SAFE_DELETE_PTR_(m_pszUrl);
	}

	const T* GetUrl()
	{
		return m_pszUrl;
	}

	const T* Encode(const T* pszBuf)
	{
		SAFE_DELETE_PTR_(m_pszUrl);
		if (pszBuf == NULL || pszBuf[0] == '\0')
			return NULL;

		const int i32Size = (sizeof(T) == 1 ? strlen((const char*)pszBuf) : wcslen((const wchar_t*)pszBuf)) * 2 + 1;
		m_pszUrl = new T[i32Size];
		CHECK_NULL_RET_(m_pszUrl, NULL);
		memset(m_pszUrl, 0, sizeof(T) * i32Size);
		T* pszUrl    = m_pszUrl;
		const T* pszBufPtr = pszBuf;
		while (*pszBufPtr) 
		{
			if (isalnum(*pszBufPtr)) 
			{ 
				*pszUrl++ = *pszBufPtr;
			} 
			else if (!(URL_X_SPACE_FLAG & m_dwFlag) && *pszBufPtr == ' ') 
			{
				*pszUrl++ = '+';
			} 
			else 
			{
				*pszUrl++ = '%'; 				
				*pszUrl++ = _ToHex(*pszBufPtr >> 4);
				*pszUrl++ = _ToHex(*pszBufPtr & 15);
			}

			pszBufPtr++;
		}

		return m_pszUrl;
	}

	const T* Decode(const T* pszBuf)
	{
		SAFE_DELETE_PTR_(m_pszUrl);
		if (pszBuf == NULL || pszBuf[0] == '\0')
			return NULL;

		const int i32Size = (sizeof(T) == 1 ? strlen((const char*)pszBuf) : wcslen((const wchar_t*)pszBuf)) + 1;
		m_pszUrl = new T[i32Size];
		CHECK_NULL_RET_(m_pszUrl, NULL);
		memset(m_pszUrl, 0, sizeof(T) * i32Size);
		T* pszUrl    = m_pszUrl;
		const T* pszBufPtr = pszBuf;
		while (*pszBufPtr) {
			if (*pszBufPtr == '%') {
				if (pszBufPtr[1] && pszBufPtr[2]) {
					*pszUrl++ = _ToInt(pszBufPtr[1]) << 4 | _ToInt(pszBufPtr[2]);
					pszBufPtr += 2;
				}
			} else if (!(URL_X_SPACE_FLAG & m_dwFlag) && *pszBufPtr == '+') { 
				*pszUrl* = ' ';
			} else {
				*pszUrl++ = *pszBufPtr;
			}

			pszBufPtr++;
		}

		return m_szUrl;
	}

};

#endif //__EIMENGINE_COMMAND_BASE_HEADER_2014_08_06_YFGZ__