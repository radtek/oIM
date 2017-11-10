#include "stdafx.h"
#include "C_eIMSessionMgr.h"


IMPLEMENT_PLUGIN_REF_(C_eIMSessionMgr, INAME_EIMENGINE_SESSIONMGR, m_dwRef);
C_eIMSessionMgr::C_eIMSessionMgr(void)
	: m_dwRef( 1 )
	, m_pIDb( NULL )
{
}

C_eIMSessionMgr::~C_eIMSessionMgr(void)
{
	Uninit();
}

BOOL C_eIMSessionMgr::Init(I_SQLite3* pIDb)
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

BOOL C_eIMSessionMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}


BOOL C_eIMSessionMgr::ReceiptMsgIsRead(QSID qsid, DWORD dwFlag)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kSelectReceiptMsgFlag,qsid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		int flag = pITable->GetColInt(0);
		if (flag & eSEFLAG_RECEIPT_READ)
			return TRUE;
	}

	return FALSE;
}

BOOL C_eIMSessionMgr::UpdateFlag(QSID qsid, DWORD dwFlag, BOOL bSet)
{
	CHECK_INIT_RET_(FALSE);
	S_SessionInfo	 sSessionInfo    = { 0 };
	S_SessionCounter sSessionCounter = { qsid, UINT_MAX, UINT_MAX };
	Get(qsid, &sSessionInfo);
	const char* pszSql = m_pIDb->VMPrintf(bSet ? kUpdateSessionSetFlag : kUpdateSessionClearFlag, dwFlag, qsid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	if ( bSet && (dwFlag & DB_FLAG_DELETED) )
	{	// 同时删除置顶会话
		pszSql = m_pIDb->VMPrintf( kDeleteSessionTop, qsid);
		m_pIDb->SQL(pszSql, true);
	}

	CHECK_NULL_RET_((SQLITE_OK == i32Ret),FALSE);
	if(!bSet && (dwFlag & eSEFLAG_OFFLINE))
	{
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt, "off_msg", 0, qsid);
		i32Ret = m_pIDb->SQL(pszSql, true);
		CHECK_NULL_RET_((SQLITE_OK == i32Ret),FALSE);
		sSessionCounter.dwOfflineCounter = 0;
	}
	if (!bSet && (dwFlag & eSEFLAG_NEW))
	{
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt, "new_msg", 0, qsid);
		i32Ret = m_pIDb->SQL(pszSql, true);
		CHECK_NULL_RET_((SQLITE_OK == i32Ret),FALSE);
		sSessionCounter.dwNewCounter = 0;
	}

	if ((dwFlag & eSEFLAG_RECEIPT_READ) || (dwFlag & eSEFLAG_AT))
	{
		pszSql = m_pIDb->VMPrintf(kUpdateReceiptMsgFlag,dwFlag,qsid);
		i32Ret = m_pIDb->SQL(pszSql,true);
		CHECK_NULL_RET_((SQLITE_OK == i32Ret),FALSE);
	}

	if ( !(dwFlag & DB_FLAG_DELETED) && !(dwFlag & eSEFLAG_RECEIPT_READ) )
	{
		if ( sSessionCounter.dwOfflineCounter == 0 && sSessionInfo.dwOfflineMsg == 0 )
			sSessionCounter.dwOfflineCounter = UINT_MAX - 1;	// Not changed flag

		if ( sSessionCounter.dwNewCounter == 0 && sSessionInfo.dwNewMsg == 0 )
			sSessionCounter.dwNewCounter = UINT_MAX - 1;		// Not changed flag

		hpi.SendEvent(EVENT_UPDATE_SESSION_COUNTER, &sSessionCounter);
	}
	//SendMsgRead(qsid);

	return TRUE;
}

int C_eIMSessionMgr::Set(QSID qsid, PQEID paQeid, DWORD dwCount, BOOL bAdd, BOOL bClearFirst)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(paQeid, FALSE);
	
	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceSessionEmp, pIStmt, i32Ret);

	if ( bClearFirst )
		Set(qsid, eSET_TYPE_CLEAR_SESSION_EMP, dwCount);	// Delete at first	

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		TRACE_(_T("Update session member: qsid:%llu, qeid:%u, flag:%u"), qsid, paQeid[dwIndex], bAdd);
		i32Ret = pIStmt->Bind("@sid_",	(INT64)qsid);			ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@eid_",	(int)paQeid[dwIndex]);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@flag",	bAdd ? 0 : eSEFLAG_DELETE);	ASSERT_(i32Ret == SQLITE_OK);

		i32Ret = pIStmt->Step();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	}

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMSessionMgr::Set(QSID  qsid,regulargroup_member* paMember, DWORD dwCount,BOOL  bAdd, BOOL  bClearOld)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(paMember, FALSE);

	// 清除旧的成员数据
	if(bClearOld)
		Set(qsid, eSET_TYPE_CLEAR_SESSION_EMP, dwCount);	// Delete at first	
	
	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceSessionEmp2, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		TRACE_(_T("Update Regular group member: qsid:%llu, qeid:%u, flag:%u, attrib:%u"), qsid, paMember[dwIndex].dwUserID, bAdd, paMember[dwIndex].cAttribute);
		i32Ret = pIStmt->Bind("@sid_",	(INT64)qsid);				ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@eid_",	(int)paMember[dwIndex].dwUserID);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@attrib",	(int)paMember[dwIndex].cAttribute);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@flag",	bAdd ? 0 : eSEFLAG_DELETE);	ASSERT_(i32Ret == SQLITE_OK);

		i32Ret = pIStmt->Step();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	}

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMSessionMgr::Set(PS_SessionInfo_ psInfo,QSID qsid)
{
	CHECK_NULL_RET_(psInfo, FALSE);
	CHECK_INIT_RET_(FALSE);
#define BIND_TCHAR_(BIND, SRC) \
	if (_tcslen(SRC) ) \
	{ \
		::eIMTChar2UTF8(SRC, szValueA); \
		i32Ret = pIStmt->Bind(BIND, szValueA.c_str());	\
	} \
	else \
	{ \
		i32Ret = pIStmt->Bind(BIND); \
	}

	int i32Ret = 0;
	eIMStringA		szValueA;
	I_SQLite3Stmt*	pIStmt = NULL; 
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceSession, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@sid_",	(INT64)psInfo->qsid);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@type",	psInfo->eType);					ASSERT_(i32Ret == SQLITE_OK);
	BIND_TCHAR_("@title", psInfo->szTitle);							ASSERT_(i32Ret == SQLITE_OK);
	BIND_TCHAR_("@remark", psInfo->szRemark);						ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",	  (int)psInfo->eFlag);			ASSERT_(i32Ret == SQLITE_OK);      
	i32Ret = pIStmt->Bind("@cid", 	  (int)psInfo->CreaterId);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@ctime",	  (int)psInfo->dwCreateTime);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@utime",   (int)psInfo->dwUpdateTime);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@off_msg", (int)psInfo->dwOfflineMsg);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@new_msg", (int)psInfo->dwNewMsg);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sutime",  (int)psInfo->dwSUpdateTime);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@virgroupid",  (INT64)qsid);	            ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return TRUE;
}

