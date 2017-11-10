#include "stdafx.h"
#include "C_eIMLoginUserMgr.h"
#include "Des.h"

IMPLEMENT_PLUGIN_REF_(C_eIMLoginUserMgr, INAME_EIMENGINE_LOGIN_USER_MGR, m_dwRef);
C_eIMLoginUserMgr::C_eIMLoginUserMgr(void)
	: m_dwRef( 1 )
	, m_pIDb( NULL )
{
}

C_eIMLoginUserMgr::~C_eIMLoginUserMgr(void)
{
	Uninit();
}

int __stdcall C_eIMLoginUserMgr::LoginUserCb(const TCHAR* pszName, DWORD dwFlag, void* pvUserData)
{
	return 0;
}

BOOL C_eIMLoginUserMgr::Init(I_SQLite3* pIDb)
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
		GetDbFile(eDBTYPE_USERS, szDbFileA) == NULL ||
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

BOOL C_eIMLoginUserMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

//=============================================================================
//Function:     Set
//Description:	Set the login user information, the timestamp will be auto updated
//
//Parameter:
//	qeid       - Employee id
//	pszName    - Login name
//	pszPsw     - Login psw
//	dwFlag     - Flag
//
//Return:
//		Return the error code            
//=============================================================================
int C_eIMLoginUserMgr::Set(QEID qeid, const TCHAR* pszName, const TCHAR* pszPsw, DWORD dwFlag)
{
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	CHECK_NULL_RET_(pszName, EIMERR_INVALID_PARAM);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	int				i32Ret  = 0;
	eIMStringA		szValueA;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceLoginUser, pIStmt, i32Ret);
	
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	// Bind
	i32Ret = pIStmt->Bind("@eid_",		(INT64)qeid);		ASSERT_(i32Ret == SQLITE_OK);

	::eIMTChar2UTF8(pszName, szValueA);
	i32Ret = pIStmt->Bind("@name",		szValueA.c_str());	ASSERT_(i32Ret == SQLITE_OK);

	::eIMTChar2UTF8(pszPsw, szValueA);
	if ( dwFlag & DB_FLAG_UPDATED )
	{
		dwFlag &=~DB_FLAG_UPDATED;
		if (!DES(szValueA, false, "ecloud"))
		{
			szValueA.clear();	// CAN NOT DES
			dwFlag = 1;			// Not auto login
		}
	}

	DES(szValueA);

	i32Ret = pIStmt->Bind("@psw",		szValueA.c_str());	ASSERT_(i32Ret == SQLITE_OK);

//	i32Ret = pIStmt->Bind("@db_ver",		(INT)dwDbVer);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp",	time(NULL));		ASSERT_(i32Ret == SQLITE_OK);		
	i32Ret = pIStmt->Bind("@flag",		(INT)dwFlag);		ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

BOOL C_eIMLoginUserMgr::Get(const TCHAR* pszName, eIMString& szPsw)
{
	szPsw.clear();
	CHECK_NULL_RET_(pszName, FALSE);
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	eIMStringA   szValueA;
	::eIMTChar2UTF8(pszName, szValueA);
	const char* pszSql = m_pIDb->VMPrintf(kSelectLoginUserPsw, szValueA.c_str());
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if ( SQLITE_ROW == pITable->Step() )
	{
		szValueA = eIMCheckColTextNull(pITable->GetColText(0));
		DES(szValueA, false);
		::eIMUTF8ToTChar(szValueA.c_str(), szPsw);
		return TRUE;
	}

	return FALSE;
}
//=============================================================================
//Function:     UpdateFlag
//Description:	Set/Clear the flag
//
//Parameter:
//	qeid      - Employee ID
//	dwFlag    - Flag to be set or clear
//	bSet      - TRUE to set, else to clear
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMLoginUserMgr::UpdateFlag(QEID qeid, DWORD dwFlag, BOOL bSet)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(bSet ? kUpdateLoginUserSetFlag : kUpdateLoginUserClearFlag, dwFlag, qeid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}

//=============================================================================
//Function:     Delete
//Description:	Delete a remembered user login information
//
//Parameter:
//	pszName		- Login user name
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMLoginUserMgr::Delete(const TCHAR* pszName)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pszName, FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kDeleteLoginUserName, pszName);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}

