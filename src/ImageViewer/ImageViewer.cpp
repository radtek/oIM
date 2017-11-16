// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
#include "SImageSwitcher.h"
#include "Shellapi.h"
 //从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 1
// #define RES_TYPE 1  //从PE资源中加载UI资源

	
//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP
	
BOOL Usage()
{
	SStringT szUsage = GETSTRING(R.string.usages);
	SStringT szHelp  = GETSTRING(R.string.usage_help);
	SMessageBox(NULL, szUsage, szHelp, MB_OK | MB_ICONINFORMATION);

	return FALSE;
}

DWORD FindImages(const SStringT& szFindPath, const SStringT& szFilter, VectImage& vectImage)
{
	WIN32_FIND_DATA ffd = { 0 };
	BOOL	bMultiFilter= (szFilter.Find(_T('|')) > 0 || szFilter.Find(_T(';')) > 0);	// 有分隔符，就当成多个过滤器
	HANDLE	hFind = FindFirstFile(szFindPath + (bMultiFilter ? _T("*.*") : szFilter), &ffd);
	if ( hFind == INVALID_HANDLE_VALUE )
		return 0;

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Is folder
			continue;

		if ( bMultiFilter == FALSE )
		{	// 不是多过滤器，直接添加到列表
			vectImage.push_back(szFindPath + ffd.cFileName);
			continue;
		}

		// 是多过滤器，只用扩展名进行过滤
		if ( szFilter.Find(PathFindExtension(ffd.cFileName)) >= 0)
			vectImage.push_back(szFindPath + ffd.cFileName);
	}while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	STRACE(_T("Find '%s' result count: %d"), szFilter, vectImage.size());
	return vectImage.size();
}

SStringT GetParamValue(LPWSTR pszArg, LPWSTR pszNext, int& i32Index)
{	// splited by ' ', '=',':' and not is a '"'
	while(*pszArg)
	{	// Find the value begin position
		if ( *pszArg == _T(' ') || *pszArg == _T('=') || *pszArg == _T(':') )
		{
			pszArg++;
			break;
		}

		if ( *pszArg == _T('"') )
			break;

		pszArg++;
	}

	while( *pszArg == _T(' ') || *pszArg == _T('"') )
		pszArg++;	// Skip space and '"'

	if ( pszArg && pszArg[_tcslen(pszArg) - 1] == _T('"') )
		pszArg[_tcslen(pszArg) - 1] = _T('\0');	// Remove end '"'

	SStringT szRet;
	if ( pszArg )
		szRet = pszArg;
	else if (pszNext && !(pszNext[0] == _T('-') || pszNext[0] == _T('/')))
	{
		i32Index++;
		szRet = pszNext; // The value is in next arg
	}

	szRet.TrimBlank();
	szRet.Trim(_T('\"'));
	return szRet;
}

BOOL IsOption(LPWSTR pszArg, LPWSTR pszName)
{
	if ( pszArg && pszName && (pszArg[0] == _T('-') || pszArg[0] == _T('/')) ) 
		return (_tcsnicmp(&pszArg[1], pszName, _tcslen(pszName)) == 0);

	return FALSE;
}

