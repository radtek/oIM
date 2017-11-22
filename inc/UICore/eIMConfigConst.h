// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: eIMUI config file const defined 
// 
// Auth: yfgz
// Date: 
// 
// History: 
//    2014/6/26 yfgz Finished first version
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_CONFIG_CONST_HEADER_2014_06_26_YFGZ__
#define __EIM_CONFIG_CONST_HEADER_2014_06_26_YFGZ__

#include <windows.h>
#include <tchar.h>

#define KB  ( 1024 )	// 1KB
#define MB	( KB * KB )	// 1MB

// Config files define
#define PLUGIN_CONFIG_FILE		_T("Plugins.config")		// Plugin config file
#define PROTOCOL_CONFIG_FILE	_T("Protocol.config")		// Protocol config file
#define APP_CONFIG_FILE			_T("App.config")			// Application config file
#define USER_CONFIG_FILE		_T("User.config")			// Application for user config
#define FONT_CONFIG_FILE        _T("Font.config")           // Font config file 
#define UPDATE_CONFIG_FILE		_T("Update.config")			// Updater config file
#define CONTACTS_CONFIG_FILE	_T("Group.config")			// Contact config file

// Head image file name
#define HEAD_IMAGE_FILE			_T("HeadLogo.png")			// Head image file name
#define HEAD_IMAGE_TEMP_FILE	_T("HeadLogoTemp.jpg")		// Head image temp file name
#define DEFAULT_HEAD_FILE_BOY	_T("Common/user_head_boy.png")		// Default boy head logo
#define DEFAULT_HEAD_FILE_GIRL	_T("Common/user_head_girl.png")		// Default girl head logo

//App.Config///////////////////////////////////////////////////////////////////
// EL: Element
#define APP_ROOT_EL                         _T("eIMApp")
#define APP_ROOT_ATTR_VERSION               _T("Version")

// App
#define APP_EL                              _T("App")
#define APP_ATTR_LANGUAGE                   _T("Language")
#define APP_ATTR_COMPANY					_T("Company")	// Company english name
#define APP_ATTR_VERSION                    _T("Version")
#define APP_ATTR_DATABASE_VERSION           _T("Database")
#define APP_ATTR_DOWNLOADURL                _T("DownloadUrl")
#define APP_ATTR_FEEDBACKURL                _T("FeedbackUrl")
#define APP_ATTR_CLEAR_HISTORY_LOG_DAYS		_T("ClearHistoryLogDays")
#define APP_ATTR_CLEAR_DOWNLOAD_CONTS_DAYS	_T("ClearDownloadContsDays")
#define APP_ATTR_CLEAR_BKDB_DAYS			_T("ClearBkDBDays")
# define CLEAR_FILE_DAYS_DEFAULT			(31)
#define APP_ATTR_DISK_SPACE_PROMPT			_T("DiskSpacePrompt")
# define APP_ATTR_DISK_SPACE_PROMPT_DEFAULT	( 1024 )		// 1GB


#define APP_ATTR_UI_LOG_LEVEL				_T("UILogLevel")
#define APP_ATTR_UI_LOG_FLAG				_T("UILogFlag")

// Special Id
#define SPECIALID_EL						_T("SpecialId")
#define SPECIALID_ATTR_FILETRANSFER			_T("Filetransfer")
#define SPECIALID_ATTR_IROBOT				_T("iRobot")
#define SPECIALID_ATTR_ITSUPPORT			_T("ITSupport")
#define SPECIALID_ATTR_SYSTEM_MSG			_T("SystemMsg")
#define SPECIALID_ATTR_SYSTEM_MSG_RTX		_T("SystemMsgRtx")
#define SPECIALID_ATTR_SYSTEM_MSG_MAIL		_T("SystemMsgMail")
#define SPECIALID_ATTR_PRINTER				_T("Printer")

