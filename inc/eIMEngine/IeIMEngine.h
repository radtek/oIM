// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMUEngine is the engine function DLL
// 
// Auth: yfgz
// Date: 2013/11/14 9:27:21
// 
// History: 
//    2013/7/4 yfgz Finished first version
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIMENGINE_HEADER_2013_11_07_YFGZ__
#define __IEIMENGINE_HEADER_2013_11_07_YFGZ__

#include "Public\Plugin.h"
#include "eIMEngine\IeIMCmd.h"
#include "eIMEngine\IeIMProtocol.h"
#include "eIMEngine\IeIMContacts.h"
#include "eIMEngine\IeIMMsgMgr.h"
#include "eIMEngine\IeIMSessionMgr.h"
#include "eIMEngine\IeIMHistory.h"
#include "eIMEngine\IeIMFileMgr.h"
#include "eIMEngine\IeIMSMSMgr.h"
#include "eIMEngine\IeIMMsgCollectMgr.h"
#include "eIMPinyin\IeIMPinyin.h"
#include "eIMEngine\IeIMLoginUserMgr.h"
#include "eIMEngine\IeIMStateSubscribeMgr.h"
#include "eIMEngine\IeIMRoamDataMgr.h"
#include "eIMP2P\IeIMP2P.h"
#include "../../src/ClientAgent/src/errors.h"
#include "eIMEngine\IeIMUrlFileMgr.h"
#include "eIMEngine/IMeetingMgr.h"

// EIMEngine error code 
#define ERR_INVALID_PARAMETER		( 1 )	// Invalid Parameter


// Interface Name in eIMEngine
#define INAME_EIMENGINE					_T("SXIT.eIMEngine")				// I_EIMEngine interface name
#define INAME_EIMENGINE_EMOTICON		_T("SXIT.eIMEngine.Emoticon")       // I_eIMXmlEmoticon interface name
#define INAME_EIMENGINE_PROTOCOL		_T("SXIT.eIMEngine.Protocol")		// I_EIMProtocol interface name, Only by QueryInterface
#define INAME_EIMENGINE_CONTACTS		_T("SXIT.eIMEngine.Contacts")		// I_EIMContacts interface name, Only by QueryInterface
#define INAME_EIMENGINE_SESSIONMGR		_T("SXIT.eIMEngine.SessionMgr")		// I_EIMSessionMgr interface name, Only by QueryInterface
#define INAME_EIMENGINE_MEETING         _T("SXIT.eIMEngine.MeetingMgr")
#define INAME_EIMENGINE_LOGIN_USER_MGR	_T("SXIT.eIMEngine.LoginUserMgr")	// I_EIMLoginUserMgr interface name, Only by QueryInterface
// SQLite3 interface defined in "I_SQLite.h"
//#define INAME_SQLITE_DB				_T("SXIT.SQLite.Database")			// I_SQLiteDatabase interface name
//#define INAME_SQLITE_TABLE			_T("SXIT.SQLite.Table")				// I_SQLiteTable interface name
//#define INAME_SQLITE_STMT				_T("SXIT.SQLite.Statement")			// I_SQLiteStmt interface name
#define INAME_EIMENGINE_HISTORY			_T("SXIT.eIMEngine.History")		// I_EIMHistory interface name, Only by QueryInterface
#define INAME_EIMENGINE_MSGMGR			_T("SXIT.eIMEngine.MsgMgr")			// I_EIMMsgMgr interface name, Only by QueryInterface
#define INAME_EIMENGINE_FILEMGR			_T("SXIT.eIMEngine.FileMgr")		// I_EIMFileMgr interface name, Only by QueryInterface
#define INAME_EIMENGINE_SMSMGR			_T("SXIT.eIMEngine.SmsMgr")			// I_EIMSMSMgr interface name, Only by QueryInterface
//#define INAME_EIMENGINE_P2P			_T("SXIT.eIMEngine.P2P")			// I_EIMP2P interface name, Only by QueryInterface
//#define INAME_EIMENGINE_RECENT		_T("SXIT.eIMEngine.Recent")			// I_EIMRecent interface name, Only by QueryInterface
//#define INAME_EIMENGINE_FILETASKS		_T("SXIT.eIMEngine.FileTasks")		// I_EIMFileTask interface name, Only by QueryInterface
#define INAME_EIMENGINE_SUBSCRIBE		_T("SXIT.eIMEngine.Subscribe")		// I_EIMSubScribeMgr interface name, Only by QueryInterface
#define INAME_EIMENGINE_ROAMDATA		_T("SXIT.eIMEngine.RoamData")		// I_EIMFileTask interface name, Only by QueryInterface
// SXIT.EIMInternetFile.* interface defined in "IeIMInternetFile.h"
//#define INAME_EIMSINGLE_INTER_FILE	_T("SXIT.EIMInternetFile.SINGLE")	// I_eIMSingleInternetFile interface name Single File
//#define INAME_EIMBATCH_INTER_FILE		_T("SXIT.EIMInternetFile.BATCH")	// I_eIMBatchInternetFile interface name Batch File
#define INAME_EIMENGINE_MSGREAD			_T("SXIT.eIMEngine.MsgReadMgr")		// I_eIMMsgReadMgr interface name, Only by QueryInterface
#define INAME_EIMENGINE_BROMGR          _T("SXIT.eIMEngine.BroMgr")			// I_EIMBroMgr interface name
#define INAME_EIMENGINE_MSGCOLLECTMGR	_T("SXIT.eIMEngine.MsgCollectMgr")	// I_EIMMsgCollectMgr interface name
//#define INAME_EIMENGINE_MULTIMEDIALIST  _T("SXIT.eIMEngine.MultiMediaList")
#define INAME_EIMENGINE_IMPORT_MSG		_T("SXIT.eIMEngine.ImportMsg")		// I_EIMImportMsgMgr interface name
#define INAME_EIMENGINE_URLFILE			_T("SXIT.eIMEngine.UrlFileMgr")		// I_EIMUrlFileMgr interface name
#define INAME_EIMENGINE_MEETINGMGR		_T("SXIT.eIMEngine.MeetingMgr")		// I_MeetingMgr interface name

