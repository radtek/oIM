#include "StdAfx.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <Dbghelp.h>
#include <sys/stat.h>
#include "C_UICore.h"
#include "UICore\AppConfig.h"
#include <resprovider-zip\zipresprovider-param.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Dbghelp.lib")

//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 0
// #define RES_TYPE 1  //从PE资源中加载UI资源
	
//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP


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
	, m_pdlgMain(NULL)
{
	// 配置文件，必须最先加载
	SStringT szCfgFile = GetPath(PATH_TOKEN_APP_FILE_(APP_CONFIG_FILE));
	SStringT szRCfgFile = GetPath(PATH_TOKEN_APPDATA_FILE_(APP_CONFIG_FILE));

	m_AppCfg.Open(szCfgFile, szRCfgFile);
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));
}

C_UICore::~C_UICore(void)
{
	OleUninitialize();
	SAFE_CLOSE_HANDLE_(m_hCompanyMutex);

}

int C_UICore::Run(DWORD dwFlag)
{
	//if (!(RUN_FLAG_DISABLE_LOGIN & dwFlag))
	//{	// 没有禁用自动显示登录窗口时
	//	if ( !ShowLoginWnd(dwFlag) )
	//		return FALSE;
	//}

    int nRet = 0;
    SComMgr *pComMgr = new SComMgr;

    //将程序的运行路径修改到项目所在目录所在的目录
	SStringT szAppPath = GetPath(PATH_TOKEN_APP);
    SetCurrentDirectory(szAppPath);
    {
        BOOL bLoaded=FALSE;
        CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<IRenderFactory> pRenderFactory;
        bLoaded = pComMgr->CreateRender_Skia((IObjRef**)&pRenderFactory);
        SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"),_T("imgdecoder"));

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		SApplication *theApp = new SApplication(pRenderFactory, m_hInstance);
        {
            CAutoRefPtr<IResProvider> sysResProvider;
            CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
            sysResProvider->Init((WPARAM)m_hInstance, 0);
            theApp->LoadSystemNamedResource(sysResProvider);
        }

        CAutoRefPtr<IResProvider>   pResProvider;
		int nType = UIGetAttributeInt(PATH_SKIN, SKIN_ATTR_TYPE, SKIN_TYPE_ZIP);
		SStringT szSkin = GetPath(UIGetAttributeStr(PATH_SKIN, SKIN_ATTR_PATH));
		if ( nType == SKIN_TYPE_FILE )
		{
			CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
			if (!pResProvider->Init((LPARAM)(LPCTSTR)szSkin, 0))
			{
				SASSERT_FMT(0, _T("Initial file resource failed!"));
				return 1;
			}
		}
		else if ( nType == SKIN_TYPE_RES )
		{
			CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
			if (!pResProvider->Init((WPARAM)m_hInstance, 0))
			{
				SASSERT_FMT(0, _T("Initial from resource failed!"));
				return 1;
			}
		} 
		else if ( nType == SKIN_TYPE_ZIP )
		{	// ZIP 压缩包，【不能】有 uires 这级父目录，也就是进入到 uires 中，选中全部文件再zip
			bLoaded=pComMgr->CreateResProvider_7ZIP((IObjRef**)&pResProvider);
			SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T(SCreateInstance7Zip));

			ZIPRES_PARAM param;
			param.ZipFile(pRenderFactory, szSkin, "123456");
			if (!pResProvider->Init((WPARAM)&param,0))
			{
				SASSERT_FMT(0, _T("Initial zip resource failed!"));
				return 1;
			}
		}

		theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
        theApp->AddResProvider(pResProvider);
        //加载LUA脚本模块。
        CAutoRefPtr<IScriptModule> pScriptLua;
        bLoaded=pComMgr->CreateScrpit_Lua((IObjRef**)&pScriptLua);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("script_lua"));

        //加载多语言翻译模块。
        CAutoRefPtr<ITranslatorMgr> trans;
        bLoaded=pComMgr->CreateTranslator((IObjRef**)&trans);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("translator"));
        if(trans)
        {//加载语言翻译包
            theApp->SetTranslator(trans);
			SStringT szLang = UIGetAttributeStr(PATH_APP, APP_ATTR_LANGUAGE, _T("en"));
			szLang = _T("lang_") + szLang;	// To: lang_en, lang_cn
            pugi::xml_document xmlLang;
            if(theApp->LoadXmlDocment(xmlLang, szLang,_T("translator")))
            {
                CAutoRefPtr<ITranslator> langCN;
                trans->CreateTranslator(&langCN);
                langCN->Load(&xmlLang.child(L"language"),1);//1=LD_XML
                trans->InstallTranslator(langCN);
            }
        }
        
        { // BLOCK: Run application
            CMainDlg dlgMain;
            dlgMain.Create(GetActiveWindow());
            dlgMain.SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow(dlgMain.m_hWnd);
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet = theApp->Run(dlgMain.m_hWnd);
        }

        delete theApp;
    }
    
    delete pComMgr;  
    return nRet;
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
	if ( !m_FontCfg.IsOpen() )
	{
		SStringT szCfgFile = GetPath(PATH_TOKEN_APP_FILE_(FONT_CONFIG_FILE));
		m_FontCfg.Open(szCfgFile);
	}

	SStringT szPathEl;

	szPathEl.Format(PATH_FONT_ITEM_FMT, i32Index);
	if ( LPCTSTR pszValue = m_FontCfg.GetAttributeStr(szPathEl, FONT_ITEM_NAME_ATTR, NULL, FALSE) )
		return pszValue;

 	if (i32Index < 0 )
		return FONT_DEFAULT_FAMILY;

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
C_XmlConfig& C_UICore::GetErrorLang(LPCTSTR pszLangId, BOOL bRelease)   // ONLY used by App load error information when init and switch language
{
    static bool bLoaded = false;
    static C_XmlConfig obj;

	if ( bLoaded && bRelease)
	{	
		obj.Close();
		bLoaded = false;
	}
  
    if ( pszLangId && bLoaded == false )
    {   // Format as: _T("$(Exe)\\Languages\\Errors.%s.config")
		SStringT szLangFile;
		
		szLangFile.Format(_T("%sLanguages\\Errors.%s.config"), PATH_TOKEN_APP, pszLangId ? pszLangId : _T("EN"));
		if ( SUCCEEDED(obj.Open(GetPath(szLangFile))) )
         {
            bLoaded = true;
            return obj;
         }
    }
     
	return obj;
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
    C_XmlConfig& xmlLang = GetErrorLang(NULL);
    
//	if ( pszRet = xmlLang.GetAttributeStr(dwErrId)))
//       return pszRet;
       
	if ( lpszDefault )
		return lpszDefault;

	if ( FAILED(dwErrId))
	{
		STRACE(_T("Unknown error, err:0x%08x"), dwErrId);
		return _T("Unknown error");
	}

	return _T("");
}

