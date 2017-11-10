#include "StdAfx.h"
#include "C_eIMImportMsgMgr.h"

IMPLEMENT_PLUGIN_REF_(C_eIMImportMsgMgr, INAME_EIMENGINE_IMPORT_MSG, m_dwRef);
C_eIMImportMsgMgr::C_eIMImportMsgMgr()
	: m_dwRef(1)
	//, m_pIDb (NULL)
{
	
}

C_eIMImportMsgMgr::~C_eIMImportMsgMgr()
{
	//Uninit();
}

//BOOL C_eIMImportMsgMgr::Init()
//{
//	if(m_pIDb) return TRUE;
//
//	eIMStringA szDbFileA;
//	if ( FAILED(GetEngineInterface(INAME_SQLITE_DB, (void**)&m_pIDb)) ||
//	GetDbFile(eDBTYPE_MSG, szDbFileA) == NULL ||
//	SQLITE_OK != m_pIDb->Open(szDbFileA.c_str()) ||
//	SQLITE_OK != m_pIDb->Key(DB_KEY, DB_KEY_LEN)
//	)
//	{
//	SAFE_RELEASE_INTERFACE_(m_pIDb);
//	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
//	EIMLOGGER_ERROR_(pILog, _T("Initial mamager failed!")); 
//	return FALSE;
//	}
//
//	m_pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);
//	return TRUE;
//}

//BOOL C_eIMImportMsgMgr::Uninit()
//{
//	SAFE_RELEASE_INTERFACE_(m_pIDb);
//	return TRUE;
//}

BOOL C_eIMImportMsgMgr::ImportMsg(LPCTSTR lpszFullPath, int i32DbType)
{
	if(1 == i32DbType)
		return ImportContactsDb(lpszFullPath);

	if(2 == i32DbType)
		return ImportMsgDb(lpszFullPath);

	return FALSE;
}

BOOL C_eIMImportMsgMgr::ImportTable(I_SQLite3*	pIDb, const char* const pszSrcTable, const char* const pszDstTable)
{
	CHECK_NULL_RET_(pszSrcTable, FALSE);
	CHECK_NULL_RET_(pszDstTable, FALSE);
	CHECK_NULL_RET_(pIDb,        FALSE);

	const char*	pszSql  = NULL;
	pszSql = pIDb->VMPrintf("REPLACE INTO %s SELECT * FROM %s;", pszDstTable, pszSrcTable);
	int i32Ret = pIDb->SQL(pszSql, true);

	return SQLITE_OK == i32Ret;
}

BOOL C_eIMImportMsgMgr::ImportContactsDb(LPCTSTR lpszFullPath)
{
	const char* asContactsDbTable[] = 
	{
		{"t_define_group"		},
		{"t_define_group_emps"	},
		{"t_define_top_dept"	},
		{"t_define_top_emp"		},
		/*{"t_dept"				},*/
		/*{"t_dept_emp" 		},*/
		/*{"t_emp" 				},*/
		/*	{"t_expanditem_info"},*/
		/*{"t_robot_info"		},*/
		{"t_session" 			},
		{"t_session_emp"		},
		/*	{"t_session_resign"	},*/ 
		/*	{"t_subscribe_info"	},*/
		/*{"t_timestamp"		},*/
		/*{"t_update_contacts"	},*/
		/*{"t_user_purview"		},*/
		/*	{"t_user_purview"	},*/
	};


	I_SQLite3*	pIDb = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	eIMStringA szDbFileA;
	if ( FAILED(GetEngineInterface(INAME_SQLITE_DB, (void**)&pIDb)) ||
		GetDbFile(eDBTYPE_CONTS, szDbFileA) == NULL ||
		SQLITE_OK != pIDb->Open(szDbFileA.c_str()) ||
		SQLITE_OK != pIDb->Key(DB_KEY, DB_KEY_LEN)
		)
	{
		SAFE_RELEASE_INTERFACE_(pIDb);
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Initial mamager failed!")); 
		return FALSE;
	}
	AUTO_RELEASE_(pIDb);

	pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);

	eIMStringA szPath;
	eIMTChar2UTF8(lpszFullPath, szPath);
	int i32Ret = pIDb->Attach(szPath.c_str(), BKUP_DB_AS_NAME, DB_KEY);

	if ( SQLITE_OK != i32Ret )
	{
		i32Ret = EIMERR_DB_ERROR_(i32Ret);
		EIMLOGGER_ERROR_(pILog, _T("Import %S failed, ret:0x%08x"), lpszFullPath, i32Ret);
		return FALSE;
	}

	char szSrcTable[128];
	char szDstTable[128];
	int i32Cnt = COUNT_OF_ARRAY_(asContactsDbTable);
	for ( int i32Index = 0; i32Index < i32Cnt; i32Index++)
	{
		_snprintf(szSrcTable, COUNT_OF_ARRAY_(szSrcTable), "%s.%s", BKUP_DB_AS_NAME, asContactsDbTable[i32Index]);
		_snprintf(szDstTable, COUNT_OF_ARRAY_(szDstTable), "%s.%s", MAIN_DB_AS_NAME, asContactsDbTable[i32Index]);
		ImportTable(pIDb, szSrcTable, szDstTable);
	}

	pIDb->Detach(BKUP_DB_AS_NAME);

	return TRUE;
}

BOOL C_eIMImportMsgMgr::ImportMsgDb(LPCTSTR lpszFullPath)
{
	const char* asMsgDbTable[] = 
	{	
		{"t_alert_records"  },
		{"t_bro_records"	},
		{"t_msg_read"		},
		{"t_p2p_files"		},
		{"t_schedule"		},
		{"t_session_files"	},
		{"t_session_records"},
		{"t_sms_records"	},
	};

	I_SQLite3*	pIDb = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	eIMStringA szDbFileA;
	if ( FAILED(GetEngineInterface(INAME_SQLITE_DB, (void**)&pIDb)) ||
		GetDbFile(eDBTYPE_MSG, szDbFileA) == NULL ||
		SQLITE_OK != pIDb->Open(szDbFileA.c_str()) ||
		SQLITE_OK != pIDb->Key(DB_KEY, DB_KEY_LEN)
		)
	{
		SAFE_RELEASE_INTERFACE_(pIDb);
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Initial mamager failed!")); 
		return FALSE;
	}
	AUTO_RELEASE_(pIDb);

	pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);

	eIMStringA szPath;
	eIMTChar2UTF8(lpszFullPath, szPath);
	int i32Ret = pIDb->Attach(szPath.c_str(), BKUP_DB_AS_NAME, DB_KEY);

	if ( SQLITE_OK != i32Ret )
	{
		i32Ret = EIMERR_DB_ERROR_(i32Ret);
		EIMLOGGER_ERROR_(pILog, _T("Backup %S failed, ret:0x%08x"), lpszFullPath, i32Ret);
		return FALSE;
	}

	char szSrcTable[128];
	char szDstTable[128];
	int i32Cnt = COUNT_OF_ARRAY_(asMsgDbTable);
	for ( int i32Index = 0; i32Index < i32Cnt; i32Index++)
	{
		_snprintf(szSrcTable, COUNT_OF_ARRAY_(szSrcTable), "%s.%s", BKUP_DB_AS_NAME, asMsgDbTable[i32Index]);
		_snprintf(szDstTable, COUNT_OF_ARRAY_(szDstTable), "%s.%s", MAIN_DB_AS_NAME, asMsgDbTable[i32Index]);
		ImportTable(pIDb, szSrcTable, szDstTable);
	}

	pIDb->Detach(BKUP_DB_AS_NAME);
	
	return TRUE;
}