BOOL C_eIMSessionMgr::Set(VIRTUAL_GROUP_INFO_NOTICE* virtualGroupInfo)
{
	CHECK_INIT_RET_(FALSE);
	
	virtual_group_basic_info Base_Info = virtualGroupInfo->mVirGroupInfo.mBasicInfo;
	if(strlen(Base_Info.strGroupID) == 0)
	{
		return FALSE;
	}

	int i32RetInfo;
	int i32RetMember;
	eIMStringA szWaiting = Base_Info.strWaiting;
	eIMStringA szHangup = Base_Info.strHangup;
	eIMStringA szOnCall = Base_Info.strOncall;	

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	if (Base_Info.cUpdateType == VIRGROUP_ADD || Base_Info.cUpdateType == VIRGROUP_UPDATE)
	{
		////////insert into 
		const char* pszSqlGroupInfo_insert = m_pIDb->VMPrintf(kUpdateGroupInfo,Base_Info.dwMainUserID,Base_Info.strGroupID,Base_Info.wMemberNum
			,Base_Info.wSingleSvcNum,Base_Info.wTimeoutMinute,szWaiting.c_str(),szHangup.c_str(),szOnCall.c_str()
			,(int)(Base_Info.cDisplaysUsercode),Base_Info.dwGroupTime,(int)(Base_Info.cUpdateType));

		i32RetInfo = m_pIDb->Begin();
		ROLLBACK_FAILED_RET_(m_pIDb, i32RetInfo);

		if ( pszSqlGroupInfo_insert )
		{
			i32RetInfo = m_pIDb->SQL(pszSqlGroupInfo_insert, true);
		}
		EIMLOGGER_INFO_(pILog, _T("%d,SUCCESS:0,insert into t_groupinfo_virtual"),i32RetInfo); 
		EIMLOGGER_INFO_(pILog,_T("into t_groupinfo_virtual dwMainUserID:%u strGroupID:%S dwGroupTime:%u wMemberNum:%d strWaiting:%S"),
			Base_Info.dwMainUserID,Base_Info.strGroupID,Base_Info.dwGroupTime,Base_Info.wMemberNum,Base_Info.strWaiting);

		const char* pszSqlGroupMember;
		for (int i = 0;i < MAX_VIR_GROUP_MEMBER;i++)
		{
			if (virtualGroupInfo->mVirGroupInfo.dwGroupMember[i] <= 0)
			{
				break;
			}

			pszSqlGroupMember = m_pIDb->VMPrintf(kUpdateGroupMember,Base_Info.strGroupID,virtualGroupInfo->mVirGroupInfo.dwGroupMember[i]
			,1,Base_Info.dwGroupTime,Base_Info.cUpdateType);

			if (pszSqlGroupMember)
			{
				i32RetMember = m_pIDb->SQL(pszSqlGroupMember, true);
				EIMLOGGER_INFO_(pILog, _T("%d,SUCCESS:0,insert into t_groupmember_virtual"),i32RetMember);
				EIMLOGGER_INFO_(pILog,_T("into t_groupmember_virtual strGroupID:%S userid:%u update_time:%u"),
					Base_Info.strGroupID,virtualGroupInfo->mVirGroupInfo.dwGroupMember[i],Base_Info.dwGroupTime);
			}
		}
		i32RetInfo = m_pIDb->Commit();
		ROLLBACK_FAILED_RET_(m_pIDb, i32RetInfo);
	}
	else if (Base_Info.cUpdateType == VIRGROUP_DELETE)
	{
		if (const char* pszSqlGroupInfo_delete = m_pIDb->VMPrintf(kUpdateGroupInfo_delete,Base_Info.strGroupID))
			i32RetInfo = m_pIDb->SQL(pszSqlGroupInfo_delete,true);

		if (const char* pszSqlGroupMember_delete = m_pIDb->VMPrintf(kUpdateGroupMember_delete,Base_Info.strGroupID))
			i32RetInfo = m_pIDb->SQL(pszSqlGroupMember_delete,true);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL C_eIMSessionMgr::Set(QSID qsid, E_SetType eSetType, DWORD dwValue,const TCHAR* pszValue, PS_SessionInfo_ psInfo)
{
	CHECK_INIT_RET_(FALSE);

	eIMStringA szValueA;
	const char* pszSql  = NULL;
	S_SessionInfo sInfo = { 0 };
	if (!Get(qsid, &sInfo) && eSetType != eSET_TYPE_ALL )
		return FALSE;

	switch(eSetType)
	{
	case eSET_TYPE_ALL:
		return Set(psInfo);
	case eSET_TYPE_UTIME:
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt, "utime", dwValue, qsid);
		break;
	case eSET_TYPE_SUTIME:
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt, "sutime", dwValue, qsid);
		break;
	case eSET_TYPE_OFF_MSG:
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt2, "off_msg", "off_msg", dwValue,eSEFLAG_OFFLINE, qsid);
		break;
	case eSET_TYPE_NEW_MSG:
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt2, "new_msg", "new_msg", dwValue,eSEFLAG_NEW, qsid);
		break;
	case eSET_TYPE_FLAG:
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt, "flag", dwValue, qsid);
		break;
	case eSET_TYPE_TYPE:	
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldInt, "utime", dwValue, qsid);
		break;
	case eSET_TYPE_TITLE:
		::eIMTChar2UTF8(pszValue, szValueA);
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldStr, "title", szValueA.c_str(), qsid);
		break;
	case eSET_TYPE_REMARK:
		::eIMTChar2UTF8(pszValue, szValueA);
		pszSql = m_pIDb->VMPrintf(kUpdateSessionSidFieldStr, "remark", szValueA.c_str(), qsid);
		break;
	case eSET_TYPE_CHECKPOINT:
		pszSql = m_pIDb->VMPrintf(kWalCheckpoint);
		break;	
	case eSET_TYPE_CLEAR_SESSION_EMP:
		pszSql = m_pIDb->VMPrintf(kClearSessionEmp, qsid);
		break;
	default:
		ASSERT_(FALSE);
		return FALSE;
	}

	if ( pszSql )
	{
		int i32Ret = m_pIDb->SQL(pszSql, true);
		return (i32Ret == SQLITE_OK);
	}

	return TRUE;
}

BOOL C_eIMSessionMgr::_GetRow(I_SQLite3Table* pITable, PS_SessionInfo psSessionInfo) 
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pITable, FALSE);
	CHECK_NULL_RET_(psSessionInfo, FALSE);

	eIMString		szValue;

	psSessionInfo->qsid  = pITable->GetColInt64( 0 );					// sid
	psSessionInfo->eType = (E_SessionType)pITable->GetColInt( 1 );		// type
								// title
	::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText( 2 )), szValue);
	_tcsncpy(psSessionInfo->szTitle, szValue.c_str(), MAX_SESSION_TITLE -1);	
							// remark
	::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText( 3 )), szValue);
	_tcsncpy(psSessionInfo->szRemark, szValue.c_str(), MAX_SESSION_REMARK -1);

	psSessionInfo->eFlag        = (E_SEFlag)pITable->GetColInt( 4 );	// flag
	psSessionInfo->CreaterId    = (QEID)pITable->GetColInt64( 5 );		// cid
	psSessionInfo->dwCreateTime = pITable->GetColInt( 6 );				// ctime
	psSessionInfo->dwUpdateTime = pITable->GetColInt( 7 );				// utime
	psSessionInfo->dwOfflineMsg = pITable->GetColInt( 8 );				// off_msg
	psSessionInfo->dwNewMsg     = pITable->GetColInt( 9 );				// new_msg
	psSessionInfo->dwSUpdateTime= pITable->GetColInt( 10 );				// Session update time
	psSessionInfo->dwTopTime    = pITable->GetColInt( 12 );				// Top time
	return TRUE;
}

BOOL C_eIMSessionMgr::_GetRow1(I_SQLite3Table* pITable, PS_SessionInfo psSessionInfo)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pITable, FALSE);
	CHECK_NULL_RET_(psSessionInfo, FALSE);

	eIMString		szValue;

	psSessionInfo->qsid  = pITable->GetColInt64( 0 );					// sid
	psSessionInfo->eType = (E_SessionType)pITable->GetColInt( 1 );		// type
	// title
	::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText( 2 )), szValue);
	_tcsncpy(psSessionInfo->szTitle, szValue.c_str(), MAX_SESSION_TITLE -1);	
	psSessionInfo->CreaterId    = (QEID)pITable->GetColInt( 3 );		// cid
	psSessionInfo->dwUpdateTime = pITable->GetColInt( 4 );				// utime
	return TRUE;
}

BOOL C_eIMSessionMgr::IsVirGroupSession(QSID qsid)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kisVirGroupSession,qsid,eSESSION_TYPE_VIRGROUP);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL C_eIMSessionMgr::GetMsgByFid(QSID fid_,PFN_FileInfo pFileMsg,void* pvUserData)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kSelectMsgByFilefid,fid_);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		eIMString		szValue;
								
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(0)), szValue);// title

		pFileMsg(szValue, pvUserData);
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::GetSidByFid(QSID fid_,PFN_FileAnexAckCb pFileAnexAck,void* pvUserData)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kSelectSidByFilefid,fid_);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		pFileAnexAck(pITable->GetColInt64( 0 ), pvUserData);
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::GetSidByVirGroupid(QSID &qsid,QEID qeid,E_SessionType type)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kGetSidbyVirGroupid,eSESSION_TYPE_VIRGROUP,qsid,qeid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		qsid = pITable->GetColInt64(0);
		return TRUE;
	}

	return FALSE;
}

