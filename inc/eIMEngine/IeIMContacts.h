// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMContacts is the engine function DLL
// 
// Auth: yfgz
// Date:    2013/11/15 16:14:45 
// 
// History: 
//  2013/12/7 19:31:15 Optimize pinyin store and search
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_CONTACTS_HEADER_2013_11_15_YFGZ__
#define __EIMENGINE_CONTACTS_HEADER_2013_11_15_YFGZ__

#include "Public\Plugin.h"

// Search condition Max size
#define IME_SEARCH_MAX_SIZE			( 128 )	// Search max size
#define IME_MAX_PID					( 6   ) // One employee max in departments
// IMEngine Company information ===============================================
#define IME_CO_CODE_SIZE			( 10 )	// Code size
#define IME_CO_NAME_SIZE			( 50 )	// Name size
#define IME_CO_LOGO_SIZE			( 50 )	// Logo size

// IMEngine department information ============================================
//#define IME_DEPT_NAME_SIZE		( 30 + 1 )	// name size
//#define IME_DEPT_NAME_EN_SIZE		( 250 + 1 )	// English name size
//#define IME_DEPT_PINYIN_ABBR_SIZE	( IME_DEPT_NAME_SIZE )		// Name's abbreviate pinying size
//#define IME_DEPT_PINYIN_SIZE		( IME_DEPT_NAME_SIZE * 5 )	// Name's pinying size
//#define IME_DEPT_TEL_SIZE			( 20 )	// telephone size

// IMEngine employee informaiton ==============================================
//#define IME_EMP_NAME_SIZE			( 32 )	// Name size
//#define IME_EMP_PINYIN_ABBR_SIZE	( IME_EMP_NAME_SIZE )		// Name's abbreviate pinying size
//#define IME_EMP_PINYIN_SIZE		( IME_EMP_NAME_SIZE * 5 )	// Name's pinying size
//#define IME_EMP_CODE_SIZE			( 30 + 1)	// Code size
//#define IME_EMP_PSW_SIZE			( 16 + 1 )	// Password size
//#define IME_EMP_DUTY_SIZE			( 32 )	// Duty size
//#define IME_EMP_TEL_SIZE			( 64 + 1 )	// Telephone size
//#define IME_EMP_PHONE_SIZE		( 11 + 1 )	// Phone size
//#define IME_EMP_EMAIL_SIZE		( 32 )	// EMail size
//#define IME_EMP_LOGO_SIZE			( 6 + 1 )	// Logo size
//#define IME_EMP_SIGN_SIZE			( 46 )  // Sign
//#define IME_EMP_ADDRESS_SIZE		( 170 ) //address
//#define IME_EMP_POSTALCODE_SIZE   ( 10 )   //postalcode
//#define IME_EMP_FAX_SIZE			( 20 )   //fax

// Sex ========================================================================
#define IME_EMP_SEX_WOMAN			( 0 )	// Woman
#define IME_EMP_SEX_MAN				( 1 )	// Man
#define IME_GET_SEX_(SEX)			(SEX == 1 ? IME_EMP_SEX_MAN : IME_EMP_SEX_WOMAN)

// Status =====================================================================
#define IME_EMP_STATUS_MASK			( 3 )	// Mask of status
#define IME_EMP_STATUS_OFFLINE		( 0 )	// Offline
#define IME_EMP_STATUS_ONLINE		( 1 )	// Online
#define IME_EMP_STATUS_LEAVE		( 2 )	// Leave
#define IME_EMP_STATUS_QUIT			( 3 )	// Quit

#define IME_EMP_TERM_MASK			( 0xF )	// Mask of login type
#define IME_EMP_TERM_ANDROID		( 1 )	// Android
#define IME_EMP_TERM_IOS			( 2 )	// IOS
#define IME_EMP_TERM_PC				( 3 )	// PC

// Message synchronization ====================================================
#define IME_EMP_SYNC_MASK			( 3 )	// Mask
#define IME_EMP_SYNC_ONLY_PC		( 0 )	// PC receive only
#define	IME_EMP_SYNC_PC_MOB			( 1 )	// PC & Mobile receive too
#define IME_EMP_SYNC_ONLINE_MOB		( 2 )	// Mobile receive when online, PC receive too