int  C_UICore::SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag)
{
	return 0;
}

inline BOOL C_UICore::_GetPath(LPCTSTR pszToken, LPTSTR pszPath, DWORD dwSize)
{
	CHECK_NULL_RET_(pszToken, FALSE);
	CHECK_NULL_RET_(pszPath, FALSE);
	
	DWORD dwRet = 0;
	if ( _tcsicmp(pszToken, PATH_TOKEN_APP) == 0 )
	{	// 获取主程序所在的位置
		dwRet = ::GetModuleFileName(NULL, pszPath, dwSize );
		if ( dwRet < dwSize )
		{
			TCHAR* pszFind = _tcsrchr(pszPath, _T('\\'));
			if ( pszFind == NULL )
				pszFind = _tcsrchr(pszPath, _T('/'));

			if ( pszFind )
				*pszFind = _T('\0');
		}
	}
	else if ( _tcsicmp(pszToken, PATH_TOKEN_APPDATA) == 0 )
	{
		dwRet = ::GetEnvironmentVariable(PATH_TOKEN_APPDATA, pszPath, dwSize);	// 如果有重定向
		if( dwRet > 0 && dwRet < dwSize ||
			::SHGetSpecialFolderPath(NULL, pszPath, CSIDL_APPDATA, TRUE) )		// 得到默认 %APPDATA%
		{
			if ( !::StrStrI(pszPath, PATH_TYPE_APPDATA) )
			{
				::PathAppend(pszPath, PATH_TYPE_APPDATA);
				dwRet = _tcslen(pszPath);
			}
		}
	}
	else if ( _tcsicmp(pszToken, PATH_TOKEN_APPDOC) == 0 )
	{
		dwRet = ::GetEnvironmentVariable(PATH_TOKEN_APPDOC, pszPath, dwSize);	// 如果有重定向
		if( dwRet > 0 && dwRet < dwSize || 
			::SHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, TRUE) )		// 得到默认 %HOMEPATH%\Documents
		{
			if ( !::StrStrI(pszPath, PATH_TYPE_APPDOC) )
			{
				::PathAppend(pszPath, PATH_TYPE_APPDOC);
				dwRet = _tcslen(pszPath);
			}
		}
	}
	else if ( _tcsicmp(pszToken, PATH_TOKEN_LOGIN) == 0 )
	{
		dwRet = ::GetEnvironmentVariable(PATH_TOKEN_LOGIN, pszPath, dwSize);	// 如果有重定向
	}
	else if ( _tcsicmp(pszToken, PATH_TOKEN_TMP) == 0 )
	{
		dwRet = ::GetTempPath(dwSize, pszPath);
	}

	SASSERT(dwRet > 0 && dwRet < dwSize);
	PathAddBackslash(pszPath);
	return (dwRet > 0 && dwRet < dwSize);
}

