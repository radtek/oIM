#pragma once

#include "eIMEngine\IeIMSessionMgr.h"

typedef enum tagSetType
{
	eSET_TYPE_ALL	= 0,	// Set all information
	eSET_TYPE_UTIME,		// Set update time
	eSET_TYPE_OFF_MSG,		// Set offline message count
	eSET_TYPE_NEW_MSG,		// Set unreaded message count
	eSET_TYPE_FLAG,			// Set flag
	eSET_TYPE_TYPE,			// Set type
	eSET_TYPE_TITLE,		// Set title
	eSET_TYPE_REMARK,		// Set remark
	eSET_TYPE_SUTIME,		// Set session update time
	eSET_TYPE_CHECKPOINT,	// Checkpoint
	eSET_TYPE_CLEAR_SESSION_EMP,	// Clear session emps
}E_SetType, *PE_SetType;

class C_eIMSessionMgr: public I_EIMSessionMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

	BOOL _GetRow(I_SQLite3Table* pITable, PS_SessionInfo psSessionInfo);
	BOOL _GetRow1(I_SQLite3Table* pITable, PS_SessionInfo psSessionInfo);
	DWORD SearchMsgCount(PS_MsgEnum psCond);
	BOOL Get1To10T(PQEID paQeid, DWORD dwCount, PS_SessionInfo psSessionInfo);
	BOOL GetSingleTitle(TCHAR szTitle[MAX_SESSION_TITLE], QSID sid);

	static int __stdcall SingleSessionMemberCb(QEID qeid, void* pvUserData);
	static int __stdcall SingleMemberCb(QEID qeid, void* pvUserData);
	static int Compare( const void *pArg1, const void* pArg2 );
	static int __stdcall SessionMemberCb(QEID qeid, void* pvUserData);
	static int __stdcall SessionMemberLstCb(QEID qeid, void* pvUserData);

public:
	DECALRE_PLUGIN_(C_eIMContacts)
	
	C_eIMSessionMgr(void);
	virtual ~C_eIMSessionMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	virtual BOOL Set( 
		PS_SessionInfo_ psInfo,
		QSID qsid=0
		);	// Update or create

	virtual BOOL Set( 
		VIRTUAL_GROUP_INFO_NOTICE* virtualGroupInfo
		);	

	virtual int Set(
		QSID			qsid,
		E_SetType		eSetType,
		DWORD			dwValue,
		const TCHAR*	pszValue = NULL,
		PS_SessionInfo_ psInfo = NULL
		);

	int Set(
		QSID  qsid,
		PQEID paQeid, 
		DWORD dwCount,
		BOOL  bAdd,
		BOOL  bClearFirst
		);

	int Set(
		QSID  qsid,
		regulargroup_member* paMember, 
		DWORD dwCount,
		BOOL  bAdd,
		BOOL  bClearOld
		);

	BOOL UpdateFlag(QSID qsid, DWORD dwFlag, BOOL bSet = TRUE);

	virtual BOOL ReceiptMsgIsRead(QSID qsid, DWORD dwFlag);

	virtual int SendMsgRead(QSID qsid);

	///////////////////////////////////////////////////////////////////////////
	virtual BOOL Get(
		QEID qeidSender,
		QEID qeidRecver,
		PS_SessionInfo psSessionInfo,
		QSID qsidGroupID = 0,
		BOOL bIs1To10T = FALSE
		);

	virtual BOOL Get(
		QSID qsid,
		PS_SessionInfo psSessionInfo
		); 

	virtual BOOL Get(UINT32* update_time);

	virtual BOOL Get(QEID qeidRecver,QSID& VirGroupId);

	virtual BOOL Get(QSID sid_,QSID& VirGroupId);

	virtual BOOL IsVirGroupMainId(QEID qeidReceive,QSID Groupid,int type=0);

	virtual BOOL IsVirGroupSession(QSID qsid) ;

	virtual BOOL GetVirGroupTip(PFN_SessionVirTipCb fpnTip,QEID qeidRecver,void* strVirTip);

	virtual BOOL GetSidByFid(QSID fid_,PFN_FileAnexAckCb pFileAnexAck,void* msgid);

    virtual BOOL GetMsgByFid(QSID fid_,PFN_FileInfo pfnCb,void* msgid);

	virtual BOOL Get(
		PQEID paQeid,
		DWORD dwCount,
		PS_SessionInfo psSessionInfo,
		int sessionType = 0,
		QSID qsid = 0,
		BOOL  bSingle = FALSE,
		QEID  QOrderCreatorID = 0
		);
    
	virtual BOOL SetTitle(
		QSID  qsid,
		const TCHAR* pszValue
		);

	virtual BOOL SetRemark(
		QSID  qsid,
		const TCHAR* pszValue
		);

	virtual BOOL Remove( 
		QSID qsid
		);

	virtual DWORD SearchSession(
		PS_SessionEnum	psCond,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD SearchSession(
		DWORD	timestamp,
		int sessionCount,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD SearchSession(
		PS_SessionEnum	psCond,
		E_SessionType   eSessionType2,
		PFN_NewSessionCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD Enum( 
		E_SEFlag	  eFlag,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD Enum( 
		E_SEFlag	  eFlag,
		E_SEFlag	  eFlag2,
		PFN_SessionCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD EnumMember( 
		QSID 		  qsid,
		PFN_SessionMemberCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD EnumMemberAll( 
		QSID 		  qsid,
		PFN_SessionMemberCb pfnCb,
		void*		  pvUserData
		);

	virtual BOOL GetAutoGroupTitle( 
		PFN_GroupTitleCb pfnCb,
		void*		  pvUserData
		);

	virtual DWORD RgroupMember( 
		QMID 		  qmid,
		PFN_SessionMemberCb pfnCb,
		void*		  pvUserData
		);

	virtual BOOL GetSidByVirGroupid(QSID &qsid,QEID qeid,E_SessionType type);

    virtual BOOL DeleteAllSession();
    virtual BOOL DeleteByTime(DWORD dwDelTime);
    virtual BOOL EnumQSIDbyTime(DWORD dwTime, PFN_NewSessionCb pfnCb, void* pvUserData);
    virtual int Save2File();

	virtual int RepairMsg();
	virtual int DeleteSessionMsg();

	virtual BOOL getQsidByMsgid(QSID msgid, QSID& sid);
	virtual BOOL CheckIncludeLogin(QSID qsid);

	virtual BOOL GetImageMore(const TCHAR* filePath,QSID SessionId ,QMID Msgid,PFN_ImageMore pfnCb,void* pvUserData);

	void GetFilePathByMsg(eIMString& msg,eIMString& filepath);

	BOOL GetEmpInfoByEmp(QEID qeid,PS_EmpInfoByEmp psEmpInfo);

	virtual DWORD TopSession(QSID qsid, E_TopSession eTopOperate);
	virtual DWORD EnumTopSession(PFN_NewSessionCb pfnCb, void* pvUserData);

	virtual BOOL CheckSessionValid(QSID qsid);

	int EnumMemberOffMsg(QSID qsid,VectSessionMember& vectSessionMember);

	int GetSingleSessionMail(QSID sid);
	int GetSessionMsgCount(QSID ssid);
};

