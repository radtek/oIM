#include "StdAfx.h"
#include "C_CommandLine.h"
#include "Shellapi.h"

C_CommandLine& C_CommandLine::GetObject()
{
	static C_CommandLine s_CommandLine;
	return s_CommandLine;
}

C_CommandLine::C_CommandLine(void)
	: m_nScale(100)
	, m_u64Fid(0)
{
	// 获取用户当前语言
	LANGID lang = GetUserDefaultLangID(); //GetSystemDefaultLangID();
	if ( LOBYTE(lang) == LANG_CHINESE )
		m_szLang = GETSTRING(R.string.lang_cn);
	else 
		m_szLang = GETSTRING(R.string.lang_en);

	// 根据当前系统DPI，设置默认值
	HWND hWnd = ::GetDesktopWindow();
	HDC hdc   = ::GetWindowDC(hWnd);
	int nDpiX = ::GetDeviceCaps(hdc, LOGPIXELSX); // 启用高DPI功能
	:: ReleaseDC(hWnd, hdc);
	if ( nDpiX >= 288 )			// *3
		m_nScale = 300;
	else if ( nDpiX >= 240 )	// *2.5
		m_nScale = 250;
	else if ( nDpiX >= 192 )	// *2
		m_nScale = 200;
	else if ( nDpiX >= 144 )	// *1.5
		m_nScale = 150;
	else if (nDpiX >= 120 )		// *1.25
		m_nScale = 125;
	else						// *1
		m_nScale = 100;
}

C_CommandLine::~C_CommandLine(void)
{
}

BOOL C_CommandLine::Usage()
{
	SStringT szUsage = GETSTRING(R.string.usages);
	SStringT szHelp  = GETSTRING(R.string.usage_help);
	SMessageBox(NULL, szUsage, szHelp, MB_OK | MB_ICONINFORMATION);

	return FALSE;
}

DWORD C_CommandLine::FindImages(const SStringT& szFindPath, const SStringT& szFilter, VectImage& vectImage)
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

SStringT C_CommandLine::GetParamValue(LPWSTR pszArg, LPWSTR pszNext, int& i32Index)
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

BOOL C_CommandLine::IsOption(LPWSTR pszArg, LPWSTR pszName)
{
	if ( pszArg && pszName && (pszArg[0] == _T('-') || pszArg[0] == _T('/')) ) 
		return (_tcsnicmp(&pszArg[1], pszName, _tcslen(pszName)) == 0);

	return FALSE;
}

BOOL C_CommandLine::ParseCommandLine()
{
	BOOL bRet   = FALSE;
	int nArgs = 0;;
	if (LPWSTR* pszArgs = CommandLineToArgvW(GetCommandLine(), &nArgs))
	{
		if ( nArgs > 1 )
			bRet = Parse(pszArgs, nArgs);

		LocalFree(pszArgs);
	}

	return bRet;
}

BOOL C_CommandLine::Parse(LPWSTR* pszArgs, int nArgs)
{
	int i32Index = 0;
	for ( ; i32Index < nArgs; i32Index++ )
	{
		LPWSTR pszArg = pszArgs[i32Index];
		LPWSTR pszNext=(i32Index + 1 < nArgs) ? pszArgs[i32Index + 1] : NULL;
		if ( IsOption(pszArg, _T("images")) )
		{
			SStringT szImgPath = _T("${ipath}");	// 不能使用格式: $(ipath), 因为它会被VS替换成空的。
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.IsEmpty() )
				return FALSE;

			for (LPWSTR pszToken = _tcstok((TCHAR*)(const TCHAR*)szValue, _T("|")); pszToken; pszToken = _tcstok(NULL, _T("|")) )
			{
				SStringT szImage = pszToken;
				if (szImage.Find(szImgPath) >= 0 )	// 有"${ipath}"
					szImage.Replace(szImgPath, m_szImgPath);
				else if (szImage.Left(2) == _T(".\\") || szImage.Left(2) == _T("./"))	// 是要对路径".\" 或者 "./"
					szImage = m_szImgPath + szImage.Right(szImage.GetLength() - 2);	
				else if (!(szImage.Find(_T('\\')) > 0 || szImage.Find(_T('/')) > 0))	// 只是文件名，没有路径
					szImage = m_szImgPath + szImage;
		
				m_vectImage.push_back(szImage);
			}
		}
		else if ( IsOption(pszArg, _T("ifilter")) )
		{
			m_szImgFilter = GetParamValue(pszArg, pszNext, i32Index);
			if ( m_szImgFilter.IsEmpty() || m_szImgPath.IsEmpty() )
				return FALSE;

			FindImages(m_szImgPath, m_szImgFilter, m_vectImage);
		}
		else if ( IsOption(pszArg, _T("ipath")) )
		{
			m_szImgPath = GetParamValue(pszArg, pszNext, i32Index);
			m_szImgPath.TrimRight(_T('\\'));
			m_szImgPath.TrimRight(_T('/'));
			m_szImgPath += _T("\\");
			if ( m_szImgPath.IsEmpty() || !PathFileExists(m_szImgPath) )
				return FALSE;
		}
		else if ( IsOption(pszArg, _T("db")) )
		{
			m_szDbFile = GetParamValue(pszArg, pszNext, i32Index);
			if ( m_szDbFile.IsEmpty() || !PathFileExists(m_szDbFile) )
				return FALSE;
		}
		else if ( IsOption(pszArg, _T("fid")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.IsEmpty() )
				return FALSE;

			m_u64Fid = _tcstoui64(szValue, NULL, 0);
		}
		else if ( IsOption(pszArg, _T("lang")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.CompareNoCase(_T("cn")) == 0 )
				m_szLang = szValue;
			else if ( szValue.CompareNoCase(_T("en")) == 0 )
				m_szLang = szValue;
			else 
				return FALSE;
		}
		else if ( IsOption(pszArg, _T("scale")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			m_nScale = _tcstol(szValue, NULL, 0);
			if ( m_nScale < 100 )
				m_nScale = 100;
			else if ( m_nScale > 300 )
				m_nScale = 300;
		}
		else if ( IsOption(pszArg, _T("h")) || IsOption(pszArg, _T("?")) )
		{
			return FALSE;
		}
	}

	return (m_vectImage.size() > 0 || !m_szDbFile.IsEmpty());
}
