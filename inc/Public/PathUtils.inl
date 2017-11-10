// PathUtils.inl
#include <shlobj.h>
#include <shlwapi.h>
#include <Dbghelp.h>
#include <sys/stat.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Dbghelp.lib")


//=============================================================================
//Function:     eIMGetSpecialFolderPath
//Description:	Get the special folder path
//Parameter:
// lpszPath		- A pointer to a null-terminated string that receives the drive and 
//				  path of the specified folder. This buffer must be at least MAX_PATH characters in size.
// nFolder		- A CSIDL that identifies the folder of interest. If a virtual folder 
//				  is specified, this function will fail. 
// eg.:
//  CSIDL_APPDATA  - C:\Documents and Settings\username\Application Data
//  CSIDL_PERSONAL - C:\Documents and Settings\username\My Documents
//  CSIDL_SYSTEM   - C:\Windows\System32
//Return:
//		TRUE	if successful
//		FALSE	else otherwise. 
//=============================================================================
inline BOOL eIMGetSpecialFolderPath(LPTSTR lpszPath, int i32Folder)
{
	return ::SHGetSpecialFolderPath( NULL, lpszPath, i32Folder, TRUE );
}

//=============================================================================
//Function:     eIMGetTempPath
//Description:	Get the temp path
//
//Parameter:
//	dwSize			  - The size of the string buffer identified by lpBuffer, in TCHARs.
//  lpszBuffer        - A pointer to a string buffer that receives the null-terminated
//                    string specifying the temporary file path. The returned string 
//                    ends with a backslash, for example, C:\TEMP\.
//Return:
//  If the function succeeds, the return value is the length, in TCHARs, of the string 
//  copied to lpBuffer, not including the terminating null character. If the return value 
//  is greater than nBufferLength, the return value is the length, in TCHARs, of the buffer
// required to hold the path.
//
// If the function fails, the return value is zero. To get extended error information, call GetLastError.
//=============================================================================
inline DWORD eIMGetTempPath( DWORD dwSize, LPTSTR lpszBuffer)
{
	__try
	{
		return ::GetTempPath( dwSize, lpszBuffer);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		ASSERT_( FALSE );
		return 0;
	}
}

//=============================================================================
//Function:     eIMGetTempFileName
//Description:	Creates a name for a temporary file
//
//Parameter:
// lpszPathName - The directory path for the file name. Applications typically specify a 
//              period (.) for the current directory or the result of the GetTempPath function. The 
//              string cannot be longer than MAX_PATH¨C14 characters or GetTempFileName will fail. 
//              If this parameter is NULL, the function fails.
// lpszTempFileName - A pointer to the buffer that receives the temporary file name. This buffer should
//                  be MAX_PATH characters to accommodate the path plus the terminating null character.
//
//Return:
//		If the function succeeds, the return value specifies the unique numeric value used in the temporary file name.
//      If the function fails, the return value is zero. To get extended error information, call GetLastError.
//=============================================================================
inline DWORD eIMGetTempFileName( LPCTSTR lpszPathName, LPTSTR lpszTempFileName)
{
	 __try 
	 {
		return ::GetTempFileName( lpszPathName, _T("eIM"), 0, lpszTempFileName );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		ASSERT_( FALSE );
		return 0;
	}
}

//=============================================================================
//Function:     eIMGetModuleFilePath
//Description:	Get a module file path
//
//Parameter:
// lpszModuleName   - The module name to get module(*.exe, *.dll) file path, NULL is return the exe path
// lpszFilePath	  - A pointer to a buffer that receives the fully-qualified path of the module. 
// dwSize		  - The size of the lpszFilePath buffer, in TCHARs.

// Return: 
//      If the function succeeds, the return value is the length of the string that is copied to the buffer, 
//      in characters, not including the terminating null character. If the buffer is too small to hold the 
//      module name, the string is truncated to nSize characters including the terminating null character, 
//      the function returns nSize, and the function sets the last error to ERROR_INSUFFICIENT_BUFFER.
//=============================================================================
inline DWORD eIMGetModuleFilePath( LPCTSTR lpszModuleName, LPTSTR lpszFilePath, DWORD dwSize )
{
	return ::GetModuleFileName( GetModuleHandle( lpszModuleName ), lpszFilePath, dwSize );
}

