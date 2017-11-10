// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
// C_XmlLang is a XML file read configurator to get attributes(just read only)
//////////////////////////////////////////////////////////////////////////////
// eg.:
//    C_XmlLang XmlLang;
//    
//    eIMString szConfig = _T("$(Exe)\\Languages\\Languages.config");
//    
//    eIMReplaceToken(szConfig);
//    
//    if ( XmlLang.Open(szConfig.c_str(), _T("CN")) )
//	  {
//			XmlLang.GetLangAttr(_T("InvalidCmdLine"), szValue);
//			XmlLang.Close();
//	   }
// 
// Auth: LongWQ
// Date:  2014/1/7 16:53:50 
// 
// History: 
//    2014/1/7 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_XMLLang_API_2013_12_31_BY_YFGZ__
#define __EIM_XMLLang_API_2013_12_31_BY_YFGZ__

#define	LANG_ROOT_VALUE				"LangRoot"
#define LANG_ATTRIBUTE_NAME			"Name"
#define	LANG_ATTRIBUTE_ID			"Id"
#define	LANG_ATTRIBUTE_VALUE		"Value"

#include "XmlConfiger.h"
#include <map>

using std::map;
using std::pair;

class C_XmlLang: public C_XmlConfiger
{
public:
	C_XmlLang();
	virtual ~C_XmlLang();

public:
	//=============================================================================
	//Function:     Open
	//Description:	Open XML format configuration file
	//
	//Parameter:
	//	lpszFile        - Main config file, readonly
	//	lpszLanguage    - Which Language you want to lode			  
	//
	//Return:
	//  See error code of EIMERR_*  
	//  EIMERR_NO_ERROR			Succeed
	//	EIMERR_INVALID_PARAM	Invalid parameter 
	//  EIMERR_FILE_NOT_EXIST	File not exist
	//  EIMERR_OPEN_FILE_FAILED	Open failed
	//	EIMERR_FILE_NOT_CONFORM File not conform to requirement
	//  EIMERR_LANG_NOT_EXIST	Language not exist
	//=============================================================================
	int Open( LPCTSTR lpszFile, LPCTSTR lpszLanguage );

	//=============================================================================
	//Function:     GetLangAttr
	//Description:	Get the Lang attribute
	//
	//Parameter:
	//	dwId			- Error Id
	//
	//Return:
	//		NULL		- Failed
	//=============================================================================
    LPCTSTR GetLangAttr( const DWORD dwId );

	//=============================================================================
	//Function:     GetLangAttr
	//Description:	Get the Lang attribute
	//
	//Parameter:
	//	dwId			- Error Id
	//  lpcszDesc		- Error Desc
	//
	//Return:
	//		NULL		- Failed
	//=============================================================================    
	LPCTSTR SetLangAttr( const DWORD dwId, LPCTSTR lpcszDesc);

protected:
	//=============================================================================
	//Function:     LoadLangAttr
	//Description:	Load all string attribute according to pszLang
	//
	//Parameter:
	//	lpszPath        - Full path, see: _GetElementByPath
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed
	//=============================================================================
	BOOL LoadLangAttr( const TCHAR*	pszPath );

	//=============================================================================
	//Function:     IsExistLang
	//Description:	Judge if it exist this language
	//
	//Parameter:
	//	pszLang			- Language
	//
	//Return:
	//		if exist return the index of language
	//		else return -1
	//=============================================================================
	int IsExistLang( const TCHAR* pszLang );

private:
	//<ID, value>
	typedef std::map<DWORD, eIMString> MapErrLang;
	typedef MapErrLang::iterator	   MapErrLangIt;	
	
	MapErrLang	m_mapErrLang;

};

#include "XmlLang.inl"

#endif  // __EIM_XMLLang_API_2014_1_7_BY_YFGZ__