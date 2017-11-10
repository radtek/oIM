#include "StdAfx.h"
#include "C_eIMSMSMgr.h"


IMPLEMENT_PLUGIN_REF_(C_eIMSMSMgr, INAME_EIMENGINE_SMSMGR, m_dwRef);
C_eIMSMSMgr::C_eIMSMSMgr(void)
	: m_dwRef( 1 )
	, m_pIDb ( NULL )
{
}

C_eIMSMSMgr::~C_eIMSMSMgr(void)
{
	Uninit();
}

BOOL C_eIMSMSMgr::Init(I_SQLite3* pIDb)
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

BOOL C_eIMSMSMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

QSID C_eIMSMSMgr::Get(QMID qmid)
{
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsQmid, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return (QSID)pITable->GetColInt64(0);
}

BOOL C_eIMSMSMgr::Get(QMID qmid, S_Msg& sMsg, eIMStringA& szXmlA)
{
	szXmlA.clear();
	CHECK_INIT_RET_(FALSE);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsQmid2, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if( pITable->Step()!= SQLITE_ROW )
		return FALSE;

	//SELECT sid, eid, msg, timestamp, type, flag,
	sMsg.qsid   = (QSID)pITable->GetColInt64(0);
	sMsg.qmid   = qmid;
	sMsg.qeid   = pITable->GetColInt(1);
	sMsg.dwTime = pITable->GetColInt(3);
	sMsg.eType  = (E_SessionType)pITable->GetColInt(4);
	sMsg.dwFlag = pITable->GetColInt(5);

	szXmlA = eIMCheckColTextNull(pITable->GetColText(2));
	return TRUE;
}

int C_eIMSMSMgr::Set(PS_Sms psMsg, const TCHAR* pszXml)
{
	CHECK_NULL_RET_(psMsg,  EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(pszXml, EIMERR_INVALID_PARAM);
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	//	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceRecords, pIStmt, i32Ret);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertSmsRecords, pIStmt, i32Ret);

	eIMStringA szXmlA;
	::eIMTChar2UTF8(pszXml, szXmlA);
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@msgid_",			(INT64)psMsg->qmid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@eid_or_phone",		(INT64)psMsg->eidorPhone);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@type",				psMsg->i32eidOrPhoneType);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@msg",				szXmlA.c_str());			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",			(int)psMsg->dwTime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@offmsg_flag",		(int)psMsg->i32offmsg_flag);ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@newmsg_flag",		(int)psMsg->i32newmsg_flag);ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",				(int)psMsg->dwFlag);		ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

