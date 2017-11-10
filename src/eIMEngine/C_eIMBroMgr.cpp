#include "StdAfx.h"
#include "C_eIMBroMgr.h"


IMPLEMENT_PLUGIN_REF_(C_eIMBroMgr, INAME_EIMENGINE_BROMGR, m_dwRef);
C_eIMBroMgr::C_eIMBroMgr(void)
	: m_dwRef( 1 )
	, m_pIDb ( NULL )
{
}

C_eIMBroMgr::~C_eIMBroMgr(void)
{
	Uninit();
}

BOOL C_eIMBroMgr::Init(I_SQLite3* pIDb)
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

BOOL C_eIMBroMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

int C_eIMBroMgr::Set(PS_BroInfo psBro, const TCHAR* pszXml)
{
	CHECK_NULL_RET_(psBro,  EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(pszXml, EIMERR_INVALID_PARAM);
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertBroRecords, pIStmt, i32Ret);

	eIMStringA szXmlA;
	::eIMTChar2UTF8(pszXml, szXmlA);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@msgid_",			(INT64)psBro->qMid);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@recver",			psBro->pszrecver);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@title",			    psBro->psztitle);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@msg",				szXmlA.c_str());				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@type",				psBro->i32type);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",			psBro->i32timestamp);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@offmsg_flag",		psBro->i32offmsg_flag);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@newmsg_flag",		psBro->i32newmsg_flag);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",				psBro->i32flag);				ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMBroMgr::SetAlert(PS_BroInfo psBro, const TCHAR* pszXml)
{
	CHECK_NULL_RET_(psBro,  EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(pszXml, EIMERR_INVALID_PARAM);
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertAlertRecords, pIStmt, i32Ret);

	eIMStringA szXmlA;
	::eIMTChar2UTF8(pszXml, szXmlA);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@msgid_",			(INT64)psBro->qMid);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sendid",			(INT64)psBro->u32SendID);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@title",			    psBro->psztitle);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@msg",				szXmlA.c_str());				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@type",				psBro->i32type);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",			psBro->i32timestamp);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@offmsg_flag",		psBro->i32offmsg_flag);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@newmsg_flag",		psBro->i32newmsg_flag);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",				psBro->i32flag);				ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

//DWORD C_eIMBroMgr::Enum(PS_SMSEnum psCond, PFN_CountCb pfnCountCb, void*	pvUserData)
//{
//	CHECK_INIT_RET_(0);
//	CHECK_NULL_RET_(psCond, 0);
//
//	DWORD		dwCount = 0;
//
//	return dwCount;
//}
//
//DWORD C_eIMBroMgr::GetCount(QSID QeidOrPhone, int iType)
//{
//	CHECK_INIT_RET_(0);
//
//	return 0;
//}
//
//DWORD C_eIMBroMgr::Enum(PS_SMSEnum psCond, PFN_SMSMsgCb pfnMsgCb, void* pvUserData)
//{
//	CHECK_NULL_RET_(psCond, 0);
//	CHECK_NULL_RET_(pfnMsgCb, 0);
//	CHECK_INIT_RET_(0);
//
//	DWORD		dwCount = 0;
//
//	return dwCount;
//}