// Optional Funcion
#define FUNC_EL								_T("Function") 
#define FUNC_ENABLE							1
#define FUNC_ATTR_APP_MANAGER				_T("AppManager")		// FunAppManager
#define FUNC_ATTR_1_TO_HSESSION				_T("A2HSession")		// Fun1to100Session
#define FUNC_ATTR_1_TO_HUNDRED				_T("A2Hundred")			//IsSupport_1_to_hundred
#define FUNC_ATTR_COLLECT					_T("Collect")			// FunCollect
#define FUNC_ATTR_CREATE_GROUP_PROMPT		_T("CreateGroupPrompt") // IsCreateGroupPrompt
#define FUNC_ATTR_FILE_TRANSFER				_T("FileTransfer")		// FuncFileTransfer
#define FUNC_ATTR_GREY_HELP					_T("GreyHelp")			// GreyHelp
#define FUNC_ATTR_HIDE2SCREENSHOT			_T("Hide2Screenshot")
#define FUNC_ATTR_INVITE_LIST_ADD_CODE		_T("InviteListAddCode")	// InviteListAddCode 
#define FUNC_ATTR_INVITE_SHOW_DEPT			_T("InviteShowDept")
#define FUNC_ATTR_MSG_ADDDING				_T("MsgAddDing")		// Msg_MsgAddDing
#define FUNC_ATTR_MSG_ATAIL_ADD_ID			_T("MsgAtailAddId")		// Msg_MsgAtailAddId
#define FUNC_ATTR_ID_BEFORE					_T("MsgIdBefore")		// Msg_MsgidBefore
#define FUNC_ATTR_MSG_IROBOT				_T("MsgiRobot")			// MSG_iRobot
#define FUNC_ATTR_RECALL					_T("MsgRecall")			// Msg_MsgRecall
#define FUNC_ATTR_RDP						_T("Rdp")	// Remote desktop
#define RDP_DEFAULT				( 0 )		// Disable of default
#define FUNC_ATTR_RECEIPT_MODE				_T("ReceiptMode")		// ReceiptMode
#define FUNC_ATTR_RECEIPT_ICON				_T("ReceiptIcon")		// ReceiptIcon
#define FUNC_ATTR_RECV_FILE_ACK				_T("RecvFileAck")		// Fun_Receive_File_Ack
#define FUNC_ATTR_REPAIR_MSG				_T("RepairMsg")			// FunRepairMsg
#define FUNC_ATTR_SAVE_ADDWANXIN			_T("SaveFileNameAddWanxin")	// SaveFileNameAddWanxin
#define FUNC_ATTR_SEARCH_LIST_ADDD_EPT		_T("SearchListAddDept")	// SearchListAddDept 
#define FUNC_ATTR_SEND_SMS					_T("SendSMS")			// FunSendMSM
#define FUNC_ATTR_SMS_HAVE_NAME				_T("SmsIsHaveName")
#define FUNC_ATTR_SHOW_SIGN					_T("ShowSign")
#define FUNC_RESET_RESERVER					_T("ShowReset")			//Show reset server configration
#define FUNC_ATTR_SOUND2TEXT				_T("Sound2Text")		// FunSendMSM
#define FUNC_ATTR_RUN_EXPORT_TOOL			_T("RunExportTool")	
#define FUNC_ATTR_EXPORT_MENU_ENABLE		_T("ExportMenuIsEnable")
#define FUNC_ATTR_SHOW_EXPORT_OLDIM_MENU	_T("ShowExportOldImMenu")	// IsShowExportOldImMenu
#define FUNC_ATTR_EXPORT_MSG_MODE			_T("ExportMsgMode")
#define FUNC_ATTR_TOP_SESSION_MAX			_T("TopSessionMax")
# define TOP_SESSION_MAX_DEFAULT			(10)
#define FUNC_ATTR_FAST_LOAD_CONTACTS		_T("FastLoadContacts")
#define FUNC_ATTR_VOICE2TEXT_URL			_T("VoiceToTextUrl")
# define VOICE2TEXT_URL_DEFAULT				_T("http://ctx.wanda.cn:8090/USCService/usc?")
#define FUNC_CLEAR_INVAILD_DEPT_EMP         _T("ClearInvaildDeptEmp")
#define FUNC_CLOUD_DISK_EL					_T("CloudDisk")			// FuncCloudDisk
#define FUNC_CLOUD_DISK_ATTR_ENABLE			_T("Enable")			// Enable
#define FUNC_ATTR_RTXTOOL_FLAG				_T("RtxToolFlag")		// Open RtxTool
#define FUNC_ATTR_NEW_FILE_CTRL				_T("NewFileCtrl")		// 是否使用新文件控件，默认使用旧的ImageCtrl
#define FUNC_ATTR_CLEAR_LOGO_TIME			_T("ClearLogoTime")		// 清除所有头像标识（需要重新下载）
#define FUNC_ATTR_CHECK_SEND_FILE			_T("CheckSendFile")		// 检查发送文件占用
# define CHECK_SEND_FILE_DEFAULT			(1)						// 默认检查

#define FUNC_ATTR_BROADCAST					_T("Broadcast")			// 广播
# define BROADCAST_DEFAULT					(0)						// 禁止广播功能
# define BROADCAST_ALL						(1)						// 允许全员广播
# define BROADCAST_DEPT						(2)						// 允许部门广播

