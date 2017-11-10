#include "stdafx.h"
#include "C_eIMStateSubscribeMgr.h"
//#include "eIMEngine\C_CmdReflection.h"

#define  RECENT_SUBSCRIBE_COUNT_LIMIT  30

C_eIMStateSubscribeMgr::C_eIMStateSubscribeMgr(void)
	: m_dwRef( 1 )
	, m_pIDb ( NULL )
	, m_iCurUIPage(UIPAGE_CONTACT)
	, m_pIMEngine(NULL)
	, m_pIMSessionMgr(NULL)
{
}

// 发送订阅请求
void C_eIMStateSubscribeMgr::SendSubscribeInfoReq(PQEID pallqeid, int inum, int iOperType)				
{
	if(inum <= 0)
	{
		return;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("count=%d"), inum); 
	TCHAR  szValue[XML_VALUE_SIZE_MAX] = { 0 };

	eIMString strCmd;
	swprintf(szValue, kCmdSubscribeAction, iOperType);
	strCmd += szValue;

	std::map<QEID, QEID> mapRobot;
	C_eIMRoamDataMgr rmgr;
	rmgr.Enum(ROAM_TYPE_ROBOT, EnumRobotMember, &mapRobot);

	for(int iIndex = 0; iIndex < inum; iIndex++ )
	{
		if(mapRobot.find(pallqeid[iIndex]) != mapRobot.end())
		{
			continue;
		}
		memset(szValue, 0, sizeof(szValue));
		swprintf(szValue, kMemberId, pallqeid[iIndex]);
		strCmd += szValue;
	}
	strCmd += kAllCmdEnd;

	m_pIMEngine->Command((TCHAR*)strCmd.c_str());
}

// 发送订阅请求
void C_eIMStateSubscribeMgr::SendSubscribeInfoReq(VT_EmpID& vtallqeid, int iOperType)							
{
	if(vtallqeid.size() == 0)
	{
		return;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("count=%d"), vtallqeid.size()); 
	TCHAR  szValue[XML_VALUE_SIZE_MAX] = { 0 };

	eIMString strCmd;
	swprintf(szValue, kCmdSubscribeAction, iOperType);
	strCmd += szValue;

	std::map<QEID, QEID> mapRobot;
	C_eIMRoamDataMgr rmgr;
	rmgr.Enum(ROAM_TYPE_ROBOT, EnumRobotMember, &mapRobot);

	for(VT_EmpID::iterator Iter = vtallqeid.begin(); Iter != vtallqeid.end(); Iter++)
	{
		if(mapRobot.find(*Iter) != mapRobot.end())
		{
			continue;
		}
		memset(szValue, 0, sizeof(szValue));
		swprintf(szValue, kMemberId, *Iter);
		strCmd += szValue;
	}
	strCmd += kAllCmdEnd;

	m_pIMEngine->Command((TCHAR*)strCmd.c_str());
}

// 发送订阅请求
void C_eIMStateSubscribeMgr::SendSubscribeInfoReq(Map_EmpID& mapallqeid, int iOperType)						
{
	if(mapallqeid.size() == 0)
	{
		return;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("count=%d"), mapallqeid.size()); 
	TCHAR  szValue[XML_VALUE_SIZE_MAX] = { 0 };
	eIMString strCmd;

	swprintf(szValue, kCmdSubscribeAction, iOperType);
	strCmd += szValue;

	std::map<QEID, QEID> mapRobot;
	C_eIMRoamDataMgr rmgr;
	rmgr.Enum(ROAM_TYPE_ROBOT, EnumRobotMember, &mapRobot);

	for(Map_EmpID::iterator Iter = mapallqeid.begin(); Iter != mapallqeid.end(); Iter++)
	{
		if(mapRobot.find(Iter->first) != mapRobot.end())
		{
			continue;
		}
		memset(szValue, 0, sizeof(szValue));
		swprintf(szValue, kMemberId, Iter->first);
		strCmd += szValue;
	}
	strCmd += kAllCmdEnd;

	m_pIMEngine->Command((TCHAR*)strCmd.c_str());
}