LPCTSTR C_UICore::GetPath(LPCTSTR pszToken)
{
	const int kPathMax = MAX_PATH * 2;	// 定义默认最大长度，如果不够，就加大
	__declspec(thread) static TCHAR s_szRet[kPathMax] = { 0 };	// 线程安全变量
	static TCHAR s_szApp[kPathMax] = { 0 };
	static TCHAR s_szAppData[kPathMax] = { 0 };
	static TCHAR s_szAppDoc[kPathMax] = { 0 };
	static TCHAR s_szTmp[kPathMax] = { 0 };

	if ( !s_szApp[0] || !s_szAppData[0] || !s_szAppDoc[0] || !s_szTmp[0] )
	{	// 初始化全局变量
		_GetPath(PATH_TOKEN_APP, s_szApp, kPathMax);
		_GetPath(PATH_TOKEN_APPDATA, s_szAppData, kPathMax);
		_GetPath(PATH_TOKEN_APPDOC, s_szAppDoc, kPathMax);
		_GetPath(PATH_TOKEN_TMP, s_szTmp, kPathMax);
	}

	if ( _tcsnicmp(pszToken, PATH_TOKEN_APP, _countof(PATH_TOKEN_APP) - 1) == 0 )
	{
		_tcscpy(s_szRet, s_szApp);
		pszToken += _countof(PATH_TOKEN_APP) - 1;
	}
	else if ( _tcsnicmp(pszToken, PATH_TOKEN_APPDATA, _countof(PATH_TOKEN_APPDATA) - 1) == 0 )
	{
		_tcscpy(s_szRet, s_szAppData);
		pszToken += _countof(PATH_TOKEN_APPDATA) - 1;
	}
	else if ( _tcsnicmp(pszToken, PATH_TOKEN_APPDOC, _countof(PATH_TOKEN_APPDOC) - 1) == 0 )
	{
		_tcscpy(s_szRet, s_szAppDoc);
		pszToken += _countof(PATH_TOKEN_APPDOC) - 1;
	}
	else if ( _tcsnicmp(pszToken, PATH_TOKEN_TMP, _countof(PATH_TOKEN_TMP) - 1) == 0 )
	{
		_tcscpy(s_szRet, s_szTmp);
		pszToken += _countof(PATH_TOKEN_TMP) - 1;
	}
	else
		SASSERT(FALSE);

	while ( *pszToken == _T('\\') || *pszToken == _T('/') )
		pszToken++;	// 略过路径分隔符

	if ( _tcsnicmp(pszToken, PATH_TOKEN_LOGIN, _countof(PATH_TOKEN_LOGIN) - 1) == 0 )
	{	// 登录账号： account@company
		TCHAR szLogin[128] = { 0 };

		_GetPath(PATH_TOKEN_LOGIN, szLogin, _countof(szLogin));
		_tcscat(s_szRet, szLogin);
		pszToken += _countof(szLogin);
	}

	::PathAppend(s_szRet, pszToken);
	MakeDir(s_szRet);
	return s_szRet;
}

