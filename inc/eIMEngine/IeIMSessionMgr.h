// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMSessionMgr is the engine function to 
//	 operate the sessions
// 
// Auth: yfgz
// Date:     2014/7/2 13:53:05 
// 
// History: 
//    
// Example:
//int __stdcall SessionCb)(PS_SessionInfo_ psInfo, void* pvUserData);

/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_SESSION_MANAGER_HEADER_2014_07_02_YFGZ__
#define __EIMENGINE_SESSION_MANAGER_HEADER_2014_07_02_YFGZ__
#include "Public\Plugin.h"

#define MAX_SESSION_TITLE				( 50 + 1 )
#define MAX_SESSION_REMARK				( 50 + 1 )
#define MAX_FILENAME                    ( 100 + 1 )
#define MAX_NAME                        ( 50 )

typedef enum tagTopSession
{
	eTOP_SESSION_ADD = 0,			// Add or update to top session
	eTOP_SESSION_DEL,				// Delete a top session
	eTOP_SESSION_GET,				// Get a session from top session
	eTOP_SESSION_COUNT,				// Get the count of top session, qsid is 0
}E_TopSession, *PE_TopSession;

typedef enum tagSessionType
{
	eSESSION_TYPE_UNKNOWN= -3,		// Unknown
	eSESSION_TYPE_MSGAUTO= -2,		// Auto Response message
	eSESSION_TYPE_ERROR  = -1,		// Show Error information of send
	eSESSION_TYPE_SINGLE = 0,		// Single session
	eSESSION_TYPE_1TO_HUNDRED,		// 1 to hundred session
	eSESSION_TYPE_1TO_TEN_THOUSAND,	// 1 to ten thousand session
	eSESSION_TYPE_MULTI,			// Multi-session
	eSESSION_TYPE_RGROUP,			// Regular group
	eSESSION_TYPE_SMS,				// SMS
	eSESSION_TYPE_VIRGROUP,         // Virtual group
	eSESSION_TYPE_ALL,              // ALL type session
}E_SessionType, *PE_SessionType;

// Bit flag for enum session[can OR any the flags]
typedef enum tagSEnumFlag
{	// LOWORD for session type
	eSEFLAG_SINGLE		= (1 << eSESSION_TYPE_SINGLE),			// Single session	
	eSEFLAG_MULTI		= (1 << eSESSION_TYPE_MULTI),			// Multi-session
	eSEFLAG_1TO1H		= (1 << eSESSION_TYPE_1TO_HUNDRED),		// 1 to hundred
	eSEFLAG_1TOTH		= (1 << eSESSION_TYPE_1TO_TEN_THOUSAND),// 1 to ten thousand
	eSEFLAG_RGROUP		= (1 << eSESSION_TYPE_RGROUP),			// virtual group
	eSEFLAG_VGROUP		= (1 << eSESSION_TYPE_VIRGROUP),		// virtual group
	eSEFLAG_TYPE_MASK	= 0x0000FFFF,

	// HIWORD for others flag, eSEFLAG_ASC_UTIME and eSEFLAG_ASC_CTIME is exclusive, ONLY can use one of it.
	// Default is DESC by update time(!eSEFLAG_ASC_UTIME)
	eSEFLAG_DESC_UTIME	= 0x00000000,	// DESC order by utime, default
	eSEFLAG_ASC_UTIME	= 0x00010000,	// ASC order by update time
	eSEFLAG_DESC_CTIME	= 0x00020000,	// DESC order by create time
	eSEFLAG_ASC_CTIME	= 0x00030000,	// ASC order by create time
	eSEFLAG_ORDER_MASK	= 0x00030000,	// Order flag mask

	eSEFLAG_SHIELD		= 0x00040000,	// Shielding session
	eSEFLAG_DELETE		= 0x00080000,	// Deleted session, default DONT
	eSEFLAG_UPDATE		= 0x00100000,	// Updated flag
	eSEFLAG_NEW			= 0x00200000,	// Exist new msg
	eSEFLAG_OFFLINE		= 0x00400000,	// Exist offline msg
	eSEFLAG_MARK_STAR	= 0x00800000,	// Mark star
	eSEFLAG_REMOVE		= 0x01000000,	// Remove session by local

	eSEFLAG_USER		= 0x80000000,	// User define group

	eSEFLAG_CUSTOM_MASK			= 0x8000FFFF,	//Custom mask
	eSEFLAG_CUSTOM_TOP_CONTACTS = 0x80000001,	//Top contacts
	eSEFLAG_CUSTOM_USER_GROUP	= 0x80000002,	//User group
	eSEFLAG_CUSTOM_TOP_GROUP	= 0x80000003,	//Top group

	eSEFLAG_ADMINE = 0x00000001,      //Rgroup Admines

	eSEFLAG_RECEIPT_READ= 0x00008000, //receipt model is 0 ,msg was clicked to readed;
	eSEFLAG_AT			= 0x00004000, // 标识已经点了@消息（@工具栏）
}E_SEFlag, *PE_SEFlag;

