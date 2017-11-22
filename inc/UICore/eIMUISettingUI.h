#ifndef EIM_UI_SETTING_UI_H
#define EIM_UI_SETTING_UI_H

#pragma pack(push, 8)
// �ö�
typedef enum tagAlwaysTop
{
	eSYSSETTING_NOT_TOP=0,	// ���ö�
	eSYSSETTING_TOP=1,		// �ö�
}E_AlwaysTop;

// ��ʾ
typedef enum tagRemberAppPrompt
{
	eSYSSETTING_PROMPT_APP=1,		// ��ʾ
	eSYSSETTING_NOT_PROMPT_APP=2,	// ����ʾ
}E_RemberAppPrompt;

// �Զ�����
typedef enum tagAutoHide
{
	eSYSSETTING_NOT_AUTOHIDE=0,		// ���Զ�����
	eSYSSETTING_AUTOHIDE=1,			// �Զ�����
}E_AutoHide;

// �Ự�ر���ʾ
typedef enum tagRemberSessionPrompt
{
	eSYSSETTING_PROMPT_SESSION=1,		// ��ʾ
	eSYSSETTING_NOT_PROMPT_SESSION=2,   // ����ʾ
}E_RemberSessionPrompt;

// ������ʾ
typedef enum tagPopupPrompt
{
	eSYSSETTING_NOT_POPUPPROMPT=0,	// ����ʾ
	eSYSSETTING_POPUPPROMPT=1,		// ��ʾ
}E_PopupPrompt;

// Ⱥ��Ϣ��ʾ
typedef enum tagPromptGropMsg
{
	eSYSSETTING_PROMPT_GMSG=0,		// ��ʾ
	eSYSSETTING_NOT_PROMPT_GMSG=1,	// ����ʾ
}E_PromptGropMsg;

// �Զ�������ʾ
typedef enum tagAutoPopupMsg
{
	eSYSSETTING_NOT_AUTO_POPUPMSG=0,// ����ʾ
	eSYSSETTING_AUTO_POPUPMSG=1,    // ��ʾ
}E_AutoPopupMsg;

// �رճ�������
typedef enum tagCloseApp
{
	eSYSSETTING_APP_MINI=1,			// �ر�ʱ��С��
	eSYSSETTING_APP_EXIT=2,			// �ر�ʱ�˳�
}E_CloseApp;

// �رջỰ����
typedef enum tagCloseSession
{
	eSYSSETTING_CLOSE_CURRENT_SESSION=1,	// ֻ�رյ�ǰ�Ự
	eSYSSETTING_CLOSE_ALL_SESSION=2,		// �ر����лỰ
}E_CloseSession;

// �Զ���¼
typedef enum tagAutoLogin
{
	eSYSSETTING_NOT_AUTOLOGIN=0,	// ���Զ���¼
	eSYSSETTING_AUTOLOGIN=1,		// �Զ���¼ 
}E_AutoLogin;

// ����
typedef enum tagLanguage
{
	eSYSSETTING_ENGLISH=0,			// Ӣ��
	eSYSSETTING_CHINESE=1,			// ����
}E_Language;

// ��ʾ��
typedef enum tagOpenAudio
{
	eSYSSETTING_CLOSE_AUDIO=0,		// �ر���ʾ��
	eSYSSETTING_OPEN_AUDIO=1,		// ����ʾ��
}E_OpenAudio;

// ��ʾ������
typedef enum tagAudioType
{
	eSYSSETTING_SYS_AUDIO=0,		// ϵͳ��ʾ��
	eSYSSETTING_NEWLINE_AUDIO=1,	// ������ʾ��
	eSYSSETTING_MSG_PROMPT=2,		// ��Ϣ��ʾ��
}E_AudioType;

// �����ļ�Ŀ¼�ռ���
typedef enum tagCurFolderMaxSpace
{
	eSYSSETTING_FOLDER_MAX_SPACE_UNSEL=0,	// �����
	eSYSSETTING_FOLDER_MAX_SPACE_SEL=1,		// ��� 
}E_CurFolderMaxSpace;

// �����ļ�Ŀ¼�Զ�����
typedef enum tagAutoClear
{
	eSYSSETTING_CLEAR_PROMPT=0,		// ������ʾ
	eSYSSETTING_AUTO_CLEAR=1,		// �Զ�����
}E_AutoClear;

// �����ļ�Ŀ¼�Զ���������
typedef enum tagAutoClearOnWeek
{
	eSYSSETTING_NOT_ONE_WEEK=0,		// ��ɾ��һ��ǰ���ļ�
	eSYSSETTING_ONE_WEEK=1,			// ɾ��һ��ǰ���ļ� 
}E_AutoClearOnWeek;