//=============================================================================
//Function:     eIMMakeDir
//Description:	Make multi-level folders
//
//Parameter:
// lpszPath		- The path to be make(MUST be ended by '\')
//
//Return:
//		TRUE    - Succeeded
//		FALSE	- Failed
//
//=============================================================================
inline BOOL C_UICore::MakeDir(LPCTSTR lpszPath)
{
	if ( ::PathFileExists(lpszPath) )
		return TRUE;

	if (lpszPath )
	{
		TCHAR szPathTmp[MAX_PATH] = { 0 };
		_tcscpy(szPathTmp, lpszPath);

		int i32Index = _tcslen(szPathTmp);
		while ( --i32Index > 0 )
		{
			if ( szPathTmp[i32Index] == _T('\\') || szPathTmp[i32Index] == _T('/') )
			{	// Truncate file name
				szPathTmp[i32Index] = _T('\0');
				break;
			}
		}

		int i32Ret = SHCreateDirectoryEx(NULL, szPathTmp, NULL);
		if (SUCCEEDED(i32Ret) || i32Ret == ERROR_FILE_EXISTS || i32Ret == ERROR_ALREADY_EXISTS )
			return TRUE;
	}

	STRACE(_T("Path is invalid or not have the write privilege, error: 0x%08X"), ::GetLastError());
	return FALSE;
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
//    C_XmlLang* pXmlLang = GetErrorLang(NULL);
    
//    if ( pXmlLang && (pszRet = pXmlLang->SetLangAttr(dwErrId, lpszDesc)))
//       return pszRet;
       
	return lpszDesc;
}



HWND C_UICore::UIGetMainWnd()
{
	if ( m_pdlgMain )
	return m_pdlgMain->GetContainer()->GetHostHwnd(); 

	return NULL;
}

HWND C_UICore::UIGetPopupWnd()
{
//	CHECK_NULL_RET_( m_pwndDummy, NULL );
//	return m_pwndDummy->GetHWND();
	return NULL;
}



BOOL C_UICore::ShowLoginWnd(DWORD dwFlag)
{
	//// 创建并且显示 登录窗口
	//C_LoginWnd* pwndLogin = new C_LoginWnd();
	//CHECK_NULL_RET_(pwndLogin, FALSE);

	//if (!pwndLogin->Create(NULL, _T("eIMLoginWnd"),  (UI_WNDSTYLE_DIALOG & ~WS_VISIBLE), WS_EX_APPWINDOW))
	//{
	//	SAFE_DELETE_PTR_(pwndLogin);
	//	EIMLOGGER_ERROR_(m_pIUILogger, _T("Create C_LoginWnd window failed"));
	//	return FALSE;
	//}
	//
	//if ( UIGetAttributeInt(PATH_APP, APP_ATTR_MULTI_INST) )
	//	dwFlag |= RUN_FLAG_MULTI_INSTANCE;

	//if ( !pwndLogin->CheckMultiInstance(dwFlag & RUN_FLAG_MULTI_INSTANCE) )
	//{
	//	pwndLogin->Close();
	//	return FALSE;
	//}

	//UICenterWindow(pwndLogin->GetHWND());
	//if ( IDOK != pwndLogin->ShowModal() )
	//{
	//	EIMLOGGER_ERROR_(m_pIUILogger, _T("User canceled login"));
	//	return FALSE;
	//}

	//return TRUE;
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
	return m_AppCfg.GetAttributeStr(lpszPath, lpszAttrName, pszDefault, bEnableRedirect == TRUE);
}

BOOL C_UICore::UISetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue )
{
	return m_AppCfg.SetAttributeStr(lpszPath, lpszAttrName, lpszValue);
}

int C_UICore::UIGetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default, BOOL bEnableRedirect )
{
	return m_AppCfg.GetAttributeInt(lpszPath, lpszAttrName, i32Default, bEnableRedirect == TRUE);
}

