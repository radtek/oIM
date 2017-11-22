// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMUISearch is the Search ui interface
// 
// Auth: hdw
// Date:  2014/7/18
// 
// History: 
//   
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIMUI_CORE_HEADER_2017_04_09_BY_YFGZ__
#define __IEIMUI_CORE_HEADER_2017_04_09_BY_YFGZ__

#include "Public\Plugin.h"
//#include "Public\XmlLang.h"
//#include "eIMEngine\IeIMCmd.h"
#include "eIMUICore\IeIMUIInviteWnd.h"

#define INAME_EIMUI_CORE_DLL	_T("eIMUICore.dll")	
#define INAME_EIMUI_CORE		_T("SXIT.EIMUI.CORE")	// I_EIMUISearch interface name

const TCHAR* const kLogoTplLarge = _T("Common/Logo@large.png");
const TCHAR* const kLogoTplMedium= _T("Common/Logo@medium.png");
const TCHAR* const kLogoTplSmall = _T("Common/Logo@small.png");

typedef enum tagTypeId
{
	eTYPE_ROBOT,			// 是机器人	(pISubScribeMgr->IsRobot(qeid))
	eYTPE_VIRGROUP,			// 虚拟组 (pISubScribeMgr->IsVirGroupMain(qeid))
}E_TypeId, *PE_TypeId;

#define RUN_FLAG_DISABLE_LOOP	(0x00000001)	// 禁用消息循环（嵌入时使用，由宿主程序提供消息循环）
#define RUN_FLAG_DISABLE_LOGIN	(0x00000002)	// 禁用自动显示登录窗口（嵌入时，可以由宿主程序提供或显示）
#define RUN_FLAG_MULTI_INSTANCE	(0x00000004)	// 允许打开多个实例
class __declspec(novtable) I_EIMUICore: public I_EIMPlugin
{
public:
	virtual BOOL Run(DWORD dwFlag = 0) = 0;

	// *** HelpApi ///////////////////////////////////////////////////////////////////////////
	virtual LPCTSTR GetFontFamily(int i32Index) = 0;
	virtual LPCTSTR	GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault) = 0;
	virtual LPCTSTR	SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc) = 0;
	virtual QFID	GetQfid(const char* pszKey=NULL) = 0;
	virtual BOOL	UpdateLogo(QEID eid, LPCTSTR pszLogoTpl, LPCTSTR pszLogo);
	virtual int   	SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag);

	// *** 事件
	virtual void	SendEvent(DWORD dwEventId, void* pvParam) = 0;
	virtual BOOL	RegisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin, E_EIMEventType eType) = 0;
	virtual BOOL	UnregisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin) = 0;
	
	// *** 窗口
	virtual void	UICenterWindow(HWND hwnd) = 0;
	virtual HWND	UIGetMainWnd() = 0;
	virtual HWND	UIGetPopupWnd() = 0;
	virtual HWND	UISetActiveWindowForInput(HWND hWnd) = 0;
	virtual int		UIMessageBox(HWND hwndParent, LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uType = MB_OK, LPCTSTR pszSkinFile = NULL) = 0;

	// *** 邀请窗口 
	virtual BOOL	ShowInviteWnd(PS_InviteInfo psInfo) = 0;
	virtual BOOL	ShowInviteWndEx(PS_InviteInfoEx psInfoEx) = 0;
	//virtual BOOL	ShowScreenshot(LPCTSTR pszSaveFile) = 0;

	// **** UIGetAttributeStrEx 【不是】线程安全的 ****
	virtual LPCTSTR UIGetAttributeStrEx( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszDefault=NULL, BOOL bEnableRedirect=TRUE ) = 0;
	virtual int		UIGetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect=TRUE ) = 0;
	virtual BOOL	UISetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue ) = 0;
	virtual int		UIGetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default=0, BOOL bEnableRedirect=TRUE ) = 0;
	virtual BOOL	UISetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value ) = 0;
	virtual int		UIGetText( LPCTSTR lpszPath, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect=TRUE ) = 0;
	virtual BOOL	UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue ) = 0;
	virtual BOOL	UISaveConfig() = 0;
	virtual LPCTSTR	UIGetAppVersion(BOOL bTrimBuildVer = TRUE);
	virtual LPCTSTR UIGetSkinPath(int* pi32DpiRatio = NULL);

	virtual BOOL	UIUpdateHotkey(DWORD dwType, UINT u32Key, UINT u32Modifier) = 0;
	virtual LPCTSTR UIGetLanguage(int i32Lang, BOOL bErrorInfo = FALSE) = 0;
	virtual BOOL	UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName=FALSE) = 0;

	// UIGetLogger 请都【不需要】添加引用计数，也不需要释放，统一由主模块负责生命周期。
	virtual I_EIMLogger* UIGetLogger(BOOL bAddRef=FALSE) = 0;

	// *** 引擎
	virtual int		AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId =1) = 0;
	virtual int		AddCmd(void* pICmd, DWORD dwThreadId = 1, BOOL bAddRef = TRUE) = 0;
	virtual int 	Command(LPCTSTR pszXml, DWORD dwThreadId = 1) = 0;
	virtual DWORD	GetTimeNow() = 0;
	virtual int		GetLoginState() = 0;
	virtual DWORD	GetPurview(DWORD u32PurType, DWORD dwId=0) = 0;
	virtual LPCTSTR GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault = NULL) = 0;
	virtual int		GetAttributeInt(LPCTSTR pszName, int i32Default = 0) = 0;
	virtual BOOL	IsTypeOfId(QEID qeid, E_TypeId eTypeId);
	virtual BOOL	IsFace(LPCTSTR pszFace) = 0;
	virtual BOOL	VerifyFile(LPCTSTR pszFile, LPCTSTR pszFileKey) = NULL;

};


#endif // __IEIMUI_CORE_HEADER_2017_04_09_BY_YFGZ__