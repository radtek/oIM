// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
//		表情编辑文件，通过文件名获取和修改名字和快捷键
//	通过快捷键获取文件全路径
//		使用参考函数介绍
// 
// Auth: HuDongwen
// Date: 2014/7/15 15:30:31
// 
// History: 
//    2014/7/15 HuDongwen 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __I_EIM_XMLEMOTICON_2013_12_31_BY_YFGZ__
#define __I_EIM_XMLEMOTICON_2013_12_31_BY_YFGZ__

#include "Public\Plugin.h"

typedef struct tagEmoticonInfo
{
	tagEmoticonInfo()
	{
		i32Order = 0;
	}

	int       i32Order;
	eIMString szFile;
	eIMString szName;
	eIMString szShortcut;
}S_EmoticonInfo, *PS_EmoticonInfo;
typedef const struct tagEmoticonInfo* PS_EmoticonInfo_;

typedef std::map<eIMString, S_EmoticonInfo>		MapFileEmots;			//Emoticons map, index by file
typedef MapFileEmots::iterator					MapFileEmotsIt;			//iterator of emoticons map

typedef std::map<eIMString, S_EmoticonInfo>		MapShortcutEmots;		//Emoticons map, index by shortcut
typedef MapShortcutEmots::iterator				MapShortcutEmotsIt;		//iterator of emoticons map

class __declspec(novtable) I_eIMXmlEmoticon : public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     GetEmoticons
	//Description:	Get the map of all emoticon, index by file
	//
	//Parameter:
	//
	//Return:
	//		MapFileEmots	- map of emoticons
	//=============================================================================
	virtual MapFileEmots& GetMapFileEmots() = 0;

	//=============================================================================
	//Function:     GetEmoticons
	//Description:	Get the map of all emoticon, index by shortcut
	//
	//Parameter:
	//
	//Return:
	//		MapShortcutEmots	- map of emoticons
	//=============================================================================
	virtual MapShortcutEmots& GetMapShortcutEmots() = 0;

	//=============================================================================
	//Function:     SetEmotName
	//Description:	Set Emoticon Name
	//
	//Parameter:
	//	pszFile		- The value of file attribute
	//	pszValue		- Name value, when is invalid, clear it
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed
	//=============================================================================
	virtual BOOL SetEmotName( const TCHAR* , const TCHAR*  ) = 0;

	//=============================================================================
	//Function:     SetEmotShortcut
	//Description:	Set Emoticon Shortcut
	//
	//Parameter:
	//	pszFile			- The value of file attribute
	//	pszValue		- Shortcut value, when is invalid, clear it
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed
	//=============================================================================
	virtual BOOL SetEmotShortcut( const TCHAR* , const TCHAR*  ) = 0;

	//=============================================================================
	//Function:     InsertEmot
	//Description:	
	//
	//Parameter:
	//	pszFile        -	File value (Not null)
	//	pszFilePos     -	Insert the end of configure if NULL
	//						insert the first of parent if _T("")
	//						else insert the back of the file.
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed            
	//=============================================================================
	virtual int InsertEmot(const TCHAR*, const TCHAR* ) = 0;

	//=============================================================================
	//Function:     RemoveEmot
	//Description:	
	//
	//Parameter:
	//	pszFile        -	File value (Not null)
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed            
	//=============================================================================
	virtual BOOL RemoveEmot(const TCHAR* ) = 0;

	//=============================================================================
	//Function:     SwapEmot
	//Description:	Swap position by file attribute
	//
	//Parameter:
	//	pszFileA    -	A File value
	//	pszFileB    - Another File value
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed                 
	//=============================================================================
	virtual BOOL SwapEmot(const TCHAR* , const TCHAR* ) = 0;

	//=============================================================================
	//Function:     IsExistEmot
	//Description:	
	//
	//Parameter:
	//	pszShortcut    -	The shortcut of emoticon
	//
	//Return:
	//		TRUE		- Exist
	//		FALSE		- Not exist                 
	//=============================================================================
	virtual BOOL IsExistEmot(const TCHAR* pszShortcut) = 0;

};

#endif  // __I_EIM_XMLEMOTICON_2013_12_31_BY_YFGZ__