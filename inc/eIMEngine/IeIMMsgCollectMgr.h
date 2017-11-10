/*
I_EIMMsgCollectMgr is a interface of manager collect db

*/
#ifndef __IEIMMSGCOLLECTMSGR_H__
#define __IEIMMSGCOLLECTMSGR_H__

#include "eIMEngine\IeIMSessionMgr.h"

/* collect type*/
typedef enum tagCollectType
{
    eCOLLECT_TYPE_ALL = 0,     //all type
    eCOLLECT_TYPE_TEXT,        //text type
    eCOLLECT_TYPE_IMAGE_TEXT,   //image_text type
    eCOLLECT_TYPE_VIOCE,        //voice type
    eCOLLECT_TYPE_IMAGE,       //image type
    eCOLLECT_TYPE_FILE,        //file type
}E_CollectType, *PE_CollectType;

typedef struct tagCollectMsg
{
    QMID               m_collectID;      //collectID
    E_CollectType	m_eType;	    // Session Type
    DWORD			m_dwTime;	// Send time
    int                    m_isGroup;   //0是单人,1是群主
    QEID				    m_sendid;    //sendid
    QSID                m_Sid;             //sessionid
}S_CollectMsg, *PS_CollectMsg;

/* get collect all info*/
typedef int (__stdcall* PFN_GetCollectItem)(S_CollectMsg sInfo, LPCTSTR pszXml,
                                                                    LPCTSTR pszTile, void* pvUserData);

/*get collect show content*/
typedef int (__stdcall* PFN_GetCollectContents)(LPCTSTR pszXml,  void* pvUserData);

class  __declspec(novtable) I_EIMMsgCollectMgr: public I_EIMUnknown
{
public:
    virtual int Set(const S_CollectMsg& sMsg, const TCHAR* pszXml, const TCHAR* pszTitle) = 0;
    virtual DWORD GetCount() = 0;
    /*get all collect info  by collecttype */
    virtual DWORD  EnumMsgCollect(E_CollectType eType, PFN_GetCollectItem pfnGetCollectItem, void *pvUserData) = 0;
    /*get all collect info by search vlaue*/
    virtual DWORD  EnumMsgCollect(LPCTSTR pszSearch, PFN_GetCollectItem pfnGetCollectItem, void *pvUserData) = 0;
    /*delete one collect by collectid*/
    virtual BOOL Delete(QMID collectID) = 0;
    /*get content by collectid*/
     virtual BOOL GetContents(QMID collectID, PFN_GetCollectContents pfnGetContents, void* pvUserData) = 0;
     /*get collecttime by userid*/
     virtual BOOL GetCollectTime(QEID UserId,  DWORD& collectTime) = 0;
     /*set collect time by userid*/
     virtual BOOL SetCollectTime(QEID UserId,  DWORD collectTime) = 0;
};

#endif // __IEIMMSGCOLLECTMSGR_H__