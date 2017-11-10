// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
//		函数实现，通过GetObject()获取接口
// 
// 
// Auth: HuDongwen
// Date: 2014/7/15 15:34:52
// 
// History: 
//    2014/7/15 HuDongwen 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlEmoticon.h"

IMPLEMENT_PLUGIN_SINGLETON_(C_XmlEmoticon, INAME_EIMENGINE_EMOTICON);
C_XmlEmoticon& C_XmlEmoticon::GetObject( BOOL bReopen )
{
	static C_XmlEmoticon obj;

	if ( !obj.IsOpen() || bReopen )
	{
		TCHAR szUserName[64] = { 0 };
		eIMString szEmotFile = EMOT_CONFIG_PATH_FILE;
		
		eIMReplaceToken( szEmotFile );
		//if ( GetEnvironmentVariable(IME_ATTRIB_USER_NAME, szUserName, 64) )
		//{
		//	eIMString szUserEmotFile =  EMOT_USER_CONFIG_PATH_FILE;
		//	eIMReplaceTokenAs( szUserEmotFile, PATH_TOKEN_LOGINID, szUserName );
		//	obj.Open( szEmotFile.c_str(), szUserEmotFile.c_str() );
		//}
		//else
		{	// No redirect
			obj.Open(szEmotFile.c_str(), NULL);
		}
	}

	return obj;
}

C_XmlEmoticon::C_XmlEmoticon(void)
{
	m_i32Order = 0;
}

C_XmlEmoticon::~C_XmlEmoticon(void)
{

}

int C_XmlEmoticon::Open( LPCTSTR lpszFile, LPCTSTR lpszRediFile )
{
	int i32Rlt = C_XmlConfiger::Open(lpszFile, lpszRediFile);
	if ( EIMERR_NO_ERROR == i32Rlt )
	{
		if ( _stricmp(m_XmlConfiger.RootElement()->Value(), EMOT_ROOT_NODE ) )
		{
			return EIMERR_FILE_NOT_CONFORM;
		}
	}

	_GetEmoticons();
	return i32Rlt;
}

void C_XmlEmoticon::_GetEmoticons()
{
	if ( IsRedirect() )
		_GetEmoticons(m_XmlConfigerRedi, true);

	_GetEmoticons(m_XmlConfiger, false);
}

void C_XmlEmoticon::_GetEmoticons(const TiXmlDocument& xmlConfigur, bool bRedic)
{
	const TiXmlElement* pRoot = xmlConfigur.RootElement();
	if ( !pRoot )
		return ;

	const TiXmlElement* pChildElm = pRoot->FirstChildElement();
	while ( pChildElm )
	{
		const TiXmlElement* pGrandChildElm = pChildElm->FirstChildElement();
		while ( pGrandChildElm )
		{
			eIMString szFile;
			eIMString szName;
			eIMString szShortcut;
			_GetAttribute(pGrandChildElm, _T(EMOT_ATTRIBUTE_FILE), szFile);
			_GetAttribute(pGrandChildElm, _T(EMOT_ATTRIBUTE_NAME), szName);
			_GetAttribute(pGrandChildElm, _T(EMOT_ATTRIBUTE_SHORTCUT), szShortcut);

			if (bRedic)
				szFile = EMOT_USER_CONFIG_PATH _T("\\") + szFile;
			else
				szFile = EMOT_CONFIG_PATH _T("\\") + szFile;
			eIMReplaceToken( szFile );
			S_EmoticonInfo sEmotInfo;
			sEmotInfo.szFile = szFile;
			sEmotInfo.szName = szName;
			sEmotInfo.szShortcut = szShortcut;

			
			sEmotInfo.i32Order = m_i32Order;
			m_i32Order++;

			m_mapFileEmots[szFile] = sEmotInfo;
			if (szShortcut != _T(""))
				m_mapShortcutEmots[szShortcut] = sEmotInfo;

			pGrandChildElm = pGrandChildElm->NextSiblingElement();
		}

		pChildElm = pChildElm->NextSiblingElement();
	}
}

