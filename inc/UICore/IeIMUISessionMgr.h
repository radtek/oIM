// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMUISessionMgr is the session ui manager
// 
// Auth: yfgz
// Date:  15:32 2014/4/14
// 
// History: 
//   
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIMUI_SESSION_MANAGER_HEADER_2014_04_14_YFGZ__
#define __IEIMUI_SESSION_MANAGER_HEADER_2014_04_14_YFGZ__

#include <vector>
#include <list>
#include "public\plugin.h"
#include "public\XmlConfiger.h"
#include "eIMEngine\IeIMContacts.h"
#include "eIMEngine\IeIMEngine.h"

#define INAME_EIMUI_SESSIONMGR	_T("SXIT.EIMUI.SESSIONMGR")	// I_EIMUISessionMgr interface name

#define MAX_DISPLAY_SESSION				(120)	// Max display session to display
#define	MAX_DISPLAY_SESSION_DEFAULT		(20)	// Default max display session to display

#define	MAX_MERGED_WINDOW				(10)	// Max merged session window
#define	MAX_MERGED_SESSION_DEFAULT		(5)		// Default max merged session window

#define MAX_SESSION_WND_TITLE			(30)	//The long of session wnd title

//#define SEND_FILE_PURVIEW				0x2     //
//
////会话窗口类型
//typedef enum tagSessionType
//{
//	ST_NONE,			// Unknown
//	ST_SINGLE,			// Single
//	ST_GROUP,			// Group
//	ST_RGROUP = 4,		// regular group
//	//ST_*				// reserve
//	ST_VIRGROUP = 5,
//}E_SessionType,*PE_SessionType;

typedef struct tagMsgFont
{
//	CDuiString	szFace;			// Font face name
	BYTE		u8Id;			// Font id(index) 
	BYTE		u8Size;			// Font size
	union{
		BYTE	u8Mode;			// Font mode
		struct{
			BYTE bitBold:1;		// Bold
			BYTE bitItalic:1;	// Italic
			BYTE bitUnderline:1;// Underline
		};
	};
	COLORREF	crColor;		// Font color
}S_MsgFont, *PS_MsgFont;

#define MSG_SUMMARY_SIZE		(240)		// 摘要消息Buffer长度

// 如果只参考微信，保留最近几十条消息，就是不需要更多信息。
// RE 需要支持从前面插入
#define MSG_FLAG_ERROR		(0x00000001)	// RE显示错误类型(eErrorType)
#define MSG_FLAG_NOTICE		(0x00000002)	// RE显示通知类型(eNoticeType)
#define MSG_FLAG_NEWMSG		(0x00000004)	// 新消息
#define MSG_FLAG_RECALL		(0x00000008)	// 撤回消息
#define MSG_FLAG_READ		(0x00000010)	// 已读
#define MSG_FLAG_AT			(0x00000020)	// 已经@
typedef struct tagMsgRecord
{
	QMID		mid;		// 消息ID
	DWORD		dwSendTime;	// 发送时间
	QEID		eidSender;	// 发送者
	DWORD		dwFlag;		// 标识
	UINT8		u8Type;		// Type
	eIMString	szMsgRich;	// 消息<FIELD_MSGRICH>...</FIELD_MSGRICH>, 或者错误信息
	tagMsgRecord()
		: mid(0)
		, dwSendTime(0)
		, eidSender(0)
		, dwFlag(0)
		, u8Type(0)
	{
	}

	tagMsgRecord& operator= (const tagMsgRecord& s)
	{
		mid			= s.mid;		
		dwSendTime	= s.dwSendTime;	
		eidSender	= s.eidSender;	
		dwFlag		= s.dwFlag;		
		u8Type		= s.u8Type;
		szMsgRich	= s.szMsgRich;	

		return *this;
	}

	DWORD SetFlag(DWORD dwFlag, BOOL bClear)
	{
		if ( bClear )
			this->dwFlag &=~dwFlag;	// 清除标识
		else
			this->dwFlag |= dwFlag;	// 设置标识

		return dwFlag;
	}
}S_MsgRecord, *PS_MsgRecord;
typedef const struct tagMsgRecord* PS_MsgRecord_;

