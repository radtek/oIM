#include "stdafx.h"
#include "C_eIMContactsMgr.h"


C_eIMContactsMgr::C_eIMContactsMgr(void)
	: m_pIDb( NULL )
	, m_qeidLogin( 0 )
	, m_i32Pinyin( IME_ATTRIB_PINYIN_DEFAULT )
{
}


C_eIMContactsMgr::~C_eIMContactsMgr(void)
{
	Uninit();
}

BOOL C_eIMContactsMgr::Init(I_SQLite3* pIDb)
{
	m_i32Pinyin = Eng.GetAttributeInt(IME_ATTRIB_PINYIN, IME_ATTRIB_PINYIN_DEFAULT);

	if ( pIDb )
	{
		SAFE_RELEASE_INTERFACE_(m_pIDb);	// Release old

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

BOOL C_eIMContactsMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

BOOL C_eIMContactsMgr::_BindDept(I_SQLite3Stmt* pIStmt, const DEPTINFO& sDept, DWORD dwCoId, BOOL bUpdate, const char* pszPinyin, const char* pszPinyinS)
{
	int i32Ret = SQLITE_OK;
	DWORD dwFlag = (sDept.wUpdate_type == 3 ? DB_FLAG_DELETED : sDept.wUpdate_type);

	i32Ret = pIStmt->Bind("@pinyin",	  pszPinyin);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@pinyin_s",	  pszPinyinS);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@did_",		  (int)sDept.dwDeptID);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@name",		  sDept.szCnDeptName);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@ename",		  sDept.szEnDeptName);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@pid",		  (int)sDept.dwPID);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@tel",		  sDept.aszDeptTel);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@orders",	  (WORD)CHECK_MAP_NO_ORDERS(sDept.wSort)); ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",		  (int)dwFlag);					ASSERT_(i32Ret == SQLITE_OK);	
	i32Ret = pIStmt->Bind("@coid",		  (int)dwCoId);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@update_time", (int)sDept.dwUpdateTime);		ASSERT_(i32Ret == SQLITE_OK);
	if ( bUpdate )
		i32Ret = pIStmt->Bind("@qdid",	  (int)sDept.dwDeptID);			ASSERT_(i32Ret == SQLITE_OK);	// Condition

	return (i32Ret == SQLITE_OK);
}

int C_eIMContactsMgr::Set(VectDeptInfo& vectDeptInfo_)
{
	int				i32Ret		= 0;
	DWORD			dwCoId		= Eng.GetAttributeInt(IME_ATTRIB_CO_ID);
	DEPTINFO		sDeptInfo	= { 0 };
	eIMStringA		szPinyinA;
	eIMStringA		szSearchPyA;
	eIMString		szValue;
	I_SQLite3Stmt*	pIStmt   = NULL;
	I_SQLite3Stmt*	pIStmt2  = NULL;
//	TCHAR			szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPY_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		
	VectDeptInfo vectDeptInfo;
	vectDeptInfo.swap(vectDeptInfo_);

	EIMLOGGER_DEBUG_(pILog, _T("Begin to write database of Dept, Count:%u"), vectDeptInfo.size());
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertDept, pIStmt, i32Ret);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kUpdateDept, pIStmt2,i32Ret);
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	pIPy->SetFlag(m_i32Pinyin);

	for ( VectDeptInfoIt_ it = vectDeptInfo.begin(); it != vectDeptInfo.end(); it++)
	{
		pIPy->GetUtf8Pinyins(it->szCnDeptName, szPinyinA, szSearchPyA);
		_BindDept(pIStmt, *it, dwCoId, FALSE, szPinyinA.c_str(), szSearchPyA.c_str());
		i32Ret = pIStmt->Step();
		if ( i32Ret == SQLITE_CONSTRAINT )
		{
			_BindDept(pIStmt2, *it, dwCoId, TRUE, szPinyinA.c_str(), szSearchPyA.c_str());
			i32Ret = pIStmt2->Step();
		}

		if ( i32Ret != SQLITE_OK )
		{
			EIMLOGGER_ERROR_(pILog, _T("did_:%u, pid:%u, orders:%u, flag:%u, name:%s, ename:%S, tel:%S, update_time:%u, pinyin:%S, pinyin_s:%S"),
				it->dwDeptID, it->dwPID, it->wSort, it->wUpdate_type, 
				(::eIMUTF8ToTChar(it->szCnDeptName, szValue), szValue.c_str()),
				it->szEnDeptName, it->aszDeptTel, it->dwUpdateTime,
				szPinyinA.c_str(), szSearchPyA.c_str());

			ASSERT_(FALSE);
			break;
		}
		else
		{
			EIMLOGGER_DATA_(pILog, NULL, 0, _T("did_:%u, pid:%u, orders:%u, flag:%u, name:%s, ename:%S, tel:%S, update_time:%u, pinyin:%S, pinyin_s:%S"),
				it->dwDeptID, it->dwPID, it->wSort, it->wUpdate_type, 
				(::eIMUTF8ToTChar(it->szCnDeptName, szValue), szValue.c_str()),
				it->szEnDeptName, it->aszDeptTel, it->dwUpdateTime,
				szPinyinA.c_str(), szSearchPyA.c_str());
		}
	}

	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	i32Ret = m_pIDb->Commit();  ASSERT_(i32Ret == SQLITE_OK);
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	m_pIDb->SQL(kCleanupDept);
	EIMLOGGER_DEBUG_(pILog, _T("End to write database of Dept"));

	return EIMERR_NO_ERROR;
}

BOOL C_eIMContactsMgr::_BindEmps(I_SQLite3Stmt* pIStmt, const USERDEPT& sUser, BOOL bUpdate, const char* pszPinyin, const char* pszPinyinS)
{
	int i32Ret = SQLITE_OK;

	i32Ret = pIStmt->Bind("@pinyin",	pszPinyin);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@pinyin_s",	pszPinyinS);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@eid_",		(int)sUser.dwUserID);		ASSERT_(i32Ret == SQLITE_OK);		
	i32Ret = pIStmt->Bind("@name",		sUser.aszCnUserName);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@ename",		sUser.aszEnUserName);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@ucode",		sUser.aszUserCode);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sex",		IME_GET_SEX_(sUser.cSex));	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@logo",		sUser.aszLogo);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",		(int)(2));					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@rank_id",	(int)sUser.cRankID);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@prof_id",	(int)sUser.cProfessionalID);ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@aera_id",	(int)sUser.dwAreaID);		ASSERT_(i32Ret == SQLITE_OK);
	if ( bUpdate )
		i32Ret = pIStmt->Bind("@qeid",		(int)sUser.dwUserID);	ASSERT_(i32Ret == SQLITE_OK);	// condition

	return (i32Ret == SQLITE_OK);
}