DWORD C_eIMSMSMgr::Enum(PS_SMSEnum psCond, PFN_CountCb pfnCountCb, void*	pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(psCond, 0);

	DWORD		dwCount = 0;
	const char* pszSql    = NULL;
	const char* pszDate   = NULL;

	if ( psCond->dwLimit == 0 )
		psCond->dwLimit = 0xFFFFFFFF;

	// Date
	if ( psCond->eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp=%u ", psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
		pszDate = m_pIDb->VMPrintf("AND timestamp>=%u ", psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u ", psCond->dwStart, psCond->dwEnd);
	

	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearch = NULL;

	// Content and contacts
	if ( (psCond->eOperate & eMSG_OP_TEXT) && (psCond->eOperate & eMSG_OP_CONTS) )
	{
		::eIMTChar2UTF8(psCond->szContent, szTextA);
		::eIMTChar2UTF8(psCond->szContacts, szContsA);
		pszSearch = m_pIDb->VMPrintf(kSelectSmsRecordsAccountContent,szContsA.c_str(), szContsA.c_str(), szTextA.c_str());

		if(pszDate)
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsJuseEidCount, pszSearch, pszDate, "And type=0 group by eid_or_phone order by timestamp desc");
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsJuseEidCount, pszSearch, "", "And type=0 group by eid_or_phone order by timestamp desc");
		}
	}
	else if (psCond->eOperate & eMSG_OP_TEXT)
	{
		::eIMTChar2UTF8(psCond->szContent, szTextA);
		pszSearch = m_pIDb->VMPrintf(kSelectSmsRecordsContent, szTextA.c_str()); 

		if(pszDate)
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsCount, pszSearch, pszDate, "And type=0 group by eid_or_phone", 
				pszSearch, pszDate, "And type=1 group by eid_or_phone order by timestamp desc");
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsCount, pszSearch, "", "And type=0 group by eid_or_phone",
				pszSearch, "", "And type=1 group by eid_or_phone order by timestamp desc");
		}
	}
	else if (psCond->eOperate & eMSG_OP_CONTS)
	{
		::eIMTChar2UTF8(psCond->szContacts, szContsA);
		pszSearch = m_pIDb->VMPrintf(kSelectSmsRecordsAccount, szContsA.c_str(), szContsA.c_str()); // join...
		if(pszDate)
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsJuseEidCount, pszSearch, pszDate, "And type=0 group by eid_or_phone order by timestamp desc");
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsJuseEidCount, pszSearch, "", "And type=0 group by eid_or_phone order by timestamp desc");
		}
	}
	else
	{
		if(pszDate)
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsCount, "Where 1", pszDate, "And type=1 group by eid_or_phone"
				, "Where 1", pszDate, "And type=0 group by eid_or_phone order by timestamp desc");
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kSelectSmsRecordsCount, "Where 1", "", "And type=1 group by eid_or_phone"
				, "Where 1", "", "And type=0 group by eid_or_phone order by timestamp desc");;
		}
	}

	m_pIDb->Free((void*)pszSearch);

	m_pIDb->Free((void*)pszDate);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	S_SmsListInfo rInfo;
	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		memset(&rInfo, 0, sizeof(S_SmsListInfo));
		rInfo.QeidOrPhone = pITable->GetColInt64(0);
		rInfo.iCount = pITable->GetColInt(1);
		rInfo.iType = pITable->GetColInt(2);
		rInfo.dwTimestamp = pITable->GetColInt(3);
		rInfo.qmid = pITable->GetColInt64(4);
		if ( !pfnCountCb(rInfo, pvUserData) )
			break;
	}

	return dwCount;
}

DWORD C_eIMSMSMgr::GetCount(QSID QeidOrPhone, int iType)
{
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectSmsAllCount, QeidOrPhone, iType);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return pITable->GetColInt(0);
}

DWORD C_eIMSMSMgr::Enum(PS_SMSEnum psCond, PFN_SMSMsgCb pfnMsgCb, void* pvUserData)
{
	CHECK_NULL_RET_(psCond, 0);
	CHECK_NULL_RET_(pfnMsgCb, 0);
	CHECK_INIT_RET_(0);

	DWORD		dwCount = 0;
	char		szQsidCond[40] = { 0 };
	const char* pszSql    = NULL;
	const char* pszDate   = NULL;
	char		szOrderbyInfo[10] = { 0 };

	if ( psCond->dwLimit == 0 )
		psCond->dwLimit = 0xFFFFFFFF;

	pszDate = m_pIDb->VMPrintf("WHERE eid_or_phone=%lld And type=%d ORDER BY timestamp ASC", psCond->rQSID, psCond->iType);

	const char* pszlimit   = NULL;
	pszlimit = m_pIDb->VMPrintf("LIMIT %u OFFSET %u", psCond->dwLimit, psCond->dwOffset);

	if(pszDate)
	{
		pszSql = m_pIDb->VMPrintf(kSelectSmsRecords, pszDate, pszlimit);
	}
	else
	{
		pszSql = m_pIDb->VMPrintf(kSelectSmsRecords, "", pszlimit);
	}

	m_pIDb->Free((void*)pszlimit);

	m_pIDb->Free((void*)pszDate);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	S_Sms rInfo;
	eIMString		sTemp;
	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		rInfo.eidorPhone = psCond->rQSID;
		rInfo.qmid = pITable->GetColInt64(0);
		rInfo.iType = pITable->GetColInt(2);
		::eIMUTF8ToTChar((char*)pITable->GetColText(3), sTemp);
		rInfo.dwTime = pITable->GetColInt(4);
		rInfo.i32offmsg_flag = pITable->GetColInt(5);
		rInfo.i32newmsg_flag = pITable->GetColInt(6);
		rInfo.dwFlag = pITable->GetColInt(7);
		
		if ( !pfnMsgCb(rInfo,sTemp.c_str(), pvUserData) )
			break;
	}

	return dwCount;
}
