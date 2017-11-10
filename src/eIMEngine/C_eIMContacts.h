#pragma once

#include "eIMEngine\IeIMContacts.h"

#define GET_QEID_(V)	((QEID)(V))
#define GET_ORDER_(V)	((DWORD)((V) >> 32))


// Clear flag==================================================================
#define IMECFLAG_CLEAR_EMPS			( 0x00000001 )	// Clear employees
#define IMECFLAG_CLEAR_DEPTS		( 0x00000002 )	// Clear departments
#define IMECFLAG_CLEAR_EMPS_SCACHE	( 0x00000004 )	// Clear employee search cache
#define IMECFLAG_CLEAR_DEPTS_SCACHE	( 0x00000008 )	// Clear departments search cache

// Ref
#define IME_REF_MAX					( 7	)			// Reference 

// Clear all search cache
#define IMECFLAG_CLEAR_SCACHE_ALL	( IMECFLAG_CLEAR_EMPS_SCACHE | IMECFLAG_CLEAR_DEPTS_SCACHE )
// Clear all data
#define IMECFLAG_CLEAR_ALL			( IMECFLAG_CLEAR_EMPS | IMECFLAG_CLEAR_DEPTS | IMECFLAG_CLEAR_EMPS_SCACHE | IMECFLAG_CLEAR_DEPTS_SCACHE )

// 3(hz) * 6(pinyin) * 2(search & sort full spell) + 4(abbr) = ~40
// 2MB / 40 = ~5,2000(emps)
//#define IME_EMP_PINYIN_BUF_BLOCK_SIZE	( 2 * 1024 * 1024 )	// Employee Pinyin buffer block size, default as 2MB
//#define IME_DEPT_PINYIN_BUF_BLOCK_SIZE	( 1 * 1024 * 1024 ) // Department Pinyin buffer block size, default as 5K

#pragma pack( push, 8 )

// eIM Engine Conttacts set operate mode
typedef enum tagIMECSetMode
{
	eIMECSET_ADD = 0x1,				// Add department-employee or employee
	eIMECSET_DEL = 0x2,				// Delete department-employee or employee
	eIMECSET_MOD = 0x4,				// Modify department-information or employee
	eIMECSET_MODS = 0x8,			// Modify employee online status and terminal type

	eIMECSET_MODS_CONCERN		= 0x18,	// mods -- Modify concern emp flag
	eIMECSET_MODS_UPDATE_LOGO	= 0x28, // mods -- Modity emp logo flag
}E_IMECSMode, *PE_IMECSMode;

typedef std::vector< UINT64 >	VectEmpId;		// Employee Id vector of department, LODWORD: QEID, HIDWORD: Order(Only when EnumEmps) or 0
typedef VectEmpId::iterator		VectEmpIdIt;	// Iterator of Employee Id vector
typedef struct tagDeptEmp
{
	S_DeptInfo	sDeptInfo;			// Department information
	const char*	pszNamePinyin;		// Name's pinyin for sort
	const char*	pszNamePinyinS;		// Name's pinyin for search
	DWORD		dwParentId;			// Parent Id
	DWORD		dwFirstChildId;		// First Child Id
	DWORD		dwPrevId;			// Prev Id
	DWORD		dwNextId;			// Next Id
	DWORD		bitSorted:1;		// Sort flag: 1 - Sorted
	DWORD		bitVisible:2;		// 0- Hide, 1- Show department, 2- Show department and employee
	DWORD		dwPurview;			// 权限（在搜索，枚举时，保存通过权限检查回调函数结果）
	VectEmpId	vectEmpId;			// Department's employee Id list	
}S_DeptEmp, *PS_DeptEmp;
typedef const struct tagDeptEmp* PS_DeptEmp_;

#pragma pack( pop )

// Department's employee map < DeptId, S_DeptEmp >
typedef std::map< QDID, S_DeptEmp >		MapDeptEmp;				// Department map
typedef MapDeptEmp::iterator			MapDeptEmpIt;			// Iterator of department map, 
typedef std::pair< QDID, S_DeptEmp >	PairDeptEmp;			// Pair of DeptEmp