#define SESSION_MSG_CONTENT_MAX		( 128 )		// Max size to search content
#define SESSION_MSG_CONTACTS_MAX	( 32  )		// Max size to search contacts

typedef enum tagSessionOperate
{	// A [Date flag] can OR a [Contents flag], if Error combination, use the first appear flag by bit order
	eSession_OP_DATE		= 0x00000001,	// Equal the timestamp[ts == dwStart]
	eSession_OP_GE_DATE		= 0x00000002,	// Be equal or greater than timestamp[ts >= dwStart]
	eSession_OP_BETWEEN_DATE= 0x00000004,	// Between the timestamp[ts >= dwStart && ts <= dwEnd]

	// Contents flag[second byte]
	eSession_OP_TEXT		= 0x00000400,	// By content[szContent]
	eSession_OP_CONTS		= 0x00000800,	// By contacts[szContacts]
	eSession_OP_TEXT_CONTS	= (eSession_OP_TEXT | eSession_OP_CONTS),	// By content and contacts[szContent, szContacts]
}E_SessionOperate, *PE_SessionOperate;

typedef struct tagSessionEnum
{
	E_SessionOperate	eOperate;					// Search operate
	E_SessionType	eType;							// Search session type(Reserved now)
	DWORD	dwOffset;								// Offset
	DWORD	dwLimit;								// Limit, 0 to ignore it
	DWORD	dwStart;								// Start date
	DWORD	dwEnd;									// End date
	TCHAR	szContent[SESSION_MSG_CONTENT_MAX];		// Search content
	TCHAR	szContacts[SESSION_MSG_CONTACTS_MAX];	// Search contacts
}S_SessionEnum, *PS_SessionEnum;

typedef struct tagSessionInfo		
{
	QSID 			qsid;							// The Session ID
	QEID			CreaterId;						// Creater ID
	DWORD			dwCreateTime;					// Create time
	DWORD			dwUpdateTime;					// Last msg update time
	DWORD			dwSUpdateTime;					// Session updae time
	DWORD			dwOfflineMsg;					// Offline message count
	DWORD			dwNewMsg;						// New(unreaded) message count
	DWORD			dwTopTime;						// Top timestamp
	E_SessionType	eType;							// Session Type
	E_SEFlag		eFlag;							// Flag
	TCHAR			szTitle[MAX_SESSION_TITLE];		// Session Title
	TCHAR			szRemark[MAX_SESSION_REMARK];	// Session remark
	DWORD           dwSingleSessionSex ;            // single session sex
	DWORD           dwAllMsg;                       // all msg count
}S_SessionInfo, *PS_SessionInfo;
typedef const struct tagSessionInfo* PPS_SessionInfo_;

typedef std::vector<S_SessionInfo>		VectSessionInfo;	// 会话信息
typedef VectSessionInfo::iterator		VectSessionInfoIt;	
typedef VectSessionInfo::const_iterator	VectSessionInfoIt_;

typedef struct tagEmpInfoByEmp
{
	TCHAR	pszName[MAX_NAME];						// Name
	TCHAR	pszCode[MAX_NAME];						// Code				
}S_EmpInfoByEmp,*PS_EmpInfoByEmp;

typedef const struct tagSessionInfo* PS_SessionInfo_;

// Enumerator callback, retorn 0 to break 
typedef int (__stdcall* PFN_SessionCb)(PS_SessionInfo_ psInfo, void* pvUserData);
typedef int (__stdcall* PFN_NewSessionCb)(QSID qsid, void* pvUserData);
typedef int (__stdcall* PFN_SessionMemberCb)(QEID qeid, void* pvUserData);
typedef int (__stdcall* PFN_SessionVirTipCb)(eIMString& virTip,void* pvUserData);
typedef int (__stdcall* PFN_GroupTitleCb)(int Num,void* pvUserData);
typedef int (__stdcall* PFN_VirGroupCb)(BOOL isGroupMem,void* pvUserData);
typedef int (__stdcall* PFN_FileAnexAckCb)(QSID qsid, void* pvUserData);
typedef int (__stdcall* PFN_FileInfo)(eIMString& psFileInfo,void* pvUserData );
typedef int (__stdcall* PFN_ImageMore)(eIMString& psFileInfo,int insertType,void* pvUserData);

