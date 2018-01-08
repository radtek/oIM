// NOTICE:
//	Please DON'T modify this class, if you have any requirement to implement, PLEASE
//  inherit it and implement it.
//
// Description: C_XmlConfig is a helper class to read and write standard XML
//   configuration file. it only support ELEMENT and it ATTRIBUTE to be operated.
//   so your configuration file only can be organize by element with attribute(s), 
//   the others type can not be operated, but no error occur, such as TEXT, COMMENT
//   and so on.
//  
//  This helper class support write REDIRECT by the second parameter of Open function.
//  
//  The main operate function [G|S]etAttributeStr and [G|S]etAttributeInt, support 
//  full path to access the element attribute.
//  1. '\\' is equal '/', like as:
//	   "\\IMServer"					- Use prefix '\\' instead of root
//	   "\\IMServer\\"				- The postfix '\\' is optional
//  2. Lead with '\\' or '/' represent ROOT element, can NOT be use SetAttribute* when not exist ROOT element
//  3. NULL represent ROOT element, can NOT be use SetAttribute* when not exist ROOT element
//  4. NAME_[n] can be access NAME_ element of n'th, n is zero-index, NAME_ can be optional when 
//     is GetAttribute*, and exist the element to SetAttribute*
//  5. Use full path to access any element attribute always RIGHT, like as:
//     "eIMApp\\IMServer"			- Use full path
//	   "eIMApp\\IMServer\\[0]		- Get of first sub-element of IMServer, can NOT be set [0] when not exist
//	   "eIMApp\\IMServer\\Set[0]	- Get of first 'Set' sub-element of IMServer
//
//  Example:
//	  C_XmlConfig xmlCfg;
//    if ( xmlCfg.Open(_T("C:\\App.config"), _T("D:\\App.config")) )
//    {
//	  	xmlCfg.GetAttributeStr(_T("\\"), _T("Version"))
//	  	xmlCfg.GetAttributeStr(_T("eIMProtocol\\Group[0]\\[0]"), _T("Name"));
//	  
//	  	// Set sub-index always lead with element name
//	  	xmlCfg.SetAttributeStr(_T("eIMProtocol\\Group[0]\\Field[1]"), _T("Name"), _T("Type"));
//	  
//	  	xmlCfg.Close();
//    }
//
// Auth: lwq
// Date: 2017/12/28
// 
// History: 
//    2017/12/28 lwq 
//    
//    
/////////////////////////////////////////////////////////////////////////////
#include "Errors.h"
#include "UIEngine\UIEngine.h"

#define CHECK_PARAMETER_RET_(RET) \
	do{ \
		if(lpszPath == NULL || lpszAttrName == NULL || !IsOpen()) \
		{ \
			SASSERT(FALSE); \
			return RET; \
		} \
	}while(0)

inline C_XmlConfig::C_XmlConfig(void)
	: m_bOpen( FALSE )
	, m_bModify( FALSE )
	, m_bRedirect( FALSE )
{

}

inline C_XmlConfig::~C_XmlConfig(void)
{
 	Close();	
}

inline bool C_XmlConfig::_Open(const TCHAR* pszFile, pugi::xml_document& doc, const char* pszPsw)
{
	if ( pszFile == NULL )
		return false;

	BOOL bZip = FALSE;
	if ( FILE* pflFile = _tfopen(pszFile, _T("rb")) )
	{
		char szTag[4] = {0};	// ZIP tag: 0x04034b50
		fread(szTag, 4, 1, pflFile);
		fclose(pflFile);
		bZip = (szTag[0] == 0x50 && szTag[1] == 0x4b && szTag[2] == 0x03 && szTag[3] == 0x04);
	}
	else
	{
		STRACE( _T("Open \'%s\' failed"), pszFile);
		return false;
	}

	pugi::xml_parse_result ret;
	if ( bZip == FALSE )
	{
		ret = doc.load_file(pszFile);
		if ( ret.isOK() )
			return true;
	}
	else
	{
		SUIEngine uie;
		CAutoRefPtr<IZip> pIZip;

		if ( uie.Create7Zip((IObjRef**)&pIZip) && pIZip->Open(pszFile, pszPsw) )
		{
			DWORD dwFileSize = 0;
			if ( BYTE* pu8Buf = pIZip->GetFile(PathFindFileName(pszFile), &dwFileSize) )
			{
				ret = doc.load_buffer_inplace(pu8Buf, dwFileSize);
				if ( ret.isOK() )
					return true;
			}
		}
	}

	STRACE( _T("Open \'%s\' failed, err:%u, desc:%S"), pszFile, ret.status, ret.description());
	return false;
}

