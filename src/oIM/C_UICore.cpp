#include "StdAfx.h"
#include "C_UICore.h"


C_UICore& C_UICore::GetObject()
{
	static C_UICore s_UICore;
	return s_UICore;
}


C_UICore::C_UICore(void)
	: m_hInstance(NULL)
	, m_bInited(FALSE)
	, m_hLoginMutex(NULL)
	, m_hCompanyMutex(NULL)
	, m_i32DpiRatio(100)
{
	// 配置文件，必须最先加载
	eIMString szCfgrFile     = PATH_TOKEN_EXE_FILE_(APP_CONFIG_FILE);
	eIMString szRediCfgrFile = PATH_TOKEN_APPDATA_FILE_(APP_CONFIG_FILE);

	::eIMReplaceToken(szCfgrFile);
	::eIMReplaceToken(szRediCfgrFile);
	m_XmlAppCfgr.Open(szCfgrFile.c_str(), szRediCfgrFile.c_str());
	m_bDisableSmallFace = !!m_XmlAppCfgr.GetAttributeInt(PATH_FUNC, FUNC_ATTR_DISABLE_SMALL_FACE);
	ZERO_ARRAY_(m_szCfgrValue);

	CDuiString szAppDoc = UIGetAttributeStrEx(PATH_SETTING_BASIC, SETTING_FILE_ATTR_PATH);
	CDuiString szAppDocDef = UIGetAttributeStrEx(PATH_SETTING_BASIC, SETTING_FILE_ATTR_PATH, NULL, FALSE);
	if ( szAppDocDef.CompareNoCase(szAppDoc) != 0 )
		::SetEnvironmentVariable(PATH_TYPE_APPDOC, szAppDoc);

	memset(m_szAppVer, 0, sizeof(m_szAppVer));
}

C_UICore::~C_UICore(void)
{
	SAFE_CLOSE_HANDLE_(m_hCompanyMutex);

}

BOOL C_UICore::Run(DWORD dwFlag)
{
	if (!(RUN_FLAG_DISABLE_LOGIN & dwFlag))
	{	// 没有禁用自动显示登录窗口时
		if ( !ShowLoginWnd(dwFlag) )
			return FALSE;
	}

	// 创建主窗口
	SAFE_DELETE_PTR_(m_pwndMain);
	m_pwndMain = new C_MainWnd();
	CHECK_NULL_RET_(m_pwndMain, FALSE);
	if ( !m_pwndMain->Create(NULL, _T("eIMMainWnd"),  WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW | WS_EX_ACCEPTFILES))
	{
		SAFE_DELETE_PTR_(m_pwndMain);
		EIMLOGGER_ERROR_(m_pIUILogger, _T("Create C_MainWnd window failed"));
		return FALSE;
	}

	// 显示主窗口
	m_pwndMain->ShowWindow();
	m_pwndMain->OnLoginAfter(EIMERR_NO_ERROR);	// 需要调用一次，来初始化首次登录后的内容
	
	// 更新登录状态
	int nStatus = GetLoginState();
	if ( nStatus == eENGINE_STATUS_ONLINE )
	{
		S_LoginStatus sStatus = { EIMERR_STATUS_ONLINE };
		m_pwndMain->OnLoginStatus(&sStatus);
	}
	else if ( nStatus == eENGINE_STATUS_LEAVE )
	{
		S_LoginStatus sStatus = { EIMERR_STATUS_ONLEAVE };
		m_pwndMain->OnLoginStatus(&sStatus);
	}

	if (!(RUN_FLAG_DISABLE_LOOP & dwFlag))
	{	// 没有禁用消息循环时
		CPaintManagerUI::MessageLoop();
		SAFE_DELETE_PTR_(m_pwndMain);
	}

	return TRUE;
}

//=============================================================================
//Function:     GetFontFamily
//Description:	Get Family name by index, range[0,255].
//				You can pass the index from 0, to enumerate all the family, finished 
//				when return NULL
//
//Parameter:
//	i32Index    - The index of font to get, <0 to get the default family
//
//Return:
//		NULL	Reach the end, else is family name           
//=============================================================================
LPCTSTR C_UICore::GetFontFamily(int i32Index)
{
typedef std::map<int, eIMString>  MapFont;
typedef MapFont::iterator		  MapFontIt;
typedef std::pair<int, eIMString> PairFont;

	static MapFont mapFont;
	static eIMString szDefaultFont = FONT_DEFAULT_FAMILY;

	if ( i32Index == INT_MAX )
	{
		mapFont.clear();
		return NULL;
	}

	if (mapFont.size() <= 0)
	{	// initial it.
		C_XmlConfiger xml;
		eIMString szFontFile = PATH_TOKEN_EXE_FILE_(FONT_CONFIG_FILE);
		
		::eIMReplaceToken(szFontFile);
		if ( SUCCEEDED(xml.Open(szFontFile.c_str()) ))
		{
			int		i32Id;
			int		i32Idx = 0;
			TCHAR	szName[XML_VALUE_SIZE_MAX];
			TCHAR	szPath[MAX_PATH];

			if (xml.GetAttributeStr(FONT_ROOT_EL, FONT_DEFAULT_ATTR, szName, XML_VALUE_SIZE_MAX) > 0 )
			{	// Get default font
				szDefaultFont = szName;
				szName[0] = _T('\0');
			}

			_sntprintf(szPath, MAX_PATH, PATH_FONT_ITEM_FMT, i32Idx);
			while ( xml.GetAttributeStr(szPath, FONT_ITEM_NAME_ATTR, szName, XML_VALUE_SIZE_MAX) > 0 )
			{	// Get the font config file
				i32Id = xml.GetAttributeInt(szPath, FONT_ITEM_ID_ATTR );
				mapFont.insert(PairFont(i32Id, szName));
				
				szName[0] = _T('\0');
				_sntprintf(szPath, MAX_PATH, PATH_FONT_ITEM_FMT, ++i32Idx);
			}
		}
		else
		{
			TRACE_(_T("Open font config file[%s] failed"), szFontFile);
		}
	}

	if (i32Index < 0 )
		return szDefaultFont.c_str();

	MapFontIt it = mapFont.find(i32Index);
	if ( it != mapFont.end() )
		return it->second.c_str();

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//=============================================================================
//Function:     GetErrorLang
//Description:	Get the error information language config 
//
//Parameter:
//	pszLang	        NULL: Only to get the error language config information
//                  "CN": Load Chinese error language config
//                  "EN": Load English error language config
//
//Return:
//		NOT NULL  The reference Error Language object's pointer
//      NULL      Load error
//=============================================================================
C_XmlLang* C_UICore::GetErrorLang(LPCTSTR pszLangId, BOOL bRelease)   // ONLY used by App load error information when init and switch language
{
    static bool bLoaded = false;
    static C_XmlLang obj;

	if ( bLoaded && bRelease)
	{	// Release
		obj.Close();
		bLoaded = false;
	//	return NULL;
	}
  
    if ( pszLangId && bLoaded == false )
    {   // Format as: _T("$(Exe)\\Languages\\Errors.%s.config")
        eIMString szErrorLang = PATH_TOKEN_EXE_FILE_(_T("Languages\\Errors."));     
        szErrorLang += pszLangId ? pszLangId : _T("EN"); 
        szErrorLang += _T(".config");
		if (eIMReplaceToken(szErrorLang) > 0 && SUCCEEDED(obj.Open(szErrorLang.c_str(), pszLangId)) )
         {
            bLoaded = true;
            return &obj;
         }
    }
     
    if ( bLoaded )
        return &obj;
    
    TRACE_(_T("The error language of [%s] not been loaded!"), pszLangId);   
    return NULL;
}

//=============================================================================
//Function:     GetErrorLangInfo
//Description:	Get the error information by current language 
//
//Parameter:
//	dwErrId	        Error ID
//  lpszDefault     Default value
//
//Return:
//		Return the value of finded, else return lpszDefault 
//=============================================================================
LPCTSTR C_UICore::GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault)
{
    LPCTSTR     pszRet  = NULL;
    C_XmlLang* pXmlLang = GetErrorLang(NULL);
    
    if ( pXmlLang && (pszRet = pXmlLang->GetLangAttr(dwErrId)))
       return pszRet;
       
	if ( lpszDefault )
		return lpszDefault;

	if ( FAILED(dwErrId))
	{
		TRACE_(_T("Unknown error, err:0x%08x"), dwErrId);
		return _T("Unknown error");
	}

	return _T("");
}

//=============================================================================
//Function:     SetErrorLangInfo
//Description:	Get the error information by current language 
//
//Parameter:
//	dwErrId	        Error ID
//  lpszDefault     Default value
//
//Return:
//		Return the value of finded, else return lpszDefault 
//=============================================================================
LPCTSTR C_UICore::SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc)
{
    LPCTSTR     pszRet  = NULL;
    C_XmlLang* pXmlLang = GetErrorLang(NULL);
    
    if ( pXmlLang && (pszRet = pXmlLang->SetLangAttr(dwErrId, lpszDesc)))
       return pszRet;
       
	return lpszDesc;
}

QFID C_UICore::GetQfid(const char* pszKey)
{
	GUID guid = { 0 };
	QFID qfid  = 0;
	CoCreateGuid(&guid);
	qfid  = guid.Data1;
	qfid += qfid * 13 + guid.Data2 + 1;
	qfid += qfid * 13 + guid.Data3 + 1;
	qfid += qfid * 13 + guid.Data4[0] + 1;
	qfid += qfid * 13 + guid.Data4[1] + 1;
	qfid += qfid * 13 + guid.Data4[2] + 1;
	qfid += qfid * 13 + guid.Data4[3] + 1;
	qfid += qfid * 13 + guid.Data4[4] + 1;
	qfid += qfid * 13 + guid.Data4[5] + 1;
	qfid += qfid * 13 + guid.Data4[6] + 1;
	qfid += qfid * 13 + guid.Data4[7] + 1;

	TRACE_(_T("qfid:0x%I64X, uuid:{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			qfid, guid.Data1, guid.Data2, guid.Data3,  
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],  
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return qfid;
}

//=============================================================================
//Function:     SendEvent
//Description:	Send event
//
//Parameter:
//	dwEventId    - Event Id
//	pvParam      - Event parameter
//
//=============================================================================
void C_UICore::SendEvent(DWORD dwEventId, void* pvParam)
{
	CHECK_NULL_RETV_(m_pIEventMgr);
	m_pIEventMgr->SendEvent(dwEventId, pvParam);
}

//=============================================================================
//Function:     RegisterEvent
//Description:	Register event to the event manager,
//
//Parameter:
//	dwEventID    - Event ID, User event id start from TPE_USER_EVENT
//	pPlugin      - Plugin handle, which will receive or send registered event
//	eType        - Event type, see E_TEventType
//
//Return:
//      TRUE	succeed, else
//		FALSE
//=============================================================================
BOOL C_UICore::RegisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin, E_EIMEventType eType)
{
	CHECK_NULL_RET_(m_pIEventMgr, FALSE);
	return m_pIEventMgr->RegisterEvent(dwEventID, pPlugin, eType);
}

//=============================================================================
//Function:    UnregisterEvent
//Description:	Unregister event from event manager
//
//Parameter:
//	dwEventID    - Event ID 
//	pPlugin      - Plugin handle
//
//Return:
//      TRUE	succeed, else
//		FALSE
//=============================================================================
BOOL C_UICore::UnregisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin)
{
	if (m_pIEventMgr)
		return m_pIEventMgr->UnregisterEvent(dwEventID, pPlugin);

	return FALSE;
}