int C_eIMContactsMgr::Set(VectUserDept& vectUserDept_)
{
	int				i32Ret = 0;
	eIMStringA		szPinyinA;
	eIMStringA		szSearchPyA;
	eIMString		szValue;
	I_SQLite3Stmt*	pIStmt   = NULL;
	I_SQLite3Stmt*	pIStmt2  = NULL;
	I_SQLite3Stmt*	pIStmt3  = NULL;
	TCHAR			szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
	C_eIMEngine& eng = Eng;

	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPY_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceDeptEmp, pIStmt,  i32Ret);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertEmpDept, pIStmt2, i32Ret);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kUpdateEmpDept,  pIStmt3, i32Ret);
	
	VectUserDept vectUserDept;
	vectUserDept.swap(vectUserDept_);
	
	EIMLOGGER_DEBUG_(pILog, _T("Begin to write database of UserDept, Count:%u"), vectUserDept.size());
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	pIPy->SetFlag(m_i32Pinyin);
	if ( m_qeidLogin == 0 )
		m_qeidLogin = eng.GetPurview(GET_LOGIN_QEID);

	for ( VectUserDeptIt_ it = vectUserDept.begin(); it != vectUserDept.end(); it++)
	{	// contacts.t_dept_emp
		DWORD dwFlag = (it->wUpdate_type == 3 ? DB_FLAG_DELETED : it->wUpdate_type);
		if (dwFlag & DB_FLAG_DELETED)
		{
			int i = 0;
		}
		if ( m_qeidLogin == it->dwUserID && (dwFlag & DB_FLAG_DELETED) )
			 eng.SetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO, 1);

		i32Ret = pIStmt->Bind("@did_",		(int)it->dwDeptID);		ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@eid_",		(int)it->dwUserID);		ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@flag",		(int)dwFlag);			ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@orders",	(WORD)CHECK_MAP_NO_ORDERS(it->wSort)); ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@update_time",(int)it->dwUpdateTime); ASSERT_(i32Ret == SQLITE_OK);

		i32Ret = pIStmt->Step();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

		// contacts.t_emp
		//if ( it->wUpdate_type == 3 )	// DONT update t_emp when is delete t_dept_emp
			//continue;

		pIPy->GetUtf8Pinyins(it->aszCnUserName, szPinyinA, szSearchPyA);
		_BindEmps(pIStmt2, *it, FALSE, szPinyinA.c_str(), szSearchPyA.c_str());
		i32Ret = pIStmt2->Step();
		if ( i32Ret == SQLITE_CONSTRAINT )
		{	// Update
			_BindEmps(pIStmt3, *it, TRUE, szPinyinA.c_str(), szSearchPyA.c_str());
			i32Ret = pIStmt3->Step();
		}
	
		if ( i32Ret != SQLITE_OK )
		{
			EIMLOGGER_ERROR_(pILog, _T("did:%u, eid:%u, flag:%u, orders:%u, name:%s, ename:%S, ucode:%S, pinyin:%S, pinyin_s:%S, sex:%u, rank_id:%u, prof_id:%u, aera_id:%u"),
				it->dwDeptID, it->dwUserID, it->wUpdate_type, it->wSort, 
				(::eIMUTF8ToTChar(it->aszCnUserName, szValue), szValue.c_str()),
				it->aszEnUserName, it->aszUserCode, szPinyinA.c_str(), szSearchPyA.c_str(),
				IME_GET_SEX_(it->cSex), it->cRankID, it->cProfessionalID, it->dwAreaID);

			ASSERT_(FALSE);
			break;
		}
		else
		{
			EIMLOGGER_DATA_(pILog, NULL, 0, _T("did:%u,	eid:%u,	flag:%u, orders:%u, name:%s, ename:%S, ucode:%S, pinyin:%S, pinyin_s:%S, sex:%u, rank_id:%u, prof_id:%u, aera_id:%u"),
				it->dwDeptID, it->dwUserID, it->wUpdate_type, it->wSort, 
				(::eIMUTF8ToTChar(it->aszCnUserName, szValue), szValue.c_str()),
				it->aszEnUserName, it->aszUserCode, szPinyinA.c_str(), szSearchPyA.c_str(),
				IME_GET_SEX_(it->cSex), it->cRankID, it->cProfessionalID, it->dwAreaID);
		}
	}
	
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	i32Ret = m_pIDb->Commit();  ASSERT_(i32Ret == SQLITE_OK);
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	m_pIDb->SQL(kCleanupDeptEmp);
//	m_pIDb->SQL(kCleanupEmp);

	EIMLOGGER_DEBUG_(pILog, _T("End to write database of UserDept"));

	return EIMERR_NO_ERROR;
}


BOOL C_eIMContactsMgr::_BindUserInfo(I_SQLite3Stmt* pIStmt,  BOOL bUpdate,const USERINFO& UserInfo, const char* szPinyinA, const char* szSearchPyA)
{
    int i32Ret = SQLITE_OK;

    i32Ret = pIStmt->Bind("@pinyin",	szPinyinA);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@pinyin_s",  szSearchPyA);	ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@eid_",		(int)UserInfo.dwUserID);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@ucode",		UserInfo.aszUserCode);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@name",		UserInfo.aszCnUserName);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@ename",		UserInfo.aszEnUserName);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@sex",		IME_GET_SEX_(UserInfo.cSex));ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@flag",		(int)(UserInfo.wUpdate_type == 3 ? DB_FLAG_DELETED : UserInfo.wUpdate_type));	ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@duty",		UserInfo.aszPost);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@email",		UserInfo.aszEmail);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@tel",		UserInfo.aszTel);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@phone",		UserInfo.aszPhone);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@address",	UserInfo.aszAdrr);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@postalcode",UserInfo.aszPostcode);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@fax",		UserInfo.aszFax);			ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@update_time",(int)UserInfo.dwUpdateTime);ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@hiredate",	(int)UserInfo.dwHiredate);	ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@sign",		UserInfo.aszSign);			ASSERT_(i32Ret == SQLITE_OK);
    if ( bUpdate )
        i32Ret = pIStmt->Bind("@qeid",		(int)UserInfo.dwUserID);	ASSERT_(i32Ret == SQLITE_OK);	// condition

    return (i32Ret == SQLITE_OK);
}