//=============================================================================
//Function:     eIMNormalPathFile
//Description:	Normal path file 
//
//Parameter:
// lpszFileName		- The file path to be normaled
// dwSize			- The size of the buffer to receive the null-terminated string for the drive and path, in TCHARs. 
// lpszBuffer		- A pointer to a buffer that receives the null-terminated string for the drive and path.
// Return: 
//	If the function succeeds, the return value is the length of the string copied to lpBuffer,
//	not including the terminating null character, in TCHARs.
//	If the lpBuffer buffer is too small to contain the path, the return value is the size of the
//	buffer that is required to hold the path and the terminating null character, in TCHARs. 
//	Therefore, if the return value is greater than nBufferLength, call the function again with 
//	a buffer that is large enough to hold the path.
//	If the function fails for any other reason, the return value is zero (0). To get extended 
//	error information, call GetLastError.
//=============================================================================
inline DWORD eIMNormalPathFile( LPCTSTR lpszFileName, LPTSTR lpszBuffer, DWORD dwSize )
{
	if ( lpszFileName && lpszFileName == lpszBuffer )
	{
		int		i32TempSize = ( _tcslen(lpszFileName) + 1 ) * sizeof(TCHAR);
		TCHAR*	pszTmp = (TCHAR*)_alloca( i32TempSize );
		
		_tcscpy( (TCHAR*)pszTmp, lpszFileName);
		return ::PathCanonicalize( lpszBuffer, pszTmp);
	}

	return ::PathCanonicalize( lpszBuffer, lpszFileName);
}

//=============================================================================
//Function:     eIMAddPath
//Description:	Add a subfolder or file to the path
//
//Parameter:
// lpszPath			- The path to be add to
// lpszAdd			- The added subfolder or file 
// 
// Return: 
//	If the function succeeds, the return value is the length of the string copied to lpBuffer,
//	not including the terminating null character, in TCHARs.
//	If the lpBuffer buffer is too small to contain the path, the return value is the size of the
//	buffer that is required to hold the path and the terminating null character, in TCHARs. 
//	Therefore, if the return value is greater than nBufferLength, call the function again with 
//	a buffer that is large enough to hold the path.
//	If the function fails for any other reason, the return value is zero (0). To get extended 
//	error information, call GetLastError.
//=============================================================================
inline DWORD eIMAddPath( LPTSTR lpszPath, LPCTSTR lpszAdd)
{
	if (!::PathAppend(lpszPath, lpszAdd))
		return 0;

	return _tcslen(lpszPath);
}

//=============================================================================
//Function:     eIMPathFileExists
//Description:	Determines whether a path to a file system object such as a file or directory is valid.
//
//Parameter:
// lpszPath		- A pointer to a null-terminated string of maximum length MAX_PATH that 
//				  contains the full path of the object to verify.
//Return:
//		Returns TRUE if the file exists, or FALSE otherwise.
//=============================================================================
inline BOOL eIMPathFileExists( LPCTSTR lpszPath )
{
	return ::PathFileExists( lpszPath );
}

inline BOOL	eIMPathFileExistNoEmpty(LPCTSTR lpszPath)
{
	struct __stat64	statFile = { 0 };
	if ( lpszPath && lpszPath[0] && ::PathFileExists(lpszPath) &&
		!_tstat64(lpszPath, &statFile) && statFile.st_size > 0)
		return TRUE;

	return FALSE;
}

