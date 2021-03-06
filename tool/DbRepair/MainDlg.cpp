// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
//#include "eIMUICore\eIMDbConst.h"
#include "sqlite3\I_SQLite.h"
#include "des\des.h"
#include "md5\md5_storage.h"
#include "controls.extend\FileHelper.h"
#include "helper\SMenuEx.h"	

#define DB_KEY				("LSJT@0102$YFGZ#")
#define DB_KEY_LEN			(15)	
#define MAX_CORRUPT			(2000)
#define CORRUPT_STEP		(10)
#define MAX_CORRUPT_BEST	(20000)
#define CORRUPT_STEP_BEST	(1)

const char* const kSelUser		= "SELECT eid_, name FROM t_login_user ORDER BY TIMESTAMP DESC;";
const char* const kCheck		= "PRAGMA integrity_check;";
const char* const kCheckpoint	= "PRAGMA wal_checkpoint(FULL);";
const char* const kSelMaster	= "SELECT tbl_name,sql FROM sqlite_master WHERE sql NOT NULL AND type='table';";
const char* const kSelTableFmt	= "SELECT rowid,* FROM %s WHERE rowid >= %lld ORDER BY rowid;";
const char* const kSelLastRowid = "SELECT rowid FROM %s ORDER BY rowid DESC LIMIT 1;";
const char* const kResetVersion = "PRAGMA user_version=0;";

const TCHAR* const kCmdDES   = _T("des");
const TCHAR* const kCmdMD5   = _T("md5");
const TCHAR* const kCmdHelp0 = _T("?");
const TCHAR* const kCmdHelp1 = _T("？");
const TCHAR* const kCmdHelp2 = _T("help");
const TCHAR* const kCmdHelp3 = _T(".help");
const TCHAR* const kCmdCls0  = _T("cls");
const TCHAR* const kCmdCls1  = _T("clear");
const TCHAR* const kCmdCls2  = _T("clean");
const TCHAR* const kCmdDate  = _T("date");
const TCHAR* const kCmdHex   = _T("hex");
const TCHAR* const kCmdDec   = _T("dec");
const TCHAR* const kCmdRtx   = _T("rtx");

#define USERS_DB_FILE			_T("Users.db")				// [1]Users db file, global, $(eIMData)
#define CONTACTS_DB_FILE		_T("Contacts.db")			// [1]Contacts db file, user, $(LoginId)
#define MSG_DB_FILE				_T("msg.db")				// [N]Msg db file, user, $(LoginId) 
#define MSG_DB_AS_NAME			"msg_"						// Attach as name

extern "C" __declspec(dllexport) int __stdcall CreateInterface(const TCHAR* pctszIID, void** ppvIObject);


#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif
	
class CAboutDlg : public SHostDialog
{
public:
	CAboutDlg():SHostDialog(_T("LAYOUT:XML_ABOUTWND"))	{}
	~CAboutDlg(void){}

	BOOL OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		if ( SStatic* pText = FindChildByName2<SStatic>(R.name.txt_version) )
		{
			SStringT szAboutVer = GETSTRING(R.string.aboutVersion);
			SStringT szVer = GETSTRING(R.string.ver);

			szAboutVer = tr(szAboutVer);
			szAboutVer.Replace(_T("%ver%"), szVer);
			pText->SetWindowText(szAboutVer);
		}

		return TRUE;
	}

protected:
	//soui消息
	EVENT_MAP_BEGIN()
	EVENT_MAP_END()
		//消息映射表
	BEGIN_MSG_MAP_EX(CAboutDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostDialog)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
};

CMainDlg::CMainDlg()
	: SHostWnd(_T("LAYOUT:XML_MAINWND"))
	, m_szLog(_T(""))
	, m_bRepairing(FALSE)
	, m_bEncrypted(TRUE)
	, m_i32MaxCorrupt(MAX_CORRUPT)
	, m_i32CorruptStep(CORRUPT_STEP)
	, m_bSQL(FALSE)
	, m_bStopSQL(FALSE)
	, m_pISqlDb(NULL)
	, m_hThreadSql(NULL)
	, m_pbtnSQL(NULL)
	, m_prdtLog(NULL)
	, m_pcmbAccount(NULL)
	, m_pcmbSQL(NULL)
	, m_i32LangId(R.id.menuChinese)
	, m_bNeedFullscreen(FALSE)
{
	m_bLayoutInited = FALSE;
	ZERO_ARRAY_(m_aszHead);
	ZERO_ARRAY_(m_aszDbFile);
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	::DragAcceptFiles(m_hWnd, TRUE);
	::RegisterDragDrop(m_hWnd, GetDropTarget());

	m_bLayoutInited = TRUE;

	m_pbtnSQL = FindChildByName2<SButton>(R.name.btn_sql);
	m_prdtLog = FindChildByName2<SRichEdit>(R.name.rdt_Log);
	m_pcmbSQL = FindChildByName2<SComboBox>(R.name.cmb_sql);
	m_pcmbAccount = FindChildByName2<SComboBox>(R.name.cmb_db);

	Init();

	HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO minfo = {sizeof(MONITORINFO), 0};
	CRect rtWnd    = GetWindowRect();
	CRect rtPos(0, 0, (LONG)(rtWnd.Width() * args.GetScale()/100.f), (LONG)(rtWnd.Height() * args.GetScale()/100.f) );
	::GetMonitorInfo(hMonitor, &minfo);

	if ( rtPos.Width() > minfo.rcWork.right - minfo.rcWork.left &&
		rtPos.Height() > minfo.rcWork.bottom - minfo.rcWork.top )
		m_bNeedFullscreen = TRUE;

	if ( rtPos.left < minfo.rcWork.left )
		rtPos.left = minfo.rcWork.left;

	if ( rtPos.right > minfo.rcWork.right )
		rtPos.right = minfo.rcWork.right;	// 不能超过屏幕宽度

		if ( rtPos.top < minfo.rcWork.top ) 
		rtPos.top = minfo.rcWork.top;

	if ( rtPos.bottom > minfo.rcWork.bottom )
		rtPos.bottom = minfo.rcWork.bottom;	// 不能超过屏幕的高度

	SDispatchMessage(UM_SETSCALE, args.GetScale(), 0);	
	MoveWindow(&rtPos);				// 调整窗口大小【主程序会再局中】

	return 0;
}

BOOL CMainDlg::IsOutofScreen()
{
	return m_bNeedFullscreen;
}

LRESULT CMainDlg::OnDbRepairLog(UINT uMsg,WPARAM wp,LPARAM lp,BOOL & bHandled)
{
	SetMsgHandled(FALSE);
	if ( PS_LogMsg_ psMsg = (PS_LogMsg_)wp )
	{
		m_szLog += S_CA2T(psMsg->szMsg);
		m_prdtLog->SetWindowText(m_szLog);
		INT i32MaxPos = 0;
		if (m_prdtLog->HasScrollBar(TRUE) &&
			m_prdtLog->GetScrollRange(TRUE, NULL, &i32MaxPos) )
		{
			m_prdtLog->SetScrollPos(TRUE, i32MaxPos, TRUE);
		}
		return TRUE;
	}
	return S_OK;
}

LRESULT CMainDlg::OnUpdateUI(UINT uMsg,WPARAM wp,LPARAM lp,BOOL & bHandled)
{
	SetMsgHandled(FALSE);
	switch ( wp )
	{
	case 0:
		UpdateUI(lp);
		break;
	case R.id.btn_sql:
		m_pbtnSQL->SetWindowText(LoadString(lp));
		break;
	case R.id.cmb_sql:
		if ( lp )
			m_pcmbSQL->InsertItem(m_pcmbSQL->GetCount(), (LPCTSTR)lp, 0, 0);
		else
			((SWindow*)m_pcmbSQL)->SetWindowText(NULL);
		break;
	case R.id.rdt_Log:
		m_szLog.Empty();
		m_prdtLog->SetWindowText(m_szLog);
		break;
	}

	return S_OK;
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	SAFE_RELEASE_INTERFACE_(m_pISqlDb);
	m_Log.Uninit();

	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}

