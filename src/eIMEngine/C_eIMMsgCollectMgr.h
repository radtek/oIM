#pragma once

#include "eIMEngine\IeIMMsgCollectMgr.h"

class C_eIMMsgCollectMgr:public I_EIMMsgCollectMgr
{
public:
	DECALRE_PLUGIN_(C_eIMMsgCollectMgr)

    C_eIMMsgCollectMgr(void);
    ~C_eIMMsgCollectMgr(void);

	virtual BOOL Init(I_SQLite3* pIDb = NULL);
	virtual BOOL Uninit();
    virtual int Set(const S_CollectMsg& sMsg, const TCHAR* pszXml, const TCHAR* pszTitle);
    DWORD GetCount();
    virtual DWORD  EnumMsgCollect(E_CollectType eType, PFN_GetCollectItem pfnGetCollectItem, void *pvUserData);
    virtual DWORD  EnumMsgCollect(LPCTSTR pszSearch, PFN_GetCollectItem pfnGetCollectItem, void *pvUserData);
    virtual BOOL Delete(QMID collectID);
    virtual BOOL GetContents(QMID collectID, PFN_GetCollectContents pfnGetContents, void* pvUserData);
    virtual BOOL GetCollectTime(QEID UserId,  DWORD& collectTime);
    virtual BOOL SetCollectTime(QEID UserId,  DWORD collectTime);
private:
	
    BOOL isSearchData(DWORD dwCollectTime, LPCTSTR pszSearch);
    BOOL hasSearchData(const LPCTSTR& pszSearch, const eIMString& szXml);
private:
    DWORD		m_dwRef;
    I_SQLite3*	m_pIDb;
};

