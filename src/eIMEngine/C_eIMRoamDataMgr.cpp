#include "stdafx.h"
#include "C_eIMRoamDataMgr.h"
#include "eIMEngine\IeIMContacts.h"

#define  SYN_NOT					0
#define  SYN_ING					1
#define  SYN_YES					2

IMPLEMENT_PLUGIN_REF_(C_eIMRoamDataMgr, INAME_EIMENGINE_ROAMDATA, m_dwRef);
C_eIMRoamDataMgr::C_eIMRoamDataMgr(void)
	: m_dwRef( 1 )
	, m_pIDb( NULL )
{
}

C_eIMRoamDataMgr::~C_eIMRoamDataMgr(void)
{
	Uninit();
}

BOOL C_eIMRoamDataMgr::Init(I_SQLite3* pIDb)
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
		 SQLITE_OK != m_pIDb->Open(szDbFileA.c_str()) )
	{
		SAFE_RELEASE_INTERFACE_(m_pIDb);
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Initial mamager failed!")); 

		return FALSE;
	}

	m_pIDb->Key(DB_KEY, DB_KEY_LEN);
	m_pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);
	return TRUE;
}

BOOL C_eIMRoamDataMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

void C_eIMRoamDataMgr::RoamDataSyncReq()
{
}

DWORD C_eIMRoamDataMgr::Enum(int iType,PFN_RoamDataCB pfnCb,void* pvUserData)
{
	DWORD		dwCount     = 0;
	CHECK_INIT_RET_(FALSE);
	switch(iType)
	{
	case ROAM_TYPE_COM_CONTACT:
		{
			I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectTopEmpsInfo);
			CHECK_NULL_RET_(pITable, 0);
			AUTO_RELEASE_(pITable);
			ROAM_DATA_INFO sInfo = { 0 };
			while(SQLITE_ROW == pITable->Step())
			{
				dwCount++;
				sInfo.id = pITable->GetColInt(0);
				sInfo.iis_default = pITable->GetColInt(1);
				sInfo.iUdateFlag = pITable->GetColInt(2);

				if (! pfnCb(&sInfo, pvUserData) )
					return dwCount;
			}
		}
		break;
	case ROAM_TYPE_COM_DEPT:
		{
			I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectTopDeptsInfo);
			CHECK_NULL_RET_(pITable, 0);
			AUTO_RELEASE_(pITable);
			ROAM_DATA_INFO sInfo = { 0 };
			while(SQLITE_ROW == pITable->Step())
			{
				dwCount++;
				sInfo.id = pITable->GetColInt(0);
				sInfo.iis_default = 0;
				sInfo.iUdateFlag = pITable->GetColInt(1);

				if (! pfnCb(&sInfo, pvUserData) )
					return dwCount;
			}
		}
		break;
	case ROAM_TYPE_CONCERN_CONTACT:
		{
		}
		break;
	case ROAD_TYPE_DEFINE_GROUP:
		{
			I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectDefineGroupInfo);
			CHECK_NULL_RET_(pITable, 0);
			AUTO_RELEASE_(pITable);
			ROAM_DATA_INFO sInfo = { 0 };
			eIMString		szValue;

			while(SQLITE_ROW == pITable->Step())
			{
				dwCount++;
				sInfo.id = pITable->GetColInt(0);
				sInfo.iis_default = 0;
				// 自定义组名
				::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText( 1 )), szValue);
				_tcsncpy(sInfo.szName, szValue.c_str(), ROAMINGDATA_MAX_NAME_LEN -1);	

				sInfo.iUdateFlag = pITable->GetColInt(2);

				if (! pfnCb(&sInfo, pvUserData) )
					return dwCount;
			}
		}
		break;
	case ROAM_TYPE_ROBOT:
		{
			I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectAllRobotInfo);
			CHECK_NULL_RET_(pITable, 0);
			AUTO_RELEASE_(pITable);
			ROAM_DATA_INFO sInfo = { 0 };
			while(SQLITE_ROW == pITable->Step())
			{
				dwCount++;
				sInfo.id = pITable->GetColInt(0);

				if (! pfnCb(&sInfo, pvUserData) )
					return dwCount;
			}
		}
		break;
	case ROAM_TYPE_VIRGROUP:
		{
			I_SQLite3Table* pITable = m_pIDb->GetTable(kgetvirGroupmainid);
			CHECK_NULL_RET_(pITable, 0);
			AUTO_RELEASE_(pITable);
			ROAM_DATA_INFO sInfo = { 0 };
			while(SQLITE_ROW == pITable->Step())
			{
				dwCount++;
				sInfo.id = pITable->GetColInt(0);

				if (! pfnCb(&sInfo, pvUserData) )
					return dwCount;
			}
		}
	case ROAM_TYPE_COMMON_GROUP:
		if ( I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectDefineCommonGroupInfo))
		{
			AUTO_RELEASE_(pITable);
			ROAM_DATA_INFO sInfo = { 0 };
			eIMString	   szValue;

			while(SQLITE_ROW == pITable->Step())
			{
				dwCount++;
				sInfo.id = pITable->GetColInt64(0);
				sInfo.iis_default = 0;

				::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szValue);
				_tcsncpy(sInfo.szName, szValue.c_str(), ROAMINGDATA_MAX_NAME_LEN -1);	
				sInfo.iUdateFlag = pITable->GetColInt(2);
				if (! pfnCb(&sInfo, pvUserData) )
					return dwCount;
			}
		}
		break;
	default:
		break;
	}

	return dwCount;
}