void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}

void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(R.name.btn_max);
	SWindow *pBtnRestore = FindChildByName(R.name.btn_restore);
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void __stdcall CMainDlg::LogCallback(PS_LogMsg_ psMsg, void* pvUserData)
{
	if ( CMainDlg* pDlg = static_cast<CMainDlg*>(pvUserData) )
	{
		if (psMsg && pDlg->m_prdtLog)
			pDlg->SendMessage(WM_DBREPAIR_LOG, (WPARAM)psMsg, NULL);
	}
}

void CMainDlg::OnRepairClick()
{
	StartRepair();
}

void CMainDlg::OnOpenDbClick()
{
	CFileDialogEx dlgFile(
		TRUE,
		L"*.db",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, 
		_T("eIM Database Files (*.db)|*.db|All files(*.*)|*.*||") );

	if ( IDOK != dlgFile.DoModal() )
		return;
	
	if ( TCHAR* pos = dlgFile.GetStartPosition() )
	{
		S_AccountInfo sAInfo = { 0, };
		do
		{
			sAInfo.szPath = dlgFile.GetNextPathName(pos);
			AddDb(sAInfo);
		}while(pos);
	}
}

void CMainDlg::OnOpenFolderClick()
{
	S_AccountInfo sAInfo;

	if(GetAccountInfo(sAInfo))
		OpenFolder(sAInfo.szPath);
}

void CMainDlg::OnSqlClick()
{
	DWORD dwExitCode = 0;
	if ( m_hThreadSql && GetExitCodeThread(m_hThreadSql, &dwExitCode) )
	{
		if ( dwExitCode == STILL_ACTIVE )
		{
			m_bStopSQL = TRUE;
			return;
		}

		SAFE_CLOSE_HANDLE_(m_hThreadSql);
	}

	m_szSql = m_pcmbSQL->GetWindowText();
	m_pbtnSQL->SetWindowText(LoadString(R.string.execStop));
	m_hThreadSql = (HANDLE)_beginthreadex( NULL, 0, _SqlThread,  this, 0, NULL );
	if (m_hThreadSql == NULL)
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.startSqlThreadFailed), GetLastError());
}

SStringT CMainDlg::LoadString(DWORD dwNamedId)
{
	SStringT szRet = GETSTRING(dwNamedId);
	return tr(szRet);
}

BOOL CMainDlg::StartRepair(BOOL bBest)
{
	ZERO_ARRAY_(m_aszDbFile);
	S_AccountInfo sAInfo;
	if(GetAccountInfo(sAInfo))
	{
		SStringT szLoad;
		m_i32MaxCorrupt  = bBest ? MAX_CORRUPT_BEST : MAX_CORRUPT;
		m_i32CorruptStep = bBest ? CORRUPT_STEP_BEST : CORRUPT_STEP;

		LOGGER_INFO0_(&m_Log, _T("============================== %s[%u] ======================================"), sAInfo.szAccount, sAInfo.dwQeid);
		LOGGER_INFO0_(&m_Log, _T("%s"), bBest ? LoadString(R.string.modeBest) : LoadString(R.string.modeFast));
		LOGGER_INFO0_(&m_Log, LoadString(R.string.startRepair), sAInfo.szPath);

		if ( PathIsDirectory(sAInfo.szPath) )
		{	// 文件夹，再枚举数据库文件
			PathCombine(m_aszDbFile[0], sAInfo.szPath, CONTACTS_DB_FILE);
			PathCombine(m_aszDbFile[1], sAInfo.szPath, MSG_DB_FILE);
		}
		else
		{	// 数据库文件，直接走修复流程
			_tcsncpy(m_aszDbFile[0], sAInfo.szPath, MAX_PATH);
		}

		if ( uintptr_t h = _beginthreadex( NULL, 0, _RepairThread,  this, 0, NULL ))
		{
			SAFE_CLOSE_HANDLE_(h);
		}
		else
		{
			LOGGER_ERROR0_(&m_Log, LoadString(R.string.startFailed), GetLastError());
		}
	}

	return TRUE;
}

BOOL CMainDlg::GetAccountInfo(S_AccountInfo& sAInfo)
{
	int i32Sel = m_pcmbAccount->GetCurSel();
	if ( i32Sel < 0 )
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.notFindDb));
		return FALSE;
	}

	sAInfo = m_vectAccountInfo[i32Sel];
	return TRUE;
}

void CMainDlg::OpenFolder(LPCTSTR lpszFile)
{
	if ( !PathFileExists(lpszFile) )
	{
		TCHAR szPath[MAX_PATH] = { 0 };

		_tcsncpy(szPath, lpszFile, MAX_PATH);
		PathRemoveFileSpec(szPath);
		::ShellExecute(NULL, NULL, _T("explorer.exe"), szPath, NULL, SW_SHOW);
		LOGGER_INFO0_(&m_Log, LoadString(R.string.openFolder), szPath);
	}
	else
	{
		SStringT szCmd;

		if ( PathIsDirectory(lpszFile) )
			szCmd = lpszFile;
		else
			szCmd.Format(_T("/select,\"%s\""), lpszFile);

		::ShellExecute(NULL, NULL, _T("explorer.exe"), szCmd, NULL, SW_SHOW);
		LOGGER_INFO0_(&m_Log, LoadString(R.string.openFolder), lpszFile);
	}
}


BOOL CMainDlg::AddDb(const S_AccountInfo& sAInfo)
{
	int	i32Index  = m_pcmbAccount->FindString(sAInfo.szPath);
	SAFE_RELEASE_INTERFACE_(m_pISqlDb);

	if ( i32Index < 0 )
	{
		m_vectAccountInfo.push_back(sAInfo);
		if ( sAInfo.szAccount.IsEmpty() )
			i32Index = m_pcmbAccount->InsertItem(m_pcmbAccount->GetCount(), sAInfo.szPath, 0, 0);
		else
			i32Index = m_pcmbAccount->InsertItem(m_pcmbAccount->GetCount(), sAInfo.szAccount + _T(": ") + sAInfo.szPath, 0, 0);
	}

	m_pcmbAccount->SetCurSel(i32Index);	
	return TRUE;
}

BOOL CMainDlg::Init()
{
	// 初始化日志
	SStringT szLogFile = oIMGetPath(PATH_TOKEN_APPDATA_FILE_(PATH_TYPE_APPDATA_LOG _T("\\DbRepair.log")));
	const int nFlag = LOG_FLAG_OUT_FILE | LOG_FLAG_APPEND | LOG_FLAG_HEAD_LEVEL | LOG_FLAG_OUT_DEBUGVIEW;
	m_Log.Init(szLogFile, LOG_LEVEL_DEFAULT, nFlag, LogCallback, this);
	
	LOGGER_INFO0_(&m_Log, LoadString(R.string.logFile), szLogFile);
	LOGGER_INFO0_(&m_Log, LoadString(R.string.notice));

	S_AccountInfo sAInfo;
	if ( args.InitDb(sAInfo) )
	{	// Command Line Format: -db=dppath [-account=account] [-eid=qeid]
		AddDb(sAInfo);
	}
	else
	{
		InitDb(oIMGetPath(PATH_TOKEN_APPDATA));
	}

	return FALSE;
}