typedef std::list<S_MsgRecord>				ListSessionRecord;		// Session record
typedef ListSessionRecord::iterator			ListSessionRecordIt;	// Session record's iterator
typedef ListSessionRecord::const_iterator	ListSessionRecordIt_;	// Session record's const iterator	

typedef enum tagUserStatus
{
	eUnknown		= -1,	// 为了不显示状态
	eOFFLINE		= 0,	// 离线
	ePC_ONLINE		= MAKELONG(IME_EMP_TERM_PC, IME_EMP_STATUS_ONLINE),		// PC 在线
	ePC_LEAVE		= MAKELONG(IME_EMP_TERM_PC, IME_EMP_STATUS_LEAVE),		// PC 离开
	eIOS_ONLINE		= MAKELONG(IME_EMP_TERM_IOS, IME_EMP_STATUS_ONLINE),	// IOS 在线
	eIOS_LEAVE		= MAKELONG(IME_EMP_TERM_IOS, IME_EMP_STATUS_LEAVE),		// IOS 离开
	eANDROID_ONLINE = MAKELONG(IME_EMP_TERM_ANDROID, IME_EMP_STATUS_ONLINE),// Android 在线
	eANDROID_LEAVE  = MAKELONG(IME_EMP_TERM_ANDROID, IME_EMP_STATUS_LEAVE),	// Android 离开

}E_UserStatus, *PE_UserStatus;

inline E_UserStatus SetUserStatus(int i32Term, int i32Status)
{
	if ( i32Status == IME_EMP_STATUS_ONLINE )
		return (E_UserStatus)MAKELONG(i32Term, IME_EMP_STATUS_ONLINE);
	else if ( i32Status == IME_EMP_STATUS_LEAVE )
		return (E_UserStatus)MAKELONG(i32Term, IME_EMP_STATUS_LEAVE);
	else
		return eOFFLINE;
}

//会话窗口信息
#define LAST_SESSION_RECORD_MAX		(50)	// 保留最近消息的条数

// UI 会话标识
#define  SESSION_FLAG_MALE				(0x00000001)	// 是男性（没有设置就是女）
#define  SESSION_FLAG_SHIELD			(0x00000002)	// 消息免打扰
#define  SESSION_FLAG_TOP				(0x00000004)	// 置顶标识，涉及dwTopTime
#define  SESSION_FLAG_HIDE_ROW1			(0x00000008)	// 隐藏标题行
# define SESSION_FLAG_HIDE_TIME			(0x00000010)	// 隐藏时间
#define  SESSION_FLAG_HIDE_ROW2			(0x00000020)	// 隐藏消息行
# define SESSION_FLAG_HIDE_FLAG			(0x00000040)	// 隐藏FLAG
#define  SESSION_FLAG_HIDE_LOGO			(0x00000080)	// 隐藏LOGO
#define  SESSION_FLAG_SUMMARY_NAME		(0x00000400)	// 摘要显示名字（由外部设置是否显示发送者名字）
#define  SESSION_FLAG_SEND_FILE			(0x00000800)	// 打开会话时要发送文件
#define  SESSION_FLAG_CREATE_GROUP		(0x00001000)	// 需要创建群
#define  SESSION_FLAG_MSG_INITED		(0x00002000)	// 会话消息已经初始化
#define  SESSION_FLAG_INITED			(0x00002000)	// 会话已经初始化
#define  SESSION_FLAG_MAKE_LOGO			(0x00004000)	// 生成会话LOGO
#define  SESSION_FLAG_MEMBER_CHANGED	(0x00008000)	// 会话成员有变更
#define  SESSION_FLAG_SORT_RECORD		(0x00010000)	// 当接收到了新消息后，要再排序，以处理时间乱序问题
#define  SESSION_FLAG_NEW_FILES			(0x00020000)	// 有收发新的文件，需要更新Key
#define  SESSION_FLAG_CLOSE				(0x00040000)	// 启用Close按钮

