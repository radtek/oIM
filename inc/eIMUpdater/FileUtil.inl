#include "FileUtil.h"
#include "eIMUpdater/UpdaterError.h"
#include <Shlwapi.h>
#include <ShellAPI.h>

inline int ReNameFile(LPCTSTR lpszOldName, LPCTSTR lpszNewName)
{
	int i32Ret = 0;

	int i32OldLen = _tcslen(lpszOldName);
	int i32NewLen = _tcslen(lpszNewName);
	if(NULL==lpszOldName || 0==i32OldLen || NULL==lpszNewName || 0==i32NewLen)
	{
		return UPD_ERR_PARAM_POINTER_NULL;
	}

	TCHAR *szOldName = new TCHAR[i32OldLen+2];
	TCHAR *szNewName = new TCHAR[i32OldLen+2];

	_tcscpy(szOldName, lpszOldName);
	szOldName[i32OldLen] = _T('\0');
	szOldName[i32OldLen+1] = _T('\0');
	
	_tcscpy(szNewName, lpszNewName);
	szNewName[i32NewLen] = _T('\0');
	szNewName[i32NewLen+1] = _T('\0');

	SHFILEOPSTRUCT lpFile; 
	lpFile.hwnd = NULL;
	lpFile.wFunc = FO_RENAME;
	lpFile.pFrom = szOldName;
	lpFile.pTo = szNewName;
	lpFile.fFlags = FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION/*|FOF_SILENT|FOF_NOERRORUI*/;
	lpFile.fAnyOperationsAborted = FALSE; 
	lpFile.hNameMappings = NULL; 
	lpFile.lpszProgressTitle = NULL; 

	i32Ret = SHFileOperation(&lpFile);
	
	if(szOldName){delete szOldName; szOldName = NULL;}
	if(szNewName){delete szNewName; szNewName = NULL;}

	return i32Ret;
}

inline int DelFile(LPCTSTR lpszName)
{
	int i32Ret = 0;

	int i32NameLen = _tcslen(lpszName);
	if(NULL==lpszName || 0==i32NameLen)
	{
		return UPD_ERR_PARAM_POINTER_NULL;
	}

	TCHAR *szName = new TCHAR[i32NameLen+2];
	_tcscpy(szName, lpszName);
	szName[i32NameLen] = _T('\0');
	szName[i32NameLen+1] = _T('\0');

	SHFILEOPSTRUCT lpFile; 
	lpFile.hwnd = NULL;
	lpFile.wFunc = FO_DELETE;
	lpFile.pFrom = szName;
	lpFile.pTo = NULL;
	lpFile.fFlags = FOF_NOCONFIRMATION|FOF_NOERRORUI;
	lpFile.fAnyOperationsAborted = FALSE; 
	lpFile.hNameMappings = NULL; 
	lpFile.lpszProgressTitle = NULL; 
	i32Ret = SHFileOperation(&lpFile);
	if ( i32Ret == 0x7C/*DE_INVALIDFILES*/ )
		i32Ret = 0;	// 还没有创建文件接收目录（没有接收任何文件）

	if(szName)
	{
		delete szName;
		szName = NULL;
	}

	return i32Ret;
}

inline int CopyFiles(LPCTSTR lpszSrc, LPCTSTR lpszDest)
{
	int i32Ret = 0;

	int i32SrcLen  = _tcslen(lpszSrc);
	int i32DestLen = _tcslen(lpszDest);

	if(NULL==lpszSrc || 0==i32SrcLen || NULL==lpszDest || 0==i32DestLen)
	{
		return UPD_ERR_PARAM_POINTER_NULL;
	}

	TCHAR *szSrc  = new TCHAR[i32SrcLen+2];
	TCHAR *szDest = new TCHAR[i32DestLen+2];

	_tcscpy(szSrc, lpszSrc);
	szSrc[i32SrcLen]   = _T('\0');
	szSrc[i32SrcLen+1] = _T('\0');

	_tcscpy(szDest, lpszDest);
	szDest[i32DestLen]   = _T('\0');
	szDest[i32DestLen+1] = _T('\0');

	SHFILEOPSTRUCT lpFile; 
	lpFile.hwnd = NULL; 
	lpFile.wFunc = FO_COPY;
	lpFile.pFrom = szSrc;
	lpFile.pTo = szDest;
	lpFile.fFlags = FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION|FOF_SILENT|FOF_NOERRORUI;
	lpFile.fAnyOperationsAborted = FALSE; 
	lpFile.hNameMappings = NULL; 
	lpFile.lpszProgressTitle = NULL;

	i32Ret = SHFileOperation(&lpFile);

	if(szSrc)
	{
		delete szSrc;
		szSrc = NULL;
	}
	if(szDest)
	{
		delete szDest;
		szDest = NULL;
	}

	return i32Ret;
}

inline BOOL RunProgram(LPCTSTR lpszProPath, LPCTSTR lpszCmdLine, int i32ShowType)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = lpszProPath;
	ShExecInfo.lpParameters = lpszCmdLine;  //参数：1：更新类型 2：下载URL 3：本地路径
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = i32ShowType;
	ShExecInfo.hInstApp = NULL;

	return ShellExecuteEx(&ShExecInfo);
}

inline BOOL CreateDir(LPCTSTR lpszNewDir)
{
	if(NULL==lpszNewDir || 0==_tcslen(lpszNewDir))
	{
		return FALSE;
	}

	if(!PathFileExists(lpszNewDir))
	{
		return CreateDirectory(lpszNewDir, NULL);
	}

	return TRUE;
}