// 拉取一次状态
void C_eIMStateSubscribeMgr::SendGetStatesInfoReq(PQEID pallqeid, int inum)									
{
	if(inum <= 0)
	{
		return;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("count=%d"), inum); 
	TCHAR  szValue[XML_VALUE_SIZE_MAX] = { 0 };
	eIMString strCmd;
 
	swprintf(szValue, kCmdGetStatesAction, GETSTATE_USER);
	strCmd += szValue;

	std::map<QEID, QEID> mapRobot;
	C_eIMRoamDataMgr rmgr;
	rmgr.Enum(ROAM_TYPE_ROBOT, EnumRobotMember, &mapRobot);

	for(int iIndex = 0; iIndex < inum; iIndex++ )
	{
		if(mapRobot.find(pallqeid[iIndex]) != mapRobot.end())
		{
			continue;
		}
		memset(szValue, 0, sizeof(szValue));
		swprintf(szValue, kMemberId, pallqeid[iIndex]);
		strCmd += szValue;
	}
	strCmd += kAllCmdEnd;
	m_pIMEngine->Command((TCHAR*)strCmd.c_str());
}

// 拉取一次状态
void C_eIMStateSubscribeMgr::SendGetStatesInfoReq(Map_EmpID& mapallqeid)
{
	if(mapallqeid.size() == 0)
	{
		return;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("count=%d"), mapallqeid.size()); 
	TCHAR  szValue[XML_VALUE_SIZE_MAX] = { 0 };
	eIMString strCmd;

	swprintf(szValue, kCmdGetStatesAction, GETSTATE_USER);
	strCmd += szValue;

	std::map<QEID, QEID> mapRobot;
	C_eIMRoamDataMgr rmgr;
	rmgr.Enum(ROAM_TYPE_ROBOT, EnumRobotMember, &mapRobot);

	for(Map_EmpID::iterator Iter = mapallqeid.begin(); Iter != mapallqeid.end(); Iter++)
	{
		if(mapRobot.find(Iter->first) != mapRobot.end())
		{
			continue;
		}
		memset(szValue, 0, sizeof(szValue));
		swprintf(szValue, kMemberId, Iter->first);
		strCmd += szValue;
	}
	strCmd += kAllCmdEnd;
	m_pIMEngine->Command((TCHAR*)strCmd.c_str());
}

// 拉取一次状态
void C_eIMStateSubscribeMgr::SendGetDeptStatesInfoReq(Map_DeptID& mapallqdid)
{
	if(mapallqdid.size() == 0)
	{
		return;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("count=%d"), mapallqdid.size()); 
	TCHAR  szValue[XML_VALUE_SIZE_MAX] = { 0 };
	eIMString strCmd;

	swprintf(szValue, kCmdGetStatesAction, GETSTATE_DEPT);
	strCmd += szValue;

	for(Map_DeptID::iterator Iter = mapallqdid.begin(); Iter != mapallqdid.end(); Iter++)
	{
		memset(szValue, 0, sizeof(szValue));
		swprintf(szValue, kMemberId, Iter->first);
		strCmd += szValue;
	}
	strCmd += kAllCmdEnd;
	m_pIMEngine->Command((TCHAR*)strCmd.c_str());
}

// 拉取节点的状态
void C_eIMStateSubscribeMgr::SendGetStatesInfoReq(UINT64 u64ItemID, int iItemType)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("dwItemID=%lld iItemType=%d"), u64ItemID, iItemType); 
	switch(iItemType)
	{
	case ITEMTYPE_DEPT:
	case ITEMTYPE_TOP_DEPT:
		{
			// 通过人员拉取状态 
#if 0
			Map_EmpID mapMember;
			GetItemMember(iItemID, iItemType, mapMember);
			SendGetStatesInfoReq(mapMember);
#else
			// 通过部门拉取状态
			Map_DeptID mapdept;
			mapdept[(QDID)u64ItemID] = (QDID)u64ItemID;
			SendGetDeptStatesInfoReq(mapdept);
#endif
		}
		break;
	case ITEMTYPE_CUSTOMGROUP:
		{
			Map_EmpID mapMember;
			GetItemMember(u64ItemID, iItemType, mapMember);
			SendGetStatesInfoReq(mapMember);
		}
		break;
	case ITEMTYPE_COMMONGROUP:
		{
			Map_EmpID mapMember;
			GetItemMember(u64ItemID, iItemType, mapMember);
			SendGetStatesInfoReq(mapMember);
		}
		break;
	}
}