BOOL ParseCommandLine(LPTSTR lpstrCmdLine)
{
	int i32Num = 0;
	int i32Index = 0;
	LPWSTR* pszArgs = CommandLineToArgvW(GetCommandLine(), &i32Num);
	if ( i32Num == 1 )
		return Usage();

	for ( ; i32Index < i32Num; i32Index++ )
	{
		LPWSTR pszArg = pszArgs[i32Index];
		LPWSTR pszNext=(i32Index + 1 < i32Num) ? pszArgs[i32Index + 1] : NULL;
		if ( IsOption(pszArg, _T("images")) )
		{
			SStringT szImgPath = _T("${ipath}");	// 不能使用格式: $(ipath), 因为它会被VS替换成空的。
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.IsEmpty() )
				return Usage();

			for (LPWSTR pszToken = _tcstok((TCHAR*)(const TCHAR*)szValue, _T("|")); pszToken; pszToken = _tcstok(NULL, _T("|")) )
			{
				SStringT szImage = pszToken;
				if (szImage.Find(szImgPath) >= 0 )
					szImage.Replace(szImgPath, g_sParams.szImgPath);
		
				g_sParams.vectImage.push_back(szImage);
			}
		}
		else if ( IsOption(pszArg, _T("ifilter")) )
		{
			g_sParams.szImgFilter = GetParamValue(pszArg, pszNext, i32Index);
			if ( g_sParams.szImgFilter.IsEmpty() || g_sParams.szImgPath.IsEmpty() )
				return Usage();

			FindImages(g_sParams.szImgPath, g_sParams.szImgFilter, g_sParams.vectImage);
		}
		else if ( IsOption(pszArg, _T("ipath")) )
		{
			g_sParams.szImgPath = GetParamValue(pszArg, pszNext, i32Index);
			g_sParams.szImgPath.TrimRight(_T('\\'));
			g_sParams.szImgPath.TrimRight(_T('/'));
			g_sParams.szImgPath += _T("\\");
			if ( g_sParams.szImgPath.IsEmpty() )
				return Usage();
		}
		else if ( IsOption(pszArg, _T("db")) )
		{
			g_sParams.szDbFile = GetParamValue(pszArg, pszNext, i32Index);
			if ( g_sParams.szDbFile.IsEmpty() || !PathFileExists(g_sParams.szDbFile) )
				return Usage();
		}
		else if ( IsOption(pszArg, _T("fid")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.IsEmpty() )
				return Usage();

			g_sParams.u64Fid = _tcstoui64(szValue, NULL, 0);
		}
		else if ( IsOption(pszArg, _T("h")) || IsOption(pszArg, _T("?")) )
		{
			return Usage();
		}
	}

	return (g_sParams.vectImage.size() > 0 || !g_sParams.szDbFile.IsEmpty());
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));
	int nRet = 0;
    
    SComMgr *pComMgr = new SComMgr;

    //将程序的运行路径修改到项目所在目录所在的目录
    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
    LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
    _tcscpy(lpInsertPos + 1, _T("..\\ImageViewer"));
    SetCurrentDirectory(szCurrentDir);
    {
        BOOL bLoaded=FALSE;
        CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
        bLoaded = pComMgr->CreateRender_Skia((IObjRef**)&pRenderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        SApplication *theApp = new SApplication(pRenderFactory, hInstance);
		theApp->RegisterWindowClass<SImageSwitcher>();
	//	theApp->RegisterWindowClass<SToolbox>();
		{
            CAutoRefPtr<IResProvider> sysResProvider;
            CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
            sysResProvider->Init((WPARAM)hInstance, 0);
            theApp->LoadSystemNamedResource(sysResProvider);
        }

        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
        CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
        if (!pResProvider->Init((LPARAM)_T("uires"), 0))
        {
            SASSERT(0);
            return 1;
        }
#else 
        CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
        pResProvider->Init((WPARAM)hInstance, 0);
#endif

		theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
        theApp->AddResProvider(pResProvider);

        //加载多语言翻译模块。
        CAutoRefPtr<ITranslatorMgr> trans;
        bLoaded=pComMgr->CreateTranslator((IObjRef**)&trans);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("translator"));
        if(trans)
        {//加载语言翻译包
            theApp->SetTranslator(trans);
            pugi::xml_document xmlLang;
            if(theApp->LoadXmlDocment(xmlLang,_T("lang_cn"),_T("translator")))
            {
                CAutoRefPtr<ITranslator> langCN;
                trans->CreateTranslator(&langCN);
                langCN->Load(&xmlLang.child(L"language"),1);//1=LD_XML
                trans->InstallTranslator(langCN);
            }
        }
        
        // BLOCK: Run application
		if ( ParseCommandLine(GetCommandLine()) )
		{
			CMainDlg dlgMain;
            dlgMain.Create(GetActiveWindow());
            dlgMain.SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow(dlgMain.m_hWnd);
		//	dlgMain.ParseCommandLine(lpstrCmdLine);
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet = theApp->Run(dlgMain.m_hWnd);
        }

        delete theApp;
    }
    
    delete pComMgr;
    
    OleUninitialize();
    return nRet;
}
