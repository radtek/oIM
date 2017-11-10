#include "stdafx.h"
#include "C_eIMMsgReadErrorMgr.h"


C_eIMMsgReadErrorMgr::C_eIMMsgReadErrorMgr(void)
	:m_pIDb ( NULL )
{
}

C_eIMMsgReadErrorMgr::~C_eIMMsgReadErrorMgr(void)
{
    Uninit();
}


BOOL C_eIMMsgReadErrorMgr::Init(I_SQLite3* pIDb)
{
    if ( pIDb )
    {
        SAFE_RELEASE_INTERFACE_(m_pIDb);// Release old
        pIDb->AddRef();		
        m_pIDb = pIDb;			// Set the new db
    }

    if ( m_pIDb )
        return TRUE;

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
    return TRUE;
}

BOOL C_eIMMsgReadErrorMgr::Uninit()
{
    SAFE_RELEASE_INTERFACE_(m_pIDb);
    return TRUE;
}

BOOL C_eIMMsgReadErrorMgr::Set(UINT64 u64MsgId, UINT64 u32SenderId)
{
    const char* const kInsertMsgRead     = "INSERT INTO t_MsgRead_Error("
        "msgid, eid ) VALUES (@msgid, @eid);";

    CHECK_INIT_RET_(FALSE);
    int i32Ret = 0;
    I_SQLite3Stmt*	pIStmt = NULL;
    GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
    GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertMsgRead, pIStmt, i32Ret);
    i32Ret = m_pIDb->Begin();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
    i32Ret = pIStmt->Bind("@msgid",	(INT64)u64MsgId);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@eid",	 (INT64)u32SenderId);	ASSERT_(i32Ret == SQLITE_OK);
    
    i32Ret = pIStmt->Step();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    i32Ret = m_pIDb->Commit();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    return EIMERR_NO_ERROR;
}

BOOL C_eIMMsgReadErrorMgr::Del(UINT64 dwMsgId)
{
    CHECK_INIT_RET_(FALSE);
    const char* const kDeleteItem = "DELETE From t_MsgRead_Error WHERE msgid=%llu;";
    const char* const pszSql = m_pIDb->VMPrintf(kDeleteItem, dwMsgId);
    int i32Ret = m_pIDb->SQL(pszSql, true);

    return (i32Ret == SQLITE_OK);
}

int C_eIMMsgReadErrorMgr::GetCount()
{
    CHECK_INIT_RET_(FALSE);
    const char* const kInsertMsgReadCount = "SELECT count(msgid) FROM t_MsgRead_Error";
    const char*		pszSql  = m_pIDb->VMPrintf(kInsertMsgReadCount);
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);

    if( pITable->Step() != SQLITE_ROW )
        return 0;

    return (DWORD)pITable->GetColInt(0);
}

BOOL C_eIMMsgReadErrorMgr::GetAllMsgReads(PFN_GetAllMsgRead pfnGetMsgs, void* pvUserData)
{
    CHECK_INIT_RET_(FALSE);
    const char* const kSelectAllMsgReadError		= "SELECT msgid, eid FROM t_MsgRead_Error";
    const char*     pszSql  = m_pIDb->VMPrintf(kSelectAllMsgReadError);
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    CHECK_NULL_RET_(pITable, FALSE);
    AUTO_RELEASE_(pITable);

    while( pITable->Step() == SQLITE_ROW )
    {
        UINT64 dwMsgID = pITable->GetColInt64(0); 
        UINT64 dwSenderID =pITable->GetColInt64(1); 
        int iMsgType = pITable->GetColInt(2);
        //<Cmd Id=73 MsgId=4638864315859378689 RecverId=197748 ReadType=0 />
        eIMStringA szXmlA;
        szXmlA += "<Cmd Id=73 MsgId=";
        szXmlA += Num2String(dwMsgID);
        szXmlA += " RecverId=";
        szXmlA += Num2String(dwSenderID);
        szXmlA += " ReadType=0 />";
        eIMString szXml;
        ::eIMUTF8ToTChar(szXmlA.c_str(), szXml);
        if ( !pfnGetMsgs(szXml.c_str(),  pvUserData) )
            break;
    }
    return EIMERR_NO_ERROR;
}

eIMStringA C_eIMMsgReadErrorMgr::Num2String(UINT64 dwNum)
{
    eIMStringA szNum;
    if(0 == dwNum)
    {
        return "0";
    }
    UINT64 uTmp = dwNum;
    for (int i=0; dwNum > 0; )
    {
        int iNum = dwNum%10;
        iNum = iNum*(i+1);
        char cNum = iNum +48;
        eIMStringA szTmp;
        szTmp = cNum;
        szNum = szTmp + szNum;
        dwNum = (dwNum - iNum)/10;
    }
    return szNum;
}