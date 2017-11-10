#pragma once

#include "eIMEngine\IMeetingMgr.h"

class C_MeetingMgr:public I_MeetingMgr
{
public:
	DECALRE_PLUGIN_(C_MeetingMgr)

    C_MeetingMgr(void);
    virtual ~C_MeetingMgr(void);

	virtual BOOL Init(I_SQLite3* pIDb = NULL);
	virtual BOOL Uninit();
    virtual int Set(const confBasicInfo* basicInfo);
	virtual int Set(const confMbrInfoNotice* mbrInfo);
	virtual int Set(const confFileInfoNotice* fileInfo);
	virtual int Set(const confRemarksNotice* remarkInfo);
	virtual int SetMeetingUrl(const char* confid,const char* url,tagMeetingUrlColumn urlType);
    virtual DWORD  EnumMeeting(PFN_GetMeetingItem pfnGetMeetingItem, void *pvUserData);	

	virtual int Get(char* confId,PFN_MeetingBasicInfoCb pfnCb,void* pvUserData);
	virtual int Get(char* confId,PFN_MeetingBasicInfoCb_All pfnCb,void* pvUserData);
	virtual int Get(char* confId,PFN_MeetingBasicMbrInfoCb pfnCb,void* pvUserData);
	virtual int Get(char* confId,PFN_MeetingFileInfoCb pfnCb,void* pvUserData);
	virtual int GetMeetingTimeStamp();

	virtual int GetTodayMeetingCount(QEID userid,int &count);

	virtual int UpdateMeetingLevel(const char* confId,QEID qeid,int Level);
	virtual int GetMeetingLevel(char* confId,QEID qeid);

	virtual int GetMeetingCreator(const char* confid);

	virtual int UpdateFilePath(const char* confid,const TCHAR* fileToken,const TCHAR* filePath,long timeKey,int flag);

	virtual int UpdateMeetingIsRead(char* confId,QEID qeid,int IsRead);
	virtual int GetMeetingIsRead(char* confId,QEID qeid);

	virtual int GetMeetingIsAccept(const char* confId,QEID qeid);

	virtual BOOL CheckeFileIsExist(char* confid,char* token,long timeKey);

	virtual int DeletePathTableFile(const char* confid,const char* token);

	int ClearFilePathTable();
	BOOL _GetRow(I_SQLite3Table* pITable, confBasicInfo* psBasicInfo) ;
	BOOL _GetRow(I_SQLite3Table* pITable, S_ConfBasicInfo* psBasicInfo) ;
	BOOL _GetRow(I_SQLite3Table* pITable, confBasicMbrInfo* psBasicInfo) ;
	BOOL _GetRow(I_SQLite3Table* pITable, S_FileInf2Detail* psFileInfo);
	INT32  _GetOneResult(const char* pszSql, bool bFree);
	BOOL MeetingExists(const char* confid,const char* pzSql);

private:
private:
    DWORD		m_dwRef;
    I_SQLite3*	m_pIDb;

	BOOL m_HaveClear;
};

