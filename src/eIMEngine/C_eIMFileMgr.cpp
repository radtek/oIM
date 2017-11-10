#include "C_eIMFileMgr.h"

IMPLEMENT_PLUGIN_REF_(C_eIMFileMgr, INAME_EIMENGINE_FILEMGR, m_dwRef);
C_eIMFileMgr::C_eIMFileMgr(void)
	: m_dwRef( 1 )
	, m_pIDb ( NULL )
{
}

C_eIMFileMgr::~C_eIMFileMgr(void)
{
	Uninit();
}

BOOL C_eIMFileMgr::Init(I_SQLite3* pIDb)
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

BOOL C_eIMFileMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

BOOL C_eIMFileMgr::Get(QFID Qfid, PS_OffFile psOffFile)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(psOffFile, FALSE);

	eIMString		szValue;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectFilesFid, Qfid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return FALSE;

	psOffFile->Qfid			= Qfid;
	psOffFile->Qmid			= (UINT64)pITable->GetColInt64(1);
	psOffFile->u64Size		= (UINT64)pITable->GetColInt64(3);
	psOffFile->eidSender	= pITable->GetColInt(4);
	psOffFile->dwTimestamp	= pITable->GetColInt(5);
	psOffFile->dwFlag		= pITable->GetColInt(7);

	SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(0)), psOffFile->szName);
	SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(2)), psOffFile->szKey);
	SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(6)), psOffFile->szLocalPath);

	return TRUE;
}

BOOL C_eIMFileMgr::Get(QFID Qfid, PS_P2pFile psP2pFile)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(psP2pFile, FALSE);

	eIMString		szValue;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectP2pFid, Qfid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return FALSE;

	psP2pFile->Qfid			= Qfid;
	psP2pFile->Qmid			= (UINT64)pITable->GetColInt64(1);
	psP2pFile->u64Size		= (UINT64)pITable->GetColInt64(2);
	psP2pFile->eidRecver	= pITable->GetColInt(3);
	psP2pFile->eidSender	= pITable->GetColInt(4);
	psP2pFile->dwTimestamp	= pITable->GetColInt(5);
	psP2pFile->dwFlag		= pITable->GetColInt(8);

	SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(0)), psP2pFile->szName);
	SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(6)), psP2pFile->szToken);
	SET_UTF8_VALUE_(eIMCheckColTextNull(pITable->GetColText(7)), psP2pFile->szLocalPath);

	return TRUE;
}

int C_eIMFileMgr::Set(PS_OffFile psOffFile)
{
	CHECK_INIT_RET_(EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(psOffFile, EIMERR_INVALID_PARAM);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceFiles, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@fid_",		(INT64)psOffFile->Qfid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@msgid",		(INT64)psOffFile->Qmid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@name",		psOffFile->szName);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@key",		psOffFile->szKey);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@size",		(INT64)psOffFile->u64Size);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sender",	(int)psOffFile->eidSender);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",	(int)psOffFile->dwTimestamp);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@localpath",	psOffFile->szLocalPath);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",		(int)psOffFile->dwFlag);		ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMFileMgr::Set(PS_P2pFile psP2pFile)
{
	CHECK_INIT_RET_(EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(psP2pFile, EIMERR_INVALID_PARAM);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceP2p, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@fid_",		(INT64)psP2pFile->Qfid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@msgid",		(INT64)psP2pFile->Qmid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@name",		psP2pFile->szName);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@size",		(INT64)psP2pFile->u64Size);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@receiver",	(int)psP2pFile->eidRecver);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sender",	(int)psP2pFile->eidSender);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",	(int)psP2pFile->dwTimestamp);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@token",		psP2pFile->szToken);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@localpath",	psP2pFile->szLocalPath);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",		(int)psP2pFile->dwFlag);		ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

BOOL C_eIMFileMgr::UpdateFlag(QFID Qfid, DWORD dwFlag, BOOL bSet, BOOL bP2p)
{
	CHECK_INIT_RET_(FALSE);
	const char* pszSetSql   = bP2p ? kUpdateP2pSetFlagFid   : kUpdateFilesSetFlagFid;
	const char* pszClearSql = bP2p ? kUpdateP2pClearFlagFid : kUpdateFilesClearFlagFid;
	const char* pszSql = m_pIDb->VMPrintf( bSet ? pszSetSql : pszClearSql, dwFlag, Qfid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}

DWORD C_eIMFileMgr::GetCount(DWORD dwStart, DWORD dwEnd, BOOL bP2p)
{
	CHECK_INIT_RET_(0);
	const char*		pszSql  = m_pIDb->VMPrintf(bP2p ? kSelectP2pCount : kSelectFilesCount, dwStart, dwEnd);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return (DWORD)pITable->GetColInt(0);
}

DWORD C_eIMFileMgr::Enum( DWORD dwStart, DWORD dwEnd, PFN_OffFileCb pfnOffFileCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnOffFileCb, 0);

	eIMString		szValue;
	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectFiles, eDEL_RECORDS, eDEL_RECORDS, dwStart, dwEnd);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szValue);
		if ( !pfnOffFileCb(pITable->GetColInt(0), szValue.c_str(), (QEID)pITable->GetColInt(2), pvUserData) )
			break;
	}
		
	return dwCount;
}