DWORD C_eIMRoamDataMgr::EnumDefineGroupMember(QGID qgid,PFN_DefineGroupMemberCb pfnCb,void* pvUserData)
{
	DWORD dwCount     = 0;
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(kSelectDefineGroupEmpsInfo, qgid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	QEID qeid = 0;
	while(SQLITE_ROW == pITable->Step())
	{
		dwCount++;
		qeid = pITable->GetColInt(0);
		if (! pfnCb(qeid, pvUserData) )
			return dwCount;
	}
	return dwCount;
}

int C_eIMRoamDataMgr::EnumDefineGroupMember(QGID qgid,VectEmpId& vtEmpID)
{
	vtEmpID.clear();
	DWORD dwCount     = 0;
	CHECK_INIT_RET_(FALSE);
	const char* pszSql;
	BOOL findByMemory = TRUE;
	I_EIMContacts*	pIContacts = NULL;
	if (FAILED(GetEngineInterface(INAME_EIMENGINE_CONTACTS,(void**)&pIContacts)))
		findByMemory = FALSE;

	AUTO_RELEASE_(pIContacts);
	pszSql = m_pIDb->VMPrintf(findByMemory?kSelectDefineGroupEmpsInfo1:kSelectDefineGroupEmpsInfo,qgid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	QEID qeid = 0;
	while(SQLITE_ROW == pITable->Step())
	{
		qeid = pITable->GetColInt(0);
		if (findByMemory)
		{
			if (pIContacts && pIContacts->FindEmp(qeid))
			{
				dwCount++;
				vtEmpID.push_back(qeid);
			}
		}
		else
		{
			dwCount++;
			vtEmpID.push_back(qeid);
		}	
	}
	return dwCount;
}

// 同步漫游数据
int C_eIMRoamDataMgr::Set(const ROAMDATASYNCACK* psAck)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	if(psAck->dwUserid != Eng.GetLoginEmp()->Id)
	{
		EIMLOGGER_ERROR_(pILog, _T("ROAM data not match the userid. userid=%u type=%d count=%u "),
			psAck->dwUserid, psAck->cResponseType, psAck->wNum);
		return EIMERR_NOT_IMPL;
	}
	int iRet = EIMERR_NO_ERROR;
	switch(psAck->cResponseType)
	{
	case ROAM_TYPE_COM_CONTACT:
		{
			// 先删除本地数据
			if(DelAllNotDefaultTopContact())
			{
			}
			EIMLOGGER_DEBUG_(pILog, _T("ROAM_TYPE_COM_CONTACT count=%u "),psAck->wNum);
			// 再插入所有数据
			for (DWORD dwIndex = 0; dwIndex < psAck->wNum; dwIndex++)
			{
				AddTopContact(psAck->dwUsersList[dwIndex], 0, SYN_YES);
			}
		}
		break;
	case ROAM_TYPE_DEFAULT_COMCONTACT:
		{
			// 先删除本地数据
			if(DelDefaultTopContact())
			{
			}
			EIMLOGGER_DEBUG_(pILog, _T("ROAM_TYPE_DEFAULT_COMCONTACT count=%u "),psAck->wNum);
			// 再插入所有数据
			for (DWORD dwIndex = 0; dwIndex < psAck->wNum; dwIndex++)
			{
				AddTopContact(psAck->dwUsersList[dwIndex], 1, SYN_YES);
			}
		}
		break;
	case ROAM_TYPE_COM_DEPT:
		{
			// 先删除本地数据
			if(DelAllTopDept())
			{
			}
			EIMLOGGER_DEBUG_(pILog, _T("ROAM_TYPE_COM_DEPT count=%u "),psAck->wNum);
			// 再插入所有数据
			for (DWORD dwIndex = 0; dwIndex < psAck->wNum; dwIndex++)
			{
				AddTopDept(psAck->dwUsersList[dwIndex], SYN_YES);
			}
		}
		break;
	case ROAM_TYPE_CONCERN_CONTACT:
		{
			// 删除本地数据
			if(ClearAllEmpConcernFlag())
			{
			}
			EIMLOGGER_DEBUG_(pILog, _T("ROAM_TYPE_CONCERN_CONTACT count=%u "),psAck->wNum);
			for (DWORD dwIndex = 0; dwIndex < psAck->wNum; dwIndex++)
			{
				SetEmpConcernFlag(psAck->dwUsersList[dwIndex], TRUE);
			}
		}
		break;
	default:
		return EIMERR_NOT_IMPL;
	}

	return EIMERR_NO_ERROR;
}

int C_eIMRoamDataMgr::Set(const ROAMDATAMODIACK* psAck)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	if(psAck->dwUserid != Eng.GetLoginEmp()->Id)
	{
		EIMLOGGER_ERROR_(pILog, _T("ROAM data not match the userid. userid=%u type=%d"),
			psAck->dwUserid, psAck->cResponseType);
		return EIMERR_NOT_IMPL;
	}
	switch(psAck->cResponseType)
	{
	case ROAM_TYPE_COM_CONTACT:
		{
			int iCount = psAck->tUserStatus.wNum[0] + psAck->tUserStatus.wNum[1] + psAck->tUserStatus.wNum[2] + psAck->tUserStatus.wNum[3];
			if(psAck->cModifyType == ROAM_OPER_TYPE_ADD)
			{
				for (int dwIndex = 0; dwIndex < iCount; dwIndex++)
				{
					AddTopContact(psAck->tUserStatus.dwUserID[dwIndex], 0, SYN_YES);
				}
			}
			else
			{
				for (int dwIndex = 0; dwIndex < iCount; dwIndex++)
				{
					DelTopContact(psAck->tUserStatus.dwUserID[dwIndex]);
				}
			}
		}
		break;
	case ROAM_TYPE_COM_DEPT:
		{
			if(psAck->cModifyType == ROAM_OPER_TYPE_ADD)
			{
				for (int dwIndex = 0; dwIndex < psAck->tDeptlist.wNum; dwIndex++)
				{
					AddTopDept(psAck->tDeptlist.dwDept[dwIndex], SYN_YES);
				}
			}
			else
			{
				for (int dwIndex = 0; dwIndex < psAck->tDeptlist.wNum; dwIndex++)
				{
					DelTopDept(psAck->tDeptlist.dwDept[dwIndex]);
				}
			}
		}
		break;
	case ROAM_TYPE_CONCERN_CONTACT:
		{
		}
		break;
	default:
		return EIMERR_NOT_IMPL;
	}
	return EIMERR_NO_ERROR;
}

