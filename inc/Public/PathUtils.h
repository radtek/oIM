// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Path and Token helper function
// 
// 
// 
// Auth: LongWQ
// Date: 2013/12/21 19:12:15
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

// PathUtils.h
#ifndef __EIM_PATHUTIL_API_2013_12_12_BY_YFGZ__
#define __EIM_PATHUTIL_API_2013_12_12_BY_YFGZ__

#include "StrUtil.h"

#define TOKEN_MAX_SIZE			(4 * 1024)			// 4KB max of token
//Token ///////////////////////////////////////////////////////////////////////////
// The simplest usage:
// eIMString szPluginPath = PATH_TOKEN_PLUGIN;
// eIMReplaceToken(szPluginPath);	// The szPluginPath will be store the full path of $(Plugins)

#define PATH_TOKEN_EXE			_T("$(Exe)")		// Application location, ref: PATH_TYPE_EXE
#define PATH_TOKEN_PLUGIN		_T("$(Plugins)")	// Plugin location, ref: PATH_TYPE_PLUGIN
#define PATH_TOKEN_APPMAN		_T("$(AppMan)")		// App manage location, ref: PATH_TYPE_APPMAN
#define PATH_TOKEN_EMOTICON		_T("$(Emoticon)")	// Emoticon location, ref: PATH_TYPE_EMOTICON
#define PATH_TOKEN_APPDATA		_T("$(eIMData)")	// Application data location, ref: PATH_TYPE_APPDATA
#define PATH_TOKEN_APPDOC		_T("$(eIMFiles)")	// Application document location, ref: PATH_TYPE_APPDOC
#define PATH_TOKEN_LOG			_T("$(Logs)")		// Log file location, ref: PATH_TYPE_LOG
#define PATH_TOKEN_LOGINID		_T("$(LoginId)")	// *** This token can NOT be getted before LoginEnd, it is the EID that save as '$(LoginId)' within app environment variable.
#define PATH_TOKEN_LOGINID_DOC	_T("$(LoginIdDoc)")	// *** This token can NOT be getted before LoginEnd, it is the EID that save as '$(LoginId)' within app environment variable.
#define PATH_TOKEN_LOGINNAME	_T("$(LoginName)")	// *** This token can NOT be getted before Login, it is the login name save as '$(LoginName)' within app environment variable.

// Help micro for path token
#define PATH_TOKEN_EXE_FILE_(FILE)			PATH_TOKEN_EXE		   FILE
#define PATH_TOKEN_PLUGIN_FILE_(FILE)		PATH_TOKEN_PLUGIN	   FILE
#define PATH_TOKEN_APPMAN_FILE_(FILE)		PATH_TOKEN_APPMAN	   FILE
#define PATH_TOKEN_EMOTICON_FILE_(FILE)		PATH_TOKEN_EMOTICON    FILE
#define PATH_TOKEN_APPDATA_FILE_(FILE)		PATH_TOKEN_APPDATA     FILE
#define PATH_TOKEN_APPDOC_FILE_(FILE)		PATH_TOKEN_APPDOC	   FILE
#define PATH_TOKEN_LOG_FILE_(FILE)			PATH_TOKEN_LOG		   FILE
#define PATH_TOKEN_LOGINID_FILE_(FILE)		PATH_TOKEN_LOGINID	   FILE	
#define PATH_TOKEN_LOGINID_DOC_FILE_(FILE)	PATH_TOKEN_LOGINID_DOC FILE	
//#define PATH_TOKEN_LOGINNAME_FILE_(FILE)	PATH_TOKEN_LONINNAME _T("\\") FILE	

int eIMReplaceToken( eIMString& szValue);
int eIMReplaceTokenAs( eIMString& szValue, const TCHAR* pszToken, const TCHAR* pszTo);
int eIMGetLogFile( eIMString& szLogFile, const TCHAR* pszPrefix, const TCHAR* pszPostfix);


