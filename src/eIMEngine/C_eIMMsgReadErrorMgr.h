#pragma once
#include "sqllite\I_SQLite.h"

typedef int (__stdcall* PFN_GetAllMsgRead)(LPCTSTR pszXml, void* pvUserData);

class C_eIMMsgReadErrorMgr
{
public:
	 C_eIMMsgReadErrorMgr(void);
    ~C_eIMMsgReadErrorMgr(void);
    virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
    virtual BOOL Uninit();
    BOOL Set(UINT64 u64MsgId, UINT64 u32SenderId);
    BOOL Del(UINT64 dwMsgId);
    int GetCount();
    BOOL GetAllMsgReads(PFN_GetAllMsgRead pfnGetMsgs, void* pvUserData);
    eIMStringA Num2String(UINT64 dwNum);
private:
    I_SQLite3*	m_pIDb;
};