int CMainDlg::InitDb(LPCTSTR lpszPathData)
{
	CHECK_NULL_RET_(lpszPathData, 0);

	SStringT szUserDb;
	CAutoRefPtr<I_SQLite3> pIDb;

	szUserDb.Format(_T("%s%s"), lpszPathData, USERS_DB_FILE);
	if ( OpenDb(szUserDb, pIDb) )
	{
		if (I_SQLite3Table* pITable = pIDb->GetTable(kSelUser))
		{
			S_AccountInfo sAInfo;
			while( pITable->Step() == SQLITE_ROW )
			{	
				sAInfo.dwQeid    = (DWORD)pITable->GetColInt(0);	// qeid
				sAInfo.szAccount = S_CA2T((const char*)pITable->GetColText(1));
				sAInfo.szPath.Format(_T("%s%u\\"), lpszPathData, sAInfo.dwQeid);

				// 需要同时存在 contacts.db 和 msg.db
				if ( PathFileExists(sAInfo.szPath + CONTACTS_DB_FILE) && 
					PathFileExists(sAInfo.szPath + MSG_DB_FILE) )
					AddDb(sAInfo);
			}

			pITable->Release();
			return TRUE;
		}
		else
		{
			LOGGER_ERROR0_(&m_Log, LoadString(R.string.checkUserDbFailed), szUserDb);
		}
	}
	else
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.openUserDbFailed), szUserDb);
	}

	return FALSE;
}


// 检查数据库完整性检查
// 返回值:
// 1: 完整
// 0: 不完整
//-1: 无法访问（数据库主表有问题）
int CMainDlg::Check(LPCTSTR lpszDbFile)
{
	CAutoRefPtr<I_SQLite3>	pIDb;

	if ( !OpenDb(lpszDbFile, pIDb) )
		return 0;

	if (I_SQLite3Table* pITable = pIDb->GetTable(kCheck))
	{
		if ( SQLITE_ROW == pITable->Step() )
		{
			SStringT szValue = S_CA2T((const char*)pITable->GetColText(0));
			if ( szValue == _T("ok") )
			{
				LOGGER_INFO0_(&m_Log, LoadString(R.string.checkOk), lpszDbFile, szValue);
				pITable->Release();
				return 1;
			}
			else
			{
				LOGGER_ERROR0_(&m_Log, LoadString(R.string.checkFailed), lpszDbFile, szValue);
			}
		}

		pITable->Release();
	}
	else
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.checkFailed2), lpszDbFile);
		return -1;
	}

	SStringT szFile = lpszDbFile;
	if ( ::PathFileExists(szFile + "-wal") || ::PathFileExists(szFile + "-shm") )
		pIDb->SQL(kCheckpoint);	// 使WAL文件，写入到库中。

	pIDb->Close();

	return 0;
}

// 备份有问题的数据库
BOOL CMainDlg::BackupBadDbFile(const TCHAR* pszDbFile, SStringT& szBkFile)
{
	szBkFile.Format(_T("%s.bk(%u)"), pszDbFile, (DWORD)time(NULL));
	if ( MoveFile(pszDbFile, szBkFile) )
	{
		LOGGER_INFO0_(&m_Log, LoadString(R.string.backupOk), pszDbFile, szBkFile);
		return TRUE;
	}
	else
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.backupFailed), pszDbFile, szBkFile);
		return FALSE;
	}
}

// 得到eIM实时同步备份的数据库路径
const TCHAR* CMainDlg::GetBackupDbFile(SStringT& szDbFile)
{
	int i32Index = szDbFile.ReverseFind(_T('\\'));
	if ( i32Index > 0 )
	{
		szDbFile.Insert(i32Index, _T("\\~1"));
		if ( ::PathFileExists(szDbFile) )
			return szDbFile;
	}

	return NULL;
}

// 文件是否已经被打开
BOOL CMainDlg::IsFileOccupied(const TCHAR* pszFile, BOOL bPrompt)
{
	CHECK_NULL_RET_(pszFile, FALSE);
	HANDLE hFile = CreateFile(pszFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		if ( bPrompt )
		{
			SStringT szText;

			szText.Format(LoadString(R.string.dbOpened), pszFile);
			SMessageBox(m_hWnd, szText, LoadString(R.string.prompt), MB_OK | MB_ICONINFORMATION);
		}

		return TRUE;
	}

	SAFE_CLOSE_HANDLE_(hFile);
	return FALSE;
}

// 打开数据库
BOOL CMainDlg::OpenDb(LPCTSTR lpszDbFile, CAutoRefPtr<I_SQLite3>& pIDb)
{
	I_SQLite3* pIDb_ = NULL;

	if (SUCCEEDED(CreateInterface(INAME_SQLITE_DB, (void**)&pIDb_)) )
	{
		SStringA szDbFileA = S_CT2A(lpszDbFile);
		if ( SQLITE_OK == pIDb_->Open(szDbFileA) )
		{
			pIDb = pIDb_;
			if ( m_bEncrypted )
				pIDb_->Key(DB_KEY, DB_KEY_LEN);

			return TRUE;
		}
		else
		{
			SAFE_RELEASE_INTERFACE_(pIDb_);
		}
	}

	return FALSE;
}

// 导入数据库内容
BOOL CMainDlg::DumpDb(LPCTSTR lpszFrom, LPCTSTR lpszTo)
{
	CAutoRefPtr<I_SQLite3> pIDbFrom;
	CAutoRefPtr<I_SQLite3> pIDbTo;

	if ( !OpenDb(lpszFrom, pIDbFrom) || !OpenDb(lpszTo, pIDbTo) )
	{
		SAFE_RELEASE_INTERFACE_(pIDbFrom);
		SAFE_RELEASE_INTERFACE_(pIDbTo);
		return FALSE;
	}

	BOOL bRet = TRUE;
	pIDbTo->SQL(kResetVersion);	// 需要重置版本号，才能让数据库再次进行初始化（因为有可能某些表完全没有了）
	if (I_SQLite3Table* pITable = pIDbFrom->GetTable(kSelMaster))
	{
		while( pITable->Step() == SQLITE_ROW)
		{
			int	  i32Ret   = 1;
			int   i32Corrupts = 0;
			int   i32CorruptN = 0;
			BOOL  bCorrupt = FALSE;
			INT64 i64Rowid      = 0;
			const char* pszTable=(const char*)pITable->GetColText(0);
			const char* pszSql  = pIDbFrom->VMPrintf(kSelTableFmt, pszTable, i64Rowid);
			INT64 i64LastRowid  = GetTableLastRowid(pIDbFrom, pszTable);
			SStringA szCreateTableSqlA = (const char*)pITable->GetColText(1);
			SStringA szBindSqlA;

			szCreateTableSqlA.Replace(szCreateTableSqlA.Left(14), "CREATE TABLE IF NOT EXISTS ");
			pIDbTo->SQL(szCreateTableSqlA);	// 创建表
			if ( I_SQLite3Table* pISrcTbl = pIDbFrom->GetTable(pszSql, true) )
			{
				LOGGER_INFO0_(&m_Log, LoadString(R.string.startTableRepair), pszTable);
				GetBindSql(pISrcTbl, pszTable, szBindSqlA);
				i32Ret = pISrcTbl->Step();
				if ( i32Ret == SQLITE_CORRUPT || DumpTable(pISrcTbl, pIDbTo, szBindSqlA, i64Rowid) < 0 )
				{	// SQLITE_CORRUPT
					if ( i32Ret == SQLITE_CORRUPT )
					{
						i32Corrupts++;
						STRACE(_T("Rowid:%lld, %s(%u)"), i64Rowid, pIDbTo->GetErrorMsg(i32Ret), i32Ret);
						i64Rowid += m_i32CorruptStep;
					}

					do
					{
						if ( i64LastRowid > 0 && i64Rowid >= i64LastRowid )
							break;

						pszSql = pIDbFrom->VMPrintf(kSelTableFmt, pszTable, i64Rowid );
						if ( I_SQLite3Table* pISrcTbl2 = pIDbFrom->GetTable(pszSql, true) )
						{
							i32Ret = pISrcTbl2->Step();
							if ( i32Ret == SQLITE_CORRUPT )
							{
								i32CorruptN++;
								STRACE(_T("Rowid:%lld, i32CorruptN:%d, %s(%u)"), i64Rowid, i32CorruptN, pIDbTo->GetErrorMsg(i32Ret), i32Ret);
								if ( i64LastRowid > 0 )
									i64Rowid += m_i32CorruptStep;
								else
								{
									i64Rowid += -i64LastRowid;
									if ( i32CorruptN > m_i32MaxCorrupt )
										break;	
								}
								
								continue;
							}

							if ( DumpTable(pISrcTbl2, pIDbTo, szBindSqlA, i64Rowid) < 0 )
							{
								i32CorruptN = 1;
								i32Corrupts++;
								continue;
							}

							pISrcTbl2->Release();
							break;
						}
					}while(true);
				}

				if ( i32Corrupts > 0 )
					LOGGER_INFO0_(&m_Log, LoadString(R.string.finishedTableRepair), pszTable, i32Corrupts);
				else
					LOGGER_INFO0_(&m_Log, LoadString(R.string.finishedTableRepair2), pszTable);

				pISrcTbl->Release();
			}
			else
			{
				LOGGER_ERROR0_(&m_Log, LoadString(R.string.tableRepairFailed), pszTable);
			}
		}

		pIDbTo->SQL(kCheckpoint);	// 使WAL文件，写入到库中。
		pITable->Release();
	}
	else
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.repairFailed2), lpszTo);	// To 的名字才是原始的数据库， From是生成 的备份
		return FALSE;
	}

	return bRet;
}