BOOL C_UICore::UISetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value )
{
	return m_AppCfg.SetAttributeInt(lpszPath, lpszAttrName, i32Value);
}

LPCTSTR C_UICore::UIGetText( LPCTSTR lpszPath, LPTSTR pszDefault, BOOL bEnableRedirect )
{
	return m_AppCfg.GetText(lpszPath, pszDefault, bEnableRedirect == TRUE);
}

BOOL C_UICore::UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue )
{
	return m_AppCfg.SetText(lpszPath, lpszValue);
}

BOOL C_UICore::UISaveConfig()
{
	return m_AppCfg.Save();
}

DWORD C_UICore::GetTimeNow()
{
	//CHECK_NULL_RET_(m_pIEngine, (DWORD)time(NULL));
	//return m_pIEngine->GetTimeNow();
	return (DWORD)time(NULL);
}

int C_UICore::GetLoginState()
{
//	CHECK_NULL_RET_(m_pIEngine, 0);
//	return m_pIEngine->GetPurview(GET_LOGIN_STATUS);
	return 0;
}


BOOL C_UICore::UIUpdateHotkey(DWORD dwType, UINT u32Key, UINT u32Modifier)
{
	return TRUE;// m_pwndMain->UpdateHotkey(dwType, u32Key, u32Modifier);
}

DWORD C_UICore::GetPurview(DWORD u32PurType, DWORD dwId)
{
	return 1;
}

LPCTSTR C_UICore::GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault)
{
//	CHECK_NULL_RET_( m_pIEngine, NULL );
//	return m_pIEngine->GetAttributeStr(pszName, pszDefault);
	return NULL;
}

int C_UICore::GetAttributeInt(LPCTSTR pszName, int i32Default)
{
	//CHECK_NULL_RET_( m_pIEngine, 0 );
	//return m_pIEngine->GetAttributeInt(pszName, i32Default);
	return 0;
}

BOOL C_UICore::UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName)
{
//	CHECK_NULL_RET_(m_pwndMain, FALSE);
//	return m_pwndMain->GetMsgSummary(pszXmlMsg, pszMsgSummary, i32Size, bPrefixName);
	return FALSE;
}

int C_UICore::AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId)
{
//	CHECK_NULL_RET_( m_pIEngine, EIMERR_NO_INTERFACE);
//	return m_pIEngine->AddCmd((E_ThreadIdx)dwThreadId, dwCmdId, pvData);
	return 0;
}

//int C_UICore::AddCmd(I_EIMCmd* pCmd, DWORD dwThreadId, BOOL bAddRef)
//{
//	CHECK_NULL_RET_( m_pIEngine, EIMERR_NO_INTERFACE);
//	return m_pIEngine->AddCmd((E_ThreadIdx)dwThreadId, pCmd, bAddRef);
//}

int C_UICore::Command(LPCTSTR pszXml, DWORD dwThreadId)
{
//	CHECK_NULL_RET_( m_pIEngine, EIMERR_NO_INTERFACE);
//	return m_pIEngine->Command(pszXml, (E_ThreadIdx)dwThreadId);
	return 0;
}

BOOL C_UICore::IsFace(LPCTSTR pszFace)
{
//	CHECK_NULL_RET_( m_pIEngine, FALSE);
//	return m_pIEngine->IsFace(pszFace);
	return FALSE;
}

