#pragma once

#include "public\Utils.h"
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

	// 事件响应函数
//	void OnServersChangedEvent(DWORD dwType);

	// I_EIMUISessionMgr /////////////////////////////////////////////////////////////////////
	//BOOL  CreateSessionView(PS_UISessionInfo_ psUISInfo_, BOOL bShow = TRUE);
	//DWORD GetSessionMember(QSID sid, VectSessionMember& vectSMember);
	//DWORD TopSession(QSID qsid, E_TopSession eTopOperate);
	//BOOL  GetGroupName(PS_UISessionInfo psUISInfo);
	//BOOL  CreateGroup(PS_UISessionInfo psUISInfo);
	//DWORD GetMsg(PS_MsgRecord psMsg);
	//DWORD GetSessionNewRecordStart(QSID& sid, E_SessionType eType);
	//DWORD GetSessionRecord(PS_UISessionInfo psUISInfo);

	//// 会话相关
	//DWORD	GetDeptEmps(QDID did, int i32Level, VectSessionMember& vectSMember, DWORD dwFlag = 0);
	//BOOL    GetSession(QSID sid, PS_SessionInfo psSessionInfo);
	//BOOL	LoadSession(PS_UISessionInfo psInfo, int i32Type = LOAD_SESSION_ALL);
	//DWORD	LoadSessions(E_SEFlag eFlag, DWORD dwFlag, CListUI* pList, int i32Limit = -1);
	//BOOL	GetLastMsgSummary(QSID sid, PS_UISessionInfo psSInfo);

	//BOOL	AddCommon(PS_UISessionInfo psSInfo);
	//BOOL	RemoveCommon(PS_UISessionInfo psInfo);

	//BOOL  AddInfo(PS_UISessionInfo psUISInfo, PS_MsgRecord psMsgInfo);
	//BOOL  AddError(PS_UISessionInfo psUISInfo, DWORD dwErrId);
	//int   SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag);
	//int   SendMsg(PS_UISessionInfo psUISInfo, LPCTSTR pszXml, DWORD dwFlag);
	//int	  GetSendMsg(PS_UISessionInfo psUISInfo, LPCTSTR pszXml, DWORD dwFlag, CDuiString& szSendMsg);
	//BOOL  ShowVirWindow(int i32Index, DWORD dwFlag = 0);
	//BOOL  Jump2Dept(DWORD dwId, BOOL bEmp);
	//void  ModifyGroupMember( PS_ModifyGroupMember psMGM);
	//BOOL  HideWindow(POINT* pptCursor, DWORD dwFlag = HIDE_ALL_WND);
	//DWORD IsMainWindowStaus(DWORD dwFlag);
	//DWORD GetPySortKey(const char* lpPySort);


	// I_EIMUICore ///////////////////////////////////////////////////////////////////////////
	virtual BOOL Run(DWORD dwFlag = 0);

	// HelpApi ///////////////////////////////////////////////////////////////////////////
	LPCTSTR	GetFontFamily(int i32Index);
	LPCTSTR	GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault = NULL);
	LPCTSTR	SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc);
	//QFID	GetQfid(const char* pszKey=NULL);
	//CDuiString GetDeptPath(QEID id, BOOL bDept=FALSE, int i32Level = -1);
	//LPCTSTR GetEmpInfo(QEID eid, DWORD dwType);

	//void	SendEvent(DWORD dwEventId, void* pvParam);
	//BOOL	RegisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin, E_EIMEventType eType);
	//BOOL	UnregisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin);

//	void	UICenterWindow(HWND hwnd);
	HWND	UIGetMainWnd();
	HWND	UIGetPopupWnd();
//	HWND	UISetActiveWindowForInput(HWND hWnd);
	void	UISetInstance(HINSTANCE hInst);
//	int		UIMessageBox(HWND hwndParent, LPCTSTR pszText, LPCTSTR pszCaption = NULL, UINT uType = MB_OK, LPCTSTR pszSkinFile = NULL);

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
	int		UIGetText( LPCTSTR lpszPath, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect=TRUE );
	BOOL	UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue );
	BOOL	UISaveConfig();
	LPCTSTR	UIGetAppVersion(BOOL bTrimBuildVer = TRUE);
	LPCTSTR UIGetSkinPath(int* pi32DpiRatio = NULL);

	BOOL		 UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName=FALSE);
//	I_EIMLogger* UIGetLogger(BOOL bAddRef=FALSE);

	DWORD	GetPurview(DWORD u32PurType, DWORD dwId=0);
	int		GetLoginState();
//	DWORD	IsLoginState(DWORD dwMask, QEID eid = 0);
	DWORD	GetTimeNow();
	LPCTSTR	GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault = NULL);
	int		GetAttributeInt(LPCTSTR pszName, int i32Default = 0);
	int		AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId =1);
//	int		AddCmd(I_EIMCmd* pCmd, DWORD dwThreadId = 1, BOOL bAddRef = TRUE);
	int 	Command(LPCTSTR pszXml, DWORD dwThreadId = 1);
//	BOOL	IsTypeOfId(QEID eid, E_TypeId eTypeId);
	BOOL	IsFace(LPCTSTR pszFace);
//	BOOL    GetFacePath(QEID eid, eIMString& szFacePath, BOOL bCheckExist, BOOL bSmall);
//	BOOL	DownloadFace(QEID eid, BOOL bCheck, BOOL bSmall);
//	BOOL	UpdateLogo(QEID eid, LPCTSTR pszLogoTpl, LPCTSTR pszLogo);
	BOOL	VerifyFile(LPCTSTR pszFile, LPCTSTR pszFileKey);

	// 状态订阅