//=============================================================================
//Function:     Open
//Description:	Open XML format configuration file
//
//Parameter:
//	lpszFile        - Main config file, readonly when lpszRediFile is valid
//	lpszRediFile    - NULL: Forbidden the redirect function,
//					        else Write redirect to config file
//					  
//
//Return:
//  See error code of EIMERR_*  
//  EIMERR_NO_ERROR			Succeed
//	EIMERR_INVALID_PARAM	Invalid parameter 
//  EIMERR_FILE_NOT_EXIST	File not exist
//  EIMERR_OPEN_FILE_FAILED	Open failed
//=============================================================================
inline int C_XmlConfig::Open(LPCTSTR lpszFile, LPCTSTR lpszRediFile, bool bCreate, const char* pszPsw)
{
 	Close();		// Close it at first
	
	if ( lpszFile == NULL || lpszFile[0] == _T('\0') )
	{
		STRACE( _T("Invalid config file") ); 
		return ERR_INVALID_PARAM;	// Invalid file
	}

	if ( bCreate == false && !PathFileExists(lpszFile) )
	{
		STRACE( _T("Config file \'%s\' not exist"), lpszFile ); 
		return ERR_FILE_NOT_EXIST;	// File not exist
	}

	if ( !_Open(lpszFile, m_XmlCfg, pszPsw) )
	{	// Open config file
		if ( bCreate == false )
			return ERR_OPEN_FILE_FAILED;
	}

	m_bOpen = true;
	STRACE( _T("Open config file \'%s\' succeeded!"), lpszFile);

	m_szSaveFile = lpszFile;
	if ( lpszRediFile == NULL || lpszRediFile[0] == _T('\0') )
		return ERR_NO_ERROR;	// No redirect config, return 

	m_szSaveFile = lpszRediFile;
	if ( PathFileExists(lpszRediFile) )
	{
		if ( _Open(lpszRediFile, m_XmlCfgRedi, NULL) )
		{
			STRACE( _T("Open redirect config file \'%s\' succeeded!"), lpszFile);
			m_bRedirect = true;		
			return ERR_NO_ERROR;
		}
	}

	m_bRedirect = true;	// Auto create redirect if not exist
	return ERR_NO_ERROR;
}

//=============================================================================
//Function:     Save
//Description:	Save modify, save to main config when not redirection, else save
//				to redirection file
//
//Return:
//       true	- Saved
//		 false	- Save failed
//=============================================================================
inline bool C_XmlConfig::Save(LPCTSTR lpszFile)
{
	if( !IsOpen() )
		return false;

	if( !IsModify() )
		return true;

	SetModify( false );
	if ( lpszFile )
	{
		if( m_bRedirect )
			return m_XmlCfgRedi.save_file(lpszFile);
		else
			return m_XmlCfg.save_file(lpszFile);
	}
	else
	{
		if( m_bRedirect )
			return m_XmlCfgRedi.save_file(m_szSaveFile);
		else
			return m_XmlCfg.save_file(m_szSaveFile);
	}
}

//=============================================================================
//Function:     Close
//Description:	Close configruation, will be auto save when modified
//
//
//Return:
//       true	- Closed
//		 false	- Close failed
//=============================================================================
inline bool C_XmlConfig::Close()
{
	Save();

	m_bOpen = false;
	m_bRedirect = false;

	return true;
}

//=============================================================================
//Function:     SetModify
//Description:	Set modify flag
//
//Parameter:
//	bModified    - true: Modified
//				   false: Not modify
//
//Return:
//      true: Original is modified
//		false: Original is not modified
//=============================================================================
inline bool C_XmlConfig::SetModify(bool bModified)
{
	bool bOld = m_bModify; 
	m_bModify = bModified; 

	return bOld;
}

//=============================================================================
//Function:     IsOpen
//Description:	Get the open status
//
//
//Return:
//		true	- Opened
//		false	- Not open
//=============================================================================
inline bool C_XmlConfig::IsOpen()
{ 
	return m_bOpen;
}

//=============================================================================
//Function:     IsModify
//Description:	Get the modify flag
//
//
//Return:
//		true	- Modified
//		false	- Not modify
//=============================================================================
inline bool C_XmlConfig::IsModify()
{
	return m_bModify; 
}

//=============================================================================
//Function:     IsRedirect
//Description:	Get the Redirect flag
//
//
//Return:
//		true	- Redirected
//		false	- Not Redirect
//=============================================================================
inline bool C_XmlConfig::IsRedirect()
{
	return m_bRedirect; 
}