DWORD C_UICore::ClearHistoryFile(LPCTSTR pszFindPath, LPCTSTR pszPattern, DWORD dwBeforeDays, int i32Level)
{
	WIN32_FIND_DATA ffd = { 0 };
	//eIMString	szPath;
	FILETIME	fileTimeNow = { 0 };
	UINT64		u64DaysofNs = (UINT64)dwBeforeDays * 864000000000I64;	// 864000000000I64 is 1days of 100-ns
	UINT64		u64FileTime = 0;
	DWORD		dwCount     = 0;

	//::GetSystemTimeAsFileTime(&fileTimeNow);
	//::eIMGetFullPath(pszFindPath, NULL, pszPattern, szPath, FALSE);

	//BOOL	bRet  = TRUE;
	//HANDLE	hFind = FindFirstFile(szPath.c_str(), &ffd);
	//UINT64	u64TimeNow  = (((UINT64)fileTimeNow.dwHighDateTime << 32) + (UINT64)fileTimeNow.dwLowDateTime);
	//if ( hFind == INVALID_HANDLE_VALUE )
	//	return dwCount;

	//do
	//{
	//	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Is file
	//	{
	//		if ( i32Level > 0 &&						// Need find sub folder
	//			_tcsicmp(ffd.cFileName, _T(".")) &&		// Not folder "."
	//			_tcsicmp(ffd.cFileName, _T("..")) )		// Not folder ".."
	//		{	// Sub folder
	//			::eIMGetFullPath(pszFindPath, ffd.cFileName, _T(""), szPath, FALSE);
	//			dwCount += ClearHistoryFile(szPath.c_str(), pszPattern, dwBeforeDays, i32Level - 1);
	//		}

	//		continue;
	//	}

	//	u64FileTime = (((UINT64)ffd.ftCreationTime.dwHighDateTime << 32) + (UINT64)ffd.ftCreationTime.dwLowDateTime);
	//	if ( u64TimeNow - u64FileTime > u64DaysofNs )	// Only subject result greater than the days
	//	{
	//		::eIMGetFullPath(pszFindPath, NULL, ffd.cFileName, szPath, FALSE);
	//		if( ::DeleteFile(szPath.c_str()) )
	//		{
	//			dwCount++;
	//			LOGFMTD(LOG_OIM, _T("Delete history file[%s] succeed!"), szPath.c_str());
	//		}
	//		else
	//		{
	//			LOGFMTD(LOG_OIM, _T("Delete history file[%s] failed! error:0x%08x"), szPath.c_str(), ::GetLastError());
	//		}
	//	}
	//}while (FindNextFile(hFind, &ffd) != 0);

	//FindClose(hFind);
	return dwCount;
}

LPCTSTR C_UICore::UIGetAppVersion(BOOL bTrimBuildVer)
{	
	return m_AppCfg.GetAttributeStr(PATH_APP, APP_ATTR_VERSION, _T("0.0.1.0") , FALSE);
}

//
//BOOL C_UICore::ShowViewer(LPCTSTR pszImage, QFID fid)
//{
//	static PROCESS_INFORMATION pi = { 0 }; 
//	STARTUPINFO si;
//	DWORD	dwExitCode = 0;
//	eIMString szViewer = PATH_TOKEN_EXE_FILE_(_T("ImageViewer.exe"));
//	SStringT  szCmdLine;
//
//	ZeroMemory(&si, sizeof(si));
//	si.cb = sizeof(si);
//	eIMReplaceToken(szViewer);
//
//	if ( !PathFileExists(szViewer.c_str()) )
//		return FALSE;
//
//	if ( pi.hProcess && GetExitCodeProcess(pi.hProcess, &dwExitCode) && dwExitCode == STILL_ACTIVE)	// 关闭之前的进程
//		TerminateProcess(pi.hProcess, 1);
//
//	SAFE_CLOSE_HANDLE_(pi.hProcess);
//	if ( fid )
//	{	// 有FID，就使用数据库模式(-db)
//		eIMString szDbFile = PATH_TOKEN_LOGINID_FILE_(_T("msg.db"));
//	
//		eIMReplaceToken(szDbFile);
//		szCmdLine.Format(_T("-taskbar=0 -fid=%llu -db=\"%s\""), fid, szDbFile.c_str());
//	}
//	else
//	{	// 没有FID， 只显示单个文件（表情，文件类型）
//		CHECK_NULL_RET_(pszImage, FALSE);
//		szCmdLine.Format(_T("-taskbar=0 -images=\"%s\""), pszImage);
//	}
//
//	if (!CreateProcess((TCHAR*)szViewer.c_str(), (TCHAR*)szCmdLine.GetData(), NULL,NULL,FALSE,0,NULL,NULL, &si, &pi))
//		return FALSE;
//
//	CloseHandle(pi.hThread);
//	return TRUE;
//}