//=============================================================================
//Function:     SendEvent
//Description:	Send event
//
//Parameter:
//	dwEventId    - Event Id
//	pvParam      - Event parameter
//
//=============================================================================
void C_UICore::UICenterWindow(HWND hwnd)
{
	int cy = GetSystemMetrics(SM_CYSCREEN);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	RECT rWndRt;
	::GetClientRect(hwnd, &rWndRt);
	RECT rt;
	rt.left = (cx -rWndRt.right) / 2;
	rt.top = (cy - rWndRt.bottom)/2;
	if(rt.top < 0)
	{
		rt.top = 10;
	}

	::SetWindowPos(hwnd, NULL , rt.left, rt.top, 0, 0, SWP_NOSIZE);
}

HWND C_UICore::UIGetMainWnd()
{
	CHECK_NULL_RET_(m_pwndMain, NULL);
	return m_pwndMain->GetHWND();
}

HWND C_UICore::UIGetPopupWnd()
{
	CHECK_NULL_RET_( m_pwndDummy, NULL );
	return m_pwndDummy->GetHWND();
}

HWND C_UICore::UISetActiveWindowForInput(HWND hWnd)
{
	HWND	hForeWnd = NULL; 
	DWORD	dwForeID; 
	DWORD	dwCurID; 

	hForeWnd = ::GetForegroundWindow(); 
	dwCurID  = ::GetCurrentThreadId(); 
	dwForeID = ::GetWindowThreadProcessId( hForeWnd, NULL ); 
	::AttachThreadInput( dwCurID, dwForeID, TRUE); 
	if ( IsIconic(hWnd) )
	{	// 如果是最小化，就还原工
		::ShowWindow( hWnd, SW_RESTORE ); 
	}
	else
	{
		if ( IsZoomed(hWnd) )	// 是最大化的
			::ShowWindow( hWnd, SW_SHOWMAXIMIZED ); 
		else
			::ShowWindow( hWnd, SW_SHOWNORMAL ); 
	}

	::SetWindowPos( hWnd, HWND_TOPMOST,   0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE ); 
	::SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE ); 
	::SetForegroundWindow( hWnd ); 
	::AttachThreadInput( dwCurID, dwForeID, FALSE);

	return hForeWnd;
}

BOOL C_UICore::ShowInviteWnd(PS_InviteInfo psInfo)
{
	if ( psInfo->hParent == NULL )
		psInfo->hParent = ::GetActiveWindow();

	if ( psInfo->pfnCallback == NULL )
		psInfo->pfnCallback = InvaliteCallback;

	if ( C_eIMUIInviteWnd* pwndInvite = C_eIMUIInviteWnd::GetObject(TRUE, psInfo->hParent) )
		return pwndInvite->ShowInviteWnd(psInfo);

	return FALSE;
}

BOOL C_UICore::ShowInviteWndEx(PS_InviteInfoEx psInfoEx)
{
	if ( psInfoEx->hParent == NULL )
		psInfoEx->hParent = ::GetActiveWindow();

	if ( psInfoEx->pfnCallbackEx == NULL )
		psInfoEx->pfnCallbackEx = InvaliteCallbackEx;

	if ( C_eIMUIInviteWnd* pwndInvite = C_eIMUIInviteWnd::GetObject(TRUE, psInfoEx->hParent) )
		return pwndInvite->ShowInviteWndEx(psInfoEx);

	return FALSE;
}

BOOL C_UICore::ShowLoginWnd(DWORD dwFlag)
{
	// 创建并且显示 登录窗口
	C_LoginWnd* pwndLogin = new C_LoginWnd();
	CHECK_NULL_RET_(pwndLogin, FALSE);

	if (!pwndLogin->Create(NULL, _T("eIMLoginWnd"),  (UI_WNDSTYLE_DIALOG & ~WS_VISIBLE), WS_EX_APPWINDOW))
	{
		SAFE_DELETE_PTR_(pwndLogin);
		EIMLOGGER_ERROR_(m_pIUILogger, _T("Create C_LoginWnd window failed"));
		return FALSE;
	}
	
	if ( UIGetAttributeInt(PATH_APP, APP_ATTR_MULTI_INST) )
		dwFlag |= RUN_FLAG_MULTI_INSTANCE;

	if ( !pwndLogin->CheckMultiInstance(dwFlag & RUN_FLAG_MULTI_INSTANCE) )
	{
		pwndLogin->Close();
		return FALSE;
	}

	UICenterWindow(pwndLogin->GetHWND());
	if ( IDOK != pwndLogin->ShowModal() )
	{
		EIMLOGGER_ERROR_(m_pIUILogger, _T("User canceled login"));
		return FALSE;
	}

	return TRUE;
}

//=============================================================================
//Function:     ShowScreenshot
//Description:	Startup the screen capture procedure
//
//Parameter:
// szCutFile	- The capture file save path
//
//Return:
//	TRUE	- Succeeded
//  FALSE	- Failed
//=============================================================================
BOOL C_UICore::ShowScreenshot(LPCTSTR pszSaveFile)
{
	PROCESS_INFORMATION Processlnfo;
	STARTUPINFO Startupinfo;
	ZeroMemory(&Startupinfo,sizeof(Startupinfo));
	Startupinfo.cb = sizeof(Startupinfo);
	eIMString ScreenCapturePath = PATH_TOKEN_EXE_FILE_(_T("eIMUIScreenCaptureEXE.exe"));
	::eIMReplaceToken(ScreenCapturePath) ;

	if (CreateProcess(ScreenCapturePath.c_str(), (LPWSTR)pszSaveFile,
		NULL,NULL,FALSE,0,NULL,NULL,&Startupinfo,&Processlnfo))
	{
		//EIMLOGGER_INFO_(::eIMUIGetLogger(), _T("Start ScreenCapture process"));
		HANDLE hEvent = ::CreateEvent(NULL, TRUE, FALSE, _T("eIMScreenCapture.Event"));
		HANDLE ahWait[] = { Processlnfo.hProcess, hEvent };
		const int i32Count = sizeof(ahWait) / sizeof(ahWait[0]);
		while(true)
		{
			DWORD dwRet = ::MsgWaitForMultipleObjects(i32Count, ahWait, FALSE, 200, QS_HOTKEY);
			if ( dwRet == WAIT_OBJECT_0 || dwRet == WAIT_OBJECT_0+1)
				break;	// ScreenCapture reached
			else if ( dwRet == WAIT_OBJECT_0 + i32Count )
			{
				MSG msg = { 0 };
				if ( ::PeekMessage(&msg, NULL, WM_HOTKEY, WM_HOTKEY, PM_REMOVE))   
				{
					if ( msg.message == WM_HOTKEY )
					{
						TRACE_(_T("Skip the Hotkey, lParam:0x%08x, wParam:0x%08x"), msg.lParam, msg.wParam);
					}
					else
					{
						::TranslateMessage(&msg);     
						::DispatchMessage(&msg);  
					}
				}
			}
		}

		CloseHandle(Processlnfo.hThread);
		CloseHandle(Processlnfo.hProcess);
		CloseHandle(hEvent);
		//EIMLOGGER_INFO_(::eIMUIGetLogger(), _T("Stop ScreenCapture process"));
		return TRUE;
	}
	else
	{
		//EIMLOGGER_ERROR_(::eIMUIGetLogger(), _T("Run the ScreenCapture process fails"));
		return FALSE;
	}
	return FALSE;
}

BOOL C_UICore::ShowEmpWnd(QEID eid, HWND hParent, PPOINT pptScreenPos)
{
	if ( C_MainEmpInfoWnd* pwndEmpInfo = C_MainEmpInfoWnd::GetObject(TRUE, hParent) )
		return pwndEmpInfo->ShowEmpWnd(eid, pptScreenPos);

	return FALSE;
}

BOOL C_UICore::ShowRelayWnd(LPCTSTR pszXml)
{
	if ( C_RelayWnd* pwndRelay = C_RelayWnd::GetObject(TRUE, ::GetActiveWindow()) )
		return pwndRelay->ShowRelay(pszXml);

	return FALSE;
}

void C_UICore::UISetInstance(HINSTANCE hInst)
{
	m_hInstance = hInst;
}

//=============================================================================
//Function:     UIGetAttributeStrEx
//Description:	Get a string value of configuration, ONLY Work in UI thread
//Parameter:
//	lpszPath   		- Path of element
//  lpszAttrName  	- Attribute name
//	pszDefault		- Default value
//	bEnableRedirect	- Redirect flag
//Return:
//  The value
//=============================================================================
LPCTSTR C_UICore::UIGetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszDefault, BOOL bEnableRedirect )
{
//	if ( m_XmlAppCfgr.GetAttributeStr(lpszPath, lpszAttrName, m_szCfgrValue, COUNT_OF_ARRAY_(m_szCfgrValue), bEnableRedirect) > 0 )
//		return m_szCfgrValue;

	return _T("");
}

BOOL C_UICore::UISetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue )
{
	return m_XmlAppCfgr.SetAttributeStr(lpszPath, lpszAttrName, lpszValue);
}

int C_UICore::UIGetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default, BOOL bEnableRedirect )
{
	return m_XmlAppCfgr.GetAttributeInt(lpszPath, lpszAttrName, i32Default, bEnableRedirect);
}

BOOL C_UICore::UISetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value )
{
	return m_XmlAppCfgr.SetAttributeInt(lpszPath, lpszAttrName, i32Value);
}

int C_UICore::UIGetText( LPCTSTR lpszPath, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect )
{
	return m_XmlAppCfgr.GetText(lpszPath, pszValue, i32Size, bEnableRedirect);
}

BOOL C_UICore::UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue )
{
	return m_XmlAppCfgr.SetText(lpszPath, lpszValue);
}

BOOL C_UICore::UISaveConfig()
{
	return m_XmlAppCfgr.Save();
}

DWORD C_UICore::GetTimeNow()
{
	CHECK_NULL_RET_(m_pIEngine, (DWORD)time(NULL));
	return m_pIEngine->GetTimeNow();
}

I_EIMLogger* C_UICore::UIGetLogger(BOOL bAddRef)
{
	if ( m_pIUILogger && bAddRef )
	{
		m_pIUILogger->AddRef();
		return m_pIUILogger;
	}

	return m_pIUILogger;
}

int C_UICore::GetLoginState()
{
	CHECK_NULL_RET_(m_pIEngine, 0);
	return m_pIEngine->GetPurview(GET_LOGIN_STATUS);
}

DWORD C_UICore::IsLoginState(DWORD dwMask, QEID eid)
{
	CHECK_NULL_RET_(m_pIEngine, FALSE);
	DWORD  dwRet = 0;
	if ( PS_EmpInfo_ psEmpInfo = m_pIContacts->GetEmp(eid ? eid : m_pIEngine->GetPurview(GET_LOGIN_QEID)) )
	{
		DWORD dwStatus = psEmpInfo->bitStatus;

		if ((dwMask & LOGIN_STATUS_ONLINE) && (dwStatus == eENGINE_STATUS_ONLINE))
			dwRet |= LOGIN_STATUS_ONLINE;

		if ((dwMask & LOGIN_STATUS_LEAVE) && (dwStatus == eENGINE_STATUS_LEAVE))
			dwRet |= LOGIN_STATUS_LEAVE;

		if ((dwMask & LOGIN_STATUS_OFFLINE) && (dwStatus == eENGINE_STATUS_OFFLINE))
			dwRet |= LOGIN_STATUS_OFFLINE;

		if ((dwMask & LOGIN_STATUS_QUIT) && (dwStatus == eENGINE_STATUS_QUIT))
			dwRet |= LOGIN_STATUS_QUIT;

		if ((dwMask & LOGIN_STATUS_KICK) &&	(dwStatus == eENGINE_STATUS_KICK))
			dwRet |= LOGIN_STATUS_KICK;

		if ((dwMask & LOGIN_STATUS_FORBIDDEN) && (dwStatus == eENGINE_STATUS_FORBIDDEN))
			dwRet |= LOGIN_STATUS_FORBIDDEN;

		if ((dwMask & LOGIN_TERM_ANDROID) && (psEmpInfo->bitLoginType == IME_EMP_TERM_ANDROID))
			dwRet |= LOGIN_TERM_ANDROID;

		if ((dwMask & LOGIN_TERM_IOS) && (psEmpInfo->bitLoginType == IME_EMP_TERM_IOS))
			dwRet |= LOGIN_TERM_IOS;

		if ((dwMask & LOGIN_TERM_PC) && (psEmpInfo->bitLoginType == IME_EMP_TERM_PC))
			dwRet |= LOGIN_TERM_PC;
	}

	return dwRet;
}