int C_eIMRoamDataMgr::Set (S_CustomizeInfo* psModify)
{
	switch(psModify->dwCustomizeId)
	{
	case ROAM_TYPE_COM_CONTACT:
		{
			if(psModify->iOperateType == ROAM_OPER_TYPE_ADD)
			{
				for (DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					AddTopContact(psModify->adwId[dwIndex], 0, SYN_ING);
				}
			}
			else
			{
				// 删除等服务器回应
			}
		}
		break;
	case ROAM_TYPE_COM_DEPT:
		{
			if(psModify->iOperateType == ROAM_OPER_TYPE_ADD)
			{
				for (DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					AddTopDept(psModify->adwId[dwIndex], SYN_ING);
				}
			}
			else
			{
				// 删除等服务器回应
			}
		}
		break;
	case ROAM_TYPE_CONCERN_CONTACT:
		{
			if(psModify->iOperateType == ROAM_OPER_TYPE_ADD)
			{
				for(DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					SetEmpConcernFlag(psModify->adwId[dwIndex], TRUE);
				}
			}
			else if(psModify->iOperateType == ROAM_OPER_TYPE_DELETE)
			{
				for(DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					SetEmpConcernFlag(psModify->adwId[dwIndex], FALSE);
				}
			}
		}
		break;
	case ROAD_TYPE_DEFINE_GROUP:
		{
			
			if(psModify->iOperateType == ROAM_OPER_TYPE_ADD)
			{
				int iID = psModify->iGroupId;
				if(iID == 0)
				{
					iID = GetInsertDefineGroupID();
				}
				ASSERT_(iID);
				eIMStringA sNameA;
				eIMTChar2UTF8(psModify->aszName, sNameA);
				AddDefineGroup(iID, sNameA.c_str(), SYN_NOT);
				psModify->iGroupId = iID;
				for(DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					AddGroupMember(iID, psModify->adwId[dwIndex], SYN_NOT);
				}
			}
			else if(psModify->iOperateType == ROAM_OPER_TYPE_DELETE)
			{
				ASSERT_(psModify->iGroupId);
				DelDefineGroup(psModify->iGroupId);
				for(DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					DelGroupMember(psModify->iGroupId, psModify->adwId[dwIndex]);
				}
			}
		}
		break;
	case ROAD_TYPE_DEFINE_GROUP_MEM:
		{
			if(psModify->iOperateType == ROAM_OPER_TYPE_ADD)
			{
				for(DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					AddGroupMember(psModify->iGroupId, psModify->adwId[dwIndex], SYN_NOT);
				}
			}
			else if(psModify->iOperateType == ROAM_OPER_TYPE_DELETE)
			{
				for(DWORD dwIndex = 0; dwIndex < psModify->dwIdNum; dwIndex++)
				{
					DelGroupMember(psModify->iGroupId, psModify->adwId[dwIndex]);
				}
			}
		}
		break;
	default:
		return EIMERR_NOT_IMPL;
	}
	return EIMERR_NO_ERROR;
}