INT64 CMainDlg::GetTableLastRowid(I_SQLite3* pIDb, const char* pszTable)
{
	CHECK_NULL_RET_(pIDb, 0);
	CHECK_NULL_RET_(pszTable, 0);

	INT64 iRowid = 0;
	const char* pszSql = pIDb->VMPrintf(kSelLastRowid, pszTable);
	if (I_SQLite3Table* pITbl = pIDb->GetTable(pszSql, true) )
	{
		if ( pITbl->Step() == SQLITE_ROW )
			iRowid = pITbl->GetColInt64(0);
		else
			iRowid = -m_i32CorruptStep;

		pITbl->Release();
	}
	
	return 0;
}

BOOL CMainDlg::GetBindSql(I_SQLite3Table* pISrcTbl, const char* pszDstTable, SStringA& szSqlA)
{
	CHECK_NULL_RET_(pszDstTable, FALSE);
	CHECK_NULL_RET_(pISrcTbl, FALSE);
	memset(m_aszHead, 0, sizeof(m_aszHead));
	if (int i32Cols	= pISrcTbl->GetCols())
	{
		int  i32Pos = 0;
		char aaszHead[100][100] = { 0 };
		szSqlA = "REPLACE INTO ";
		szSqlA+= pszDstTable;
		szSqlA+= "(";

		// Make-up columns name
		for ( int i32Col = 1; i32Col < i32Cols; i32Col++ )
		{
			szSqlA+= pISrcTbl->GetColName(i32Col);
			szSqlA+=",";
		}

		i32Pos = szSqlA.GetLength();
		szSqlA.SetAt(i32Pos - 1, ' ');	// trimright ','
		szSqlA+=") VALUES (";

		// Make-up bind columns name
		for ( int i32Col = 1; i32Col < i32Cols; i32Col++ )
		{
			szSqlA+= "@";
			szSqlA+= pISrcTbl->GetColName(i32Col);
			szSqlA+=",";

			sprintf(m_aszHead[i32Col], "@%s", pISrcTbl->GetColName(i32Col));
		}

		i32Pos = szSqlA.GetLength();
		szSqlA.SetAt(i32Pos - 1, ' ');	// trimright ','
		szSqlA+=");";

		return TRUE;
	}

	return FALSE;
}

// 导入表数据
int CMainDlg::DumpTable(I_SQLite3Table* pISrcTbl, I_SQLite3* pIDstDb, const char* SqlA, INT64& i64Rowid)
{
	CHECK_NULL_RET_(pISrcTbl, 0);
	CHECK_NULL_RET_(pIDstDb, 0);
	CHECK_NULL_RET_(SqlA, 0);

	int iRet = SQLITE_ROW;
	int i32Cols	= pISrcTbl->GetCols();
	I_SQLite3Stmt* pIStmt = pIDstDb->Compile(SqlA);
	CHECK_NULL_RET_(pIStmt, 0);

	pIDstDb->Begin();
	do
	{ 
		if ( iRet == SQLITE_CORRUPT )
		{
			STRACE(_T("Rowid:%lld, %s(%u)"), i64Rowid, pIDstDb->GetErrorMsg(iRet), iRet);
			i64Rowid += CORRUPT_STEP;
			iRet = -1;
			break;
		}
		else if ( iRet == SQLITE_ROW )
		{
			i64Rowid = pISrcTbl->GetColInt64(0);
			for ( int i32Col = 1; i32Col < i32Cols; i32Col++ )
			{
				pIStmt->Bind(m_aszHead[i32Col], (char*)pISrcTbl->GetColText(i32Col));
			}

			pIStmt->Step();
		}
		else if ( iRet  == SQLITE_DONE )
		{
			iRet = 1;
			STRACE(_T("Rowid:%lld, done"), i64Rowid);
			break;
		}
		else
		{
			LOGGER_ERROR0_(&m_Log, _T("%s(%u)"), pIDstDb->GetErrorMsg(iRet), iRet);
			iRet = 0;
			break;
		}
		iRet = pISrcTbl->Step();
	}while(true);

	pIDstDb->Commit();
	pIStmt->Release();

	return iRet;
}