//Attribute///////////////////////////////////////////////////////////
// IME_ATTRIBS_* is String Attribute
// IME_ATTRIBI_* is Integer Attribute
// Define eIMEngine attribute
#define IME_ATTRIB_SERVER_IP				_T("ServerIP")					// IM main server IP
#define IME_ATTRIB_SERVER_PORT				_T("ServerPort")				// IM main server port
#define IME_ATTRIB_SERVER_IPB				_T("ServerIPB")					// IM remote server IP
#define IME_ATTRIB_SERVER_PORTB				_T("ServerPortB")				// IM remote server Port

#define IME_ATTRIB_P2P_IP					_T("P2PIP")						// P2P server IP
#define IME_ATTRIB_P2P_PORT					_T("P2PPort")					// P2P server Port

#define IME_ATTRIB_FILE_UPLOAD				_T("FileUpload")				// File upload server url or ip
#define IME_ATTRIB_FILE_DOWNLOAD			_T("FileDownload")				// File download server url or ip

#define IME_ATTRIB_IMAGE_UPLOAD				_T("ImageUpload")				// Image upload server url or ip
#define IME_ATTRIB_IMAGE_DOWNLOAD			_T("ImageDownload")				// Image download server url or ip

#define IME_ATTRIB_HEAD_UPLOAD				_T("HeadUpload")				// Head image upload server url or ip
#define IME_ATTRIB_HEAD_DOWNLOAD			_T("HeadDownload")				// Head image download server url or ip

#define IME_ATTRIB_FILE_UPLOAD_TOKEN		_T("FileUploadToken")			// File BT get upload token server url or ip
#define IME_ATTRIB_FILE_UPLOAD_BT			_T("FileUploadBT")				// File BT upload server url or ip
#define IME_ATTRIB_FILE_DOWNLOAD_BT			_T("FileDownloadBT")			// File BT download server url or ip
#define IME_ATTRIB_IMAGE_UPLOAD_TOKEN		_T("ImageUploadToken")			// Image BT get upload token server url or ip
#define IME_ATTRIB_IMAGE_UPLOAD_BT			_T("ImageUploadBT")				// Image BT upload server url or ip
#define IME_ATTRIB_IMAGE_DOWNLOAD_BT		_T("ImageDownloadBT")			// Image BT uplooad server url or ip

// VoSIP Metting configuration
#define IME_ATTR_MEETING_FILETOKEN			_T("Meeting_FileToken")			
#define IME_ATTR_MEETING_FILEUPLOAD			_T("Meeting_FileUpload")
#define IME_ATTR_MEETING_FILEDOWNLOAD		_T("Meeting_FileDownLoad")
#define IME_ATTR_MEETING_GETLAUNCHSTRING	_T("Meeting_GetLaunchString")
#define IME_ATTR_MEETING_CREATEMEETING		_T("Meeting_CreateMeeting")
#define IME_ATTR_MEETING_SYSCONFERENCE		_T("Meeting_SysConference")
#define IME_ATTR_MEETING_UPDATEMEETING		_T("Meeting_UpdateMeeting")
#define IME_ATTR_MEETING_CANCELMEETING		_T("Meeting_CancelMeeting")
#define IME_ATTR_MEETING_MEMBERCHANGE		_T("Meeting_MemberChange")
#define IME_ATTR_MEETING_FILECHANGE			_T("Meeting_FileChange")
#define IME_ATTR_MEETING_REGETMEETINGINFO	_T("Meeting_ReGetMeetingInfo")
#define IME_ATTR_MEETING_RECEIVEMEETING		_T("Meeting_ReceiveMeeting")
#define IME_ATTR_MEETING_MEETINGLEVELCHANGE  _T("Meeting_MeetingLevelChange")
#define IME_ATTR_MEETING_MEETINGREAD         _T("Meeting_MeetingRead")
#define IME_ATTR_MEETING_DEFAULT_IMAGE_MAN   _T("Meeting_DefaultImage_man")
#define IME_ATTR_MEETING_DEFAULT_IMAGE_WOMAN _T("Meeting_DefaultImage_woman")
#define IME_ATTR_MEETING_GETVIDEOURL		 _T("Meeting_GetMeetingVideo")
#define IME_ATTR_MEETING_GETSENIORURL        _T("Meeting_SeniorWndUrl")
#define IME_ATTR_MEETING_JOINMEETINGPHONE    _T("Meeting_JoinMeetingByPhone")
#define IME_ATTR_MEETING_GETMEETINGROOMLIST  _T("Meeting_GetMeetingRoomList")


