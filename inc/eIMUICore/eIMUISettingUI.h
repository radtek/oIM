#ifndef EIM_UI_SETTING_UI_H
#define EIM_UI_SETTING_UI_H

#pragma pack(push, 8)
// 置顶
typedef enum tagAlwaysTop
{
	eSYSSETTING_NOT_TOP=0,	// 不置顶
	eSYSSETTING_TOP=1,		// 置顶
}E_AlwaysTop;

// 提示
typedef enum tagRemberAppPrompt
{
	eSYSSETTING_PROMPT_APP=1,		// 提示
	eSYSSETTING_NOT_PROMPT_APP=2,	// 不提示
}E_RemberAppPrompt;

// 自动隐藏
typedef enum tagAutoHide
{
	eSYSSETTING_NOT_AUTOHIDE=0,		// 不自动隐藏
	eSYSSETTING_AUTOHIDE=1,			// 自动隐藏
}E_AutoHide;

// 会话关闭提示
typedef enum tagRemberSessionPrompt
{
	eSYSSETTING_PROMPT_SESSION=1,		// 提示
	eSYSSETTING_NOT_PROMPT_SESSION=2,   // 不提示
}E_RemberSessionPrompt;

// 弹窗提示
typedef enum tagPopupPrompt
{
	eSYSSETTING_NOT_POPUPPROMPT=0,	// 不提示
	eSYSSETTING_POPUPPROMPT=1,		// 提示
}E_PopupPrompt;

// 群消息提示
typedef enum tagPromptGropMsg
{
	eSYSSETTING_PROMPT_GMSG=0,		// 提示
	eSYSSETTING_NOT_PROMPT_GMSG=1,	// 不提示
}E_PromptGropMsg;

// 自动弹窗提示
typedef enum tagAutoPopupMsg
{
	eSYSSETTING_NOT_AUTO_POPUPMSG=0,// 不提示
	eSYSSETTING_AUTO_POPUPMSG=1,    // 提示
}E_AutoPopupMsg;

// 关闭程序类型
typedef enum tagCloseApp
{
	eSYSSETTING_APP_MINI=1,			// 关闭时最小化
	eSYSSETTING_APP_EXIT=2,			// 关闭时退出
}E_CloseApp;

// 关闭会话类型
typedef enum tagCloseSession
{
	eSYSSETTING_CLOSE_CURRENT_SESSION=1,	// 只关闭当前会话
	eSYSSETTING_CLOSE_ALL_SESSION=2,		// 关闭所有会话
}E_CloseSession;

// 自动登录
typedef enum tagAutoLogin
{
	eSYSSETTING_NOT_AUTOLOGIN=0,	// 不自动登录
	eSYSSETTING_AUTOLOGIN=1,		// 自动登录 
}E_AutoLogin;

// 语言
typedef enum tagLanguage
{
	eSYSSETTING_ENGLISH=0,			// 英文
	eSYSSETTING_CHINESE=1,			// 中文
}E_Language;

// 提示音
typedef enum tagOpenAudio
{
	eSYSSETTING_CLOSE_AUDIO=0,		// 关闭提示音
	eSYSSETTING_OPEN_AUDIO=1,		// 打开提示音
}E_OpenAudio;

// 提示音类型
typedef enum tagAudioType
{
	eSYSSETTING_SYS_AUDIO=0,		// 系统提示音
	eSYSSETTING_NEWLINE_AUDIO=1,	// 上线提示音
	eSYSSETTING_MSG_PROMPT=2,		// 消息提示音
}E_AudioType;

// 接收文件目录空间检查
typedef enum tagCurFolderMaxSpace
{
	eSYSSETTING_FOLDER_MAX_SPACE_UNSEL=0,	// 不检查
	eSYSSETTING_FOLDER_MAX_SPACE_SEL=1,		// 检查 
}E_CurFolderMaxSpace;

// 接收文件目录自动清理
typedef enum tagAutoClear
{
	eSYSSETTING_CLEAR_PROMPT=0,		// 清理提示
	eSYSSETTING_AUTO_CLEAR=1,		// 自动清理
}E_AutoClear;

// 接收文件目录自动清理周期
typedef enum tagAutoClearOnWeek
{
	eSYSSETTING_NOT_ONE_WEEK=0,		// 不删除一周前的文件
	eSYSSETTING_ONE_WEEK=1,			// 删除一周前的文件 
}E_AutoClearOnWeek;