// Employee integrity =========================================================
#define IMP_EMP_INTEGRITY_UGLY		( 0 )	// Ugly, only have EmpId, ParentId,
#define IME_EMP_INTEGRITY_BASIC		( 1 )	// Basic, only have EmpId, Name, UserCode, from department-employee table
#define IME_EMP_INTEGRITY_SIMPLE	( 2 )	// Simple, same as: USERINFO
#define IME_EMP_INTEGRITY_FULL		( 3 )	// Full, same as: EMPLOYEE

// Purview ====================================================================
#define IMP_EMP_PURVIEW_MASK		(0xFF)	// Mask
#define IMP_EMP_PURVIEW_DEFAULT		( 0 )	// Default

//Employee head logo download
#define IME_EMP_LOGO_DOWNLOAD		( 0 )
#define IME_EMP_LOGO_NO_DOWNLOAD	( 1 )

// ID NONE
#define IME_ID_NONE				( 0xFFFFFFFF ) // Mark as invalid Id, or virtual ROOT Node

//SearchWnd Search Emps
#define IME_SEARCH_EMP_DEFAULT_FLAG	  (1)	// default
#define IME_SEARCH_EMP_SEARCHWND_FLAG (2)	// SearchWnd Search Emps by phone、email...
#define IME_SEARCH_EMP_BY_UCODE		  (3)	// By UCODE search, and exact match
#define IME_SEARCH_EMP_BY_PHONE		  (4)	// By phone search, and exact match


#pragma pack( push, 8 )
// Company information
typedef struct tagCompanyInfo
{
	DWORD	dwId;							// ID
	DWORD	dwEstTime;						// Establish time
	BYTE	au8Code[IME_CO_CODE_SIZE];		// Code
	TCHAR	szName[IME_CO_NAME_SIZE];		// Name
	BYTE	au8Logo[IME_CO_LOGO_SIZE];		// Logo
} S_CoInfo, *PS_CoInfo;
typedef const struct tagCompanyInfo* PS_CoInfo_;

// Department information
typedef struct tagDeptInfo
{
	QDID	Id;								// Id
	const TCHAR*	pszName;				// Name
	const TCHAR*	pszNameEn;				// English name
	const TCHAR*	pszTel;					// Telephone
	DWORD			dwUpdateTime;			// Update time
	//	UINT8 wUpdate_type;					// 更新类型, 1: 新增 2: 修改 3: 删除
	//	UINT16 wSort;						// 排序序号
}S_DeptInfo, *PS_DeptInfo;
typedef const struct tagDeptInfo* PS_DeptInfo_;

// Employee information
typedef struct tagEmployeeInfo
{
	DWORD	dwSize:16;						// Size of this struct
	DWORD	bitStatus:2;					// Status, see: IME_EMP_STATUS_*
	DWORD	bitMsgSyncType:2;				// Message synchronization type, see: IME_EMP_SYNC_* 
	DWORD	bitLoginType:4;					// LoginType, see: IME_EMP_TERM_*
	DWORD	bitRef:4;						// Reference in difference department count, 0: Not in any department
	DWORD	bitPurview:8;					// Purview, see: IMP_EMP_PURVIEW_*
	DWORD	bitFixedOrders:16;				// 1- Fixed orders in department, default is 0
	DWORD	bitSex:1;						// Sex, see: IME_EMP_SEX_*
	DWORD	bitIntegrity:3;					// Integrity, see: IME_EMP_INTEGRITY_*
	DWORD	bitConcern:1;					// 1- Is concern 
	DWORD	bitVisible:1;					// 1- Visible
//	DWORD	bitReserved:15;					// Reserved
	QDID	qdidPid[IME_MAX_PID];			// ParentIds
	DWORD	dwCoId;							// Company Id
	QEID	Id;								// Id
	DWORD	dwBirthday;						// Birthday
	DWORD	dwHiredate;						// Hiredate
	DWORD	dwRankId;						// Rank Id
	DWORD	dwProfId;						// Prof Id
	DWORD	dwAeraId;						// Aera Id
	const TCHAR*	pszName;				// en Name
	const TCHAR*	pszEnName;				// cn Name
	const char*		pszNamePinyin;			// Name's pinyin for sort
	const char*		pszNamePinyinS;			// Name's pinyin for search
	const TCHAR*	pszDuty;				// Duty
	const TCHAR*	pszSign;				// Sign
	const TCHAR*	pszCode;;				// Code
//	const TCHAR*	pszPsw;;				// Password
	const TCHAR*	pszTel;;				// Telephone
	const TCHAR*	pszPhone;;				// Phone
	const TCHAR*	pszHomeTel;				// Home telephone
	const TCHAR*	pszEmrgTel;				// Emergency Phone
	const TCHAR*	pszEmail;				// EMail 
//	const TCHAR*	pszLogo;				// Logo 
	const TCHAR*	pszAddress;				// Address
	const TCHAR*	pszPostalCode;			// Zipcode
	const TCHAR*	pszFax;					// Fax
	DWORD			dwUpdateTime;			// Updatetime
	DWORD			dwLogoTime;				//Emp head update see IME_EMP_LOGO_*
	DWORD			dwPurview;				// 权限（在搜索，枚举时，保存通过权限检查回调函数结果）
// 	EMPLOYEE_PURVIEW mPurview[MAX_PURVIEW]; // 该企业有属性的内置权限
}S_EmpInfo, *PS_EmpInfo;
typedef const struct tagEmployeeInfo* PS_EmpInfo_;