DWORD C_eIMFileMgr::Enum(DWORD dwStart, DWORD dwEnd, PFN_P2pFileCb pfnP2pFileCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnP2pFileCb, 0);

	eIMString		szValue;
	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectP2p, eDEL_RECORDS, eDEL_RECORDS, dwStart, dwEnd);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szValue);
		if ( !pfnP2pFileCb(pITable->GetColInt(0), szValue.c_str(), pITable->GetColInt(2), pITable->GetColInt(3), pvUserData) )
			break;
	}
		
	return dwCount;
}

DWORD C_eIMFileMgr::Enum(BOOL	bIsSendFile, PFN_FileMsgDataCb	pfnFileMsgDataCb,void*	pvUserData)
{
	return 0;
}

DWORD C_eIMFileMgr::Enum(S_FilesEnum&	sEnumInfo, PFN_FileMsgDataCb	pfnFileMsgDataCb,void*	pvUserData)
{
	CHECK_INIT_RET_(0);

	DWORD		dwCount = 0;
	const char* pszSql    = NULL;
	const char* pszDate   = NULL;

	GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	C_eIMEngine& eng = Eng;
	if ( sEnumInfo.dwLimit == 0 )
		sEnumInfo.dwLimit = 0xFFFFFFFF;

	// Date
	if(sEnumInfo.bIsSender)		// 文件发送
	{
		if ( sEnumInfo.eOperate & eMSG_OP_DATE )
			pszDate = m_pIDb->VMPrintf("AND timestamp=%u And sender=%u ORDER BY timestamp DESC", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_GE_DATE ) 
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u And sender=%u ORDER BY timestamp DESC", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_BETWEEN_DATE )
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u And sender=%u ORDER BY timestamp DESC", sEnumInfo.dwStart, sEnumInfo.dwEnd, sEnumInfo.qeid);
	}
	else
	{
		if ( sEnumInfo.eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp=%u And sender<>%u ORDER BY timestamp DESC", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_GE_DATE ) 
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u And sender<>%u ORDER BY timestamp DESC", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_BETWEEN_DATE )
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u And sender<>%u ORDER BY timestamp DESC", sEnumInfo.dwStart, sEnumInfo.dwEnd, sEnumInfo.qeid);
	}

	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearch = NULL;

	// Content and contacts
	if ( (sEnumInfo.eOperate & eMSG_OP_TEXT) && (sEnumInfo.eOperate & eMSG_OP_CONTS) )
	{
		::eIMTChar2UTF8(sEnumInfo.szContent, szTextA);
		::eIMTChar2UTF8(sEnumInfo.szContacts, szContsA);
		pszSearch = m_pIDb->VMPrintf(kUseSelectFilesAccountFile,szContsA.c_str(), szContsA.c_str(), szTextA.c_str());
	}
	else if (sEnumInfo.eOperate & eMSG_OP_TEXT)
	{
		::eIMTChar2UTF8(sEnumInfo.szContent, szTextA);
		pszSearch = m_pIDb->VMPrintf(kUseSelectFilesFileName, szTextA.c_str()); 
	}
	else if (sEnumInfo.eOperate & eMSG_OP_CONTS)
	{
		::eIMTChar2UTF8(sEnumInfo.szContacts, szContsA);
		pszSearch = m_pIDb->VMPrintf(kUseSelectFilesAccount, szContsA.c_str(), szContsA.c_str()); // join...
	}

	const char* pszlimit   = NULL;
	pszlimit = m_pIDb->VMPrintf("LIMIT %u OFFSET %u", sEnumInfo.dwLimit, sEnumInfo.dwOffset);

	if(pszSearch)
	{
		if(pszDate)
		{
			pszSql = m_pIDb->VMPrintf(kUseSelectFiles, pszSearch, pszDate, pszlimit);
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kUseSelectFiles, pszSearch, "", pszlimit);
		}
	}
	else
	{
		if(pszDate)
		{
			pszSearch = m_pIDb->VMPrintf("WHERE 1");
			pszSql = m_pIDb->VMPrintf(kUseSelectFiles, pszSearch, pszDate, pszlimit);
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kUseSelectFiles, "", "", pszlimit);
		}
	}

	m_pIDb->Free((void*)pszSearch);

	m_pIDb->Free((void*)pszDate);

	m_pIDb->Free((void*)pszlimit);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_FileMsgInfo rInfo;
	eIMString		sTemp;
	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		memset(&rInfo, 0, sizeof(rInfo));
		rInfo.fid				= (UINT64)pITable->GetColInt64(0);
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), sTemp);
		_tcscpy(rInfo.szName, sTemp.c_str());
		rInfo.msgid				= (UINT64)pITable->GetColInt64(2);
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(3)), sTemp);
		_tcscpy(rInfo.szKey, sTemp.c_str());

		rInfo.u64FileSize		= (UINT64)pITable->GetColInt64(4);
		rInfo.qSenderID			= pITable->GetColInt(5);
		rInfo.datatime			= pITable->GetColInt(6);

		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(7)), sTemp);
		_tcscpy(rInfo.szLocalPath, sTemp.c_str());
		if ( !pfnFileMsgDataCb(rInfo, pvUserData) )
			break;
	}
	return dwCount;
}