int __stdcall C_eIMStateSubscribeMgr::EnumSession(PS_SessionInfo_ psInfo, void* pvUserData)
{
	vector<QSID>* pvect = (vector<QSID>*)pvUserData;
	pvect->push_back(psInfo->qsid);
	if(pvect->size() == RECENT_SUBSCRIBE_COUNT_LIMIT)	// 最近联系人限制
	{
		return 0;
	}
	return 1;
}

int __stdcall C_eIMStateSubscribeMgr::EnumSessionEmps(QEID qeid, void* pvUserData)
{
	vector<QEID>* pvect = (vector<QEID>*)pvUserData;
	pvect->push_back(qeid);
	return 1;
}

// 获取节点成员
void C_eIMStateSubscribeMgr::GetItemMember(UINT64 u64ItemID, int iItemType, Map_EmpID& mapAddMember)				
{
	QEID eid = 0;
	switch(iItemType)
	{
	case ITEMTYPE_DEPT:
	case ITEMTYPE_TOP_DEPT:
		{
			C_eIMContactsMgr cmgr;
			VectEmpId vtEmpID;
			cmgr.GetDeptMembers((QDID)u64ItemID, vtEmpID);
			for(VectEmpId::iterator Iter = vtEmpID.begin(); Iter != vtEmpID.end(); Iter++)
			{
				eid = GET_QEID_(*Iter);
				mapAddMember[eid] = GET_QEID_(eid);
			}
		}
		break;
	case ITEMTYPE_CUSTOMGROUP:
		{
			C_eIMRoamDataMgr rmgr;
			VectEmpId vtEmpID;
			rmgr.EnumDefineGroupMember((QGID)u64ItemID, vtEmpID);
			for(VectEmpIdIt Iter = vtEmpID.begin(); Iter != vtEmpID.end(); Iter++)
			{
				eid = GET_QEID_(*Iter);
				mapAddMember[eid] = GET_QEID_(eid);
			}
		}
		break;
	case ITEMTYPE_COMMONGROUP:
		{
			VectSessionMember vtEmpID;
			m_pIMSessionMgr->EnumMember(u64ItemID,EnumSessionEmps,&vtEmpID);
			for(VectSessionMember::iterator Iter = vtEmpID.begin(); Iter != vtEmpID.end(); Iter++)
			{
				eid = GET_QEID_(*Iter);
				mapAddMember[eid] = GET_QEID_(eid);
			}
		}
		break;
	}
}

// 重发送所有状态订阅的（离线后上线）
void C_eIMStateSubscribeMgr::DoReSendAllStateSubScribe()
{
	Map_EmpID mapMember;
	Map_SubscribeInfoIter it = m_mapAllSubscribeInfo.begin();
	while(it != m_mapAllSubscribeInfo.end())
	{
		if(it->second.count > 0)
		{
			mapMember[it->second.eid_] = it->second.eid_;
		}
		it++;
	}
	SendSubscribeInfoReq(mapMember, OPERATE_ADD);
}