#define FUNC_ATTR_MAINWND_POS				_T("MainWnd.Pos")		// 主配置标识是否记住位置，用户配置保存窗口位置
# define MAINWND_POS_DEFAULT				(1)						// 默认记住
#define FUNC_SENDBROCASTTIP                      _T("SendBroCastTip")    // 发送广播消息后是否有提示
// 排序
#define FUNC_ATTR_INVITE_SORT_FLAG			_T("InviteSortFlag")		// 邀请排序标识，参考CONTACT_FLAG_SORT_*
#define FUNC_ATTR_SESSION_MEMBER_SORT_FLAG	_T("SessionMemberSortFlag")	// 会话成员排序标识，参考CONTACT_FLAG_SORT_*
#define FUNC_ATTR_CONTACT_SORT_FLAG			_T("ContactSortFlag")		// 通讯录排序标识，参考CONTACT_FLAG_SORT_*
#define FUNC_ATTR_CONTACT_SEARCH_FLAG		_T("ContactSearchFlag")		// 通讯录搜索标识
#define FUNC_ATTR_DISABLE_SMALL_FACE		_T("DisableSmallFace")		// 禁止使用小头像
#define FUNC_DEPT_TOPLEVEL_PID				_T("DeptTopLevelPid")
# define DEPT_TOPLEVEL_PID_DEFAULT			(0)

#define FUNC_ATTR_SHOW_ONLINE				_T("ShowOnlineList")		// 显示在线人员列表（部门右键菜单）
#define FUNC_ATTR_SEARCH_CONTACT			_T("SearchContacts")		// 显示在线人员列表（部门右键菜单）
#define FUNC_TOLOWER_ACCOUNT				_T("ToLowerAccount")		// 登录账号转小写

#define FUNC_PORTRAIT_MODE					_T("PortraitMode")  // Portrait Mode
# define PORTRAIT_MODE_DEFAULT              ( 0 )               // Old mode
# define PORTRAIT_MODE_NEW                  ( 1 )               // New mode as Portrait Mode

#define FUNC_DPIAWARE						_T("DpiAware")		// 是否由程序适配HPI，配置【必须】与 eIMMain.exe.manifest 中的一致，才会有效
# define DPIAWARE_DEFAULT					( 0 )				// 默认程序不适配HPI，由操作系统自动处理（也就会出现模糊问题）
# define DPIAWARE_X2						( 2 )	// 2 * 96DPI as MAX dpi support
# define DPIAWARE_X3						( 3 )	// 3 * 96DPI as MAX dpi support 
# define DPIAWARE_X4						( 4 )	// 4 * 96DPI as MAX dpi support
// Translate
//			<Translate Enable="1" method="post" ServerUrl="http://ctx-test-meeting.wanda.cn:8080/translate" />
#define FUNC_TRANSLATE_EL					_T("Translate")			
#define FUNC_TRANSLATE_ATTR_ENABLE			_T("Enable")		// Enable
#define FUNC_TRANSLATE_ATTR_LIMIT			_T("Limit")			// 
# define FUNC_TRANSLATE_ATTR_LIMIT_DEFAULT	(1024)				// default 
#define FUNC_TRANSLATE_ATTR_SERVERURL		_T("ServerUrl")		// 服务器地址：http://ctx-test-meeting.wanda.cn:8080/translate

// Fonts(Maybe add others font at here...)
#define FONTS_EL                            _T("Fonts")
#define FONT_RECV_EL						_T("FontRecv")
#define FONT_SEND_EL						_T("FontSend")
#define FONT_FONT1TOH_EL					_T("Font1ToH")
#define FONT_FONT1TOT_EL					_T("Font1ToT")
#define FONT_ERROR							_T("FontError")
#define FONT_MSGAUTO						_T("FontMsgAuto")
#define FONT_SEND_MSG_EL					_T("FontSendMsg")
#define FONT_ATTR_SIZE						_T("Size")
#define FONT_ATTR_COLOR						_T("Color")
#define FONT_ATTR_ID						_T("Id")
#define FONT_ATTR_MODE						_T("Mode")
#define FONT_ATTR_VALUE_FMT					_T("<MsgFont Size=%d Color=0x%06X Id=%d Mode=%d />")
#define FONT_RECEIPT_PIN_RCV				_T("FontReceiptPinRcv")