DWORD C_eIMFileMgr::GetCount(S_FilesEnum&	sEnumInfo)
{
	CHECK_INIT_RET_(0);

	DWORD		dwCount = 0;
	const char* pszSql    = NULL;
	const char* pszDate   = NULL;

	GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	C_eIMEngine& eng = Eng;
	if ( sEnumInfo.dwLimit == 0 )
		sEnumInfo.dwLimit = 0xFFFFFFFF;

	// Date
	if(sEnumInfo.bIsSender)		// 文件发送
	{
		if ( sEnumInfo.eOperate & eMSG_OP_DATE )
			pszDate = m_pIDb->VMPrintf("AND timestamp=%u And sender=%u", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_GE_DATE ) 
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u And sender=%u", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_BETWEEN_DATE )
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u And sender=%u", sEnumInfo.dwStart, sEnumInfo.dwEnd, sEnumInfo.qeid);
	}
	else
	{
		if ( sEnumInfo.eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp=%u And sender<>%u", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_GE_DATE ) 
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u And sender<>%u", sEnumInfo.dwStart, sEnumInfo.qeid);
		else if ( sEnumInfo.eOperate & eMSG_OP_BETWEEN_DATE )
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u And sender<>%u", sEnumInfo.dwStart, sEnumInfo.dwEnd, sEnumInfo.qeid);
	}
	

	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearch = NULL;

	// Content and contacts
	if ( (sEnumInfo.eOperate & eMSG_OP_TEXT) && (sEnumInfo.eOperate & eMSG_OP_CONTS) )
	{
		::eIMTChar2UTF8(sEnumInfo.szContent, szTextA);
		::eIMTChar2UTF8(sEnumInfo.szContacts, szContsA);
		pszSearch = m_pIDb->VMPrintf(kUseSelectFilesAccountFile,szContsA.c_str(), szContsA.c_str(), szTextA.c_str());
	}
	else if (sEnumInfo.eOperate & eMSG_OP_TEXT)
	{
		::eIMTChar2UTF8(sEnumInfo.szContent, szTextA);
		pszSearch = m_pIDb->VMPrintf(kUseSelectFilesFileName, szTextA.c_str()); 
	}
	else if (sEnumInfo.eOperate & eMSG_OP_CONTS)
	{
		::eIMTChar2UTF8(sEnumInfo.szContacts, szContsA);
		pszSearch = m_pIDb->VMPrintf(kUseSelectFilesAccount, szContsA.c_str(), szContsA.c_str()); // join...
	}

	if(pszSearch)
	{
		if(pszDate)
		{
			pszSql = m_pIDb->VMPrintf(kUseSelectFilesCount, pszSearch, pszDate, "");
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kUseSelectFilesCount, pszSearch, "", "");
		}
	}
	else
	{
		if(pszDate)
		{
			pszSearch = m_pIDb->VMPrintf("WHERE 1");
			pszSql = m_pIDb->VMPrintf(kUseSelectFilesCount, pszSearch, pszDate, "");
		}
		else
		{
			pszSql = m_pIDb->VMPrintf(kUseSelectFilesCount, "", "", "");
		}
	}

	m_pIDb->Free((void*)pszSearch);

	m_pIDb->Free((void*)pszDate);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	if( pITable->Step() == SQLITE_ROW )
	{
		dwCount		= pITable->GetColInt(0);
	}

	return dwCount;
}

BOOL C_eIMFileMgr::Delete(DWORD	dwStart, DWORD dwEnd, E_DelFileFlag eFlag, BOOL bP2p )
{	// Only set the delete flag, the record must be delay delete for file receive, but the delete files maybe immediate...
	// However, the file and image maybe deleted on needed by total size.
	CHECK_INIT_RET_(FALSE);
	const char* pszSql = m_pIDb->VMPrintf(bP2p ? kUpdateP2pSetFlagTt : kUpdateFilesSetFlagTt, eFlag, dwStart, dwEnd);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}