// 修复过程中，有几个考虑重点：
// 1、有完整备份时，就先直接使用完整备份，再同步增量数据
// 2、备份也有问题时，会直接将备份转为问题备份（eIM 启动后，会自动再进行完整备份）
// 3、没有备份，或者备份也有问题时，就直接进行修复流程
// 4、检查数据库文件是否已经被打开，需要在最后需要的时候才作检查（尽量减少用户麻烦）
// 5、当数据库确定有问题时，如果数据库已经被打开，就需要提示用户先关闭打开的程序（可能是eIM,也可能是数据库工具）
// 6、新库的创建，尽量从损坏的库来直接创建（sqlite_master)
//?7、当无法从有问题的库来进行创建时,就需要使用update.config来初始化
//?8、当无法找到update.config时，才提醒用户进行选择。
BOOL CMainDlg::Repair(LPCTSTR lpszDbFile)
{
	if ( lpszDbFile == NULL || lpszDbFile[0] == _T('\0') )
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.dbOpened));
		return FALSE;
	}

	if ( !::PathFileExists(lpszDbFile) )
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.dbNotExist), lpszDbFile);
		return FALSE;
	}

	LOGGER_INFO0_(&m_Log, LoadString(R.string.repairReady), 
		m_bEncrypted ? LoadString(R.string.dbEncrypted) : LoadString(R.string.dbNoEncrypted),
		lpszDbFile);
	
	int nResult = Check(lpszDbFile);
	if (nResult <= 0)
	{	// 数据库有问题，需要修复
		if ( IsFileOccupied(lpszDbFile, TRUE) )
			return FALSE;

		SStringT szBkDbFileBad;
		SStringT szBkDbFile = lpszDbFile;

		if ( !BackupBadDbFile(lpszDbFile, szBkDbFileBad) )
		{
			LOGGER_ERROR0_(&m_Log, LoadString(R.string.backupFailed2), lpszDbFile);
			return FALSE;
		}		

		AddBadDb(szBkDbFileBad);
		if ( GetBackupDbFile(szBkDbFile) )
		{	// 存在备份数据库
			if ( IsFileOccupied(szBkDbFile, TRUE) )
				return FALSE;

			if ( Check(szBkDbFile) > 0 )
			{	// 备份数据库完好
				if (CopyFile(szBkDbFile, lpszDbFile, TRUE) )
				{	// 使用备份数据库还原
					LOGGER_INFO0_(&m_Log, LoadString(R.string.useBackupOk), szBkDbFile, lpszDbFile);
					if ( nResult < 0 )
					{
						LOGGER_INFO0_(&m_Log, LoadString(R.string.syncFailed), szBkDbFileBad);
						LOGGER_INFO0_(&m_Log, LoadString(R.string.repairFinished), lpszDbFile);
						return TRUE;
					}

					LOGGER_INFO0_(&m_Log, LoadString(R.string.syncBegin), szBkDbFileBad, lpszDbFile);
					if ( DumpDb(szBkDbFileBad, lpszDbFile))	// 使用备份库还原成功，同步增量数据
					{
						LOGGER_INFO0_(&m_Log, LoadString(R.string.syncEnd), szBkDbFileBad, lpszDbFile);
						LOGGER_INFO0_(&m_Log,LoadString(R.string.repairFinished), lpszDbFile);
						return TRUE;
					}
					else
					{
						LOGGER_ERROR0_(&m_Log, LoadString(R.string.syncEndFailed), szBkDbFileBad, lpszDbFile);
						LOGGER_ERROR0_(&m_Log, LoadString(R.string.repairFailed), lpszDbFile);
						return FALSE;
					}
				}
			 }
			 else
				 BackupBadDbFile(szBkDbFile, szBkDbFileBad);	// 备份损坏的备份库，eIM 会再次自动创建
		}
		
		// 直接从损坏数据库进行修复
		LOGGER_INFO0_(&m_Log, LoadString(R.string.repairBegin), lpszDbFile);
		if ( DumpDb(szBkDbFileBad, lpszDbFile) )	// 从损坏数据库进行还原
		{
			LOGGER_INFO0_(&m_Log, LoadString(R.string.repairEnd), lpszDbFile);
			LOGGER_INFO0_(&m_Log, LoadString(R.string.repairFinished), lpszDbFile);
			return TRUE;
		}
		else
		{
			LOGGER_ERROR0_(&m_Log, LoadString(R.string.repairFailed), lpszDbFile);
			return FALSE;
		}
	}

	LOGGER_INFO0_(&m_Log, LoadString(R.string.checkOk2), lpszDbFile);
	return TRUE;
}

unsigned __stdcall CMainDlg::_RepairThread( void* pvData )
{
	CMainDlg* pDlg = static_cast<CMainDlg*>(pvData);

	try
	{
		 if ( pDlg )
			 return pDlg->DoRepair();

		 return 1;
	}
	catch(...)
	{
		if ( pDlg )
			pDlg->SendMessage(WM_UPDATE_UI, 0, TRUE);

		STRACE(_T("Thread throw abnormal"));
		return 1;
	}
}

unsigned CMainDlg::DoRepair()
{
	SendMessage(WM_UPDATE_UI, 0, FALSE);
	for ( int i32Index = 0; i32Index < COUNT_OF_ARRAY_(m_aszDbFile) && m_aszDbFile[i32Index][0]; i32Index++)
	{
		Repair(m_aszDbFile[i32Index]);
	}

	SendMessage(WM_UPDATE_UI, 0, TRUE);
	return 0;
}

BOOL CMainDlg::UpdateUI(BOOL bEnable)
{
	m_bRepairing = !bEnable;
	if ( SWindow* pBtn = FindChildByName(R.name.btn_db) )
		pBtn->EnableWindow(bEnable, TRUE);

	if ( SWindow* pBtn = FindChildByName(R.name.btn_open_folder) )
		pBtn->EnableWindow(bEnable, TRUE);

	if ( SWindow* pBtn = FindChildByName(R.name.btn_check_repair) )
		pBtn->EnableWindow(bEnable, TRUE);

	if ( m_pcmbAccount )
		m_pcmbAccount->EnableWindow(bEnable, TRUE);

	return TRUE;
}

void CMainDlg::OnDropFiles(HDROP hDropInfo)
{
    WORD    wCount = DragQueryFile(hDropInfo, -1, NULL, 0);  
	TCHAR	szFile[MAX_PATH];
	S_AccountInfo sAInfo = { 0, };

	for ( WORD wIndex = 0; wIndex < wCount; wIndex++)
	{
       if ( DragQueryFile(hDropInfo, wIndex, szFile, MAX_PATH) )
	   {
		   if ( PathIsDirectory(szFile) )
		   {
			   PathAddBackslash(szFile);
			   sAInfo.szPath = szFile;

			   if ( PathFileExists(sAInfo.szPath + CONTACTS_DB_FILE) || 
				    PathFileExists(sAInfo.szPath + MSG_DB_FILE) )
			   {
				   AddDb(sAInfo);
				   LOGGER_INFO0_(&m_Log, LoadString(R.string.addDbFolder), szFile);
			   }
			   else
			   {
				   LOGGER_ERROR0_(&m_Log, LoadString(R.string.notFound), sAInfo.szPath);
			   }
		   }
		   else
		   {
			   SHORT  u16State = GetKeyState(VK_CONTROL);	// 按住Ctrl 拖拽进来时，不检查文件后缀
			   TCHAR* pszFileExt = PathFindExtension(szFile);
			   if ( (pszFileExt && _tcsicmp(pszFileExt, _T(".db")) == 0) || u16State )
			   {
				   sAInfo.szPath = szFile;
				   AddDb(sAInfo);
				   LOGGER_INFO0_(&m_Log, LoadString(R.string.addDbFile), sAInfo.szPath);
			   }
			   else
			   {
				   LOGGER_ERROR0_(&m_Log, LoadString(R.string.notFound2), szFile);
			   }
		   }
	   }
	}

    DragFinish(hDropInfo); 
}

BOOL CMainDlg::AddBadDb(const TCHAR* pszBadDbFile)
{
	CHECK_NULL_RET_(pszBadDbFile, FALSE);

	VectBadDbIt it = find(m_vectBadDb.begin(), m_vectBadDb.end(), pszBadDbFile);
	if ( it != m_vectBadDb.end() )
		return TRUE;

	m_vectBadDb.push_back(pszBadDbFile);
	return TRUE;
}