// Engine
#define ENGINE_EL                           _T("Engine")
#define ENGINE_ATTR_LOGFILE                 _T("LogFile")
#define ENGINE_ATTR_LOGLEVEL                _T("LogLevel")
#define ENGINE_ATTR_LOGFLAG                 _T("LogFlag")
#define ENGINE_ATTR_RECONNECTTIMES          _T("ReconnectTimes")
#define ENGINE_ATTR_DISABLE_DEPTUSER_SHOW	_T("DisableDeptUserShow")	// 1: Disable

#define ENGINE_ATTR_DOWNLOAD_EMPS_DELAY		_T("DownloadEmpsDelay")	// Download employee's detail delay times, unit as minute, default: 5min
#define ENGINE_ATTR_DOWNLOAD_CONTACTDB		_T("DownloadContactsDb")// Download contacts.db if current user not init contacts.db
#define ENGINE_ATTR_DOWNLOAD_CONTACT_FILE	_T("DownloadContactsFile")// Download contacts's file
#define ENGINE_ATTR_FILE_TRANSFER_THREAD_NUM	_T("FileTransferThreadNum")// File transfer thread count, [1,12]
#define ENGINE_ATTR_SETUP_TIMESTAMP				_T("SetupTimestamp")	// Setup timestamp

#define ENGINE_ATTR_PINYIN					_T("Pinyin")	
#define ENGINE_ATTR_DOWNLOAD_CONTACTDB_DISABLE_PROXY	_T("DownloadContactsDbDisbaleProxy")// Download contacts.db no proxy

// Setting/////////////////////////////////////////////////////////////////////
// My
#define SETTING_EL                          _T("Setting")              
#define SETTING_MY_EL						_T("My")				// Path "eIMRoot\\Setting\\My"
#define SETTING_MY_ATTR_EDITABLE			_T("Editable")
# define EDITABLE_NAME						( 0x00000001 )
# define EDITABLE_AGE						( 0x00000002 )
# define EDITABLE_EMAIL						( 0x00000004 )
# define EDITABLE_CODE						( 0x00000008 )
# define EDITABLE_SEX						( 0x00000010 )
# define EDITABLE_TEL						( 0x00000020 )
# define EDITABLE_PHONE						( 0x00000040 )
# define EDITABLE_HOMETEL					( 0x00000080 )
# define EDITABLE_EMGRTEL					( 0x00000100 )
# define EDITABLE_SIGNATURE					( 0x00000200 )
# define EDITABLE_JOB						( 0x00000400 )
# define EDITABLE_DEPT						( 0x00000800 )
# define EDITABLE_LOGO						( 0x00001000 )
# define EDITABLE_FAX						( 0x00002000 )
# define EDITABLE_ADDRESS					( 0x00004000 )
# define EDITABLE_ZIPCODE					( 0x00008000 )
# define EDITABLE_EDITABLE					( 0x00010000 )

#define SETTING_MY_ATTR_COUNTRY				_T("Country")
#define SETTING_MY_ATTR_PROVINCE			_T("Province")
#define SETTING_MY_ATTR_CITY				_T("City")
#define SETTING_MY_ATTR_BLOODTYPE			_T("BloodType")
#define SETTING_MY_ATTR_BIRTHDAYY			_T("BirthdayY")
#define SETTING_MY_ATTR_BIRTHDAYM			_T("BirthdayM")
#define SETTING_MY_ATTR_BIRTHDAYD			_T("BirthdayD")
#define SETTING_MY_ATTR_HOMEPAGE			_T("Homepage")
#define SETTING_MY_ATTR_ABOUTME				_T("AboutMe")

#define SETTING_MODIDY_PSW_TYPE				_T("Psw_Modify_Type")   //0:by URL 1:UI,2:By Dialog of menu
# define MODIFY_PSW_TYPE_URL				(0)	// By Url
# define MODIFY_PSW_TYPE_SETTING			(1)	// By Setting UI
# define MODIFY_PSW_TYPE_MENU				(2)	// By Dialog of menu
#define SETTING_MODIDY_PSW_URL				_T("Psw_Modify_Url")	//0:url
#define SETTING_PAGE_CLEAR_COUNT			_T("Rich_Clear_Msg_Count")
#define SETTING_PAGE_ACTION_CLEAR_COUNT		_T("Rich_Action_Clear_Msg_Count")