TiXmlElement* C_XmlEmoticon::_GetXmlElementByFile(const TiXmlDocument& xmlConfigure, const TCHAR* pszFile)
{
	const TiXmlElement* pRoot = xmlConfigure.RootElement();
	if ( !pRoot )
		return NULL;

	const TiXmlElement* pChildElm = pRoot->FirstChildElement();
	while ( pChildElm )
	{
		const TiXmlElement* pGrandChildElm = pChildElm->FirstChildElement();
		while ( pGrandChildElm )
		{
			eIMString szFileTemp;
			_GetAttribute(pGrandChildElm, _T(EMOT_ATTRIBUTE_FILE), szFileTemp);

			if ( _tcsncmp( szFileTemp.c_str(), pszFile, szFileTemp.length() ) == 0 )
			{
				return (TiXmlElement*)pGrandChildElm;
			}

			pGrandChildElm = pGrandChildElm->NextSiblingElement();
		}

		pChildElm = pChildElm->NextSiblingElement();
	}

	return NULL;
}

BOOL C_XmlEmoticon::IsExistEmot(const TCHAR* pszShortcut)
{
	if (pszShortcut == NULL || pszShortcut[0] == 0)
		return FALSE;

	 MapShortcutEmotsIt it;
	 it = m_mapShortcutEmots.find(pszShortcut);
	 if (it == m_mapShortcutEmots.end())
		 return FALSE;

	 return TRUE;
}

MapFileEmots& C_XmlEmoticon::GetMapFileEmots()
{
	return m_mapFileEmots;
}

MapShortcutEmots& C_XmlEmoticon::GetMapShortcutEmots()
{
	return m_mapShortcutEmots;
}

BOOL C_XmlEmoticon::SetEmotShortcut( const TCHAR* pszFile, const TCHAR* pszValue )
{
	if ( !IsRedirect() || !pszFile || pszFile[0] == 0 )
		return FALSE;

	if (IsExistEmot(pszValue))
		return EMOT_SHORTCUT_CONFLICT;
	
	eIMString szFile;
	szFile += EMOT_USER_CONFIG_PATH;
	szFile += _T("\\");
	szFile += pszFile;
	eIMReplaceToken(szFile);
	MapFileEmotsIt it = m_mapFileEmots.find(szFile);
	if (it == m_mapFileEmots.end())
		return EMOT_FILE_NOT_EXIST;

	S_EmoticonInfo sEmotInfo = m_mapFileEmots[szFile];
	sEmotInfo.szShortcut = pszValue;
	if (m_mapFileEmots[szFile].szShortcut != _T(""))
		m_mapShortcutEmots.erase(m_mapFileEmots[szFile].szShortcut);
	m_mapShortcutEmots[pszValue] = sEmotInfo;
	m_mapFileEmots[szFile].szShortcut = pszValue;

	TiXmlElement* pEle = _GetXmlElementByFile(m_XmlConfigerRedi, pszFile);
	if ( pEle )
	{
		if (!pszValue)
		{
			pEle->RemoveAttribute(EMOT_ATTRIBUTE_SHORTCUT);
			SetModify();
		}
		if ( _SetAttribute(pEle, _T(EMOT_ATTRIBUTE_SHORTCUT), pszValue) )
			return TRUE;
	}

	return FALSE;

}

BOOL C_XmlEmoticon::SetEmotName( const TCHAR* pszFile, const TCHAR* pszValue )
{
	if ( !IsRedirect() || !pszFile || pszFile[0] == 0 )
		return FALSE;

	eIMString szFile;
	szFile += EMOT_USER_CONFIG_PATH;
	szFile += _T("\\");
	szFile += pszFile;
	eIMReplaceToken(szFile);
	MapFileEmotsIt it = m_mapFileEmots.find(szFile);
	if (it == m_mapFileEmots.end())
		return EMOT_FILE_NOT_EXIST;

	m_mapFileEmots[szFile].szName = pszValue;
	if (m_mapFileEmots[szFile].szShortcut != _T(""))
		m_mapShortcutEmots[m_mapFileEmots[pszFile].szShortcut].szName = pszValue;

	TiXmlElement* pEle = _GetXmlElementByFile(m_XmlConfigerRedi, pszFile);
	if ( pEle )
	{
		if (!pszValue)
		{
			pEle->RemoveAttribute(EMOT_ATTRIBUTE_NAME);
			SetModify();
		}

		if ( _SetAttribute(pEle, _T(EMOT_ATTRIBUTE_NAME), pszValue) )
			return TRUE;
	}	

	return FALSE;
}