unsigned __stdcall CMainDlg::_MakeZipThread( void* pvData )
{
	SStringT szText;
	S_AccountInfo sAInfo;
	CMainDlg* pThis = (CMainDlg*)pvData;
	CHECK_NULL_RET_(pThis, 0);

	if ( pThis->GetAccountInfo(sAInfo) )
	{
		SStringT szPath = sAInfo.szPath;	 
		if ( PathIsDirectory(sAInfo.szPath) )
			szPath += CONTACTS_DB_FILE;

		if ( PathFileExists(szPath) )
		{
			szText.Format(pThis->LoadString(R.string.makeZip),	sAInfo.szAccount, sAInfo.dwQeid, szPath);

			if ( IDOK == SMessageBox(pThis->m_hWnd, szText, pThis->LoadString(R.string.prompt), MB_OKCANCEL | MB_ICONWARNING) )
			{
				CAutoRefPtr<I_SQLite3> pIDb;
				SStringT   szParam;
				SStringT szPathExe   = oIMGetPath(PATH_TOKEN_APP); 
				SStringT szPathExeDb = oIMGetPath(PATH_TOKEN_APP_FILE_(CONTACTS_DB_FILE));

				if ( pThis->Check(szPath) != 1 )
					return 0;

				CopyFile(szPath, szPathExeDb, FALSE);
				if ( pThis->OpenDb(szPathExeDb, pIDb) )
				{
					LOGGER_INFO0_(&pThis->m_Log, pThis->LoadString(R.string.beginMakeZip));
					pIDb->SQL("DELETE FROM t_define_common_group;");
					pIDb->SQL("DELETE FROM t_define_group;");
					pIDb->SQL("DELETE FROM t_define_group_emps;");
					pIDb->SQL("DELETE FROM t_define_top_emp;");
					pIDb->SQL("DELETE FROM t_define_top_dept;");
					pIDb->SQL("DELETE FROM t_dept WHERE (flag & 524288);");
					pIDb->SQL("DELETE FROM t_dept_emp WHERE (flag & 524288);");
					pIDb->SQL("DELETE FROM t_emp WHERE (flag & 524288);");
					pIDb->SQL("DELETE FROM t_expanditem_info;");
					pIDb->SQL("DELETE FROM t_groupinfo_virtual;");
					pIDb->SQL("DELETE FROM t_groupmember_virtual;");
					pIDb->SQL("DELETE FROM t_robot_info;");
					pIDb->SQL("DELETE FROM t_session;");
					pIDb->SQL("DELETE FROM t_session_emp;");
					pIDb->SQL("DELETE FROM t_session_resign;");
					pIDb->SQL("DELETE FROM t_session_top;");
					pIDb->SQL("DELETE FROM t_subscribe_info;");
					pIDb->SQL("DELETE FROM t_user_purview;");
					pIDb->SQL("DELETE FROM t_update_contacts;");
					pIDb->SQL("DELETE FROM t_user_purview;");
					pIDb->SQL("REINDEX;");
					pIDb->SQL("VACUUM;");
					pIDb->SQL("PRAGMA user_version=0;");

					SAFE_RELEASE_INTERFACE_(pIDb);

					// 加密压缩
					szParam.Format(_T("-pwd150119yfgz a -mx=1 -tzip \"%scontactdbpc.zip\" \"%scontacts.db\""), szPathExe, szPathExe);
					ShellExecute(NULL, NULL, _T("7z.exe"), szParam, szPathExe, SW_HIDE);
					LOGGER_INFO0_(&pThis->m_Log, pThis->LoadString(R.string.makeZipOk), szPathExe);

					// 打开文件夹
					szParam.Format(_T("/select,\"%scontactdbpc.zip\""), szPathExe);
					ShellExecute(NULL, NULL, _T("explorer.exe"), szParam, NULL, SW_SHOW);
				}
				else
				{
					LOGGER_ERROR0_(&pThis->m_Log, pThis->LoadString(R.string.openUserDbFailed), szPath);
				}
			}
		}
		else
		{
			LOGGER_ERROR0_(&pThis->m_Log, pThis->LoadString(R.string.noExistDb), szPath);
		}
	}

	return 0;
}

unsigned __stdcall CMainDlg::_SqlThread( void* pvData )
{
	if ( CMainDlg* pThis = (CMainDlg*)pvData )
	{
		unsigned u32Ret = pThis->DoSql();
		pThis->SendMessage(WM_UPDATE_UI, R.id.btn_sql, R.string.sql);
		return u32Ret;
	}

	return 0;
}

unsigned CMainDlg::DoSql()
{
	if ( m_pISqlDb == NULL )
	{	// 打开当前数据库
		S_AccountInfo sAInfo;
		if ( !GetAccountInfo(sAInfo) )
			return 1;

		if ( PathIsDirectory(sAInfo.szPath) )
		{
			SStringT szPath = sAInfo.szPath + CONTACTS_DB_FILE;
			if ( !OpenDb(szPath, m_pISqlDb) )
			{
				LOGGER_ERROR0_(&m_Log, LoadString(R.string.openContactsDbFailed), szPath);
				return 2;
			}

			LOGGER_INFO0_(&m_Log, LoadString(R.string.openDbOk), szPath );
			szPath = sAInfo.szPath + MSG_DB_FILE;
			if ( PathFileExists(szPath) )
			{	// 存在消息库
				SStringA szPathA = S_CT2A(szPath);

				int i32Ret = m_pISqlDb->Attach(szPathA, MSG_DB_AS_NAME, DB_KEY);
				if ( i32Ret != SQLITE_OK )
					LOGGER_ERROR0_(&m_Log, LoadString(R.string.attachMsgDbFailed), szPath, i32Ret, m_pISqlDb->GetErrorMsg(i32Ret) );
				else
					LOGGER_INFO0_(&m_Log,  LoadString(R.string.attachMsgDbOk), szPath, _T(MSG_DB_AS_NAME) );
			}
		}
		else
		{
			SStringT szPath = sAInfo.szPath;
			if ( !OpenDb(szPath, m_pISqlDb) )
			{
				LOGGER_ERROR0_(&m_Log, LoadString(R.string.openContactsDbFailed), szPath);
				return 2;
			}

			LOGGER_INFO0_(&m_Log, LoadString(R.string.openDbOk), szPath );
		}
	}

	SStringA szSqlA;
	SStringT szSql = m_szSql;
	szSql.Trim();
	if ( ExecFunc(szSql) )
	{	// 执行成功时，就清除输入
		SendMessage(WM_UPDATE_UI, R.id.cmb_sql, 0);	// 因为在线程中，不能直接操作
		return 0;
	}

	if ( szSql.Right(1) != _T(";") )
		szSql += _T(";");

	szSqlA = S_CT2A(szSql);
	m_bFirstRow = TRUE;
	m_bStopSQL  = FALSE;
	m_i32Rows   = 0;
	LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
	
	int i32Ret = m_pISqlDb->SQL(szSqlA, false, sqlite3_callback, this);
	if ( i32Ret != SQLITE_OK )
	{
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.execSqlFailed), szSql, i32Ret, m_pISqlDb->GetErrorMsg(i32Ret) );
		return 3;
	}

	SendMessage(WM_UPDATE_UI, R.id.cmb_sql, 0);	// 因为在线程中，不能直接操作
	LOGGER_INFO0_(&m_Log, LoadString(R.string.rowCount), m_i32Rows );
	szSql.TrimRight(_T(';'));
	if ( CB_ERR == m_pcmbSQL->FindString(szSql) )
		SendMessage(WM_UPDATE_UI, R.id.cmb_sql, (LPARAM)(LPCTSTR)szSql);

	return 0;
}


inline BOOL CMainDlg::IsAsciiStr(char* pszValue)
{
	while(pszValue && *pszValue)
	{
		if ( !__isascii(*pszValue++) )
			return FALSE;
	}

	return TRUE;
}

int CMainDlg::sqlite3_callback(void* pUserData,int nCols,char** ppCols, char** ppColNames)
{
	CHECK_NULL_RET_(nCols, 1);
	CMainDlg* pThis = (CMainDlg*)pUserData;
	if ( pThis->m_bFirstRow )
	{
		pThis->m_bFirstRow = FALSE;
		SStringT szNames;
		for ( int i32Index = 0; i32Index < nCols; i32Index++ )
		{
			
			szNames += S_CA2T(ppColNames[i32Index]);
			szNames += _T("|");
		}

		szNames.TrimRight(_T('|'));
		LOGGER_INFO0_(&pThis->m_Log, _T("=========================================================================="));
		LOGGER_INFO0_(&pThis->m_Log, _T("%s"), szNames);
	}

	SStringT szValues;
	for ( int i32Index = 0; i32Index < nCols; i32Index++ )
	{
		if ( pThis->IsAsciiStr(ppCols[i32Index]) )
		{
			if (ppCols[i32Index] )
				szValues += S_CA2T(ppCols[i32Index]);
		}
		else
		{
			SStringT szValue = S_CA2T(ppCols[i32Index]);
			szValues += szValue;
		}
		szValues += _T("|");
	}

	szValues.TrimRight(_T('|'));
	LOGGER_INFO0_(&pThis->m_Log, _T("%s"), szValues);
	pThis->m_i32Rows++;

	return pThis->m_bStopSQL;
}