class __declspec(novtable) I_EIMSessionMgr: public I_EIMUnknown
{
public:
	virtual int SendMsgRead(QSID qsid) = 0;
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
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(
		QSID qsid,
		PS_SessionInfo psSessionInfo
		) = 0;

	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     Get 
	//Description:  Get a single session id, it will create it when not exist 
	//
	//Parameter:
	//	qeidSender      - Sender id, if is 0, will be get the loginId as sender id
	//	qeidRecver      - Receiver id
	//	psSessionInfo   - Return the result
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(
		QEID qeidSender,
		QEID qeidRecver,
		PS_SessionInfo psSessionInfo,
		QSID qsidGroupID = 0,
		BOOL bIs1To10T = FALSE
		) = 0;

	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     Get 
	//Description:  Get a virtual group update time 
	//
	//Parameter:
	//	update_time      - Return the update time
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(UINT32* update_time) = 0;

	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     Get 
	//Description:  Get a virtual group session id by receiver 
	//
	//Parameter:
	//	qeidRecver      - receiver id
	//  VirGroupId		- Return virtual group id
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(QEID qeidRecver,QSID& VirGroupId ) = 0;

	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     Get 
	//Description:  Get a virtual group session id by sid 
	//
	//Parameter:
	//	sid_			- session id
	//  VirGroupId		- Return virtual group id
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(QSID sid_,QSID& VirGroupId) = 0;

	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     IsVirGroupMainId 
	//Description:  Check a session whether is virtual group
	//
	//Parameter:
	//	qeidReceive		- receiver id
	//  groupid			- session id
	//  type			- the type
	//
	//Return:
	//		TRUE	- Is virtual group main id
	//		FALSE	- Not a virtual group main id
	//=============================================================================
	virtual BOOL IsVirGroupMainId(QEID qeidReceive ,QSID groupid ,int type)=0;

	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     GetVirGroupTip 
	//Description:  Get virtual group tips
	//
	//Parameter:
	//	fpnTip		- Callback
	//  qeidRecver	- Receiver
	//  strVirTip	- User data
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL GetVirGroupTip(PFN_SessionVirTipCb fpnTip,QEID qeidRecver,void* strVirTip)=0;
	
	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     GetSidByVirGroupid 
	//Description:  Get virtual group tips
	//
	//Parameter:
	//	qsid		- Return the session id
	//  qeid		- empolyee id
	//  type		- Session type
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL GetSidByVirGroupid(QSID &qsid,QEID qeid,E_SessionType type) = 0;

	//=============================================================================
	//Function:     Get 
	//Description:  Get a session information by members
	//
	//Parameter:
	//	paQEID          - Session's members to find the QEID
	//  dwCount			- Members count
	//	psSessionInfo   - Return the result
	//  QOrderCreatorID - Creator ID for Order higner priority
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(
		PQEID paQeid,
		DWORD dwCount,
		PS_SessionInfo psSessionInfo,
		int sessionType = 0,
		QSID qsid=0,
		BOOL  bSingle = FALSE,
		QEID  QOrderCreatorID = 0
		) = 0;

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
	virtual BOOL SetTitle(
		QSID  qsid,
		const TCHAR* pszValue
		) = 0;

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
	virtual BOOL SetRemark(
		QSID  qsid,
		const TCHAR* pszValue
		) = 0;

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
	virtual BOOL Remove( 
		QSID qsid
		) = 0;

	//=============================================================================
	//Function:     UpdateFlag
	//Description:	Update flag
	//
	//Parameter:
	//	qsid		- Update the flag of session by QSID
	//	bSet		- TRUE :  update flag. FALSE: delete flag.
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//		
	//=============================================================================
	virtual BOOL UpdateFlag(QSID qsid, DWORD dwFlag, BOOL bSet = TRUE);

	//=============================================================================
	//Function:     ReceiptMsgIsRead
	//Description:	Check the receipt message whether is readed?
	//
	//Parameter:
	//	psCond		- Condition
	//	pfnCb		- Callback
	//  pvUserData	- User data
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed
	//=============================================================================
	virtual BOOL ReceiptMsgIsRead(QSID qsid, DWORD dwFlag) = 0;

