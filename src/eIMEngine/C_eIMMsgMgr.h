#pragma once

#include "eIMEngine\IeIMMsgMgr.h"


class C_eIMMsgMgr: public I_EIMMsgMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMMsgMgr)

	C_eIMMsgMgr(void);
	~C_eIMMsgMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	virtual QSID Get(QMID qmid);
	virtual BOOL Get(QMID qmid, S_Msg& sMsg, eIMStringA& pszXmlA);
	virtual int  Set(PS_Msg psMsg, const TCHAR* pszXml, BOOL bReplace);	// pszXml is <Message />
	virtual BOOL UpdateFlag(QMID qmid, DWORD dwFlag, BOOL bSet = TRUE);

	virtual BOOL UpdateFlagByQSID(QSID qsid, DWORD dwFlag, BOOL bSet = TRUE);
    virtual BOOL UpdateType(QMID qmid, DWORD dwType);
    virtual BOOL UpdateMsg2ReCallType(SENDRECALLMSGACK sRecallInfo );
	virtual QMID GetLastId();

	//////////////////////////////////////////////////////////////////////////////
	virtual DWORD GetCount(
		QSID	qsid,
		DWORD	dwStart, 
		DWORD	dwEnd);

	virtual DWORD GetCount(PS_MsgEnum psCond);

	virtual DWORD Enum(
		QSID		qsid, 
		DWORD		dwStart,
		DWORD		dwEnd,
		PFN_DateCb	pfnDateCb,
		void*		pvUserData
		);

	virtual DWORD Enum( 
		PS_MsgEnum psCond,
		PFN_MsgCb pfnMsgCb,
		void*	  pvUserData,
		BOOL	  bRecent = FALSE
		);

	virtual DWORD SearchMsgCount(PS_MsgEnum psCond);

	virtual DWORD SearchMsg(PS_MsgEnum psCond, PFN_MsgCb pfnMsgCb, void* pvUserData);

	virtual BOOL Get(QMID qmid, PFN_MsgCb pfnMsgCb,   void*	 pvUserData);
    virtual BOOL Get(QMID qmid, PFN_MsgInfoCb pfnMsgCb,   void*	 pvUserData);

	virtual DWORD EnumBroMsg(PS_SessionEnum psCond, PFN_BroCb pfnBroMsgCb, void* pvUserData);

	virtual DWORD EnumMsgNoticeMsg(PS_MsgEnum psCond, PFN_MsgNoticeCb pfnMsgNoticeCb, void* pvUserData);

	virtual BOOL Delete(QSID qsid);

	virtual BOOL DeleteMsgByMsgID(QMID qmid);
	
	virtual BOOL DeleteSMSByID( QMID qmid);

	virtual BOOL DeleteBoardcastByID(QMID qmid);

	virtual BOOL DeleteMsgNoticeByID(QMID qmid);

    virtual BOOL DeleteMsg(QSID qSid, DWORD dwDelTime);


	virtual BOOL IsMsgIDExist(QMID qmid);
	virtual BOOL IsSMSIDExist(QMID qmid);
	virtual BOOL IsBroIDExist(QMID qmid);
	virtual BOOL IsMsgNoticeIDExist(QMID qmid);

	virtual DWORD GetSessionLastMsgTime(QSID qsid);

	virtual int  ReplaceSet(void* pszData);

	virtual DWORD GetRecordsBySid(QSID qSid,  PFN_RecordsAll pfnRecords, void* pvUserData);
    void DelMsgFile(TiXmlElement* pMessage);
    eIMString GetName(const TiXmlElement *pElement, LPCTSTR szDefault);
    eIMString GetAttributeValue(const TiXmlElement *pElement, LPCTSTR szAttriName, LPCTSTR szDefault);
    void GetNodeText(const TiXmlElement* pElementchild, eIMString& strText);
    void DelTextMsgImg(const TiXmlElement* pElementchild);
    void SetMidToXml(QMID qMsgId, eIMString&		szXml);
    virtual DWORD EnumMsgId(QSID qsid, DWORD dwTime,  PFN_MsgIdCb pfnMsgIdCb, void* pvUserData);
	BOOL UpdateReadMsgSyncNotice(QSID sid, DWORD dwTimestamp);

};