int C_eIMContactsMgr::Set(VectUserInfo& vectUserInfo_)
{
	int			i32Ret = 0;
	UINT32		u32StartPos = 0;
	USERINFO	sUserInfo = { 0 };
	eIMStringA	szPinyinA;
	eIMStringA	szSearchPyA;
	eIMString	szName;
	eIMString	szDuty;
	eIMString	szAddress;
//	TCHAR		szBuf[CONVERT_TO_STRING_SIZE] = { 0 };

	I_SQLite3Stmt*	pIStmt   = NULL;
    I_SQLite3Stmt*	pIStmt1   = NULL;
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPY_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

    GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertUserInfo, pIStmt, i32Ret);
    GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kUpdateUserInfo,  pIStmt1, i32Ret);

	VectUserInfo vectUserInfo;
	vectUserInfo.swap(vectUserInfo_);

	EIMLOGGER_DEBUG_(pILog, _T("Begin to write database of User, Count:%u"), vectUserInfo.size());
	pIPy->SetFlag(m_i32Pinyin);
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	for ( VectUserInfoIt_ it = vectUserInfo.begin(); it != vectUserInfo.end(); it++ )
	{	// contacts.t_emp
		QEID qeid=it->dwUserID;
		pIPy->GetUtf8Pinyins(it->aszCnUserName, szPinyinA, szSearchPyA);
		
       _BindUserInfo(pIStmt, FALSE,*it, szPinyinA.c_str(), szSearchPyA.c_str());
       i32Ret = pIStmt->Step();
       if (i32Ret == SQLITE_CONSTRAINT)
       {
            _BindUserInfo(pIStmt1, TRUE, *it, szPinyinA.c_str(), szSearchPyA.c_str());
           i32Ret = pIStmt1->Step();
       }
		if ( i32Ret != SQLITE_OK )
		{
			EIMLOGGER_ERROR_(pILog, _T("eid:%u, ucode:%S, name:%s, ename:%S, sex:%u, flag:%u, duty:%s, email:%S, tel:%S, phone:%S, address:%s, postalcode:%S, fax:%S, update_time:%u, hiredate:%u, pinyin:%S, pinyin_s:%S, sign:%S"),
				it->dwUserID, it->aszUserCode, 
				(::eIMUTF8ToTChar(it->aszCnUserName, szName), szName.c_str()), it->aszEnUserName, 
				IME_GET_SEX_(it->cSex), it->wUpdate_type,
				(::eIMUTF8ToTChar(it->aszPost, szDuty), szDuty.c_str()), 
				it->aszEmail, it->aszTel, it->aszPhone,
				(::eIMUTF8ToTChar(it->aszAdrr, szAddress), szAddress.c_str()), 
				it->aszPostcode, it->aszFax, it->dwUpdateTime, it->dwHiredate,
				szPinyinA.c_str(), szSearchPyA.c_str(), it->aszSign);						
			break;									
		}
		else
		{
			EIMLOGGER_DATA_(pILog, NULL, 0, _T("eid:%u, ucode:%S, name:%s, ename:%S, sex:%u, flag:%u, duty:%s, email:%S, tel:%S, phone:%S, address:%s, postalcode:%S, fax:%S, update_time:%u, hiredate:%u, pinyin:%S, pinyin_s:%S"),
				it->dwUserID, it->aszUserCode, 
				(::eIMUTF8ToTChar(it->aszCnUserName, szName), szName.c_str()), it->aszEnUserName, 
				IME_GET_SEX_(it->cSex), it->wUpdate_type,
				(::eIMUTF8ToTChar(it->aszPost, szDuty), szDuty.c_str()), 
				it->aszEmail, it->aszTel, it->aszPhone,
				(::eIMUTF8ToTChar(it->aszAdrr, szAddress), szAddress.c_str()), 
				it->aszPostcode, it->aszFax, it->dwUpdateTime, it->dwHiredate,
				szPinyinA.c_str(), szSearchPyA.c_str());
		}
	}

	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	i32Ret = m_pIDb->Commit();  ASSERT_(i32Ret == SQLITE_OK);
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
//	m_pIDb->SQL(kCleanupEmp);
	EIMLOGGER_DEBUG_(pILog, _T("End to write database of User"));

	return EIMERR_NO_ERROR;
}

int C_eIMContactsMgr::Set(const EMPLOYEE* psEmp)
{
	int				i32Ret;
	I_SQLite3Stmt*	pIStmt   = NULL;
	TCHAR			szBuf[CONVERT_TO_STRING_SIZE] = { 0 };

	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	CHECK_NULL_RET_(psEmp, EIMERR_INVALID_PARAM);

	GET_IPY_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR); 
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceEmp, pIStmt, i32Ret);
	pIPy->SetFlag(m_i32Pinyin);
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	eIMStringA szPinyinA;
	eIMStringA szSearchPyA;
	pIPy->GetUtf8Pinyins(psEmp->tUserInfo.aszCnUserName, szPinyinA, szSearchPyA);
	i32Ret = pIStmt->Bind("@pinyin",	szPinyinA.c_str());					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@pinyin_s", szSearchPyA.c_str());				ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Bind("@eid_",		(int)psEmp->tUserInfo.dwUserID);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@ucode",		psEmp->tUserInfo.aszUserCode);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@name",		psEmp->tUserInfo.aszCnUserName);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@ename",		psEmp->tUserInfo.aszEnUserName);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sex",		IME_GET_SEX_(psEmp->tUserInfo.cSex));ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Bind("@flag",		(int)(psEmp->tUserInfo.wUpdate_type == 3 ? DB_FLAG_DELETED : psEmp->tUserInfo.wUpdate_type));	ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Bind("@duty",		psEmp->tUserInfo.aszPost);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@email",		psEmp->tUserInfo.aszEmail);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@tel",		psEmp->tUserInfo.aszTel);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@phone",		psEmp->tUserInfo.aszPhone);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@address",	psEmp->tUserInfo.aszAdrr);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@postalcode",psEmp->tUserInfo.aszPostcode);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@fax",		psEmp->tUserInfo.aszFax);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@update_time",(int)psEmp->tUserInfo.dwUpdateTime);ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@hiredate",	(int)psEmp->tUserInfo.dwHiredate);	ASSERT_(i32Ret == SQLITE_OK);

	// extend
	i32Ret = pIStmt->Bind("@home_tel",	psEmp->tUserExtend.aszHomeTel);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@emgr_phone",psEmp->tUserExtend.aszEmergencyphone);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@logo",		psEmp->tUserExtend.aszLogo);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sign",		psEmp->tUserExtend.aszSign);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@logotime",	(int)psEmp->tUserExtend.dwLogoUpdateTime);	ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();  ASSERT_(i32Ret == SQLITE_OK);
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return EIMERR_NO_ERROR;
}

int C_eIMContactsMgr::Set(const RESETSELFINFONOTICE* psInfo)
{
	int			i32Ret = EIMERR_NO_ERROR;
	const char* pszField = NULL;

	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	CHECK_NULL_RET_(psInfo, EIMERR_INVALID_PARAM);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	switch(psInfo->cModiType)
	{
	case 0:		// 性别 
		pszField = "sex";
		break;
	case 2:		// 出生日期 
		pszField = "birthday";
		break;
	case 3:
		pszField = "address";
		break;
	case 4:		// 办公电话号码 
		pszField = "tel";
		break;
	case 5:		// 手机号码 
		pszField = "phone";
		break;
	case 6:		// 密码 
		pszField = "psw";
		break;
	case 7:		// 头像ID 
		pszField = "logo";
		break;
	case 8:		// 个人签名 
		pszField = "sign";
		break;
//	case 9:		// 权限 
//		pszField = "**";
//		break;
	case 10:	// 宅电 
		pszField = "home_tel";
		break;
	case 11:	// 紧急联系手机  
		pszField = "emgr_tel";
		break;
	case 13:	// 职务
		pszField = "duty";
		break;
	case 14:	// 修改邮箱 
		pszField = "email";
		break;
	case 15:
		pszField = "logotime";
		break;
	case 100:	// 修改多项资料
	//	pszSql = pIDb->VMPrintf(kUpdateEmpFiledStr, "sex", m_sData.aszModiInfo, m_sData.dwUserID);
		break;
	default:
		ASSERT_(FALSE);
		EIMLOGGER_ERROR_(pILog, _T("Unknown modify type:%d"), psInfo->cModiType);
		break;
	}

	if (pszField)
	{
		const char* pszSql = m_pIDb->VMPrintf(kUpdateEmpFiledStr, pszField, psInfo->aszModiInfo, psInfo->dwUserID);
		i32Ret = EIMERR_DB_ERROR_(m_pIDb->SQL(pszSql, true));
	}

	return i32Ret;
}

int C_eIMContactsMgr::GetDeptMembers(QDID qdid, VectEmpId& vectEmpId)
{
	vectEmpId.clear();
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectDeptEmp, qdid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, EIMERR_GET_TABLE_FAILED);
	AUTO_RELEASE_(pITable);
	UINT64 v = 0;
	while( pITable->Step() == SQLITE_ROW)
	{
		v  = pITable->GetColInt(1);	// Sort
		v<<=32;
		v |= pITable->GetColInt(0);
		vectEmpId.push_back(v);
	}

	return EIMERR_NO_ERROR;
}