	//=============================================================================
	//Function:     SearchSession
	//Description:	Search session
	//
	//Parameter:
	//	psCond		- Condition
	//	pfnCb		- Callback
	//  pvUserData	- User data
	//Return:
	//		Session count
	//=============================================================================
	virtual DWORD SearchSession(
		PS_SessionEnum	psCond,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		) = 0;

	//=============================================================================
	//Function:     SearchSession
	//Description:	Search session
	//
	//Parameter:
	//	timestamp		- timestamp
	//  sessionCount   - count
	//	pfnCb		- Callback
	//  pvUserData	- User data
	//Return:
	//		Session count
	//=============================================================================
	virtual DWORD SearchSession(
		DWORD	timestamp,
		int sessionCount,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		) = 0;

	//=============================================================================
	//Function:     SearchSession
	//Description:	Search session
	//
	//Parameter:
	//	psCond			- Condition
	//  eSessionType2	- Session type
	//	pfnCb			- Callback
	//  pvUserData		- User data
	//Return:
	//		Session count
	//=============================================================================
	virtual DWORD SearchSession(
		PS_SessionEnum	psCond,
		E_SessionType   eSessionType2,
		PFN_NewSessionCb pfnCb,
		void*		  pvUserData
		) = 0;

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
	virtual DWORD Enum( 
		E_SEFlag	  eFlag,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		) = 0;

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
	virtual DWORD Enum( 
		E_SEFlag	  eFlag,
		E_SEFlag	  eFlag2,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		) = 0;

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
	virtual DWORD EnumMember( 
		QSID 		  qsid,
		PFN_SessionMemberCb pfnCb,
		void*		  pvUserData
		) = 0;

	//=============================================================================
	//Function:     EnumMemberAll
	//Description:	Enumerate all session's members
	//
	//Parameter:
	//	qsid          - Session ID
	//	pfnCb         - Callback function to receive the result, return 0 to break;
	//	pvUserData    - User defined data, pass to pfnCb
	//
	//Return:
	//		The count of enumerated members 
	//=============================================================================
	virtual DWORD EnumMemberAll( 
		QSID 		  qsid,
		PFN_SessionMemberCb pfnCb,
		void*		  pvUserData
		)=0;

	//=============================================================================
	//Function:     GetAutoGroupTitle
	//Description:	Get default title
	//
	//Parameter:
	//	pfnCb          - CAllback 
	//	pvUserData    - User data
	//
	//Return:
	//	None
	//=============================================================================
	virtual BOOL GetAutoGroupTitle( 
		PFN_GroupTitleCb pfnCb,
		void*		  pvUserData
		)=0;

	//=============================================================================
	//Function:     Set
	//Description:	Set session information
	//
	//Parameter:
	//	qsid		- Session Id 
	//	paQeid		- Session member's id
	//  dwCount		- Member count
	//	bAdd		- Is Add
	//  bClearFirst	- Clear at first
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int Set(
		QSID  qsid,
		PQEID paQeid, 
		DWORD dwCount,
		BOOL  bAdd,
		BOOL  bClearFirst
		) = 0;

	//=============================================================================
	//Function:     GetSidByFid
	//Description:	Get session id by file id
	//
	//Parameter:
	//	fid_			- File id
	//	pFileAnexAck	- Callback 
	//	qsid			- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- FAILED
	//=============================================================================
	virtual BOOL GetSidByFid(
		QSID fid_,
		PFN_FileAnexAckCb pFileAnexAck,
		void* qsid
		) = 0;

