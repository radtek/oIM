#pragma once

#include "public\Utils.h"
#include "public\XmlConfig.h"
#include "UICore\I_UICore.h"
#include "MainDlg.h"

class CDummyWindow;
class C_UICore 
	: public I_UICore
{
private:
	C_UICore(void);

public:
	~C_UICore(void);
	static C_UICore& GetObject();


public:
	// Interface ///////////////////////////////////////////////////////////////////////////
	virtual long AddRef(){return 1;}
	virtual long Release(){return 1;}
	virtual void OnFinalRelease(){}

	// I_EIMUICore ///////////////////////////////////////////////////////////////////////////
	virtual BOOL Run(DWORD dwFlag = 0);

	// HelpApi ///////////////////////////////////////////////////////////////////////////
	LPCTSTR	GetFontFamily(int i32Index);
	C_XmlConfig& GetErrorLang(LPCTSTR pszLangId, BOOL bRelease = FALSE);   // ONLY used by App load error information when init and switch language
	LPCTSTR	GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault = NULL);
	LPCTSTR	SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc);
	int   	SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag);
	LPCTSTR GetPath(LPCTSTR pszToken);
	BOOL	MakeDir(LPCTSTR lpszPath);

	HWND	UIGetMainWnd();
	HWND	UIGetPopupWnd();
	void	UISetInstance(HINSTANCE hInst);

	//BOOL	ShowInviteWnd(PS_InviteInfo psInfo);
	//BOOL	ShowInviteWndEx(PS_InviteInfoEx psInfo);
	//BOOL	ShowScreenshot(LPCTSTR pszSaveFile);
	//BOOL	ShowEmpWnd(QEID eid, HWND hParent = NULL, PPOINT pptScreenPos=NULL);
	//BOOL	ShowRelayWnd(LPCTSTR pszXml);
	//BOOL	ShowViewer(LPCTSTR pszImage, QFID fid=0);

	BOOL	UIUpdateHotkey(DWORD dwType, UINT u32Key, UINT u32Modifier);
	LPCTSTR	UIGetLanguage(int i32Lang = -1, BOOL bErrorInfo = FALSE);
	LPCTSTR	UIGetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszDefault=NULL, BOOL bEnableRedirect=TRUE );
	BOOL	UISetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue );
	int		UIGetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default=0, BOOL bEnableRedirect=TRUE );
	BOOL	UISetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value );
	LPCTSTR UIGetText( LPCTSTR lpszPath, LPTSTR pszDefault, BOOL bEnableRedirect=TRUE );
	BOOL	UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue );
	BOOL	UISaveConfig();
	LPCTSTR	UIGetAppVersion(BOOL bTrimBuildVer = TRUE);
	LPCTSTR UIGetSkinPath(int* pi32DpiRatio = NULL);
	BOOL	UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName=FALSE);

	DWORD	GetPurview(DWORD u32PurType, DWORD dwId=0);
	int		GetLoginState();
	DWORD	GetTimeNow();
	LPCTSTR	GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault = NULL);
	int		GetAttributeInt(LPCTSTR pszName, int i32Default = 0);
	int		AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId =1);
//	int		AddCmd(I_EIMCmd* pCmd, DWORD dwThreadId = 1, BOOL bAddRef = TRUE);
	int 	Command(LPCTSTR pszXml, DWORD dwThreadId = 1);
	BOOL	IsFace(LPCTSTR pszFace);


private:
	inline BOOL _GetPath(LPCTSTR pszToken, LPTSTR pszPath, DWORD dwSize);

	BOOL	ShowLoginWnd(DWORD dwFlag);
	DWORD	ClearHistoryFile(LPCTSTR pszFindPath, LPCTSTR pszPattern, DWORD dwBeforeDays = 31, int i32Level = 0);

private:
	HINSTANCE		m_hInstance;	// 资源DLL句柄（eIMUICore.dll）
	HANDLE			m_hLoginMutex;	// 唯一登录互斥变量
	HANDLE			m_hCompanyMutex;// 唯一公司互斥变量
//	CDummyWindow*	m_pwndDummy;	// 顶级哑元窗口
	CMainDlg		m_dlgMain;		// 主窗口
	BOOL			m_bInited;


	C_XmlConfig			m_AppCfg;
	C_XmlConfig			m_FontCfg;
	int					m_i32DpiRatio;
	SStringT			m_szSkinFile;
//	TCHAR				m_szCfgrValue[XML_VALUE_SIZE_MAX];
	BOOL				m_bDisableSmallFace;
};

