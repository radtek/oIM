// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: This is a config Read/WriteRedirect configer. setted  
//   lpctszWriteConfigFile when call SetConfiger function, it will enable
//   write redirect function. otherwise, it is a normal configer.
// 
//  When the redirect is enabled, will only write to the redirect config file,
//  but will read from redirect file at first, then read from main config when
//  failed read from redirect file.
// 
// Auth: yfgz
// Date: 2013/6/28 12:46:04
// 
// History: 
//    2013/6/28 yfgz First version, only read/write Int, String type 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <TCHAR.h>

#define	READ_BUFFER_SIZE		(8192)

#define CHECK_PARAMETER_RET(RET) \
	if(lptszSection == NULL || lptszKey == NULL) \
	{ \
		ASSERT_(FALSE); \
		return RET; \
	}

inline C_Configer::C_Configer(void)
{
	m_bWriteRedirect		  = FALSE;
	memset(m_szConfigFile, 0, sizeof(m_szConfigFile));
	memset(m_szRedirectConfigFile, 0, sizeof(m_szRedirectConfigFile));
}

inline C_Configer::~C_Configer(void)
{
}

inline BOOL C_Configer::SetConfiger(
	LPCTSTR lpctszConfigFile, 
	LPCTSTR lpctszWriteConfigFile
	)
{
	if(lpctszConfigFile == NULL) 
		return FALSE;

	_tcsncpy(m_szConfigFile, lpctszConfigFile, MAX_PATH);
	if(lpctszWriteConfigFile)
	{
		m_bWriteRedirect    = TRUE;
		_tcsncpy(m_szRedirectConfigFile, lpctszWriteConfigFile, MAX_PATH);
	}
	else
	{
		m_bWriteRedirect    = FALSE;
		memset(m_szRedirectConfigFile, 0, sizeof(m_szRedirectConfigFile));
	}

	return TRUE;
}

inline const TCHAR* C_Configer::GetConfigFile(
	 BOOL bRedirectConfigFile
	 )
 {
	 if( bRedirectConfigFile )
		 return m_szConfigFile;
	 else 
		 return m_szRedirectConfigFile;
 }

inline int C_Configer::ReadInt(
	LPCTSTR lptszSection, 
	LPCTSTR lptszKey, 
	int		i32Default, 
	BOOL	bRedirect)
{
	CHECK_PARAMETER_RET(i32Default);

	DWORD dwRet = 0;
	TCHAR szReadBuffer[READ_BUFFER_SIZE] = { 0 };
	if( m_bWriteRedirect &&	bRedirect && // First read from redirect config file if redirected
		(dwRet = GetPrivateProfileString(lptszSection, lptszKey, NULL, szReadBuffer, READ_BUFFER_SIZE, m_szRedirectConfigFile)) > 0)
		return _tstoi(szReadBuffer);

	if( (dwRet = GetPrivateProfileString(lptszSection, lptszKey, NULL, szReadBuffer, READ_BUFFER_SIZE, m_szConfigFile)) > 0)
		return _tstoi(szReadBuffer);

	return i32Default;
}

inline BOOL C_Configer::WriteInt(
	LPCTSTR lptszSection,
	LPCTSTR lptszKey, 
	int		i32Value
	)
{
	CHECK_PARAMETER_RET(FALSE);
	
	TCHAR szWriteBuffer[20] = { 0 };
	if(m_bWriteRedirect)
		return !!WritePrivateProfileString(lptszSection, lptszKey, _itot(i32Value, szWriteBuffer, 10), m_szRedirectConfigFile);
	else
		return !!WritePrivateProfileString(lptszSection, lptszKey, _itot(i32Value, szWriteBuffer, 10), m_szConfigFile);
}

inline INT64 C_Configer::ReadInt64(
	LPCTSTR lptszSection,
	LPCTSTR lptszKey, 
	INT64	i64Default,
	BOOL	bRedirect
	)
{
	CHECK_PARAMETER_RET(i64Default);

	TCHAR szReadBuffer[READ_BUFFER_SIZE] = { 0 };
	if( m_bWriteRedirect &&	bRedirect && // First read from redirect config file if redirected
		GetPrivateProfileString(lptszSection, lptszKey, NULL, szReadBuffer, READ_BUFFER_SIZE, m_szRedirectConfigFile) > 0)
		return _ttoi64(szReadBuffer);

	if(GetPrivateProfileString(lptszSection, lptszKey, NULL, szReadBuffer, READ_BUFFER_SIZE, m_szConfigFile) > 0)
		return _ttoi64(szReadBuffer);

	return i64Default;
}

inline BOOL C_Configer::WriteInt64(
	LPCTSTR lptszSection, 
	LPCTSTR lptszKey, 
	INT64	i64Value)
{
	CHECK_PARAMETER_RET(FALSE);
	
	TCHAR szWriteBuffer[20] = { 0 };
	if(m_bWriteRedirect)
		return !!WritePrivateProfileString(lptszSection, lptszKey, _i64tot(i64Value, szWriteBuffer, 10), m_szRedirectConfigFile);
	else
		return !!WritePrivateProfileString(lptszSection, lptszKey, _i64tot(i64Value, szWriteBuffer, 10), m_szConfigFile);
}

inline const TCHAR* C_Configer::ReadString(
	LPCTSTR lptszSection,
	LPCTSTR lptszKey, 
	LPTSTR	lptszBuffer, 
	DWORD*	pdwBufferSize, 
	LPCTSTR lptszDefault, 
	BOOL	bRedirect
	)
{
	CHECK_PARAMETER_RET(lptszDefault);

	if( m_bWriteRedirect &&	bRedirect && // First read from redirect config file if redirected
		GetPrivateProfileString(lptszSection, lptszKey, NULL, lptszBuffer, *pdwBufferSize, m_szRedirectConfigFile) > 0)
		return lptszBuffer;

	if(GetPrivateProfileString(lptszSection, lptszKey, NULL, lptszBuffer, *pdwBufferSize, m_szConfigFile) > 0)
		return lptszBuffer;

	return lptszDefault;
}

inline BOOL C_Configer::WriteString(
	LPCTSTR lptszSection,
	LPCTSTR lptszKey,
	LPCTSTR lptszValue
	)
{	
	CHECK_PARAMETER_RET(FALSE);

	if(m_bWriteRedirect)
		return !!WritePrivateProfileString(lptszSection, lptszKey, lptszValue, m_szRedirectConfigFile);
	else
		return !!WritePrivateProfileString(lptszSection, lptszKey, lptszValue, m_szConfigFile);
}