BOOL C_UICore::UIUpdateHotkey(DWORD dwType, UINT u32Key, UINT u32Modifier)
{
	return m_pwndMain->UpdateHotkey(dwType, u32Key, u32Modifier);
}

DWORD C_UICore::GetPurview(DWORD u32PurType, DWORD dwId)
{
	CHECK_NULL_RET_(m_pIEngine, 0);

	// 特殊ID，使用配置的方式，以便公司化
	if ( u32PurType == CHECK_SYSTEM_ACCOUNT )
	{
		return (dwId == GetPurview(GET_SYSTEM_MSG_ID) ||
				dwId == GetPurview(GET_SYSTEM_MSG_RTX_ID) ||
				dwId == GetPurview(GET_SYSTEM_MSG_MAIL_ID) ||
				dwId == GetPurview(GET_SYSTEM_PRINTER_ID) );
	}
	else if (u32PurType == CHECK_SPECIAL_ACCOUNT)
	{
		return (GetPurview(CHECK_SYSTEM_ACCOUNT, dwId) ||
				dwId == GetPurview(GET_ITSUPPORT_ID) || 
				dwId == GetPurview(GET_FILETRANSFER_ID) || 
				dwId == GetPurview(GET_IROBOT_QEID) );
	}
	else if (u32PurType == GET_ITSUPPORT_ID)
	{	
		static QEID eidItSupport = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_ITSUPPORT);
		return eidItSupport;
	}
	else if (u32PurType ==  GET_FILETRANSFER_ID)
	{
		static QEID eidFileTransfer = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_FILETRANSFER);
		return eidFileTransfer;
	}
	else if (u32PurType == GET_IROBOT_QEID)
	{
		static QEID eidRobot = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_IROBOT);
		return eidRobot;
	}
	else if (u32PurType == GET_SYSTEM_MSG_ID)
	{
		static QEID idNotice = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_SYSTEM_MSG);
		return idNotice;
	}
	else if (u32PurType == GET_SYSTEM_MSG_RTX_ID)
	{
		static QEID idRtx = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_SYSTEM_MSG_RTX);
		return idRtx;
	}
	else if (u32PurType == GET_SYSTEM_MSG_MAIL_ID)
	{
		static QEID idMail = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_SYSTEM_MSG_MAIL);
		return idMail;
	}
	else if (u32PurType == GET_SYSTEM_PRINTER_ID)
	{
		static QEID idPrinter = m_XmlAppCfgr.GetAttributeInt(PATH_SPECIALID, SPECIALID_ATTR_PRINTER);
		return idPrinter;
	}

	return m_pIEngine->GetPurview(u32PurType);
}

LPCTSTR C_UICore::UIGetLanguage(int i32Lang, BOOL bErrorInfo)
{
	if ( i32Lang < 0 )
	{
		i32Lang = m_XmlAppCfgr.GetAttributeInt(PATH_SETTING_BASIC, SETTING_BASIC_ATTR_LANGUAGE, -1);
		if ( i32Lang < 0 )
		{	// Get from Setup setted the language value
			TCHAR szLang[32] = { _T("1033") };
			i32Lang = m_XmlAppCfgr.GetAttributeStr(PATH_APP, APP_ATTR_LANGUAGE, szLang, COUNT_OF_ARRAY_(szLang));
			if ( _tcsicmp(szLang,  _T("2052")) == 0 || 
				StrStrI(szLang, _T("Chinese")) == 0 )
				i32Lang = eSYSSETTING_CHINESE;	// Chinese
			else
				i32Lang = eSYSSETTING_ENGLISH;	// English

			m_XmlAppCfgr.SetAttributeInt(PATH_SETTING_BASIC, SETTING_BASIC_ATTR_LANGUAGE, i32Lang);
		}
	}

	TCHAR* pszLang = NULL;
	switch( i32Lang )
	{	// Maybe add other language here...
	case eSYSSETTING_CHINESE:
		pszLang = bErrorInfo ? _T("CN") : _T("LangChinese");
		break;
	case eSYSSETTING_ENGLISH:
	default:	//  eSYSSETTING_ENGLISH:
		pszLang = bErrorInfo ? _T("EN") : _T("LangEnglish");
		break;
	}

	TRACE_(_T("Language: %d, %s"), i32Lang, pszLang);
	return pszLang;
}

LPCTSTR C_UICore::GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault)
{
	CHECK_NULL_RET_( m_pIEngine, NULL );
	return m_pIEngine->GetAttributeStr(pszName, pszDefault);
}

int C_UICore::GetAttributeInt(LPCTSTR pszName, int i32Default)
{
	CHECK_NULL_RET_( m_pIEngine, 0 );
	return m_pIEngine->GetAttributeInt(pszName, i32Default);
}


BOOL C_UICore::UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName)
{
	CHECK_NULL_RET_(m_pwndMain, FALSE);
	return m_pwndMain->GetMsgSummary(pszXmlMsg, pszMsgSummary, i32Size, bPrefixName);
}

int C_UICore::AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId)
{
	CHECK_NULL_RET_( m_pIEngine, EIMERR_NO_INTERFACE);
	return m_pIEngine->AddCmd((E_ThreadIdx)dwThreadId, dwCmdId, pvData);
}

//int C_UICore::AddCmd(I_EIMCmd* pCmd, DWORD dwThreadId, BOOL bAddRef)
//{
//	CHECK_NULL_RET_( m_pIEngine, EIMERR_NO_INTERFACE);
//	return m_pIEngine->AddCmd((E_ThreadIdx)dwThreadId, pCmd, bAddRef);
//}

int C_UICore::Command(LPCTSTR pszXml, DWORD dwThreadId)
{
	CHECK_NULL_RET_( m_pIEngine, EIMERR_NO_INTERFACE);
	return m_pIEngine->Command(pszXml, (E_ThreadIdx)dwThreadId);
}

BOOL C_UICore::IsTypeOfId(QEID qeid, E_TypeId eTypeId)
{
	switch(eTypeId)
	{
	case eTYPE_ROBOT:
		return m_pISubscribeMgr->IsRobot(qeid);
	case eYTPE_VIRGROUP:
		return m_pISubscribeMgr->IsVirGroupMain(qeid);
	default:
		ASSERT_(FALSE);
		return FALSE;
	}
}

BOOL C_UICore::IsFace(LPCTSTR pszFace)
{
	CHECK_NULL_RET_( m_pIEngine, FALSE);
	return m_pIEngine->IsFace(pszFace);
}

DWORD C_UICore::ClearHistoryFile(LPCTSTR pszFindPath, LPCTSTR pszPattern, DWORD dwBeforeDays, int i32Level)
{
	WIN32_FIND_DATA ffd = { 0 };
	eIMString	szPath;
	FILETIME	fileTimeNow = { 0 };
	UINT64		u64DaysofNs = (UINT64)dwBeforeDays * 864000000000I64;	// 864000000000I64 is 1days of 100-ns
	UINT64		u64FileTime = 0;
	DWORD		dwCount     = 0;

	::GetSystemTimeAsFileTime(&fileTimeNow);
	::eIMGetFullPath(pszFindPath, NULL, pszPattern, szPath, FALSE);

	BOOL	bRet  = TRUE;
	HANDLE	hFind = FindFirstFile(szPath.c_str(), &ffd);
	UINT64	u64TimeNow  = (((UINT64)fileTimeNow.dwHighDateTime << 32) + (UINT64)fileTimeNow.dwLowDateTime);
	if ( hFind == INVALID_HANDLE_VALUE )
		return dwCount;

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Is file
		{
			if ( i32Level > 0 &&						// Need find sub folder
				_tcsicmp(ffd.cFileName, _T(".")) &&		// Not folder "."
				_tcsicmp(ffd.cFileName, _T("..")) )		// Not folder ".."
			{	// Sub folder
				::eIMGetFullPath(pszFindPath, ffd.cFileName, _T(""), szPath, FALSE);
				dwCount += ClearHistoryFile(szPath.c_str(), pszPattern, dwBeforeDays, i32Level - 1);
			}

			continue;
		}

		u64FileTime = (((UINT64)ffd.ftCreationTime.dwHighDateTime << 32) + (UINT64)ffd.ftCreationTime.dwLowDateTime);
		if ( u64TimeNow - u64FileTime > u64DaysofNs )	// Only subject result greater than the days
		{
			::eIMGetFullPath(pszFindPath, NULL, ffd.cFileName, szPath, FALSE);
			if( ::DeleteFile(szPath.c_str()) )
			{
				dwCount++;
				EIMLOGGER_DEBUG_(m_pIUILogger, _T("Delete history file[%s] succeed!"), szPath.c_str());
			}
			else
			{
				EIMLOGGER_ERROR_(m_pIUILogger, _T("Delete history file[%s] failed! error:0x%08x"), szPath.c_str(), ::GetLastError());
			}
		}
	}while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return dwCount;
}


