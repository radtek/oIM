#pragma once

#include "eIMEngine\IeIMUrlFileMgr.h"

class C_eIMUrlFileMgr:public I_EIMUrlFileMgr
{
public:
	DECALRE_PLUGIN_(C_eIMUrlFileMgr)
    ~C_eIMUrlFileMgr(void);
    static C_eIMUrlFileMgr& GetObject();

    virtual BOOL Init();
    virtual BOOL Uninit();
    virtual int Set(const S_UrlFileInfo& sUrlInfo);
    virtual BOOL GetLocalPath(const S_UrlFileInfo& sUrlInfo, PFN_GetLocalPath pfnGetLocalPath, void* pvUserData);
private:
    C_eIMUrlFileMgr(void);
    I_SQLite3*	m_pIDb;
};