int C_eIMStateSubscribeMgr::UpdateEmpSubcribe(QEID qeid, int iOperateType, BOOL bAddTime/*=FALSE*/)
{
	CHECK_INIT_RET_(FALSE);
	int iRet = 0;

//	PS_EmpInfo_ psEmpInfo = m_pIMEngine->GetLoginEmp();
	if(qeid == m_pIMEngine->GetPurview(GET_LOGIN_QEID))
	{
		return iRet;
	}

#define ADD_TIME_VALUE 24*60*60
	SUBSCRIBE_INFO rInfo = { 0 };

	if(iOperateType == OPERATE_ADD)
	{
		DWORD dwNow = (DWORD)time(NULL);
		Map_SubscribeInfoIter it = m_mapAllSubscribeInfo.find(qeid);
		if(it != m_mapAllSubscribeInfo.end())
		{
			rInfo = it->second;
			rInfo.count += 1;
			rInfo.createtime = dwNow;
			if(bAddTime)
			{
				rInfo.createtime = rInfo.createtime + ADD_TIME_VALUE;
			}
		}
		else
		{
			rInfo.eid_ = qeid;
			rInfo.count = 1;
			rInfo.updatetime = dwNow;
			rInfo.createtime = dwNow;
			if(bAddTime)
			{
				rInfo.createtime = rInfo.createtime + ADD_TIME_VALUE;
			}
		}
		if(rInfo.count == 1)
		{
			// 发起订阅
			SendSubscribeInfoReq(&qeid, 1, iOperateType);
		}
		m_mapAllSubscribeInfo[qeid] = rInfo;
	}
	else if(iOperateType == OPERATE_DELETE)
	{
		Map_SubscribeInfoIter it = m_mapAllSubscribeInfo.find(qeid);
		if(it != m_mapAllSubscribeInfo.end())
		{
			rInfo = it->second;
			// 大于1 减引用计数
			if(rInfo.count > 1)
			{
				rInfo.count -= 1;
			}
			// 删除数据
			else
			{
				rInfo.count = 0;
				// 上报删除订阅消息
				//SendSubscribeInfoReq(&qeid, 1, iOperateType);
			}
			m_mapAllSubscribeInfo[qeid] = rInfo;
		}
	}
	return iRet;
}

int C_eIMStateSubscribeMgr::UpdateEmpsSubcribe(VT_EmpID& vtEid, int iOperateType)
{
	CHECK_INIT_RET_(FALSE);
	int iRet = 0;

	if(vtEid.size() == 0)
	{
		return FALSE;
	}

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("emps count=%d OperType=%d"), vtEid.size(), iOperateType); 

	QEID qeid = m_pIMEngine->GetPurview(GET_LOGIN_QEID);

	VT_EmpID vtToServer;

	SUBSCRIBE_INFO rInfo = { 0 };

	BOOL bGet = FALSE;
	if(iOperateType == OPERATE_ADD)
	{
		DWORD dwNow = (DWORD)time(NULL);
		for(VT_EmpID::iterator Iter = vtEid.begin(); Iter != vtEid.end(); Iter++)
		{
			if(*Iter == qeid)
			{
				continue;
			}
			Map_SubscribeInfoIter it = m_mapAllSubscribeInfo.find(*Iter);
			if(it != m_mapAllSubscribeInfo.end())
			{
				rInfo = it->second;
				rInfo.count += 1;
				rInfo.createtime = dwNow;
			}
			else
			{
				rInfo.eid_ = *Iter;
				rInfo.count = 1;
				rInfo.updatetime = dwNow;
				rInfo.createtime = dwNow;
			}
			if(rInfo.count == 1)
			{
				// 新增订阅数据
				vtToServer.push_back(*Iter);
			}
			m_mapAllSubscribeInfo[*Iter] = rInfo;
		}

		// 发起订阅
		SendSubscribeInfoReq(vtToServer, iOperateType);
	}
	else if(iOperateType == OPERATE_DELETE)
	{
		for(VT_EmpID::iterator Iter = vtEid.begin(); Iter != vtEid.end(); Iter++)
		{
			Map_SubscribeInfoIter it = m_mapAllSubscribeInfo.find(*Iter);
			if(it != m_mapAllSubscribeInfo.end())
			{
				rInfo = it->second;
				// 大于1 减引用计数
				if(rInfo.count > 1)
				{
					rInfo.count -= 1;
				}
				// 删除数据
				else
				{
					rInfo.count = 0;
					// 添加删除订阅消息
					//vtToServer.push_back(*Iter);
				}
				m_mapAllSubscribeInfo[*Iter] = rInfo;
			}
		}

		// 删除订阅
		SendSubscribeInfoReq(vtToServer, iOperateType);
	}

	return iRet;
}