	//=============================================================================
	//Function:     GetMsgByFid
	//Description:	Get message id by file id
	//
	//Parameter:
	//	fid_			- File id
	//	pFileMsg		- Callback 
	//	msgid			- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL GetMsgByFid(
		QSID fid_,
		PFN_FileInfo pFileMsg,
		void* msgid
		) = 0;

	//=============================================================================
	//Function:     RgroupMember
	//Description:	Get fix group member id by msgid
	//
	//Parameter:
	//	qmid		- Msgid
	//	pfnCb		- Callback 
	//	pvUserData	- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual DWORD RgroupMember( 
		QMID 		  qmid,
		PFN_SessionMemberCb pfnCb,
		void*		  pvUserData
		) = 0;

	//=============================================================================
	//Function:     IsVirGroupSession
	//Description:	Check session whether is a virtual group
	//
	//Parameter:
	//	qsid		- Session Id
	//
	//Return:
	//	TRUE	- Is virtual group
	//	FALSE	- Not a virtual group
	//=============================================================================
	virtual BOOL IsVirGroupSession(QSID qsid) = 0;

	//=============================================================================
	//Function:     Set
	//Description:	Set session information
	//
	//Parameter:
	//  psInfo		- Session information
	//	qsid		- Virtual session Id
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL Set(PS_SessionInfo_ psInfo,QSID qsid=0) = 0;

	//=============================================================================
	//Function:     DeleteAllSession
	//Description:	Delete all session
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
    virtual BOOL DeleteAllSession() = 0;

	//=============================================================================
	//Function:     DeleteByTime
	//Description:	Delete old session by time 
	//
	//Parameter:
	//  dwDelTime	- time
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
    virtual BOOL DeleteByTime(DWORD dwDelTime) = 0;

	//=============================================================================
	//Function:     EnumQSIDbyTime
	//Description:	Enum session id by timestamp
	//
	//Parameter:
	//  dwTime		- time
	//	pfnCb		- Callback
	//	pvUserData	- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
    virtual BOOL EnumQSIDbyTime(DWORD dwTime, PFN_NewSessionCb pfnCb, void* pvUserData) = 0;

	//=============================================================================
	//Function:     Save2File
	//Description:	Sync data to database file
	//
	//Parameter:
	//  dwTime		- time
	//	pfnCb		- Callback
	//	pvUserData	- User data
	//
	//Return:
	//	1		- Succeeded
	//	else failed
	//=============================================================================
    virtual int Save2File() = 0;

	//=============================================================================
	//Function:     RepairMsg
	//Description:	Repair message(bug fix of temp use)
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int RepairMsg() = 0;

	//=============================================================================
	//Function:     DeleteSessionMsg
	//Description:	Repair message(bug fix of temp use)
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int DeleteSessionMsg() = 0;

	//=============================================================================
	//Function:     getQsidByMsgid
	//Description:	Get session id by MsgId
	//
	//Parameter:
	//  msgid		- MsgId
	//  sid			- Return Session id
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL getQsidByMsgid(QSID msgid, QSID& sid) = 0;

	//=============================================================================
	//Function:     CheckIncludeLogin
	//Description:	Check whether include me of session member
	//
	//Parameter:
	//  qsid		- Session id
	//
	//Return:
	//	TRUE	- Include me
	//	FALSE	- Not include
	//=============================================================================
	virtual BOOL CheckIncludeLogin(QSID qsid) = 0;

	//=============================================================================
	//Function:     GetImageMore
	//Description:	Get images of session
	//
	//Parameter:
	//  filePath	- image file path
	//  SessionId	- Session id
	//	pfnCb		- Callback
	//  pvUserData	- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL GetImageMore(const TCHAR* filePath,QSID SessionId,QMID Msgid,PFN_ImageMore pfnCb,void* pvUserData) = 0;

	//=============================================================================
	//Function:     GetEmpInfoByEmp
	//Description:	Get employee information by qeid
	//
	//Parameter:
	//  qeid		- Employee id
	//  psEmpInfo	- Employee information
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL GetEmpInfoByEmp(QEID qeid,PS_EmpInfoByEmp psEmpInfo) = 0;

	//=============================================================================
	//Function:     TopSession
	//Description:	Topmost session operate
	//
	//Parameter:
	//  qsid		- Session id
	//  eTopOperate	- Operate
	//
	//Return:
	//	Reference the operate
	//=============================================================================
	virtual DWORD TopSession(QSID qsid, E_TopSession eTopOperate)=0;

	//=============================================================================
	//Function:     EnumTopSession
	//Description:	Enum Topmost session
	//
	//Parameter:
	//  pfnCb		- Callback
	//  pvUserData	- User data
	//
	//Return:
	//	Topmost session count
	//=============================================================================
	virtual DWORD EnumTopSession(PFN_NewSessionCb pfnCb, void* pvUserData) = 0;

	//=============================================================================
	//Function:     CheckSessionValid
	//Description:	Check the session is valid
	//Return:
	//is valid
	//=============================================================================
	virtual BOOL CheckSessionValid(QSID qsid) = 0;
};

#endif // __EIMENGINE_SESSION_MANAGER_HEADER_2014_07_02_YFGZ__