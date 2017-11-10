// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
// C_XmlConfiger is a XML file read/write(redirect) configurator
// For compatible with VISTA and later OS, write can be redirect if configured
//////////////////////////////////////////////////////////////////////////////
// eg.:
//    C_XmlConfiger XmlConfiger;
//    
//    eIMString szConfig = _T("$(Exe)\\App.config");
//    eIMString szConfigRedi = _T("$(eIMData)\\App.config");
//    
//    eIMReplaceToken(szConfig);
//	  eIMReplaceToken(szConfigRedi);
//    
//    if ( XmlConfiger.Open(szConfig.c_str(), szConfigRedi.c_str()) )
//	  {
//    
//			TCHAR* pszName = XmlConfiger.GetAttributeStr(_T("eIMApp\\P2P"), _T("IP"));
//			int i32Port    = XmlConfiger.GetAttributeInt(_T("eIMApp\\P2P"), _T("Port"));
//			//....
//			XmlConfiger.Close();
//	   }
// 
// Auth: LongWQ
// Date: 2013/12/21 19:15:29
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_XMLCONFIGER_API_2013_12_12_BY_YFGZ__
#define __EIM_XMLCONFIGER_API_2013_12_12_BY_YFGZ__

#include "StrUtil.h"
#include "TinyXml\tinyxml-lib.h"

#define TYPE_ELEMENT_NAME_SIZE	( 64 )	// XML configuration file element name MAX size
#define XML_VALUE_SIZE_MAX		(4096)  // Config value max value default, you need check the return value of GetAttributeStr when greater than it.

class C_XmlConfiger 
{
private:
	BOOL			m_bModify;			// Modify flag
	BOOL			m_bOpen;			// Open flag
	BOOL			m_bRedirect;		// Redirect flag
	BOOL			m_bUtf8;			// UTF-8 Flag
	eIMString		m_szSaveFile;
protected:

	TiXmlDocument	m_XmlConfiger;		// Configuration file
	TiXmlDocument	m_XmlConfigerRedi;	// Configuration redirect file

	int				_GetSubElementName( eIMString& szPath, eIMString& szElement );
	int				_GetSubElementCount( TiXmlDocument& xmlDoc, LPCTSTR lpszPath, LPCTSTR pszName );
	int				_GetSubElementCount( TiXmlElement* pElementParent, LPCTSTR pszName );
	TiXmlElement*	_GetElementByPath( TiXmlDocument& xmlDoc, LPCTSTR lpszPath, BOOL bCreate = FALSE );

	BOOL			_GetAttribute(const TiXmlElement* pElement, const TCHAR* pszName, eIMString& szAttr );	// Set the attribute, convert to UTF-8 when is UTF-8 format
	BOOL			_SetAttribute(TiXmlElement* pElement, const TCHAR* pszName, const TCHAR* pszAttr );	// convert to TCHAR when is UTF-8 format

public:
	C_XmlConfiger(void);
	virtual ~C_XmlConfiger(void);
	TiXmlDocument&	_GetTiXmlDocument();

public:
	//=============================================================================
	//Function:     Open
	//Description:	Open XML format configuration file
	//
	//Parameter:
	//	lpszFile        - Main config file, readonly when lpszRediFile is valid
	//	lpszRediFile    - Non-NULL: Write redirect to config file
	//					  NULL: Forbidden the redirect function
	//
	//Return:
	//  See error code of EIMERR_*  
	//  EIMERR_NO_ERROR			Succeed
	//	EIMERR_INVALID_PARAM	Invalid parameter 
	//  EIMERR_FILE_NOT_EXIST	File not exist
	//  EIMERR_OPEN_FILE_FAILED	Open failed
	//=============================================================================
	virtual int Open(LPCTSTR lpszFile, LPCTSTR lpszRediFile = NULL, BOOL bCreate = FALSE);

