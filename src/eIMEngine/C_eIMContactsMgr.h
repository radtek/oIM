#pragma once

#define IME_EMP_DB_INTEGERITY_MASK		( 0x00000007 )	// See IMP_EMP_INTEGRITY_*
#define IME_EMP_DB_CONCERN_FLAG			( 0x00000008 )	// 1-Is concern

#define HEAD_IMAGE_NEEDDOWNLOAD		0
#define HEAD_IMAGE_DOWNLOAD_OK		1

#define CLEAR_FLAG_DEPT				( 0x00000001 )	// Clear Dept
#define CLEAR_FLAG_DEPT_EMPS		( 0x00000002 )	// Clear dept.emps
#define CLEAR_FLAG_EMPS				( 0x00000004 )	// Clear emps
#define CLEAR_FLAG_INVALID_DEPT_EMPS ( 0x00000008 )  // Clear Invalid dept.emps
#define CLEAR_FLAG_ALL				(CLEAR_FLAG_DEPT | CLEAR_FLAG_DEPT_EMPS | CLEAR_FLAG_EMPS)
#define CLEAR_FLAG_LOGO_TIME		( 0x00000010 )	// 清除头像时间

typedef std::vector<DEPTINFO>			VectDeptInfo;
typedef VectDeptInfo::iterator			VectDeptInfoIt;
typedef VectDeptInfo::const_iterator	VectDeptInfoIt_;

typedef std::vector<USERDEPT>			VectUserDept;
typedef VectUserDept::iterator			VectUserDeptIt;
typedef VectUserDept::const_iterator	VectUserDeptIt_;

typedef std::vector<USERINFO>			VectUserInfo;
typedef VectUserInfo::iterator			VectUserInfoIt;
typedef VectUserInfo::const_iterator	VectUserInfoIt_;

typedef std::vector<SINGLEDEPTSHOWLEVEL>	VectDeptShowInfo;
typedef VectDeptShowInfo::iterator			VectDeptShowInfoIt;
typedef VectDeptShowInfo::const_iterator	VectDeptShowInfoIt_;

class C_eIMContactsMgr
{
private:
	I_SQLite3*	m_pIDb;
	QEID		m_qeidLogin;	// Login's qeid
	int			m_i32Pinyin;	

protected:
	inline BOOL _SetEmpInfo(I_SQLite3Table* pITable, S_EmpInfo& sEmpInfo, BOOL bDisablePy=FALSE);
	inline int  _GetDept(I_SQLite3Table* pITable, QDID qdidPrev, S_DeptEmp& sDeptEmp, BOOL bDisablePy=FALSE);
	inline BOOL	_BindEmps(I_SQLite3Stmt* pIStmt, const USERDEPT& sUser, BOOL bUpdate, const char* pszPinyin=NULL, const char* pszPinyinS=NULL);
	inline BOOL _BindDept(I_SQLite3Stmt* pIStmt, const DEPTINFO& sDept, DWORD dwCoId, BOOL bUpdate, const char* pszPinyin, const char* pszPinyinS);
    inline BOOL _BindUserInfo(I_SQLite3Stmt* pIStmt, BOOL bUpdate,const USERINFO&  UserInfo, const char* szPinyinA, const char* szSearchPyA);
public:
	C_eIMContactsMgr(void);
	~C_eIMContactsMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	int Set(VectDeptInfo& vectDeptInfo);
	int Set(VectUserDept& vectUserDept);
	int Set(VectUserInfo& vectUserInfo);
	int Set(VectDeptShowInfo& vectDeptShowInfo, int i32DefaultFlag);
	int Set(const EMPLOYEE* psEmp);
	int Set(const RESETSELFINFONOTICE* psInfo);

	int GetDeptMembers(QDID qdid, VectEmpId& vectEmpId);
	int		LoadContacts(BOOL* pbAbort = NULL);
	int		LoadEmps(BOOL* pbAbort = NULL, BOOL bUpdate = FALSE);
	int		LoadEmpInfo(QEID, S_EmpInfo& sEmpInfo);				// 可用来加载个人信息
	int		UpdateEmp(USERINFO& sUserInfo);
	BOOL	Clear(DWORD dwFlag = CLEAR_FLAG_ALL);

	BOOL	GetTimestamp(PS_UpdateTTs psUpdateTTs, QEID qeidLogin);
	int		SetTimestamp(PS_UpdateTTs psUpdateTTs, QEID qeidLogin);

	BOOL	GetReloadContactsInfo(PS_UpdateContsInfo psInfo);	// Get the last reload contacts information
	int		SetReloadContactsInfo(DWORD dwId, DWORD dwType, DWORD dwCounter, DWORD dwFlag);	// Update the specify reload contacts information

	int GetEmpLogotimeInfo(QEID qeid);
	int SetEmpLogotimeInfo(QEID qeid, int iLogotimInfo);
	int SetEmpsAllLogotime(PQEID pallid, int icount, int iLogotimInfo);
};