//#define IME_ATTRIB_APP_VERSION			_T("@AppVersion")				// Application verson
#define IME_ATTRIB_SKIP_VERSION			_T("SkipVersion")				// Skip the version to update

#define IME_ATTRIB_ACCESS_MODE			_T("AccesMode")					// 1: CMNET 2: CMWAP, Default: CMNET
#define IME_ATTRIB_USER_NAME			PATH_TOKEN_LOGINNAME			// Current user name
#define IME_ATTRIB_USER_ID				PATH_TOKEN_LOGINID				// UID
#define IME_ATTRIB_SM_USER				_T("SM_USER")					// User Code
#define IME_ATTRIB_SM_KEY				_T("SM_KEY")					// User sso key
#define IME_ATTRIB_SSO_PARAMS_APPEND	_T("SSO_ParamsAppend")			// Append parameter of IME_ATTRIB_SSO_PARAMS
#define IME_ATTRIB_SSO_PARAMS			_T("SSO_Params")				// SSO Params, eg.: ?param1=S%2fCEv0lQ1O9RMyub2nwFUg%3d%3d&param2=XPjdHZvUD4Xnz1CTUBdYeg%3d%3d&param3=0ee5b8fb18ceab9199717626550445a9
#define IME_ATTRIB_CO_ID				_T("@CID")						// Company ID
#define IME_ATTRIB_USER_PSW				_T("UserPsw")					// Current user's password
#define IME_ATTRIB_AUTO_LOGIN			_T("AutoLogin")					// Auto login
#define IME_ATTRIB_REMEMBER_PSW			_T("RememberPsw")				// Remember psw
#define IME_ATTRIB_LOGIN_TYPE			_T("LoginType")					// Login type
#define IME_ATTRIB_OFF_GROUP_COUNT		_T("OffRGroupCount")			// Off regular group count
#define IME_ATTRIB_GET_ROBOT_PAGE_COUNT	_T("GetRobotInfoPageCount")		// Get robot info page count
#define IME_ATTRIB_OFFMSG_FINISHED		_T("OffMsgFinished")			// Offline message finished(after it to notify new msg):BOOL value
#define IME_ATTRIB_CONTACTS_LOADED		_T("ContactsLoaded")			// Contacts loaded
#define IME_LOGIN_TYPE_ONLINE			( 0 )							// Login as online
#define IME_LOGIN_TYPE_ONLIVE			( 1 )							// Login as onlive
#define IME_FILE_TRAN_NEW_MODEL			_T("FileTranNewModel")			// Login as onlive
#define IME_FAST_LOAD_CONTACTS			_T("FastLoadContacts")			// Fast load contacts

#define IME_ATTRIB_RECONNECT_TIMES		_T("ReconnectTimes")			// Default 10 times
# define IME_RECONNECT_TIMES_DEFAULT	(0)
#define IME_ATTRIB_DOWNLOAD_EMPS_DELAY	_T("DownloadEmpsDelay")			// Download employee's detail delay times, unit as minute, default: 5min
//#define IME_ATTRIB_DOWNLOAD_CONTACTDB	_T("DownloadContactsDb")		// Download contacts.db if current user not init contacts.db
//#define IME_ATTRIB_DOWNLOAD_CONTACTFILE	_T("DownloadContactsFile")		// Download contacts's file
//#define IME_ATTRIB_DOWNLOAD_CONTACTDB_DISABLE_PROXY	_T("DownloadContactsDbDisableProxy")// Download contacts.db if current user not init contacts.db
#define IME_ATTRIB_FILE_TRANSFER_THREAD_NUM	_T("FileTransferThreadNum")	// File transfer thread count, [1,12]
#define IME_ATTRIB_SETUP_TIMESTAMP		_T("SetupTimestamp")			// Setup timestamp
#define IME_ATTRIB_SWITCH_FILE_SERVER	_T("SwitchFileServer")			// Sitch file server
#define IME_ATTRIB_ENABLE_SSO			_T("EnableSSO")					// SSO
#define IME_ATTRIB_ROBOT_MENU_URL		_T("RobotMenuUrl")				// Robot menu request URL
#define IME_ATTRIB_ROBOT_TOPIC_URL		_T("RobotTopicUrl")				// Robot topic request URL
#define IME_ATTRIB_WEBAPP_URL			_T("WebAppUrl")					// Web app request URL
#define IME_ATTRIB_WEBAPPS_DEFAULT		_T("WebAppsDefault")			// Web app JSON content
#define IME_ATTRIB_WEBAPPS				_T("WebApps")					// Web app JSON content
#define IME_ATTRIB_WEBAPPS_INITIALIZED	_T("WebAppInited")				// initialized?
#define IME_ATTRIB_FORCE_RELOAD2EMPS	_T("ForceReload2Emps")			// Force reload emps when detail download ed(Only for LongFor now)