int C_eIMRoamDataMgr::Set(const ROAMDATAMODINOTICE* psAck)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	if(psAck->dwUserid != Eng.GetLoginEmp()->Id)
	{
		EIMLOGGER_ERROR_(pILog, _T("ROAM data not match the userid. userid=%u type=%d "),
			psAck->dwUserid, psAck->cResponseType);
		return EIMERR_NOT_IMPL;
	}
	switch(psAck->cResponseType)
	{
	case ROAM_TYPE_COM_CONTACT:
		{
			DWORD dwCount = psAck->tUserStatus.wNum[0] + psAck->tUserStatus.wNum[1] + psAck->tUserStatus.wNum[2] + psAck->tUserStatus.wNum[3];
			if(psAck->cModifyType == ROAM_OPER_TYPE_ADD)
			{
				for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
				{
					AddTopContact(psAck->tUserStatus.dwUserID[dwIndex], 0, SYN_YES);
				}
			}
			else
			{
				for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
				{
					DelTopContact(psAck->tUserStatus.dwUserID[dwIndex]);
				}
			}
		}
		break;
	case ROAM_TYPE_COM_DEPT:
		{
			if(psAck->cModifyType == ROAM_OPER_TYPE_ADD)
			{
				for (DWORD dwIndex = 0; dwIndex < psAck->tDeptlist.wNum; dwIndex++)
				{
					AddTopDept(psAck->tDeptlist.dwDept[dwIndex], SYN_YES);
				}
			}
			else
			{
				for (DWORD dwIndex = 0; dwIndex < psAck->tDeptlist.wNum; dwIndex++)
				{
					DelTopDept(psAck->tDeptlist.dwDept[dwIndex]);
				}
			}
		}
		break;
	case ROAM_TYPE_CONCERN_CONTACT:
		{
			DWORD dwCount = psAck->tUserStatus.wNum[0] + psAck->tUserStatus.wNum[1] + psAck->tUserStatus.wNum[2] + psAck->tUserStatus.wNum[3];
			if(psAck->cModifyType == ROAM_OPER_TYPE_ADD)
			{
				for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
				{
					SetEmpConcernFlag(psAck->tUserStatus.dwUserID[dwIndex], TRUE);
				}
			}
			else
			{
				for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
				{
					SetEmpConcernFlag(psAck->tUserStatus.dwUserID[dwIndex], FALSE);
				}
			}
		}
		break;
	default:
		return EIMERR_NOT_IMPL;
	}
	return EIMERR_NO_ERROR;
}

int C_eIMRoamDataMgr::AddTopContact(QEID eid, int is_default, int iFlag)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceTopEmpInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@eid",			 (int)eid);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@is_default",	is_default);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@update_flag",	iFlag);			ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

inline int C_eIMRoamDataMgr::DelTopContact(QEID eid)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kDeleteTopEmpInfo, eid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