//=============================================================================
//Function:     GetDbVer
//Description:	Get database version, the default is from app.config\app:database
//
//Parameter:
//	pszName    - Login user name
//
//Return:
//		Return the database version
//=============================================================================
DWORD C_eIMLoginUserMgr::GetDbVer(const TCHAR* pszName)
{
	DWORD	dwDbVer = Eng.GetAttributeInt(APP_ATTR_DATABASE_VERSION);
	CHECK_INIT_RET_(dwDbVer);
	CHECK_NULL_RET_(pszName, dwDbVer);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectLoginUserDbVer, pszName);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, dwDbVer);
	AUTO_RELEASE_(pITable);
	
	return pITable->GetColInt(0);
}

//=============================================================================
//Function:     SetDbVer
//Description:	Set database version
//
//Parameter:
//	pszName    - Login user name
//	dwVer      - Database version
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMLoginUserMgr::SetDbVer(const TCHAR* pszName, DWORD dwVer)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pszName, FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kUpdateLoginUserDbVer, dwVer, pszName);
	DWORD dwRet = m_pIDb->SQL(pszSql, true);

	return (dwRet == SQLITE_OK);
}

//=============================================================================
//Function:     Enum
//Description:	Enumerate the remembered login users
//
//Parameter:
//	pfnCb         - Login users callback function, return 0 to break
//	pvUserData    - User define data
//
//Return:
//		Return the count             
//=============================================================================
DWORD C_eIMLoginUserMgr::Enum(PFN_LoginUserCb pfnCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnCb, 0);

	DWORD			dwCount = 0;
	eIMString		szName;
	I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectLoginUserNameFlag);
	CHECK_NULL_RET_(pITable, dwCount);
	AUTO_RELEASE_(pITable);

	while(pITable->Step() == SQLITE_ROW)
	{
		dwCount++;
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(0)), szName);
		if ( !pfnCb(szName.c_str(), pITable->GetColInt(1), pITable->GetColInt(2), pvUserData) )
			break;
	}

	return dwCount;
}

//des º”Ω‚√‹
bool C_eIMLoginUserMgr::DES(eIMStringA& szDataA, bool bEncrypt, const char* pszKey)
{
	if (szDataA.size() < 2 || szDataA.size() > 64)
		return false;

//	const char* pszKey = "yf~gz@16\t03\a11";	// old1:sxit_ecloud; old2:ecloud
	char szTag[3] = { 'A', 24};
	CDes des;

	des.Setkey(pszKey);
	if ( bEncrypt )
	{
		if ( szDataA[0] == szTag[0] && szDataA[1] == szTag[1] )
		{
			TRACE_(_T("Data already is encrypted"));
			return true;
		}

		eIMStringA szEncData = szDataA;
		char aszPasswd[65] = {0};
		int  nLoop = 0;
		while( !szEncData.empty() )
		{
			eIMStringA szEnc = szEncData.substr(0,8);
			des.Desrun(&aszPasswd[nLoop], szEnc.c_str(), DES_ENCRYPT);
			nLoop += 8;

			int len = szEncData.length() - 8;
			if (len > 0)
				szEncData = szEncData.substr(8,len);
			else
				szEncData = "";
		}

		// To hex char
		szDataA = szTag;
		for (int i = 0;i<nLoop;i++)
		{
			char st[3] = {0};
			sprintf(st,"%02x",(unsigned char )aszPasswd[i]);
			szDataA.append(st);
		}
	}
	else // if ( bEncrypt )
	{
		if ( szDataA[0] == szTag[0] && szDataA[1] == szTag[1] )
		{
			char czBuff[65] = {0};
			char aszPasswd[65] = {0};
			int  nLoop = 0;

			eIMStringA szDecCode = szDataA.substr(2, szDataA.length() - 2);
			for (int i = 0; i< (int)szDecCode.length(); i += 2)
			{	// Hex char to int
				sscanf(&szDecCode[i],"%02x",&czBuff[i/2]);
			}

			while(nLoop < (int)szDecCode.length()/2)
			{
				des.Desrun(&aszPasswd[nLoop], &czBuff[nLoop], DES_DECRYPT);
				nLoop += 8;
			}

			szDataA = aszPasswd;
		}
	}

	return true;
}

QEID C_eIMLoginUserMgr::Get(const TCHAR* pszName)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pszName, 0);
	eIMStringA szNameA;

	eIMTChar2UTF8(pszName, szNameA);
	const char* pszSql = m_pIDb->VMPrintf(kSelectLoginUserId, szNameA.c_str());
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if ( pITable->Step() == SQLITE_ROW )
		return pITable->GetColInt(0);

	return 0;
}