#define IME_ATTRIB_SSO_COOKIE_URL		_T("SSOCookieUrl")				// "http://qysso.csair.com/login.aspx";
#define IME_ATTRIB_SSO_URL				_T("SSOUrl")					// "https://ssoii.csair.com/siteminderagent/forms/login/login.fcc?TYPE=33554433&REALMOID=06-483cc0ef-e5ff-447e-ba80-c3cee3b38c4e&GUID=&SMAUTHREASON=0&METHOD=GET&SMAGENTNAME=-SM-pzaN%2bJfnBs8RBKGiQjcuBeuvFM4D0Em8R%2bfrtBNCX4lLRfY0ui%2fdx39lKZWzGtxt&TARGET=-SM-http%3a%2f%2fqysso%2ecsair%2ecom%2flogin%2easpx";
#define IME_ATTRIB_SSO_HOST				_T("SSOHost")					// "HOST: qysso.csair.com";

#define IME_ATTRIB_PINYIN				_T("Pinyin")	
# define IME_ATTRIB_PINYIN_DEFAULT		( 0x00 )						// Search all full pinyin combination 
# define IME_ATTRIB_PINYIN_FAST			( 0x01 )						//Search NO all full pinyin combination(only first chooice)
# define IME_ATTRIB_PINYIN_CONSTS_X		( 0x10 )						// Disable contacts pinyin function
# define IME_ATTRIB_PINYIN_EMPS_X		( 0x20 )						// Disable emps pinyin function

#define IME_DISABLE_DEPTUSER_VISABLE	_T("DisableDeptUserVisable")	// 1: Disable
#define IME_GET_DEPTUSER_VISABLE_INFO	_T("GetDeptUserVisibleInfo")	// 1: Need get, -1: Finished, -2: Failed and continue to load contacts, <= -3: timeout
#define IME_GET_DATA_LIST_TYPE			_T("GetDataListType")			// <= -3: timeout
#define IME_UPDATE_CONTACTS_TYPE		_T("UpdateContactsType")		// <= -3: timeout
#define IME_RELOAD_CONTACTS				_T("ReloadContacts")			// <= -3: timeout


// <MsgUI><MsgFont /><MsgUI> type defined
#define MSGUI_FONT_RECV_DEFAULT		"<MsgFont Size=9 Color=0x0000ff Id=0 Mode=0 />"	// Receive Msg default font
#define MSGUI_FONT_SEND_DEFAULT		"<MsgFont Size=8 Color=0x008040 Id=0 Mode=0 />"	// Send Msg default font
#define MSGUI_FONT_ERROR_DEFAULT	"<MsgFont Size=9 Color=0x659DB2 Id=0 Mode=0 />"	// Error Msg default font 
#define MSGUI_FONT_MSGAUTO_DEFAULT	"<MsgFont Size=9 Color=0x6c7075 Id=0 Mode=0 />"	// Auto respone Msg default font

#define MSGUI_FONT_RECV				_T("FontRecv")			// Received font
#define MSGUI_FONT_SEND				_T("FontSend")			// Default if not defined
#define MSGUI_FONT_1TOH				_T("Font1ToH")			// eSESSION_TYPE_1TO_HUNDRED
#define MSGUI_FONT_1TOT				_T("Font1ToT")			// eSESSION_TYPE_1TO_TEN_THOUSAND
#define MSGUI_FONT_ERROR			_T("FontError")			// eSESSION_TYPE_ERROR
#define MSGUI_FONT_MSGAUTO			_T("FontMsgAuto")		// eSESSION_TYPE_ERROR
#define MSGUI_FONT_RECEIPT_PIN_RECV _T("FontReceiptPinRcv")	// Receive receipt

///////////////////////////////////////////////////////////////////////////////
#define IME_ATTRIB_ENGINE_STATUS	_T("@EngineStatus")		// Engine status, see: eENGINE_STATUS_*
typedef enum tagEngineStatus
{	// ClientAgent: 0:离线 1:上线 2:离开
	eENGINE_STATUS_NO_LOGIN	= -2,		// No login
	eENGINE_STATUS_LOGINING = -1,		// Logining
	eENGINE_STATUS_OFFLINE  = 0,		// Offline
	eENGINE_STATUS_ONLINE	= 1,		// Online
	eENGINE_STATUS_LEAVE	= 2,		// Leave
	eENGINE_STATUS_QUIT		= 3,		// Quit
	eENGINE_STATUS_KICK		= 4,		// Kick
	eENGINE_STATUS_FORBIDDEN= 5,		// Frobidden
}E_EngineStatus, *PE_EngineStatus;

// eIMEngine Logger attribute
#define IME_ATTRIB_LOG_FILE			_T("LogFile")
#define IME_ATTRIB_LOG_LEVEL		_T("LogLevel")
#define IME_ATTRIB_LOG_FLAG			_T("LogFlag")

// Logined user purview
#define  CREATE_SESSION_PURVIEW			( 0x00000001 )	// Create session emps: the limit value 
# define CREATE_SESSION_MAX_DEFAULT		( 80 )			// 80 emps default max

