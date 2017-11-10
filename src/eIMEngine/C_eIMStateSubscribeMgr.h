#pragma once

#include "eIMEngine\IeIMStateSubscribeMgr.h"

typedef struct tagSubscribeInfo		// 订阅信息
{
	QEID eid_;						// 用户ID
	int	 createtime;				// 创建时间
	int  updatetime;				// 更新时间
	int  count;						// 引用计数
	BOOL operator < (tagSubscribeInfo const& _A) const
    {
		if(createtime < _A.createtime)
		{
			return TRUE;
		}
		return FALSE;
    }
}SUBSCRIBE_INFO, *PSUBSCRIBE_INFO;

typedef struct tagExpandItemInfo	// 展开节点信息
{
	UINT64	id_;					// 节点ID值
	int type;						// 节点类型	参看ITEMTYPE_*
	int expandtime;					// 展开时间
	int count;						// 引用计数
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

	void SendSubscribeInfoReq(PQEID pallqeid, int inum, int iOperType);						// 发送订阅请求
	void SendSubscribeInfoReq(VectSessionMember& vtallqeid, int iOperType);					// 发送订阅请求
	void SendSubscribeInfoReq(Map_EmpID& mapallqeid, int iOperType);						// 发送订阅请求

	void SendGetStatesInfoReq(PQEID pallqeid, int inum);									// 拉取一次状态

	void SendGetStatesInfoReq(Map_EmpID& mapallqeid);										// 拉取一次状态
	void SendGetDeptStatesInfoReq(Map_DeptID& mapallqdid);									// 拉取一次状态

	void SendGetStatesInfoReq(UINT64 dwItemID, int iItemType);									// 拉取节点的状态

	static int __stdcall EnumSession(PS_SessionInfo_ psInfo, void* pvUserData);
	static int __stdcall EnumSessionEmps(QEID qeid, void* pvUserData);

	void GetItemMember(UINT64 u64ItemID, int iItemType, Map_EmpID& mapAddMember);				// 获取节点成员

	void DoReSendAllStateSubScribe();														// 重发送所有状态订阅的（离线后上线）
public:
	virtual int UpdateEmpSubcribe(QEID qeid, int iOperateType, BOOL bAddTime=FALSE);		// 更新单个人订阅数据
	virtual int UpdateEmpsSubcribe(VT_EmpID& vtEid, int iOperateType);						// 更新多个人的订阅数据
	virtual int UpdateExpandItemData(UINT64 u64ItemID, int iItemType, int iOperateType);			// 更新展开节点数据
	
	virtual void SetUIPage(int iUIPage);													// 设置当前UI界面
	virtual int GetUIPage();																// 获取当前UI界面

	virtual int FirstSubscribeRecentContact();												// 初始订阅最近联系人的用户状态

	virtual void CheckStateSubscribeData(BOOL* pbAbort);									// 订阅状态数据维护
	virtual void CheckStateGetData(BOOL* pbAbort);											// 拉取状态数据维护

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