// �����ļ�Ŀ¼�Զ���������
typedef enum tagAutoClearByDate
{
	eSYSSETTING_CLEAR_NOT_BYDATA=0,	// ��ʹ��
	eSYSSETTING_CLEAR_BYDATA=1,		// ʹ��
}E_AutoClearByDate;

typedef enum tagAutoClearDateType
{
	eSYSSETTING_DATA_TYPE_M=0,		// ��
	eSYSSETTING_DATA_TYPE_WEEK=1,	// ��
	eSYSSETTING_DATA_TYPE_D=2,		// ��
}E_AutoClearDateType;

//�Զ�״̬ת��
typedef enum tagResponseAutoChangeState
{
	eSYSSETTING_RESPONSE_STATE_NO_CHANGE=0,	// ���л� 
	eSYSSETTING_RESPONSE_STATE_CHANGE=1,	// �л�
}E_ResponseAutoChangeState;


//�������̶���ʱȡ���뿪/����״̬
typedef enum tagResponseCancelLeaveWithMove
{
	eSYSSETTING_RESPONSE_NO_LEVAVE_MOVE=0,	// ȡ���뿪
	eSYSSETTING_RESPONSE_LEVAVE_MOVE=1,		// ����״̬
}E_ResponseAction;

//X��������궯��ʱ�л���...
typedef enum tagResponseState
{
	eSYSSETTING_RESPONSE_LEVAVE=0,			// �뿪
	eSYSSETTING_RESPONSE_OFFLINE=1,			// ����
}E_ResponseState;

//�Զ������ʾ��0��ϵͳ���� 1���Զ���
typedef enum tagResponseCustom
{
	eSYSSETTING_RESPONSE_SYS=0,				// ϵͳ����
	eSYSSETTING_RESPONSE_CUSTOM=1,			// �Զ���
}E_ResponseCustom;

//�뿪ʱ�Զ��ظ�
typedef enum tagResponseEnable
{
	eSYSSETTING_RESPONSE_DISABLE=0,			// ����
	eSYSSETTING_RESPONSE_ENABLE=1,			// ����
}E_ResponseEnable;

// ���� 
typedef enum tagProxy
{
	eSYSSETTING_PROXY_NONE=0,				// ��ʹ�ô���
	eSYSSETTING_PROXY_SOCKET5=1,			// Socket5 ����
	eSYSSETTING_PROXY_HTTP=2,				// Http ����
}E_Proxy;

// ��Ⱥ��ʾ
typedef enum tagPromptCreateGroup
{
	eSYSSETTING_NO_CREATE_G=0,				// ����ʾ
	eSYSSETTING_CREATE_G=1,					// ��ʾ
}E_CreateGroupPrompt;

typedef enum tagHotkeyStatus
{
	eHOTKEY_CONFLICT = -1,					// Conflicted
	eHOTKEY_DISABLE  = 0,					// Disabled
	eHOTKEY_ENABLE   = 1,					// Enabled
				
	eHOTKEY_DEFAULT  = 0,					// Default hotkey 
	eHOTKEY_CUSTOM   = 1,					// Custom hotkey
}E_HotkeyStatus, *PE_HotkeyStatus;

//ע���ȼ�ʧ����ʾ
typedef enum tagRegHotKeyPrompt
{
	ePERSETTING_HOTKEY_REG_PROMPT_DISABLE=0,// ��ֹ��ʾ
	ePERSETTING_HOTKEY_REG_PROMPT_ENABLE=1,	// ��ʾ
}E_RegHotKeyPrompt;

//�������ݣ�0���������ݿ� 1��������RTF
typedef enum tagExportMsgMode
{
	eCONFIG_BASIC_EXPORT_DB=0,				// �������ݿ�
	eCONFIG_BASIC_EXPORT_MSG=1,				// ������RTF
}E_ExportMsgMode;

//��ִ�ظ���ʽ��0������ 1��������
typedef enum tagReceiptMode
{
	eCONFIG_BASIC_RECEIPT_CLICK=0,			// ����
	eCONFIG_BASIC_RECEIPT_NON_CLICK=1,		// ������
}E_ReceiptMode;

//��ִͼ�����ͣ�0����ͨ 1����ͼ��
typedef enum tagReceiptIcon
{
	eCONFIG_BASIC_RECEIPT_ICON_DEF=0,		// ��ͨ
	eCONFIG_BASIC_RECEIPT_ICON_PIN=1,		// ��ͼ��
}E_ReceiptIcon;

//Ĭ�Ϲ��ܼ�Alt
#define DEFAULT_FUN_KEY 1

//��ȡ��ϢĬ������� X
#define DEFEXTRACT_VK   88

//����Ĭ������� Q
#define DEFSCREEN_VK	81



#pragma pack(pop)

#endif