#define  SEND_FILE_PURVIEW				( 0x00000002 )	// Send 500M files: the limit value, unit is MB
# define SEND_FILE_MAX_DEFAULT			( 50 )			// 50MB default max

#define  SEND_P2P_FILE_PURVIEW			( 0x00000004 )	// P2P file: bool value

#define  SEND_ALL_BROADCAST_PURVIEW		( 0x00000008 )	// Send broadcast to all emps: bool value
#define  SEND_DEPT_BROADCAST_PURVIEW	( 0x00000010 )	// Send department's broadcast: bool value
# define SEND_BROADCAST_MAX_DEFAULT		( 100 )			// Send broadcast default mak 

#define  SEND_SMS_PURVIEW				( 0x00000020 )	// Send SMS: the limit value, 0 is disable 
# define SEND_SMS_MAX_DEFAULT			( 80 )			// SMS default max 

#define  SEND_1TO1H_RESP_PURVIEW		( 0x00000040 )	// 1 to hundred responses

#define  CREATE_MEETING_PURVIEW         ( 0x00000041 )  // Create Meeting:0 is no purview
//#define  CREATE_MEETING_MAXNUM          ( 0x00000042 )  // Create Meeting Max Num
//#define  CREATE_MEETING_MAX_DEFAULT     ( 20 )          // Create Meeting Max MemberNum

#define  SEND_1TOTH_RESP_PURVIEW		( 0x00000080 )	// 1 to ten thousand responses
# define SEND_1TOTH_RESP_DEFAULT		( 200 )

#define  SEND_KAPOK_PURVIEW				( 0x00000100 )	// Kapok Tong Fei(Only Mobile used)

#define SEND_READMSG_PURVIEW			( 0x00000200 )
#define SEND_RECALLMSG_PURVIEW         ( 0x00000400 )

#define CALL_TO_MEETING                ( 0x00000800 )   //12,CALL TO MEETING
#define INVITE_TO_MEETING              ( 0x00001000 )   //13,INVITE TO MEETING

// m_sLoginAck.*** ==============================
#define SEND_SMS_MAX_LENGTH				( 0x80000001 )	// LOGINACK.wPCSMSMaxLength
#define SEND_SMS_SPLIT_LENGTH			( 0x80000002 )	// LOGINACK.wPCSMSSplitLength
#define FILE_TRANS_MODE_RESUME			( 0x80000003 )	// LOGINACK.wPCMaxSendFileSize Is file resume transfer mode: bool value
#define SUBSCRIBE_INTERVAL				( 0x80000004 )	// LOGINACK.cPCSubscribeInterval
#define TEMP_SUBSCRIBE_MAX_NUM			( 0x80000005 )	// LOGINACK.wPCTempSubscribeMaxNum
#define GET_STATUS_INTERVAL				( 0x80000006 )	// LOGINACK.cPCGetStatusInterval
#define GET_STATUS_MAX_NUM				( 0x80000007 )	// LOGINACK.wGetStatusMaxNum
#define GET_MSG_SYNC_TYPE				( 0x80000008 )	// LOGINACK.cMsgSynType
#define GET_PERSONAL_DISPLAY			( 0x80000009 )	// LOGINACK.dwPersonalDisplay
#define GET_PERSONAL_EDIT				( 0x8000000A )	// LOGINACK.dwPersonalEdit
#define GET_DEPT_USER_LANGUAGE_DISPLAY	( 0x8000000B )	// LOGINACK.cDeptUserLanguageDisplay
//#define GET_GROUP_MAX_MEMBER_NUM		( 0x8000000C )	// LOGINACK.wGroupMaxMemberNum
#define GET_ALIVE_MAX_INTERVAL			( 0x8000000D )	// LOGINACK.wPCAliveMaxInterval
#define GET_UPLOAD_RECENT_CONTACT		( 0x8000000E )	// LOGINACK.wPCUploadRecentContact
#define GET_HTTP_SESSIONID				( 0x8000000F )	// LOGINACK.dwSessionID
#define GET_MODIFY_PERSONAL_AUDIT_PERIOD (0x80000010 )	// LOGINACK.cModifyPersonalAuditPeriod
# define MODIFY_PSW_NOW					( 0 )			// Modify password now!

