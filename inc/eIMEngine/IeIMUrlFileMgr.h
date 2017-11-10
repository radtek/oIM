
#ifndef __IEIMURLFILEMGR_H__
#define __IEIMURLFILEMGR_H__

#include <wininet.h>

typedef struct tagUrlFileInfo
{
    QMID qMid;
    TCHAR szDownUrl[INTERNET_MAX_URL_LENGTH];
    TCHAR szLocalPath[MAX_PATH];
    tagUrlFileInfo()
    {
        memset(szDownUrl, 0 , INTERNET_MAX_URL_LENGTH);
        memset(szLocalPath, 0 , MAX_PATH);
    };
}S_UrlFileInfo;

typedef int (__stdcall* PFN_GetLocalPath)(LPCTSTR pszLocalPath, void* pvUserData);

class  __declspec(novtable) I_EIMUrlFileMgr: public I_EIMUnknown
{
public:
    virtual int Set(const S_UrlFileInfo& sUrlInfo) = 0;
     virtual BOOL GetLocalPath(const S_UrlFileInfo& sUrlInfo, PFN_GetLocalPath pfnGetLocalPath, void* pvUserData) = 0;
};

#endif 