// Basic ///////////////////////////////////////////////////////////
#define SETTING_BASIC_EL						_T("Basic")					// Path "eIMRoot\\Setting\\Basic"
#define SETTING_BASIC_ATTR_ALWAYS_TOP			_T("AlwaysTop")
#define SETTING_BASIC_ATTR_ISREMEPROPTAPP 		_T("IsRemePromptApp")
#define SETTING_BASIC_ATTR_AUTO_HIDE			_T("AutoHide")
#define SETTING_BASIC_ATTR_ISREMESESSPROMPT 	_T("IsRemPromptSess")		//
#define SETTING_BASIC_ATTR_MAX_SESSION			_T("MaxRecentSession")		
# define SETTING_BASIC_MAX_SESSION_DEFAULT		(50)
#define SETTING_BASIC_ATTR_POPUP_PROMPT			_T("PopupPrompt")
#define SETTING_BASIC_ATTR_NOT_PROMPT_GMSG		_T("NotPromptGroupMsg")
#define SETTING_BASIC_ATTR_AUTO_POPUP_MSG		_T("AutoPopupMsg")
#define SETTING_BASIC_ATTR_CLOSE_PROMPT			_T("CloseAppPrompt")		// 0: Prompt when close, default; 1: To hide; 2: exit app
#define SETTING_BASIC_ATTR_CLOSE_SESS_PROMPT 	_T("CloseSessPrompt")
# define CLOSE_SESSION_CURRENT					( 1 )						// Close current session(default)
# define CLOSE_SESSION_ALL						( 2 )						// Close all session
#define SETTING_BASIC_ATTR_AUTO_LOGIN			_T("AutoLogin")
#define SETTING_BASIC_ATTR_LANGUAGE				_T("Language")

#define SETTING_BASIC_ATTR_OPEN_SOUND			_T("SoundOpen")
#define SETTING_BASIC_ATTR_SOUND_TYPE			_T("SoundType")

#define SETTING_BASIC_ATTR_DEFAULT_MYDEPT		_T("DefaultMyDept2FrequentDept")
#define SETTING_BASIC_ATTR_SIGNATUREHORNAMESHOW _T("SignatureHorNameShow")
#define SETTING_BASIC_ATTR_DELETESESSION        _T("DeleteSessionMsg")
#define SETTING_BASIC_ATTR_ADDNETSENSE          _T("File_HeadreAddNetsense")
#define SETTING_BASIC_ATTR_ISCHANGINGIMAGE      _T("ChangingImage")
#define SETTING_BASIC_ATTR_RANKID               _T("RankId")
#define RANKID_LEVEL                            _T("RankIdLevel")
#define SETTING_BASIC_ATTR_RGROUPADMINMSGTIP    _T("RgroupAdminMsgTip")
#define SETTING_BASIC_ATTR_IMAGEMORENUM         _T("ImageMoreNum")
#define SETTING_BASIC_ATTR_UPDATETIPFTTPS       _T("UpdateTipHttps")

#define SETTING_BASIC_ATTR_SOUND_FILE_0			_T("SoundFile_0")
#define SETTING_BASIC_ATTR_SOUND_FILE_1			_T("SoundFile_1")
#define SETTING_BASIC_ATTR_SOUND_FILE_2			_T("SoundFile_2")

#define SETTING_FILE_ATTR_RECVSIZE				_T("File_AutoRecv_MaxSize")   // Unit is MB
# define FILE_RECV_SIZE_DEFAULT					( 5 )	// 5MB
#define SETTING_FILE_ATTR_PATH					_T("File_Path")
#define SETTING_FILE_ATTR_DELCURSPACE			_T("File_DelCurSpace")
#define SETTING_FILE_ATTR_CURSPACESIZE			_T("File_CurSpaceMaxSize")
#define SETTING_FILE_ATTR_AUTODEL				_T("File_AutoDel")
#define SETTING_FILE_ATTR_AUTODEL_ONEWEEK		_T("File_AutoDelOneWeek")
#define SETTING_FILE_ATTR_AUTODEL_ONEWEEK_SIZE	_T("File_AutoDelOnWeekSize")
#define SETTING_FILE_ATTR_AUTODEL_BYDATE		_T("File_AutoDel_ByDate")
#define SETTING_FILE_ATTR__AUTODEL_DATE_NUM		_T("File_AutoDel_DateNum")
#define SETTING_FILE_ATTR__AUTODEL_DATETYPE		_T("File_AutoDel_DateType")

#define SETTING_BASIC_ATTR_AUDIO_PROMPT			_T("AudioPrompt")
#define SETTING_BASIC_ATTR_AUTO_RUN				_T("AutoRun")
#define SETTING_BASIC_ATTR_AUTO_PROMPT			_T("AutoPrompt")