// LOGINACK.tTimeStamp.*** (UTS: Update TimeStamp)================
#define GET_UTS_CO						( 0x800000A1 )	// LOGINACK.tTimeStamp.dwCompUpdateTime;				
#define GET_UTS_DEPT					( 0x800000A2 )	// LOGINACK.tTimeStamp.dwDeptUpdateTime;				
#define GET_UTS_USER					( 0x800000A3 )	// LOGINACK.tTimeStamp.dwUserUpdateTime;				
#define GET_UTS_DEPT_USER				( 0x800000A4 )	// LOGINACK.tTimeStamp.dwDeptUserUpdateTime;			
#define GET_UTS_PERSONAL_INFO			( 0x800000A5 )	// LOGINACK.tTimeStamp.dwPersonalInfoUpdateTime;		
#define GET_UTS_PERSONAL_COMMON_CONTACT	( 0x800000A6 )	// LOGINACK.tTimeStamp.dwPersonalCommonContactUpdateTime
#define GET_UTS_PERSONAL_COMMON_DEPT	( 0x800000A7 )	// LOGINACK.tTimeStamp.dwPersonalCommonDeptUpdateTime;	
#define GET_UTS_PERSONAL_ATTENTION		( 0x800000A8 )	// LOGINACK.tTimeStamp.dwPersonalAttentionUpdateTime;	
#define GET_UTS_GLOBAL_COMMON_CONTACT	( 0x800000A9 )	// LOGINACK.tTimeStamp.dwGlobalCommonContactUpdateTime;	
#define GET_UTS_OTHERS_AVATAR			( 0x800000AA )	// LOGINACK.tTimeStamp.dwOthersAvatarUpdateTime;		
#define GET_UTS_PERSONAL_AVATAR			( 0x800000AB )	// LOGINACK.tTimeStamp.dwPersonalAvatarUpdateTime;		
#define GET_UTS_REGULAR_GROUP			( 0x800000AC )	// LOGINACK.tTimeStamp.dwRegularGroupUpdateTime;		
#define GET_UTS_USER_RANK				( 0x800000AD )	// LOGINACK.tTimeStamp.dwUserRankUpdateTime;			
#define GET_UTS_USER_PRO				( 0x800000AE )	// LOGINACK.tTimeStamp.dwUserProUpdateTime;				
#define GET_UTS_USER_AREA				( 0x800000AF )	// LOGINACK.tTimeStamp.dwUserAreaUpdateTime;			
#define GET_UTS_SPECIAL_LIST			( 0x800000B0 )	// LOGINACK.tTimeStamp.dwSpecialListUpdatetime;			
#define GET_UTS_SPECIAL_WHITE_LIST		( 0x800000B1 )	// LOGINACK.tTimeStamp.dwSpecialWhiteListUpdatetime;	
#define GET_UTS_SERVER_CURRENT_TIME		( 0x800000B2 )	// LOGINACK.tTimeStamp.nServerCurrentTime;				

// LOGINACK.tTimeStamp.*** (UTS: Update TimeStamp)================
#define GET_LTS_CO						( 0x800000C1 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwCompUpdateTime;				
#define GET_LTS_DEPT					( 0x800000C2 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime;				
#define GET_LTS_USER					( 0x800000C3 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime;				
#define GET_LTS_DEPT_USER				( 0x800000C4 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime;			
#define GET_LTS_PERSONAL_INFO			( 0x800000C5 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalInfoUpdateTime;		
#define GET_LTS_PERSONAL_COMMON_CONTACT	( 0x800000C6 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime
#define GET_LTS_PERSONAL_COMMON_DEPT	( 0x800000C7 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime;	
#define GET_LTS_PERSONAL_ATTENTION		( 0x800000C8 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAttentionUpdateTime;	
#define GET_LTS_GLOBAL_COMMON_CONTACT	( 0x800000C9 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime;	
#define GET_LTS_OTHERS_AVATAR			( 0x800000CA )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime;		
#define GET_LTS_PERSONAL_AVATAR			( 0x800000CB )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAvatarUpdateTime;		
#define GET_LTS_REGULAR_GROUP			( 0x800000CC )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime;		
#define GET_LTS_USER_RANK				( 0x800000CD )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwUserRankUpdateTime;			
#define GET_LTS_USER_PRO				( 0x800000CE )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwUserProUpdateTime;				
#define GET_LTS_USER_AREA				( 0x800000CF )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwUserAreaUpdateTime;			
#define GET_LTS_SPECIAL_LIST			( 0x800000D0 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwSpecialListUpdatetime;			
#define GET_LTS_SPECIAL_WHITE_LIST		( 0x800000D1 )	// m_sLastUpdateTTs.rUpdateTimeInfo.dwSpecialWhiteListUpdatetime;	
#define GET_LTS_SERVER_CURRENT_TIME		( 0x800000D2 )	// m_sLastUpdateTTs.rUpdateTimeInfo.nServerCurrentTime;	

#define GET_USER_OFFLINE_FLAG			( 0x800000F1 )	// m_bIsUserOffline
#define GET_SYSTEM_MSG_ID				( 0x800000F2 )	// 系统通知账号
#define GET_SYSTEM_MSG_RTX_ID			( 0x800000F3 )	// RTX系统通知账号
#define GET_SYSTEM_MSG_MAIL_ID			( 0x800000F4 )	// 邮件系统通知账号
#define GET_SYSTEM_PRINTER_ID			( 0x800000F5 )	// 打印系统账号
#define GET_ITSUPPORT_ID				( 0x800000FA )	// Get itsupport's qeid
#define GET_FILETRANSFER_ID				( 0x800000FB )	// Get Filetransfer qeid
#define GET_IROBOT_QEID					( 0x800000FC )	// Get iRobot qeid
#define GET_RECALL_TIME					( 0x800000FD )	// Get Recall time
#define CHECK_SYSTEM_ACCOUNT			( 0x800000FE )	// 检查是不是系统通知账号， GET_SYSTEM_MSG_*
#define CHECK_SPECIAL_ACCOUNT			( 0x800000FF )	// 检查是不是特殊账号