int C_eIMStateSubscribeMgr::UpdateExpandItemData(UINT64 u64ItemID, int iItemType, int iOperateType)
{
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	if( iItemType > ITEMTYPE_COUNT)
	{
		EIMLOGGER_ERROR_(pILog, _T("itmetype error itemtype=%d"), iItemType); 
		return 0;
	}
	EIMLOGGER_DEBUG_(pILog, _T("itemid=%lld, itemtype=%d, iOperType=%d"), u64ItemID, iItemType, iOperateType); 

	int iRet = 0;

	EXPANDITEM_INFO rInfo = { 0 };
	BOOL bGet = FALSE;
	Map_ExpandItemInfoIter Iter = m_zMapAllExpandItemInfo[iItemType-1].find(u64ItemID);
	if(Iter != m_zMapAllExpandItemInfo[iItemType-1].end())
	{
		bGet = TRUE;
		rInfo = Iter->second;
	}
	if(iOperateType == OPERATE_ADD)
	{
		DWORD dwNow = (DWORD)time(NULL);
		if(bGet)
		{
			rInfo.count += 1;//曾经拉取过状态的，由定时线程来拉取状态
		}
		else
		{
			rInfo.id_ = u64ItemID;
			rInfo.count = 1;
			rInfo.expandtime = dwNow;
			rInfo.type = iItemType;
		}
		if(rInfo.count == 1)
		{
			//拉取一次状态
			SendGetStatesInfoReq(u64ItemID, iItemType);
		}
		m_zMapAllExpandItemInfo[iItemType-1][u64ItemID] = rInfo;
	}
	else if(iOperateType == OPERATE_DELETE && bGet)
	{
		// 大于1 减引用计数
		if(rInfo.count > 1)
		{
			rInfo.count -= 1;
		}
		// 删除数据
		else
		{
			rInfo.count = 0;
		}
		m_zMapAllExpandItemInfo[iItemType-1][u64ItemID] = rInfo;
	}

	return iRet;
}


void C_eIMStateSubscribeMgr::SetUIPage(int iUIPage)
{
	m_iCurUIPage = iUIPage;
}

int C_eIMStateSubscribeMgr::GetUIPage()
{
	return m_iCurUIPage;
}


// 初始订阅最近联系人的用户状态
int C_eIMStateSubscribeMgr::FirstSubscribeRecentContact()
{
	if(m_mapAllSubscribeInfo.size() > 0)
	{
		DoReSendAllStateSubScribe();
		return 0;
	}
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("go")); 

	CHECK_INIT_RET_(FALSE);

	vectSessionIDs vectSessionId;
	int iNum = m_pIMSessionMgr->Enum((E_SEFlag)(eSEFLAG_SINGLE),EnumSession,(void*)&vectSessionId);
	VT_EmpID			vtEmpInfo;
	VectSessionMember	vectEmps;
	VectSessionMemberIt vectEmpsIt;

	QEID qeid = m_pIMEngine->GetPurview(GET_LOGIN_QEID);

	itSessionsIDs it = vectSessionId.begin();
	while(it != vectSessionId.end())
	{
		vectEmps.clear();
		m_pIMSessionMgr->EnumMember(*it,EnumSessionEmps,&vectEmps);
		ASSERT_(vectEmps.size() <=2 );
		for (vectEmpsIt = vectEmps.begin();vectEmpsIt!=vectEmps.end();vectEmpsIt++)
		{
			if(*vectEmpsIt != qeid) 
			{
				vtEmpInfo.push_back(*vectEmpsIt);
				break;
			}
		}
		it++;
	}
	UpdateEmpsSubcribe(vtEmpInfo, OPERATE_ADD);
	return TRUE;
}