BOOL C_eIMSessionMgr::IsVirGroupMainId(QEID qeidReceive,QSID Groupid ,int type)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = NULL;   

	if (0 == type)
	{
		pszSql = m_pIDb->VMPrintf(kgetvirGroupmainid);
	}
	else if (1 == type)
	{
		pszSql = m_pIDb->VMPrintf(kgetvirGroupGroupId);
	}
	else
	{
		pszSql = m_pIDb->VMPrintf(kgetvirGroupUserId);
	}
		
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		UINT64 tempid = pITable->GetColInt64(0);
		if (((0 == type || 2 == type) && tempid == qeidReceive) || (1 == type && tempid == Groupid ))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::Get(QSID sid_,QSID& VirGroupId)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kGetVirGroupId,sid_);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		VirGroupId = pITable->GetColInt64(0);
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::Get(QEID qeidRecver,QSID& VirGroupId)
{	
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	const char* pszSql = m_pIDb->VMPrintf(kvirtualGroupId,qeidRecver);
	EIMLOGGER_INFO_(pILog, _T("FUNVIRGROUP: select virgroupid :%S"),pszSql); 

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		TCHAR* sztempGroupId = (TCHAR*)pITable->GetColText16(0);
		eIMStringA tempstrGroupId;
		eIMTChar2UTF8(sztempGroupId,tempstrGroupId);
		VirGroupId = std::stoll(tempstrGroupId.c_str());

		EIMLOGGER_INFO_(pILog, _T("FUNVIRGROUP: Virgroupid :%lld"),VirGroupId);
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::GetVirGroupTip(PFN_SessionVirTipCb fpnTip,QEID qeidRecver,void* strVirTip)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kvirtualGroupWaitTip,qeidRecver);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		TCHAR* szTempWaitTip = (TCHAR*)pITable->GetColText16(0);
		eIMStringA tempstrWaitTip;
		eIMTChar2UTF8(szTempWaitTip,tempstrWaitTip);
		eIMString tempWaitTip = szTempWaitTip;
		fpnTip(tempWaitTip,strVirTip);
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::Get(QEID qeidSender, QEID qeidRecver, PS_SessionInfo psSessionInfo,QSID qsidGroupID, BOOL bIs1To10T)
{
	CHECK_NULL_RET_(psSessionInfo, FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR); 

	int isVirGroupByNormal = 0;
	QEID qeidLogin = Eng.GetPurview(GET_LOGIN_QEID);

	if ( qeidSender == 0 )
	{
		GET_IEGN_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		qeidSender = pIEgn->GetAttributeInt(IME_ATTRIB_USER_ID);
	}

	QEID  aQeid[2] = {qeidSender, qeidRecver};
	EIMLOGGER_INFO_(pILog, _T("qeidSender:%d   qeidReceive:%d"),qeidSender,qeidRecver);
	//ASSERT_(qeidSender != qeidRecver);

	BOOL bGet = FALSE;
	if(bIs1To10T)
		bGet = Get1To10T(aQeid, 2, psSessionInfo);
	else
	{
		if (qsidGroupID == 0)
		{
			bGet = Get(aQeid, 2, psSessionInfo,0,0, TRUE);
		}
		else
		{
			if (IsVirGroupMainId(qeidLogin,0,2))
			{
				bGet = Get(aQeid, 2, psSessionInfo,eSESSION_TYPE_VIRGROUP,qsidGroupID, TRUE);
				isVirGroupByNormal = 1;
			}
			else
			{
				bGet = Get(aQeid, 2, psSessionInfo,eSESSION_TYPE_VIRGROUP,0, TRUE);
				isVirGroupByNormal = 2;
			}
		}
	}

	if (bGet)
		return TRUE;
	else
	{
		QSID qsidSession = Eng.NewID(qeidRecver, qeidSender);
		memset(psSessionInfo, 0, sizeof(S_SessionInfo));
		psSessionInfo->CreaterId	= qeidSender;
		psSessionInfo->dwCreateTime = (DWORD)time(NULL);
		if (qsidGroupID != 0)
			psSessionInfo->eType = eSESSION_TYPE_VIRGROUP;
		else
			psSessionInfo->eType = eSESSION_TYPE_SINGLE;

		if (isVirGroupByNormal == 2)
		{
			psSessionInfo->qsid	= qsidGroupID;
			qsidGroupID = 0;
		}
		else if (isVirGroupByNormal == 1)
			psSessionInfo->qsid	= qsidSession;
		else
			psSessionInfo->qsid	= qsidSession;
		
		psSessionInfo->eFlag = eSEFLAG_SINGLE;

		return  SUCCEEDED(Set(psSessionInfo->qsid, aQeid, 2, TRUE, TRUE)) &&	// Add single session members
				SUCCEEDED(Set(psSessionInfo,qsidGroupID) &&
				GetSingleTitle(psSessionInfo->szTitle, psSessionInfo->qsid));
	}
}

///////////////////////////////////////////////////////////////////////////
//=============================================================================
//Function:     Get 
//Description:  Get a session information
//
//Parameter:
//	qsid            - Session to be get
//	psSessionInfo   - Return the result
//
//Return:
//		TRUE		- Succeeded
//		FALSE		- Failed, maybe not exist
//=============================================================================
BOOL C_eIMSessionMgr::Get(QSID qsid, PS_SessionInfo psSessionInfo) 
{
	CHECK_NULL_RET_(psSessionInfo, FALSE);
	CHECK_INIT_RET_(FALSE);

	const char*     pszSql  = m_pIDb->VMPrintf(kSelectSessionSid, qsid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return FALSE;

	return _GetRow(pITable, psSessionInfo);
}

BOOL C_eIMSessionMgr::GetEmpInfoByEmp(QEID qeid,PS_EmpInfoByEmp psEmpInfo)
{
	CHECK_NULL_RET_(psEmpInfo, FALSE);
	CHECK_INIT_RET_(FALSE);

	const char*     pszSql  = m_pIDb->VMPrintf(kSelectEmpInfoByEmp, qeid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	const unsigned char* pszValueA    = NULL;
	eIMString		szValue;

	while( pITable->Step() == SQLITE_ROW )
	{
		pszValueA = pITable->GetColText( 0 );							
		::eIMUTF8ToTChar((const char*)pszValueA, szValue);
		_tcsncpy(psEmpInfo->pszCode, szValue.c_str(), MAX_NAME -1);	

		pszValueA = pITable->GetColText(1);
		::eIMUTF8ToTChar((const char*)pszValueA, szValue);
		_tcsncpy(psEmpInfo->pszName, szValue.c_str(), MAX_NAME -1);	

		return TRUE;
	}

	return FALSE;
}

// Help function for Get by members
typedef struct tagVerifyMembers
{
	DWORD dwCount;
	DWORD dwIndex;
	PQEID qaSearch;
}S_VerifyMembers, *PS_VerifyMembers;

// For qsort
int C_eIMSessionMgr::Compare( const void *pArg1, const void* pArg2 )
{
	if ( *(DWORD*)pArg1 < *(DWORD*)pArg2)
		return -1;

	if ( *(DWORD*)pArg1 > *(DWORD*)pArg2)
		return 1;

	return 0;
}

// For EnumMember
int __stdcall C_eIMSessionMgr::SessionMemberCb(QEID qeid, void* pvUserData)
{
	PS_VerifyMembers psVM = (PS_VerifyMembers)pvUserData;
	if ( psVM == NULL || psVM->qaSearch == NULL)
		return 0;	// break;

	if ( psVM->dwIndex < psVM->dwCount && psVM->qaSearch[psVM->dwIndex++] == qeid )
		return 1;

	return 0;
}

int __stdcall C_eIMSessionMgr::SessionMemberLstCb(QEID qeid, void* pvUserData)
{
    VectSessionMember* pvectMember = (VectSessionMember*)pvUserData;
    if (0 != qeid)
    {
        pvectMember->push_back(qeid);
    }
    
    return 0;
}

//=============================================================================
//Function:     Get 
//Description:  Get a session information by members
//
//Parameter:
//	paQSID          - Session's members to find the QSID
//  dwCount			- Members count
//	psSessionInfo   - Return the result
//
//Return:
//		TRUE		- Succeeded
//		FALSE		- Failed, maybe not exist
//=============================================================================
BOOL C_eIMSessionMgr::Get(PQEID paQeid, DWORD dwCount, PS_SessionInfo psSessionInfo, int sessionType,QSID qsid,BOOL  bSingle,QEID  QOrderCreatorID/* = 0*/)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(paQeid, FALSE);
	CHECK_NULL_RET_(psSessionInfo, FALSE);
	
	// Prepare data for find 
	S_VerifyMembers sVM = { dwCount, 0, NULL };
	sVM.qaSearch = (PQEID)_alloca(sizeof(QEID) * dwCount);
	CHECK_NULL_RET_(sVM.qaSearch, FALSE);

	memcpy(sVM.qaSearch, paQeid, sizeof(QEID) * dwCount);	// copy to temp memory
	qsort(sVM.qaSearch, dwCount, sizeof(QEID), Compare);	// Sort by asc(The EnumMember also is asc by UID)

	const char* pszSql = NULL;
	if(QOrderCreatorID != 0)
	{
		pszSql = m_pIDb->VMPrintf(kSelectSessionGroupMembersOderCreator, QOrderCreatorID, dwCount, QOrderCreatorID, dwCount);
	}
	else
	{
		if (eSESSION_TYPE_SINGLE == sessionType)
		{
			pszSql = m_pIDb->VMPrintf(kSelectSessionGroupMembersSingle,sessionType,dwCount);
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kSelectSessionGroupMembers,sessionType,qsid, dwCount);
		}
	}

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		sVM.dwIndex = 0;
		if (dwCount == EnumMember(pITable->GetColInt64(0), &SessionMemberCb, &sVM))
		{
			BOOL bRet = Get(pITable->GetColInt64(0), psSessionInfo);	// Finded
			if ( bSingle )
			{
				if ( bRet && (psSessionInfo->eFlag & eSEFLAG_SINGLE))
					return bRet;
			}
			else
			{
				if( (psSessionInfo->eFlag & eSEFLAG_DELETE) != eSEFLAG_DELETE )
					return bRet;
			}
		}
	}

	return FALSE;
}

