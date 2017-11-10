#pragma once

#include "eIMEngine\IeIMStateSubscribeMgr.h"

typedef struct tagSubscribeInfo		// ������Ϣ
{
	QEID eid_;						// �û�ID
	int	 createtime;				// ����ʱ��
	int  updatetime;				// ����ʱ��
	int  count;						// ���ü���
	BOOL operator < (tagSubscribeInfo const& _A) const
    {
		if(createtime < _A.createtime)
		{
			return TRUE;
		}
		return FALSE;
    }
}SUBSCRIBE_INFO, *PSUBSCRIBE_INFO;

typedef struct tagExpandItemInfo	// չ���ڵ���Ϣ
{
	UINT64	id_;					// �ڵ�IDֵ
	int type;						// �ڵ�����	�ο�ITEMTYPE_*
	int expandtime;					// չ��ʱ��
	int count;						// ���ü���
}EXPANDITEM_INFO, *PEXPANDITEM_INFO;

typedef std::vector<SUBSCRIBE_INFO> VT_SubscribeInfo;
typedef std::vector<EXPANDITEM_INFO> VT_ExpandItemInfo;

typedef std::vector<QSID> vectSessionIDs;
typedef std::vector<QSID>::iterator itSessionsIDs;

typedef std::map<QEID, SUBSCRIBE_INFO>					Map_SubscribeInfo;
typedef std::map<QEID, SUBSCRIBE_INFO>::iterator		Map_SubscribeInfoIter;

typedef std::map<UINT64, EXPANDITEM_INFO>				Map_ExpandItemInfo;
typedef std::map<UINT64, EXPANDITEM_INFO>::iterator		Map_ExpandItemInfoIter;

class C_eIMStateSubscribeMgr: public I_EIMSubScribeMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;
	I_EIMEngine*		m_pIMEngine;
	I_EIMSessionMgr*	m_pIMSessionMgr;

	DWORD		m_dwLastCheckTime;
	int			m_iCurUIPage;
	std::map<QEID,QEID> m_mapRobot;
	std::map<QEID,QEID> m_mapVirGroupMem;

	Map_SubscribeInfo m_mapAllSubscribeInfo;
	Map_ExpandItemInfo m_zMapAllExpandItemInfo[ITEMTYPE_COUNT];
protected:
	C_eIMStateSubscribeMgr(void);

	void SendSubscribeInfoReq(PQEID pallqeid, int inum, int iOperType);						// ���Ͷ�������
	void SendSubscribeInfoReq(VectSessionMember& vtallqeid, int iOperType);					// ���Ͷ�������
	void SendSubscribeInfoReq(Map_EmpID& mapallqeid, int iOperType);						// ���Ͷ�������

	void SendGetStatesInfoReq(PQEID pallqeid, int inum);									// ��ȡһ��״̬

	void SendGetStatesInfoReq(Map_EmpID& mapallqeid);										// ��ȡһ��״̬
	void SendGetDeptStatesInfoReq(Map_DeptID& mapallqdid);									// ��ȡһ��״̬

	void SendGetStatesInfoReq(UINT64 dwItemID, int iItemType);									// ��ȡ�ڵ��״̬

	static int __stdcall EnumSession(PS_SessionInfo_ psInfo, void* pvUserData);
	static int __stdcall EnumSessionEmps(QEID qeid, void* pvUserData);

	void GetItemMember(UINT64 u64ItemID, int iItemType, Map_EmpID& mapAddMember);				// ��ȡ�ڵ��Ա

	void DoReSendAllStateSubScribe();														// �ط�������״̬���ĵģ����ߺ����ߣ�
public:
	virtual int UpdateEmpSubcribe(QEID qeid, int iOperateType, BOOL bAddTime=FALSE);		// ���µ����˶�������
	virtual int UpdateEmpsSubcribe(VT_EmpID& vtEid, int iOperateType);						// ���¶���˵Ķ�������
	virtual int UpdateExpandItemData(UINT64 u64ItemID, int iItemType, int iOperateType);			// ����չ���ڵ�����
	
	virtual void SetUIPage(int iUIPage);													// ���õ�ǰUI����
	virtual int GetUIPage();																// ��ȡ��ǰUI����

	virtual int FirstSubscribeRecentContact();												// ��ʼ���������ϵ�˵��û�״̬

	virtual void CheckStateSubscribeData(BOOL* pbAbort);									// ����״̬����ά��
	virtual void CheckStateGetData(BOOL* pbAbort);											// ��ȡ״̬����ά��

	static int __stdcall EnumRobotMember(PROAM_DATA_INFO psInfo, void* pvUserData);

	void UpdateRobotStateOnline();

	virtual BOOL IsRobot(QEID eid);

	virtual BOOL IsVirGroupMain(QEID eid);
public:
	DECALRE_PLUGIN_(C_eIMStateSubscribeMgr)

	~C_eIMStateSubscribeMgr(void);
	static C_eIMStateSubscribeMgr& GetObject();

	virtual BOOL Init();
	virtual BOOL Uninit();
};
