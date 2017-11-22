#ifndef __IEIMUI_MESSAGE_PROMPTMGR_HEADER_2014_09_05_HDP__
#define __IEIMUI_MESSAGE_PROMPTMGR_HEADER_2014_09_05_HDP__

#include "public\plugin.h"
#include "Public\Typedefs.h"

#define INAME_EIMUI_PROMPTMGR					_T("SXIT.EIMUI.PROMPTMGR")	// I_EIMUIPromptMgr interface name

#define WARN_PROMPT_WND_NO_NEED		0
#define WARN_PROMPT_WND_NEED		1

typedef enum tagWndType
{
	eMsgPrompt,			//��Ϣ��ʾ����
	eMsgOffline,		//����Tip��ʾ����
	eWarnPrompt,		//������ʾ���� WndInfo 
	eConcernPrompt,		//��ע��ϵ����ʾ
	eKickPrompt,		//������ʾ
	eOpenFilePrompt,	//���ļ���ʾ
	eSmsReply,			//���Żظ�
	eBroReply,			//���յ��Ĺ㲥
    eRemindCollect		//�ղ�
}E_WndType,*PE_WndType;

typedef enum tagWarnType
{
	eWarnBeyondMaxMemberInGroup, //���棺Ⱥ�鳬������Ա
	eWarnBeyondMaxMemberInMeeting, //���棺���鳬������Ա

}E_WarnType,*PE_WarnType;

typedef enum tagIconType
{
	ePROMPT_LOGOLINE = 1,	// ����
	ePROMPT_LOGOLIVE,		// �뿪
	ePROMPT_LOGINBF,		// ����
	ePROMPT_MSGBOY,			// ��Ϣ��ʾ-��
	ePROMPT_MSGGROUP,		// ��Ϣ��ʾ-Ⱥ
	ePROMPT_OFFLINE,		// ��Ϣ��ʾ-������Ϣ
	ePROMPT_MSGGIRL,		// ��Ϣ��ʾ-Ů
	ePROMPT_SMS,			// ��Ϣ��ʾ-����
	ePROMPT_BRO,			// ��Ϣ��ʾ-�㲥
	ePROMPT_RDP,			// ��Ϣ��ʾ-RDP
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
	DWORD			dwRet;				//����ֵ /*�Ƿ��Ժ���Ҫ��ʾ*/ �鿴WARN_PROMPT_WND_*
	DWORD			dwShowModalWndRet;	//DoModal ����
	HWND			hWndParent;			//������ʾ���ڸ�����
	QEID			qeid;				//��ԱId

	/*eMsgPrompt wnd info*/
	QSID			qsid;				//��Ϣ�ỰId
	int				i32MsgType;			//��Ϣ����group��single
	const			TCHAR* pszTitle;	//��Ϣ�Ự����
	const			TCHAR* pszMsg;		//��Ϣ����
	int				iMsgCount;			//Message type

	/*warn prompt info*/
	E_WarnType		eWarnType;			//�������ͣ���ֵ eWarnBeyondMaxMemberInGroup ��ע��

	UINT64			u64Phone;			// Phone
	ENUM_ICONTYPE   eIconType;			// Icon type
	BOOL			bIs1To10T;          //�Ƿ�һ����Ӧ��ʾ�����Ϊһ����Ӧ�򲻵���δ����Ϣ

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