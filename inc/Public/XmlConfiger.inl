// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// NOTICE:
//	Please DON'T modify this class, if you have any requirement to implement, PLEASE
//  inherit it and implement it.
//
// Description: C_XmlConfiger is a helper class to read and write standard XML
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
//	  C_XmlConfiger xmlConfiger;
//    if ( xmlConfiger.Open(_T("C:\\App.config"), _T("D:\\App.config")) )
//    {
//	  	xmlConfiger.GetAttributeStr(_T("\\"), _T("Version"))
//	  	xmlConfiger.GetAttributeStr(_T("eIMProtocol\\Group[0]\\[0]"), _T("Name"));
//	  
//	  	// Set sub-index always lead with element name
//	  	xmlConfiger.SetAttributeStr(_T("eIMProtocol\\Group[0]\\Field[1]"), _T("Name"), _T("Type"));
//	  
//	  	xmlConfiger.Close();
//    }
//
// Auth: lwq
// Date: 2013/12/19 14:46:22
// 
// History: 
//    2013/12/19 lwq 
//    
//    
/////////////////////////////////////////////////////////////////////////////
#include "XmlConfiger.h"
#include "Debugs.h"
#include "Errors.h"
#include "PathUtils.h"
#include "StrUtil.h"

#define CHECK_PARAMETER_RET_(RET) \
	do{ \
		if(lpszPath == NULL || lpszAttrName == NULL || !IsOpen()) \
		{ \
			ASSERT_(FALSE); \
			return RET; \
		} \
	}while(0)

inline C_XmlConfiger::C_XmlConfiger(void)
	: m_bOpen( FALSE )
	, m_bModify( FALSE )
	, m_bRedirect( FALSE )
{

}