bool SubscribeCompare(SUBSCRIBE_INFO& first,SUBSCRIBE_INFO& second) 
{																	
	return first.createtime < second.createtime;
}
// 订阅状态数据维护
void C_eIMStateSubscribeMgr::CheckStateSubscribeData(BOOL* pbAbort)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("go")); 
	// 订阅======================================
	DWORD dwNowTime = (DWORD)time(NULL);

	VectSessionMember vtToServer;
	// 取消订阅超时的
	Map_SubscribeInfoIter Iter = m_mapAllSubscribeInfo.begin();
	DWORD dwSubscribeInterval  = m_pIMEngine->GetPurview(SUBSCRIBE_INTERVAL) * 60;//订阅超时间隔
	while(Iter != m_mapAllSubscribeInfo.end())
	{
		if(*pbAbort)
		{
			return;
		}
		SUBSCRIBE_INFO& rInfo = Iter->second;
		// 未超时不作处理
		if(dwNowTime - rInfo.updatetime < dwSubscribeInterval)
		{
			Iter++;
			continue;
		}
		// 已经超时
		// 引用计数大于0 更新时间
		if(rInfo.count > 0)
		{
			rInfo.updatetime = dwNowTime;
			m_mapAllSubscribeInfo[Iter->first] = rInfo;
			Iter++;
		}
		// 取消订阅
		else
		{
			// 添加取消订阅数据
			vtToServer.push_back(Iter->first);

			Iter = m_mapAllSubscribeInfo.erase(Iter);
		}
	}

	// 超过订阅数量限制的要清除
	int iOver = m_mapAllSubscribeInfo.size() - m_pIMEngine->GetPurview(TEMP_SUBSCRIBE_MAX_NUM);
	if(iOver > 0)// 此处有按创建时间排序，所以清除队列前面的即可
	{
		EIMLOGGER_DEBUG_(pILog, _T("over count %d"), iOver); 
		std::vector<SUBSCRIBE_INFO> VtSort;
		for(Map_SubscribeInfoIter Iter = m_mapAllSubscribeInfo.begin();Iter != m_mapAllSubscribeInfo.end();Iter++)
		{
			VtSort.push_back(Iter->second);
		}
		sort(VtSort.begin(),VtSort.end(),SubscribeCompare);
		for(std::vector<SUBSCRIBE_INFO>::iterator IterTemp = VtSort.begin();iOver > 0 && IterTemp != VtSort.end();IterTemp++)
		{
			iOver--;
			vtToServer.push_back(IterTemp->eid_);
			Map_SubscribeInfoIter IterFind = m_mapAllSubscribeInfo.find(IterTemp->eid_);
			if(IterFind != m_mapAllSubscribeInfo.end())
			{
				m_mapAllSubscribeInfo.erase(IterFind);
			}
		}
	}
	// 处理取消订阅
	SendSubscribeInfoReq(vtToServer, OPERATE_DELETE);
}

// 拉取状态数据维护
void C_eIMStateSubscribeMgr::CheckStateGetData(BOOL* pbAbort)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_DEBUG_(pILog, _T("go")); 

