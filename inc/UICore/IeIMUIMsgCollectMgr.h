
#ifndef __IEIMUIMSGCOLLECTMGR__H__
#define __IEIMUIMSGCOLLECTMGR__H__

#include "public\plugin.h"
#include "Public\Typedefs.h"

#include "eIMEngine/IeIMMsgCollectMgr.h"
	
#define INAME_EIMUI_MSGCOLLECTMGR					_T("SXIT.EIMUI.PLUGIN.EIMCOLLECT")	

#pragma region MessageCollectManagerConst

typedef int (__stdcall* PFN_GetRemindPrompt)(DWORD remindTime, LPCTSTR pszTile,   void* pvUserData);

typedef struct tagCollectPromptInfo
{
    DWORD m_RemindTime;		// Remind time
    CDuiString m_strTitle;	// Title
}S_CollectPromptInfo;

#define COLLECT_FLAG_SHOW	(0x00000001)	// 显示，未设置就是隐藏
#define COLLECT_FLAG_SIZE	(0x00000002)	// 更新窗口大小

class __declspec(novtable) I_EIMUIMsgCollectMgr: public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     Show
	//Description:	Show collect dialog
	//					
	//Return:
	//	HRESULT
	//=============================================================================
	virtual HWND Show(
		const RECT* prtPos,
		HWND  hwndParent = NULL,
		DWORD dwFlag = COLLECT_FLAG_SHOW
		) = 0;
	//=============================================================================
	//Function:     InitLoginAfter
	//Description:	Initial it when logined
	//
	//Return:
	//	None
	//=============================================================================
    //virtual void InitLoginAfter() = 0;
};




#endif // __IEIMUIMSGCOLLECTMGR__H__