void C_UICore::OnServersChangedEvent(DWORD dwType)
{		
	DWORD		dwPort;
	TCHAR		szValue[XML_VALUE_SIZE_MAX] = { 0 };
	TCHAR		szValue2[XML_VALUE_SIZE_MAX]= { 0 };
	TCHAR		szValue3[XML_VALUE_SIZE_MAX]= { 0 };

	if ( m_XmlAppCfgr.GetAttributeInt(PATH_ENGINE, SETTING_ATTR_SWITCH_FILE_SERVER) )
	{
		ASSERT_(m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_(1),     SETTING_ATTR_UPLOAD_URL, szValue, XML_VALUE_SIZE_MAX));
		ASSERT_(m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_BT(1),   SETTING_ATTR_UPLOAD_URL, szValue, XML_VALUE_SIZE_MAX));
		ASSERT_(m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_BT_(1), SETTING_ATTR_UPLOAD_URL, szValue, XML_VALUE_SIZE_MAX));
		ASSERT_(m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_(1),    SETTING_ATTR_UPLOAD_URL, szValue, XML_VALUE_SIZE_MAX));
		ASSERT_(m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_HEAD_(1),     SETTING_ATTR_UPLOAD_URL, szValue, XML_VALUE_SIZE_MAX));

		m_XmlAppCfgr.SetAttributeInt(PATH_ENGINE, SETTING_USE_SERVER, (dwType & SERVERS_SWITCH_FILE_SVR) ? 1 : 0);
	}

	if ( dwType & SERVERS_TYPE_RESET )
	{	// Need delete (1) at first, and then (0); else delete(0) twice
		if ( SERVERS_TYPE_IM & dwType )
		{
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_IM_(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_IM_(0));
		}

		if ( SERVERS_TYPE_P2P & dwType )
		{
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_P2P_(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_P2P_(0));
		}

		if ( SERVERS_TYPE_FILE & dwType )
		{
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_FILE_(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_FILE_(0));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_FILE_BT(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_FILE_BT(0));
		}

		if ( SERVERS_TYPE_IMAGE & dwType )
		{
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_IMAGE_(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_IMAGE_(0));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_IMAGE_BT_(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_IMAGE_BT_(0));
		}

		if ( SERVERS_TYPE_HEAD & dwType )
		{
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_HEAD_(1));
			m_XmlAppCfgr.Remove(PATH_SETTING_SERVER_HEAD_(0));
		}

		m_XmlAppCfgr.Save();
		return;
	}

	DWORD dwConntctType = m_XmlAppCfgr.GetAttributeInt(PATH_SETTING_SERVER, SETTING_ATTR_CONNECT_TYPE, CONNECT_TYPE_IP);
	if ( dwType & SERVERS_TYPE_IM)
	{
		CDuiString szAttr = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_IP : SETTING_ATTR_DOMAIN;

		// IM Server 1
		if ( m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IM_(0), szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			(dwPort = m_XmlAppCfgr.GetAttributeInt(PATH_SETTING_SERVER_IM_(0), SETTING_ATTR_PORT)))
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_SERVER_IP, szValue);
			m_pIEngine->SetAttributeInt(IME_ATTRIB_SERVER_PORT, dwPort);
		}

		// IM Server 2
		if ( m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IM_(1), szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			(dwPort = m_XmlAppCfgr.GetAttributeInt(PATH_SETTING_SERVER_IM_(1), SETTING_ATTR_PORT)))
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_SERVER_IPB, szValue);
			m_pIEngine->SetAttributeInt(IME_ATTRIB_SERVER_PORTB, dwPort);
		}
	}

	// The fellow config: if is SWITCH server, will try REMOTE server at first, then try main server if failed
	if ( dwType & SERVERS_TYPE_P2P)
	{	// P2P server
		CDuiString szPath  = (dwType & SERVERS_SWITCH_FILE_SVR) ? PATH_SETTING_SERVER_P2P_(1) : PATH_SETTING_SERVER_P2P_(0);
		if ( m_XmlAppCfgr.GetAttributeStr(szPath, SETTING_ATTR_IP, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			(dwPort = m_XmlAppCfgr.GetAttributeInt(szPath, SETTING_ATTR_PORT)))
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_P2P_IP,   szValue);
			m_pIEngine->SetAttributeInt(IME_ATTRIB_P2P_PORT, dwPort);
		}
		else if ( (dwType & SERVERS_SWITCH_FILE_SVR) &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_P2P_(0), SETTING_ATTR_IP, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			(dwPort = m_XmlAppCfgr.GetAttributeInt( PATH_SETTING_SERVER_P2P_(0), SETTING_ATTR_PORT)))
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_P2P_IP,   szValue);
			m_pIEngine->SetAttributeInt(IME_ATTRIB_P2P_PORT, dwPort);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pIUILogger, _T("Set the P2P server config failed, please check the config"));
		}
	}

	if ( dwType & SERVERS_TYPE_FILE)
	{	// File transfer server
		CDuiString szAttr  = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP : SETTING_ATTR_UPLOAD_URL;
		CDuiString szAttr2 = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_DOWNLOAD_IP : SETTING_ATTR_DOWNLOAD_URL;
		CDuiString szPath  = (dwType & SERVERS_SWITCH_FILE_SVR) ? PATH_SETTING_SERVER_FILE_(1) : PATH_SETTING_SERVER_FILE_(0);
		if ( m_XmlAppCfgr.GetAttributeStr(szPath, szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			 m_XmlAppCfgr.GetAttributeStr(szPath, szAttr2, szValue2, XML_VALUE_SIZE_MAX) > 0 )
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_UPLOAD, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD, szValue2);
		}
		else if ( (dwType & SERVERS_SWITCH_FILE_SVR) &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_(0), szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_(0), szAttr2, szValue2, XML_VALUE_SIZE_MAX) > 0 )
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_UPLOAD, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD, szValue2);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pIUILogger, _T("Set the FILE transfer server config failed, please check the config"));
		}

		CDuiString szAttrBT  = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP_TOKEN : SETTING_ATTR_UPLOAD_URL_TOKEN;
		CDuiString szAttr2BT = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP_BT    : SETTING_ATTR_UPLOAD_URL_BT;
		CDuiString szAttr3BT = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_DOWNLOAD_IP_BT  : SETTING_ATTR_DOWNLOAD_URL_BT;
		CDuiString szPathBT  = (dwType & SERVERS_SWITCH_FILE_SVR) ? PATH_SETTING_SERVER_FILE_BT(1) : PATH_SETTING_SERVER_FILE_BT(0);
		if( m_XmlAppCfgr.GetAttributeStr(szPathBT, szAttrBT,  szValue,  XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(szPathBT, szAttr2BT, szValue2, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(szPathBT, szAttr3BT, szValue3, XML_VALUE_SIZE_MAX) > 0)
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_UPLOAD_TOKEN, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_UPLOAD_BT,    szValue2);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD_BT,  szValue3);
		}
		else if( (dwType & SERVERS_SWITCH_FILE_SVR) &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_BT(0), szAttrBT,  szValue,  XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_BT(0), szAttr2BT, szValue2, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_FILE_BT(0), szAttr3BT, szValue3, XML_VALUE_SIZE_MAX) > 0)
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_UPLOAD_TOKEN, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_UPLOAD_BT,    szValue2);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD_BT,  szValue3);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pIUILogger, _T("Set the BT FILE transfer server config failed, please check the config"));
		}
	}

	if ( dwType & SERVERS_TYPE_IMAGE)
	{	// Image server
		CDuiString szAttr  = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP : SETTING_ATTR_UPLOAD_URL;
		CDuiString szAttr2 = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_DOWNLOAD_IP : SETTING_ATTR_DOWNLOAD_URL;
		CDuiString szPath  = (dwType & SERVERS_SWITCH_FILE_SVR) ? PATH_SETTING_SERVER_IMAGE_(1) : PATH_SETTING_SERVER_IMAGE_(0);
		if ( m_XmlAppCfgr.GetAttributeStr(szPath, szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			 m_XmlAppCfgr.GetAttributeStr(szPath, szAttr2, szValue2, XML_VALUE_SIZE_MAX) > 0 )
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_DOWNLOAD, szValue2);
		}
		else if ( (dwType & SERVERS_SWITCH_FILE_SVR) &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_(0), szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_(0), szAttr2, szValue2, XML_VALUE_SIZE_MAX) > 0 )
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_DOWNLOAD, szValue2);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pIUILogger, _T("Set the IMAGE transfer server config failed, please check the config"));
		}

		CDuiString szAttrBT  = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP_TOKEN : SETTING_ATTR_UPLOAD_URL_TOKEN;
		CDuiString szAttr2BT = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP_BT    : SETTING_ATTR_UPLOAD_URL_BT;
		CDuiString szAttr3BT = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_DOWNLOAD_IP_BT  : SETTING_ATTR_DOWNLOAD_URL_BT;
		CDuiString szPathBT  = (dwType & SERVERS_SWITCH_FILE_SVR) ? PATH_SETTING_SERVER_IMAGE_BT_(1) : PATH_SETTING_SERVER_IMAGE_BT_(0);
		if( m_XmlAppCfgr.GetAttributeStr(szPathBT, szAttrBT,  szValue,  XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(szPathBT, szAttr2BT, szValue2, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(szPathBT, szAttr3BT, szValue3, XML_VALUE_SIZE_MAX) > 0)
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD_TOKEN, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD_BT,    szValue2);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_DOWNLOAD_BT,  szValue3);
		}
		else if( (dwType & SERVERS_SWITCH_FILE_SVR) &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_BT_(0), szAttrBT,  szValue,  XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_BT_(0), szAttr2BT, szValue2, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_IMAGE_BT_(0), szAttr3BT, szValue3, XML_VALUE_SIZE_MAX) > 0)
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD_TOKEN, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD_BT,    szValue2);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_IMAGE_DOWNLOAD_BT,  szValue3);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pIUILogger, _T("Set the BT IMAGE transfer server config failed, please check the config"));
		}
	}

	if(dwType & SERVERS_TYPE_HEAD)
	{
		CDuiString szAttr  = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_UPLOAD_IP : SETTING_ATTR_UPLOAD_URL;
		CDuiString szAttr2 = (dwConntctType == CONNECT_TYPE_IP) ? SETTING_ATTR_DOWNLOAD_IP : SETTING_ATTR_DOWNLOAD_URL;
		CDuiString szPath  = (dwType & SERVERS_SWITCH_FILE_SVR) ? PATH_SETTING_SERVER_HEAD_(1) : PATH_SETTING_SERVER_HEAD_(0);
		if ( m_XmlAppCfgr.GetAttributeStr(szPath, szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			 m_XmlAppCfgr.GetAttributeStr(szPath, szAttr2, szValue2, XML_VALUE_SIZE_MAX) > 0 )
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_HEAD_UPLOAD, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_HEAD_DOWNLOAD, szValue2);
		}
		else if ( (dwType & SERVERS_SWITCH_FILE_SVR) &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_HEAD_(0), szAttr, szValue, XML_VALUE_SIZE_MAX) > 0 &&
			m_XmlAppCfgr.GetAttributeStr(PATH_SETTING_SERVER_HEAD_(0), szAttr2, szValue2, XML_VALUE_SIZE_MAX) > 0 )
		{
			m_pIEngine->SetAttributeStr(IME_ATTRIB_HEAD_UPLOAD, szValue);
			m_pIEngine->SetAttributeStr(IME_ATTRIB_HEAD_DOWNLOAD, szValue2);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pIUILogger, _T("Set the HEAD image transfer server config failed, please check the config"));
		}
	}

	m_XmlAppCfgr.Save();
}

LPCTSTR C_UICore::UIGetAppVersion(BOOL bTrimBuildVer)
{	// Maybe read from App's version resource too when config is none...
	DWORD adwVer[4] = { 0 };
	m_XmlAppCfgr.GetAttributeStr(PATH_APP, APP_ATTR_VERSION, m_szAppVer, COUNT_OF_ARRAY_(m_szAppVer), FALSE);
	_stscanf( m_szAppVer, _T("%d.%d.%d.%d"), &adwVer[0], &adwVer[1], &adwVer[2], &adwVer[3] );
	if ( bTrimBuildVer )
		_sntprintf(m_szAppVer, COUNT_OF_ARRAY_(m_szAppVer), _T("%d.%02d.%03d"), adwVer[0], adwVer[1], adwVer[2]);
	else
		_sntprintf(m_szAppVer, COUNT_OF_ARRAY_(m_szAppVer), _T("%d.%02d.%03d.%03d"), adwVer[0], adwVer[1], adwVer[2], adwVer[3]);

	return m_szAppVer;
}

int __stdcall C_UICore::EnumSessionCb(PS_SessionInfo_ psInfo, void* pvUserData)
{
	CHECK_NULL_RET_(psInfo, 0);
	if ( PS_EnumSessionInfo psESI = (PS_EnumSessionInfo)pvUserData )
	{
		psESI->vectSInfo.push_back(*psInfo);
		if ( psESI->i32Limit < 0 || (int)psESI->vectSInfo.size() < psESI->i32Limit )
			return 1;
	}

	return 0;
}

int __stdcall C_UICore::LastMsgCb(QSID qsid, QMID qmid, QEID eidSender, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData)
{
	if ( PS_UISessionInfo psSInfo = (PS_UISessionInfo)pvUserData)
	{
		psSInfo->dwLastMsgTime = dwTimestamp;
		psSInfo->eidLastMsg   = eidSender;
		psSInfo->szLastSummary = pszXml;
	}

	return 0;
}

BOOL C_UICore::GetLastMsgSummary(QSID sid, PS_UISessionInfo psSInfo)
{
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.GetLastMsgSummary(sid, psSInfo);
}

BOOL C_UICore::GetSession(QSID sid, PS_SessionInfo psSessionInfo)
{
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.GetSession(sid, psSessionInfo);
}

bool C_UICore::CompareMsgTimeGE(const S_MsgRecord& l, const S_MsgRecord& r)
{
	return (l.dwSendTime < r.dwSendTime);
}

bool C_UICore::CompareMsgMidGE(const S_MsgRecord& l, const S_MsgRecord& r)
{
	return (l.mid < r.mid);
}