// These internal path will be created in App initial
#define PATH_TYPE_EXE					_T("Exe")			// Application location ( eg.: .\ )
#define PATH_TYPE_PLUGIN				_T("Plugins")		// Plugin location (eg.: .\Plugins )
#define PATH_TYPE_APPMAN				_T("AppMan")		// App manage location ( eg.: .\AppMan )
#define PATH_TYPE_EMOTICON				_T("Emoticon")		// Emoticon location (eg.: .\Emoticon or $(LoginId)\Emoticon)
#define PATH_TYPE_SKIN					_T("Skin")			// Skin location (eg.: .\Skin or $(LoginId)\Skin)
#define PATH_TYPE_FACE					_T("Face")			// Face location (eg.: $(LoginId)\Face )
#define PATH_TYPE_APPDATA				_T("eIMData")		// Application data location ( eg.: %APPDATA%\eIM Data )
#define PATH_TYPE_APPDOC				_T("eIMFiles")		// Application document location ( eg.: %HOMEPATH%\eIM Files )
#define PATH_TYPE_APPDOC_VEDIO			_T("Vedio")			// Vedio location (eg.: $(LoginIdDoc)\$(QSID)\Vedio
#define PATH_TYPE_APPDOC_AUDIO			_T("Audio")			// Audio location (eg.: $(LoginIdDoc)\$(QSID)\Audio
#define PATH_TYPE_APPDOC_IMAGE			_T("Image")			// Image location (eg.: $(LoginIdDoc)\$(QSID)\Image
#define PATH_TYPE_APPDOC_FILERECV		_T("FileRecv")		// FileRecv location (eg.: $(LoginIdDoc)\$(QSID)\FileRecv
#define PATH_TYPE_APPDOC_LOGO			_T("Logo")			// Session Logo
#define PATH_TYPE_LOG					_T("Logs")			// Application log file location (eg.:  $(eIMData)\Log )
#define PATH_TYPE_LOGINID				_T("LoginId")		// User logined id location(eg.: $(eIMData)\123 )
#define PATH_TYPE_LOGINID_DOC			_T("LoginIdDoc")	// User logined id location(eg.: $(eIMFiles)\123 )
#define PATH_TYPE_LOGINID_DOC_GRAYFACE	_T("GrayFace")		// User logined id location(eg.: $(eIMData)\GrayFace )
#define PATH_TYPE_LOGINID_DOC_USERFACE	_T("UserFace")		// User logined id location(eg.: $(eIMFiles)\$(LoginId)\UserFace\123.jpg )
#define PATH_TYPE_LOGINNAME				_T("LoginName")		// User logined id location(eg.: $(Logs)\[$(LoginName)]_20140701.log ), used for log file name
#define PATH_TYPE_SCREENSHOT			_T("Screenshot")	// Screen shot folder, parent: PATH_TOKEN_LOGINID_DOC
#define PATH_TYPE_MSGLONG				_T("MsgLong")		// Message long file folder, parent: PATH_TOKEN_LOGINID_DOC
#define PATH_TYPE_CONTACTS_TMP			_T("~1")			// Temp folder for contacts download

DWORD	eIMGetPath(LPCTSTR lpszPathType, LPTSTR lpszPath, DWORD nSize);

BOOL	eIMPathFileExists(LPCTSTR lpszPath);
BOOL	eIMPathFileExistNoEmpty(LPCTSTR lpszPath);
BOOL	eIMGetSpecialFolderPath(LPTSTR lpszPath, int i32Folder);
BOOL	eIMSplitPathFile(LPTSTR lpszPathFile, LPTSTR* lppszFile, LPTSTR* lppszExt);
DWORD	eIMGetTempPath( DWORD dwSize, LPTSTR lpszBuffer);
DWORD	eIMGetTempFileName( LPCTSTR lpszPathName, LPTSTR lpszTempFileName);
DWORD	eIMGetModuleFilePath(LPCTSTR lpszModuleName, LPTSTR lpszFilePath, DWORD dwSize);
DWORD	eIMNormalPathFile( LPCTSTR lpszFileName, LPTSTR lpszBuffer, DWORD dwSize);
DWORD	eIMAddPath( LPTSTR lpszPath, LPCTSTR lpszAdd);
BOOL	eIMMakeDir( LPCTSTR lpszPath);								// Create multi-level directory
DWORD	eIMGetSessionDocDir(eIMString& szDir, QSID qsid);			// Get the session's sub directory of $(LoginIdDoc), created it when not exist
DWORD	eIMGetFullPath(LPCTSTR lpszTokenPath, LPCTSTR lpszSubDir, LPCTSTR lpszFile, eIMString& szFullPath, BOOL bRename = TRUE);
BOOL	eIMGetGrayImgPath(LPTSTR lpszPath,DWORD nSize);

#include "PathUtils.inl"

#endif // __EIM_PATHUTIL_API_2013_12_12_BY_YFGZ__