inline int C_eIMContactsMgr::_GetDept(I_SQLite3Table* pITable, QDID qdidPrev, S_DeptEmp& sDeptEmp, BOOL bDisablePy)
{
	if ( pITable->Step() == SQLITE_ROW )	
	{
		eIMString szValue;

		sDeptEmp.bitSorted			= 1;	// Already sorted
		sDeptEmp.sDeptInfo.Id		= pITable->GetColInt(0);
		sDeptEmp.sDeptInfo.pszName  = eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szValue), szValue.c_str()));
		sDeptEmp.sDeptInfo.pszNameEn= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szValue), szValue.c_str()));
		sDeptEmp.dwParentId			= pITable->GetColInt(3) ? pITable->GetColInt(3) : IME_ID_NONE;
		sDeptEmp.sDeptInfo.pszTel   = eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(4)), szValue), szValue.c_str()));
		sDeptEmp.pszNamePinyinS		= eIMAddString_(bDisablePy ? NULL : eIMCheckColTextNull(pITable->GetColText(7)));
		sDeptEmp.pszNamePinyin		= bDisablePy ? NULL : eIMCheckColTextNull(pITable->GetColText(6));
		if ( sDeptEmp.pszNamePinyin && sDeptEmp.pszNamePinyinS )
		{
			const char* pszFind = strstr(sDeptEmp.pszNamePinyinS, sDeptEmp.pszNamePinyin);
			
			if ( pszFind )
				sDeptEmp.pszNamePinyin = pszFind;
		}
		else
			sDeptEmp.pszNamePinyin	= eIMAddString_(sDeptEmp.pszNamePinyin);

		sDeptEmp.dwFirstChildId		= pITable->GetColInt(9) ? pITable->GetColInt(9) : IME_ID_NONE;
		sDeptEmp.dwPrevId			= qdidPrev;
		sDeptEmp.dwNextId			= IME_ID_NONE;
		sDeptEmp.sDeptInfo.dwUpdateTime = pITable->GetColInt(10);
//		sDeptEmp.dwEmpOnlineCount   = 0;
		
		GetDeptMembers(sDeptEmp.sDeptInfo.Id, sDeptEmp.vectEmpId);
		sDeptEmp.bitVisible = pITable->GetColInt(11); 

		return EIMERR_NO_ERROR;
	}

	return EIMERR_SQLITE_DONE;
}


int C_eIMContactsMgr::LoadContacts(BOOL* pbAbort)
{	// After finished DeptEmps
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	BOOL bDisablePy = m_i32Pinyin & IME_ATTRIB_PINYIN_CONSTS_X;
	PS_UpdateTTs_	psUpdateTTs = Eng.GetUpdateTimestamp();
	C_eIMContacts* pConts = static_cast<C_eIMContacts*>(pIConts);
	CHECK_NULL_RET_(psUpdateTTs, EIMERR_INVALID_POINTER);
	CHECK_NULL_RET_(pIConts, EIMERR_INVALID_POINTER);
	DWORD			dwLoadContactDeptTimestamp     = pConts->GetLoadContactTimestamp(TRUE);
	DWORD			dwLoadContactDeptUserTimestamp = pConts->GetLoadContactTimestamp(FALSE);
	if ( dwLoadContactDeptTimestamp != 0 && dwLoadContactDeptUserTimestamp != 0 && 
		 dwLoadContactDeptTimestamp >= psUpdateTTs->rUpdateTimeInfo.dwDeptUpdateTime &&
		 dwLoadContactDeptUserTimestamp >= psUpdateTTs->rUpdateTimeInfo.dwDeptUserUpdateTime )
	{
		EIMLOGGER_DEBUG_(pILog, _T("Not need to reload contact, LoadContactDeptTimestamp: %u, LastDeptUpdateTimestamp: %u, LoadContactDeptUserTimestamp: %u, LastDeptUserUpdateTimestamp: %u"),
			dwLoadContactDeptTimestamp, psUpdateTTs->rUpdateTimeInfo.dwDeptUpdateTime,
			dwLoadContactDeptUserTimestamp, psUpdateTTs->rUpdateTimeInfo.dwDeptUserUpdateTime);

		return EIMERR_CONSTACT_LOADED;
	}

	int			i32Ret;
	DWORD		dwCount     = 0;
	QDID		qdidPrev	= IME_ID_NONE;	// Last qdid
	QDID		qdidParent	= IME_ID_NONE;
	TCHAR		szBuf[CONVERT_TO_STRING_SIZE];
	DWORD		dwIndex		= 0;
	S_DeptEmp	sDeptEmp[2];
	S_LoginStatus sStatus = { EIMERR_LOAD_DEPT };

	GetEnvironmentVariable(IME_ATTRIB_CO_ID, szBuf, CONVERT_TO_STRING_SIZE);	// Set UID as environment variable
	const char* pszSql  = m_pIDb->VMPrintf(kSelectDept,Str2Int(szBuf, 0));
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, EIMERR_GET_TABLE_FAILED);
	AUTO_RELEASE_(pITable);
	SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
	EIMLOGGER_DEBUG_(pILog, _T("Begin"));
	if ( SUCCEEDED(i32Ret = _GetDept(pITable, qdidPrev, sDeptEmp[dwIndex], bDisablePy)) )	
	{
		pConts->Clear(IMECFLAG_CLEAR_DEPTS | IMECFLAG_CLEAR_DEPTS_SCACHE);	// Clear depts
		qdidPrev   = sDeptEmp[dwIndex].sDeptInfo.Id;
		qdidParent = sDeptEmp[dwIndex].dwParentId;
		pConts->SetVirtRootDepts(qdidPrev);

		dwIndex = 1;
		dwCount++;
		while( SUCCEEDED( i32Ret = _GetDept(pITable, qdidPrev, sDeptEmp[dwIndex], bDisablePy)) )
		{
			if ( sDeptEmp[dwIndex].dwParentId != qdidParent )
			{
				qdidPrev   = IME_ID_NONE;
				qdidParent = sDeptEmp[dwIndex].dwParentId;
			}
			else
			{
				qdidPrev = sDeptEmp[dwIndex].sDeptInfo.Id;
				sDeptEmp[!dwIndex].dwNextId = sDeptEmp[dwIndex].sDeptInfo.Id;
			}

			EIMLOGGER_DATA_(pILog, NULL, 0, _T("did:%u, pid:%u, Prev:%u, Next:%u, Child:%u, Name:%s, Visible:%u"),
				sDeptEmp[!dwIndex].sDeptInfo.Id, sDeptEmp[!dwIndex].dwParentId,
				sDeptEmp[!dwIndex].dwPrevId, sDeptEmp[!dwIndex].dwNextId,
				sDeptEmp[!dwIndex].dwFirstChildId, sDeptEmp[!dwIndex].sDeptInfo.pszName,
				sDeptEmp[!dwIndex].bitVisible);

			pConts->SetDepts(sDeptEmp[!dwIndex], eIMECSET_ADD);
			dwIndex = !dwIndex;
			dwCount++;
			if (pbAbort && *pbAbort)
			{
				EIMLOGGER_DEBUG_(pILog, _T("Aborted!"));
				return EIMERR_ABORT_CMD;
			}
		}

		EIMLOGGER_DATA_(pILog, NULL, 0, _T("did:%u, pid:%u, Prev:%u, Next:%u, Child:%u, Name:%s, Visible:%u"),
			sDeptEmp[!dwIndex].sDeptInfo.Id, sDeptEmp[!dwIndex].dwParentId,
			sDeptEmp[!dwIndex].dwPrevId, sDeptEmp[!dwIndex].dwNextId,
			sDeptEmp[!dwIndex].dwFirstChildId, sDeptEmp[!dwIndex].sDeptInfo.pszName,
			sDeptEmp[!dwIndex].bitVisible);

		pConts->SetDepts(sDeptEmp[!dwIndex], eIMECSET_ADD);
	}
	
	EIMLOGGER_DEBUG_(pILog, _T("End, Count:%u"), dwCount);
	if ( EIMERR_SQLITE_DONE == i32Ret )
		i32Ret = EIMERR_NO_ERROR;

	if ( dwCount == 0 )
		i32Ret = EIMERR_NO_INIT;
	else
	{
		pConts->SetLoadContactTimestamp(psUpdateTTs->rUpdateTimeInfo.dwDeptUpdateTime, TRUE);
		pConts->SetLoadContactTimestamp(psUpdateTTs->rUpdateTimeInfo.dwDeptUserUpdateTime, FALSE);
	}

	sStatus.dwStatus = EIMERR_LOAD_DEPT_OK;
	SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);

	return i32Ret;
}