int C_XmlEmoticon::InsertEmot(const TCHAR* pszFile, const TCHAR* pszFilePos)
{
	if ( !IsRedirect()		||
		 !pszFile			||
		 pszFile[0] == 0 
	 )
	{
		return EMOT_INSERT_FAILED;
	}

	if ( _GetXmlElementByFile(m_XmlConfigerRedi, pszFile) )
		return EMOT_FILE_EXIST;

	eIMString szFile;
	szFile += EMOT_USER_CONFIG_PATH;
	szFile += _T("\\");
	szFile += pszFile;
	eIMReplaceToken(szFile);

	S_EmoticonInfo sEmotInfo;
	sEmotInfo.szFile = szFile;
	sEmotInfo.szName = _T("");
	sEmotInfo.szShortcut = _T("");

	m_mapFileEmots[szFile] = sEmotInfo;

	if ( NULL == pszFilePos )
	{
		TiXmlElement* pRoot = m_XmlConfigerRedi.RootElement();
		if ( !pRoot )
		{
			TiXmlDeclaration Declaration("1.0", "UTF-8", "");   //创建xml文件头
			m_XmlConfigerRedi.InsertEndChild(Declaration);					 //加入文件头

			TiXmlElement RootEle(EMOT_ROOT_NODE);								// <eIMEmot />

			TiXmlElement GroupEle(EMOT_GROUP_NODE);		
			GroupEle.SetAttribute(EMOT_ATTRIBUTE_NAME, EMOT_GROUPC_NAME_VALUE);	// <Group Name="Favorate" />

			TiXmlElement NewEle(EMOT_EMOT_NODE);		
			_SetAttribute(&NewEle, _T(EMOT_ATTRIBUTE_FILE), pszFile);			// <Emot File="***" />
			GroupEle.InsertEndChild(NewEle);									// <Group Name="Favorate"><Emot File="***"/></Group>

			RootEle.InsertEndChild(GroupEle);									// <eIMEmot><Group Name="Favorate"><Emot File="***"/></Group></eIMEmot>
			m_XmlConfigerRedi.InsertEndChild(RootEle);
			
			return EMOT_INSERT_SUCCEED;
		}

		TiXmlElement* pChild = pRoot->FirstChildElement();
		if (pChild)
		{
			TiXmlElement NewEle(EMOT_EMOT_NODE);
			_SetAttribute(&NewEle, _T(EMOT_ATTRIBUTE_FILE), pszFile);
			pChild->InsertEndChild(NewEle);
			return EMOT_INSERT_SUCCEED;
		}
	}
	else
	{
		TiXmlElement* pRoot = m_XmlConfigerRedi.RootElement();
		if ( !pRoot )
			return FALSE;

		TiXmlElement* pChild = pRoot->FirstChildElement();
		TiXmlElement* pGrandChildElm = NULL;
		if (pChild)
		{
			if ( _tcscmp(pszFilePos, _T("")) == 0 )
			{
				TiXmlElement NewEle(EMOT_EMOT_NODE);
				_SetAttribute(&NewEle, _T(EMOT_ATTRIBUTE_FILE), pszFile);
				pChild->InsertEndChild(NewEle);
				return EMOT_INSERT_SUCCEED;
			}

			pGrandChildElm = pChild->FirstChildElement();
		}
		while ( pGrandChildElm )
		{
			eIMString szFileTemp;
			_GetAttribute(pGrandChildElm, _T(EMOT_ATTRIBUTE_FILE), szFileTemp);
			if ( szFileTemp == pszFilePos )
			{
				TiXmlElement NewEle(EMOT_EMOT_NODE);
				_SetAttribute(&NewEle, _T(EMOT_ATTRIBUTE_FILE), pszFile);

				pChild->InsertAfterChild(pGrandChildElm, NewEle);
				return EMOT_INSERT_SUCCEED;
			}
			pGrandChildElm = pGrandChildElm->NextSiblingElement();
		}

	}

	return EMOT_INSERT_FAILED;
}