// Login user information ==========================
#define GET_LOGIN_SEX					( 0x8FFFFFFA )	// Get login sex
#define GET_LOGIN_TERM					( 0x8FFFFFFB )	// Get login terminal type
#define GET_LOGIN_STATUS				( 0x8FFFFFFC )	// Get login status
#define GET_LOGIN_QDID					( 0x8FFFFFFD )	// Get login qdid
#define GET_LOGIN_QEID					( 0x8FFFFFFE )	// Get login qeid
#define GET_LOGIN_COID					( 0x8FFFFFFF )	// Get login coid
#define GET_IS_DOMAIN_USER				( 0x8FFFFFF0 )	// Get IsDomainUser, implement in MainFrame::IsDomainUser()
#define GET_PCONF_USER_INFO				( 0x8FFFFFF1 )	// Get &m_sMeetingAccountInfo
#define GET_PLOGIN_EMP_INFO				( 0x8FFFFFF2 )	// Get &m_sLoginEmpInfo

// Reload contacts type([S]: is server troggle, [U]: is user troggle)
#define IME_UPDATE_CONTS_TYPE_NOW			( 0x0001 )	// [S]Reload contacts data by server command(clear local database then download) immediate
#define IME_UPDATE_CONTS_TYPE_NEXT_LOGIN	( 0x0002 )	// [S]Reload contacts data by server command next time login
#define IME_UPDATE_CONTS_TYPE_SERVER		( 0x8000 )	// [U]Reload contacts data by download from server
#define IME_UPDATE_CONTS_TYPE_LOCAL			( 0x8001 )	// [U]Reload contacts data by local
#define IME_UPDATE_CONTS_TYPE_SERVER_UI		( 0x8002 )	// [U]Reload contacts data by download from server => IME_UPDATE_CONTS_TYPE_SERVER

// Meeting
#define MEETING_MODIFY_TITLE               1
#define MEETING_MODIFY_REMARK              2
#define MEETING_MODIFY_BEGINTIME           4
#define MEETING_MODIFY_ENDTIME             8
#define MEETING_MODIFY_PLACE               16
#define MEETING_MODIFY_MEETING_LEN         32
#define MEETING_MODIFY_SMS_NOTICE          64

typedef enum tagThreadIndex
{
	eTRHEAD_FLAG_MGR_IDX =  0,	
	eTHREAD_FLAG_SEND_IDX,
	eTHREAD_FLAG_RECV_IDX,
	eTHREAD_FLAG_DISP_IDX,
	eTHREAD_FLAG_DB_IDX,
	eTHREAD_FLAG_STATESUB_IDX,
	eTHREAD_FLAG_CURL_IDX,
	eTHREAD_FLAG_UI_WAIT_OVERTIME,
    eTHREAD_FLAG_DOWNLOAD_URLFILE,
}E_ThreadIdx, *PE_ThreadIdx;