inline C_XmlConfiger::~C_XmlConfiger(void)
{
 	Close();	
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
inline int C_XmlConfiger::Open(LPCTSTR lpszFile, LPCTSTR lpszRediFile, BOOL bCreate)
{
 	Close();		// Close it at first
	
	if ( lpszFile == NULL || lpszFile[0] == _T('\0') )
	{
		TRACE_( _T("Invalid config file") ); 
		return EIMERR_INVALID_PARAM;	// Invalid file
	}

	if ( bCreate == FALSE && !eIMPathFileExists(lpszFile) )
	{
		TRACE_( _T("Config file \'%s\' not exist"), lpszFile ); 
		return EIMERR_FILE_NOT_EXIST;	// File not exist
	}

	eIMString	szValue;
	if ( !TiXmlLoadFile(lpszFile, m_XmlConfiger) )
	{	// Open config file
		::eIMMByte2TChar(m_XmlConfiger.ErrorDesc(), szValue);
		TRACE_(_T("Open config file \'%s\' failed! Err:%d (L%d, R%d) %s"), 
			lpszFile, 
			m_XmlConfiger.ErrorId(),
			m_XmlConfiger.ErrorRow(),
			m_XmlConfiger.ErrorCol(),
			szValue.c_str() );

		if ( bCreate == FALSE )
			return EIMERR_OPEN_FILE_FAILED;
	}

	m_bOpen = TRUE;
	TRACE_( _T("Open config file \'%s\' succeeded!"), lpszFile);

	// Get encoding
	TiXmlNode* pNode = m_XmlConfiger.FirstChild();
	TiXmlDeclaration* pDec = NULL;
	if ( pNode && (pDec = pNode->ToDeclaration()) )
	{
		const char* pszEncoding = pDec->Encoding();
		if ( pszEncoding && _strnicmp(pszEncoding, "utf-8", TYPE_ELEMENT_NAME_SIZE ) == 0 )
			m_bUtf8 = TRUE;
	}

	m_szSaveFile = lpszFile;
	if ( lpszRediFile == NULL || lpszRediFile[0] == _T('\0') )
		return EIMERR_NO_ERROR;	// No redirect config, return 

	m_szSaveFile = lpszRediFile;
	if ( eIMPathFileExists(lpszRediFile) )
	{
		if ( TiXmlLoadFile(lpszRediFile, m_XmlConfigerRedi) )
		{
			TRACE_( _T("Open redirect config file \'%s\' succeeded!"), lpszFile);
			m_bRedirect = TRUE;		
			return EIMERR_NO_ERROR;
		}
		else
		{
			::eIMMByte2TChar(m_XmlConfigerRedi.ErrorDesc(), szValue);
			TRACE_( _T("Open redirect config file \'%s\' failed! Err:%d (L%d, R%d) %s"), 
				lpszRediFile, 
				m_XmlConfigerRedi.ErrorId(),
				m_XmlConfigerRedi.ErrorRow(),
				m_XmlConfigerRedi.ErrorCol(),
				szValue.c_str() );
		}
	}

	m_bRedirect = TRUE;							// Auto create redirect if not exist
	m_XmlConfigerRedi.ClearError();				// Clear error

	return EIMERR_NO_ERROR;
}

//=============================================================================
//Function:     Save
//Description:	Save modify, save to main config when not redirection, else save
//				to redirection file
//
//Return:
//       TRUE	- Saved
//		 FALSE	- Save failed
//=============================================================================
inline BOOL C_XmlConfiger::Save(LPCTSTR lpszFile)
{
	if( !IsOpen() )
		return FALSE;

	if( !IsModify() )
		return TRUE;

	SetModify( FALSE );
	if ( lpszFile )
	{

		if( m_bRedirect )
			return TiXmlSaveFile(lpszFile, m_XmlConfigerRedi);
		else
			return TiXmlSaveFile(lpszFile, m_XmlConfiger);
	}
	else
	{
		if( m_bRedirect )
			return TiXmlSaveFile(m_szSaveFile.c_str(), m_XmlConfigerRedi);
		else
			return TiXmlSaveFile(m_szSaveFile.c_str(), m_XmlConfiger);
	}
}

//=============================================================================
//Function:     Close
//Description:	Close configruation, will be auto save when modified
//
//
//Return:
//       TRUE	- Closed
//		 FALSE	- Close failed
//=============================================================================
inline BOOL C_XmlConfiger::Close()
{
	Save();

	m_XmlConfiger.Clear();
	m_XmlConfigerRedi.ClearError();

	m_bUtf8 = FALSE;
	m_bOpen = FALSE;
	m_bRedirect = FALSE;

	return TRUE;
}

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
inline BOOL C_XmlConfiger::SetModify(BOOL bModified)
{
	BOOL bOld = m_bModify; 
	m_bModify = bModified; 

	return bOld;
}

//=============================================================================
//Function:     IsOpen
//Description:	Get the open status
//
//
//Return:
//		TRUE	- Opened
//		FALSE	- Not open
//=============================================================================
inline BOOL C_XmlConfiger::IsOpen()
{ 
	return m_bOpen;
}


//=============================================================================
//Function:     IsModify
//Description:	Get the modify flag
//
//
//Return:
//		TRUE	- Modified
//		FALSE	- Not modify
//=============================================================================
inline BOOL C_XmlConfiger::IsModify()
{
	return m_bModify; 
}

//=============================================================================
//Function:     IsRedirect
//Description:	Get the Redirect flag
//
//
//Return:
//		TRUE	- Redirected
//		FALSE	- Not Redirect
//=============================================================================
inline BOOL C_XmlConfiger::IsRedirect()
{
	return m_bRedirect; 
}

//=============================================================================
//Function:     _GetAttribute
//Description:	Get the attribute, convert to TCHAR when is UTF-8 format
//
// pElement		- Element to be operate
// pszName		- Attribute name
// szAttr		- Attribute value
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
inline BOOL C_XmlConfiger::_GetAttribute(const TiXmlElement* pElement, const TCHAR* pszName, eIMString& szAttr )
{
	szAttr.clear();
	if ( pElement == NULL || pszName == NULL )
		return FALSE;

	eIMStringA szNameA;
	::eIMTChar2UTF8(pszName, szNameA);
	const char* pszValue = pElement->Attribute(szNameA.c_str());
	if ( pszValue == NULL )
		return FALSE;

	::eIMUTF8ToTChar(pszValue, szAttr);

	return TRUE;
}

//=============================================================================
//Function:     _SetAttribute
//Description:	Set the attribute, convert to UTF-8 when is UTF-8 format
//
// pElement		- Element to be operate
// pszName		- Attribute name
// szAttr		- Attribute value
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
inline BOOL C_XmlConfiger::_SetAttribute(TiXmlElement* pElement, const TCHAR* pszName, const TCHAR* pszAttr )
{
	if ( pElement == NULL || pszName == NULL )
		return FALSE;

	SetModify();

	eIMStringA szNameA;
	eIMStringA szValueA;

	::eIMTChar2UTF8(pszName, szNameA);
	::eIMTChar2UTF8(pszAttr, szValueA);

	pElement->SetAttribute(szNameA.c_str(), szValueA.c_str());
	return TRUE;
}

//=============================================================================
//Function:     _GetSubElementCount
//Description:	Get the child element count
//
//Parameter:
//	szPath       - Path of the parent element path
//	pszName      - The child element name to be count if not NULL, else
//                 is all sub element count
//
//Return:
//		Return the element count         
//=============================================================================
inline int C_XmlConfiger::_GetSubElementCount( TiXmlElement* pElementParent, LPCTSTR pszName )
{
	if ( pElementParent == NULL )
		return 0;

	int			  i32Count = 0;
	const char*   pszValue = NULL;
	eIMStringA	  szNameA;
	TiXmlElement* pElement = pElementParent->FirstChildElement();

	::eIMTChar2UTF8(pszName, szNameA);
	while ( pElement )
	{
		if ( pszName )
		{
			pszValue = pElement->Value();
			if ( pszValue && szNameA.compare(pszValue) == 0 )
				i32Count++;
		}
		else
		{
			i32Count++;
		}

		pElement = pElement->NextSiblingElement();
	}

	return i32Count;
}

//=============================================================================
//Function:     _GetSubElementCount
//Description:	Get the child element count
//
//Parameter:
//  xmlDoc		 - XML Doc object
//	szPath       - Path of the parent element path
//	pszName      - The child element name to be count if not NULL, else
//                 is all sub element count
//
//Return:
//		Return the element count         
//=============================================================================
inline int	C_XmlConfiger::_GetSubElementCount( TiXmlDocument& xmlDoc, LPCTSTR lpszPath, LPCTSTR pszName )
{
	return _GetSubElementCount( _GetElementByPath( xmlDoc, lpszPath ), pszName );
}

//=============================================================================
//Function:     _GetSubElementName
//Description:	Get a sub-element name by full path
//
//Parameter:
//	szPath       - Path to be get a sub-element name, will be removed the getted name
//	szElement    - Return the element name
//
//Return:
//		Return the getted element name size         
//=============================================================================
inline int	C_XmlConfiger::_GetSubElementName(eIMString& szPath, eIMString& szElement)
{
	int nIndex = szPath.find( _T('\\') );
	if ( eIMString::npos == nIndex )
		nIndex = szPath.find( _T('/') );

	if ( eIMString::npos == nIndex )
	{
		szElement = szPath;
		szPath.clear();
	}
	else
	{
		szElement = szPath.substr(0, nIndex);
		szPath.erase(0, nIndex + 1);
	}

	return szElement.size();
}

//=============================================================================
//Function:     _GetElementByPath
//Description:	Get element by path
//
//Parameter:
//	xmlDoc      - XmlDoc object
//	lpszPath    - Full path to be access, '\\' equal to '/'
//    NULL						- Return ROOT element
//	  "\\"						- Return ROOT element
//	  "\\IMServer"				- Use prefix '\\' instead of root
//	  "\\IMServer\\"			- The postfix '\\' is optional
//    "eIMApp\\IMServer"		- Use full path
//	  "eIMApp\\IMServer\\[0]	- Get of first sub-element of IMServer
//	  "eIMApp\\IMServer\\Set[0]	- Get of first 'Set' sub-element of IMServer
//	bCreate		- TRUE: Create element if not exist, sub-index MUST BE begin
//				        with element name, ROOT can not use '\\' or '/'.
//Return:
//		NULL	- Not finded, else is element pointer          
//=============================================================================
inline TiXmlElement* C_XmlConfiger::_GetElementByPath(TiXmlDocument& xmlDoc, LPCTSTR lpszPath, BOOL bCreate)
{
	if( !IsOpen() )				// Not opened
		return NULL;

	if (lpszPath == NULL ||		// NULL
		lpszPath[0] == _T('\0') )
		return xmlDoc.RootElement();

	int				nSize = 0;
	const char*		pszValue = NULL;
	eIMString		szPath = lpszPath;
	eIMString		szElement;
	eIMStringA		szElementA;
	TiXmlElement*	pElementChild   = NULL;
	TiXmlElement*	pElementParrent = xmlDoc.RootElement();

	nSize = _GetSubElementName(szPath, szElement);	
	::eIMTChar2UTF8(szElement.c_str(), szElementA);
	if ( pElementParrent == NULL )
	{	// Check ROOT
		if ( bCreate == TRUE && !szElement.empty())
		{
			TiXmlElement e(szElementA.c_str());
			pElementParrent = (TiXmlElement*)xmlDoc.InsertEndChild( e );
			SetModify();
		}
		else
		{
			return NULL;
		}
	}

	if ( nSize == 0 && szPath.empty() )	// '\\' or '/', is ONLY ROOT
		return pElementParrent;

	pszValue = pElementParrent->Value();
	if(	szPath.empty() && nSize && pszValue &&	// Only ROOT element
		szElementA.compare(pszValue) == 0 )
	{
		return pElementParrent;
	}

	if ( nSize && pszValue && szElementA.compare(pszValue) )
	{
		TRACE_( _T("ROOT element(\'%s\' <> \'%s\') error of file \'%s\'"), pszValue, szElement.c_str(), xmlDoc.Value() );
		return NULL;
	}

	while ( pElementParrent && (nSize = _GetSubElementName(szPath, szElement)) )
	{
		::eIMTChar2UTF8(szElement.c_str(), szElementA);
		if( szElement[nSize - 1] == _T(']') )
		{	// As [n] or Name[n]
			eIMString::size_type nFindIndex = szElement.find( _T('[') );
			if ( nFindIndex < 0 )
			{
				TRACE_(_T("Error format of \'%s\'"), szElement.c_str());
				return NULL;
			}

			// Get array element name and index
			int i = 0;
			int i32Index  = _ttol( &szElement[nFindIndex + 1] );
			pElementChild = pElementParrent->FirstChildElement();
			szElement.erase(nFindIndex);
			szElementA.erase(nFindIndex);
			do
			{
				if ( pElementChild == NULL  )
				{
					if ( bCreate == TRUE && !szElement.empty() )
					{	// sub-index with element name
						::eIMTChar2UTF8(szElement.c_str(), szElementA);
						TiXmlElement e(szElementA.c_str());
						pElementChild = (TiXmlElement*)pElementParrent->InsertEndChild(e);
						SetModify();

						++i;
					}
					else
						return NULL;
				}
				else
				{
					if ( szElement.empty() ||	// Only sub-index
						((pszValue = pElementChild->Value()) && // Sub-index with element name
						szElementA.compare(pszValue) == 0) )
					{
						++i;
					}
				}

				if ( i <= i32Index )
					pElementChild = pElementChild->NextSiblingElement();
			}while( i <= i32Index );

			if ( i >= i32Index )
			{
				if( szPath.empty() )	// Finded
					return pElementChild;
			}
			else
				return NULL;
		}
		else	// if( szElement[nSize - 1] == _T(']') )
		{
			pElementChild = pElementParrent->FirstChildElement();
			while ( pElementChild )
			{
				pszValue = pElementChild->Value();
				if ( pszValue && szElementA.compare(pszValue) == 0)
				{
					if ( szPath.empty() )	// Finded
						return pElementChild;

					break;
				}
				else
				{
					pElementChild = pElementChild->NextSiblingElement();
				}
			}	// while ( pElementChild )
		}	// if( szElement[nSize - 1] == _T(']') )

		if ( pElementChild == NULL )
		{
			if ( bCreate && !szElement.empty() )
			{
				TiXmlElement e(szElementA.c_str());
				pElementChild = (TiXmlElement*)pElementParrent->InsertEndChild(e);
				SetModify();

				if ( szPath.empty() )	// Finded
					return pElementChild;
			}
			else
			{
				TRACE_(_T("Not found element \'%s\'"), szElement.c_str());
				return NULL;
			}
		}

		pElementParrent = pElementChild;
	}
	
	return NULL;
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
inline int C_XmlConfiger::GetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, TCHAR* pszValue, int i32Size, BOOL bEnableRedirect )
{
	CHECK_PARAMETER_RET_( 0 );
    
    eIMString szValue;
	TiXmlElement* pE = NULL;
	if ((m_bRedirect && bEnableRedirect &&
		(pE = _GetElementByPath( m_XmlConfigerRedi, lpszPath )) &&
		_GetAttribute(pE, lpszAttrName, szValue)) ||
        ((pE = _GetElementByPath( m_XmlConfiger, lpszPath )) &&
		_GetAttribute(pE, lpszAttrName, szValue)) )
    {
        if (szValue.size() + 1 <= (unsigned)i32Size)
        {
            _tcsncpy(pszValue, szValue.c_str(), szValue.size());
			pszValue[szValue.size()] = _T('\0');
            return szValue.size();
        }
            
        return -(int)(szValue.size() + 1);
    }
	
	return pszValue ? _tcslen(pszValue) : 0;
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
//		TRUE		- Succeeded
//		FALSE		- Falied
//=============================================================================
inline BOOL C_XmlConfiger::SetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue )
{
	CHECK_PARAMETER_RET_( NULL );

	TiXmlElement* pE = _GetElementByPath( m_bRedirect ? m_XmlConfigerRedi : m_XmlConfiger, lpszPath, TRUE );
	if ( pE == NULL )
		return FALSE;
	
	return _SetAttribute(pE, lpszAttrName, lpszValue);
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
inline int C_XmlConfiger::GetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default, BOOL bEnableRedirect )
{
	CHECK_PARAMETER_RET_( 0 );

	const char*   pszAttr = NULL;
	eIMStringA	  szNameA;
	TiXmlElement* pE = NULL;
	::eIMTChar2UTF8(lpszAttrName, szNameA);
	if ( m_bRedirect && bEnableRedirect &&
		(pE = _GetElementByPath(m_XmlConfigerRedi, lpszPath)))
		pszAttr = pE->Attribute(szNameA.c_str());

	if ( pszAttr == NULL && 
		(pE = _GetElementByPath(m_XmlConfiger, lpszPath)) )
		pszAttr = pE->Attribute(szNameA.c_str());

	if ( pszAttr )
		return strtoul( pszAttr, NULL, 0 );

	return i32Default;
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
//		TRUE		- Succeeded
//		FALSE		- Falied
//=============================================================================
inline BOOL C_XmlConfiger::SetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value )
{
	CHECK_PARAMETER_RET_( NULL );

	TiXmlElement* pE = _GetElementByPath( m_bRedirect ? m_XmlConfigerRedi : m_XmlConfiger, lpszPath, TRUE );
	if ( pE == NULL )
		return FALSE;

	eIMStringA szNameA;
	::eIMTChar2UTF8(lpszAttrName, szNameA);
	pE->SetAttribute( szNameA.c_str(), i32Value );
	SetModify();

	return TRUE;
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
inline TiXmlDocument& C_XmlConfiger::_GetTiXmlDocument()
{
	return (m_bRedirect ? m_XmlConfigerRedi : m_XmlConfiger);
}

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
inline BOOL C_XmlConfiger::Remove( LPCTSTR lpszPath )
{
	TiXmlElement* pEl = _GetElementByPath( m_bRedirect ? m_XmlConfigerRedi : m_XmlConfiger, lpszPath, FALSE );
	if ( pEl == NULL )
		return FALSE;

	TiXmlNode* pNode = pEl->Parent();
	return !!pNode->RemoveChild(pEl);
}

inline int C_XmlConfiger::GetText( LPCTSTR lpszPath, TCHAR* pszText, int i32Size, BOOL bEnableRedirect )
{
	if ( lpszPath == NULL )
		return 0;
    
    eIMString	  szValue;
	const char*   pszValue = NULL;
	TiXmlElement* pE = NULL;
	if ((m_bRedirect && bEnableRedirect &&
		 (pE = _GetElementByPath( m_XmlConfigerRedi, lpszPath )) &&
		 (pszValue =  pE->GetText())) ||
		((pE = _GetElementByPath( m_XmlConfiger, lpszPath )) &&
		 (pszValue = pE->GetText())) )
	{
		::eIMUTF8ToTChar(pszValue, szValue);
		if ( szValue.size() + 1 <= (unsigned)i32Size )
		{
			_tcsncpy(pszText, szValue.c_str(), szValue.size());
			pszText[szValue.size()] = _T('\0');
			return szValue.size();
		}
        
		return -(int)(szValue.size() + 1);
	}
	
	return pszText ? _tcslen(pszText) : 0;
}

inline BOOL C_XmlConfiger::SetText( LPCTSTR lpszPath, LPCTSTR lpszText )
{
	if ( lpszPath == NULL )
		return FALSE;

	TiXmlElement* pE = _GetElementByPath( m_bRedirect ? m_XmlConfigerRedi : m_XmlConfiger, lpszPath, TRUE );
	if ( pE == NULL )
		return FALSE;
	
    eIMStringA	  szValueA;
	
	::eIMTChar2UTF8(lpszText, szValueA);
	if (TiXmlText* pText = pE->FirstChild() ? pE->FirstChild()->ToText() : NULL )
	{
		pText->SetCDATA(true);
		pText->SetValue(szValueA.c_str());
	}
	else
	{
		TiXmlText Text(szValueA.c_str());
		Text.SetCDATA(true);
		pE->InsertEndChild(Text);
	}

	SetModify();
	return TRUE;
}