//=============================================================================
//Function:     eIMGetPath
//Description:	Get the predefined path, or relative $(EXE)'s subfolder.
//
//Parameter:
// lpszPathType - See PATH_TYPE_*
// lpszPath		- The path to received.
// dwSize		- lpszPath's size in TCHARs
//Return:
//	If the function succeeds, the return value is the length of the string copied to lpBuffer,
//	not including the terminating null character, in TCHARs.
//	If the lpBuffer buffer is too small to contain the path, the return value is the size of the
//	buffer that is required to hold the path and the terminating null character, in TCHARs. 
//	Therefore, if the return value is greater than nBufferLength, call the function again with 
//	a buffer that is large enough to hold the path.
//	If the function fails for any other reason, the return value is zero (0). To get extended 
//	error information, call GetLastError.
//=============================================================================
inline DWORD eIMGetPath(LPCTSTR lpszPathType, LPTSTR lpszPath, DWORD dwSize)
{
	BOOL  bLog  = FALSE;
	BOOL  bId   = FALSE;
	DWORD dwRet = 0;
	TCHAR* pszFind = NULL;

	if( !_tcsicmp( lpszPathType, PATH_TYPE_APPDATA ) ||
		(bLog = ( 0 == _tcsicmp( lpszPathType, PATH_TYPE_LOG ))) ||
		(bId  = ( 0 == _tcsicmp( lpszPathType, PATH_TYPE_LOGINID ))))
	{	// %APPDATA%\eIM Data
		dwRet = ::GetEnvironmentVariable(PATH_TYPE_APPDATA, lpszPath, dwSize);
		if( (dwRet > 0 && dwRet < dwSize) || // First read the App environment var if exist redirect
			eIMGetSpecialFolderPath(lpszPath, CSIDL_APPDATA) ) // Get the system default %APPDATA%
		{
			if ( !::StrStrI(lpszPath, PATH_TYPE_APPDATA) ) 
				dwRet = eIMAddPath(lpszPath, PATH_TYPE_APPDATA);

			if( bLog )
				dwRet = eIMAddPath(lpszPath, PATH_TYPE_LOG);

			if ( bId )
			{
				TCHAR szLoginId[128] = { 0 };
				if ( ::GetEnvironmentVariable(PATH_TOKEN_LOGINID, szLoginId, 128) > 0 )
					dwRet = eIMAddPath(lpszPath, szLoginId);
				else
				{
					TRACE_(_T("***Can not call the token function with \"%s\" that without logined"), PATH_TOKEN_LOGINID);
					return 0;
				}
			}
		}
	}
	else if( !_tcsicmp( lpszPathType, PATH_TYPE_APPDOC ) ||
		(bId  = ( 0 == _tcsicmp( lpszPathType, PATH_TYPE_LOGINID_DOC ))))
	{	// %APPDATA%\eIM Files
		dwRet = ::GetEnvironmentVariable(PATH_TYPE_APPDOC, lpszPath, dwSize);
		if( (dwRet > 0 && dwRet < dwSize)  || // First read the App environment var if exist redirect
			eIMGetSpecialFolderPath(lpszPath, CSIDL_PERSONAL) )
		{
			if ( !::StrStrI(lpszPath, PATH_TYPE_APPDOC) ) 
				dwRet = eIMAddPath(lpszPath, PATH_TYPE_APPDOC);
			
			if ( bId )
			{
				TCHAR szLoginId[128] = { 0 };
				if ( GetEnvironmentVariable(PATH_TOKEN_LOGINID, szLoginId, 128) > 0 )
					dwRet = eIMAddPath(lpszPath, szLoginId);
				else
				{
					TRACE_(_T("***Can not call the token function with \"%s\" that without logined"), PATH_TOKEN_LOGINID);
					return 0;
				}
			}
		}
	}
	else
	{
		dwRet = eIMGetModuleFilePath(NULL, lpszPath, dwSize);
		if( dwRet < dwSize )
		{
			eIMSplitPathFile(lpszPath, NULL, NULL);
			dwRet = _tcslen(lpszPath);
			if( _tcsicmp( lpszPathType, PATH_TYPE_EXE ) )
				dwRet = eIMAddPath(lpszPath, lpszPathType);
		}
	}

	::PathAddBackslash(lpszPath);			// Append '\\' if not exist
	if( dwRet < dwSize &&					// No error
		!eIMPathFileExists(lpszPath) &&		// Not exist
		!eIMMakeDir(lpszPath) )				// Create directory
	{
		TRACE_(_T("eIMGetPath failed, error:0x%08X"), ::GetLastError());
		return 0;
	}

	return dwRet;
}