bool C_UICore::CompareMsgMidNEQ(const S_MsgRecord& l, const S_MsgRecord& r)
{
	return (l.mid == r.mid);
}

BOOL C_UICore::LoadSession(PS_UISessionInfo psUISInfo, int i32Type)
{
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.LoadSession(psUISInfo, i32Type);
}

DWORD C_UICore::LoadSessions(E_SEFlag eFlag, DWORD dwFlag, CListUI* pList, int i32Limit)
{
	if ( dwFlag & SESSION_FLAG_INITED )
	{	// 会话的初始化已经线程化，这儿使用这个标识来标记已经初始化
		m_bInited = TRUE;
		return 0;
	}

	S_EnumSessionInfo sESI;
	sESI.i32Limit = i32Limit;
	m_pISessionMgr->Enum(eFlag, EnumSessionCb, (void*)&sESI);

	for ( VectSessionInfoIt it = sESI.vectSInfo.begin(); it != sESI.vectSInfo.end(); it++ )
	{
		DWORD dwFlag2 = dwFlag;
		S_UISessionInfo sSInfo;
		CListSessionElementUI* pEl = ::CreateDuiInstance<CListSessionElementUI*>(_T("CListSessionElementUI"));
		CHECK_NULL_RET_(pEl, 0);
		if ( it->eType != eSESSION_TYPE_SINGLE )
			dwFlag2 |= SESSION_FLAG_SUMMARY_NAME;	// 非单聊，需要加上发送者名字

		if ( it->dwTopTime )
			dwFlag2 |= SESSION_FLAG_TOP;

		if ( it->eFlag & eSEFLAG_SHIELD )
			dwFlag2 |= SESSION_FLAG_SHIELD;

		sSInfo.sid			= it->qsid;
		sSInfo.eType		= it->eType;
		sSInfo.dwFlag		= dwFlag2;
		sSInfo.dwTopTime	= it->dwTopTime;
		sSInfo.eidCreater	= it->CreaterId;
		sSInfo.szTitle		= it->szTitle;
		sSInfo.dwLastMsgTime= it->dwUpdateTime;
		sSInfo.dwModifyTime = it->dwSUpdateTime ? it->dwSUpdateTime : it->dwCreateTime;
			
		GetLastMsgSummary(sSInfo.sid, &sSInfo);
		LoadSession(&sSInfo, LOAD_SESSION_MEMBER);

		pList->Add(pEl);
		pEl->SetInfo(sSInfo);
		pEl->SetFlag(dwFlag2);
	}

	m_bInited = TRUE;
	return sESI.vectSInfo.size();
}

typedef struct tagEnumEmpData
{
	DWORD dwFlag;
	I_EIMLogger* pIUILogger;
	VectSessionMember& vectMember;
}S_EnumEmpData, *PS_EnumEmpData;
int __stdcall C_UICore::EnumEmpCbEx(PS_EmpInfo_ psEmpInfo, void* pvUserData, DWORD dwPyOrder, DWORD dwFixOrder)
{
	CHECK_NULL_RET_(psEmpInfo, 0);
	if ( PS_EnumEmpData psMember = (PS_EnumEmpData)pvUserData )
	{
		if ( (psMember->dwFlag & EMP_NO_SESSION) && (psEmpInfo->dwPurview & EMP_NO_SESSION) )
		{
			EIMLOGGER_INFO_(psMember->pIUILogger, _T("eid:%u, purview:0x%08x, ucode:%s no purview access it"), psEmpInfo->Id, psEmpInfo->dwPurview, psEmpInfo->pszCode);
			return 1;
		}

		psMember->vectMember.push_back(psEmpInfo->Id);
		return 1;
	}

	return 0;
}

DWORD C_UICore::GetDeptEmps(QDID did, int i32Level, VectSessionMember& vectSMember, DWORD dwFlag)
{
	if ( PS_DeptInfo_ psDInfo = m_pIContacts->GetDeptItem(did, eGDIM_SELF) )
	{
		S_EnumEmpData sData = {dwFlag, m_pIUILogger, vectSMember};
		m_pIContacts->EnumEmps(psDInfo, i32Level, C_UICore::EnumEmpCbEx, (void*)&sData);	// 枚举成员
	}

	return vectSMember.size();
}

int __stdcall C_UICore::SessionMemberCb(QEID qeid, void* pvUserData)
{
	if ( VectSessionMember* pvectMember = (VectSessionMember*)pvUserData )
	{
		pvectMember->push_back(qeid);
		return 1;
	}

	return 0;
}

DWORD C_UICore::GetSessionMember(QSID sid, VectSessionMember& vectSMember)
{
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.GetSessionMember(sid, vectSMember);
}

int __stdcall C_UICore::EnumMsgCb(QSID qsid, QMID qmid, QEID SenderID, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData)
{
	if ( ListSessionRecord* plistRecord = (ListSessionRecord*)pvUserData )
	{
		S_MsgRecord sMsgRecord;
		CDuiString  szMsg = MSGUI_START(_T(FIELD_MSGRICH));
		szMsg += pszXml;
		szMsg += MSGUI_END(_T(FIELD_MSGRICH));

		sMsgRecord.mid		  = qmid;
		sMsgRecord.dwSendTime = dwTimestamp;
		sMsgRecord.eidSender  = SenderID;
		sMsgRecord.szMsgRich  = szMsg.GetData();

		plistRecord->push_back(sMsgRecord);
		return 1;
	}

	return 0;
}

int __stdcall C_UICore::GetMsgCb(QSID qsid, QMID qmid, QEID SenderID, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData)
{
	if ( PS_MsgRecord psMsg = (PS_MsgRecord)pvUserData)
	{
		psMsg->szMsgRich = MSGUI_START(_T(FIELD_MSGRICH));
		psMsg->szMsgRich += pszXml;
		psMsg->szMsgRich += MSGUI_END(_T(FIELD_MSGRICH));
	}

	return 0;
}

DWORD C_UICore::GetMsg(PS_MsgRecord psMsg)
{
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.GetMsg(psMsg);
}

DWORD C_UICore::GetSessionNewRecordStart(QSID& sid, E_SessionType eType)
{	// 得到第一条件未读/离线消息的时间。
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.GetSessionNewRecordStart(sid, eType);
}

DWORD C_UICore::GetSessionRecord(PS_UISessionInfo psUISInfo)
{
	C_UISessionHelper h(m_pISessionMgr, m_pIMsgMgr);
	return h.GetSessionRecord(psUISInfo);
}

BOOL C_UICore::CreateSessionView(PS_UISessionInfo_ psUISInfo_, BOOL bShow)
{
	CHECK_NULL_RET_(psUISInfo_, FALSE);
	S_UISessionInfo sUISInfo = *psUISInfo_;
	S_SessionInfo sInfo = { 0 };
	
	if ( sUISInfo.sid )
	{	// 是已经存在的会话
		if ( PS_UISessionInfo psFind = m_pwndMain->IsSessionVisible(sUISInfo.sid) )
		{	// 已经加载的会话
			if ( sUISInfo.dwFlag & SESSION_FLAG_SEND_FILE )
				psFind->dwFlag |= SESSION_FLAG_SEND_FILE;

			return m_pwndMain->ShowSessionView(psFind, bShow);
		}
		else
		{	// 没有显示的会话，才需要初始化
			if ( !m_pISessionMgr->Get(sUISInfo.sid, &sInfo) )
			{
				EIMLOGGER_ERROR_(m_pIUILogger, _T("Get session failed，sid:0x%I64x"), sUISInfo.sid);
				return  FALSE;
			}

			if ( sInfo.eType == eSESSION_TYPE_MULTI )
				sUISInfo.dwFlag	|= SESSION_FLAG_SUMMARY_NAME;

			if ( sInfo.eFlag & eSEFLAG_REMOVE )
				m_pISessionMgr->UpdateFlag(sInfo.qsid, eSEFLAG_REMOVE, FALSE);	// 清除移除标识，重新打开会话


			sUISInfo.eType		   = sInfo.eType;
			sUISInfo.sid           = sInfo.qsid;
			sUISInfo.eidCreater	   = sInfo.CreaterId;
			sUISInfo.dwLastMsgTime = sInfo.dwUpdateTime;
			sUISInfo.dwModifyTime  = sInfo.dwSUpdateTime ? sInfo.dwSUpdateTime : sInfo.dwCreateTime;
			sUISInfo.dwNewMsgCount =(sInfo.dwNewMsg + sInfo.dwOfflineMsg);	// 新消息 + 离线消息
			sUISInfo.dwTopTime     = sInfo.dwTopTime;
			sUISInfo.szTitle	   = sInfo.szTitle;
			if ( sInfo.dwTopTime )
				sUISInfo.dwFlag	|= SESSION_FLAG_TOP;

			GetLastMsgSummary(sUISInfo.sid, &sUISInfo);

			if ( sUISInfo.vectMember.empty() )
				LoadSession(&sUISInfo, sUISInfo.dwNewMsgCount ? LOAD_SESSION_ALL : LOAD_SESSION_MEMBER);	// 加载成员

			if ( sInfo.dwCreateTime == 0 && sInfo.eType == eSESSION_TYPE_MULTI)	// RTX导入的群，需要自动创建群【只有讨论组才需要】
				sUISInfo.dwFlag	|= SESSION_FLAG_CREATE_GROUP;					// 最后加这个标识，才会加载成员

			return m_pwndMain->ShowSessionView(&sUISInfo, bShow);
		}
	}
	else if ( sUISInfo.eType == eSESSION_TYPE_SINGLE )
	{
		if ( sUISInfo.sid == 0 )
		{	// 需要初始化单聊会话
			sUISInfo.eidCreater = GetPurview(GET_LOGIN_QEID);
			if ( sUISInfo.eidCreater == sUISInfo.eidReceiver && m_pwndMain)
			{
				m_pwndMain->MessageBox(kNo2SelfPrompt, NULL, MB_OK | MB_ICONWARNING);
				return FALSE;
			}

			if ( sUISInfo.eidReceiver == 0 && sUISInfo.vectMember.size() > 1 )
			{	// 使用成员创建时
				if ( sUISInfo.vectMember[0] != sUISInfo.eidCreater )
					sUISInfo.eidReceiver = sUISInfo.vectMember[0];
				else if ( sUISInfo.vectMember.size() == 2 && sUISInfo.vectMember[1] != sUISInfo.eidCreater )
					sUISInfo.eidReceiver = sUISInfo.vectMember[1];
			}

			// 新创建的单聊会话，检查权限 
			DWORD dwPurview = m_pIContacts->CheckEmpPurview(sUISInfo.eidReceiver);
			if ( dwPurview & EMP_NO_SESSION )
			{	// 没有权限发起会话
				if ( !(dwPurview & EMP_NO_PROMPT) && m_pwndMain)
					m_pwndMain->MessageBox(kNoPurviewPrompt, NULL, MB_OK | MB_ICONINFORMATION);

				return FALSE;
			}

			if ( !m_pISessionMgr->Get(sUISInfo.eidCreater, sUISInfo.eidReceiver, &sInfo) )
			{	// 获取会话信息，单聊永远会成功
				EIMLOGGER_ERROR_(m_pIUILogger, _T("Get single session failed，Creater:%u, Receiver:%u"), sUISInfo.eidCreater, sUISInfo.eidReceiver);
				return  FALSE;
			}

			sUISInfo.sid		   = sInfo.qsid;
			sUISInfo.dwLastMsgTime = sInfo.dwUpdateTime ? sInfo.dwUpdateTime : sInfo.dwCreateTime; // 当没有历史消息时，使用创建时间，才能让会话靠前
			sUISInfo.dwModifyTime  = sInfo.dwSUpdateTime ? sInfo.dwSUpdateTime : sInfo.dwCreateTime;
			sUISInfo.dwNewMsgCount =(sInfo.dwNewMsg + sInfo.dwOfflineMsg);	// 新消息 + 离线消息
			sUISInfo.dwTopTime     = sInfo.dwTopTime;
			sUISInfo.szTitle	   = sInfo.szTitle;

			if ( sUISInfo.eidCreater != sInfo.CreaterId )
				sUISInfo.eidCreater  = sInfo.CreaterId;	// 创建者不是自己
		}

		// 单聊，确保成员依次是:【我，接收者】
		sUISInfo.vectMember.clear();
		sUISInfo.vectMember.push_back(GetPurview(GET_LOGIN_QEID));		// 我
		sUISInfo.vectMember.push_back(sUISInfo.eidReceiver);			// 对方
	}
	else if ( sUISInfo.eType == eSESSION_TYPE_MULTI )
	{
		if ( sUISInfo.sid == 0 )
		{	// 需要初始化群聊会话
			sUISInfo.eidCreater = GetPurview(GET_LOGIN_QEID);
			VectSessionMemberIt itFind = find(sUISInfo.vectMember.begin(), sUISInfo.vectMember.end(), sUISInfo.eidCreater);
			if (itFind == sUISInfo.vectMember.end())
				sUISInfo.vectMember.push_back(sUISInfo.eidCreater);	// 追加自己

			if ( sUISInfo.vectMember.size() <= 1 )
			{
				EIMLOGGER_ERROR_(m_pIUILogger, _T("Only one employee，can not create session，eid:%u, Receiver:%u"), sUISInfo.vectMember[0] );
				return FALSE;
			}
			else if ( sUISInfo.vectMember.size() == 2 )
				 sUISInfo.eType = eSESSION_TYPE_SINGLE;					// 只有两个人，转为单聊
			else 
			{	// 因为 vector 本来就是DWORD的连线内存，直接使用
				if ( m_pISessionMgr->Get(sUISInfo.vectMember.begin()._Ptr, sUISInfo.vectMember.size(), &sInfo, sUISInfo.eType, sUISInfo.sid, 0, sUISInfo.eidCreater) )
				{	// 复用已经存在的群
					sUISInfo.sid           = sInfo.qsid;
					sUISInfo.eidCreater    = sInfo.CreaterId;
					sUISInfo.dwFlag		  |= SESSION_FLAG_SUMMARY_NAME;
					sUISInfo.dwLastMsgTime = sInfo.dwUpdateTime;
					sUISInfo.dwModifyTime  = 0/*sInfo.dwSUpdateTime*/;
					sUISInfo.dwNewMsgCount =(sInfo.dwNewMsg + sInfo.dwOfflineMsg);	// 新消息 + 离线消息
					sUISInfo.dwTopTime     = sInfo.dwTopTime;
					sUISInfo.szTitle	   = sInfo.szTitle;

					GetLastMsgSummary(sUISInfo.sid, &sUISInfo);
				}
				else
				{
					sUISInfo.sid = m_pIEngine->NewID();
					if ( sInfo.dwCreateTime == 0 )
						sInfo.dwCreateTime = GetTimeNow();
					sUISInfo.dwLastMsgTime = sInfo.dwUpdateTime ? sInfo.dwUpdateTime : sInfo.dwCreateTime; // 当没有历史消息时，使用创建时间，才能让会话靠前
					sUISInfo.dwFlag |= SESSION_FLAG_CREATE_GROUP;
					sUISInfo.dwFlag |= SESSION_FLAG_SUMMARY_NAME;
				}
			}
		}
	}
	else if ( sUISInfo.eType == eSEFLAG_RGROUP )
	{

	}
	else if ( sUISInfo.eType == eSEFLAG_VGROUP )
	{
		ASSERT_(FALSE);	// 其它类型，暂未处理...
	}

	return m_pwndMain->ShowSessionView(&sUISInfo, bShow);
}