int C_eIMSessionMgr::DeleteSessionMsg()
{
	CHECK_INIT_RET_(0);

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_INFO_(pILog, _T("Delete Session")); 

	int i32Ret = 0; 

	/*i32Ret = m_pIDb->SQL(kDeleteSession_records);
	EIMLOGGER_INFO_(pILog, _T("DELETE FROM t_session_records :%d"),i32Ret);

	i32Ret = m_pIDb->SQL(kDeleteSession_emp);
	EIMLOGGER_INFO_(pILog, _T("DELETE FROM t_session_emp :%d"),i32Ret);

	i32Ret = m_pIDb->SQL(kDeleteSessionMsg);
	EIMLOGGER_INFO_(pILog, _T("DELETE FROM t_session :%d"),i32Ret);
	*/
	
	/*eIMString tt = _T("<MsgUI Type=\"0\" SenderId=\"268561\" SendTime=\"1477959829\" UserCode=\"chenzheng17\" IsSend=\"1\" Is1To100=\"0\" ReceNum=\"0\" Read=\"0\"><MsgFont Size=\"9\" Color=\"0xFF0000\" Id=\"0\" Mode=\"0\" /><Message><MsgFont Size=\"11\" Color=\"0x00000000\" Id=\"0\" Mode=\"0\" /><MsgText><![CDATA[咱们现在去九号楼吗？]]></MsgText></Message></MsgUI>");
	eIMStringA temp;
	eIMTChar2UTF8(tt.c_str(),temp);
	const char* const kchangemsg = "UPDATE t_session_records set msg='%s' , timestamp='%s' where msgid_='6954439796787595265';";
	const char* pszsql = m_pIDb->VMPrintf(kchangemsg,temp.c_str(),"1477959829");
	int i = m_pIDb->SQL(pszsql);
	m_pIDb->SQL("drop table t_meeting_basicinfo");
	m_pIDb->SQL("drop table t_meeting_member");
	m_pIDb->SQL("drop table t_meeting_file");*/

	/*TCHAR tt[30] = _T("咱们现在去");
	eIMStringA uu;
	eIMTChar2UTF8(tt,uu);
	const char* sqlpsz4 = "<MsgUI Type=\"0\" Read=\"0\" IsSend=\"0\" IsSelfSend=\"2\" SendTime=\"1477959867\" SenderId=\"187813\"><MsgFont Size=\"9\" Color=\"0xFF0000\" Id=\"0\" Mode=\"0\" /><Message><MsgFont Id=\"0\" Size=\"12\" Mode=\"0\" Color=\"0xFF005200\" /><MsgText><![CDATA[";
	const char* sqlosz6 = "<MsgUI Type=\"0\" SenderId=\"268561\" SendTime=\"1477961629\" UserCode=\"chenzheng17\" IsSend=\"1\" Is1To100=\"0\" ReceNum=\"0\" Read=\"0\"><MsgFont Size=\"9\" Color=\"0xFF0000\" Id=\"0\" Mode=\"0\" /><Message><MsgFont Size=\"11\" Color=\"0x00000000\" Id=\"0\" Mode=\"0\" /><MsgText><![CDATA[鍜变滑鐜板湪鍘讳節鍙锋ゼ鍚楋紵]]></MsgText></Message></MsgUI>";
	const char* sqlpsz5 = "]]></MsgText></Message></MsgUI>";
	
	const char* sqlpsz1 = "DELETE FROM  t_session_records WHERE msgid_=6954439796787595265;";
	const char* sqlpsz2 = m_pIDb->VMPrintf(sqlpsz1,sqlosz6);
	const char* const kSeleteMsg123 = "SELECT msg,timestamp,msgid_ FROM t_session_records WHERE msg like '%1477959829%';";
	int i = m_pIDb->SQL(sqlpsz2);
	return 0;
    I_SQLite3Table* pITable = m_pIDb->GetTable(kSeleteMsg123,false);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);
	while (pITable->Step() == SQLITE_ROW)
	{
		const char* msg = (const char*)pITable->GetColText(0);
		eIMString ff;
		eIMUTF8ToTChar(msg,ff);
		long yy= pITable->GetColInt(1);
		QSID hh= (QSID)pITable->GetColInt64(2);
	}*/
	
	return i32Ret;
}

int C_eIMSessionMgr::RepairMsg()
{
	CHECK_INIT_RET_(FALSE);

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_INFO_(pILog, _T("start RepairMsg")); 

	EIMLOGGER_INFO_(pILog, _T("init db success"));

	const char* pszSql = NULL;
	int i32Ret = 0; 

	i32Ret = m_pIDb->SQL(kDropTempTable);  //drop table if exists t_session_emp_temp
	EIMLOGGER_INFO_(pILog, _T("drop table if exists t_session_emp_temp :%d"),i32Ret);
 
	i32Ret = m_pIDb->SQL(kCreateTempTable);   //create table t_session_emp_temp
	EIMLOGGER_INFO_(pILog, _T("create table t_session_emp_temp :%d"),i32Ret);

	pszSql = m_pIDb->VMPrintf(kSelectSessionGroupMembersSingleRepair, Eng.GetPurview(GET_LOGIN_QEID));
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql,true);   //select sid_ eid_ from t_session_emp
	EIMLOGGER_INFO_(pILog, _T("select sid_ eid_ from t_session_emp "));
	CHECK_NULL_RET_(pITable,FALSE);

	EIMLOGGER_INFO_(pILog, _T("select sid_ eid_ from t_session_emp table not null"));

	m_pIDb->Begin();
	while( pITable->Step() == SQLITE_ROW )
	{
		EIMLOGGER_INFO_(pILog, _T("sid_:%lld <> eid_:%u"),pITable->GetColInt64(0),pITable->GetColInt(1));
		pszSql = m_pIDb->VMPrintf(kInsertTempTableValue,(INT64)pITable->GetColInt64(0),(int)pITable->GetColInt(1));
		i32Ret = m_pIDb->SQL(pszSql,true);
		EIMLOGGER_INFO_(pILog, _T("insert data into t_session_emp_temp :::: sid_:%lld <> eid_:%u <>return:%d"),pITable->GetColInt64(0),pITable->GetColInt(1),i32Ret);
	}
	m_pIDb->Commit();
	pITable->Release();
	EIMLOGGER_INFO_(pILog, _T("insert data into t_session_emp_temp :%d"),i32Ret);

	I_SQLite3Table* pISingleTable = m_pIDb->GetTable(kGetSameEidTempTable);
	EIMLOGGER_INFO_(pILog, _T("select repeating eid_ from t_session_emp_temp "));
	CHECK_NULL_RET_(pISingleTable,FALSE);
	AUTO_RELEASE_(pISingleTable);

	m_pIDb->Begin();
	while( pISingleTable->Step() == SQLITE_ROW )
	{
		EIMLOGGER_INFO_(pILog, _T("select sid_,utime from t_session"));
		pszSql = m_pIDb->VMPrintf(kGetSameSidTempTable,pISingleTable->GetColInt(0));
		I_SQLite3Table* pITableQsid = m_pIDb->GetTable(pszSql,true);
		CHECK_NULL_RET_(pITableQsid,FALSE);
		AUTO_RELEASE_(pITableQsid);

		int uutime = -1;
		QSID sid;

		while( pITableQsid->Step() == SQLITE_ROW )
		{
			if (uutime >0)
			{
				pszSql = m_pIDb->VMPrintf(kUpdateSession,pITableQsid->GetColInt(1),sid);
				i32Ret = m_pIDb->SQL(pszSql,true);
				EIMLOGGER_INFO_(pILog, _T("update utime where sid_ is old :%d"),i32Ret);

				pszSql = m_pIDb->VMPrintf(kDeleteSession,pITableQsid->GetColInt64(0));
				i32Ret = m_pIDb->SQL(pszSql,true);
				EIMLOGGER_INFO_(pILog, _T("delete new create sid_ from t_session :%d"),i32Ret);

				pszSql = m_pIDb->VMPrintf(kDeleteSessionEmp,pITableQsid->GetColInt64(0));
				i32Ret = m_pIDb->SQL(pszSql,true);
				EIMLOGGER_INFO_(pILog, _T("delete new create sid_ from t_session_emp :%d"),i32Ret);

				pszSql = m_pIDb->VMPrintf(kUpdateSessionRecord,sid,pITableQsid->GetColInt64(0));
				i32Ret = m_pIDb->SQL(pszSql,true);
				EIMLOGGER_INFO_(pILog, _T("update t_session_records :%d"),i32Ret);
			}
			else
			{
				sid = pITableQsid->GetColInt64(0);
				uutime = pITableQsid->GetColInt(1);
				EIMLOGGER_INFO_(pILog, _T("sid_:%lld <> utime:%d"),sid,uutime);
			}
		}
	}
	m_pIDb->Commit();

	i32Ret = m_pIDb->SQL(kDropTempTable);

	EIMLOGGER_INFO_(pILog, _T("drop table t_session_emp_temp :%d"),i32Ret);

	Uninit();

	return EIMERR_NO_ERROR;
}

BOOL C_eIMSessionMgr::Get(UINT32* update_time)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kvirtualGroupUpdatetime);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		*update_time  = pITable->GetColInt(0);
	}
	if (update_time < 0)
	{
		*update_time = 0;
	}
	return TRUE;
}

//=============================================================================
//Function:     SetTitle
//Description:	Set title information
//
//Parameter:
//	qsid        - Set session's title by QSID 
//	pszValue    - Remark value
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMSessionMgr::SetTitle(QSID  qsid, const TCHAR* pszValue)
{
	return Set(qsid, eSET_TYPE_TITLE, 0, pszValue);
}

//=============================================================================
//Function:     SetRemark
//Description:	Set remark information
//
//Parameter:
//	qsid        - Set session's remark by QSID 
//	pszValue    - Remark value
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMSessionMgr::SetRemark(QSID  qsid, const TCHAR* pszValue)
{
	return Set(qsid, eSET_TYPE_REMARK, 0, pszValue);
}

//=============================================================================
//Function:     Remove
//Description:	Remove a session
//
//Parameter:
//	qsid		- The session to be removed by QSID
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMSessionMgr::Remove(QSID qsid)
{
	return UpdateFlag(qsid, eSEFLAG_REMOVE, TRUE);
}