inline int C_eIMRoamDataMgr:: DelAllNotDefaultTopContact()
{
	CHECK_INIT_RET_(FALSE);

	int i32Ret = m_pIDb->SQL(kDeleteAllNoDefaultEmpInfo);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

inline int C_eIMRoamDataMgr:: DelDefaultTopContact()
{
	CHECK_INIT_RET_(FALSE);

	int i32Ret = m_pIDb->SQL(kDeleteAllDefaultEmpInfo);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

int C_eIMRoamDataMgr::AddTopDept(QDID did, int iFlag)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceTopDeptInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@did",			 (int)did);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@update_flag",	iFlag);			ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

inline int C_eIMRoamDataMgr::DelTopDept(QDID did)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kDeleteTopDeptInfo, did);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

inline int C_eIMRoamDataMgr::DelAllTopDept()
{
	CHECK_INIT_RET_(FALSE);

	int i32Ret = m_pIDb->SQL(kDeleteAllTopDeptInfo);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

int C_eIMRoamDataMgr::AddDefineGroup(int id, const char* pszAlias, int iFlag)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceDefineGroupInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@id",			id);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@alias",			pszAlias);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@update_flag",	iFlag);				ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMRoamDataMgr::AddDefineCommonGroup(QSID id, const TCHAR* pszAlias, int iFlag)
{
	CHECK_INIT_RET_(FALSE);
	if ( id == 0 && iFlag == -1 )
	{	// Upgrade
		const char* pszSql = m_pIDb->VMPrintf(kSelectSessionFlag, eSEFLAG_MULTI, "ctime", "DESC");
		if ( I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true) )
		{
			AUTO_RELEASE_(pITable);
			QEID qeidLogin = Eng.GetPurview(GET_LOGIN_QEID);
			while(SQLITE_ROW == pITable->Step())
			{
				QSID  qsid   = pITable->GetColInt64(0);
				DWORD dwFlag = pITable->GetColInt(4);
				QEID  qeid   = pITable->GetColInt(5);
				if ( qeid == qeidLogin && !(dwFlag & DB_FLAG_DELETED) )	// Is myself create group, and not deleted
				{	// Add myself group to common group
					AddDefineCommonGroup(qsid);
				}
			}
		}
	}
	else
	{
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		int				i32Ret = 0;
		I_SQLite3Stmt*	pIStmt = NULL;
		eIMStringA		szValueA;

		GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceDefineCommonGroupInfo, pIStmt, i32Ret);
		::eIMTChar2UTF8(pszAlias, szValueA);

		i32Ret = m_pIDb->Begin();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

		i32Ret = pIStmt->Bind("@id",			(INT64)id);			ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@alias",			szValueA.c_str());	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@update_flag",	iFlag);				ASSERT_(i32Ret == SQLITE_OK);

		i32Ret = pIStmt->Step();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

		i32Ret = m_pIDb->Commit();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	}
	return EIMERR_NO_ERROR;
}

BOOL C_eIMRoamDataMgr::DelDefineCommonGroup(QSID id, BOOL bNoSelfs)
{
	CHECK_INIT_RET_(FALSE);
	if ( bNoSelfs )
	{
		const char* pszSql = m_pIDb->VMPrintf(kSelectSessionSid, id);
		if ( I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true) )
		{
			AUTO_RELEASE_(pITable);
			QEID qsidCreater = pITable->GetColInt(5);	// Creater QEID
			QEID qeidLogin = Eng.GetPurview(GET_LOGIN_QEID);
			if ( qsidCreater == qeidLogin )
			{
				TRACE_(_T("Can not delete self-common group, QSID:0x%llx"), id);
				return FALSE;
			}
		}
	}

	const char* pszSql = m_pIDb->VMPrintf(kDeleteDefineCommonGroupInfo, id);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

inline int C_eIMRoamDataMgr::GetInsertDefineGroupID()
{
	CHECK_INIT_RET_(FALSE);

	I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectInsertDefineGroupID);
	CHECK_NULL_RET_(pITable, EIMERR_GET_TABLE_FAILED);
	AUTO_RELEASE_(pITable);

	int iID = 0;
	if( pITable->Step() == SQLITE_ROW)
	{
		iID = pITable->GetColInt(0);
	}
	
	return iID + 1;
}

inline int C_eIMRoamDataMgr::DelDefineGroup(int id)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kDeleteDefineGroupInfo, id, id);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

int C_eIMRoamDataMgr::AddGroupMember(int iGroupID, QEID qeid, int iFlag)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceDefineGroupEmpInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@id",			iGroupID);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@eid",			(int)qeid);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@update_flag",	iFlag);				ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