#define	SESSION_FLAG_DISABLE_EVENT		(0x80000000)	// 禁用所有的事件（会话搜索列表，消息管理器等可以不用响应即时状态变化）

typedef struct tagUISessionInfo	
{
	QSID				sid;			// 会话ID
	E_SessionType		eType;			// 会话类型, 默认单聊
	DWORD				dwFlag;			// 会话标识，参看SESSION_FLAG_*
	QEID				eidCreater;		// 创建者ID
	QEID				eidReceiver;	// 单聊的接收者【对方】
	QEID				eidLastMsg;		// 最后一条消息的发送者
	E_UserStatus		eUserStatus;	// 单聊对方用户状态
	DWORD				dwTopTime;		// 置顶时间
	DWORD				dwModifyTime;	// 群成员最后变更时间（0需要再加载）
	DWORD				dwLastMsgTime;	// 最后消息更新时间, 初始默认是会话更新时间
	DWORD				dwNewMsgCount;	// 新消息条数（离线与未读，打开会话就是清除）
	SIZE				sizeScrollPos;	// 滚动条位置
	CDuiString			szTitle;		// 会话标题[单聊为对方名字]
	CDuiString			szLastSummary;	// 最后一条消息的摘要
	CDuiString			szDraft;		// 草稿
	VectSessionMember	vectMember;		// [qeidLogin, qeidReveiver,..]: 单聊第二个用户是接收者
	ListSessionRecord	listRecord;		// 保留最近消息(可能会是全部消息，自动清理一天之前的历史) // 微信只每次缓存了30条，切换会话就会清理
	tagUISessionInfo()
		: sid(0)
		, eType(eSESSION_TYPE_UNKNOWN)
		, dwFlag(0)
		, eidCreater(0)
		, eidReceiver(0)
		, eidLastMsg(0)
		, eUserStatus(eOFFLINE)
		, dwTopTime(0)
		, dwModifyTime(0)
		, dwLastMsgTime(0)
		, dwNewMsgCount(0)
	{
	}

	tagUISessionInfo& operator= (const tagUISessionInfo& s)
	{
		sid		     = s.sid;
		eType		 = s.eType;
		dwFlag		 = s.dwFlag;
		eidCreater   = s.eidCreater;
		eidReceiver  = s.eidReceiver;
		eidLastMsg   = s.eidLastMsg;
		eUserStatus  = s.eUserStatus;
		dwTopTime	 = s.dwTopTime;
		dwLastMsgTime= s.dwLastMsgTime;
		dwNewMsgCount= s.dwNewMsgCount;
		szTitle		 = s.szTitle;
		szLastSummary= s.szLastSummary;
		szDraft		 = s.szDraft;
		vectMember.assign(s.vectMember.begin(), s.vectMember.end());
		listRecord.assign(s.listRecord.begin(), s.listRecord.end());

		return *this;
	}

	DWORD SetMsgFlag(QMID mid, DWORD dwFlag, BOOL bClear)
	{
		for ( ListSessionRecordIt it = listRecord.begin(); it != listRecord.end(); it++)
		{
			if (it->mid != mid)
				continue;

			if ( dwFlag == MSG_FLAG_READ )
			{
				const TCHAR* pszRead0 =_T("Read=\"0\"");
				const TCHAR* pszRead1 =_T("Read=\"1\"");

				eIMString::size_type nPos = it->szMsgRich.find(pszRead1);
				if ( nPos != eIMString::npos )	// 更新内存中的Read标识
					it->szMsgRich.replace(nPos, _tcslen(pszRead1), pszRead0);
			}
			else if ( dwFlag == MSG_FLAG_AT )
			{
				const TCHAR* pszAt0 =_T("At=\"0\"");
				const TCHAR* pszAt1 =_T("At=\"1\"");

				eIMString::size_type nPos = it->szMsgRich.find(pszAt0);
				if ( nPos != eIMString::npos )	// 更新内存中的At标识
					it->szMsgRich.replace(nPos, _tcslen(pszAt0), pszAt1);
			}

			return it->SetFlag(dwFlag, bClear);
		}

		return 0;
	}
}S_UISessionInfo,*PS_UISessionInfo;
typedef const struct tagUISessionInfo* PS_UISessionInfo_;