DWORD C_UICore::TopSession(QSID qsid, E_TopSession eTopOperate)
{
	return m_pISessionMgr->TopSession(qsid, eTopOperate);
}

BOOL C_UICore::GetGroupName(PS_UISessionInfo psUISInfo)
{
	CDuiString szDefaultTitle;
	for ( VectSessionMemberIt it = psUISInfo->vectMember.begin(); it != psUISInfo->vectMember.end(); it++ )
	{	// 组成默认群标题
		if ( const TCHAR* pszName = m_pIContacts->GetEmpName(*it) )
		{
			szDefaultTitle += pszName;
			if(szDefaultTitle.GetLength() > GROUP_NAME_LENGTH)
			{
				szDefaultTitle = szDefaultTitle.Left(GROUP_NAME_LENGTH - 2);
				szDefaultTitle.TrimRight(_T('、'));
				szDefaultTitle += _T("...");
				break;
			}

			szDefaultTitle +=  _T("、");
		}
	}

	szDefaultTitle.TrimRight(_T('、'));
	psUISInfo->szTitle = szDefaultTitle;

	int i32IsShowPrompt = hpi.UIGetAttributeInt(PATH_FUNC, FUNC_ATTR_CREATE_GROUP_PROMPT, eSYSSETTING_CREATE_G);
	if(eSYSSETTING_CREATE_G == i32IsShowPrompt)
	{
		if ( C_GroupNameWnd* pwndGroupName = new C_GroupNameWnd )
		{
			pwndGroupName->DoModalWnd(UIGetMainWnd(), psUISInfo->szTitle, szDefaultTitle);
			return TRUE;		
		}
	}
	
	return TRUE;
}

BOOL C_UICore::CreateGroup(PS_UISessionInfo psUISInfo)
{
	CHECK_NULL_RET_(psUISInfo, FALSE);

	//会话信息
	CDuiString szCreateGroup;

	if ( psUISInfo->szTitle.GetLength() > MAX_UISESSION_TITLE )
		psUISInfo->szTitle.SetAt(MAX_UISESSION_TITLE, _T('\0'));	// 标题最长16，截断

	szCreateGroup.Format(kCmdCreateGroupBegin, psUISInfo->eidCreater, psUISInfo->sid, psUISInfo->szTitle.GetData(), GetTimeNow());

	CDuiString szItem;
	for ( VectSessionMemberIt it = psUISInfo->vectMember.begin(); it != psUISInfo->vectMember.end(); it++ )
	{	//会话参与人
		szItem.Format(kUserId, *it);
		szCreateGroup += szItem;
	}

	szCreateGroup += kCmdEnd;
	int i32Ret = Command((TCHAR*)szCreateGroup.GetData());
	if (FAILED(i32Ret))
	{
		EIMLOGGER_ERROR_(m_pIUILogger, _T("Create group failed, error: 0x%08X, desc: %s"), i32Ret, GetErrorLangInfo(i32Ret));
		return AddError(psUISInfo, EIMERR_CREATE_GROUPFAILED);
	}

	return AddError(psUISInfo, EIMERR_CREATIG_GROUP);
}

BOOL C_UICore::AddInfo(PS_UISessionInfo psUISInfo, PS_MsgRecord psMsgInfo)
{
	CHECK_NULL_RET_(psUISInfo, FALSE);
	if ( !m_pwndMain->ShowInfo(psUISInfo->sid, psMsgInfo) )
		psMsgInfo->dwFlag |= MSG_FLAG_NEWMSG;

	psUISInfo->listRecord.push_back(*psMsgInfo);
	return TRUE;
}

BOOL C_UICore::AddError(PS_UISessionInfo psUISInfo, DWORD dwErrId)
{
	CHECK_NULL_RET_(psUISInfo, FALSE);
	S_MsgRecord sMsgRecord;

	sMsgRecord.dwFlag	  = MSG_FLAG_ERROR;
	sMsgRecord.dwSendTime = GetTimeNow();
	sMsgRecord.eidSender  = psUISInfo->eidCreater;
	sMsgRecord.szMsgRich  = GetErrorLangInfo(dwErrId);

	return AddInfo(psUISInfo, &sMsgRecord);
}

int C_UICore::SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag)
{
	return SendMsg((PS_UISessionInfo)psUISInfo, pszXml, dwFlag);
}

