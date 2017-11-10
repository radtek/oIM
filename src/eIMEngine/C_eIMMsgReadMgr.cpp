#include "StdAfx.h"
#include "C_eIMMsgReadMgr.h"


IMPLEMENT_PLUGIN_REF_(C_eIMMsgReadMgr, INAME_EIMENGINE_MSGREAD, m_dwRef);
C_eIMMsgReadMgr::C_eIMMsgReadMgr()
	: m_dwRef( 1 )
	, m_pIDb ( NULL )
{
}

C_eIMMsgReadMgr::~C_eIMMsgReadMgr()
{
	Uninit();
}

BOOL C_eIMMsgReadMgr::Init(I_SQLite3* pIDb)
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
		EIMLOGGER_ERROR_(pILog, _T("Initial msg read  mamager failed!")); 
		return FALSE;
	}

	m_pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);
	return TRUE;
}

BOOL C_eIMMsgReadMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

int C_eIMMsgReadMgr::Set(PS_MsgRead psMsgRead)
{
	CHECK_INIT_RET_(FALSE);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertMsgRead, pIStmt, i32Ret);
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@msgid_",			(INT64)psMsgRead->qmid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@receiver",			(INT64)psMsgRead->qeidRecver);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@readtimer",			(int)psMsgRead->dwTime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",				(int)psMsgRead->dwFlag);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",			(int)psMsgRead->dwTime);		ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMMsgReadMgr::AddMsgReadEmps(QMID qmid, VectSessionMember& vectEmps)
{
	CHECK_NULL_RET_(qmid != 0,EIMERR_FAIL);
	if(vectEmps.size() == 0) return EIMERR_FAIL;

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	VectSessionMemberIt it = vectEmps.begin();
	while(it != vectEmps.end())
	{
		S_MsgRead sMsgRead;
		ZERO_STRUCT_(sMsgRead);
		sMsgRead.qmid = qmid;
		sMsgRead.qeidRecver = *it;

		int i32Ret = Set(&sMsgRead);
		if( FAILED(i32Ret) )
		{
			return i32Ret;
		}

		it++;
	}

	return EIMERR_NO_ERROR;
}

int	C_eIMMsgReadMgr::GetHasMsgReadCount( QMID qmid )
{
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kCountMsgRead, qmid, (DWORD)MSG_READ_FLAG_READ);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return pITable->GetColInt(0);
}

int C_eIMMsgReadMgr::GetMsgReadCount( QMID qmid )
{
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kCountMsgReadAll, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return pITable->GetColInt(0);
}

BOOL C_eIMMsgReadMgr::UpdateFlag( QMID qmid, QEID qeid, DWORD dwFlag , BOOL bSet )
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(bSet ? kUpdateMsgReadFlag : kUpdateMsgReadClearFlag, dwFlag, qmid, qeid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}

int C_eIMMsgReadMgr::GetMsgRead(QMID qmid, PFN_1to100READ pfnRead, void* pvUserData)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectMsgRead, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while(pITable->Step() == SQLITE_ROW)
	{
		S_MsgRead sRead;
		memset(&sRead, 0, sizeof(S_MsgRead));
		sRead.qmid       = pITable->GetColInt64(0);
		sRead.qeidRecver = pITable->GetColInt(1);
		sRead.dwTime     = pITable->GetColInt(2);
		sRead.dwFlag     = pITable->GetColInt(3);
		if(pfnRead)
			pfnRead(&sRead, pvUserData);
	}

	return 1;
}

BOOL C_eIMMsgReadMgr::UpdateFlagAndReadTime(QMID qmid, QEID qeid, DWORD dwFlag, DWORD dwTime)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kUpdateMsgRead, dwFlag, dwTime, qmid, qeid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}

int C_eIMMsgReadMgr::DeleteMsgRead(const std::list<QMID>& delMsgIdLst)
{
    CHECK_INIT_RET_(FALSE);
    const char* const kDeleteMsgId = "DELETE FROM t_msg_read WHERE msgid_=%llu;";
    std::list<QMID>::const_iterator itr = delMsgIdLst.begin();
    while(itr != delMsgIdLst.end())
    {
        QMID delMid = (*itr);
        const char* pszSql = m_pIDb->VMPrintf(kDeleteMsgId, delMid);
        int i32Ret = m_pIDb->SQL(pszSql, true);
        ++itr;
    }
    return 1;
}