inline BOOL C_eIMContactsMgr::_SetEmpInfo(I_SQLite3Table* pITable, S_EmpInfo& sEmpInfo, BOOL bDisablePy)
{
	eIMString		szValue;
	sEmpInfo.Id				= pITable->GetColInt(0);				// eid_,
	sEmpInfo.pszCode		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szValue), szValue.c_str()));
	sEmpInfo.pszName		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szValue), szValue.c_str()));
	sEmpInfo.pszEnName		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(27)), szValue), szValue.c_str()));
	sEmpInfo.pszNamePinyinS = eIMAddString_(bDisablePy ? NULL : eIMCheckColTextNull(pITable->GetColText(4)));		// pinyin_s,
	sEmpInfo.pszNamePinyin  = bDisablePy ? NULL : eIMCheckColTextNull(pITable->GetColText(3));
	if ( sEmpInfo.pszNamePinyin && sEmpInfo.pszNamePinyinS )
	{
		const char* pszFind = strstr(sEmpInfo.pszNamePinyinS, sEmpInfo.pszNamePinyin);
		if ( pszFind )
			sEmpInfo.pszNamePinyin = pszFind;
	}
	else
		sEmpInfo.pszNamePinyin = eIMAddString_(sEmpInfo.pszNamePinyin);

	sEmpInfo.dwBirthday		= pITable->GetColInt(5);				// birthday,
	sEmpInfo.bitSex			= IME_GET_SEX_(pITable->GetColInt(6));	// sex,

	sEmpInfo.pszDuty		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(7)), szValue), szValue.c_str()));
	sEmpInfo.pszEmail		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(8)), szValue), szValue.c_str()));
	sEmpInfo.pszTel			= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(9)), szValue), szValue.c_str()));
	sEmpInfo.pszHomeTel		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(10)), szValue), szValue.c_str()));
	sEmpInfo.pszPhone		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(11)), szValue), szValue.c_str()));
	sEmpInfo.pszEmrgTel		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(12)), szValue), szValue.c_str()));
//	sEmpInfo.pszLogo		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(13)), szValue), szValue.c_str()));
	sEmpInfo.pszSign		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(14)), szValue), szValue.c_str()));

	memset(sEmpInfo.qdidPid, 0xFF, sizeof(sEmpInfo.qdidPid));
	INT64 i64Flag			= pITable->GetColInt64(15);						// flag,
	sEmpInfo.bitIntegrity	= (i64Flag & IME_EMP_DB_INTEGERITY_MASK);		// Integerity flag
	sEmpInfo.bitConcern		= ((i64Flag & IME_EMP_DB_CONCERN_FLAG) == IME_EMP_DB_CONCERN_FLAG);	// Concern flag
	sEmpInfo.dwRankId		= pITable->GetColInt(16);						// rank_id,
	sEmpInfo.dwProfId		= pITable->GetColInt(17);						// prof_id,
	sEmpInfo.dwAeraId		= pITable->GetColInt(18);						// aera_id
	sEmpInfo.qdidPid[0]		= pITable->GetColInt(19);						// ParentID
	sEmpInfo.bitFixedOrders = pITable->GetColInt(20);//(pITable->GetColInt(20) != NO_ORDERS_VALUE);	// Fixed orders
	sEmpInfo.pszAddress		= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(21)), szValue), szValue.c_str()));
	sEmpInfo.pszPostalCode	= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(22)), szValue), szValue.c_str()));
	sEmpInfo.pszFax			= eIMAddString_((::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(23)), szValue), szValue.c_str()));
	sEmpInfo.dwLogoTime		= pITable->GetColInt(24);
	sEmpInfo.dwUpdateTime   = pITable->GetColInt(25);
	sEmpInfo.dwHiredate		= pITable->GetColInt(26);
	sEmpInfo.bitVisible		=(pITable->GetColInt(28) == DB_FLAG_SHOW_DEPTUSER);

	return TRUE;
}

int C_eIMContactsMgr::LoadEmps(BOOL* pbAbort, BOOL bUpdate)
{	// After finished Emps
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	BOOL bDisablePy = m_i32Pinyin & IME_ATTRIB_PINYIN_EMPS_X;
	PS_UpdateTTs_	psUpdateTTs = Eng.GetUpdateTimestamp();
	C_eIMContacts*  pConts  = static_cast<C_eIMContacts*>(pIConts);
	CHECK_NULL_RET_(pIConts, EIMERR_INVALID_POINTER);
	DWORD dwLoadEmpTimestamp = pConts->GetLoadEmpTimestamp();
	DWORD dwLoadContactDeptUserTimestamp = pConts->GetLoadContactTimestamp(FALSE);
	if ( dwLoadEmpTimestamp != 0 && dwLoadEmpTimestamp >= psUpdateTTs->rUpdateTimeInfo.dwUserUpdateTime &&	// 人员详情时间更新了
		 dwLoadContactDeptUserTimestamp != 0 && dwLoadContactDeptUserTimestamp >= psUpdateTTs->rUpdateTimeInfo.dwDeptUserUpdateTime	)	// 部门人员有变化时，也加载
	{
		EIMLOGGER_DEBUG_(pILog, _T("Not need to reload emps, LoadEmpTimestamp: 0x%08X, LastUserUpdateTimestamp: 0x%08X, LoadContactDeptUserTimestamp: 0x%08X, LastDeptUserUpdateTimestamp: 0x%08X"),
			dwLoadEmpTimestamp, psUpdateTTs->rUpdateTimeInfo.dwUserUpdateTime,
			dwLoadContactDeptUserTimestamp, psUpdateTTs->rUpdateTimeInfo.dwDeptUserUpdateTime);

		return EIMERR_NO_ERROR;
	}

	int				i32Ret;
	DWORD			dwCount  = 0;
	S_EmpInfo		sEmpInfo = { 0 };
	I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectEmpsPid);
	S_LoginStatus sStatus = { EIMERR_LOAD_EMP };
	CHECK_NULL_RET_(pITable, EIMERR_GET_TABLE_FAILED);
	AUTO_RELEASE_(pITable);