BOOL C_XmlEmoticon::RemoveEmot(const TCHAR* pszFile)
{
	if ( !IsRedirect()		||
		!pszFile			||
		pszFile[0] == 0 
		)
	{
		return FALSE;
	}

	eIMString szFile;
	szFile += EMOT_USER_CONFIG_PATH;
	szFile += _T("\\");
	szFile += pszFile;
	eIMReplaceToken(szFile);
	MapFileEmotsIt it = m_mapFileEmots.find(szFile);
	if (it == m_mapFileEmots.end())
		return EMOT_FILE_NOT_EXIST;

	if ( m_mapFileEmots[szFile].szShortcut != _T("") )
		m_mapShortcutEmots.erase(m_mapFileEmots[szFile].szShortcut);
	m_mapFileEmots.erase(szFile);

	TiXmlElement* pRoot = m_XmlConfigerRedi.RootElement();
	if (!pRoot)
		return FALSE;

	TiXmlElement* pChild = pRoot->FirstChildElement();
	while( pChild )
	{
		TiXmlElement* pGrandChildElm = NULL;
		pGrandChildElm = pChild->FirstChildElement();
		while ( pGrandChildElm )
		{
			eIMString szFileTemp;
			_GetAttribute(pGrandChildElm, _T(EMOT_ATTRIBUTE_FILE), szFileTemp);
			if ( szFileTemp == pszFile )
			{
				pChild->RemoveChild(pGrandChildElm);
				SetModify();
				return TRUE;
			}

			pGrandChildElm = pGrandChildElm->NextSiblingElement();
		}

		pChild = pChild->NextSiblingElement();
	
	}

	return FALSE;
}

BOOL C_XmlEmoticon::SwapEmot(const TCHAR* pszFileA, const TCHAR* pszFileB)
{
	if ( !pszFileA			||
		 pszFileA[0] == 0	||
		 !pszFileB			||
		 pszFileB[0] == 0	||
		 !IsRedirect()
		)
	{
		return FALSE;
	}

	TiXmlElement* pEleA = _GetXmlElementByFile(m_XmlConfigerRedi, pszFileA);
	TiXmlElement* pEleB = _GetXmlElementByFile(m_XmlConfigerRedi, pszFileB);

	if ( pEleA && pEleB )
	{
		eIMString szNameA, szNameB;
		eIMString szShortcutA, szShortcutB;

		_GetAttribute(pEleA, _T(EMOT_ATTRIBUTE_NAME), szNameA);
		_GetAttribute(pEleA, _T(EMOT_ATTRIBUTE_SHORTCUT), szShortcutA);
							 
		_GetAttribute(pEleB, _T(EMOT_ATTRIBUTE_NAME), szNameB);
		_GetAttribute(pEleB, _T(EMOT_ATTRIBUTE_SHORTCUT), szShortcutB);
							 
		_SetAttribute(pEleA, _T(EMOT_ATTRIBUTE_FILE), pszFileB);
		_SetAttribute(pEleA, _T(EMOT_ATTRIBUTE_NAME), szNameB.c_str());
		_SetAttribute(pEleA, _T(EMOT_ATTRIBUTE_SHORTCUT), szShortcutB.c_str());
							 
		_SetAttribute(pEleB, _T(EMOT_ATTRIBUTE_FILE), pszFileA);
		_SetAttribute(pEleB, _T(EMOT_ATTRIBUTE_NAME), szNameA.c_str());
		_SetAttribute(pEleB, _T(EMOT_ATTRIBUTE_SHORTCUT), szNameA.c_str());

		return TRUE;	
	}

	return FALSE;

}

//TiXmlElement* C_XmlEmoticon::GetXmlElementByShortcut(const TiXmlDocument& xmlConfigure, const TCHAR* pszShortcut)
//{
//	TiXmlElement* pRoot = xmlConfigure.RootElement();
//	if ( !pRoot )
//		return NULL;
//
//	TiXmlElement* pChildElm = pRoot->FirstChildElement();
//	while ( pChildElm )
//	{
//		TiXmlElement* pGrandChildElm = NULL;
//		pGrandChildElm = pChildElm->FirstChildElement();
//		while ( pGrandChildElm )
//		{
//			eIMString szFileTemp;
//			_GetAttribute(pGrandChildElm, EMOT_ATTRIBUTE_SHORTCUT, szFileTemp);
//
//			if ( szFileTemp == pszShortcut )
//			{
//				return pGrandChildElm;
//			}
//
//			pGrandChildElm = pGrandChildElm->NextSiblingElement();
//		}
//
//		pChildElm = pChildElm->NextSiblingElement();
//	}
//
//	return NULL;
//}