BOOL CMainDlg::DES(SStringA& szDataA, const char* pszKey)
{
	if (szDataA.GetLength() < 2 || szDataA.GetLength() > 64)
		return FALSE;

	if ( szDataA[0] == 'A' && szDataA[1] == 24 )
	{
		szDataA.TrimLeft('A');
		szDataA.TrimLeft(24);
	}

	CDes des;
	char czBuff[65] = {0};
	char aszPasswd[65] = {0};
	int  nLoop = 0;

	des.Setkey(pszKey);
	const char* pszData = szDataA;
	for (int i = 0; i< (int)szDataA.GetLength(); i += 2)
	{	// Hex char to int
		sscanf(&pszData[i],"%02x",&czBuff[i/2]);
		
	}

	while(nLoop < (int)szDataA.GetLength()/2)
	{
		des.Desrun(&aszPasswd[nLoop], &czBuff[nLoop], DES_DECRYPT);
		nLoop += 8;
	}

	szDataA = aszPasswd;

	return TRUE;
}

BOOL CMainDlg::MD5(SStringA& szDataA, SStringA& szMd5A)
{
	char szMd5[256 + 1] = { 0 };
	
	Create32Md5((unsigned char*)(const char*)szDataA, szDataA.GetLength(), (unsigned char*)szMd5); // calc md5
	szMd5A = szMd5;

	return TRUE;
}

void CMainDlg::StrTrim(SStringT& szStr, LPCTSTR pszTrims)
{
	if ( pszTrims == NULL )
		return szStr.TrimBlank();

	for ( const TCHAR* pszCh = pszTrims; *pszCh; pszCh++ )
		szStr.Trim(*pszCh);
}

int CMainDlg::StrFindOneOf(SStringT& szString, LPCTSTR pszFind)
{
	for ( const TCHAR* pszCh = pszFind; *pszCh; pszCh++ )
	{
		int i32Ret = szString.Find(*pszCh, 0);
		if ( i32Ret >= 0 )
			return i32Ret;
	}

	return -1;
}

BOOL CMainDlg::Date(SStringT& szSql)
{
	LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
	SStringT szExec = szSql.Right(szSql.GetLength() - _tcslen(kCmdDate));
	StrTrim(szExec, _T(" ();\""));
	StrTrim(szSql, _T(" ();\""));

	time_t t = _tcstoui64(szExec, NULL, 0);
	const TCHAR* const kSplit = _T(" ./\\-:：");
	if ( StrFindOneOf(szExec, kSplit) > 0 || t <= 9999 )	// 是时间字符串
	{
		int i32Index  = 0;
		int ai32Val[7]= {0 };	

		TCHAR* pszTok = _tcstok((LPTSTR)(LPCTSTR)szExec, kSplit); 
		for (int i32Index = 0; pszTok && i32Index < 7; i32Index++, pszTok = _tcstok( NULL, kSplit ) )
		{
			ai32Val[i32Index] = _tstoi(pszTok);
		}

		if ( i32Index == 3 || i32Index == 4 )
		{
			if ( StrFindOneOf(szExec, _T("/\\-")) > 0 )
			{	// 只有日期
				if ( ai32Val[0] <= 99 )	// 只有年份（yy)
					ai32Val[0] += 2000;	// 添加世纪
			}
			else
			{	// 只有时间
				SYSTEMTIME sysTime = { 0 };

				GetLocalTime(&sysTime);
				memmove(&ai32Val[3], &ai32Val[0], i32Index * sizeof(ai32Val[0]));	// 把时间后移3位
				ai32Val[0] = sysTime.wYear;		// 当年
				ai32Val[1] = sysTime.wMonth;	// 当月
				ai32Val[2] = sysTime.wDay;		// 当天
			}
		}

		tm tmDate = { 0 };

		tmDate.tm_year = ai32Val[0] - 1900; // 从1900开始
		tmDate.tm_mon  = ai32Val[1] ? ai32Val[1] - 1 : 0;	// [0, 11]
		tmDate.tm_mday = ai32Val[2];		// [1, 31]
		tmDate.tm_hour = ai32Val[3];		// [0, 23]
		tmDate.tm_min  = ai32Val[4];		// [0, 59]
		tmDate.tm_sec  = ai32Val[5];		// [0, 59]

		DWORD dwTime = (DWORD)mktime(&tmDate);
		LOGGER_LOG0_(&m_Log, eLLINFO, _T("DATE (%s) = %u(0x%08x)"), szSql, dwTime, dwTime);
	}
	else
	{	// 是UTC时间
		if ( tm* p = localtime(&t) )
			LOGGER_LOG0_(&m_Log, eLLINFO, _T("DATE (%s) = %04d-%02d-%02d %02d:%02d:%02d"), szExec, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
		else
			LOGGER_LOG0_(&m_Log, eLLINFO, _T("DATE (%s) failed"), szExec);
	}

	return TRUE;
}

BOOL CMainDlg::ExecFunc(SStringT& szSql)
{
	BOOL bOk = FALSE;
	SStringT szInfo;
	SStringT szExec;
	SStringA szExecA;

	if ( szSql.IsEmpty() ||
		 szSql.Left(1).CompareNoCase(kCmdHelp0) == 0 ||
		 szSql.Left(1).CompareNoCase(kCmdHelp1) == 0 ||
		 szSql.Left(4).CompareNoCase(kCmdHelp2) == 0 || 
		 szSql.Left(5).CompareNoCase(kCmdHelp3) == 0 )
	{
		LOGGER_INFO0_(&m_Log, _T("====================== Extension SQL syntax help ================================"));
		LOGGER_INFO0_(&m_Log, _T("Help [cmd]  ;Display this help, also can be: '.help', '?'"));
		LOGGER_INFO0_(&m_Log, _T("cls         ;Cleanup the output, also can be: clear, clean"));
		LOGGER_INFO0_(&m_Log, _T("DES string  ;Decode the string, also can be: DES \"string\" or DES (string)"));
		LOGGER_INFO0_(&m_Log, _T("MD5 string  ;MD5 the string, also can be: MD5 \"string\" or MD5 (string)\""));
		LOGGER_INFO0_(&m_Log, _T("DATE string ;Convert string to utc or string, eg.: DATE \"1506065896\" or DATE \"2017/02/03 13:22:11.0\""));
		LOGGER_INFO0_(&m_Log, _T("HEX string  ;Convert string to HEX, eg.: HEX \"1506065896\""));
		LOGGER_INFO0_(&m_Log, _T("DEC string  ;Convert string to DEC, eg.: DEC \"0x1506065896\""));
		LOGGER_INFO0_(&m_Log, _T("RTX eid     ;Boardcast a RTX message with QEID, eg.: RTX 32117"));

		return TRUE;
	}
	else if ( szSql.Left(3).CompareNoCase(kCmdCls0) == 0 ||
			  szSql.Left(5).CompareNoCase(kCmdCls1) == 0 ||
			  szSql.Left(5).CompareNoCase(kCmdCls2) == 0 )
	{
		SendMessage(WM_UPDATE_UI, R.id.rdt_Log, 0);	// 因为在线程中，不能直接操作
		return TRUE;
	}
	else if ( szSql.Left(3).CompareNoCase(kCmdDES) == 0 )
	{	// 解密密码
		LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
		szExec = szSql.Right(szSql.GetLength() - _tcslen(kCmdDES));
		StrTrim(szExec, _T(" ();\""));
		szExecA = S_CT2A(szExec);
		if ( bOk = DES(szExecA) )
			szInfo = S_CA2T(szExecA);
		else
			szInfo.Format(_T("Des(%s) failed"), szExec);
		
		LOGGER_LOG0_(&m_Log, bOk ? eLLINFO : eLLERROR, szInfo);
	}
	else if ( szSql.Left(3).CompareNoCase(kCmdMD5) == 0 )
	{	// 解密密码
		LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
		szExec = szSql.Right(szSql.GetLength() - _tcslen(kCmdMD5)); 
		StrTrim(szExec, _T(" ();\""));
		szExecA = S_CT2A(szExec);

		SStringA szMd5A;
		if ( bOk = MD5(szExecA, szMd5A) )
			szInfo.Format(_T("MD5 (%s) = %S"), szExec, szMd5A);
		else
			szInfo.Format(_T("MD5(%s) failed"), szExec);
	
		LOGGER_LOG0_(&m_Log, bOk ? eLLINFO : eLLERROR, szInfo);
	}
	else if ( szSql.Left(4).CompareNoCase(kCmdDate) == 0 )
	{	// 时间字符串转换
		bOk = Date(szSql);
	}
	else if ( szSql.Left(3).CompareNoCase(kCmdHex) == 0 )
	{
		LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
		szExec = szSql.Right(szSql.GetLength() - _tcslen(kCmdHex)); 
		StrTrim(szExec, _T(" ();\""));

		UINT64 u64Value = _tcstoui64(szExec, NULL, 0);
		LOGGER_INFO0_(&m_Log, _T("HEX (%s) = 0x%llx"), szExec, u64Value);
		bOk = TRUE;
	}
	else if ( szSql.Left(3).CompareNoCase(kCmdDec) == 0 )
	{
		LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
		szExec = szSql.Right(szSql.GetLength() - _tcslen(kCmdDec)); 
		StrTrim(szExec, _T(" ();\""));

		UINT64 u64Value = _tcstoui64(szExec, NULL, 16);
		LOGGER_INFO0_(&m_Log, _T("DEC (%s) = %llu"), szExec, u64Value);
		bOk = TRUE;
	}
	else if ( szSql.Left(3).CompareNoCase(kCmdRtx) == 0 )
	{
		LOGGER_INFO0_(&m_Log, LoadString(R.string.execSql), szSql);
		szExec = szSql.Right(szSql.GetLength() - _tcslen(kCmdRtx)); 
		StrTrim(szExec, _T(" ();\""));

		int i32Value = _tcstoul(szExec, NULL, 0);
		bOk = RTX(i32Value);
	}

	return bOk;
}

BOOL CMainDlg::OnContextMenu(CPoint pt)
{
	SMenuEx menu;
	if ( !menu.LoadMenu(UIRES.SMENU.RICHEDIT_MENU) )
		return FALSE;
	
	menu.GetMenuItem(R.id.menuEncrypted)->SetCheck(m_bEncrypted);
	menu.GetMenuItem(R.id.menuChinese)->SetCheck(m_i32LangId == R.id.menuChinese);
	menu.GetMenuItem(R.id.menuEnglish)->SetCheck(m_i32LangId == R.id.menuEnglish);

	menu.GetMenuItem(R.id.menuSql)->SetCheck(m_bSQL);
	::ClientToScreen(m_hWnd, &pt);
	 menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, m_hWnd, GetScale());

	return TRUE;
}