inline int C_eIMRoamDataMgr::DelGroupMember(int iGroupID, QEID qeid)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kDeleteDefineGroupEmpInfo, iGroupID, qeid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}

inline int C_eIMRoamDataMgr::SetEmpConcernFlag(QEID eid, BOOL bConcern)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(bConcern ? kUpdateEmpSetFlag : kUpdateEmpClearFlag, IME_EMP_DB_CONCERN_FLAG, eid);
	int i32Ret =m_pIDb->SQL(pszSql, true);
	if(SQLITE_OK != i32Ret)
	{
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Update emp db  concern error!SQLErrId:[%d]"),i32Ret);
		return i32Ret;
	}

	C_eIMContacts&	objContact = C_eIMContacts::GetObject();
	S_EmpInfo sEmpInfo;
	ZERO_STRUCT_(sEmpInfo);
	sEmpInfo.Id = eid;
	sEmpInfo.bitConcern = bConcern;

	if (!objContact.SetEmps(sEmpInfo, eIMECSET_MODS_CONCERN, FALSE))
	{
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Update emp concern error!EmpId:[%d]"),sEmpInfo.Id);
	}

	return i32Ret;
}

inline int C_eIMRoamDataMgr::ClearAllEmpConcernFlag()
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kClearEmpConcernFlag, IME_EMP_DB_CONCERN_FLAG, IME_EMP_DB_CONCERN_FLAG, IME_EMP_DB_CONCERN_FLAG);
	int i32Ret = m_pIDb->SQL(pszSql, true);
	if(SQLITE_OK != i32Ret)
	{
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Update emps db  concern error!SQLErrId:[%d]"),i32Ret);
		return i32Ret;
	}
/*
	C_eIMContacts&	objContact = C_eIMContacts::GetObject();
	if (!objContact.UpdateEmpsState(eIMECSET_MODS_CONCERN, FALSE))
	{
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Update emps concern error!"));
	}*/

	return i32Ret;
}

int C_eIMRoamDataMgr::AddRobotInfo(Robot_User_Info& rInfo)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceRobotInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@userid",		(int)rInfo.dwUserID);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@usertype",		(int)rInfo.cUserType);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@attribute",	(int)rInfo.dwAttribute);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@greetings",	(char*)rInfo.aszGreetings);				ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

BOOL C_eIMRoamDataMgr::ClearRobotInfo()
{
	CHECK_INIT_RET_(FALSE);

	int i32Ret = m_pIDb->SQL(kDeleteAllRobotInfo);

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}


BOOL C_eIMRoamDataMgr::GetRobotInfo(QEID userid, Robot_User_Info& rInfo)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kSelectRobotInfo, userid);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	memset(&rInfo, 0, sizeof(Robot_User_Info));
	if( pITable->Step() == SQLITE_ROW)
	{
		rInfo.dwUserID = pITable->GetColInt(0);
		rInfo.cUserType = pITable->GetColInt(1);
		rInfo.dwAttribute = pITable->GetColInt(2);
		SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(3)), rInfo.aszGreetings);
		return TRUE;
	}
	return FALSE;
}

int C_eIMRoamDataMgr::AddGroupMember(void* pvData)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	CHECK_NULL_RET_(pvData, EIMERR_INVALID_PARAM);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;

	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceDefineGroupEmpInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	vector<S_GroupMember> *pvecMem = (vector<S_GroupMember>*)pvData;
	for(vector<S_GroupMember>::iterator it=pvecMem->begin(); it!=pvecMem->end(); it++)
	{
		i32Ret = pIStmt->Bind("@id",			it->i32GID);			ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@eid",			(int)it->dwEid);		ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@update_flag",	it->flag);				ASSERT_(i32Ret == SQLITE_OK);

		i32Ret = pIStmt->Step();
	}
	
	//ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

BOOL C_eIMRoamDataMgr::IsTypeof(QSID sid, int i32Type)
{
	CHECK_INIT_RET_(FALSE);
	I_SQLite3Table* pITable = NULL;
	if ( i32Type == ROAM_TYPE_COMMON_GROUP )
	{
		const char* pszSql = m_pIDb->VMPrintf(kSelectDefineCommonGroupInfoId, sid);
		pITable = m_pIDb->GetTable(pszSql, true);
	}
	else
	{
		ASSERT_(FALSE);
	}

	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);
	if( pITable->Step() == SQLITE_ROW)
		return TRUE;
	
	return FALSE;
}