//Del 	pConts->Clear(IMECFLAG_CLEAR_EMPS | IMECFLAG_CLEAR_EMPS_SCACHE);	//Single line deleted by LongWQ on 2014/12/7 22:18:12 CAN NOT BE CLEAR it.
	EIMLOGGER_DEBUG_(pILog, _T("Begin %s"), bUpdate ? _T("Updating") : _T("Loading"));
	sEmpInfo.dwSize = sizeof(S_EmpInfo);
	SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
	while((i32Ret = pITable->Step()) == SQLITE_ROW)
	{
		_SetEmpInfo(pITable, sEmpInfo, bDisablePy);
		pConts->SetEmps(sEmpInfo, bUpdate ? eIMECSET_MOD : eIMECSET_ADD);
		dwCount++;
		
		EIMLOGGER_DATA_(pILog, NULL, 0, _T("eid:%u, pid:%u, Code:%s, Order:%u, Name:%s, Duty:%s, Email:%s, Tel:%s, HomeTel:%s, Phone:%s, EmrgTel:%s, Sign:%s, RandId:%u, ProfId:%u, AeraId:%u, Address:%s, PostalCode:%s, Fax:%s, Visible:%u, Pinyin:%S, PinyinS:%S"),
			sEmpInfo.Id, sEmpInfo.qdidPid[0], sEmpInfo.pszCode, sEmpInfo.bitFixedOrders, sEmpInfo.pszName, sEmpInfo.pszDuty, sEmpInfo.pszEmail, sEmpInfo.pszTel, sEmpInfo.pszHomeTel, sEmpInfo.pszPhone,
			sEmpInfo.pszEmrgTel, sEmpInfo.pszSign, sEmpInfo.dwRankId, sEmpInfo.dwProfId, sEmpInfo.dwAeraId, sEmpInfo.pszAddress, sEmpInfo.pszPostalCode, sEmpInfo.pszFax, 
			sEmpInfo.bitVisible, sEmpInfo.pszNamePinyin, sEmpInfo.pszNamePinyinS);

		if (pbAbort && *pbAbort)
		{
			EIMLOGGER_DEBUG_(pILog, _T("Aborted!"));
			return EIMERR_ABORT_CMD;
		}
	}

	EIMLOGGER_DEBUG_(pILog, _T("End, Count:%u, %s"), dwCount, bUpdate ? _T("Updating") : _T("Loading"));
	i32Ret = EIMERR_DB_ERROR_(i32Ret);
	if ( EIMERR_SQLITE_DONE == i32Ret )
		i32Ret = EIMERR_NO_ERROR;

	if ( dwCount == 0 )
		i32Ret = EIMERR_NO_INIT;
	else
		pConts->SetLoadEmpTimestamp(psUpdateTTs->rUpdateTimeInfo.dwUserUpdateTime);

	sStatus.dwStatus = EIMERR_LOAD_EMP_OK;
	SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);

	return i32Ret;
}

// 可用来加载个人信息
int C_eIMContactsMgr::LoadEmpInfo(QEID qeid, S_EmpInfo& sEmpInfo)				
{
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	BOOL bDisablePy = m_i32Pinyin & IME_ATTRIB_PINYIN_EMPS_X;
	C_eIMContacts*  pConts  = static_cast<C_eIMContacts*>(pIConts);
	CHECK_NULL_RET_(pIConts, EIMERR_INVALID_POINTER);

	int				i32Ret;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectEmpPidByID, qeid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, EIMERR_GET_TABLE_FAILED);
	AUTO_RELEASE_(pITable);

	sEmpInfo.dwSize = sizeof(S_EmpInfo);
	if((i32Ret = pITable->Step()) == SQLITE_ROW)
	{
		_SetEmpInfo(pITable, sEmpInfo, bDisablePy);
		pConts->SetEmps(sEmpInfo, eIMECSET_ADD);
		return EIMERR_NO_ERROR;
	}
	else
	{	// Initial to Empty string for avoid a NULL pointer when UI layer access it by CRT crashed
		sEmpInfo.pszAddress		= eIMAddString_((TCHAR*)NULL);
		sEmpInfo.pszCode		= sEmpInfo.pszAddress;
		sEmpInfo.pszDuty		= sEmpInfo.pszAddress;
		sEmpInfo.pszEmail		= sEmpInfo.pszAddress;
		sEmpInfo.pszEmrgTel		= sEmpInfo.pszAddress;
		sEmpInfo.pszEnName		= sEmpInfo.pszAddress;
		sEmpInfo.pszFax			= sEmpInfo.pszAddress;
		sEmpInfo.pszHomeTel		= sEmpInfo.pszAddress;
		sEmpInfo.pszName		= sEmpInfo.pszAddress;
		sEmpInfo.pszNamePinyin	= eIMAddString_((char*)NULL);
		sEmpInfo.pszNamePinyinS	= sEmpInfo.pszNamePinyin;
		sEmpInfo.pszPhone		= sEmpInfo.pszAddress;
		sEmpInfo.pszPostalCode	= sEmpInfo.pszAddress;
		sEmpInfo.pszSign		= sEmpInfo.pszAddress;
		sEmpInfo.pszTel			= sEmpInfo.pszAddress;
	}

	return EIMERR_DB_ERROR_(i32Ret);;
}


