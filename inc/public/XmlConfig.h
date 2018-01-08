// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
// C_XmlConfig is a XML file read/write(redirect) configurator
// For compatible with VISTA and later OS, write can be redirect if configured
//////////////////////////////////////////////////////////////////////////////
// eg.:
//    C_XmlConfig XmlConfiger;
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

#ifndef __EIM_XMLCONFIGER_API_2017_12_22_BY_YFGZ__
#define __EIM_XMLCONFIGER_API_2017_12_22_BY_YFGZ__

#include <pugixml\pugixml.hpp>
#include <string\tstring.h>
#define TYPE_ELEMENT_NAME_SIZE	( 64 )	// XML configuration file element name MAX size

class C_XmlConfig
{
private:
	bool			m_bModify;			// Modify flag
	bool			m_bOpen;			// Open flag
	bool			m_bRedirect;		// Redirect flag
	SStringT		m_szSaveFile;

protected:
	pugi::xml_document	m_XmlCfg;		// Configuration file
	pugi::xml_document	m_XmlCfgRedi;	// Configuration redirect file

	bool _Open( const TCHAR* pszFile, pugi::xml_document& doc, const char* pszPsw);

public:
	C_XmlConfig(void);
	virtual ~C_XmlConfig(void);
	pugi::xml_node GetRoot();
	pugi::xml_node GetNode(pugi::xml_node node, LPCTSTR lpszPath, int i32Level, bool bCreate);

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
	virtual int Open(LPCTSTR lpszFile, LPCTSTR lpszRediFile = NULL, bool bCreate = false, const char* pszPsw="yf*gz@16!03$10");

	//=============================================================================
	//Function:     Save
	//Description:	Save modify, save to main config when not redirection, else save
	//				to redirection file
	//
	//
	//Return:
	//       true	- Saved
	//		 false	- Save failed
	//=============================================================================
	virtual inline bool Save(LPCTSTR lpszFile=NULL);

	//=============================================================================
	//Function:     Close
	//Description:	Close configruation, will be auto save when modified
	//
	//
	//Return:
	//       true	- Closed
	//		 false	- Close failed
	//=============================================================================
	virtual inline bool Close();

	//=============================================================================
	//Function:     IsOpen
	//Description:	Get the open status
	//
	//
	//Return:
	//		true	- Opened
	//		false	- Not open
	//=============================================================================
	virtual inline bool IsOpen();

	//=============================================================================
	//Function:     IsModify
	//Description:	Get the modify flag
	//
	//
	//Return:
	//		true	- Modified
	//		false	- Not modify
	//=============================================================================
	virtual inline bool IsModify();

	//=============================================================================
	//Function:     IsRedirect
	//Description:	Get the Redirect flag
	//
	//
	//Return:
	//		true	- Redirected
	//		false	- Not Redirect
	//=============================================================================
	virtual inline bool IsRedirect();

	//=============================================================================
	//Function:     SetModify
	//Description:	Set modify flag
	//
	//Parameter:
	//	bModified    - true: Modified
	//				   false: Not modify
	//
	//Return:
	//      TRUE: Original is modified
	//		FALSE: Original is not modified
	//=============================================================================
	virtual inline bool SetModify(bool bModified = true);

	//=============================================================================
	//Function:     GetAttributeStr
	//Description:	Get string attribute
	//
	//Parameter:
	//	lpszPath        - Full path, see: _GetElementByPath
	//	lpszAttrName    - Attribute name
	//	pszDefault		- Input Default attribute
	//	bRedirect       - FALSE to forbidden redirection
	//
	//Return:
	//		>=0     The value's data size
    //      <0      Is the real need buffer size of absolute value, you need relocate the buffer and recall it.
	//=============================================================================
	virtual LPCTSTR GetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR pszDefault = _T(""), bool bEnableRedirect = true );

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
	//		true		- Succeeded
	//		false		- Falied
	//=============================================================================
	virtual bool SetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue );

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
	virtual int GetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default = 0, bool bEnableRedirect = true  );

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
	virtual bool SetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value );

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
	virtual bool Remove( LPCTSTR lpszPath );
	
	virtual LPCTSTR GetText( LPCTSTR lpszPath, LPTSTR pszDefault, bool bEnableRedirect = true );
	virtual bool SetText( LPCTSTR lpszPath, LPCTSTR lpszText );

};

#include "XmlConfig.inl"

#endif  // __EIM_XMLCONFIGER_API_2017_12_22_BY_YFGZ__