//更新操作类型
typedef enum tagOperaType
{
	eUpdGroupAdd,	// Add
	eUpdGroupDel,	// Delete
	eUpdGroupUpd,	// Update
}E_OperaType,*PE_OperaType;

//外部更新会话类型
typedef enum tagUpdateType
{
	eUpdateGroupName	= 1,	// Update group name
	eUpdateGroupMember	= 2,	// Update group member
	eFreshGroup			= 4,	// Update group
}E_UpdateSessWndType;

//人员操作
typedef struct tagUpdateMember
{
	E_OperaType eOperaType;		// Operate type
	QEID		eid;			// Employee id
}S_UpdateMemberInfo,*PS_UpdateMemberInfo;

//定义员工的操作
typedef std::vector<S_UpdateMemberInfo>				VectUpdateMemInfos;
typedef std::vector<S_UpdateMemberInfo>::iterator	VectUpdateMemInfoIt;

//// 更新会话窗口信息
//typedef struct tagUpdateSessionWndData
//{
//	QSID					qsid;			// Employee id
//	E_UpdateSessWndType		eUpdateType;	// Update type
//	CDuiString				strGroupName;	// Group name
//	QEID					qoperid;		// ID 
//	VectUpdateMemInfos		vectMemberIds;	// Session member information
//
//	E_OperaType				eOperaType;		//批量操作类型
//	VectSessionMember		vectMembers;	//批量操作
//
//}S_UpdateSessionWndData,*PS_UpdateSessionWndData;

// Add define group's information
typedef struct tagAddDefineGroupInfo
{
	CDuiString		   strGroupName;		// Group name
	VectSessionMember  vecEmps;				// Members
}S_AddDefineGroupInfo, *PS_AddDefineGroupInfo;

