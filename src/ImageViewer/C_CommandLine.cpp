#include "StdAfx.h"
#include "C_CommandLine.h"
#include "Shellapi.h"
#include <Winerror.h>

C_CommandLine& C_CommandLine::GetObject()
{
	static C_CommandLine s_CommandLine;
	return s_CommandLine;
}

C_CommandLine::C_CommandLine(void)
	: m_nScale(100)
	, m_u64Fid(0)
	, m_bFindedFid(FALSE)
	, m_bShowInTaskbar(TRUE)
	, m_szDbKey(_T("SXIT@0518$YFGZ#"))
{
	// ��ȡ�û���ǰ����
	LANGID lang = GetUserDefaultLangID(); //GetSystemDefaultLangID();
	if ( LOBYTE(lang) == LANG_CHINESE )
		m_szLang = GETSTRING(R.string.lang_cn);
	else 
		m_szLang = GETSTRING(R.string.lang_en);
	
	STRACE(_T("default lang:%s(%0x)"), m_szLang, lang);

	// ���ݵ�ǰϵͳDPI������Ĭ��ֵ
	HWND hWnd = ::GetDesktopWindow();
	HDC hdc   = ::GetWindowDC(hWnd);
	int nDpiX = ::GetDeviceCaps(hdc, LOGPIXELSX); // ���ø�DPI����
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

int __cdecl C_CommandLine::CompareFunc(void *pvlocale, const void *item1, const void *item2)
{
	SStringT* pszItem1 = (SStringT*)item1;
	SStringT* pszItem2 = (SStringT*)item2;

	return pszItem1->CompareNoCase(*pszItem2);
}

DWORD C_CommandLine::FindImages(const SStringT& szFindPath, const SStringT& szFilter, VectImage& vectImage)
{
	WIN32_FIND_DATA ffd = { 0 };
	BOOL	bMultiFilter= (szFilter.Find(_T('|')) > 0 || szFilter.Find(_T(';')) > 0);	// �зָ������͵��ɶ��������
	HANDLE	hFind = FindFirstFile(szFindPath + (bMultiFilter ? _T("*.*") : szFilter), &ffd);
	if ( hFind == INVALID_HANDLE_VALUE )
		return 0;

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Is folder
			continue;

		if ( bMultiFilter == FALSE )
		{	// ���Ƕ��������ֱ����ӵ��б�
			vectImage.push_back(szFindPath + ffd.cFileName);
			continue;
		}

		// �Ƕ��������ֻ����չ�����й���
		if ( szFilter.Find(PathFindExtension(ffd.cFileName)) >= 0)
			vectImage.push_back(szFindPath + ffd.cFileName);
	}while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	STRACE(_T("Find '%s' result count: %d"), szFilter, vectImage.size());
	qsort_s(vectImage.data(), vectImage.size(), sizeof(SStringT), CompareFunc,(void*)&vectImage);

	return vectImage.size();
}

