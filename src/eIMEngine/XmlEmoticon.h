// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
//		表情编辑文件，通过文件名获取和修改名字和快捷键
//	通过快捷键获取文件全路径
//		使用参考函数介绍
// 
// Auth: HuDongwen
// Date: 2014/7/15 15:28:20
// 
// History: 
//    2014/7/15 HuDongwen 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_XMLEMOTICON_API_2013_12_31_BY_YFGZ__
#define __EIM_XMLEMOTICON_API_2013_12_31_BY_YFGZ__

#include "eIMEngine/IeIMXmlEmoticon.h"
#include "Public/XmlConfiger.h"

// 返回值定义
#define EMOT_SHORTCUT_CONFLICT		0x0010		//	快捷键冲突
#define EMOT_FILE_EXIST				0x0020		//	文件已经存在
#define EMOT_FILE_NOT_EXIST			0x0030		//	文件不存在
#define EMOT_INSERT_FAILED			0x0040		//  插入失败
#define EMOT_INSERT_SUCCEED			0x0050		//	插入成功

// 文件属性等
#define	EMOT_ATTRIBUTE_FILE			"File"
#define	EMOT_ATTRIBUTE_NAME			"Name"
#define	EMOT_ATTRIBUTE_SHORTCUT		"Shortcut"
#define	EMOT_GROUPA_NAME_VALUE		"Classic"
#define	EMOT_GROUPB_NAME_VALUE		"SupperMan"
#define EMOT_GROUPC_NAME_VALUE		"Favorate"

// 各个节点定义
#define EMOT_ROOT_NODE				"eIMEmot"
#define EMOT_GROUP_NODE				"Group"
#define EMOT_EMOT_NODE				"Emot"

// 主表情配置文件
#define EMOT_CONFIG_PATH			_T("$(Exe)Emoticon")     //by fxy
#define EMOT_CONFIG_PATH_FILE		EMOT_CONFIG_PATH _T("\\Emoticon.config")

#define EMOT_MAX_EMOT_COUNT			( 2048 )

// 只有在用户登录后，才有$(LoginId) 使用环境变量 "@UserName" 进行替换
#define EMOT_USER_CONFIG_PATH		_T("$(LoginId)Emoticon")    //by fxy
#define EMOT_USER_CONFIG_PATH_FILE	EMOT_USER_CONFIG_PATH _T("\\Emoticon.config")

class C_XmlEmoticon: public I_eIMXmlEmoticon, public C_XmlConfiger
{
public:
	virtual ~C_XmlEmoticon();

	DECALRE_PLUGIN_(C_XmlEmoticon)

	static C_XmlEmoticon& GetObject(BOOL bReopen = FALSE);// bReopen: 是否重新打开

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
	virtual MapFileEmots& GetMapFileEmots();

	//=============================================================================
	//Function:     GetEmoticons
	//Description:	Get the map of all emoticon, index by shortcut
	//
	//Parameter:
	//
	//Return:
	//		MapShortcutEmots	- map of emoticons
	//=============================================================================
	virtual MapShortcutEmots& GetMapShortcutEmots();

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
	virtual BOOL SetEmotName( const TCHAR* pszFile, const TCHAR* pszValue );

	//=============================================================================
	//Function:     SetEmotShortcut
	//Description:	Set Emoticon Shortcut
	//
	//Parameter:
	//	pszFile		- The value of file attribute
	//	pszValue		- Shortcut value, when is invalid, clear it
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed
	//=============================================================================
	virtual BOOL SetEmotShortcut( const TCHAR* pszFile, const TCHAR* pszValue );

	//=============================================================================
	//Function:     InsertEmot
	//Description:	
	//
	//Parameter:
	//	pszFile        -	File value (Not null)
	//	pszName        -	Name value (Optional)
	//	pszShortcut    -	Shortcut value (Optional)
	//	pszFilePos     -	Insert the end of parent if NULL,
	//						insert the first of parent if _T("")
	//						else insert the back of node which named [pszFilePos].
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed            
	//=============================================================================
	virtual int InsertEmot(const TCHAR* pszFile, const TCHAR* pszFilePos = NULL);

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
	virtual BOOL RemoveEmot(const TCHAR* pszFile);

	//=============================================================================
	//Function:     SwapEmot
	//Description:	
	//
	//Parameter:
	//	pszFileA    -	A File value
	//	pszFileB    - Another File value
	//
	//Return:
	//		TRUE		- Succeeded
	//		FALSE		- Failed                 
	//=============================================================================
	virtual BOOL SwapEmot(const TCHAR* pszFileA, const TCHAR* pszFileB);

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
	virtual BOOL IsExistEmot(const TCHAR* pszShortcut);

protected:
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
	//	EIMERR_FILE_NOT_CONFORM File not conform to requirement
	//=============================================================================
	int Open(LPCTSTR lpszFile, LPCTSTR lpszRediFile = NULL);

	void _GetEmoticons();
	void _GetEmoticons(const TiXmlDocument& xmlConfigur, bool bRedic);

	TiXmlElement* _GetXmlElementByFile(const TiXmlDocument& xmlConfigure, const TCHAR* pszFile);
	//TiXmlElement* GetXmlElementByShortcut(const TiXmlDocument& xmlConfigure, const TCHAR* pszShortcut);

private:
	C_XmlEmoticon();

	MapFileEmots		m_mapFileEmots;
	MapShortcutEmots	m_mapShortcutEmots;
	int					m_i32Order;

};

#endif  // __EIM_XMLEMOTICON_API_2013_12_31_BY_YFGZ__