	//=============================================================================
	//Function:     Save
	//Description:	Save modify, save to main config when not redirection, else save
	//				to redirection file
	//
	//
	//Return:
	//       TRUE	- Saved
	//		 FALSE	- Save failed
	//=============================================================================
	virtual inline BOOL Save(LPCTSTR lpszFile=NULL);

	//=============================================================================
	//Function:     Close
	//Description:	Close configruation, will be auto save when modified
	//
	//
	//Return:
	//       TRUE	- Closed
	//		 FALSE	- Close failed
	//=============================================================================
	virtual inline BOOL Close();

	//=============================================================================
	//Function:     IsOpen
	//Description:	Get the open status
	//
	//
	//Return:
	//		TRUE	- Opened
	//		FALSE	- Not open
	//=============================================================================
	virtual inline BOOL IsOpen();

	//=============================================================================
	//Function:     IsModify
	//Description:	Get the modify flag
	//
	//
	//Return:
	//		TRUE	- Modified
	//		FALSE	- Not modify
	//=============================================================================
	virtual inline BOOL IsModify();

	//=============================================================================
	//Function:     IsRedirect
	//Description:	Get the Redirect flag
	//
	//
	//Return:
	//		TRUE	- Redirected
	//		FALSE	- Not Redirect
	//=============================================================================
	virtual inline BOOL IsRedirect();

	//=============================================================================
	//Function:     SetModify
	//Description:	Set modify flag
	//
	//Parameter:
	//	bModified    - TRUE: Modified
	//				   FALSE: Not modify
	//
	//Return:
	//      TRUE: Original is modified
	//		FALSE: Original is not modified
	//=============================================================================
	virtual inline BOOL SetModify(BOOL bModified = TRUE);

	//=============================================================================
	//Function:     GetAttributeStr
	//Description:	Get string attribute
	//
	//Parameter:
	//	lpszPath        - Full path, see: _GetElementByPath
	//	lpszAttrName    - Attribute name
	//	szValue			- Input Default attribute, return the value
    //  i32Size         - Buffer size of pszValue
	//	bRedirect       - FALSE to forbidden redirection
	//
	//Return:
	//		>=0     The value's data size
    //      <0      Is the real need buffer size of absolute value, you need relocate the buffer and recall it.
	//=============================================================================
	virtual int GetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, TCHAR* pszValue, int i32Size, BOOL bEnableRedirect = TRUE );

	//=============================================================================
	//Function:     SetAttributeStr
	//Description:	Set string attribute
	//
	//Parameter:
	//	lpszPath        - Full path, see: _SetElementByPath
	//	lpszAttrName    - Attribute name
	//	lpszValue       - Attribute value
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Falied
	//=============================================================================
	virtual BOOL SetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue );

	//=============================================================================
	//Function:     GetAttributeInt
	//Description:	Get integet attribute
	//
	//Parameter:
	//	lpszPath        - Full path, see: _GetElementByPath
	//	lpszAttrName    - Attribute name
	//	i32Default      - Default attribute
	//	bRedirect       - FALSE to forbidden redirection
	//
	//Return:
	//		Attribute value                
	//=============================================================================
	virtual int GetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default = 0, BOOL bEnableRedirect = TRUE  );

	//=============================================================================
	//Function:     SetAttributeInt
	//Description:	Set integer attribute
	//
	//Parameter:
	//	lpszPath        - Full path, see: _SetElementByPath
	//	lpszAttrName    - Attribute name
	//	i32Value        - Attribute value
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Falied
	//=============================================================================
	virtual BOOL SetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value );

	//=============================================================================
	//Function:     Remove
	//Description:	Remove the specified path element
	//
	//Parameter:
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed
	//=============================================================================
	virtual BOOL Remove( LPCTSTR lpszPath );

	virtual int  GetText( LPCTSTR lpszPath, TCHAR* pszText, int i32Size, BOOL bEnableRedirect = TRUE );
	virtual BOOL SetText( LPCTSTR lpszPath, LPCTSTR lpszText );

};

#include "XmlConfiger.inl"

#endif  // __EIM_XMLCONFIGER_API_2013_12_12_BY_YFGZ__