#define SETTING_BASIC_ATTR_PROXY_TYPE			_T("ProxyType")
#define SETTING_BASIC_ATTR_PROXY_ADDR			_T("ProxyServerAddr")
#define SETTING_BASIC_ATTR_PROXY_PORT			_T("ProxyServerPort")
#define SETTING_BASIC_ATTR_PROXY_USERNAME		_T("ProxyUserName")
#define SETTING_BASIC_ATTR_PROXY_PSW			_T("ProxyPassword")

#define SETTING_BASIC_ATTR_NOTSHOWEMPTYDEPT     _T("OrgTreeNotShowEmptyDept")
// File Transfer
#define SETTING_TRANSFER_EL                 _T("Transfer")
#define SETTING_TRANSFER_ATTR_AUTORECEIVE   _T("AutoReceive")
#define SETTING_TRANSFER_ATTR_LOCATION      _T("Location")
#define SETTING_TRANSFER_ATTR_EXCEEDPROMPT  _T("ExceedPrompt")
#define SETTING_TRANSFER_ATTR_EXCEEDVALUE   _T("ExceedValue")

// Servers ////////////////////////////////////////////////////////////////
#define SETTING_SIMPLEST					_T("Simplest")			// Hide not used item
#define SETTING_USE_SERVER					_T("UseServer")			// Use server index[0,1]
#define SETTING_SERVER_EL				    _T("Server")
#define SETTING_ATTR_SET_MODE			    _T("SetMode")			// 0: disable, 1: only IP(bit0), 2: Only domain(bit1); 3: IP and domain
# define SET_MODE_IP                        ( 1 )	
# define SET_MODE_DOMAIN                    ( 2 )
# define SET_MODE_IP_DOMAIN                 ( SET_MODE_IP | SET_MODE_DOMAIN )	

#define SETTING_ATTR_CONNECT_TYPE		    _T("ConnectType")		// None, IP or Domain
# define CONNECT_TYPE_IP                    ( 0 )
# define CONNECT_TYPE_DOMAIN                ( 1 )

#define SETTING_ATTR_SWITCH_FILE_SERVER		_T("SwitchFileServer")	// Switch file server when login server changed
#define SETTING_ATTR_ENABLE_SSO				_T("EnableSSO")			// SSO
#define SETTING_ATTRIB_FORCE_RELOAD2EMPS	_T("ForceReload2Emps")	// Force reload emps when detail download ed(Only for LongFor now)

// IM,P2P
#define SETTING_IM_EL					    _T("IM")
#define SETTING_P2P_EL						_T("P2P")
#define SETTING_RDP_EL						_T("Rdp")
#define SETTING_ATTR_IP					    _T("IP")
#define SETTING_ATTR_DOMAIN				    _T("Domain")
#define SETTING_ATTR_PORT				    _T("Port")
#define SETTING_ATTR_PORTA				    _T("PortA")
#define SETTING_ATTR_PORTB				    _T("PortB")
#define SETTING_ATTR_TCP_IP				    _T("TcpIP")
#define SETTING_ATTR_REG_PORT				_T("RegPort")
#define SETTING_ATTR_TRG_PORT				_T("TrgPort")
#define SETTING_ATTR_UDP_IP				    _T("UdpIP")
#define SETTING_ATTR_UDP_PORT				_T("UdpPort")

// File, Image 
#define SETTING_FILE_EL						_T("File")
#define SETTING_IMAGE_EL					_T("Image")
#define SETTING_HEAD_EL						_T("Head")
#define SETTING_ATTR_UPLOAD_URL		        _T("UploadUrl")
#define SETTING_ATTR_UPLOAD_IP			    _T("UploadIp")
#define SETTING_ATTR_DOWNLOAD_URL		    _T("DownloadUrl")
#define SETTING_ATTR_DOWNLOAD_IP		    _T("DownloadIp")

//bt file, image
#define SETTING_FILE_EL_BT					_T("File_BT")
#define SETTING_IMAGE_EL_BT					_T("Image_BT")
#define SETTING_ATTR_UPLOAD_URL_TOKEN		_T("UploadUrl_Token")
#define SETTING_ATTR_UPLOAD_IP_TOKEN		_T("UploadIp_Token")
#define SETTING_ATTR_UPLOAD_URL_BT		    _T("UploadUrl_BT")
#define SETTING_ATTR_UPLOAD_IP_BT			_T("UploadIp_BT")
#define SETTING_ATTR_DOWNLOAD_URL_BT		_T("DownloadUrl_BT")
#define SETTING_ATTR_DOWNLOAD_IP_BT		    _T("DownloadIp_BT")

