// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <process.h>
#include <Windows.h>
#include <Shellapi.h>
#include "Public\Utils.h"
#include "sqlite3\I_SQLite.h"
#include "..\..\src\Logger\Logger.h"
#include "C_CommandLine.h"

#define WM_DBREPAIR_LOG		(WM_USER + 100)
#define WM_UPDATE_UI		(WM_USER + 101)

typedef std::vector<S_AccountInfo>	VectAccountInfo;
typedef VectAccountInfo::iterator	VectAccountInfoIt;
typedef VectAccountInfo::const_iterator	VectAccountInfoIt_;

typedef std::vector<SStringT>		VectBadDb;
typedef VectBadDb::iterator			VectBadDbIt;
typedef VectBadDb::const_iterator	VectBadDbIt_;

class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);

	void OnBtnMsgBox();
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnDropFiles(HDROP hDropInfo);
	BOOL OnContextMenu(CPoint pt);

	LRESULT OnDbRepairLog(UINT uMsg,WPARAM wp,LPARAM lp,BOOL & bHandled);
	LRESULT OnUpdateUI(UINT uMsg,WPARAM wp,LPARAM lp,BOOL & bHandled);
	BOOL	IsOutofScreen();

protected:
	void OnLanguage(int nID);
	void OnMenuItemAbout(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemCleanup(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemContactsZip(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemEncrypted(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemOpenDb(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemOpenFolder(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemOpenLog(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemOpenLogFolder(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemRepair(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemRepairBest(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemSql(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemChinese(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuItemEnglish(UINT uNotifyCode, int nID, HWND wndCtl);

	void OnRepairClick();
	void OnOpenDbClick();
	void OnOpenFolderClick();
	void OnSqlClick();

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	void OnDbSelChanged(EventArgs *pEvt);

	SStringT	LoadString(DWORD dwNamedId);
	void		StrTrim(SStringT& szString, LPCTSTR pszTrims = NULL);
	int			StrFindOneOf(SStringT& szString, LPCTSTR pszTrims);

	BOOL		Init();
	int			InitDb(LPCTSTR lpszImData);
	int			Check(LPCTSTR lpszDbFile);
	BOOL		Repair(LPCTSTR lpszDbFile);
	BOOL		GetAccountInfo(S_AccountInfo& sAInfo);
	const		TCHAR* GetBackupDbFile(SStringT& szDbFile);
	BOOL		IsFileOccupied(const TCHAR* pszFile, BOOL bPrompt);
	void		OpenFolder(LPCTSTR lpszFile);
	BOOL		BackupBadDbFile(const TCHAR* pszDbFile, SStringT& szBkFile);
	BOOL		DumpDb(LPCTSTR lpszFrom, LPCTSTR lpszTo);
	int			DumpTable(I_SQLite3Table* pISrcTbl, I_SQLite3* pIDstDb, const char* SqlA, INT64& i64Rowid);
	BOOL		OpenDb(LPCTSTR lpszFrom, CAutoRefPtr<I_SQLite3>& pIDb);
	BOOL		GetBindSql(I_SQLite3Table* pISrcTbl, const char* pszDstTable, SStringA& szSqlA);
	INT64		GetTableLastRowid(I_SQLite3* pIDb, const char* pszTable);
	unsigned	DoRepair();
	BOOL		UpdateUI(BOOL bEnable);
	BOOL		StartRepair(BOOL bBest=FALSE);
	BOOL		AddDb(const S_AccountInfo& sAInfo);
	BOOL		AddBadDb(const TCHAR* pszBadDbFile);

	inline BOOL IsAsciiStr(char* pszValue);
	inline BOOL ExecFunc(SStringT& szSql);

	BOOL	DES(SStringA& szDataA, const char* pszKey = "yf~gz@16\t03\a11");
	BOOL	MD5(SStringA& szDataA, SStringA& szMd5A);
	BOOL	Date(SStringT& szSql);
	BOOL    RTX(int i32UserId);

	unsigned DoSql();

	static void __stdcall LogCallback(PS_LogMsg_ psMsg, void* pvUserData);
	static unsigned __stdcall _RepairThread( void* pvData );
	static unsigned __stdcall _MakeZipThread( void* pvData );
	static unsigned __stdcall _SqlThread( void* pvData );
	static int sqlite3_callback(void* pUserData,int nCols,char** ppCols, char** ppColNames);
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(R.name.btn_close, OnClose)
		EVENT_NAME_COMMAND(R.name.btn_min, OnMinimize)
		EVENT_NAME_COMMAND(R.name.btn_max, OnMaximize)
		EVENT_NAME_COMMAND(R.name.btn_restore, OnRestore)
		EVENT_NAME_COMMAND(R.name.btn_check_repair,OnRepairClick)
		EVENT_NAME_COMMAND(R.name.btn_db,OnOpenDbClick)
		EVENT_NAME_COMMAND(R.name.btn_open_folder,OnOpenFolderClick)
		EVENT_NAME_COMMAND(R.name.btn_sql,OnSqlClick)

		EVENT_NAME_HANDLER(R.name.cmb_db, EVT_CB_SELCHANGE, OnDbSelChanged);
		EVENT_NAME_CONTEXTMENU(R.name.rdt_Log, OnContextMenu)
	EVENT_MAP_END()
	// void OnCommandIDHandlerEX(UINT uNotifyCode, int nID, CWindow wndCtl)

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MESSAGE_HANDLER(WM_UPDATE_UI,OnUpdateUI)
		MESSAGE_HANDLER(WM_DBREPAIR_LOG,OnDbRepairLog)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_KEYDOWN(OnKeyDown) 
		MSG_WM_DROPFILES(OnDropFiles)
		// 菜单
		COMMAND_ID_HANDLER_EX(R.id.menuEnglish, OnMenuItemEnglish)
		COMMAND_ID_HANDLER_EX(R.id.menuChinese, OnMenuItemChinese)
		COMMAND_ID_HANDLER_EX(R.id.menuRepairBest, OnMenuItemRepairBest)
		COMMAND_ID_HANDLER_EX(R.id.menuRepair, OnMenuItemRepair)
		COMMAND_ID_HANDLER_EX(R.id.menuOpenLogFolder, OnMenuItemOpenLogFolder)
		COMMAND_ID_HANDLER_EX(R.id.menuOpenLog, OnMenuItemOpenLog)
		COMMAND_ID_HANDLER_EX(R.id.menuOpenFolder, OnMenuItemOpenFolder)
		COMMAND_ID_HANDLER_EX(R.id.menuOpenDb, OnMenuItemOpenDb)
		COMMAND_ID_HANDLER_EX(R.id.menuEncrypted, OnMenuItemEncrypted)
		COMMAND_ID_HANDLER_EX(R.id.menuContactsZip, OnMenuItemContactsZip)
		COMMAND_ID_HANDLER_EX(R.id.menuCleanup, OnMenuItemCleanup)
		COMMAND_ID_HANDLER_EX(R.id.menuSql, OnMenuItemSql)
		COMMAND_ID_HANDLER_EX(R.id.menuAbout, OnMenuItemAbout)

		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	BOOL		m_bLayoutInited;
	VectAccountInfo m_vectAccountInfo;		// 账号信息
	VectBadDb	m_vectBadDb;				// 损坏数据库路径
	SStringT	m_szLog;					// 显示日志(m_rdtLog)
	SStringT	m_szSql;					// Sql
	BOOL		m_bRepairing;				// 修复中
	BOOL		m_bEncrypted;				// 已加密数据库
	int			m_i32MaxCorrupt;			// 损坏最大重试次娄
	int			m_i32CorruptStep;			// 检测步长
	int			m_i32LangId;
	char		m_aszHead[100][100];		// 表头Buffer
	TCHAR		m_aszDbFile[2][MAX_PATH];	// 当前进行修复的数据库

	BOOL		m_bNeedFullscreen;
	BOOL		m_bSQL;
	BOOL		m_bStopSQL;
	BOOL		m_bFirstRow;
	int			m_i32Rows;
	HANDLE		m_hThreadSql;

	CAutoRefPtr<I_SQLite3> m_pISqlDb;
	C_Logger	m_Log;						// 日志文件

	SButton*	m_pbtnSQL;
	SRichEdit*	m_prdtLog;					// 日志控件
	SComboBox*	m_pcmbAccount;				// 帐号信息
	SComboBox*	m_pcmbSQL;
};