const pugi::char_t* const kMessage = _T("Message");
const pugi::char_t* const kMsgImage= _T("MsgImage");
const pugi::char_t* const kFid     = _T("Fid");
const pugi::char_t* const kFilePath= _T("FilePath");
const char* const kGetMsgInfo = "SELECT msgid_,sid,timestamp FROM t_session_records WHERE msg LIKE '%%Fid=\"%llu\"%%' AND msg LIKE '%%<MsgImage %%';";
const char* const kGetImgMsgs = "SELECT msgid_,msg FROM t_session_records WHERE sid=%lld AND timestamp>=%u AND timestamp<=%u AND msg LIKE '%%<MsgImage %%' ORDER BY timestamp DESC;";
BOOL C_CommandLine::GetMsgInfo(I_SQLite3* pIDb, const QFID& fid, QSID& sid, DWORD& dwTimestamp)
{
	if ( pIDb == NULL || fid == 0 )
		return FALSE;

	char* pszSql = pIDb->VMPrintf(kGetMsgInfo, fid);
	if ( I_SQLite3Table* pITbl = pIDb->GetTable(pszSql, true) )
	{
		AUTO_RELEASE_(pITbl);
		QMID mid = 0;
		if ( pITbl->Step() == SQLITE_ROW )
		{
			mid = pITbl->GetColInt64(0);
			sid = pITbl->GetColInt64(1);
			dwTimestamp = pITbl->GetColInt(2);

			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL C_CommandLine::ParseImgMsg(const char* const pszMsgUI, DWORD& dwNowIndex)
{
	if ( pszMsgUI == NULL )
		return FALSE;

	pugi::xml_document xmlDoc;
	if(!xmlDoc.load_buffer(pszMsgUI, strlen(pszMsgUI), pugi::parse_default, pugi::encoding_utf8)) 
		return FALSE;

	for (pugi::xml_node node = xmlDoc.first_child().child(kMessage).first_child(); node; node = node.next_sibling())
	{
		if ( _tcsicmp((const TCHAR*)node.name(), kMsgImage) != 0 )
			continue;	// ����ͼƬ

		if ( m_bFindedFid == FALSE )
		{
			QFID fid = _tcstoui64((const TCHAR*)node.attribute(kFid).as_string(), NULL, 0);
			if ( fid == m_u64Fid )
				m_bFindedFid = TRUE;	// �Ѿ���λ����ָ����ͼƬ
			else
				dwNowIndex++;
		}

		SStringT szImage = (const TCHAR*)node.attribute(kFilePath).as_string();
		m_vectImage.push_back(szImage);
	}

	return TRUE;
}

#define DAY_OF_SECONDS		(24 * 60 * 60)	// 24h * 60min * 60s
#define MONTH_OF_DAYS_(T,D)	((T) + DAY_OF_SECONDS * (D))
DWORD C_CommandLine::LoadImages()
{ // 24 * 60 * 60
	if ( m_u64Fid == 0 )	// û��FID�����޷�ȷ���Ự�����޷�����ͼƬ��Ϣ
		return 0;	

	QSID		sid = 0;
	DWORD		dwTime = 0;
	I_SQLite3*	pIDb = NULL;
	C_PluginDll	dllSqlite3(ePLUGIN_TYPE_NORMAL, INAME_SQLITE_DB);
	SStringA	szDbFileA = S_CT2A(m_szDbFile);
	SStringA	szDbKeyA  = S_CT2A(m_szDbKey);

	if ( dllSqlite3.Load(_T("SQLite3.dll")) && 
		SUCCEEDED(dllSqlite3.eIMCreateInterface(INAME_SQLITE_DB, (void**)&pIDb)) )
	{
		AUTO_RELEASE_(pIDb);
		if ( SQLITE_OK != pIDb->Open(szDbFileA) )
			return 0;

		if ( !szDbKeyA.IsEmpty() )
			pIDb->Key(szDbKeyA, szDbKeyA.GetLength());

		if ( !GetMsgInfo(pIDb, m_u64Fid, sid, dwTime) )
			return 0;	// δ�鵽�Ự

		// ��ȡǰһ���£��������µ�ͼƬ��Ϣ
		char* pszSql = pIDb->VMPrintf(kGetImgMsgs, sid,	MONTH_OF_DAYS_(dwTime, -31), MONTH_OF_DAYS_(dwTime, 62) );
		DWORD dwIndex = m_vectImage.size();	// �Ե�ǰ�Ѿ��еĸ���Ϊ��������
		if ( I_SQLite3Table* pITbl = pIDb->GetTable(pszSql, true) )
		{
			AUTO_RELEASE_(pITbl);
			while(pITbl->Step() == SQLITE_ROW )
			{
				ParseImgMsg((const char*)pITbl->GetColText(1), dwIndex);
			}

			m_u64Fid = dwIndex;	// ����FIDΪʵ�ʵ�ͼƬ����
		}
	}

	return m_vectImage.size();
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

BOOL C_CommandLine::Parse(LPWSTR* pszArgs, int nArgs)
{
	int i32Index = 0;
	for ( ; i32Index < nArgs; i32Index++ )
	{
		LPWSTR pszArg = pszArgs[i32Index];
		LPWSTR pszNext=(i32Index + 1 < nArgs) ? pszArgs[i32Index + 1] : NULL;
		if ( IsOption(pszArg, _T("images")) )
		{
			SStringT szImgPath = _T("${ipath}");	// ����ʹ�ø�ʽ: $(ipath), ��Ϊ���ᱻVS�滻�ɿյġ�
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.IsEmpty() )
			{
				STRACE(_T("'-images' is empty value"));
				return FALSE;
			}

			for (LPWSTR pszToken = _tcstok((TCHAR*)(const TCHAR*)szValue, _T("|")); pszToken; pszToken = _tcstok(NULL, _T("|")) )
			{
				SStringT szImage = pszToken;
				if (szImage.Find(szImgPath) >= 0 )	// ��"${ipath}"
					szImage.Replace(szImgPath, m_szImgPath);
				else if (szImage.Left(2) == _T(".\\") || szImage.Left(2) == _T("./"))	// ��Ҫ��·��".\" ���� "./"
					szImage = m_szImgPath + szImage.Right(szImage.GetLength() - 2);	
				else if (!(szImage.Find(_T('\\')) > 0 || szImage.Find(_T('/')) > 0))	// ֻ���ļ�����û��·��
					szImage = m_szImgPath + szImage;
		
				m_vectImage.push_back(szImage);
			}
		}
		else if ( IsOption(pszArg, _T("ifilter")) )
		{
			m_szImgFilter = GetParamValue(pszArg, pszNext, i32Index);
			if ( m_szImgFilter.IsEmpty() || m_szImgPath.IsEmpty() )
			{
				STRACE(_T("'-ifilter' is empty value"));
				return FALSE;
			}

			FindImages(m_szImgPath, m_szImgFilter, m_vectImage);
		}
		else if ( IsOption(pszArg, _T("ipath")) )
		{
			m_szImgPath = GetParamValue(pszArg, pszNext, i32Index);
			m_szImgPath.TrimRight(_T('\\'));
			m_szImgPath.TrimRight(_T('/'));
			m_szImgPath += _T("\\");
			if ( m_szImgPath.IsEmpty() || !PathFileExists(m_szImgPath) )
			{
				STRACE(_T("'-ipath' is empty value"));
				return FALSE;
			}
		}
		else if ( IsOption(pszArg, _T("dbkey")) )
		{
			m_szDbKey = GetParamValue(pszArg, pszNext, i32Index);
		}
		else if ( IsOption(pszArg, _T("db")) )
		{
			m_szDbFile = GetParamValue(pszArg, pszNext, i32Index);
			if ( m_szDbFile.IsEmpty() || !PathFileExists(m_szDbFile) )
			{
				STRACE(_T("'-db' is empty value"));
				return FALSE;
			}

			LoadImages();
		}
		else if ( IsOption(pszArg, _T("fid")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.IsEmpty() )
			{
				STRACE(_T("'-fid' is empty value"));
				return FALSE;
			}

			m_u64Fid = _tcstoui64(szValue, NULL, 0);
		}
		else if ( IsOption(pszArg, _T("lang")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			if ( szValue.CompareNoCase(_T("cn")) == 0 )
				m_szLang = GETSTRING(R.string.lang_cn);
			else if ( szValue.CompareNoCase(_T("en")) == 0 )
				m_szLang = GETSTRING(R.string.lang_en);
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
		else if ( IsOption(pszArg, _T("tarskbar")) )
		{
			SStringT szValue = GetParamValue(pszArg, pszNext, i32Index);
			m_bShowInTaskbar = _tcstol(szValue, NULL, 0);
		}
		else if ( IsOption(pszArg, _T("h")) || IsOption(pszArg, _T("?")) )
		{
			return FALSE;
		}
	}

	return (m_vectImage.size() > 0 || !m_szDbFile.IsEmpty());
}
