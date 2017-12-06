// Main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <objbase.h>
#include <Shellapi.h>
#include "oIM.h"
#include "UICore\I_UICore.h"
#include "Public\Plugin.h"


static I_UICore* s_pIUICore = NULL;

#pragma comment(linker, "/EXPORT:CreateInterface=_CreateInterface@8")
extern "C" int __stdcall CreateInterface(const TCHAR* pctszIID, void** ppvIObject)
{
	CHECK_NULL_RET_(pctszIID, ERR_INVALID_POINTER);
	CHECK_NULL_RET_(ppvIObject, ERR_INVALID_POINTER);

	if( _tcsnicmp(pctszIID, INAME_UICORE, _tcslen(INAME_UICORE)) == 0 )
	{
		*ppvIObject = s_pIUICore;
		return ERR_NO_ERROR;
	}

	return ERR_NOT_IMPL;
}

BOOL Run(HINSTANCE hInstance, DWORD dwFlag)
{
	C_PluginDll dllCore(ePLUGIN_TYPE_NORMAL, INAME_UICORE);
	if ( !dllCore.Load(INAME_UICORE_DLL) )
	{
		STRACE(_T("Load [%s] failed, exit!"), INAME_UICORE_DLL); 
		return FALSE;
	}

	int i32Result = dllCore.CreateInterface(INAME_UICORE, (void**)&s_pIUICore);
	if ( FAILED(i32Result) )
	{
		STRACE(_T("Create [%s] interface failed, errcode=0x%08X, exit!"), INAME_UICORE_DLL, i32Result); 
		return FALSE;
	}

	if ( !s_pIUICore->InitPlugin(NULL, NULL) )
	{
		STRACE(_T("[%s] InitPlugin failed, exit!"), INAME_UICORE); 
		return FALSE;
	}

	STRACE(_T("Before of call I_UICore::Run(0x%08x)"), hInstance); 
	BOOL bRet = s_pIUICore->Run(dwFlag);
	STRACE(_T("After of call I_UICore::Run(0x%08x), Result:%s"), hInstance, bRet ? _T("True") : _T("False")); 

	s_pIUICore->FreePlugin();
	SAFE_RELEASE_INTERFACE_(s_pIUICore);

	return TRUE;
}

BOOL ParseParam(LPTSTR lpCmdLine,DWORD& dwFlag)
{
	int	    nArgs  = 0;
	if (LPWSTR* pwszArgs = CommandLineToArgvW(lpCmdLine, &nArgs) )
	{
		for ( int n = 0; n < nArgs; n++ )
		{
			if ( _tcscmp(pwszArgs[n], _T("-m") ) == 0 )
				dwFlag |= RUN_FLAG_MULTI_INSTANCE;
		}

		LocalFree(pwszArgs);
		return TRUE;
	}

	return FALSE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DWORD	dwFlag = 0;
	ParseParam(lpCmdLine, dwFlag);

	::OleInitialize(NULL);
	::Run(hInstance, dwFlag);
	::OleUninitialize();

	return 0;
}