// Hotkeys /////////////////////////////////////////////////////////
#define SETTING_HOTKEY_EL					_T("Hotkey")
#define SETTING_HOTKEY_ATTR_REG_PROMPT		_T("RegHotKeyPrompt")

#define SETTING_HOTKEY_EXTRACT_MSG_EL		_T("ExtractMsg")
#define SETTING_HOTKEY_SCREENSHOT_EL		_T("ScreenShot")
#define SETTING_HOTKEY_ACTIVE_SEARCH_EL		_T("ActiveSearch")

#define SETTING_HOTKEY_ATTR_TYPE			_T("Type")
#define SETTING_HOTKEY_ATTR_DEF_MODIFI		_T("Modifiers")
#define SETTING_HOTKEY_ATTR_DEF_VKCODE		_T("VKCode")
#define SETTING_HOTKEY_ATTR_CUSTOM_MODIFI	_T("CustomModifiers")
#define SETTING_HOTKEY_ATTR_CUSTOM_VKCODE	_T("CustomVKCode")
#define SETTING_HOTKEY_ATTR_HOTKEY			_T("Hotkey")	// Combined by Ctrl,Alt,Shift and [0-9A-Z]; SendMsg only[Ctrl+Enter,Enter]

#define SETTING_HOTKEY_SENDMSG_EL			_T("SendMsg")
# define HOTKEY_CONTROL_ENTER				( 1 )	// Control+Enter
# define HOTKEY_ENTER						( 2 )	// Enter
#define SETTING_HOTKEY_ATTR_STATE			_T("State")		// -1: is conflict; 0: is disable; 1: default is ok
# define HOTKEY_CONFLICT					( -1 )	// conflict
# define HOTKEY_DISABLE						( 0 )	// disable
# define HOTKEY_OK							( 1 )	// ok

// RESPONSE ///////////////////////////////////////////////////////
#define SETTING_RESPONSE_EL					_T("Response")
#define SETTING_RESP_ATTR_AUTO_LEAVE		_T("AutoLeave")	// 0: to disable, else is minute value(>0)
#define SETTING_RESP_ATTR_AUTO_ENABLE		_T("Enable")
#define SETTING_RESP_ATTR_CHOSEN			_T("Chosen")
#define SETTING_RESP_ATTR_STATE				_T("State")
#define SETTING_RESP_ATTR_LEAVE_WITH_MOVE	_T("CancelLeaveWithMove")
#define SETTING_RESP_ATTR_AUTO_CHANGE_STATE	_T("AutoChangeState")
#define SETTING_REST_ATTR_FORCE_RESET		_T("ForceReset")
# define SETTING_REST_FORCE_RESET_CHECK		1
# define SETTING_REST_FORCE_RESET_UNCHECK	-1
# define SETTING_REST_FORCE_RESET_NONE		0


#define SETTING_RESP_ITEM_EL				_T("Item")
#define SETTING_RESP_ITEM_ATTR_MSG			_T("Msg")		// Msg content
#define SETTING_RESP_ITEM_ATTR_CUSTOM		_T("Custom")	// 0 is system defined, default; 1 is custom defined

// File Transfer ////////////////////////////////////////////////////
#define SETTING_FILE_TRANSFER_EL			_T("FileTransfer")
#define SETTING_TRANS_ATTR_AUTO_RECEIVE		_T("AutoReceive")		// Auto receive group session file
#define SETTING_TRANS_ATTR_RCV_LOCATION		_T("Location")			// Empty is default location
#define SETTING_TRANS_ATTR_EXCEED_PROMPT	_T("ExceedPrompt")		// 1: enable prompt
#define SETTING_TRANS_ATTR_EXCEED_VALUE		_T("ExceedValue")	

//Font.config/////////////////////////////////////////////////////////////////////////
#define FONT_DEFAULT_FAMILY                 _T("宋体")   // Default font

#define FONT_ROOT_EL                        _T("eIMFont")
#define FONT_VERSION_ATTR                   _T("Version")
#define FONT_DEFAULT_ATTR                   _T("Default")

#define FONT_ITEM_EL                        _T("Item")
#define FONT_ITEM_ID_ATTR                   _T("Id")
#define FONT_ITEM_NAME_ATTR                 _T("Name")