// Employee map < EmpId, PS_EmpInfo >
typedef std::map< QEID, PS_EmpInfo >	MapEmp;					// Employee map
typedef MapEmp::iterator				MapEmpIt;				// Iterator of employee map, memory table of employee
typedef std::pair< QEID, PS_EmpInfo >	PairEmp;				// Pair emp

// Cache of search employee 
typedef std::vector< PS_EmpInfo_ >		VectSearchEmpsCache;	// Search employee result cache for speed up
typedef VectSearchEmpsCache::iterator	VectSearchEmpsCacheIt;	// Iterator of search employee result cache

// Cache of search department
typedef std::vector< PS_DeptInfo_ >		VectSearchDeptsCache;	// Search department result cache for speed up
typedef VectSearchDeptsCache::iterator	VectSearchDeptsCacheIt;	// Iterator of search department result cache


class C_eIMContacts : public I_EIMContacts
{
private:
	S_CoInfo	m_sCoInfo;
	DWORD		m_dwSearchDeptLastTime;			// Last time of search department
	DWORD		m_dwSearchEmpLastTime;			// Last time of search employee
	DWORD		m_dwLoadContactDeptTimestamp;	// Last time load contact time
	DWORD		m_dwLoadContactDeptUserTimestamp;// Last time load contact time
	DWORD		m_dwLoadEmpTimestamp;			// Last time load employee time
	CRITICAL_SECTION m_csConts;
	I_EIMEventMgr*	m_pIEIMEventMgr;			// Event manager

	MapDeptEmp	m_mapDeptEmp;					// Memory table of department <--> employee
	MapEmp		m_mapEmp;						// Memory table of employee

	TCHAR					m_szSearchEmpsCond[IME_SEARCH_MAX_SIZE];	// Search emp's condition cache
	TCHAR					m_szSearchDeptsCond[IME_SEARCH_MAX_SIZE];	// Search dept's condition cache
	VectSearchEmpsCache		m_vectSearchEmpsCache;	// Search employee cache for speed up
	VectSearchDeptsCache	m_vectSearchDeptsCache;	// Search department cahce for speed up
	
	int m_i32Language;

	void*					m_pvCheckDeptPurviewUserData;
	PFN_CheckDeptPurviewCb	m_pfnCheckDeptPurviewCb;

	void*					m_pvCheckEmpPurviewUserData;
	PFN_CheckEmpPurviewCb	m_pfnCheckEmpPurviewCb;

protected:
	C_eIMContacts(void);

	inline BOOL   _InitPid(QDID qdidPid[IME_MAX_PID], QDID qdid);


	inline BOOL _UpdateEmpPid(
		QDID qdidPid[IME_MAX_PID],
		QDID qdid,
		BOOL bAdd
		);

	inline BOOL _UpdateDeptEmployee(
		DWORD	dwDeptId,
		DWORD	dwEmpId,
		BOOL	bAdd
		);

	inline BOOL _AddDepts(
		S_DeptEmp&	sDeptEmp
		);

	inline BOOL _DelDepts(
		S_DeptEmp&	sDeptEmp
		);

	inline BOOL _ModDepts(
		S_DeptEmp&	sDeptEmp
		);

	inline BOOL _SetSearchDeptsCache(
		VectDepts&	vectDepts, 
		const TCHAR* pszCond
		);

	inline BOOL _CompareDeptsCond(
		PS_DeptEmp_	psDeptEmp,
		const TCHAR* ptszCond,
		const char*	 pszCond
		);

	inline BOOL _SetSearchEmpsCache(
		VectEmps&	vectEmps, 
		const TCHAR* pszCond
		);

	inline DWORD _CompareEmpsCond(
		PS_EmpInfo_	psEmpInfo,
		const TCHAR* ptszCond,
		const char*	 pszCond,
		DWORD		dwFlag
		);

	inline BOOL _CanSearchAscii(
		const TCHAR*	pszCond,
		char	szSearchEmpsCond[IME_SEARCH_MAX_SIZE]
	);

	inline DWORD _GetOnline(
		MapEmp&		mapEmps, 	
		DWORD*		pdwEmpCount,			
		PFN_EmpCb	pfnEmpCb,
		void*		pvUserData
		);

	inline BOOL _GetDeptEmps(
		MapEmp&		mapEmps, 
		PS_DeptEmp	psItem,
		int			i32Level
		);