// Enumerator callback function, return 0 to break
typedef int  (__stdcall* PFN_DeptCb)(PS_DeptInfo_ psDeptInfo, void* pvUserData);
typedef int  (__stdcall* PFN_EmpCb)(PS_EmpInfo_ psEmpInfo, void* pvUserData, DWORD dwPyOrder);						// dwPyOrder only used bye EnumEmps
typedef int  (__stdcall* PFN_EmpCbEx)(PS_EmpInfo_ psEmpInfo, void* pvUserData, DWORD dwPyOrder, DWORD dwFixOrder);	// dwPyOrder only used bye EnumEmps, dwFixOrder is in dept-emp by server set

// 部门权限检查回调函数指针
#define DEPT_FULL_PURVIEW	(0x00000000)	// 完整权限
#define DEPT_INVISIBLE		(0x00000001)	// 部门不可见
#define DEPT_NO_SEARCH		(0x10000000)	// 禁止搜索
#define DEPT_NO_ENUM		(0x20000000)	// 禁止枚举
#define DEPT_NO_PROMPT		(0x80000000)	// 没有权限时提示用户
typedef DWORD (__stdcall* PFN_CheckDeptPurviewCb)(PS_DeptInfo_ psDeptInfo, void* pvUserData);

// 员工权限检查回调函数指针
#define EMP_FULL_PURVIEW	(0x00000000)	// 完整权限
#define EMP_INVISIBLE		(0x00000001)	// 不可见
#define EMP_HIDE_TEL		(0x00000002)	// 隐藏电话
#define EMP_HIDE_PHONE		(0x00000004)	// 隐藏手机
#define EMP_HIDE_INFO		(0x00000008)	// 隐藏信息（不显示个人信息卡片）
#define EMP_NO_SESSION		(0x00010000)	// 不可（主动）发起会话（无权限）
#define EMP_NO_SEARCH		(0x10000000)	// 禁止搜索
#define EMP_NO_ENUM			(0x20000000)	// 禁止枚举
#define EMP_NO_MENU			(0x40000000)	// 禁止右键菜单
#define EMP_NO_PROMPT		(0x80000000)	// 没有权限时提示用户
typedef DWORD (__stdcall* PFN_CheckEmpPurviewCb)(PS_EmpInfo_ psEmpInfo, void* pvUserData);


#pragma pack( pop )

typedef std::vector< PS_DeptInfo_ >	VectDepts;	// for Enum Department
typedef VectDepts::iterator			VectDeptsIt;

typedef std::vector< PS_EmpInfo_ >	VectEmps;	// for Enum Employee
typedef VectEmps::iterator			VectEmpsIt;

typedef enum tagGetDeptInfoMethod
{
	eGDIM_SELF   = 0,	// Get current QDID item
	eGDIM_PARENT = 1,	// Get Parent item
	eGDIM_FIRST_CHILD,	// Get First child item
	eGDIM_PREV,			// Get Prev item
	eGDIM_NEXT,			// Get Next item
}E_GDIMethod, *PE_GDIMethod;