int C_UICore::GetSendMsg(PS_UISessionInfo psUISInfo, LPCTSTR pszXml, DWORD dwFlag, CDuiString& szSendMsg)
{
	CHECK_NULL_RET_(psUISInfo, EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(pszXml, EIMERR_INVALID_PARAM);

	BOOL bReceipt = (dwFlag & SEND_MSG_FLAG_RECEIPT);
	QSID qsidVirGroupId = 0;

	switch ( psUISInfo->eType )
	{
	case eSESSION_TYPE_SINGLE:	// 单聊
		szSendMsg.Format(kCmdSendSingleMsgBegin, 1, psUISInfo->eidReceiver, psUISInfo->sid, 0, bReceipt, 0, 0, 0);
		break;
//	case eSESSION_TYPE_1TO_HUNDRED:
//		szSendMsg.Format(kCmdSendSingleMsgBeginType, 1, psUISInfo->eidReceiver, psUISInfo->sid, 0, bReceipt, 1, 0, 0, eSESSION_TYPE_SINGLE );
//		break;
//	case eSESSION_TYPE_1TO_TEN_THOUSAND:
//		szSendMsg.Format(kCmdSendSingleMsgBeginType, 1, psUISInfo->eidReceiver, psUISInfo->sid, 0, bReceipt, 0, 1, 0, eSESSION_TYPE_SINGLE );
//		break;
	case eSESSION_TYPE_MULTI:	// 群聊
		if ( psUISInfo->dwFlag & SESSION_FLAG_CREATE_GROUP )
		{	// 群没有创建，不能发送消息
			AddError(psUISInfo, EIMERR_GROUPFAILED_MSGERROR);
			return CreateGroup(psUISInfo);
		}

		szSendMsg.Format(kCmdSendMsgBegin, 1, psUISInfo->eidReceiver, psUISInfo->sid, 1, bReceipt, 0, eSESSION_TYPE_MULTI);
		break;
	case eSESSION_TYPE_RGROUP:  // 固定组
		szSendMsg.Format(kCmdSendMsgBegin, 1, psUISInfo->eidReceiver, psUISInfo->sid, 2, bReceipt, 0, eSESSION_TYPE_RGROUP);	
		break;
	case eSESSION_TYPE_VIRGROUP:
		m_pISessionMgr->Get(psUISInfo->sid, qsidVirGroupId);
		if (qsidVirGroupId > 0)
			szSendMsg.Format(kCmdSendSingleMsgBeginVir, 1, psUISInfo->eidReceiver, psUISInfo->sid, 5, bReceipt, 0, 0, eSESSION_TYPE_SINGLE );
		else
			szSendMsg.Format(kCmdSendSingleMsgBegin, 1, psUISInfo->eidReceiver, psUISInfo->sid, 5, bReceipt, 0, 0, eSESSION_TYPE_SINGLE );
		break;
	default:
		ASSERT_(FALSE);
		break;
	}

	szSendMsg += pszXml;
	szSendMsg += kCmdSendMsgUIEnd;
      
	if( bReceipt )
	{	// 发送回执参与人列表
		CDuiString szEmp;
		CDuiString szEmps;
		QEID qeid = GetPurview(GET_LOGIN_QEID);

		for ( VectSessionMemberIt it = psUISInfo->vectMember.begin(); it != psUISInfo->vectMember.end(); it++ )
		{
			if ( qeid == *it )
				continue;

			szEmp.Format(kCmdItemEmpID, *it);
			szEmps += szEmp;
		}

		szSendMsg += szEmps;
	}

	szSendMsg += kCmdSendMsgCmdEnd;	//加上Cmd end
	return EIMERR_NO_ERROR;
}

int C_UICore::SendMsg(PS_UISessionInfo psUISInfo, LPCTSTR pszXml, DWORD dwFlag)
{
	CDuiString szSendMsg;
	int i32Ret = GetSendMsg(psUISInfo, pszXml, dwFlag, szSendMsg);
	if ( FAILED(i32Ret) )
		return i32Ret;

	return m_pIEngine->Command((TCHAR*)szSendMsg.GetData());
}

BOOL C_UICore::ShowVirWindow(int i32Index, DWORD dwFlag)
{
	CHECK_NULL_RET_(m_pwndMain, FALSE);
	return m_pwndMain->ShowVirWindow(i32Index, dwFlag);
}

BOOL C_UICore::Jump2Dept(DWORD dwId, BOOL bEmp)
{
	return m_pwndMain->Jump2Dept(dwId, bEmp);
}

void C_UICore::ModifyGroupMember( PS_ModifyGroupMember psMGM)
{
	CHECK_NULL_RETV_( psMGM );
	if ( psMGM->pvectAdd == NULL && psMGM->pvectDel == NULL)
		return ;

	int		  i32Count = 0;
	eIMString szCmd;
	TCHAR szBuf[1024] = { 0 };
	
	ASSERT_(psMGM->qsid);
	_sntprintf(szBuf, COUNT_OF_ARRAY_(szBuf), kCmdModifyGroupMemberBegin, 
		psMGM->qeid ? psMGM->qeid : m_pIEngine->GetPurview(GET_LOGIN_QEID),
		psMGM->qsid,
		psMGM->dwTimestamp ? psMGM->dwTimestamp : m_pIEngine->GetTimeNow());
	szCmd = szBuf;

	if ( psMGM->pvectAdd)
	{
		for (VectSessionMemberIt it = psMGM->pvectAdd->begin(); it != psMGM->pvectAdd->end(); it++)
		{
			i32Count++;
			_sntprintf(szBuf, COUNT_OF_ARRAY_(szBuf), kCmdModifyGroupMemberItem, MODIFY_GROUP_MEMBER_TYPE_ADD, *it);
			szCmd += szBuf;
		}
	}

	if ( psMGM->pvectDel)
	{
		for (VectSessionMemberIt it = psMGM->pvectDel->begin(); it != psMGM->pvectDel->end(); it++)
		{
			i32Count++;
			_sntprintf(szBuf, COUNT_OF_ARRAY_(szBuf), kCmdModifyGroupMemberItem, MODIFY_GROUP_MEMBER_TYPE_DEL, *it);
			szCmd += szBuf;
		}
	}

	if ( i32Count == 0 )
	{
		TRACE_(_T("Empty members to be modify, ignore it"));
		return;
	}

	szCmd += kCmdEnd;
	m_pIEngine->Command(szCmd.c_str());
}

BOOL C_UICore::HideWindow(POINT* pptCursor, DWORD dwFlag)
{
	if ( dwFlag & HIDE_SESSION_MEMBER_WND)
	{	// 会话成员窗口
		m_pwndMain->HideWindow(pptCursor);
	}

	if ( dwFlag & HIDE_EMPINFO_WND)
	{	// 员工信息窗口
		if ( C_MainEmpInfoWnd* pwndEmpInfo = C_MainEmpInfoWnd::GetObject(FALSE, NULL) )
			pwndEmpInfo->ShowWindow(false);
		//	::SendMessage(pwndEmpInfo->GetHWND(), WM_CLOSE, (WPARAM)IDCLOSE, 0L);	// POST会导致连续显示时，第二次无法显示
	}

	return TRUE;
}

int C_UICore::GetStatus(E_SubscribeOper eOp, UINT64 u64Id, int i32Type)
{
	CDuiString szSubscribeCmd;

	szSubscribeCmd.Format(kCmdSubscribeActionEx, (DWORD)eStatusGet, eOp, u64Id, i32Type, 0);
	szSubscribeCmd+= kAllCmdEnd;
	return m_pIEngine->Command(szSubscribeCmd.GetData());
}

int C_UICore::GetAllStatus()
{				
	CDuiString szSubscribeCmd;

	szSubscribeCmd.Format(kCmdGetStatesAction, GETSTATE_ALL);
	szSubscribeCmd += kAllCmdEnd;
	return m_pIEngine->Command(szSubscribeCmd.GetData());
}

int C_UICore::SubscribeStatus(E_SubscribeOper eOp, const VectSessionMember& vectSMember)
{
	CDuiString szSubscribeCmd;
	CDuiString szEmp;
	QEID eidLogin = GetPurview(GET_LOGIN_QEID);
	BOOL bFindMe  =(find(vectSMember.begin(), vectSMember.end(), eidLogin) != vectSMember.cend());

	szSubscribeCmd.Format(kCmdSubscribeActionEx, eStatusSubscribe, eOp, (UINT64)0, 0, bFindMe ? vectSMember.size() - 1 : vectSMember.size());
	for ( VectSessionMemberIt_ it = vectSMember.begin(); it != vectSMember.end(); it++ )
	{
		 if ( eidLogin == *it )
			 continue;

		szEmp.Format(kSubscribeEmpId, *it);
		szSubscribeCmd += szEmp;
	}

	szSubscribeCmd += kAllCmdEnd;
	return m_pIEngine->Command(szSubscribeCmd.GetData());
}


BOOL C_UICore::GetFacePath(QEID eid, eIMString& szFacePath, BOOL bCheckExist, BOOL bSmall)
{
	CDuiString szPath;

	if ( m_bDisableSmallFace )
		bSmall = FALSE;
	
	szPath.Format(bSmall ? _T("\\small%u.jpg") : _T("\\%u.jpg"), eid);
	szFacePath = PATH_TOKEN_LOGINID_DOC_FILE_(PATH_TYPE_LOGINID_DOC_USERFACE);
	szFacePath+= szPath;
	eIMReplaceToken(szFacePath);
	eIMMakeDir(szFacePath.c_str());

	if ( !bCheckExist )
		return TRUE;

	return CRenderEngine::IsValidImage(szFacePath.c_str());
}

BOOL C_UICore::DownloadFace(QEID eid, BOOL bCheck, BOOL bSmall)
{
	eIMString szFacePath;
	S_FileTransInfo sInfo;
	ZERO_STRUCT_(sInfo);

	if ( m_bDisableSmallFace )
		bSmall = FALSE;
 
	if (bCheck)
	{
		if ( GetFacePath(eid, szFacePath, TRUE, bSmall) )
			return TRUE;	// 已经存在，不用下载

		PS_EmpInfo_ psEmpInfo = m_pIContacts->GetEmp(eid);
		if ( psEmpInfo && psEmpInfo->dwLogoTime != IME_EMP_LOGO_DOWNLOAD )
			return TRUE;	// 不需要下载，直接返回
	}
	else
	{
		GetFacePath(eid, szFacePath, FALSE, bSmall);
	}

	_tcsncpy(sInfo.szFile, szFacePath.c_str(), MAX_PATH);

	sInfo.u64Fid    = GetQfid(NULL);
	sInfo.bitAction = FILE_ACTION_RECV;
	sInfo.bitDirect = FILE_DIR_DOWNLOAD;
	sInfo.bitIsImage= bSmall ? FILE_SERVER_TYPE_HEADS : FILE_SERVER_TYPE_HEAD;
	sInfo.bitType	= FILE_TRANS_TYPE_OFFLINE;
	sInfo.u32Eid	= eid;
	sInfo.u32LoginEmpId = GetPurview(GET_LOGIN_QEID);

	EIMLOGGER_DEBUG_(m_pIUILogger, _T("Download face:[%u]!"), eid);
	SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_FILE_TRANSFER_ACTION, &sInfo);
	return TRUE;
}

BOOL C_UICore::UpdateLogo(QEID eid, LPCTSTR pszLogoTpl, LPCTSTR pszLogo)
{	// 使用GDI+ 直接生成需要的大小（40*40），来处理会话单聊头像模糊问题
	eIMString szFace;
	eIMString szLogoSmall;

	if ( pszLogo == NULL && _tcsicmp(pszLogoTpl, kLogoTplSmall) == 0 )
	{
		if ( GetFacePath(eid, szLogoSmall, FALSE, TRUE) )
			pszLogo = szLogoSmall.c_str();
	}

	if ( m_pimgLogoSmallTpl == NULL )
	{
		eIMString szLogoTpl;
		if ( GetSkinLogoTplPath(szLogoTpl, kLogoTplSmall) )
			m_pimgLogoSmallTpl = Image::FromFile(szLogoTpl.c_str());
	}

	if ( pszLogo == NULL || !core.GetFacePath(eid, szFace, TRUE, FALSE) || m_pimgLogoSmallTpl == NULL )
		return FALSE;

	Image	 imgFace(szFace.c_str());
	Graphics g(m_pimgLogoSmallTpl);
	SolidBrush brushWhite(Color(255, 255, 255, 255));

	if ( imgFace.GetLastStatus() == Ok )
	{
		RECT   rtPos = { 0 };
		double dRatioX = (double)m_pimgLogoSmallTpl->GetWidth() / imgFace.GetWidth();
		double dRatioY = (double)m_pimgLogoSmallTpl->GetHeight() / imgFace.GetHeight();

		if ( dRatioX < dRatioY )
		{	// 使用 dRatioX
			rtPos.left  = 0;
			rtPos.right = m_pimgLogoSmallTpl->GetWidth();
			rtPos.bottom= (LONG)(imgFace.GetHeight() * dRatioX);
			rtPos.top   = (m_pimgLogoSmallTpl->GetHeight() - rtPos.bottom) / 2;
		}
		else
		{	// 使用 dRatioY 
			rtPos.top   = 0;
			rtPos.bottom= m_pimgLogoSmallTpl->GetHeight();
			rtPos.right = (LONG)(imgFace.GetWidth() * dRatioY);
			rtPos.left  = (m_pimgLogoSmallTpl->GetWidth() - rtPos.right) / 2;
		}

		g.SetInterpolationMode(InterpolationModeNearestNeighbor); // 使用 InterpolationModeNearestNeighbor 才不会导致最边上（左、上）的第一个像素，显示差异过大。
		g.FillRectangle(&brushWhite, 0, 0, m_pimgLogoSmallTpl->GetWidth(), m_pimgLogoSmallTpl->GetHeight());	// 刷背景色
		g.DrawImage(&imgFace, rtPos.left, rtPos.top, rtPos.right, rtPos.bottom); 

		static CLSID jpegClsid = GUID_NULL;
		if ( IsEqualGUID(jpegClsid, GUID_NULL) )
			GetEncoderClsid(L"image/jpeg", &jpegClsid);

		return (Ok == m_pimgLogoSmallTpl->Save(pszLogo, &jpegClsid));
	}

	return FALSE;
}

BOOL C_UICore::InitEngine()
{
	InitEngineFont(PATH_FONT_ITEM_(FONT_RECV_EL), MSGUI_FONT_RECV);
	InitEngineFont(PATH_FONT_ITEM_(FONT_SEND_EL), MSGUI_FONT_SEND);
	InitEngineFont(PATH_FONT_ITEM_(FONT_ERROR), MSGUI_FONT_ERROR);
	InitEngineFont(PATH_FONT_ITEM_(FONT_MSGAUTO), MSGUI_FONT_MSGAUTO);
	InitEngineFont(PATH_FONT_ITEM_(FONT_RECEIPT_PIN_RCV), MSGUI_FONT_RECEIPT_PIN_RECV);

	m_pIEngine->SetAttributeInt(IME_DISABLE_DEPTUSER_VISABLE, UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DISABLE_DEPTUSER_SHOW));
	m_pIEngine->SetAttributeStr(IME_ATTRIB_LOG_FILE,  UIGetAttributeStrEx(PATH_ENGINE, ENGINE_ATTR_LOGFILE));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_LOG_LEVEL, UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_LOGLEVEL, EIMLOG_LEVEL_DEFAULT));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_LOG_FLAG,  UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_LOGFLAG, EIMLOG_FLAG_DEFAULT_FULL));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_PINYIN,	  UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_PINYIN, IME_ATTRIB_PINYIN_DEFAULT));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_DOWNLOAD_EMPS_DELAY, UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_EMPS_DELAY, 5));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_FILE_TRANSFER_THREAD_NUM, UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_FILE_TRANSFER_THREAD_NUM, 2));