//=============================================================================
//Function:     GetTimestamp
//Description:	Get timestamps
//
//Parameter:
//	psUpdateTTs    - Return the timestamps
//	qeidLogin      - Logined eid
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
BOOL C_eIMContactsMgr::GetTimestamp(PS_UpdateTTs psUpdateTTs, QEID qeidLogin)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(psUpdateTTs, FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectTimestamp, qeidLogin);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if ( qeidLogin && m_qeidLogin != qeidLogin )
		m_qeidLogin = qeidLogin;

	if ( psUpdateTTs->dwTTSvrNow == 0 || psUpdateTTs->dwTTSysNow == 0 )
	{
		psUpdateTTs->dwTTSvrNow = (DWORD)time(NULL);
		psUpdateTTs->dwTTSysNow = GetTickCount();
        EIMLOGGER_DEBUG_(pILog, _T("dwTTSvrNow==0 || dwTTSysNow ==0") );
	}

	TUpdateTimeStamp& sUTS = psUpdateTTs->rUpdateTimeInfo;
	if ( SQLITE_ROW == pITable->Step() )
	{
		sUTS.dwCompUpdateTime			  = pITable->GetColInt(0);		// co,
		sUTS.dwDeptUpdateTime			  = pITable->GetColInt(1);		// dept,
		sUTS.dwUserUpdateTime			  = pITable->GetColInt(2);		// emps, 
		sUTS.dwDeptUserUpdateTime		  = pITable->GetColInt(3);		// dept_emps, 

		sUTS.dwPersonalInfoUpdateTime			= pITable->GetColInt(4);		// personal_info, 
		sUTS.dwPersonalCommonContactUpdateTime	= pITable->GetColInt(5);		// comcontact, 
		sUTS.dwPersonalCommonDeptUpdateTime		= pITable->GetColInt(6);		// comdept, 
		sUTS.dwPersonalAttentionUpdateTime		= pITable->GetColInt(7);		// personal_attention, 
		sUTS.dwGlobalCommonContactUpdateTime	= pITable->GetColInt(8);		// globalcomcontact, 
		sUTS.dwOthersAvatarUpdateTime			= pITable->GetColInt(9);		// others_avatar, 
		sUTS.dwPersonalAvatarUpdateTime			= pITable->GetColInt(10);		// personal_avatar, 

		sUTS.dwRegularGroupUpdateTime			= pITable->GetColInt(11);		// vgts,
		sUTS.dwUserRankUpdateTime				= pITable->GetColInt(12);		// emp_rank, 
		sUTS.dwUserProUpdateTime				= pITable->GetColInt(13);		// emp_duty, 
		sUTS.dwUserAreaUpdateTime				= pITable->GetColInt(14);		// emp_area, 

		sUTS.dwSpecialListUpdatetime			= pITable->GetColInt(15);		// speciallist, 
		sUTS.dwSpecialWhiteListUpdatetime		= pITable->GetColInt(16);		// whitelist, 
		psUpdateTTs->dwRobotInfoUpdatetime		= pITable->GetColInt(17);		// robot, 
		psUpdateTTs->dwTTShowDeptConfig			= pITable->GetColInt(18);		// dwTTShowDeptConfig

		EIMLOGGER_DEBUG_(pILog, _T("CompUpdateTime:%u, DeptUpdateTime:%u, UserUpdateTime:%u, DeptUserUpdateTime:%u,")
			_T("PersonalInfoUpdateTime:%u, PersonalCommonContactUpdateTime:%u, PersonalCommonDeptUpdateTime:%u,")
			_T("PersonalAttentionUpdateTime:%u, GlobalCommonContactUpdateTime:%u, OthersAvatarUpdateTime:%u,")
			_T("PersonalAvatarUpdateTime:%u, RegularGroupUpdateTime:%u, UserRankUpdateTime:%u, UserProUpdateTime:%u,")
			_T("UserAreaUpdateTime:%u, SpecialListUpdatetime:%u, SpecialWhiteListUpdatetime:%u, robottime:%u,")
			_T("ServerNow:%u, SystemNowTick:%u, TTShowDeptConfig:%u"),
			sUTS.dwCompUpdateTime, sUTS.dwDeptUpdateTime, sUTS.dwUserUpdateTime, sUTS.dwDeptUserUpdateTime,
			sUTS.dwPersonalInfoUpdateTime, sUTS.dwPersonalCommonContactUpdateTime, sUTS.dwPersonalCommonDeptUpdateTime,
			sUTS.dwPersonalAttentionUpdateTime,	sUTS.dwGlobalCommonContactUpdateTime, sUTS.dwOthersAvatarUpdateTime,	
			sUTS.dwPersonalAvatarUpdateTime, sUTS.dwRegularGroupUpdateTime, sUTS.dwUserRankUpdateTime,				
			sUTS.dwUserProUpdateTime, sUTS.dwUserAreaUpdateTime, sUTS.dwSpecialListUpdatetime, 
			sUTS.dwSpecialWhiteListUpdatetime, psUpdateTTs->dwRobotInfoUpdatetime, psUpdateTTs->dwTTSvrNow,
			psUpdateTTs->dwTTSysNow, psUpdateTTs->dwTTShowDeptConfig);

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//Function:     SetTimestamp
//Description:	Set timestamp
//
//Parameter:
//	psUpdateTTs    - Set update timestamps
//	qeidLogin      - Logined eid
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- Failed
//=============================================================================
int C_eIMContactsMgr::SetTimestamp(PS_UpdateTTs psUpdateTTs, QEID qeidLogin)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(psUpdateTTs, FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	TUpdateTimeStamp& sUTS = psUpdateTTs->rUpdateTimeInfo;
	
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceTimestamp, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@eid_",				(int)qeidLogin);								ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@co",				(int)sUTS.dwCompUpdateTime);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@dept",				(int)sUTS.dwDeptUpdateTime);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@emps",				(int)sUTS.dwUserUpdateTime);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@dept_emps",			(int)sUTS.dwDeptUserUpdateTime);				ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Bind("@personal_info",		(int)sUTS.dwPersonalInfoUpdateTime);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@comcontact",		(int)sUTS.dwPersonalCommonContactUpdateTime);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@comdept",			(int)sUTS.dwPersonalCommonDeptUpdateTime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@personal_attention",(int)sUTS.dwPersonalAttentionUpdateTime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@globalcomcontact",	(int)sUTS.dwGlobalCommonContactUpdateTime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@others_avatar",		(int)sUTS.dwOthersAvatarUpdateTime);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@personal_avatar",	(int)sUTS.dwPersonalAvatarUpdateTime);			ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Bind("@vgts",				(int)sUTS.dwRegularGroupUpdateTime);			ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@emp_rank",			(int)sUTS.dwUserRankUpdateTime);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@emp_duty",			(int)sUTS.dwUserProUpdateTime);					ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@emp_area",			(int)sUTS.dwUserAreaUpdateTime);				ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Bind("@speciallist",		(int)sUTS.dwSpecialListUpdatetime);				ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@specialwhitelist",	(int)sUTS.dwSpecialWhiteListUpdatetime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@robot",				(int)psUpdateTTs->dwRobotInfoUpdatetime);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@showdept",			(int)psUpdateTTs->dwTTShowDeptConfig);			ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	EIMLOGGER_DEBUG_(pILog, _T("CompUpdateTime:%u, DeptUpdateTime:%u, UserUpdateTime:%u, DeptUserUpdateTime:%u,")
		_T("PersonalInfoUpdateTime:%u, PersonalCommonContactUpdateTime:%u, PersonalCommonDeptUpdateTime:%u,")
		_T("PersonalAttentionUpdateTime:%u, GlobalCommonContactUpdateTime:%u, OthersAvatarUpdateTime:%u,")
		_T("PersonalAvatarUpdateTime:%u, RegularGroupUpdateTime:%u, UserRankUpdateTime:%u, UserProUpdateTime:%u,")
		_T("UserAreaUpdateTime:%u, SpecialListUpdatetime:%u, SpecialWhiteListUpdatetime:%u,Robot:%u,")
		_T("ServerNow:%u, SystemNowTick:%u, showdept:%u"),
		sUTS.dwCompUpdateTime, sUTS.dwDeptUpdateTime, sUTS.dwUserUpdateTime, sUTS.dwDeptUserUpdateTime,
		sUTS.dwPersonalInfoUpdateTime, sUTS.dwPersonalCommonContactUpdateTime, sUTS.dwPersonalCommonDeptUpdateTime,
		sUTS.dwPersonalAttentionUpdateTime,	sUTS.dwGlobalCommonContactUpdateTime, sUTS.dwOthersAvatarUpdateTime,	
		sUTS.dwPersonalAvatarUpdateTime, sUTS.dwRegularGroupUpdateTime, sUTS.dwUserRankUpdateTime,				
		sUTS.dwUserProUpdateTime, sUTS.dwUserAreaUpdateTime, sUTS.dwSpecialListUpdatetime, 
		sUTS.dwSpecialWhiteListUpdatetime, psUpdateTTs->dwRobotInfoUpdatetime, psUpdateTTs->dwTTSvrNow, 
		psUpdateTTs->dwTTSysNow, psUpdateTTs->dwTTShowDeptConfig);

	return EIMERR_NO_ERROR;
}

