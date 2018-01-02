#include <shlobj.h>
#include <shlwapi.h>
#include <Dbghelp.h>
#include <sys/stat.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Dbghelp.lib")

inline BOOL oIMGetTokenPath(LPCTSTR pszToken, LPTSTR pszPath, DWORD dwSize)
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

inline LPCTSTR oIMGetPath(LPCTSTR pszToken)
{
	const int kPathMax = MAX_PATH * 2;	// 定义默认最大长度，如果不够，就加大
	__declspec(thread) static TCHAR s_szRet[kPathMax] = { 0 };	// 线程安全变量
	static TCHAR s_szApp[kPathMax] = { 0 };
	static TCHAR s_szAppData[kPathMax] = { 0 };
	static TCHAR s_szAppDoc[kPathMax] = { 0 };
	static TCHAR s_szTmp[kPathMax] = { 0 };

	if ( !s_szApp[0] || !s_szAppData[0] || !s_szAppDoc[0] || !s_szTmp[0] )
	{	// 初始化全局变量
		oIMGetTokenPath(PATH_TOKEN_APP, s_szApp, kPathMax);
		oIMGetTokenPath(PATH_TOKEN_APPDATA, s_szAppData, kPathMax);
		oIMGetTokenPath(PATH_TOKEN_APPDOC, s_szAppDoc, kPathMax);
		oIMGetTokenPath(PATH_TOKEN_TMP, s_szTmp, kPathMax);
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
	{	// 登录账号： company\\account
		TCHAR szLogin[128] = { 0 };

		oIMGetTokenPath(PATH_TOKEN_LOGIN, szLogin, _countof(szLogin));
		_tcscat(s_szRet, szLogin);
		pszToken += _countof(szLogin);
	}

	::PathAppend(s_szRet, pszToken);
	oIMMakeDir(s_szRet);
	return s_szRet;
}

//=============================================================================
//Function:     oIMMakeDir
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
inline BOOL oIMMakeDir(LPCTSTR lpszPath)
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




