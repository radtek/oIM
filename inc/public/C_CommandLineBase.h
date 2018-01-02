#pragma once
#include <Shellapi.h>

// 定义所支持的语言标识============================
const TCHAR* const kLang   = _T("lang_");	// 语言前缀
const TCHAR* const kLangCn = _T("cn");		// 简体中文
const TCHAR* const kLangTn = _T("tn");		// 繁体中文
const TCHAR* const kLangEn = _T("en");		// 英文
class C_CommandLineBase
{
public:
	C_CommandLineBase(void)
		: m_nScale(100)
		, m_bShowInTaskbar(TRUE)
		, m_szDbKey()	
	{
		// 获取用户当前语言
		LANGID lang = GetUserDefaultLangID(); 
		if ( lang == LANG_CHINESE_TRADITIONAL )
			SetLang(kLangTn);
		if ( LOBYTE(lang) == LANG_CHINESE )
			SetLang(kLangCn);
		else 
			SetLang(kLangEn);	// 默认配配置为英文

		STRACE(_T("lang:%s(%0x)"), m_szLang, lang);

		// 根据当前系统DPI，设置默认值
		HWND hWnd = ::GetDesktopWindow();
		HDC hdc   = ::GetWindowDC(hWnd);
		int nDpiX = ::GetDeviceCaps(hdc, LOGPIXELSX); // 启用高DPI功能
		:: ReleaseDC(hWnd, hdc);
		SetScale(nDpiX);
	}

	BOOL SetLang(const SStringT& szLang)
	{
		m_szLang = kLang;
		m_szLang+= szLang;

		return TRUE;
	}

	int SetScale(int nScale)
	{
		if ( nScale >= 288 )		// *3
			m_nScale = 300;
		else if ( nScale >= 240 )	// *2.5
			m_nScale = 250;
		else if ( nScale >= 192 )	// *2
			m_nScale = 200;
		else if ( nScale >= 144 )	// *1.5
			m_nScale = 150;
		else if (nScale >= 120 )	// *1.25
			m_nScale = 125;
		else						// *1
			m_nScale = 100;

		return m_nScale;
	}

	inline int GetParamValueInt(LPWSTR pszArg, LPWSTR pszNext, int& i32Index)
	{
		SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
		return _tcstol(szValue, NULL, 0);
	}

	inline SStringT GetParamValue(LPWSTR pszArg, LPWSTR pszNext, int& i32Index)
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

	inline BOOL	IsOption(LPWSTR pszArg, LPWSTR pszName)
	{
		if ( pszArg && pszName && (pszArg[0] == _T('-') || pszArg[0] == _T('/')) ) 
			return (_tcsnicmp(&pszArg[1], pszName, _tcslen(pszName)) == 0);

		return FALSE;
	}

	BOOL IsTaskbar()
	{
		return m_bShowInTaskbar; 
	}

	int GetScale()
	{
		return m_nScale;
	}

	const SStringT& GetLang() const 
	{
		return m_szLang; 
	}

	virtual BOOL Usage(HWND hParent)
	{
		SStringT szUsage = GETSTRING(R.string.usages);
		SStringT szHelp  = GETSTRING(R.string.usage_help);

		SMessageBox(hParent, szUsage, szHelp, MB_OK | MB_ICONINFORMATION);

		return FALSE;
	}

	virtual BOOL Parse(LPWSTR* pszArgs, int nArgs) = 0;
	virtual BOOL ParseCommandLine()
	{
		BOOL bRet   = FALSE;
		int nArgs = 0;;
		TCHAR* pszCmdLine = GetCommandLine();
		if (LPWSTR* pszArgs = CommandLineToArgvW(pszCmdLine, &nArgs))
		{
			STRACE(_T("%s"), pszCmdLine);
			if ( nArgs >= 1 )
				bRet = Parse(pszArgs, nArgs);

			LocalFree(pszArgs);
		}

		return bRet;
	}

protected:
	BOOL		m_bShowInTaskbar;	// 是否显示在任务栏
	int			m_nScale;			// 放大率：【100，125，150，200】
	SStringT	m_szLang;			// 语言：【R.string.lang_cn, R.string.lang_en】
	SStringT	m_szDbKey;			// 数据库密钥 _T("SXIT@0518$YFGZ#") 
};