DWORD C_eIMSessionMgr::SearchSession( DWORD timestamp, int sessionCount, PFN_SessionCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	if (timestamp < 0)
		return 0;

	int dwCount = 0;
	const char* pszSql = NULL;

	if (timestamp == 0)
		pszSql = m_pIDb->VMPrintf(kSelectSessionList1,sessionCount);
	else
		pszSql = m_pIDb->VMPrintf(kSelectSessionList2,timestamp,sessionCount);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_SessionInfo sInfo = { 0 };
	eIMString szValue = _T("");
	while(SQLITE_ROW == pITable->Step())
	{
		_GetRow(pITable, &sInfo);
		
		if ( sInfo.eType == eSESSION_TYPE_SINGLE && sInfo.szTitle[0] == _T('\0') )
			GetSingleTitle(sInfo.szTitle, sInfo.qsid);

		//去除标题为空的会话
		if (sInfo.szTitle[0] == '\0')
			continue;
		//除去消息为空的会话
		if(GetSessionMsgCount(sInfo.qsid) <= 0)
			continue;

		if (sInfo.eType == eSESSION_TYPE_SINGLE)
		{
			//保存单聊人员的性别
			if (GetSingleSessionMail(sInfo.qsid) == 1)
				sInfo.dwSingleSessionSex = 1;
			else
				sInfo.dwSingleSessionSex = 0;

			if (sInfo.dwTopTime < 0)
				continue;
		}

		dwCount++;
		if (! pfnCb(&sInfo, pvUserData) )
			return dwCount;
	}

	return 0;
}

int C_eIMSessionMgr::GetSessionMsgCount(QSID ssid)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectSessionMsgCount,ssid);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while(SQLITE_ROW == pITable->Step())
	{
		return pITable->GetColInt(0);
	}

	return -1;
}

int C_eIMSessionMgr::GetSingleSessionMail(QSID sid)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectSingleMailBySid,sid, Eng.GetPurview(GET_LOGIN_QEID));

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while(SQLITE_ROW == pITable->Step())
	{
		return pITable->GetColInt(0);
	}

	return -1;
}

DWORD C_eIMSessionMgr::SearchSession(PS_SessionEnum psCond,PFN_SessionCb pfnCb,void* pvUserData)
{
	E_SessionType sessiontype = psCond->eType;//Save temp session type.

	CHECK_INIT_RET_(0);
	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearchConts = NULL;
	const char* pszSearchContent = NULL;

	// 标题搜索 
	if ( psCond->eOperate & eSession_OP_CONTS )
	{
		::eIMTChar2UTF8(psCond->szContacts, szContsA);
		pszSearchConts = m_pIDb->VMPrintf(kNewSelectTitle, szContsA.c_str());
	}

	//内容搜索
	if (psCond->eOperate & eSession_OP_TEXT)
	{
		::eIMTChar2UTF8(psCond->szContent, szTextA);
		pszSearchContent = m_pIDb->VMPrintf(kNewRecordsContentLike, szTextA.c_str());
	}
	
	// 时间过滤
	const char* pszDateTime = NULL;
	if ( psCond->eOperate & eSession_OP_DATE )
	{
		pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where timestamp=%u ", psCond->dwStart);
		if (psCond->eOperate & eSession_OP_TEXT)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf(kNewRecordsAddContentLike, szTextA.c_str());
		}
		else
		{
			pszSearchContent = m_pIDb->VMPrintf(" )");
		}
	}
	else if ( psCond->eOperate & eSession_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			if(psCond->eOperate & eSession_OP_TEXT)
			{
				::eIMTChar2UTF8(psCond->szContent, szTextA);
				pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where t_session_records.msg like '%%%q%%' )", szTextA.c_str());
			}
			pszSearchContent = m_pIDb->VMPrintf("");
		}
		else
		{
			pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where timestamp>=%u ", psCond->dwStart);
			if (psCond->eOperate & eSession_OP_TEXT)
			{
				::eIMTChar2UTF8(psCond->szContent, szTextA);
				pszSearchContent = m_pIDb->VMPrintf(kNewRecordsAddContentLike, szTextA.c_str());
			}
			else
			{
				pszSearchContent = m_pIDb->VMPrintf(" )");
			}
		}
	}
	else if ( psCond->eOperate & eSession_OP_BETWEEN_DATE )
	{
		pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where timestamp>=%u AND timestamp<=%u ", psCond->dwStart, psCond->dwEnd);
		if (psCond->eOperate & eSession_OP_TEXT)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf(kNewRecordsAddContentLike, szTextA.c_str());
		}
		else
		{
			pszSearchContent = m_pIDb->VMPrintf(" )");
		}
	}

	DWORD		dwCount     = 0;
	const char* pszSql = NULL;
	
	if (sessiontype == eSESSION_TYPE_ALL)
	{
		pszSql = m_pIDb->VMPrintf(kNewSelectRecords4,pszSearchConts, pszDateTime, pszSearchContent, kNewRecordsOrderBy);
	}
	else if (sessiontype ==  eSESSION_TYPE_SINGLE)
	{
		pszSql = m_pIDb->VMPrintf(kNewSelectRecords3, sessiontype, eSESSION_TYPE_1TO_TEN_THOUSAND,eSESSION_TYPE_VIRGROUP, pszSearchConts, pszDateTime, pszSearchContent, kNewRecordsOrderBy);
	}
	else
	{
		pszSql = m_pIDb->VMPrintf(kNewSelectRecords2, sessiontype, eSESSION_TYPE_1TO_TEN_THOUSAND,pszSearchConts, pszDateTime, pszSearchContent, kNewRecordsOrderBy);
	}

	if(pszSearchConts)
		m_pIDb->Free((void*)pszSearchConts);
	if(pszDateTime)
		m_pIDb->Free((void*)pszDateTime);
	if(pszSearchContent)
		m_pIDb->Free((void*)pszSearchContent);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_SessionInfo sInfo = { 0 };
	DWORD dwMsgCount = 0;
	S_MsgEnum rGetEnum;
	while(SQLITE_ROW == pITable->Step())
	{

       // int iFlag = pITable->GetColInt(2);
        //if(iFlag & eSEFLAG_DELETE)
            //continue;

		_GetRow1(pITable, &sInfo);
		
		//去除标题为空的会话
		if (sInfo.szTitle[0] == '\0')
			continue;

		rGetEnum.eOperate = (E_MsgOperate)0;
		rGetEnum.eType = sInfo.eType;
		rGetEnum.rQSID = sInfo.qsid;

		if (psCond->eOperate & eSession_OP_BETWEEN_DATE)
		{
			rGetEnum.eOperate = (E_MsgOperate)(rGetEnum.eOperate | eMSG_OP_BETWEEN_DATE);
			rGetEnum.dwStart = psCond->dwStart;
			rGetEnum.dwEnd = psCond->dwEnd;
		}

		if (psCond->szContent && psCond->szContent[0] != '\0')
		{
			_tcscpy(rGetEnum.szContent,psCond->szContent);
			rGetEnum.eOperate = (E_MsgOperate)(rGetEnum.eOperate | eMSG_OP_TEXT);
		}
		dwMsgCount = SearchMsgCount(&rGetEnum);

		//去除消息为空的
		if (dwMsgCount <= 0)
			continue;

		sInfo.dwAllMsg = dwMsgCount;

		if (sInfo.eType == eSESSION_TYPE_SINGLE)
		{
			//保存单聊人员的性别
			if (GetSingleSessionMail(sInfo.qsid) == 1)
				sInfo.dwSingleSessionSex = 1;
			else
				sInfo.dwSingleSessionSex = 0;

			if (sInfo.dwTopTime < 0)
				continue;
		}

		if ( sInfo.eType == eSESSION_TYPE_SINGLE && sInfo.szTitle[0] == _T('\0') )
			GetSingleTitle(sInfo.szTitle, sInfo.qsid);

		dwCount++;
		if (! pfnCb(&sInfo, pvUserData) )
			return dwCount;
	}

	return dwCount;
}