class __declspec(novtable) I_EIMEngine: public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     GetTimeNow
	//Description:	Get current time by server calibration
	//
	//
	//Return:
	//		Return current time             
	//=============================================================================
	virtual DWORD GetTimeNow() = 0;

	//=============================================================================
	//Function:     Command
	//Description:	Execute a command, it format reference <<Protocol.config>>
	//
	//Parameter:
	//	pszXml     - The xml format command
	//
	//Return:
	//		Return HRESULT         
	//=============================================================================
	virtual int Command(const TCHAR* pszXml, E_ThreadIdx eThreadId = eTHREAD_FLAG_SEND_IDX) = 0;
	virtual int AddCmd(E_ThreadIdx eThreadIdx, I_EIMCmd* pCmd, BOOL bAddRef=TRUE) = 0;
	virtual int AddCmd(E_ThreadIdx eThreadIdx, DWORD dwCmdId, const void* pvData) = 0;

	// Attribute /////////////////////////////////////////////////////////////////////
	// Attribute /////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     GetAttributeStr
	//Description:	Get string attribute
	//
	//Parameter:
	//	pszName       - Attribute Name
	//	pszDefault    - Default value
	//
	//Return:
	//		NULL		If pszName is invaild
	//		pszDefault	If not exist
	//		Non-Null	The attribute value
	//=============================================================================
	virtual const TCHAR* GetAttributeStr(
		const TCHAR*	pszName,
		const TCHAR*	pszDefault = NULL
		) = 0;

	//=============================================================================
	//Function:     SetAttributeStr
	//Description:	Set string attribute
	//
	//Parameter:
	//	pszName     - Attribute name
	//	pszValue    - Attribute string value
	//
	//Return:
	//		NULL		If pszName is invalid, or no old value
	//		Non-Null	The old value
	//=============================================================================
	virtual const BOOL SetAttributeStr(
		const TCHAR*	pszName,
		const TCHAR*	pszValue
		) = 0;

	//=============================================================================
	//Function:     GetAttributeInt
	//Description:	Get integer attribute
	//
	//Parameter:
	//	pszName       - Attribute name
	//	i32Default    - Default value
	//
	//Return:
	//		NULL		If name is invalid
	//		i32Default	If not exist
	//		Non-Null	The attribute value
	//=============================================================================
	virtual const int GetAttributeInt(
		const TCHAR*	pszName,
		int				i32Default = 0
		) = 0;

	//=============================================================================
	//Function:     SetAttributeInt
	//Description:	Set integer attribute
	//
	//Parameter:
	//	pszName     - Attribute name
	//	i32Value    - Attribute value
	//
	//Return:
	//		0		If name is invalid, or no old value
	//		Non-0	The old value
	//=============================================================================
	virtual const BOOL SetAttributeInt(
		const TCHAR*	pszName,
		const int		i32Value
		) = 0;

	//=============================================================================
	//Function:     Login
	//Description:	Connect to server and login
	//Notes: Before Login, you can change fellow attribute:
	//  IME_ATTRIB_SERVER_IP	
	//  IME_ATTRIB_SERVER_PORT	
	//  IME_ATTRIB_SERVER_IPB	
	//  IME_ATTRIB_SERVER_PORTB
	//  IME_ATTRIB_LOG_FILE	
	//  IME_ATTRIB_LOG_LEVEL	
	//  IME_ATTRIB_LOG_FLAG	
	//
	//Parameter:
	//	pszName    - Login Name, Id, or email and so on
	//	pszPsw     - Password
	//
	//Return:
	//		0		Return Succeeded     
	//=============================================================================
	virtual int Login(
		const TCHAR* pszName, 
		const TCHAR* pszPsw
		) = 0;

	//=============================================================================
	//Function:     Logout
	//Description:	Logout or leave
	//Parameter:
	//	nStatus     - Status, reference LOGOUT::cStatus
	//	cManual     - Manual flag, reference LOGOUT::cManual
	// Return:
	//	HRESULT
	//=============================================================================
	virtual int	Logout(
		int		nStatus, 
		BYTE	cManual
		) = 0;

	//=============================================================================
	//Function:     GetLoginEmp
	//Description:	Get login employee information
	// Return:
	//	Logined mployee information
	//=============================================================================
	virtual PS_EmpInfo_ GetLoginEmp() = 0;

	//=============================================================================
	//Function:     GetPurview
	//Description:	Get purview information, reference GET_*
	//Parameter:
	//	nStatus     - Status, reference LOGOUT::cStatus
	//	cManual     - Manual flag, reference LOGOUT::cManual
	// Return:
	//	The value
	//=============================================================================
	virtual DWORD GetPurview(DWORD u32Type) = 0;

	//=============================================================================
	//Function:     AddFontElement
	//Description:	Add the font information to element
	//Parameter:
	//	eSType     - Sesson type
	//	bSend      - TRUE is send, else is receive
	//  pParentEl  - The parent element to be add font information
	//
	// Return:
	//	None
	//=============================================================================
	virtual void AddFontElement(E_SessionType eSType, BOOL bSend, void* pParentEl) = 0;

	//=============================================================================
	//Function:     IsFace
	//Description:	Check Emoticon
	//Parameter:
	//	pszFace     - To check Emoticon
	//
	// Return:
	//	TRUE	- Is Emoticon
	//  FALSE	- Not a Emoticon
	//=============================================================================
    virtual BOOL IsFace(const TCHAR* pszFace) =0;

 	//=============================================================================
	//Function:     SetUrlFileInfo
	//Description:	Set the url information of file
	//Parameter:
	//	sUrlInfo    - url information
	//
	// Return:
	//	None
	//=============================================================================
	virtual void SetUrlFileInfo(const S_UrlFileInfo& sUrlInfo) = 0;
 
 	//=============================================================================
	//Function:     GetLocalPath
	//Description:	Get the file path
	//Parameter:
	//	sUrlInfo		- url information
	//  pfnGetLocalPath	- Callback function
	//  pvUserData		- User data
	//
	// Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL GetLocalPath(const S_UrlFileInfo& sUrlInfo, PFN_GetLocalPath pfnGetLocalPath, void* pvUserData) = 0;

 	//=============================================================================
	//Function:     DelCollectMsg
	//Description:	Delete collect messages
	//Parameter:
	//	CollectIdLst	- the collect messages to be delete
	//
	// Return:
	//	None
	//=============================================================================
	virtual void DelCollectMsg(const std::list<UINT64>& CollectIdLst) = 0;

 	//=============================================================================
	//Function:     _cUrlUpload
	//Description:	Upload file
	//Parameter:
	//	pszUrl		- Url of server
	//  pszSaveAs	- Local file path
	//  sUpload		- Meeting information
	//	i32Retry	- Retry times 
	//
	// Return:
	//	HRESULT
	//=============================================================================
	virtual int  _cUrlUpload(const char* pszUrl,const TCHAR* pszSaveAs,S_UploadMeetingFile* sUpload = NULL, int i32Retry=0, DWORD dwTimeout=30) = 0;
	
	virtual QSID NewID(QEID eidRecver=0, QEID eidSender=0) = 0;

};





#endif // __IEIMENGINE_HEADER_2013_11_07_YFGZ__