//=============================================================================
//Function:     eIMSplitPathFile
//Description:	Split path file as: Path[ + FileName[ + FileExt]]
//
//Parameter:
// lpszPathFile - Input the path file to split, return the Path
// lppszFile	- Return the FileName, Not include extension name when lpszExt not is NULL.
// lppszExt		- Return the extension name
//Return:
//		Returns TRUE if the file succeed, or FALSE otherwise.
//=============================================================================
inline BOOL eIMSplitPathFile(LPTSTR lpszPathFile, LPTSTR* lppszFile, LPTSTR* lppszExt)
{
	if( lpszPathFile == NULL || lpszPathFile[0] == _T('\0') )
		return FALSE;

	TCHAR* pszFind = _tcsrchr(lpszPathFile, _T('\\'));
	if( pszFind == NULL )
		pszFind = _tcsrchr(lpszPathFile, _T('/'));

	if( pszFind == NULL )
		return TRUE;

	if( lppszFile )
		*lppszFile = pszFind + 1;

	*pszFind++ = _T('\0');		//  truncate the File name
	
	if( lppszExt )
	{
		pszFind = _tcsrchr(pszFind, _T('.'));
		if( pszFind == NULL )
			return TRUE;

		*lppszExt = pszFind + 1;
		*pszFind = _T('\0');	//  truncate the ext name
	}

	return TRUE;
}