//	int		GetAllStatus();
//	int		GetStatus(E_SubscribeOper eOp, UINT64 u64Id, int i32Type);
//	int		SubscribeStatus(E_SubscribeOper eOp, const VectSessionMember& vectSMember);

private:
	//static int __stdcall EnumEmpCbEx(PS_EmpInfo_ psEmpInfo, void* pvUserData, DWORD dwPyOrder, DWORD dwFixOrder);
	//static int __stdcall SessionMemberCb(QEID qeid, void* pvUserData);
	//static int __stdcall EnumMsgCb(QSID qsid, QMID qmid, QEID SenderID, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData);
	//static bool CompareMsgTimeGE(const S_MsgRecord& l, const S_MsgRecord& r);
	//static bool CompareMsgMidGE (const S_MsgRecord& l, const S_MsgRecord& r);
	//static bool CompareMsgMidNEQ(const S_MsgRecord& l, const S_MsgRecord& r);
	//static int __cdecl   CompareEmpItem(void* pvData, const void* pItem1, const void* pItem2);
	//static int __stdcall EnumSessionCb(PS_SessionInfo_ psInfo, void* pvUserData);
	//static int __stdcall LastMsgCb(QSID qsid, QMID qmid, QEID eidSender, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData);
	//static int __stdcall GetMsgCb(QSID qsid, QMID qmid, QEID SenderID, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData);

//	C_XmlLang*		GetErrorLang(LPCTSTR pszLangId, BOOL bRelease=FALSE);   // ONLY used by App load error information when init and switch language
//	PS_InitPlugins_ GetPlugins();

	BOOL	ShowLoginWnd(DWORD dwFlag);
	DWORD	ClearHistoryFile(LPCTSTR pszFindPath, LPCTSTR pszPattern, DWORD dwBeforeDays = 31, int i32Level = 0);
	BOOL	InitEngine();
	BOOL    InitEngineFont(TCHAR* pszFontEl, TCHAR* pszEngineFont );
//	char*	GetShortKey(char* pszMD5, char szResult[SHORT_KEY_LEN]);

private:
	HINSTANCE		m_hInstance;	// 资源DLL句柄（eIMUICore.dll）
	HANDLE			m_hLoginMutex;	// 唯一登录互斥变量
	HANDLE			m_hCompanyMutex;// 唯一公司互斥变量
//	CDummyWindow*	m_pwndDummy;	// 顶级哑元窗口
	CMainDlg		m_dlgMain;		// 主窗口
//	CComModule		m_Module;		// 初始化ATL
	BOOL			m_bInited;
//	Image*			m_pimgLogoSmallTpl;

//	// PluginDll
//	C_PluginDll	m_dllUILogger;		// UI logger dll
//	C_PluginDll	m_dllPluginMgr;		// Plugin manager dll
//	C_PluginDll	m_dllEngine;		// UI Engine dll
//	C_PluginDll	m_dllUISetting;		// UI Setting dll
//	C_PluginDll	m_dllUIMsgMgr;		// UI MsgMgr dll
////C_PluginDll	m_dllUISearch;		// UI Search dll
//	C_PluginDll	m_dllInternetFile;	// Internet file dll
//	C_PluginDll	m_dllUIPromptMgr;	// UI PromptMgr dll
//	C_PluginDll	m_dllPluginSms;
//	C_PluginDll	m_dllPluginBro;		//broadcast

	//// Interface
	//I_EIMLogger*			m_pIUILogger;		// UI File transfer logger interface
	//I_EIMPluginMgr*			m_pIPluginMgr;		// Plugin manager interface
	//I_EIMEventMgr*			m_pIEventMgr;		// Event manager interface
	//I_EIMEngine*			m_pIEngine;			// Engine interface
	//I_EIMUISetting*			m_pIUISetting;		// Setting interface
	//I_EIMUIMsgMgr*			m_pIUIMsgMgr;		// Message manager interface
	//I_EIMUIMsgCollectMgr*	m_pIUIMsgCollectMgr;
	//I_EIMUISearch*			m_pIUISearch;		// Search interface
	//I_EIMContacts*			m_pIContacts;		// Engine dll Contacts interface
	//I_eIMSingleInternetFile*m_pISingleNetFile;	// Single internet file interface
	//I_EIMUIPromptMgr*		m_pIUIPromptMgr;
	//I_EIMSubScribeMgr*		m_pISubscribeMgr;	// Subscribe interface
	//I_EIMMsgMgr*			m_pIMsgMgr;
	//I_eIMUISms*				m_pISms;
	//I_eIMUIBroadcast*		m_pIBro;
	//I_EIMSessionMgr*		m_pISessionMgr;
	//I_EIMUIWebAppMgr*		m_pIWebAppMgr;
	//I_EIMRoamDataMgr*		m_pIRoamMgr;
	//I_EIMCloudDisk*         m_pICloudDiskMgr;

	TCHAR				m_szAppVer[30];
	pugi::xml_document	m_XmlAppCfgr;
	int					m_i32DpiRatio;
	SStringT			m_szSkinFile;
//	TCHAR				m_szCfgrValue[XML_VALUE_SIZE_MAX];
	BOOL				m_bDisableSmallFace;
};