// HELP MACRO for =============================================================
// 说明：当路径所在位置只有一项时，使用　"Path" 与 "Path[0]" 是一样的。
// _FMT: 是格式字符串，%d,
// _(N):   的N只能是常量数值，不能给变量。
// App.config
#define PATH_APP						APP_ROOT_EL  _T("\\") APP_EL				// eIMApp\App
#define PATH_SPECIALID					APP_ROOT_EL  _T("\\") SPECIALID_EL			// eIMApp\SpecialId
#define PATH_FUNC						APP_ROOT_EL  _T("\\") FUNC_EL				// eIMApp\Function
#define PATH_FUNC_ITEM_(N)				APP_ROOT_EL  _T("\\") FUNC_EL _T("\\")##N	// eIMApp\Function
#define PATH_ENGINE						APP_ROOT_EL	 _T("\\") ENGINE_EL				// eIMApp\Engine
#define PATH_SETTING					APP_ROOT_EL  _T("\\") SETTING_EL			// eIMApp\Setting
#define PATH_FONT_ITEM_(N)              APP_ROOT_EL	 _T("\\") FONTS_EL _T("\\")##N	// eIMApp\App\Fonts\Font[d]
#define PATH_SETTING_MY					PATH_SETTING _T("\\") SETTING_MY_EL			// eIMApp\Setting\My
#define PATH_SETTING_BASIC				PATH_SETTING _T("\\") SETTING_BASIC_EL		// eIMApp\Setting\Basic
#define PATH_SETTING_TRANSFER			PATH_SETTING _T("\\") SETTING_TRANSFER_EL	// eIMApp\Setting\Transfer
#define PATH_SETTING_RANKIDLEVEL		PATH_SETTING _T("\\") RANKID_LEVEL 

#define PATH_SETTING_HOTKEY				PATH_SETTING _T("\\") SETTING_HOTKEY_EL		// eIMApp\Setting\Hotkey
#define PATH_SETTING_HOTKEY_ITEM_(N)	PATH_SETTING_HOTKEY _T("\\")##N				// eIMApp\Setting\Hotkey\SendMsg

#define PATH_SETTING_RESP				PATH_SETTING _T("\\") SETTING_RESPONSE_EL									// eIMApp\Setting\Response
#define PATH_SETTING_RESP_ITEM_FMT		PATH_SETTING_RESP _T("\\") SETTING_RESP_ITEM_EL	_T("[%d]")					// eIMApp\Setting\Response\Item[d]

#define PATH_SETTING_SERVER				PATH_SETTING _T("\\") SETTING_SERVER_EL										// eIMApp\Setting\Server
#define PATH_SETTING_SERVER_IM_(N)		PATH_SETTING_SERVER _T("\\") SETTING_IM_EL _T("[") _T(#N) _T("]")			// eIMApp\Setting\Server\IM[n]
#define PATH_SETTING_SERVER_P2P_(N)		PATH_SETTING_SERVER _T("\\") SETTING_P2P_EL _T("[") _T(#N) _T("]")			// eIMApp\Setting\Server\P2P[n]
#define PATH_SETTING_SERVER_RDP_(N)		PATH_SETTING_SERVER _T("\\") SETTING_RDP_EL _T("[") _T(#N) _T("]")			// eIMApp\Setting\Server\RDP[n]
#define PATH_SETTING_SERVER_FILE_(N)	PATH_SETTING_SERVER _T("\\") SETTING_FILE_EL _T("[") _T(#N) _T("]")			// eIMApp\Setting\Server\File[n]
#define PATH_SETTING_SERVER_IMAGE_(N)	PATH_SETTING_SERVER _T("\\") SETTING_IMAGE_EL _T("[") _T(#N) _T("]")		// eIMApp\Setting\Server\Image[n]
#define PATH_SETTING_SERVER_HEAD_(N)	PATH_SETTING_SERVER _T("\\") SETTING_HEAD_EL _T("[") _T(#N) _T("]")			// eIMApp\Setting\Server\Head[n]

#define PATH_SETTING_SERVER_FILE_BT(N)	 PATH_SETTING_SERVER _T("\\") SETTING_FILE_EL_BT _T("[") _T(#N) _T("]")		// eIMApp\Setting\Server\File_BT[n]
#define PATH_SETTING_SERVER_IMAGE_BT_(N) PATH_SETTING_SERVER _T("\\") SETTING_IMAGE_EL_BT _T("[") _T(#N) _T("]")	// eIMApp\Setting\Server\Image_BT[n]

// Font.config
#define PATH_FONT_ITEM_FMT				FONT_ROOT_EL _T("\\") FONT_ITEM_EL _T("[%d]")								// eIMFont\Item[d]

#endif // __EIM_CONFIG_CONST_HEADER_2014_06_26_YFGZ__