//	m_pIEngine->SetAttributeInt(IME_ATTRIB_DOWNLOAD_CONTACTDB,   UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_CONTACTDB, 1));
//	m_pIEngine->SetAttributeInt(IME_ATTRIB_DOWNLOAD_CONTACTFILE, UIGetAttributeInt( PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_CONTACT_FILE, 0xFFFFFFFF ));
//	m_pIEngine->SetAttributeInt(IME_ATTRIB_DOWNLOAD_CONTACTDB_DISABLE_PROXY, UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_CONTACTDB_DISABLE_PROXY, 1));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_SWITCH_FILE_SERVER, UIGetAttributeInt( PATH_ENGINE, SETTING_ATTR_SWITCH_FILE_SERVER));
	m_pIEngine->SetAttributeInt(IME_ATTRIB_FORCE_RELOAD2EMPS, UIGetAttributeInt( PATH_ENGINE, SETTING_ATTRIB_FORCE_RELOAD2EMPS));
	m_pIEngine->SetAttributeInt(IME_FAST_LOAD_CONTACTS, UIGetAttributeInt(PATH_FUNC, FUNC_ATTR_FAST_LOAD_CONTACTS));

	return TRUE;
}

BOOL C_UICore::InitEngineFont(TCHAR* pszFontEl, TCHAR* pszEngineFont )
{
	CHECK_NULL_RET_(pszFontEl, FALSE);
	CHECK_NULL_RET_(pszEngineFont, FALSE);

	int		i32FontSize	= UIGetAttributeInt(pszFontEl, FONT_ATTR_SIZE);
	DWORD	dwFontColor	= UIGetAttributeInt(pszFontEl, FONT_ATTR_COLOR);
	int		i32FontId	= UIGetAttributeInt(pszFontEl, FONT_ATTR_ID);
	int	    i32FontMode	= UIGetAttributeInt(pszFontEl, FONT_ATTR_MODE);
	
	CDuiString strRecvFont;
	strRecvFont.Format(FONT_ATTR_VALUE_FMT,i32FontSize,dwFontColor,i32FontId,i32FontMode);
	m_pIEngine->SetAttributeStr( pszEngineFont, strRecvFont );

	return TRUE;
}

CDuiString C_UICore::GetDeptPath(QEID id, BOOL bDept, int i32Level)
{
	return m_pwndMain->GetDeptPath(id, bDept, i32Level);
}

LPCTSTR C_UICore::GetEmpInfo(QEID eid, DWORD dwType)
{
	PS_EmpInfo_ psEmpInfo = m_pIContacts->GetEmp(eid);
	CHECK_NULL_RET_(psEmpInfo, _T(""));

	switch ( dwType )
	{
	case GET_EMP_CODE:
		return psEmpInfo->pszCode;
	case GET_EMP_NAME:
		return m_pIContacts->GetEmpName(psEmpInfo);
	case GET_EMP_EMAIL:
		return psEmpInfo->pszEmail;
	case GET_EMP_DUTY:
		return psEmpInfo->pszDuty;
	default:
		ASSERT_(FALSE);
		break;
	}

	return _T("");
}

DWORD C_UICore::IsMainWindowStaus(DWORD dwFlag)
{
	return m_pwndMain->IsMainWindowStaus(dwFlag);
}

DWORD C_UICore::GetPySortKey(const char* lpPySort)
{
	CHECK_NULL_RET_(lpPySort,0);
	CHECK_NULL_RET_(lpPySort[0] != NULL,0);

	DWORD dwKey = 0;
	string str(lpPySort);
	int i32Index = 0;
	while( i32Index < (int)str.length() && str[i32Index] != ';' && i32Index < 4)
	{
		DWORD dwTemp = 0;
		if(str[i32Index] <= 'z' && str[i32Index] >= 'a')
		{
			dwTemp = str[i32Index] - 'a' + 10;//小写字母10~35的位置
		}
		else if(str[i32Index] <= '9' && str[i32Index] >= '0')
		{
			dwTemp = str[i32Index] - '0';	//数字从0~9开始
		}
		else
		{
			dwTemp = 'z' - 'a' + 10 + 2 ; //其他，37
		}

		dwKey |= (dwTemp << (8 * (3-i32Index)));
		i32Index++;
	}

	return dwKey;
}

LPCTSTR C_UICore::UIGetSkinPath(int* pi32DpiRatio)
{
	if ( m_szSkinFile.empty() )
		m_szSkinFile = eIMGetSkinPath(UIGetAttributeInt(PATH_FUNC, FUNC_DPIAWARE, DPIAWARE_DEFAULT), &m_i32DpiRatio);

	if ( pi32DpiRatio )
		*pi32DpiRatio = m_i32DpiRatio;

	return m_szSkinFile.c_str();
}

BOOL C_UICore::AddCommon(PS_UISessionInfo psSInfo)
{
	return m_pwndMain->AddCommon(psSInfo);
}

BOOL C_UICore::RemoveCommon(PS_UISessionInfo psInfo)
{
	return m_pwndMain->RemoveCommon(psInfo);
}

int C_UICore::UIMessageBox(HWND hwndParent, LPCTSTR pszText, LPCTSTR pszCaption, UINT uType, LPCTSTR pszSkinFile)
{
	return C_MessageBoxWnd::MessageBox(hwndParent, pszText, pszCaption, uType, pszSkinFile);
}

//获取shortkey，算法来到服务器源代码：ShortUrlGenerator.java
char* C_UICore::GetShortKey(char* pszMD5, char szResult[SHORT_KEY_LEN])
{
	static char kChars[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char szSub[9] = {pszMD5[0], pszMD5[1], pszMD5[2], pszMD5[3], pszMD5[4], pszMD5[5], pszMD5[6], pszMD5[7], 0};
	DWORD u32Sub = strtoul(szSub, NULL, 16);	// 因为原算法只处理了第一组8个字符，
	u32Sub = 0x3FFFFFFF & u32Sub;
	for (int j = 0; j < 6; j++) 
	{
		szResult[j] = kChars[0x3D & u32Sub];
		u32Sub >>= 5;
	}

	szResult[6] = _T('\0');
	return szResult;
}

BOOL C_UICore::VerifyFile(LPCTSTR pszFile, LPCTSTR pszFileKey)
{
	CHECK_NULL_RET_(pszFile, FALSE);
	CHECK_NULL_RET_(pszFileKey, FALSE);
	if ( pszFileKey[0] == _T('\0') )
		return true;	// 没有FileKey的，不检查，返回恒真

	char  szFileMD5[33] = {0};
	char  szMD5Key[33] = {0};
	char  szShortKey[SHORT_KEY_LEN] = { 0 };

	// Step1 获取文件MD5
	GetMD5_32((TCHAR*)pszFile, szFileMD5);							

	// Step2 将文件MD5再转MD5
	Create32Md5((unsigned char*)szFileMD5, strlen(szFileMD5), (unsigned char*)szMD5Key);	

	// Step3 得到ShortKey
	GetShortKey(szMD5Key, szShortKey);

	// Step4 检查前6个是否一致
	bool bResult = (
		szShortKey[0] == pszFileKey[0] &&
		szShortKey[1] == pszFileKey[1] &&
		szShortKey[2] == pszFileKey[2] &&
		szShortKey[3] == pszFileKey[3] &&
		szShortKey[4] == pszFileKey[4] &&
		szShortKey[5] == pszFileKey[5] );

	if ( bResult == false )
	{	// 验证失败
		EIMLOGGER_ERROR_(m_pIUILogger, _T("Verify file [%s] failed, FileKey:%s, FileMD5:%S, MD5Key:%S, ShortKey:%S"), pszFile, pszFileKey, szFileMD5, szMD5Key, szShortKey);
	}

	return bResult;
}

int __cdecl C_UICore::CompareEmpItem(void* pvData, const void* pItem1, const void* pItem2)
{	// 重要说明：只能在这个比较函数中，修改临时状态，因为状态还要显示
	CHECK_NULL_RET_(pItem1, 0);
	CHECK_NULL_RET_(pItem2, 0);
	static int s_i32Flag= core.UIGetAttributeInt(PATH_FUNC, FUNC_ATTR_CONTACT_SORT_FLAG);
	U_SortOrder uOrder1 = pvData ? (*(CListContactElementUI**)pItem1)->GetInfo()->uSortOrder : ((PS_ContactItemInfo)pItem1)->uSortOrder;
	U_SortOrder uOrder2 = pvData ? (*(CListContactElementUI**)pItem2)->GetInfo()->uSortOrder : ((PS_ContactItemInfo)pItem2)->uSortOrder;

	if ( s_i32Flag & CONTACT_FLAG_SORT_XSTATUS )
	{	// 排序不使用状态
		if ( uOrder1.bitState != CONTACT_STATE_DEPT )
			uOrder1.bitState = CONTACT_STATE_OFFLINE;

		if ( uOrder2.bitState != CONTACT_STATE_DEPT )
			uOrder2.bitState = CONTACT_STATE_OFFLINE;
	}
	else if ( s_i32Flag & CONTACT_FLAG_SORT_XMOBILE )
	{	// 不区分移动端，就把比较状态更新为PC
		if ( uOrder1.bitState == CONTACT_STATE_MOBILE_ONLINE )
			uOrder1.bitState = CONTACT_STATE_PC_ONLINE;
		else if ( uOrder1.bitState == CONTACT_STATE_MOBILE_LEAVE )
			uOrder1.bitState = CONTACT_STATE_PC_LEAVE;

		if ( uOrder2.bitState == CONTACT_STATE_MOBILE_ONLINE )
			uOrder2.bitState = CONTACT_STATE_PC_ONLINE;
		else if ( uOrder2.bitState == CONTACT_STATE_MOBILE_LEAVE )
			uOrder2.bitState = CONTACT_STATE_PC_LEAVE;
	}

	if (uOrder1.u64Order > uOrder2.u64Order)
		return 1;
	else if (uOrder1.u64Order < uOrder2.u64Order)
		return -1;
	else
		return 0;
}

BOOL C_UICore::ShowViewer(LPCTSTR pszImage, QFID fid)
{
	static PROCESS_INFORMATION pi = { 0 }; 
	STARTUPINFO si;
	DWORD	dwExitCode = 0;
	eIMString szViewer = PATH_TOKEN_EXE_FILE_(_T("ImageViewer.exe"));
	CDuiString szCmdLine;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	eIMReplaceToken(szViewer);

	if ( !PathFileExists(szViewer.c_str()) )
		return FALSE;

	if ( pi.hProcess && GetExitCodeProcess(pi.hProcess, &dwExitCode) && dwExitCode == STILL_ACTIVE)	// 关闭之前的进程
		TerminateProcess(pi.hProcess, 1);

	SAFE_CLOSE_HANDLE_(pi.hProcess);
	if ( fid )
	{	// 有FID，就使用数据库模式(-db)
		eIMString szDbFile = PATH_TOKEN_LOGINID_FILE_(_T("msg.db"));
	
		eIMReplaceToken(szDbFile);
		szCmdLine.Format(_T("-taskbar=0 -fid=%llu -db=\"%s\""), fid, szDbFile.c_str());
	}
	else
	{	// 没有FID， 只显示单个文件（表情，文件类型）
		CHECK_NULL_RET_(pszImage, FALSE);
		szCmdLine.Format(_T("-taskbar=0 -images=\"%s\""), pszImage);
	}

	if (!CreateProcess((TCHAR*)szViewer.c_str(), (TCHAR*)szCmdLine.GetData(), NULL,NULL,FALSE,0,NULL,NULL, &si, &pi))
		return FALSE;

	CloseHandle(pi.hThread);
	return TRUE;
}
