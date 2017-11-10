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
////�Ự��������
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

#define MSG_SUMMARY_SIZE		(240)		// ժҪ��ϢBuffer����

// ���ֻ�ο�΢�ţ����������ʮ����Ϣ�����ǲ���Ҫ������Ϣ��
// RE ��Ҫ֧�ִ�ǰ�����
#define MSG_FLAG_ERROR		(0x00000001)	// RE��ʾ��������(eErrorType)
#define MSG_FLAG_NOTICE		(0x00000002)	// RE��ʾ֪ͨ����(eNoticeType)
#define MSG_FLAG_NEWMSG		(0x00000004)	// ����Ϣ
#define MSG_FLAG_RECALL		(0x00000008)	// ������Ϣ
#define MSG_FLAG_READ		(0x00000010)	// �Ѷ�
#define MSG_FLAG_AT			(0x00000020)	// �Ѿ�@
typedef struct tagMsgRecord
{
	QMID		mid;		// ��ϢID
	DWORD		dwSendTime;	// ����ʱ��
	QEID		eidSender;	// ������
	DWORD		dwFlag;		// ��ʶ
	UINT8		u8Type;		// Type
	eIMString	szMsgRich;	// ��Ϣ<FIELD_MSGRICH>...</FIELD_MSGRICH>, ���ߴ�����Ϣ
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
			this->dwFlag &=~dwFlag;	// �����ʶ
		else
			this->dwFlag |= dwFlag;	// ���ñ�ʶ

		return dwFlag;
	}
}S_MsgRecord, *PS_MsgRecord;
typedef const struct tagMsgRecord* PS_MsgRecord_;

typedef std::list<S_MsgRecord>				ListSessionRecord;		// Session record
typedef ListSessionRecord::iterator			ListSessionRecordIt;	// Session record's iterator
typedef ListSessionRecord::const_iterator	ListSessionRecordIt_;	// Session record's const iterator	