class __declspec(novtable) I_EIMUISessionMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Uninit
	//Description:	Set configer to load and save the config
	//
	//Parameter:
	//Return:
	//		TRUE	Succeed
	//		FALSE	Failed
	//=============================================================================
	//virtual BOOL Uninit() = 0;

	//=============================================================================
	//Function:     SetMaxSession
	//Description:	Set the max member of session
	//
	//Parameter:
	//  dwMax	- The max member 
	//Return:
	//		TRUE	Succeed
	//		FALSE	Failed
	//=============================================================================
	//virtual int SetMaxSession(
	//	DWORD	dwMax = MAX_DISPLAY_SESSION_DEFAULT
	//	) = 0;

	//=============================================================================
	//Function:     SetMaxMergedWnd
	//Description:	Set the max merged window
	//
	//Parameter:
	//  dwMax	- The max window 
	//Return:
	//		TRUE	Succeed
	//		FALSE	Failed
	//=============================================================================
	//virtual int SetMaxMergedWnd(
	//	DWORD	dwMax = MAX_MERGED_SESSION_DEFAULT
	//	) = 0;

	//=============================================================================
	//Function:     EnableMergedWnd
	//Description:	Enable or disable merged window mode
	//
	//Parameter:
	//  bMergeWnd	- TRUE to enable merged window mode, else disable
	//Return:
	//		TRUE	Succeed
	//		FALSE	Failed
	//=============================================================================
	//virtual BOOL EnableMergedWnd(
	//	BOOL	bMergeWnd = TRUE
	//	) = 0;
		
	//=============================================================================
	//Function:     CreateSessionWnd
	//Description:	Only the function for create session window 
	//				by the sSessionWndInfo para.
	//				
	//
	//Parameter:
	//Return:
	//		true	Succeed
	//		false	Failed
	//=============================================================================
	virtual BOOL CreateSessionView(
		PS_UISessionInfo_ psUISInfo_,	//Session wnd information
		BOOL bShow = TRUE				// Show session
		) = 0;

	//=============================================================================
	//Function:     GetMainHWnd
	//Description:	Get the main hwnd of the sessions wnd .
	//
	//Parameter:
	//Return: HWND
	//=============================================================================
	//virtual HWND GetMainHWnd();

	//=============================================================================
	//Function:     SessionTypeToWndType
	//Description:	Engine session type to session window type .
	//
	//Parameter:
	//Return: enum
	//		true	Succeed
	//		false	Failed
	//=============================================================================
	//virtual E_SessionWndType SessionTypeToWndType(E_SessionType eWndType) = 0;

	//=============================================================================
	//Function:     WndTypeToSessionType
	//Description:	Session window type to engine session type .
	//
	//Parameter:
	//Return: enum 
	//=============================================================================
	//virtual E_SessionType WndTypeToSessionType(E_SessionWndType eSessionType) = 0;

	//=============================================================================
	//Function:     ShowInviteWnd
	//Description:	Invite wnd .
	//
	//Parameter:
	//Return: enum 
	//=============================================================================
//	virtual void ShowInviteWnd(HWND hwnd,PS_InviteWndInfo psInviteWndInfo) = 0;

	//=============================================================================
	//Function:     ShowInviteWndEx
	//Description:	Invite wnd .
	//
	//Parameter:
	//Return: return ShowModoal's
	//=============================================================================
//	virtual DWORD ShowInviteWndEx(HWND parentWnd,PS_InviteWndInfoEx psInviteWndInfoEx, CallInviteWndType eType=eWND_TYPE_OTHER) = 0;

	//=============================================================================
	//Function:      QSID
	//Description:	 Get current show session qsid
	//
	//Parameter:
	//Return: enum 
	//=============================================================================
	//virtual QSID GetCurrentShowSessionQSID() = 0;

	//=============================================================================
	//Function:      SendMsg
	//Description:	 Send message
	//
	//Parameter:
	//  qsid	- Sesson id
	//  pszXml	- Message command
	//Return: 
	//  None
	//=============================================================================
    //virtual void SendMsg(QSID qsid, const TCHAR* pszXml) = 0;

	//=============================================================================
	//Function:      UserDefindItemSendMsg
	//Description:	 Send message
	//
	//Parameter:
	//  qsid	- Sesson id
	//  pszXml	- Message command
	//Return: 
	//  None
	//=============================================================================
	//virtual void UserDefindItemSendMsg(BOOL isSendMsg) = 0; 

	//=============================================================================
	//Function:      SetClickTipCreateSession
	//Description:	 Set click tips to create session
	//
	//Parameter:
	//  isCreateSessionByClickTip	- TRUE is create session when clicked, else not
	// 
	//Return: 
	//  None
	//=============================================================================
	//virtual void SetClickTipCreateSession(BOOL isCreateSessionByClickTip) = 0;

	//=============================================================================
	//Function:      TopSession
	//Description:	 Set session as topmost
	//
	//Parameter:
	//  qsid		- Session id
	//  eTopOperate	- Operate
	// 
	//Return: 
	//  Reference the operate
	//=============================================================================
	//virtual DWORD TopSession(QSID qsid, E_TopSession eTopOperate)=0;

};

#endif // __IEIMUI_SESSION_MANAGER_HEADER_2014_04_14_YFGZ__