#define CHECK_ABORT_RET(ABORT)  if(ABORT)  return;

	DWORD dwNowTime = (DWORD)time(NULL);
	// 状态拉取===========================================
	// 判断主界面是否可见不可见不处理 todo...

	//状态拉取
	DWORD dwGetStateInterval =m_pIMEngine->GetPurview(GET_STATUS_INTERVAL) * 60;//拉取超时间隔
	for(int i= ITEMTYPE_DEPT - 1 ; i<= ITEMTYPE_COMMONGROUP - 1; i++)
	{
		if(m_zMapAllExpandItemInfo[i].size() == 0)
		{
			continue;
		}
		Map_ExpandItemInfoIter Iter = m_zMapAllExpandItemInfo[i].begin();
		while(Iter != m_zMapAllExpandItemInfo[i].end())
		{
			CHECK_ABORT_RET(*pbAbort);
			EXPANDITEM_INFO& rInfo = Iter->second;
			// 未超时 或者引用计数为0   不拉取
			if(dwNowTime - rInfo.expandtime < dwGetStateInterval)
			{
				Iter++;
				continue;
			}

			if(rInfo.count == 0)
			{
				Iter = m_zMapAllExpandItemInfo[i].erase(Iter);
				continue;
			}

			// 切换在组织架构页面  // 只判断拉取组织架构页面的节点
			if((GetUIPage() == UIPAGE_DEPT && rInfo.type == ITEMTYPE_DEPT)
				|| (rInfo.type != ITEMTYPE_DEPT))
			{
				rInfo.expandtime = dwNowTime;
				m_zMapAllExpandItemInfo[i][Iter->first] = rInfo;
				SendGetStatesInfoReq(rInfo.id_, rInfo.type);
				Iter++;
				continue;
			}
			Iter++;
		}
	}
}


IMPLEMENT_PLUGIN_SINGLETON_(C_eIMStateSubscribeMgr, INAME_EIMENGINE_SUBSCRIBE);

C_eIMStateSubscribeMgr::~C_eIMStateSubscribeMgr(void)
{
	Uninit();
}

C_eIMStateSubscribeMgr& C_eIMStateSubscribeMgr::GetObject()
{
	static C_eIMStateSubscribeMgr obj;
	return obj;
}

BOOL C_eIMStateSubscribeMgr::Init()
{
	if (m_pIMEngine==NULL && FAILED(GetEngineInterface(INAME_EIMENGINE, (void **)&m_pIMEngine)))
		return FALSE;

	if (m_pIMSessionMgr==NULL && FAILED(GetEngineInterface(INAME_EIMENGINE_SESSIONMGR, (void **)&m_pIMSessionMgr)))
		return FALSE;

	return TRUE;
}

BOOL C_eIMStateSubscribeMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIMSessionMgr);
	SAFE_RELEASE_INTERFACE_(m_pIMEngine);
	return TRUE;
}

int __stdcall C_eIMStateSubscribeMgr::EnumRobotMember(PROAM_DATA_INFO psInfo, void* pvUserData)
{
	std::map<QEID, QEID>* pvtData = (std::map<QEID, QEID>*)pvUserData;
	(*pvtData)[(QEID)psInfo->id] = (QEID)psInfo->id;
	
	return 1;
}

void C_eIMStateSubscribeMgr::UpdateRobotStateOnline()
{
	m_mapRobot.clear();
	C_eIMRoamDataMgr rmgr;
	rmgr.Enum(ROAM_TYPE_ROBOT, EnumRobotMember, &m_mapRobot);
	rmgr.Enum(ROAM_TYPE_VIRGROUP, EnumRobotMember, &m_mapVirGroupMem);

	I_EIMContacts* IEIMContacts = NULL;
	if (FAILED(GetEngineInterface(INAME_EIMENGINE_CONTACTS,(void**)&IEIMContacts)))
		return ;

	AUTO_RELEASE_(IEIMContacts);
	// 设置机器人状态为在线
	std::map<QEID, QEID>::iterator itRobot = m_mapRobot.begin();
	while(itRobot != m_mapRobot.end())
	{
		IEIMContacts->SetEmpState(itRobot->first, IME_EMP_STATUS_ONLINE, IME_EMP_TERM_PC);
		itRobot++;
	}
}

BOOL C_eIMStateSubscribeMgr::IsRobot(QEID eid)
{
	if(m_mapRobot.find(eid) != m_mapRobot.end())
	{
		return TRUE;
	}
	return FALSE;
}

BOOL C_eIMStateSubscribeMgr::IsVirGroupMain(QEID eid)
{
	if(m_mapVirGroupMem.find(eid) != m_mapVirGroupMem.end())
	{
		return TRUE;
	}
	return FALSE;
}