typedef enum tagUserStatus
{
	eUnknown		= -1,	// Ϊ�˲���ʾ״̬
	eOFFLINE		= 0,	// ����
	ePC_ONLINE		= MAKELONG(IME_EMP_TERM_PC, IME_EMP_STATUS_ONLINE),		// PC ����
	ePC_LEAVE		= MAKELONG(IME_EMP_TERM_PC, IME_EMP_STATUS_LEAVE),		// PC �뿪
	eIOS_ONLINE		= MAKELONG(IME_EMP_TERM_IOS, IME_EMP_STATUS_ONLINE),	// IOS ����
	eIOS_LEAVE		= MAKELONG(IME_EMP_TERM_IOS, IME_EMP_STATUS_LEAVE),		// IOS �뿪
	eANDROID_ONLINE = MAKELONG(IME_EMP_TERM_ANDROID, IME_EMP_STATUS_ONLINE),// Android ����
	eANDROID_LEAVE  = MAKELONG(IME_EMP_TERM_ANDROID, IME_EMP_STATUS_LEAVE),	// Android �뿪

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

//�Ự������Ϣ
#define LAST_SESSION_RECORD_MAX		(50)	// ���������Ϣ������

// UI �Ự��ʶ
#define  SESSION_FLAG_MALE				(0x00000001)	// �����ԣ�û�����þ���Ů��
#define  SESSION_FLAG_SHIELD			(0x00000002)	// ��Ϣ�����
#define  SESSION_FLAG_TOP				(0x00000004)	// �ö���ʶ���漰dwTopTime
#define  SESSION_FLAG_HIDE_ROW1			(0x00000008)	// ���ر�����
# define SESSION_FLAG_HIDE_TIME			(0x00000010)	// ����ʱ��
#define  SESSION_FLAG_HIDE_ROW2			(0x00000020)	// ������Ϣ��
# define SESSION_FLAG_HIDE_FLAG			(0x00000040)	// ����FLAG
#define  SESSION_FLAG_HIDE_LOGO			(0x00000080)	// ����LOGO
#define  SESSION_FLAG_SUMMARY_NAME		(0x00000400)	// ժҪ��ʾ���֣����ⲿ�����Ƿ���ʾ���������֣�
#define  SESSION_FLAG_SEND_FILE			(0x00000800)	// �򿪻ỰʱҪ�����ļ�
#define  SESSION_FLAG_CREATE_GROUP		(0x00001000)	// ��Ҫ����Ⱥ
#define  SESSION_FLAG_MSG_INITED		(0x00002000)	// �Ự��Ϣ�Ѿ���ʼ��
#define  SESSION_FLAG_INITED			(0x00002000)	// �Ự�Ѿ���ʼ��
#define  SESSION_FLAG_MAKE_LOGO			(0x00004000)	// ���ɻỰLOGO
#define  SESSION_FLAG_MEMBER_CHANGED	(0x00008000)	// �Ự��Ա�б��
#define  SESSION_FLAG_SORT_RECORD		(0x00010000)	// �����յ�������Ϣ��Ҫ�������Դ���ʱ����������
#define  SESSION_FLAG_NEW_FILES			(0x00020000)	// ���շ��µ��ļ�����Ҫ����Key
#define  SESSION_FLAG_CLOSE				(0x00040000)	// ����Close��ť

#define	SESSION_FLAG_DISABLE_EVENT		(0x80000000)	// �������е��¼����Ự�����б���Ϣ�������ȿ��Բ�����Ӧ��ʱ״̬�仯��

typedef struct tagUISessionInfo	
{
	QSID				sid;			// �ỰID
	E_SessionType		eType;			// �Ự����, Ĭ�ϵ���
	DWORD				dwFlag;			// �Ự��ʶ���ο�SESSION_FLAG_*
	QEID				eidCreater;		// ������ID
	QEID				eidReceiver;	// ���ĵĽ����ߡ��Է���
	QEID				eidLastMsg;		// ���һ����Ϣ�ķ�����
	E_UserStatus		eUserStatus;	// ���ĶԷ��û�״̬
	DWORD				dwTopTime;		// �ö�ʱ��
	DWORD				dwModifyTime;	// Ⱥ��Ա�����ʱ�䣨0��Ҫ�ټ��أ�
	DWORD				dwLastMsgTime;	// �����Ϣ����ʱ��, ��ʼĬ���ǻỰ����ʱ��
	DWORD				dwNewMsgCount;	// ����Ϣ������������δ�����򿪻Ự���������
	SIZE				sizeScrollPos;	// ������λ��
	CDuiString			szTitle;		// �Ự����[����Ϊ�Է�����]
	CDuiString			szLastSummary;	// ���һ����Ϣ��ժҪ
	CDuiString			szDraft;		// �ݸ�
	VectSessionMember	vectMember;		// [qeidLogin, qeidReveiver,..]: ���ĵڶ����û��ǽ�����
	ListSessionRecord	listRecord;		// ���������Ϣ(���ܻ���ȫ����Ϣ���Զ�����һ��֮ǰ����ʷ) // ΢��ֻÿ�λ�����30�����л��Ự�ͻ�����
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
				if ( nPos != eIMString::npos )	// �����ڴ��е�Read��ʶ
					it->szMsgRich.replace(nPos, _tcslen(pszRead1), pszRead0);
			}
			else if ( dwFlag == MSG_FLAG_AT )
			{
				const TCHAR* pszAt0 =_T("At=\"0\"");
				const TCHAR* pszAt1 =_T("At=\"1\"");

				eIMString::size_type nPos = it->szMsgRich.find(pszAt0);
				if ( nPos != eIMString::npos )	// �����ڴ��е�At��ʶ
					it->szMsgRich.replace(nPos, _tcslen(pszAt0), pszAt1);
			}

			return it->SetFlag(dwFlag, bClear);
		}

		return 0;
	}
}S_UISessionInfo,*PS_UISessionInfo;
typedef const struct tagUISessionInfo* PS_UISessionInfo_;

//���²�������
typedef enum tagOperaType
{
	eUpdGroupAdd,	// Add
	eUpdGroupDel,	// Delete
	eUpdGroupUpd,	// Update
}E_OperaType,*PE_OperaType;

//�ⲿ���»Ự����
typedef enum tagUpdateType
{
	eUpdateGroupName	= 1,	// Update group name
	eUpdateGroupMember	= 2,	// Update group member
	eFreshGroup			= 4,	// Update group
}E_UpdateSessWndType;

//��Ա����
typedef struct tagUpdateMember
{
	E_OperaType eOperaType;		// Operate type
	QEID		eid;			// Employee id
}S_UpdateMemberInfo,*PS_UpdateMemberInfo;

//����Ա���Ĳ���
typedef std::vector<S_UpdateMemberInfo>				VectUpdateMemInfos;
typedef std::vector<S_UpdateMemberInfo>::iterator	VectUpdateMemInfoIt;

//// ���»Ự������Ϣ
//typedef struct tagUpdateSessionWndData
//{
//	QSID					qsid;			// Employee id
//	E_UpdateSessWndType		eUpdateType;	// Update type
//	CDuiString				strGroupName;	// Group name
//	QEID					qoperid;		// ID 
//	VectUpdateMemInfos		vectMemberIds;	// Session member information
//
//	E_OperaType				eOperaType;		//������������
//	VectSessionMember		vectMembers;	//��������
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