// 接收文件目录自动清理日期
typedef enum tagAutoClearByDate
{
	eSYSSETTING_CLEAR_NOT_BYDATA=0,	// 不使用
	eSYSSETTING_CLEAR_BYDATA=1,		// 使用
}E_AutoClearByDate;

typedef enum tagAutoClearDateType
{
	eSYSSETTING_DATA_TYPE_M=0,		// 月
	eSYSSETTING_DATA_TYPE_WEEK=1,	// 周
	eSYSSETTING_DATA_TYPE_D=2,		// 天
}E_AutoClearDateType;

//自动状态转换
typedef enum tagResponseAutoChangeState
{
	eSYSSETTING_RESPONSE_STATE_NO_CHANGE=0,	// 不切换 
	eSYSSETTING_RESPONSE_STATE_CHANGE=1,	// 切换
}E_ResponseAutoChangeState;


//有鼠标键盘动作时取消离开/离线状态
typedef enum tagResponseCancelLeaveWithMove
{
	eSYSSETTING_RESPONSE_NO_LEVAVE_MOVE=0,	// 取消离开
	eSYSSETTING_RESPONSE_LEVAVE_MOVE=1,		// 离线状态
}E_ResponseAction;

//X分钟无鼠标动作时切换到...
typedef enum tagResponseState
{
	eSYSSETTING_RESPONSE_LEVAVE=0,			// 离开
	eSYSSETTING_RESPONSE_OFFLINE=1,			// 离线
}E_ResponseState;

//自定义的提示语0：系统定义 1：自定义
typedef enum tagResponseCustom
{
	eSYSSETTING_RESPONSE_SYS=0,				// 系统定义
	eSYSSETTING_RESPONSE_CUSTOM=1,			// 自定义
}E_ResponseCustom;

//离开时自动回复
typedef enum tagResponseEnable
{
	eSYSSETTING_RESPONSE_DISABLE=0,			// 禁用
	eSYSSETTING_RESPONSE_ENABLE=1,			// 启用
}E_ResponseEnable;

// 代理 
typedef enum tagProxy
{
	eSYSSETTING_PROXY_NONE=0,				// 不使用代理
	eSYSSETTING_PROXY_SOCKET5=1,			// Socket5 代理
	eSYSSETTING_PROXY_HTTP=2,				// Http 代理
}E_Proxy;

// 建群提示
typedef enum tagPromptCreateGroup
{
	eSYSSETTING_NO_CREATE_G=0,				// 不提示
	eSYSSETTING_CREATE_G=1,					// 提示
}E_CreateGroupPrompt;

typedef enum tagHotkeyStatus
{
	eHOTKEY_CONFLICT = -1,					// Conflicted
	eHOTKEY_DISABLE  = 0,					// Disabled
	eHOTKEY_ENABLE   = 1,					// Enabled
				
	eHOTKEY_DEFAULT  = 0,					// Default hotkey 
	eHOTKEY_CUSTOM   = 1,					// Custom hotkey
}E_HotkeyStatus, *PE_HotkeyStatus;

//注册热键失败提示
typedef enum tagRegHotKeyPrompt
{
	ePERSETTING_HOTKEY_REG_PROMPT_DISABLE=0,// 禁止提示
	ePERSETTING_HOTKEY_REG_PROMPT_ENABLE=1,	// 提示
}E_RegHotKeyPrompt;

//导出数据，0：导出数据库 1：导出到RTF
typedef enum tagExportMsgMode
{
	eCONFIG_BASIC_EXPORT_DB=0,				// 导出数据库
	eCONFIG_BASIC_EXPORT_MSG=1,				// 导出到RTF
}E_ExportMsgMode;

//回执回复方式，0：需点击 1：无需点击
typedef enum tagReceiptMode
{
	eCONFIG_BASIC_RECEIPT_CLICK=0,			// 需点击
	eCONFIG_BASIC_RECEIPT_NON_CLICK=1,		// 无需点击
}E_ReceiptMode;

//回执图标类型，0：普通 1：钉图标
typedef enum tagReceiptIcon
{
	eCONFIG_BASIC_RECEIPT_ICON_DEF=0,		// 普通
	eCONFIG_BASIC_RECEIPT_ICON_PIN=1,		// 钉图标
}E_ReceiptIcon;

//默认功能键Alt
#define DEFAULT_FUN_KEY 1

//提取信息默认虚拟键 X
#define DEFEXTRACT_VK   88

//截屏默认虚拟键 Q
#define DEFSCREEN_VK	81



#pragma pack(pop)

#endif