	inline BOOL _SortEmps(VectEmpId& vectEmpId);
	inline BOOL _CheckEmpsNullString(S_EmpInfo& sEmp);
	
	inline DWORD _CheckDeptPurview(PS_DeptInfo_ psDeptInfo);
	inline DWORD _CheckEmpPurview(PS_EmpInfo_ psEmpInfo);

public:

	DECALRE_PLUGIN_(C_eIMContacts)

	virtual ~C_eIMContacts(void);
	static C_eIMContacts& GetObject();

	DWORD GetLoadEmpTimestamp();
	DWORD SetLoadEmpTimestamp(DWORD dwTime);

	DWORD GetLoadContactTimestamp(BOOL bDept);
	DWORD SetLoadContactTimestamp(DWORD dwTime, BOOL bDept);

	BOOL SetEventMgr(I_EIMEventMgr* pIEIMEventMgr);
	BOOL SetCoInfo(PS_CoInfo psCoInfo);
 	BOOL SetVirtRootDepts(DWORD	dwFirstChildId );
	BOOL SetDepts(S_DeptEmp& sDeptEmp, E_IMECSMode eMode, BOOL bNotify = FALSE);
	BOOL SetEmps(S_EmpInfo&	sEmp, E_IMECSMode eMode, BOOL bNotify = FALSE);
	BOOL Clear( DWORD dwFlag );
	BOOL UpdateEmpsState(DWORD dwModify, DWORD dwValue);


	///////////////////////////////////////////////////////////////////////////
	virtual DWORD	FormatError(
		DWORD dwError, 
		TCHAR* pszError
		);

	virtual BOOL SetEmpState(QEID qeid, DWORD bitStatus, DWORD bitLoginType);

	virtual PS_CoInfo_	GetCompanyInfo();
	
	virtual PS_DeptInfo_ GetDeptItem(
		PS_DeptInfo_	psItem,
		E_GDIMethod		eGDIMethod
		);

	virtual PS_DeptInfo_	GetDeptItem(
		QDID	DeptId,
		E_GDIMethod eGDIMethod
		);

	virtual PS_EmpInfo_	GetEmp(
		QEID	EmpId
		);	

	virtual PS_DeptInfo_	GetEmpDept(
		QEID	EmpId
		);	

	virtual DWORD	EnumDepts( 
		PS_DeptInfo_	psItem,
		int				i32Level,
		PFN_DeptCb		pfnCb,
		void*			pvUserData
		);
	
	virtual DWORD	EnumEmps( 
		PS_DeptInfo_	psItem,
		int				i32Level,
		PFN_EmpCbEx		pfnCb,
		void*			pvUserData
		);

	virtual	DWORD	GetOnline(
		PS_DeptInfo_	psItem,
		DWORD*			pdwEmpCount,			
		int				i32Level, 
		PFN_EmpCb		pfnEmpCb,
		void*			pvUserData
		);

	virtual DWORD	SearchDepts(
		const TCHAR* pszCond,
		PFN_DeptCb	pfnCb,
		void*		pvUserData
		);

	virtual DWORD	SearchEmps(
		const TCHAR* pszCond,
		PFN_EmpCb	pfnCb,
		void*		pvUserData,
		BYTE		byteFlag=IME_SEARCH_EMP_DEFAULT_FLAG
		);

	virtual const TCHAR* GetDeptName(QDID did);
	virtual const TCHAR* GetDeptName(PS_DeptInfo_ psDeptInfo);
	virtual const TCHAR* GetEmpName(QEID		EmpId);	
	virtual const TCHAR* GetEmpName(PS_EmpInfo_ psEmpInfo);
	virtual int   SetLanguage(int i32Language);
	virtual BOOL  FindEmp(QEID qeid);

	virtual BOOL  SetDeptCallback(PFN_CheckDeptPurviewCb pfnCb, void* pvUserData);
	virtual BOOL  SetEmpCallback(PFN_CheckEmpPurviewCb pfnCb, void* pvUserData);
	virtual DWORD CheckDeptPurview(PS_DeptInfo_ psDeptInfo);
	virtual DWORD CheckEmpPurview(PS_EmpInfo_ psEmpInfo);
	virtual DWORD CheckDeptPurview(QDID did);
	virtual DWORD CheckEmpPurview(QEID eid);
};