inline pugi::xml_node C_XmlConfig::GetNode(pugi::xml_node node, LPCTSTR lpszPath, int i32Level, bool bCreate)
{
	if ( lpszPath == NULL )
		return pugi::xml_node();

	int nIndex = -1;
	TCHAR szNodeName[64+1] = { 0 };
	for (int i = 0; *lpszPath; lpszPath++, i++ )
	{
		if ( *lpszPath == _T('/') || *lpszPath == _T('\\') )
		{
			lpszPath++;	// Skip next spliter
			break;
		}
		else if ( *lpszPath == _T('[') )
		{
			lpszPath++;
			nIndex = _tstoi(lpszPath + 1);
		}
		else if ( nIndex >= 0 )
		{	// Skip still with next spliter
			continue;
		}

		szNodeName[i] = *lpszPath;
	}

	if ( i32Level == 0 )
	{
		if ( bCreate && node.empty() )
		{
			if ( node == m_XmlCfg.document_element() )
			{
				node = m_XmlCfg.append_child(szNodeName);
			}
			else
			{
				node = m_XmlCfgRedi.append_child(szNodeName);
			}
		}

		if ( *lpszPath == _T('\0') )
			return node;

		return GetNode(node, lpszPath, i32Level + 1, bCreate);
	}

	int  iFind = 0;
	bool bNoName = (szNodeName[0] == _T('\0'));
	pugi::xml_node child = (bNoName ? node.first_child() : node.child(szNodeName));
	while ( !child.empty() )
	{
		if ( child.type() == pugi::node_element && (nIndex < 0 || nIndex == iFind++))
			break;

		if ( bNoName )
			child = child.next_sibling();
		else
			child = child.next_sibling(szNodeName);
	}

	if ( child.empty() && bCreate )
		child = node.append_child(szNodeName);

	if ( *lpszPath == _T('\0') )
		return child;

	return GetNode(child, lpszPath, i32Level+1, bCreate);

}

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
inline LPCTSTR C_XmlConfig::GetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR pszDefault, bool bEnableRedirect )
{
	CHECK_PARAMETER_RET_( 0 );
	pugi::xml_node node;
	if ( m_bRedirect & bEnableRedirect )
		node = GetNode(m_XmlCfgRedi.document_element(), lpszPath, 0, false);

	if ( node.empty() )
		node = GetNode(m_XmlCfg.document_element(), lpszPath, 0, false);

	if ( node.empty() )
		return pszDefault;
	
	return node.attribute(lpszAttrName).as_string();
}

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
inline bool C_XmlConfig::SetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue )
{
	CHECK_PARAMETER_RET_( NULL );
	pugi::xml_node node = GetNode( m_bRedirect ? m_XmlCfgRedi.document_element() : m_XmlCfg.document_element(), lpszPath, 0, true );
	if ( node.empty() )
		return false;
	
	SetModify();
	node.append_attribute(lpszAttrName) = lpszValue;
	return true;
}

//=============================================================================
//Function:     GetAttributeInt
//Description:	Get integet attribute
//
//Parameter:
//	lpszPath        - Full path, see: _GetElementByPath
//	lpszAttrName    - Attribute name
//	i32Default      - Default attribute
//	bEnableRedirect - FALSE to forbidden redirection
//
//Return:
//		Attribute value                
//=============================================================================
inline int C_XmlConfig::GetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default, bool bEnableRedirect )
{
	CHECK_PARAMETER_RET_( 0 );
	pugi::xml_node node;
	if ( m_bRedirect & bEnableRedirect )
		node = GetNode(m_XmlCfgRedi.document_element(), lpszPath, 0, false);

	if ( node.empty() )
		node = GetNode(m_XmlCfg.document_element(), lpszPath, 0, false);

	if ( node.empty() )
		return i32Default;
	
	return node.attribute(lpszAttrName).as_int();
}

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
//		true		- Succeeded
//		false		- Falied
//=============================================================================
inline bool C_XmlConfig::SetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value )
{
	CHECK_PARAMETER_RET_( NULL );
	pugi::xml_node node = GetNode( m_bRedirect ? m_XmlCfgRedi.document_element() : m_XmlCfg.document_element(), lpszPath, 0, true );
	if ( node.empty() )
		return false;
	
	SetModify();
	node.append_attribute(lpszAttrName) = i32Value;

	return true;
}

//=============================================================================
//Function:     _GetTiXmlDocument
//Description:	Get TiXmlDocument
//
//Parameter:
//
//Return:
//		m_XmlConfiger			- Configuration file
//		m_XmlConfigerRedi		- Configuration redirect file
//=============================================================================
inline pugi::xml_node C_XmlConfig::GetRoot()
{
	if ( m_bRedirect )
		return m_XmlCfgRedi.document_element();
	else
		return m_XmlCfg.document_element();
}

//=============================================================================
//Function:     Remove
//Description:	Remove the specified path element
//
//Parameter:
//
//Return:
//		true		- Succeeded
//		false		- Failed
//=============================================================================
inline bool C_XmlConfig::Remove( LPCTSTR lpszPath )
{
	pugi::xml_node node = GetNode( m_bRedirect ? m_XmlCfgRedi.document_element() : m_XmlCfg.document_element(), lpszPath, 0, false );
	if ( node.empty() )
		return false;

	SetModify();
	pugi::xml_node parent = node.parent();
	return parent.remove_child(node);
}

inline LPCTSTR C_XmlConfig::GetText( LPCTSTR lpszPath, LPTSTR pszDefault, bool bEnableRedirect )
{
	pugi::xml_node node = GetNode( m_bRedirect && bEnableRedirect ? m_XmlCfgRedi.document_element() : m_XmlCfg.document_element(), lpszPath, 0, false );
	if ( node.empty() )
		return pszDefault;
    
	return node.text().as_string();
}

inline bool C_XmlConfig::SetText( LPCTSTR lpszPath, LPCTSTR lpszText )
{
	pugi::xml_node node = GetNode( m_bRedirect ? m_XmlCfgRedi.document_element() : m_XmlCfg.document_element(), lpszPath, 0, true );
	if ( node.empty() )
		return false;

	SetModify();
	node.text().set(lpszText);
	return TRUE;
}
