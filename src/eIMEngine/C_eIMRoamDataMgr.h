#pragma once

#include "eIMEngine\IeIMRoamDataMgr.h"

class C_eIMRoamDataMgr: public I_EIMRoamDataMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMRoamDataMgr)
	
	C_eIMRoamDataMgr(void);
	virtual ~C_eIMRoamDataMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	virtual void RoamDataSyncReq();
	virtual DWORD Enum(int iType,PFN_RoamDataCB pfnCb,void* pvUserData);
	virtual DWORD EnumDefineGroupMember(QGID qgid,PFN_DefineGroupMemberCb pfnCb,void* pvUserData);

	int EnumDefineGroupMember(QGID qgid,VectEmpId& vtEmpID);
	
	int Set(const ROAMDATASYNCACK* psAck);
	int Set(const ROAMDATAMODIACK* psAck);
	int Set (S_CustomizeInfo* psModify);
	int Set(const ROAMDATAMODINOTICE* psAck);
	virtual int AddTopContact(QEID eid, int is_default, int iFlag);
	inline int DelTopContact(QEID eid);
	inline int DelAllNotDefaultTopContact();
	inline int DelDefaultTopContact();

	virtual int AddTopDept(QDID did, int iFlag);
	inline int DelTopDept(QDID did);
	inline int DelAllTopDept();

	virtual int AddDefineGroup(int id, const char* pszAlias, int iFlag);
	inline int GetInsertDefineGroupID();
	inline int DelDefineGroup(int id);

	virtual int  AddDefineCommonGroup(QSID id, const TCHAR* pszAlias=NULL, int iFlag=0);
	virtual BOOL DelDefineCommonGroup(QSID id, BOOL bNoSelfs=FALSE);


	virtual int AddGroupMember(int iGroupID, QEID qeid, int iFlag);
	inline int DelGroupMember(int iGroupID, QEID qeid);

	inline int SetEmpConcernFlag(QEID eid, BOOL bConcern);
	inline int ClearAllEmpConcernFlag();

	int AddRobotInfo(Robot_User_Info& rInfo);
	BOOL ClearRobotInfo();	
	virtual BOOL GetRobotInfo(QEID userid, Robot_User_Info& rInfo);

	virtual int AddGroupMember(void* pvData);
	virtual BOOL IsTypeof(QSID sid, int i32Type); 


};

