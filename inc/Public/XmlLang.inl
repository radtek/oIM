// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// NOTICE:
//	Please DON'T modify this class, if you have any requirement to implement, PLEASE
//  inherit it and implement it.
//
// Description: C_XmlLang is a helper class to read languages
//   configuration file.
//  
//
//
// Auth: lwq
// Date:  2014/1/7 16:53:50 
// 
// History: 
//    2014/1/7 lwq 
//    
//    
/////////////////////////////////////////////////////////////////////////////


inline	C_XmlLang::C_XmlLang()
{

}

inline	C_XmlLang::~C_XmlLang()
{
	Close();
}

inline	int	C_XmlLang::Open( LPCTSTR lpszFile, LPCTSTR lpszLanguage )
{
	int i32Rlt = C_XmlConfiger::Open( lpszFile, NULL );
	
	if ( FAILED(i32Rlt) )
		return i32Rlt;

	if ( _strnicmp( LANG_ROOT_VALUE,
		m_XmlConfiger.RootElement()->Value(),
		TYPE_ELEMENT_NAME_SIZE ) )
	{
		return EIMERR_FILE_NOT_CONFORM;		//Root element isn't conform to requirement
	}

	int i32LangIndex = IsExistLang( lpszLanguage );
	if ( -1 == i32LangIndex )
	{
		return EIMERR_LANG_NOT_EXIST;
	}

	TCHAR	szPath[250];
	_sntprintf( szPath, 250, _T("LangRoot\\Language[%d]"), i32LangIndex );
	m_mapErrLang.clear();
	LoadLangAttr( szPath );

	return EIMERR_NO_ERROR;
}

inline	LPCTSTR C_XmlLang::GetLangAttr( const DWORD dwId )
{
	MapErrLangIt it = m_mapErrLang.find( dwId );
	if ( it != m_mapErrLang.end() )
		return it->second.c_str();
    
	return NULL;
}

inline LPCTSTR C_XmlLang::SetLangAttr( const DWORD dwId, LPCTSTR lpcszDesc)
{
	MapErrLangIt it = m_mapErrLang.find( dwId );
	if ( it == m_mapErrLang.end() )
		m_mapErrLang.insert( MapErrLang::value_type(dwId, lpcszDesc) );
	else
		it->second = lpcszDesc;

	return lpcszDesc;
}


inline	BOOL C_XmlLang::LoadLangAttr( const TCHAR* pszPath )
{
	TiXmlElement*	pElm = NULL;
	if ( pElm = _GetElementByPath( m_XmlConfiger, pszPath ) )
	{
		DWORD	  dwId;
		eIMString szId;
		eIMString szValue;
		TiXmlElement*	pChildElm = pElm->FirstChildElement();
		while ( pChildElm )
		{
			if (_GetAttribute( pChildElm, _T(LANG_ATTRIBUTE_ID), szId ) &&
				_GetAttribute( pChildElm, _T(LANG_ATTRIBUTE_VALUE), szValue) && 
				(dwId = ::Str2Int(szId.c_str(), 0)))
			{
				m_mapErrLang.insert( MapErrLang::value_type(dwId, szValue) );
			}

			pChildElm = pChildElm->NextSiblingElement();
		}

		return TRUE;
	}

	return FALSE;
}

inline int C_XmlLang::IsExistLang( const TCHAR* pszLang )
{
	int i32Cnt = 0;
	TiXmlElement*	pElm = NULL;
	eIMStringA		szLangA;
	::eIMTChar2UTF8(pszLang, szLangA);
	pElm = m_XmlConfiger.RootElement()->FirstChildElement();
	while ( pElm )
	{
		if ( szLangA.compare(pElm->Attribute( LANG_ATTRIBUTE_NAME )) == 0 )
		{
			return i32Cnt;
		}

		i32Cnt++;
		pElm = pElm->NextSiblingElement();
	}

	return -1;
}
