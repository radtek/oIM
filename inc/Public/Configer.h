// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
// C_Configer is a INI file read/write(redirect) configurator
// For compatible with VISTA and later OS, write can be redirect if configured
//////////////////////////////////////////////////////////////////////////////
// eg.:
//    C_Configer Configer;
//    
//    TCHAR szConfig[MAX_PATH] = { 0 };
//    TCHAR szRedirectConfig[MAX_PATH] = { 0 };
//    
//    eIMGetPath(PATH_TYPE_EXE, szConfig, MAX_PATH);
//    eIMPathAdd(szConfig, _T("eIM.config"), szConfig, MAX_PATH);
//    
//    eIMGetPath(PATH_TYPE_APPDATA, szRedirectConfig, MAX_PATH);
//    eIMPathAdd(szRedirectConfig, _T("eIM.config"), szRedirectConfig, MAX_PATH);
//    
//    Configer.SetConfiger(szConfig, szRedirectConfig);
//    
//    int i32Version = Configer.ReadInt(_T("eIM"), _T("Version"), 100);
//    //....
// 
// Auth: LongWQ
// Date: 2013/12/21 19:14:57
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
#ifndef __EIM_CONFIGER_API_2013_12_12_BY_YFGZ__
#define __EIM_CONFIGER_API_2013_12_12_BY_YFGZ__

#pragma once
#include <string>

class C_Configer
{
private:
	BOOL	m_bWriteRedirect;					// Write redirect flag
	TCHAR	m_szConfigFile[MAX_PATH];			// Save the config file path
	TCHAR	m_szRedirectConfigFile[MAX_PATH];	// Save the redirected config file path

public:
	C_Configer(void);
	~C_Configer(void);

	//=============================================================================
	//Function:     SetConfiger
	//Description:	Set Configer file path
	//
	//Parameter:
	//	lpctszConfigFile         - Main config file, readonly
	//	lpctszWriteConfigFile    - Non-NULL: Write redirect config file, write to config
	//                             NULL:	 Forbidden the redirect function
	//Return:
	//  TRUE	- Successed
	//  FALSE	- Failed
	//=============================================================================
	BOOL	SetConfiger(
		LPCTSTR lpctszConfigFile,				// Config file path
		LPCTSTR lpctszWriteConfigFile			// Redirected config file path
		);	

	//=============================================================================
	//Function:     GetConfigFile
	//Description:	Get Configer file path
	//
	//Parameter:
	//	bRedirectConfigFile		- Whether get the redirect config file.
	//
	//Return:
	//  Return the config file name
	//=============================================================================
 	const TCHAR* GetConfigFile(
		BOOL	bRedirectConfigFile = FALSE		// The main or redireced config file
		);	

	//=============================================================================
	//Function:     ReadInt
	//Description:	Read integral value, read first from Redirect config file when is
	//				redirected, then read from main config file when not set in redirect file.
	//
	//Parameter:
	//	lptszSection    - Section name
	//	lptszKey        - Key name
	//	i32Default      - Default value
	//	bRedirect		- TRUE to read main config file at first, else read redirect config file at first
	//Return:
	//  Default		- When can not read from config file
	//  ReadValue	- Read from config file
	//=============================================================================
	int ReadInt(
		LPCTSTR lptszSection, 
		LPCTSTR lptszKey,
		int		i32Default = 0,
		BOOL	bRedirect = TRUE
		);

	//=============================================================================
	//Function:     WriteInt
	//Description:	Write integral value to config file
	//
	//Parameter:
	//	lptszSection    - Section name
	//	lptszKey        - Key name
	//	i32Value        - Value to write
	//
	//Return:
	//  TRUE	- Successed
	//  FALSE	- Failed
	//=============================================================================
	BOOL	WriteInt(
		LPCTSTR lptszSection,
		LPCTSTR lptszKey,
		int		i32Value
		);

	//=============================================================================
	//Function:     ReadInt64
	//Description:	Read integral value, read first from Redirect config file when is
	//				redirected, then read from main config file when not set in redirect file.
	//
	//Parameter:
	//	lptszSection    - Section name
	//	lptszKey        - Key name
	//	i32Default      - Default value
	//	bRedirect		- TRUE to read main config file at first, else read redirect config file at first
	//Return:
	//  Default		- When can not read from config file
	//  ReadValue	- Read from config file
	//=============================================================================
	INT64 ReadInt64(
		LPCTSTR lptszSection, 
		LPCTSTR lptszKey,
		INT64	i64Default = 0,
		BOOL	bRedirect = TRUE
		);

	//=============================================================================
	//Function:     WriteInt64
	//Description:	Write integral value to config file
	//
	//Parameter:
	//	lptszSection    - Section name
	//	lptszKey        - Key name
	//	i32Value        - Value to write
	//
	//Return:
	//  TRUE	- Successed
	//  FALSE	- Failed
	//=============================================================================
	BOOL WriteInt64(
		LPCTSTR lptszSection,
		LPCTSTR lptszKey,
		INT64	i64Value
		);
	
	//=============================================================================
	//Function:     ReadString
	//Description:	Read string value, read first from Redirect config file when is
	//				redirected, then read from main config file when not set in redirect file.
	//
	//Parameter:
	//	lptszSection    - Section name
	//	lptszKey        - Key name
	//  lptszBuffer		- Buffer to return the value
	//	pdwBufferSize	- In buffer size, return the real size
	//	lptszDefault    - Default value
	//	bRedirect		- TRUE to read main config file at first, else read redirect config file at first
	//
	//Return:
	//  Default		- When can not read from config file
	//  ReadValue	- Read from config file
	//=============================================================================
	const TCHAR* ReadString(
		LPCTSTR lptszSection, 
		LPCTSTR lptszKey, 
		LPTSTR	lptszBuffer, 
		DWORD*	pdwBufferSize,
		LPCTSTR lptszDefault = NULL,
		BOOL	bRedirect = TRUE
		);

	//=============================================================================
	//Function:     WriteString
	//Description:	
	//
	//Parameter:
	//	lptszSection    - Section name
	//	lptszKey        - Key name
	//	lptszValue      - Value to write
	//
	//Return:
	//  TRUE	- Successed
	//  FALSE	- Failed
	//=============================================================================
	BOOL WriteString(
		LPCTSTR lptszSection,
		LPCTSTR lptszKey, 
		LPCTSTR lptszValue
		);
};


#include "Configer.inl"
#endif // __EIM_CONFIGER_API_2013_12_12_BY_YFGZ__