DWORD C_eIMSessionMgr::SearchSession(PS_SessionEnum	psCond, E_SessionType eSessionType2, 
	PFN_NewSessionCb pfnCb, void* pvUserData)
{
	E_SessionType sessiontype = psCond->eType;//Save temp session type.

	CHECK_INIT_RET_(0);
	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearchConts = NULL;
	const char* pszSearchContent = NULL;

	// 账号搜索 
	if ( psCond->eOperate & eSession_OP_CONTS )
	{
		::eIMTChar2UTF8(psCond->szContacts, szContsA);
		pszSearchConts = m_pIDb->VMPrintf(kNewRecordsAddAccountLike, szContsA.c_str(), szContsA.c_str());
	}

	// 时间过滤
	const char* pszDateTime = NULL;
	if ( psCond->eOperate & eSession_OP_DATE )
	{
		pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where timestamp=%u ", psCond->dwStart);
		if (psCond->eOperate & eSession_OP_TEXT)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf(kNewRecordsAddContentLike, szTextA.c_str());
		}
		else
		{
			pszSearchContent = m_pIDb->VMPrintf(" )");
		}
	}
	else if ( psCond->eOperate & eSession_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			if(psCond->eOperate & eSession_OP_TEXT)
			{
				::eIMTChar2UTF8(psCond->szContent, szTextA);
				pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where t_session_records.msg like '%%%q%%' )", szTextA.c_str());
			}
			pszSearchContent = m_pIDb->VMPrintf("");
		}
		else
		{
			pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where timestamp>=%u ", psCond->dwStart);
			if (psCond->eOperate & eSession_OP_TEXT)
			{
				::eIMTChar2UTF8(psCond->szContent, szTextA);
				pszSearchContent = m_pIDb->VMPrintf(kNewRecordsAddContentLike, szTextA.c_str());
			}
			else
			{
				pszSearchContent = m_pIDb->VMPrintf(" )");
			}
		}
	}
	else if ( psCond->eOperate & eSession_OP_BETWEEN_DATE )
	{
		pszDateTime = m_pIDb->VMPrintf("AND sid_ in(select distinct sid  from t_session_records where timestamp>=%u AND timestamp<=%u ", psCond->dwStart, psCond->dwEnd);
		if (psCond->eOperate & eSession_OP_TEXT)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf(kNewRecordsAddContentLike, szTextA.c_str());
		}
		else
		{
			pszSearchContent = m_pIDb->VMPrintf(" )");
		}
	}

	DWORD		dwCount     = 0;
	const char* pszSql = NULL;
	if(pszSearchConts)
	{
		pszSql = m_pIDb->VMPrintf(kNewSelectRecords2, sessiontype, eSessionType2, pszSearchConts, pszDateTime, pszSearchContent, kNewRecordsOrderBy);
	}
	else
	{
		pszSql = m_pIDb->VMPrintf(kNewSelectRecords2, sessiontype, eSessionType2, "", pszDateTime, pszSearchContent, kNewRecordsOrderBy);
	}


	if(pszSearchConts)
		m_pIDb->Free((void*)pszSearchConts);
	if(pszDateTime)
		m_pIDb->Free((void*)pszDateTime);
	if(pszSearchContent)
		m_pIDb->Free((void*)pszSearchContent);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_SessionInfo sInfo = { 0 };
	while(SQLITE_ROW == pITable->Step())
	{
        int iFlag = pITable->GetColInt(2);
        //if(iFlag & eSEFLAG_DELETE)
           // continue;
		dwCount++;
		if (! pfnCb(pITable->GetColInt64( 0 ), pvUserData) )
			return dwCount;
	}

	return dwCount;
}

//=============================================================================
//Function:     Enum
//Description:	Enumerate sessions by eFlag
//
//Parameter:
//	eFlag         - Flag to enumerate of sessions
//	pfnCb         - Callback function to receive the result, return 0 to break;
//	pvUserData    - User defined data, pass to pfnCb
//
//Return:
//		The count of enumerated sessions 
//=============================================================================
DWORD C_eIMSessionMgr::Enum( E_SEFlag eFlag, PFN_SessionCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);

	const char* pszOrderBy = "DESC";
	const char* pszOrderName = "utime";
	switch(eFlag & eSEFLAG_ORDER_MASK)
	{
	case eSEFLAG_ASC_UTIME:
		pszOrderBy   = "ASC";
		break;
	case eSEFLAG_DESC_CTIME:
		pszOrderName = "ctime";
		break;
	case eSEFLAG_ASC_CTIME:
		pszOrderBy   = "ASC";
		pszOrderName = "ctime";
		break;
	}

	E_SEFlag eSTFlag = eFlag;//Save temp session type.

	if(!(eFlag & eSEFLAG_USER))
	{
		if (!(eFlag & eSEFLAG_TYPE_MASK))
			eSTFlag = eSEFLAG_TYPE_MASK;	// All type
		else
			eSTFlag = (E_SEFlag)(eFlag & eSEFLAG_TYPE_MASK);
	}
	else
	{
		eSTFlag = (E_SEFlag)(eSEFLAG_CUSTOM_MASK|eFlag);
	}

	DWORD		dwCount     = 0;
	const char* pszSql      = m_pIDb->VMPrintf(kSelectSessionFlag, eSTFlag , pszOrderName, pszOrderBy);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_SessionInfo sInfo = { 0 };
	while(SQLITE_ROW == pITable->Step())
	{
		_GetRow(pITable, &sInfo);
		if ( sInfo.eFlag & eSEFLAG_DELETE )
			continue;

		if (sInfo.dwUpdateTime == 0)
			continue;

		if ((eFlag & eSEFLAG_OFFLINE) && (!(sInfo.eFlag & eSEFLAG_OFFLINE) || sInfo.dwOfflineMsg == 0 ))
			continue;

		if ((eFlag & eSEFLAG_NEW) && (!(sInfo.eFlag & eSEFLAG_NEW) || sInfo.dwNewMsg == 0 ))
			continue;

        VectSessionMember vectSessionMember;

		//屏蔽掉离职人员发的离线消息
		if (eFlag & eSEFLAG_OFFLINE)
			EnumMemberOffMsg(sInfo.qsid,vectSessionMember);
		else
			EnumMember(sInfo.qsid, &SessionMemberLstCb, &vectSessionMember);

        if(vectSessionMember.size() < 1)
        {
            UpdateFlag(sInfo.qsid, eSEFLAG_DELETE);
            continue;
        }

		if ((eFlag & eSEFLAG_OFFLINE) && vectSessionMember.size() == 1)
			continue;

		if ( sInfo.eType == eSESSION_TYPE_SINGLE && sInfo.szTitle[0] == _T('\0') )
			GetSingleTitle(sInfo.szTitle, sInfo.qsid);

		dwCount++;
		if (! pfnCb(&sInfo, pvUserData) )
			return dwCount;
	}

	return dwCount;
}

//=============================================================================
	//Function:     Enum
	//Description:	Enumerate sessions by eFlag   union one eFlag
	//
	//Parameter:
	//	eFlag         - Flag to enumerate of sessions
	//	eFlag2        - Flag to enumerate of sessions
	//	pfnCb         - Callback function to receive the result, return 0 to break;
	//	pvUserData    - User defined data, pass to pfnCb
	//
	//Return:
	//		The count of enumerated sessions 
	//=============================================================================
DWORD C_eIMSessionMgr::Enum( E_SEFlag eFlag, E_SEFlag eFlag2,PFN_SessionCb pfnCb, void*  pvUserData)
{
	CHECK_INIT_RET_(0);

	const char* pszOrderBy = "DESC";
	const char* pszOrderName = "utime";
	switch(eFlag & eSEFLAG_ORDER_MASK)
	{
	case eSEFLAG_ASC_UTIME:
		pszOrderBy   = "ASC";
		break;
	case eSEFLAG_DESC_CTIME:
		pszOrderName = "ctime";
		break;
	case eSEFLAG_ASC_CTIME:
		pszOrderBy   = "ASC";
		pszOrderName = "ctime";
		break;
	}

	E_SEFlag eSTFlag = eFlag;//Save temp session type.

	if(!(eFlag & eSEFLAG_USER))
	{
		if (!(eFlag & eSEFLAG_TYPE_MASK))
			eSTFlag = eSEFLAG_TYPE_MASK;	// All type
		else
			eSTFlag = (E_SEFlag)(eFlag & eSEFLAG_TYPE_MASK);
	}
	else
	{
		eSTFlag = (E_SEFlag)(eSEFLAG_CUSTOM_MASK|eFlag);
	}

	E_SEFlag eSTFlag2 = eFlag2;//Save temp session type.
	if(!(eFlag2 & eSEFLAG_USER))
	{
		if (!(eFlag2 & eSEFLAG_TYPE_MASK))
			eSTFlag2 = eSEFLAG_TYPE_MASK;	// All type
		else
			eSTFlag2 = (E_SEFlag)(eSTFlag2 & eSEFLAG_TYPE_MASK);
	}
	else
	{
		eSTFlag2 = (E_SEFlag)(eSEFLAG_CUSTOM_MASK|eFlag2);
	}

	DWORD		dwCount     = 0;
	const char* pszSql      = m_pIDb->VMPrintf(kSelectSessionFlag2, eSTFlag,eSTFlag2, pszOrderName, pszOrderBy);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_SessionInfo sInfo = { 0 };
	while(SQLITE_ROW == pITable->Step())
	{
		_GetRow(pITable, &sInfo);

		if (sInfo.dwUpdateTime == 0)
			continue;

		if ((eFlag & eSEFLAG_OFFLINE) && (!(sInfo.eFlag & eSEFLAG_OFFLINE) || sInfo.dwOfflineMsg == 0 ))
			continue;

		if ((eFlag & eSEFLAG_NEW) && (!(sInfo.eFlag & eSEFLAG_NEW) || sInfo.dwNewMsg == 0 ))
			continue;

		if ( sInfo.eType == eSESSION_TYPE_SINGLE && sInfo.szTitle[0] == _T('\0') )
			GetSingleTitle(sInfo.szTitle, sInfo.qsid);

		dwCount++;
		if (! pfnCb(&sInfo, pvUserData) )
			return dwCount;
	}

	return dwCount;
}

//=============================================================================
//Function:     EnumMember
//Description:	Enumerate session's members
//
//Parameter:
//	qsid          - Session ID
//	pfnCb         - Callback function to receive the result, return 0 to break;
//	pvUserData    - User defined data, pass to pfnCb
//
//Return:
//		The count of enumerated members 
//=============================================================================
DWORD C_eIMSessionMgr::EnumMember(QSID qsid, PFN_SessionMemberCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnCb, 0);

	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectSessionMembers, qsid, eSEFLAG_DELETE);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	C_eIMContacts& c = C_eIMContacts::GetObject();
	// Get the session members
	while(SQLITE_ROW == pITable->Step())
	{
		QEID qeid = (QEID)pITable->GetColInt(0);
		if ( !c.GetEmp(qeid) )
			continue;	// Filter the staff who leave

		if (! pfnCb(qeid, pvUserData) )
			return dwCount;
		dwCount++;
	}

	return dwCount;
}