void CMainDlg::OnMenuItemChinese(UINT uNotifyCode, int nID, HWND wndCtl)
{
	OnLanguage(m_i32LangId = R.id.menuChinese);
}

void CMainDlg::OnMenuItemEnglish(UINT uNotifyCode, int nID, HWND wndCtl)
{
	OnLanguage(m_i32LangId = R.id.menuEnglish);
}

void CMainDlg::OnMenuItemAbout(UINT uNotifyCode, int nID, HWND wndCtl)
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal(m_hWnd);
}

void CMainDlg::OnMenuItemCleanup(UINT uNotifyCode, int nID, HWND wndCtl)
{
	m_szLog.Empty();
	m_prdtLog->SetWindowText(m_szLog);
}

void CMainDlg::OnMenuItemContactsZip(UINT uNotifyCode, int nID, HWND wndCtl)
{
	if ( uintptr_t h = _beginthreadex( NULL, 0, _MakeZipThread,  this, 0, NULL ))
		SAFE_CLOSE_HANDLE_(h);
	else
		LOGGER_ERROR0_(&m_Log, LoadString(R.string.startThreadFiled), GetLastError());
}

void CMainDlg::OnMenuItemEncrypted(UINT uNotifyCode, int nID, HWND wndCtl)
{
	m_bEncrypted = !m_bEncrypted;
}

void CMainDlg::OnMenuItemOpenDb(UINT uNotifyCode, int nID, HWND wndCtl)
{
	OnOpenDbClick();
}

void CMainDlg::OnMenuItemOpenFolder(UINT uNotifyCode, int nID, HWND wndCtl)
{
	OnOpenFolderClick();
}

void CMainDlg::OnMenuItemOpenLog(UINT uNotifyCode, int nID, HWND wndCtl)
{
	SStringT szLogFile = m_Log.GetLogFile();

	LOGGER_INFO0_(&m_Log, LoadString(R.string.openLog), szLogFile);
	::ShellExecute(NULL, _T("open"), szLogFile, NULL, NULL, SW_SHOW);
}

void CMainDlg::OnMenuItemOpenLogFolder(UINT uNotifyCode, int nID, HWND wndCtl)
{
	OpenFolder(m_Log.GetLogFile());
}

void CMainDlg::OnMenuItemRepair(UINT uNotifyCode, int nID, HWND wndCtl)
{
	StartRepair();
}

void CMainDlg::OnMenuItemRepairBest(UINT uNotifyCode, int nID, HWND wndCtl)
{
	StartRepair(TRUE);
}

void CMainDlg::OnMenuItemSql(UINT uNotifyCode, int nID, HWND wndCtl)
{
	if (SWindow* pexecSql = FindChildByName2<SWindow>(R.name.execSql))
	{
		m_bSQL = !m_bSQL;
		pexecSql->SetVisible(m_bSQL, TRUE);
	}
}

void CMainDlg::OnLanguage(int nID)
{
	ITranslatorMgr *pTransMgr = SApplication::getSingletonPtr()->GetTranslator();
	SASSERT(pTransMgr);
	SStringT szLang = (nID == R.id.menuChinese) ? _T("lang_cn") : _T("lang_en");
	pugi::xml_document xmlLang;
	if (SApplication::getSingletonPtr()->LoadXmlDocment(xmlLang, szLang, _T("lang")))
	{
		CAutoRefPtr<ITranslator> lang;
		pTransMgr->CreateTranslator(&lang);
		lang->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
		pTransMgr->SetLanguage(lang->name());
		pTransMgr->InstallTranslator(lang);
		SDispatchMessage(UM_SETLANGUAGE,0,0);
	}
}

void CMainDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(FALSE);

	if ( nChar == VK_RETURN )
	{
		if ( m_bSQL )
			OnSqlClick();	// 回车执行SQL
		else
			StartRepair();	// 回车执行 数据库修复
	}
}

BOOL CMainDlg::RTX(int i32UserId)
{	// 用于测试RTX导入通知
	static UINT WM_SXIT_EXPORT  = RegisterWindowMessage(_T("SXIT.EXPORT"));
	::PostMessage(HWND_BROADCAST, WM_SXIT_EXPORT, 0, i32UserId);
	return TRUE;
}

void CMainDlg::OnDbSelChanged(EventArgs *pEvt)
{
	SAFE_RELEASE_INTERFACE_(m_pISqlDb);	// 关闭当前已经打开的数据库
}
