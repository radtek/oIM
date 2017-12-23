// Description: I_UICore is the Search ui interface
// 
// Auth: lwq
// Date:  2017/12/22
// 
// History: 
//   
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __I_UICORE_HEADER_2017_12_22_BY_YFGZ__
#define __I_UICORE_HEADER_2017_12_22_BY_YFGZ__

#include <Windows.h>
#include "unknown\obj-ref-i.h"

#define INAME_UICORE_DLL	_T("UICore.dll")	// oIM.exe, 目前没有没有将 UICore分离成独立的DLL，直接为主程序
#define INAME_UICORE		_T("OIM.UI.CORE")	// I_UICore interface name

#define RUN_FLAG_DISABLE_LOOP	(0x00000001)	// 禁用消息循环（嵌入时使用，由宿主程序提供消息循环）
#define RUN_FLAG_DISABLE_LOGIN	(0x00000002)	// 禁用自动显示登录窗口（嵌入时，可以由宿主程序提供或显示）
#define RUN_FLAG_MULTI_INSTANCE	(0x00000004)	// 允许打开多个实例
class __declspec(novtable) I_UICore: public IObjRef
{
public:
	virtual BOOL Run(DWORD dwFlag = 0) = 0;

	// *** HelpApi ///////////////////////////////////////////////////////////////////////////
	virtual LPCTSTR GetFontFamily(int i32Index) = 0;
	virtual LPCTSTR	GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault) = 0;
	virtual LPCTSTR	SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc) = 0;
	virtual int   	SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag);


	// *** 窗口
	virtual HWND	UIGetMainWnd() = 0;
	virtual HWND	UIGetPopupWnd() = 0;

	// *** 邀请窗口 
//	virtual BOOL	ShowInviteWnd(PS_InviteInfo psInfo) = 0;
///	virtual BOOL	ShowInviteWndEx(PS_InviteInfoEx psInfoEx) = 0;
	//virtual BOOL	ShowScreenshot(LPCTSTR pszSaveFile) = 0;

	virtual LPCTSTR UIGetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszDefault=NULL, BOOL bEnableRedirect=TRUE ) = 0;
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

	// *** 引擎
	virtual int		AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId =1) = 0;
//	virtual int		AddCmd(void* pICmd, DWORD dwThreadId = 1, BOOL bAddRef = TRUE) = 0;
	virtual int 	Command(LPCTSTR pszXml, DWORD dwThreadId = 1) = 0;
	virtual DWORD	GetTimeNow() = 0;
	virtual int		GetLoginState() = 0;
	virtual DWORD	GetPurview(DWORD u32PurType, DWORD dwId=0) = 0;
	virtual LPCTSTR GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault = NULL) = 0;
	virtual int		GetAttributeInt(LPCTSTR pszName, int i32Default = 0) = 0;
//	virtual BOOL	IsTypeOfId(QEID qeid, E_TypeId eTypeId);
	virtual BOOL	IsFace(LPCTSTR pszFace) = 0;
	virtual BOOL	VerifyFile(LPCTSTR pszFile, LPCTSTR pszFileKey) = NULL;

};


#endif // __I_UICORE_HEADER_2017_12_22_BY_YFGZ__