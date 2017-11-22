#ifndef __IEIMUI_MESSAGE_PROMPTMGR_HEADER_2014_09_05_HDP__
#define __IEIMUI_MESSAGE_PROMPTMGR_HEADER_2014_09_05_HDP__

#include "public\plugin.h"
#include "Public\Typedefs.h"

#define INAME_EIMUI_PROMPTMGR					_T("SXIT.EIMUI.PROMPTMGR")	// I_EIMUIPromptMgr interface name

#define WARN_PROMPT_WND_NO_NEED		0
#define WARN_PROMPT_WND_NEED		1

typedef enum tagWndType
{
	eMsgPrompt,			//消息提示窗口
	eMsgOffline,		//离线Tip提示窗口
	eWarnPrompt,		//警告提示窗口 WndInfo 
	eConcernPrompt,		//关注联系人提示
	eKickPrompt,		//被踢提示
	eOpenFilePrompt,	//打开文件提示
	eSmsReply,			//短信回复
	eBroReply,			//接收到的广播
    eRemindCollect		//收藏
}E_WndType,*PE_WndType;

typedef enum tagWarnType
{
	eWarnBeyondMaxMemberInGroup, //警告：群组超出最大成员
	eWarnBeyondMaxMemberInMeeting, //警告：会议超出最大成员

}E_WarnType,*PE_WarnType;

typedef enum tagIconType
{
	ePROMPT_LOGOLINE = 1,	// 在线
	ePROMPT_LOGOLIVE,		// 离开
	ePROMPT_LOGINBF,		// 离线
	ePROMPT_MSGBOY,			// 消息提示-男
	ePROMPT_MSGGROUP,		// 消息提示-群
	ePROMPT_OFFLINE,		// 消息提示-离线消息
	ePROMPT_MSGGIRL,		// 消息提示-女
	ePROMPT_SMS,			// 消息提示-短信
	ePROMPT_BRO,			// 消息提示-广播
	ePROMPT_RDP,			// 消息提示-RDP
	/*ePROMPT_MSGBOY_BF,
	ePROMPT_MSGBOY_LIVE,
	ePROMPT_MSGBOY_PH,
	
	ePROMPT_MSGGIRL_BF,
	ePROMPT_MSGGIRL_LIVE,
	ePROMPT_MSGGIRL_PH,
	ePROMPT_MSGGROUP,*/

}ENUM_ICONTYPE;

typedef struct tagWndInfo
{
	tagWndInfo()
		: qsid(0)
		, i32MsgType(0)
		, iMsgCount(0)
		, dwRet(0)
		, eWarnType(eWarnBeyondMaxMemberInGroup)
		, eIconType(ePROMPT_LOGOLINE)
		, u64Phone(0)
		, pszTitle(NULL)
		, pszMsg(NULL)
		, bIs1To10T(FALSE)
	{
	}
	/*common*/
	DWORD			dwRet;				//返回值 /*是否以后需要提示*/ 查看WARN_PROMPT_WND_*
	DWORD			dwShowModalWndRet;	//DoModal 返回
	HWND			hWndParent;			//警告提示窗口父窗口
	QEID			qeid;				//人员Id

	/*eMsgPrompt wnd info*/
	QSID			qsid;				//消息会话Id
	int				i32MsgType;			//消息类型group、single
	const			TCHAR* pszTitle;	//消息会话主题
	const			TCHAR* pszMsg;		//消息内容
	int				iMsgCount;			//Message type

	/*warn prompt info*/
	E_WarnType		eWarnType;			//警告类型，该值 eWarnBeyondMaxMemberInGroup 关注。

	UINT64			u64Phone;			// Phone
	ENUM_ICONTYPE   eIconType;			// Icon type
	BOOL			bIs1To10T;          //是否一呼万应提示，如果为一呼万应则不递增未读消息

}S_WndInfo,*PS_WndInfo;



class __declspec(novtable) I_EIMUIPromptMgr: public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     Show()
	//Description:	Show the MsgHandleWnd Wnd ,MsgBox Wnd, MsgPrompt
	//
	//Parameter:
	//	eWndType     	- 
	//	sWndInfo     	- 
	//Return:
	//		0 
	//		
	//=============================================================================
	virtual int Show(
		E_WndType	eWndType,
		PS_WndInfo	psWndInfo
		) = 0;
};




#endif // __IEIMUI_MESSAGE_PROMPTMGR_HEADER_2014_09_05_HDP__