// ****** If the department structure modified[Add/Del/Move], MUST be reload the Contacts
class __declspec(novtable) I_EIMContacts: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     SetEmpState
	//Description:	set emp state
	//Return:
	//		TRUE success FALSE fail               
	//=============================================================================
	virtual BOOL SetEmpState(QEID qeid, DWORD bitStatus, DWORD bitLoginType) = 0;

	//=============================================================================
	//Function:     FormatError
	//Description:	Format error code as description information
	//
	//Parameter:
	//	dwError     - Error code
	//	pszError    - Return the formated error description information,
	//                Buffer size as ERROR_BUFFER_SIZE
	//
	//Return:
	//		The Error description string size               
	//=============================================================================
	virtual DWORD	FormatError(
		DWORD		dwError, 
		TCHAR*		pszError
		) = 0;

	//=============================================================================
	//Function:     GetCompanyInfo
	//Description:	Get company information
	//
	//
	//Return:
	//		Return the company information              
	//=============================================================================
	virtual PS_CoInfo_	GetCompanyInfo() = 0;

	//=============================================================================
	//Function:     GetDeptItem
	//Description:	Get department item data
	//
	//Parameter:
	//	psItem        -	NULL: Get the First child Item of root(eGDIM_FIRST_CHILD), else is relative it to operate.
	//	eGDIMethod    - See E_GDIMethod
	//
	//Return:
	//		Return the finded department item data if succeeded             
	//		NULL otherwise
	//=============================================================================
	virtual PS_DeptInfo_	GetDeptItem(
		PS_DeptInfo_	psItem,
		E_GDIMethod		eGDIMethod
		) = 0;

	//=============================================================================
	//Function:     GetDeptItem
	//Description:	Get department information by department Id
	//
	//Parameter:
	//	DeptId    - Department Id
	//				  IME_ID_NONE: Get the first root department
	//
	//Return:
	//      Return the Employee information if succeeded 
	//		NULL otherwise
	//=============================================================================
	virtual PS_DeptInfo_	GetDeptItem(
		QDID		DeptId,
		E_GDIMethod eGDIMethod
		) = 0;



	//=============================================================================
	//Function:     GetEmp
	//Description:	Get employee information by employee Id
	//
	//Parameter:
	//	EmpId    - Employee Id
	//
	//Return:
	//      Return the Employee information if succeeded      
	//		NULL otherwise
	//=============================================================================
	virtual PS_EmpInfo_	GetEmp(
		QEID		EmpId
		) = 0;	

	//=============================================================================
	//Function:     GetEmpDept
	//Description:	Get employee's department
	//
	//Parameter:
	//	EmpId    -  Employee Id
	//
	//Return:
	//     Return the department informaiton if succeeded
	//		NULL otherwise
	//=============================================================================
	virtual PS_DeptInfo_	GetEmpDept(
		QEID		EmpId
		) = 0;	

	//=============================================================================
	//Function:     EnumDepts
	//Description:	Enumerate the departments
	//
	//Parameter:
	//	psItem       - NULL: is Enumerate from root department, else is relative it.
	//	i32Level     - 0: is only current level; 
	//				  >0: is include sub-level numbers; 
	//				  -1: all sub-level
	//	pfnCB		 - Callback function to enum department, return FALSE to break
	//	pvUserData	 - UserData, eg.: an container to save the result of enumerate, like VectDepts
	//
	//Return:
	//		Return the enumerated count             
	//=============================================================================
	virtual DWORD	EnumDepts( 
		PS_DeptInfo_	psItem,
		int				i32Level,
		PFN_DeptCb		pfnCb,
		void*			pvUserData
		) = 0;

	//=============================================================================
	//Function:     EnumEmps
	//Description:	Enumerate the employees
	//
	//Parameter:
	//	psItem       - NULL: is Enumerate from root department, else is relative it.
	//	i32Level       0: is only current department's member; 
	//				  >0: is include sub-department's level; 
	//				  -1: all sub-level
	//	pfnCB		 - Callback function to enum employee, return FALSE to break
	//	pvUserData	 - UserData, eg.: an container to save the result of enumerate, like VectEmps
	//
	//Return:
	//		Return the enumerated count             
	//=============================================================================
	virtual DWORD	EnumEmps( 
		PS_DeptInfo_	psItem,
		int				i32Level,
		PFN_EmpCbEx		pfnCb,
		void*			pvUserData
		) = 0;

	//=============================================================================
	//Function:     GetOnline
	//Description:	Get online employee count and employee count
	//
	//Parameter:
	//	psItem        - NULL: is Enumerate from root department(for Get entire company online status ), else is relative it.
	//	pdwEmpCount   - Return the employee count
	//				   Caller zero it at first
	//	i32Level       0: is only current department; 
	//				  >0: is include sub-department level ; 
	//				  -1: all sub-level
	//	pfnCB		 - Optional, Callback function to enum online employee, return FALSE to break
	//	pvUserData	 - Optional, UserData, eg.: an container to save the result of enumerate, like VectEmps
	//
	//Return:
	//       Return the online employee count       
	//=============================================================================
	virtual	DWORD	GetOnline(
		PS_DeptInfo_	psItem,
		DWORD*			pdwEmpCount,			
		int				i32Level,
		PFN_EmpCb		pfnEmpCb,
		void*			pvUserData
		) = 0;

	//=============================================================================
	//Function:     SearchDepts
	//Description:	Search departments
	//
	//Parameter:
	//	pszCond		 - Search condition
	//	pfnCB		 - Callback function to search department, return FALSE to break
	//	pvUserData	 - UserData, eg.: an container to save the result of search, like VectDepts
	//
	//Return:
	//      Return the count of searched result        
	//=============================================================================
	virtual DWORD	SearchDepts(
		const TCHAR* pszCond,
		PFN_DeptCb  pfnCb,
		void*		pvUserData
		) = 0;

	//=============================================================================
	//Function:     SearchEmps
	//Description:	Search employees
	//
	//Parameter:
	//	pszCond     - Search condition
	//	pfnCB		 - Callback function to search employee, return FALSE to break
	//	pvUserData	 - UserData, eg.: an container to save the result of search, like VectEmps
	//
	//Return:
	//      Return the count of searched result        
	//=============================================================================
	virtual DWORD	SearchEmps(
		const TCHAR* pszCond,
		PFN_EmpCb	pfnCb,
		void*		pvUserData,
		BYTE		byteFlag=IME_SEARCH_EMP_DEFAULT_FLAG
		) = 0;

	//=============================================================================
	//Function:     GetDeptName
	//Description:	Get the department name by current language
	//
	//Parameter:
	//	PS_DeptInfo_   - Department information
	//
	//Return:
	//      Return the department name bu current language  
	//=============================================================================
	virtual const TCHAR*  GetDeptName(PS_DeptInfo_ psDeptInfo) = 0;
	virtual const TCHAR*  GetDeptName(QDID did) = 0;

	//=============================================================================
	//Function:     GetEmpName
	//Description:	Get the employee name by current language
	//
	//Parameter:
	//	PS_DeptInfo_   - Department information
	//
	//Return:
	//      Return the employee name bu current language  
	//=============================================================================
	virtual const TCHAR*  GetEmpName(PS_EmpInfo_ psEmpInfo) = 0;

	//=============================================================================
	//Function:     GetEmpName
	//Description:	Get employee name for display
	//
	//Parameter:
	//	EmpId    - Employee Id
	//
	//Return:
	//      Return the Employee Name(or Code if name is NULL) if succeeded      
	//		NULL otherwise
	//=============================================================================
	virtual const TCHAR* GetEmpName(QEID eid) = 0;	

	//=============================================================================
	//Function:     SetLanguage
	//Description:	Set language 
	//
	//Parameter:
	//	i32Language   - Language id
	//
	//Return:
	//      Return prev-language id  
	//=============================================================================
	virtual int SetLanguage(int i32Language) = 0;

	//=============================================================================
	//Function:     FindEmp
	//Description:	Check employee whether exist
	//
	//Parameter:
	//	qeid	- Employee ID
	//
	//Return:
	//  TRUE	- Exist
	//  FALSE	- Not exist
	//=============================================================================
	virtual BOOL FindEmp(QEID qeid) = 0; 

	// 权限检查（可见性）回调函数，由公司化模块（如webapp）进行设置
	virtual BOOL  SetDeptCallback(PFN_CheckDeptPurviewCb pfnCb, void* pvUserData) = 0;
	virtual BOOL  SetEmpCallback(PFN_CheckEmpPurviewCb pfnCb, void* pvUserData) = 0;
	virtual DWORD CheckDeptPurview(PS_DeptInfo_ psDeptInfo) = 0;
	virtual DWORD CheckDeptPurview(QDID did) = 0;
	virtual DWORD CheckEmpPurview(PS_EmpInfo_ psEmpInfo) = 0;
	virtual DWORD CheckEmpPurview(QEID eid) = 0;

};

#endif // __EIMENGINE_CONTACTS_HEADER_2013_11_15_YFGZ__