//=============================================================================
//Function:     eIMReplaceToken
//Description:	Replace internal defined token
//Notes:		Token is case-sensitive
//
//Parameter:
// szValue - Input the variable with token, output replaced value
//Return:
//		Return the size of replaced value
//=============================================================================
inline int eIMReplaceToken(eIMString& szValue)
{
	int	  i32Ret = 0;
	TCHAR szPath[TOKEN_MAX_SIZE] = { 0 };

	if ( szValue.find(PATH_TOKEN_EXE) != eIMString::npos )
	{
		if ( eIMGetPath(PATH_TYPE_EXE, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_EXE, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_APPDATA) != eIMString::npos )
	{
		if ( eIMGetPath(PATH_TYPE_APPDATA, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_APPDATA, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_APPDOC) != eIMString::npos )
	{
		if ( eIMGetPath(PATH_TYPE_APPDOC, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_APPDOC, szPath );
	}
	else if ( szValue.find( PATH_TOKEN_PLUGIN) != eIMString::npos )
	{
		if ( eIMGetPath( PATH_TYPE_PLUGIN, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_PLUGIN, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_APPMAN) != eIMString::npos )
	{
		if ( eIMGetPath( PATH_TYPE_APPMAN, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_APPMAN, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_LOG) != eIMString::npos )
	{
		if ( eIMGetPath( PATH_TYPE_LOG, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_LOG, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_EMOTICON) != eIMString::npos )
	{
		if ( eIMGetPath( PATH_TYPE_EMOTICON, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_EMOTICON, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_LOGINID) != eIMString::npos )
	{
		if ( eIMGetPath(PATH_TYPE_LOGINID, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_LOGINID, szPath );
	}
	else if ( szValue.find(PATH_TOKEN_LOGINID_DOC) != eIMString::npos )
	{
		if ( eIMGetPath(PATH_TYPE_LOGINID_DOC, szPath, _countof(szPath)) )
			i32Ret = eIMReplaceTokenAs( szValue, PATH_TOKEN_LOGINID_DOC, szPath );
	}
	else
	{
		TRACE_(_T("No token with \"%s\""), szValue.c_str() );
		i32Ret = szValue.size();
	}

	if ( i32Ret == 0 )
	{
		TRACE_(_T("Replace token failed \"%s\""), szValue.c_str() );
		szValue.clear();
		return i32Ret;
	}

	if ( szValue.find(PATH_TOKEN_LOGINNAME) != eIMString::npos )
	{
		DWORD dwRet = GetEnvironmentVariable(PATH_TOKEN_LOGINNAME, szPath, _countof(szPath));
		if( dwRet > 0 && dwRet < _countof(szPath))  // Replace as LoginName
			i32Ret = eIMReplaceTokenAs(szValue, PATH_TOKEN_LOGINNAME, szPath);
		else								// Replace as a space ' '
			i32Ret = eIMReplaceTokenAs(szValue, PATH_TOKEN_LOGINNAME, _T(""));
	}

	return i32Ret;
}

//=============================================================================
//Function:     eIMReplaceTokenAs
//Description:	Replace Token as To
//Notes:		Token is case-sensitive
//
//Parameter:
// szValue	- Input the variable with token, output replaced value
// pszToken	- Token to be replaced
// pszTo	- The token value 
//Return:
//		Return the size of replaced value
//=============================================================================
inline int eIMReplaceTokenAs(eIMString& szValue, const TCHAR* pszToken, const TCHAR* pszTo)
{
	if ( pszToken == NULL || pszTo == NULL )
		return -1;

	eIMString::size_type nPos = szValue.find(pszToken);
	if ( nPos !=  eIMString::npos )
	{
		szValue.replace( nPos, _tcslen(pszToken), pszTo );
	}

	return szValue.size();
}

//=============================================================================
//Function:     eIMGetLogFile
//Description:	Get log file path, when szLogFile is empty, will return the default
//				Log file path by parameter, else only replace the token and return.
//
//Parameter:
// szLogFile	- Input and output the logfile path 
// pszPrefix	- Logfile prefix, used as User name 
// pszPostfix	- Logfile postfix, used as log type
//Return:
//		Return the size of replaced value
//
//The full format as: PATH_TOKEN_LOG + pszPrefix + Date + pszPostfix
//=============================================================================
inline int eIMGetLogFile( eIMString& szLogFile, const TCHAR* pszPrefix, const TCHAR* pszPostfix)
{
	if ( szLogFile.size() == 0 )
	{
		TCHAR		szLogFile_[TOKEN_MAX_SIZE] = { 0 };
		SYSTEMTIME	sTime	  = { 0 };

		GetLocalTime(&sTime);	
		_sntprintf ( szLogFile_, _countof(szLogFile_), _T("%s[P%u]%s%s%04d%02d%02d%s%s.log"), 
			PATH_TOKEN_LOG, 
			GetCurrentProcessId(),
			pszPrefix ? pszPrefix : _T(""),
			pszPrefix ? _T("_") : _T(""),
			sTime.wYear, 
			sTime.wMonth,
			sTime.wDay, 
			pszPostfix ? _T("_") : _T(""),
			pszPostfix ? pszPostfix : _T("") );

		szLogFile = szLogFile_;
	}

	return eIMReplaceToken( szLogFile );
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
inline BOOL eIMMakeDir(LPCTSTR lpszPath)
{
	if (lpszPath )
	{
		TCHAR szPathTmp[TOKEN_MAX_SIZE] = { 0 };
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

	TRACE_(_T("Path is invalid or not have the write privilege, error: 0x%08X"), ::GetLastError());
	return FALSE;
}

//=============================================================================
//Function:     eIMGetSessionDocDir
//Description:	Get the session's sub directory of $(LoginIdDoc), created it when not exist
//
//Parameter:
// szDir		- Sub directory or file, eg.: PATH_TYPE_APPDOC_* or any other folder name
//				  Return the full path
//
//Return:
//		Return the size of path value
//
//The full format as: PATH_TOKEN_LOGINID_DOC + sid_xxx + szDir
//=============================================================================
inline DWORD eIMGetSessionDocDir(eIMString& szDir, QSID qsid)
{
	TCHAR szPath[TOKEN_MAX_SIZE] = { 0 };

#if 0
	_sntprintf(szPath, _countof(szPath), _T("%ssid_%llx\\%s"), PATH_TOKEN_LOGINID_DOC, qsid, szDir.c_str());

	szDir = szPath;
	::eIMReplaceToken(szDir);
	::eIMMakeDir(szDir.c_str());
#else
	_sntprintf(szPath, _countof(szPath), _T("%s%s"), PATH_TOKEN_LOGINID_DOC, szDir.c_str());

	szDir = szPath;
	::eIMReplaceToken(szDir);
	::eIMMakeDir(szDir.c_str());

	bool bExist = false;
	int iCount = 0;
	do
	{
		_sntprintf(szPath, _countof(szPath), _T("%s"), szDir.c_str());
		if(iCount > 0)
		{
			TCHAR szCount[10] = { 0 };
			_sntprintf(szCount, 9, _T("(%d)"), iCount);
			eIMString sTemp = szPath;
			eIMString::size_type nPos = sTemp.rfind(_T("."));
			if ( nPos !=  eIMString::npos )
			{
				sTemp.replace( nPos, 0, szCount);
			}
			_sntprintf(szPath, _countof(szPath), _T("%s"), sTemp.c_str());
		}
		struct _stat64i32 st;
		memset(&st, 0, sizeof(struct _stat64i32));
		int ret = _tstat(szPath,&st);
		if ( ret < 0) 
		{
			bExist = false;
		}
		else
		{
			bExist = true;
			iCount++;
		}
	}while(bExist);
	szDir = szPath;
#endif
	return szDir.size();
}

//=============================================================================
//Function:     eIMGetFullPath
//Description:	Get the full path by sub-directory and files
//
//Parameter:
// lpszTokenPath - Token path, eg.: PATH_TOKEN_LOGINID, PATH_TOKEN_LOGINID_DOC and so on
// lpszSubDir	 - Append sub-directory, can be NULL
// lpszFile		 - The original file name, it maybe renamed when already exist by append "(n) of file name when bRename is TRUE
// szFullPath	 - Return the full file path
// bRename		 - TRUE: will be rename the file
//Return:
//		Return the size of path value
//
//The full format as: 
//  PATH_TOKEN_LOGINID [+ SubDir] + File
//  PATH_TOKEN_LOGINID_DOC [+ SubDir] + File
//  PATH_TOKEN_EXE [+ SubDir] + File
//=============================================================================
inline DWORD eIMGetFullPath(LPCTSTR lpszTokenPath, LPCTSTR lpszSubDir, LPCTSTR lpszFile, eIMString& szFullPath, BOOL bRename)
{
	if ( lpszTokenPath == NULL || lpszFile == NULL )
		return 0;

	TCHAR	  szFile[TOKEN_MAX_SIZE] = { 0 };
	eIMString szRet = lpszTokenPath;

	::eIMReplaceToken(szRet);				// Replace token
	::PathCanonicalize(szFile, szRet.c_str() );	// Normal path
	if ( lpszSubDir )
		::eIMAddPath(szFile, lpszSubDir);	// Append sub-directory

	::eIMAddPath(szFile, lpszFile);			// Append file
	szRet	   = szFile;
	szFullPath = szRet;
	if (!::eIMMakeDir(szRet.c_str()))
		return 0;

	if ( bRename == FALSE )
		return szFullPath.size();

	if ( ::eIMPathFileExists(szRet.c_str()) )
	{
		int		iCount = 1;
		TCHAR	szPathTmp[MAX_PATH] = { 0 };
		TCHAR	szPath[MAX_PATH] = { 0 };
		TCHAR*	pszExt = NULL;
		TCHAR*  pszFile = NULL;
		_tcsncpy(szPathTmp, szRet.c_str(), MAX_PATH);
		::eIMSplitPathFile(szPathTmp, &pszFile, &pszExt);

		do
		{
			if ( pszExt )
				_sntprintf(szPath, MAX_PATH, _T("%s\\%s(%d).%s"), szPathTmp, pszFile, iCount++, pszExt);
			else
				_sntprintf(szPath, MAX_PATH, _T("%s\\%s(%d)"), szPathTmp, pszFile, iCount++);
		}while( ::eIMPathFileExists(szPath) );

		szRet = szPath;
	}

	FILE* pCreateFile = _tfopen(szRet.c_str(), _T("w+"));	// Make sure it exist now!
	if (pCreateFile)
		fclose(pCreateFile);

	szFullPath = szRet;
	return szFullPath.size();    
}

//=============================================================================
//Function:     eIMGetGrayImgPath
//Description:	Get the gray image path
//
//Parameter:
// lpszPath		- Return the path
// nSize		- Buffer size
//
//Return:
//	TRUE	- Succeeded
//  FALSE	- Failed
//=============================================================================
inline BOOL	eIMGetGrayImgPath(LPTSTR lpszPath,DWORD nSize)
{
	if (eIMGetPath(PATH_TYPE_LOGINID_DOC, lpszPath, nSize))
	{
		if (eIMAddPath(lpszPath, PATH_TYPE_LOGINID_DOC_GRAYFACE))
		{
			::PathAddBackslash(lpszPath);	
			if (!PathFileExists(lpszPath))
			{
				if (eIMMakeDir(lpszPath))
					return TRUE;
				else
					return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