int C_eIMContactsMgr::GetEmpLogotimeInfo(QEID qeid)
{
	int iTimeInfo = 0;
	CHECK_INIT_RET_(FALSE);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectEmpLogoTime, qeid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if ( SQLITE_ROW == pITable->Step() )
	{
		iTimeInfo = pITable->GetColInt(0);
	}
	return iTimeInfo;
}

int C_eIMContactsMgr::SetEmpLogotimeInfo(QEID qeid, int iLogotimInfo)
{
	CHECK_INIT_RET_(FALSE);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	const char* pszSql = m_pIDb->VMPrintf(kUpdateEmpLogoTime, iLogotimInfo, qeid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	if(SQLITE_OK != i32Ret) return FALSE;

	S_EmpInfo sUpdateEmpInfo;
	ZERO_STRUCT_(sUpdateEmpInfo);
	sUpdateEmpInfo.Id = qeid;
	sUpdateEmpInfo.dwLogoTime = iLogotimInfo;
	C_eIMContacts*  pConts  = static_cast<C_eIMContacts*>(pIConts);
	pConts->SetEmps(sUpdateEmpInfo,eIMECSET_MODS_UPDATE_LOGO,FALSE);

	return TRUE;
}

int C_eIMContactsMgr::SetEmpsAllLogotime(PQEID pallid, int icount, int iLogotimInfo)
{
	CHECK_INIT_RET_(FALSE);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	CHECK_NULL_RET_(pallid, FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	eIMStringA salleid;
	char szTemp[10] = "";
	for (int iIndex = 0; iIndex < icount; iIndex++)
	{
		itoa(pallid[iIndex], szTemp, 10);
		salleid += szTemp;
		salleid += ",";
	}
	int ilen = strlen(salleid.c_str());
	if(ilen > 0)
	{
		salleid.resize(ilen -1, 0);
	}

	const char* pszSql = m_pIDb->VMPrintf(kUpdateMutiEmpLogoTime, iLogotimInfo,  salleid.c_str());
	int i32Ret = m_pIDb->SQL(pszSql, true);

	if(i32Ret != SQLITE_OK)
    {
        EIMLOGGER_ERROR_( pILog, _T("SetEmpsAllLogotime fail count:%d"), icount);
        return FALSE;
    }

	for (int i32Ind = 0; i32Ind < icount; i32Ind++)
	{
		S_EmpInfo sUpdateEmpInfo;
		ZERO_STRUCT_(sUpdateEmpInfo);
		sUpdateEmpInfo.Id = pallid[i32Ind];
		sUpdateEmpInfo.dwLogoTime = iLogotimInfo;
		C_eIMContacts*  pConts  = static_cast<C_eIMContacts*>(pIConts);
		pConts->SetEmps(sUpdateEmpInfo,eIMECSET_MODS_UPDATE_LOGO,FALSE);
		EIMLOGGER_DATA_( pILog, NULL, 0, _T("(**Head img**) Need update empid:[%u]"), sUpdateEmpInfo.Id);
	}

	return (i32Ret==SQLITE_OK) ? TRUE : FALSE;
}


BOOL C_eIMContactsMgr::Clear(DWORD dwFlag)
{
	CHECK_INIT_RET_(FALSE);

	if (CLEAR_FLAG_DEPT	& dwFlag )
	{
	//	m_pIDb->SQL(kDropBkupDept);
	//	m_pIDb->SQL(kBkupDept);
		m_pIDb->SQL(kClearDept);
	}

	if( CLEAR_FLAG_DEPT_EMPS & dwFlag )
	{
	//	m_pIDb->SQL(kDropBkupDeptEmp);
	//	m_pIDb->SQL(kBkupDeptEmp);
		m_pIDb->SQL(kClearDeptEmp);
	}

	if (CLEAR_FLAG_INVALID_DEPT_EMPS & dwFlag)
	{
		m_pIDb->SQL(kClearInValidDeptEmps);
	}

	if ( CLEAR_FLAG_EMPS & dwFlag )
	{
	//	m_pIDb->SQL(kDropBkupEmp);
	//	m_pIDb->SQL(kBkupEmp);
	//	m_pIDb->SQL(kClearEmp);
	}

	if ( CLEAR_FLAG_LOGO_TIME & dwFlag )
	{
		m_pIDb->SQL(kUpdateAllLogoTime);
	}

	return TRUE;
}

BOOL C_eIMContactsMgr::GetReloadContactsInfo(PS_UpdateContsInfo psInfo)
{
	CHECK_NULL_RET_(psInfo, FALSE);
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	I_SQLite3Table* pITable = NULL;
	if ( psInfo->dwId )
	{
		char* pszSQL = m_pIDb->VMPrintf(kSelectUpdateContactsInfoById, psInfo->dwId);
		pITable = m_pIDb->GetTable(pszSQL, true);
	}
	else
		pITable = m_pIDb->GetTable(kSelectUpdateContactsInfo, false);

	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if ( SQLITE_ROW == pITable->Step() )
	{
		EIMLOGGER_DEBUG_(pILog, _T("id:%u, type:%u, counter:%u, flag:%u,"), psInfo->dwId, psInfo->dwType, psInfo->dwElapse, psInfo->dwFlag);

		psInfo->dwId	 = pITable->GetColInt(0);
		psInfo->dwType	 = pITable->GetColInt(1);
		psInfo->dwElapse = pITable->GetColInt(2);
		psInfo->dwFlag	 = pITable->GetColInt(3);
		return TRUE;
	}

	return FALSE;
}

int	C_eIMContactsMgr::SetReloadContactsInfo(DWORD dwId, DWORD dwType, DWORD dwElapse, DWORD dwFlag)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int				i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceUpdateContactsInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@id_",		(int)dwId);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@type",		(int)dwType);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@elapse",	(int)dwElapse);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",		(int)dwFlag);	ASSERT_(i32Ret == SQLITE_OK);
	
	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	EIMLOGGER_DEBUG_(pILog, _T("id:%u, type:%u, elapse:%u, flag:%u,"), dwId, dwType, dwElapse, dwFlag);

	return EIMERR_NO_ERROR;
}

int C_eIMContactsMgr::Set(VectDeptShowInfo& vectDeptShowInfo_, int i32DefaultFlag)
{
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
	
	// Set default flag
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	VectDeptShowInfo vectDeptShowInfo;
	vectDeptShowInfo.swap(vectDeptShowInfo_);

	EIMLOGGER_DEBUG_(pILog, _T("Begin to write database of DeptShow default:%u, Count:%u"), i32DefaultFlag, vectDeptShowInfo.size());

	char* pszSQL = m_pIDb->VMPrintf(kUpdateDeptUserShowDefalutFlag, i32DefaultFlag);
	m_pIDb->SQL(pszSQL, true);

	m_pIDb->Begin();
	for ( VectDeptShowInfoIt_ it = vectDeptShowInfo.begin(); it != vectDeptShowInfo.end(); it++ )
	{
		pszSQL = m_pIDb->VMPrintf(kUpdateDeptUserShowFlag, it->cShowLevel, it->dwDeptID );
		m_pIDb->SQL(pszSQL, true);
	}
	m_pIDb->Commit();
	EIMLOGGER_DEBUG_(pILog, _T("End to write database of DeptShow"));

	return EIMERR_NO_ERROR;
}