int C_eIMSessionMgr::EnumMemberOffMsg(QSID qsid,VectSessionMember& vectSessionMember)
{
	CHECK_INIT_RET_(0);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectSessionMembers, qsid, eSEFLAG_DELETE);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	EIMLOGGER_INFO_(pILog, _T("Get OffMsg Member sid:%lld"),qsid);
	C_eIMContacts& c = C_eIMContacts::GetObject();
	// Get the session members
	while(SQLITE_ROW == pITable->Step())
	{
		QEID qeid = (QEID)pITable->GetColInt(0);
		if ( !c.GetEmp(qeid) )
			continue;	// Filter the staff who leave
		
		vectSessionMember.push_back(qeid);
		dwCount++;
	}

	EIMLOGGER_INFO_(pILog, _T("Get OffMsg Member sid:%lld Count:%d"),qsid,dwCount);
	return dwCount;
}

DWORD C_eIMSessionMgr::EnumMemberAll(QSID qsid, PFN_SessionMemberCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnCb, 0);

	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectSessionAllMembers, qsid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	C_eIMContacts& c = C_eIMContacts::GetObject();
	// Get the session members
	while(SQLITE_ROW == pITable->Step())
	{
		QEID qeid = (QEID)pITable->GetColInt(0);
		//if ( !c.GetEmp(qeid) )
		//continue;	// Filter the staff who leave

		if (! pfnCb(qeid, pvUserData) )
			return dwCount;
		dwCount++;
	}

	return dwCount;
}

DWORD C_eIMSessionMgr::RgroupMember(QMID qmid, PFN_SessionMemberCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnCb, 0);

	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRgroupAdmines, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	// Get the session members
	while(SQLITE_ROW == pITable->Step())
	{
		QEID qeid = (QEID)pITable->GetColInt(0);
		int i32attrib = (int)pITable->GetColInt(1);
		if (i32attrib & eSEFLAG_ADMINE)
		{//i32attrib & 
			if (! pfnCb(qeid, pvUserData) )
				return dwCount;
		}
		dwCount++;
	}

	return dwCount;
}

BOOL C_eIMSessionMgr::GetImageMore(const TCHAR* filePath,QSID SessionId,QMID Msgid,PFN_ImageMore pfnCb,void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnCb, 0);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	eIMStringA szfilePath;
	eIMTChar2UTF8(filePath,szfilePath);
	int imageMoreNum = hpi.UIGetAttributeInt(PATH_SETTING_BASIC,SETTING_BASIC_ATTR_IMAGEMORENUM,20);

	int i32Ret = m_pIDb->SQL(kDropMsgTempTable);
	i32Ret = m_pIDb->SQL(kCreateMsgTempTable);   //create table t_session_emp_temp
	EIMLOGGER_INFO_(pILog, _T("create table t_session_msg_temp :%d"),i32Ret);

	const char*	pszSql = m_pIDb->VMPrintf(kSelectImage1,">=",Msgid,SessionId,"asc",200);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql,true);   //select sid_ eid_ from t_session_emp
	EIMLOGGER_INFO_(pILog, _T("select sid_ eid_ from t_session_emp "));
	CHECK_NULL_RET_(pITable,FALSE);
	AUTO_RELEASE_(pITable);

	m_pIDb->Begin();
	while( pITable->Step() == SQLITE_ROW )
	{
		pszSql = m_pIDb->VMPrintf(kInsertIntoMsgTempTable,pITable->GetColText(0));
		i32Ret = m_pIDb->SQL(pszSql,true);
		EIMLOGGER_INFO_(pILog, _T("insert data into t_session_emp_temp :::: sid_:%lld <> eid_:%u <>return:%d"),pITable->GetColInt64(0),pITable->GetColInt(1),i32Ret);
	}
	m_pIDb->Commit();
	
	pszSql = m_pIDb->VMPrintf(kSelectImage2,imageMoreNum);
	I_SQLite3Table* pITable1 = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable1, 0);
	AUTO_RELEASE_(pITable1);

	eIMString tempmsg;
	eIMString tempfilePath;
	const unsigned char* pszValueA    = NULL;
	int count = 1;
	FILE* fh;

	while(SQLITE_ROW == pITable1->Step())
	{
		pszValueA = pITable1->GetColText(0);								
		::eIMUTF8ToTChar((const char*)pszValueA, tempmsg);

		GetFilePathByMsg(tempmsg,tempfilePath);

		fh = _wfopen(tempfilePath.c_str(),_T("r"));
		if (fh)
		{
			fclose(fh);
			pfnCb(tempfilePath,0,pvUserData);
			count ++;
		}
	}
	
	if (count < imageMoreNum)
	{
		m_pIDb->SQL(kClearMsgTempTable);

		pszSql = m_pIDb->VMPrintf(kSelectImage1,"<",Msgid,SessionId,"desc",200);
		I_SQLite3Table* pITable2 = m_pIDb->GetTable(pszSql, true);
		CHECK_NULL_RET_(pITable2, 0);
		AUTO_RELEASE_(pITable2);

		m_pIDb->Begin();
		while( pITable2->Step() == SQLITE_ROW )
		{
			pszSql = m_pIDb->VMPrintf(kInsertIntoMsgTempTable,pITable2->GetColText(0));
			i32Ret = m_pIDb->SQL(pszSql,true);
		}
		m_pIDb->Commit();

		pszSql = m_pIDb->VMPrintf(kSelectImage2,imageMoreNum-count);
		I_SQLite3Table* pITable3 = m_pIDb->GetTable(pszSql, true);
		CHECK_NULL_RET_(pITable3, 0);
		AUTO_RELEASE_(pITable3);

		while(SQLITE_ROW == pITable3->Step())
		{
			pszValueA = pITable3->GetColText(0);								
			::eIMUTF8ToTChar((const char*)pszValueA, tempmsg);

			GetFilePathByMsg(tempmsg,tempfilePath);

			fh = _wfopen(tempfilePath.c_str(),_T("r"));
			if (fh)
			{
				fclose(fh);
				pfnCb(tempfilePath,1,pvUserData);
				count ++;
			}
		}
	}

	m_pIDb->SQL(kDropMsgTempTable);
	return 0;
}

void C_eIMSessionMgr::GetFilePathByMsg(eIMString& msg,eIMString& filepath)
{
	eIMStringA	  szXmlA;
	::eIMTChar2UTF8(msg.c_str(), szXmlA);

	TiXmlDocument doc;
	bool bSuccessLoad = false;
	doc.Parse(szXmlA.c_str());
	bSuccessLoad = !doc.Error(); 

	if (bSuccessLoad)
	{
		TiXmlElement* pElementroot = doc.RootElement();
		const char* pName = pElementroot->Value();
		if(pName && strcmp(pName, FIELD_MSGUI) == 0)
		{
			TiXmlHandle	h(pElementroot);
			TiXmlElement* pFileElement = h.FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGIMAGE).ToElement();
			if(pFileElement)
			{
				eIMStringA szNameA;
				::eIMTChar2UTF8(_T(FIELD_FILE_PATH), szNameA);
				const char* pszValue = pFileElement->Attribute(szNameA.c_str());

				eIMString sVal;
				if(pszValue == '\0' || pszValue[0] == '\0')
				{
					filepath = _T("");
				}
				else
				{
					::eIMUTF8ToTChar(pszValue, filepath);
				}
			}
		}
	}
}

BOOL C_eIMSessionMgr::GetAutoGroupTitle( PFN_GroupTitleCb pfnCb, void* pvUserData )
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pfnCb, FALSE);

	TCHAR sTempName[20] = _T("新建讨论组");

	eIMStringA strTempName;
	::eIMTChar2UTF8(sTempName,strTempName);

	const char*	pszSql  = m_pIDb->VMPrintf(kSelectGroupTitle,strTempName.c_str());
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	int Num = 0;
	// Get the GroupTitle
	while(SQLITE_ROW == pITable->Step())
	{
		TCHAR* szTempTitle;
		for (int i = 0;i < pITable->GetCols();i++)
		{
			szTempTitle = (TCHAR*)pITable->GetColText16(i);
			eIMStringA tempstrTitle;
			eIMTChar2UTF8(szTempTitle,tempstrTitle);
			eIMString tempTitle = szTempTitle;
			eIMString strNum = tempTitle.substr(6,tempTitle.length()-1);
			int i32Num = StrToInt(strNum.c_str());

			if (i32Num > Num)
			{
				Num = i32Num;
			}
		}
	}
	pfnCb(Num+1,pvUserData);

	return Num > 0;
	
}

int __stdcall C_eIMSessionMgr::SingleMemberCb(QEID qeid, void* pvUserData)
{
	if ( *(QEID*)pvUserData != qeid)
	{
		*(QEID*)pvUserData = qeid;
		return 0;
	}

	return 1;
}

