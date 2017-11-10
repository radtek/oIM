#ifndef __EIM_SAVEDATA_CONST_HEADER_2015_02_09_YFGZ__
#define __EIM_SAVEDATA_CONST_HEADER_2015_02_09_YFGZ__

#pragma once

class C_eIMSaveData
{
public:
	C_eIMSaveData();
	~C_eIMSaveData();

public:
	// 正在更新的群组数据
	void AddGroupIDUpdatingPage(QSID qGroupID);
	void DelGroupIDUpdating(QSID qGroupID);
	int  GetGroupIDUpdatingPage(QSID qGroupID);
	void ClearGroupIDUpdatingData();

	// 请求更新的数据 避免重复请求
	void AddGroupIDReq(QSID qGroupID);
	void DelGroupIDReq(QSID qGroupID);
	BOOL IsGroupIDHasReq(QSID qGroupID);
	void ClearGroupIDReq();

private:
	std::map<QSID,int>	m_mapUpdatingGroupID;
	CRITICAL_SECTION m_UpdatingGroupDataSession;

	std::map<QSID,int>	m_mapReqGroupID;
	CRITICAL_SECTION m_ReqGroupDataSession;
};

#endif