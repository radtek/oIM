#include "stdafx.h"
#include "C_eIMUrlFileMgr.h"

#define  TM_YEAR_ADD   (1900)
#define  TM_MONTH_ADD (1)

IMPLEMENT_PLUGIN_SINGLETON_(C_eIMUrlFileMgr, INAME_EIMENGINE_URLFILE);
C_eIMUrlFileMgr& C_eIMUrlFileMgr::GetObject()
{
    static C_eIMUrlFileMgr obj;
    return obj;
}

C_eIMUrlFileMgr::C_eIMUrlFileMgr(void)
	:m_pIDb ( NULL )
{
}

C_eIMUrlFileMgr::~C_eIMUrlFileMgr(void)
{
}

BOOL C_eIMUrlFileMgr::Init()
{
    if (!m_pIDb)
    {
        eIMStringA szDbFileA;
        if ( FAILED(GetEngineInterface(INAME_SQLITE_DB, (void**)&m_pIDb)) ||
            GetDbFile(eDBTYPE_CONTS, szDbFileA) == NULL ||
            SQLITE_OK != m_pIDb->Open(szDbFileA.c_str()) ||
            SQLITE_OK != m_pIDb->Key(DB_KEY, DB_KEY_LEN) ||
            GetDbFile(eDBTYPE_MSG, szDbFileA) == NULL ||
            SQLITE_OK != m_pIDb->Attach(szDbFileA.c_str(), MSG_DB_AS_NAME, DB_KEY))
        {
            SAFE_RELEASE_INTERFACE_(m_pIDb);
            GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
            EIMLOGGER_ERROR_(pILog, _T("Initial mamager failed!")); 
            return FALSE;
        }

        m_pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);
    }
	return TRUE;
}

BOOL C_eIMUrlFileMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

int C_eIMUrlFileMgr::Set(const S_UrlFileInfo& sUrlInfo)
{
    CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

    GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
    GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
    GET_IPY_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

    int i32Ret = 0;
    I_SQLite3Stmt*	pIStmt = NULL;
    GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertUrlFileItem, pIStmt, i32Ret);
    eIMStringA szDownUrlA;
    eIMStringA szLocalPathA;
    ::eIMTChar2UTF8(sUrlInfo.szDownUrl, szDownUrlA);
    ::eIMTChar2UTF8(sUrlInfo.szLocalPath, szLocalPathA);

    i32Ret = m_pIDb->Begin();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    i32Ret = pIStmt->Bind("@msgid",	(INT64)sUrlInfo.qMid);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@downurl",		szDownUrlA.c_str());		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@loaclPath",	szLocalPathA.c_str());	ASSERT_(i32Ret == SQLITE_OK);

    i32Ret = pIStmt->Step();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    i32Ret = m_pIDb->Commit();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    return EIMERR_NO_ERROR;
}

BOOL C_eIMUrlFileMgr::GetLocalPath(const S_UrlFileInfo& sUrlInfo, PFN_GetLocalPath pfnGetLocalPath, void* pvUserData)
{
    CHECK_INIT_RET_(FALSE);
    eIMStringA szDownUrlA;
    ::eIMTChar2UTF8(sUrlInfo.szDownUrl, szDownUrlA);
    const char* const pszSql = m_pIDb->VMPrintf(kSelectLocalPath, sUrlInfo.qMid, szDownUrlA.c_str());
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    if(pITable)
    {
        AUTO_RELEASE_(pITable);
        if( pITable->Step() == SQLITE_ROW )
        {
            eIMString szLocalPath;
            ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(0)), szLocalPath);
            pfnGetLocalPath( szLocalPath.c_str(), pvUserData) ;
            return TRUE;
        }
    }
    return FALSE;
}