int C_eIMSessionMgr::SendMsgRead(QSID qsid)
{
	CHECK_INIT_RET_(0);
	S_SessionInfo sSInfo = { 0 };
	MSG_READ_SYNC sMsgRead = { Eng.GetPurview(GET_LOGIN_QEID), IME_EMP_TERM_PC, 1 };
	if(Get(qsid, &sSInfo)== FALSE)
	{
		return 0;
	}
	
	sMsgRead.aSessionData[0].dwTimestamp = sSInfo.dwUpdateTime;
	if ( sSInfo.eType == eSESSION_TYPE_SINGLE )
	{
		sMsgRead.aSessionData[0].dwUserID = sMsgRead.dwUserID;
		sMsgRead.aSessionData[0].cType = 1;
		EnumMember(qsid, SingleMemberCb, &sMsgRead.aSessionData[0].dwUserID);
	}
	else
	{
		sMsgRead.aSessionData[0].cType = 2;
		GET_QSID_ASTR_(qsid, sMsgRead.aSessionData[0].aszGroupID);
	}

	return Eng._AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_READ_MSG_SYNC_REQ, &sMsgRead);
}

//=============================================================================
//Function:     Get 
//Description:  Get a session information by members
//
//Parameter:
//	paQSID          - Session's members to find the QSID
//  dwCount			- Members count
//	psSessionInfo   - Return the result
//
//Return:
//		TRUE		- Succeeded
//		FALSE		- Failed, maybe not exist
//=============================================================================
BOOL C_eIMSessionMgr::Get1To10T(PQEID paQeid, DWORD dwCount, PS_SessionInfo psSessionInfo)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(paQeid, FALSE);
	CHECK_NULL_RET_(psSessionInfo, FALSE);

	// Prepare data for find 
	S_VerifyMembers sVM = { dwCount, 0, NULL };
	sVM.qaSearch = (PQEID)_alloca(sizeof(QEID) * dwCount);
	CHECK_NULL_RET_(sVM.qaSearch, FALSE);

	memcpy(sVM.qaSearch, paQeid, sizeof(QEID) * dwCount);	// copy to temp memory
	qsort(sVM.qaSearch, dwCount, sizeof(QEID), Compare);	// Sort by asc(The EnumMember also is asc by UID)

	const char* pszSql = NULL;
	pszSql = m_pIDb->VMPrintf(kSelectSessionGroupMembers, dwCount);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	while(pITable->Step() == SQLITE_ROW)
	{
		sVM.dwIndex = 0;
		if (dwCount == EnumMember(pITable->GetColInt64(0), &SessionMemberCb, &sVM))
		{
			BOOL bRet = Get(pITable->GetColInt64(0), psSessionInfo);	// Finded
			if ( bRet && (psSessionInfo->eFlag & eSEFLAG_1TOTH))
				return bRet;
		}
	}

	return FALSE;
}

BOOL C_eIMSessionMgr::DeleteAllSession()
{
    CHECK_INIT_RET_(FALSE);
    const char* const kUpdateSessionFlag = "UPDATE t_session SET flag=flag|%d ";
    const char* pszSql = m_pIDb->VMPrintf(kUpdateSessionFlag , eSEFLAG_DELETE);
    int i32Ret = m_pIDb->SQL(pszSql, true);
    return SQLITE_OK == i32Ret;
}

BOOL C_eIMSessionMgr::DeleteByTime(DWORD dwDelTime)
{
    CHECK_INIT_RET_(FALSE);
    const char* const kSelectSessionTime = "SELECT sid_  FROM t_session WHERE ctime<=%d;";
    const char* pszSql      = m_pIDb->VMPrintf(kSelectSessionTime, dwDelTime);
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);
    while(SQLITE_ROW == pITable->Step())
    {
        QSID qSid = pITable->GetColInt64(0);
        Remove(qSid);
    }
    return TRUE;
}

BOOL C_eIMSessionMgr::EnumQSIDbyTime(DWORD dwTime, PFN_NewSessionCb pfnCb, void*		  pvUserData)
{
    CHECK_INIT_RET_(FALSE);
    const char* const kSelectSessionTime = "SELECT sid_  FROM t_session WHERE ctime<=%d;";
    const char* pszSql      = m_pIDb->VMPrintf(kSelectSessionTime, dwTime);
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);
    while(SQLITE_ROW == pITable->Step())
    {
        QSID qSid = pITable->GetColInt64(0);
        if(!pfnCb(qSid, pvUserData))
        {
            break;
        }
    }
    return TRUE;
}

int C_eIMSessionMgr::Save2File()
{
	CHECK_INIT_RET_(0);
    int i32Ret = m_pIDb->SQL(kWalCheckpoint, false);
    return (i32Ret == SQLITE_OK);
}

BOOL C_eIMSessionMgr::getQsidByMsgid(QSID msgid, QSID& sid)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql      = m_pIDb->VMPrintf(kSelectRecordsQmid, msgid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	while(SQLITE_ROW == pITable->Step())
	{
		sid = pITable->GetColInt64(0);
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMSessionMgr::CheckIncludeLogin(QSID qsid)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql      = m_pIDb->VMPrintf(kSelectEmpBySid, qsid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 1);
	AUTO_RELEASE_(pITable);
	while(SQLITE_ROW == pITable->Step())
	{
		int iFlag = pITable->GetColInt(0);
		if(iFlag & eSEFLAG_DELETE)
		{
			return FALSE;
		}
	}
	return TRUE;
}

DWORD C_eIMSessionMgr::TopSession(QSID qsid, E_TopSession eTopOperate)
{
	CHECK_INIT_RET_(0);
	DWORD dwTime = (DWORD)time(NULL);

	switch ( eTopOperate )
	{
	case eTOP_SESSION_ADD:
		if ( const char* pszSql = m_pIDb->VMPrintf(kReplaceSessionTop, qsid, dwTime) )
		{
			if (SQLITE_OK == m_pIDb->SQL(pszSql, true))
				return dwTime;
		}
		break;
	case eTOP_SESSION_DEL:
		if ( const char* pszSql = m_pIDb->VMPrintf(kDeleteSessionTop, qsid) )
		{
			if (SQLITE_OK == m_pIDb->SQL(pszSql, true))
				return 1;
		}
		break;
	case eTOP_SESSION_GET:
		if ( const char* pszSql = m_pIDb->VMPrintf(kSelectSessionTopSid, qsid) )
		{
			if (I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true))
			{
				AUTO_RELEASE_(pITable);
				if(SQLITE_ROW == pITable->Step())
					return pITable->GetColInt(0);
			}
		}
		break;
	case eTOP_SESSION_COUNT:
		if (I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectSessionTopCount, false))
		{
			AUTO_RELEASE_(pITable);
			if(SQLITE_ROW == pITable->Step())
				return pITable->GetColInt(0);
		}
		break;
	default:
		ASSERT_(FALSE);
		break;
	}

	return 0;
}

DWORD  C_eIMSessionMgr::EnumTopSession(PFN_NewSessionCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	DWORD dwCount = 0;
	I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectSessionTop, false);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while (SQLITE_ROW == pITable->Step())
	{
		if ( pfnCb && pfnCb(pITable->GetColInt64(0), pvUserData) )
			dwCount++;
	}

	return dwCount;
}

DWORD C_eIMSessionMgr::SearchMsgCount(PS_MsgEnum psCond)
{
	CHECK_NULL_RET_(psCond, 0);
	CHECK_INIT_RET_(0);

	const char* pszDate   = NULL;
	const char* pszContact   = NULL;
	const char* pszContent   = NULL;

	// 时间过滤
	if ( psCond->eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp=%u ", psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			pszDate = m_pIDb->VMPrintf("");
		}
		else
		{
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u ", psCond->dwStart);
		}
	}
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
	{
		pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u ", psCond->dwStart, psCond->dwEnd);
	}

	// 内容过滤
	if (psCond->eOperate & eMSG_OP_TEXT)
	{
		eIMStringA	szTextA;
		::eIMTChar2UTF8(psCond->szContent, szTextA);
		pszContent = m_pIDb->VMPrintf(kNewMsgAddContentLike, szTextA.c_str()); 
	}
	
	const char* pszSql = m_pIDb->VMPrintf(kNewSelectMsgCount, psCond->rQSID, pszContact, pszDate, pszContent); 

	m_pIDb->Free((void*)pszDate);
	m_pIDb->Free((void*)pszContact);
	m_pIDb->Free((void*)pszContent);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	DWORD		dwCount = 0;
	if( pITable->Step() == SQLITE_ROW )
	{
		dwCount	= pITable->GetColInt(0);
	}

	return dwCount;
}

	
int __stdcall C_eIMSessionMgr::SingleSessionMemberCb(QEID qeid, void* pvUserData)
{
	if ( qeid != Eng.GetPurview(GET_LOGIN_QEID) )
	{
		(*(QEID*)pvUserData) = qeid;
		return 0;
	}

	return 1;
}

BOOL C_eIMSessionMgr::GetSingleTitle(TCHAR szTitle[MAX_SESSION_TITLE], QSID sid)
{
	QEID eid = 0;
	EnumMember(sid, SingleSessionMemberCb, &eid);
	if ( eid )
	{
		_tcsncpy(szTitle, C_eIMContacts::GetObject().GetEmpName(eid), MAX_SESSION_TITLE);
		szTitle[MAX_SESSION_TITLE - 1] = _T('\0');
		SetTitle(sid, szTitle);	// 设置
		return TRUE;
	}

	ASSERT_(FALSE);
	return FALSE;
}

BOOL C_eIMSessionMgr::CheckSessionValid(QSID qsid)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kSelectSessionValid,qsid); 
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	DWORD		dwCount = 0;
	if( pITable->Step() == SQLITE_ROW )
	{
		dwCount	= pITable->GetColInt(0);
	}

	return dwCount <=0 ? FALSE : TRUE;
}
