#include "stdafx.h"
#include "C_eIMSaveData.h"
#include "C_eIMEngine.h"

C_eIMSaveData::C_eIMSaveData()
{
	InitializeCriticalSection(&m_UpdatingGroupDataSession);
	InitializeCriticalSection(&m_ReqGroupDataSession);
}

C_eIMSaveData::~C_eIMSaveData()
{
	DeleteCriticalSection(&m_UpdatingGroupDataSession);
	DeleteCriticalSection(&m_ReqGroupDataSession);
}

void C_eIMSaveData::AddGroupIDUpdatingPage(QSID qGroupID)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	std::map<QSID,int>::iterator Iter = m_mapUpdatingGroupID.find(qGroupID);
	if(Iter != m_mapUpdatingGroupID.end())
	{
		m_mapUpdatingGroupID[qGroupID] = Iter->second + 1;
	}
	else
	{
		m_mapUpdatingGroupID[qGroupID] = 1;
	}
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	EIMLOGGER_DEBUG_( pILog, _T("Add Page Qsid:%llu"), qGroupID);
}

void C_eIMSaveData::DelGroupIDUpdating(QSID qGroupID)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	std::map<QSID,int>::iterator Iter = m_mapUpdatingGroupID.find(qGroupID);
	if(Iter != m_mapUpdatingGroupID.end())
	{
		m_mapUpdatingGroupID.erase(Iter);
	}
	LeaveCriticalSection(&m_UpdatingGroupDataSession);

	EIMLOGGER_DEBUG_( pILog, _T("Del updating group Qsid:%llu"), qGroupID);

}

int C_eIMSaveData::GetGroupIDUpdatingPage(QSID qGroupID)
{
	int iPageCount = 0;
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	std::map<QSID,int>::iterator Iter = m_mapUpdatingGroupID.find(qGroupID);
	if(Iter != m_mapUpdatingGroupID.end())
	{
		iPageCount = Iter->second;
	}
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	return iPageCount;
}

void C_eIMSaveData::ClearGroupIDUpdatingData()
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	m_mapUpdatingGroupID.clear();
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	EIMLOGGER_DEBUG_( pILog, _T("Clear updating group"));
}

// 请求更新的数据 避免重复请求
void C_eIMSaveData::AddGroupIDReq(QSID qGroupID)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	m_mapReqGroupID[qGroupID] = 1;
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	EIMLOGGER_DEBUG_( pILog, _T("Add group req Qsid:%llu"), qGroupID);
}

void C_eIMSaveData::DelGroupIDReq(QSID qGroupID)
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	std::map<QSID,int>::iterator Iter = m_mapReqGroupID.find(qGroupID);
	if(Iter != m_mapReqGroupID.end())
	{
		m_mapReqGroupID.erase(Iter);
	}
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	EIMLOGGER_DEBUG_( pILog, _T("Del group req Qsid:%llu"), qGroupID);
}

BOOL C_eIMSaveData::IsGroupIDHasReq(QSID qGroupID)
{
	BOOL bExist = FALSE;
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	std::map<QSID,int>::iterator Iter = m_mapReqGroupID.find(qGroupID);
	if(Iter != m_mapReqGroupID.end())
	{
		bExist = TRUE;
	}
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	return bExist;
}

void C_eIMSaveData::ClearGroupIDReq()
{
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	EnterCriticalSection(&m_UpdatingGroupDataSession);
	m_mapReqGroupID.clear();
	LeaveCriticalSection(&m_UpdatingGroupDataSession);
	EIMLOGGER_DEBUG_( pILog, _T("Clear req groups"));
}