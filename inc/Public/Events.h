#ifndef __EIM_EVENT_2013_12_17_BY_YFGZ__
#define __EIM_EVENT_2013_12_17_BY_YFGZ__

#define EIM_VERSION_URL_SIZE		( 255 )		// Update version URL size
#define EIM_VERSION_DESC_SIZE		( 255 )		// Update version desc size
#define EIM_CUSTOM_GTOUP_MEMBER_MAX		( 400 )	//自定义组成员最大数	
#define EIM_CUSTOM_GTOUP_NAME			( 50 )	//自定义组name

#define CONFID_LENGTH               ( 32 )     //全时会议ID长度
#define CONFCODE_LENGTH             ( 32 )     //全时code长度
#define CONFFILETOKEN_LENGTH        ( 32 )     //全时文件token长度
#define INVITEE_MAX                 ( 500 )    //人员数组最大长度
#define CHANGEMEMBERS_MAX              ( 300 )    //增加或删除人员长度
#pragma pack( push, 8 )
//Event/////////////////////////////////////////////////////////////////////////////
// [0x000 ~ 0x100) is reserved for ClientAgent in Engine
// [0x100 ~ 0x200) is reserved for Engine internal

#define EVENT_ENGINE_FILE_TRANS			( 0x1FE )					// Engine internal file transfer status loop
#define EVENT_EVENT_LOOP				( 0x1FF )				    // Event manager's event loop, P: 0-Disable, 1-Enable, 2-PeekQueneMessage
# define EVENT_LOOP_DISABLE             ( 0 )                       // Disable(when Engine exit)
# define EVENT_LOOP_ENABLE              ( 1 )                       // Enalbe(not need really to use it)
# define EVENT_LOOP_PEEKQUENE           ( 2 )                       // Peek quene message
# define EVENT_LOOP_PEEKMESSAGE			( 3 )						// Peek message

enum tagEIMEvent
{	//EVENT_APP + N ////////////////////////////////////////////////////////////////////////////////////
	EVENT_APP = 0x00000200,				// Apps Event start address
	EVENT_SEND_CMD,						// Send CMD, P: I_EIMCmd*, the cmd will be execute by flag[SendThread->DbThread->UIThread] 

	//EVENT_INIT,						// Init client, P: EIMERR_NO_ERROR, EIMERR_INIT_FAILED
	//EVENT_CONNECT_BEFORE,				// Before connect, P: NULL
	//EVENT_CONNECTING,					// Connecting, P: PS_ConnParam
	//EVENT_RECONNECTING,				// Auto Re-connecting, P: PS_ConnParam
	//EVENT_CONNECT_AFTER,				// After Connected, P: Connect result, 0: succeeded, else is failed

	EVENT_LOGIN_BEFORE,					// Before login, P: NULL
	EVENT_LOGIN_STATUS,					// Login status, P: PS_LoginStatus
	EVENT_LOGIN_AFTER,					// After login, P: EIMERR_NO_ERROR, EIMERR_NO_CONN, EIMERR_INVALID_PARAM
	EVENT_LOGIN_RET_USERENTER,			// Return User、Password enter ui
	EVENT_OVERLOAD_INFO,				// Overload information, P:TCHAR*
	EVENT_CONNECTED_SERVER,				// Connected server, P: server index(0: main server, 1: backup server)

	EVENT_APPBUTTONMOVE,				//The AppButton on AppManager Moved
	EVENT_CHECK_VERSION,				// Check version, P: PS_CheckVersion
	EVENT_EXIT_APP,						// Exit App, P: see fellow: EXIT_APP_*
# define EXIT_APP_UPDATE				( 0x00000001 )				// Exit app for update
# define EXIT_APP_SAVE_CONFIG			( 0x00000002 )				// Exit app for save config

	EVENT_UPDATE_TTS,					// Update timestamps in LoginAck, P: PS_UpdateTimestamps, only used in Engine
	EVENT_LOGIN_EMP_INFO,				// Login employee information, P: PS_EmpInfo
	EVENT_GET_DEPT_INFO_END,			// Finished of GetDeptInfo, P: RESULT
	EVENT_GET_DEPT_EMPS_END,			// Finished of GetDeptEmps, P: RESULT
	EVENT_GET_SPECIAL_LIST_END,			// Finished of GetSpecialList, P: RESULT
	EVENT_MODIFY_INFO_ACK,				// Modify self information ack, P: RESULT
	EVENT_GET_EMPS_INFO_END,			// Finished of GetUserList, P: RESULT
	EVENT_GET_EMPSAVATAR_LIST_END,		// Finished of C_CmdGetHeadIconAddListAck, P: RESULT
										
	EVENT_SEND_CMD_BEFORE,				// Before send DoCmd, P: I_EIMCmd*
	EVENT_SEND_CMD_AFTER,				// After  send DoCmd, P: I_EIMCmd*
	EVENT_RECV_CMD_BEFORE,				// Before Recv DoCmd, P: I_EIMCmd*
	EVENT_RECV_CMD_AFTER,				// After  Recv DoCmd, P: I_EIMCmd*
	EVENT_SEND_MSG,						// Send msg, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_SEND_MSG_ACK,					// Send msg Ack, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_RECV_MSG,						// Receive msg, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_RECV_OFFLINE_MSG,				// Receive msg, P: NULL
								
	EVENT_CREATE_GROUP,					// CreateGroup failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_CREATE_GROUP_ACK,				// CreateGroupAck failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml, Result maybe is EIMERR_GROUP_EXISTED
	EVENT_MODIFY_GROUP,					// ModifyGroup failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_MODIFY_GROUP_ACK,				// ModifyGroupAck failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_MODIFY_GROUP_NOTICE,			// ModifyGroupNotice, P: I_EIMCmd*; GetXml() get data of xml
	EVENT_MODIFY_GROUP_MEMBER,			// ModifyGroupMember failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_MODIFY_GROUP_MEMBER_ACK,		// ModifyGroupMemberAck failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_MODIFY_GROUP_MEMBER_NOTICE,	// ModifyGroupMemberNotice, P: I_EIMCmd*; GetXml() get data of xml
	EVENT_QUIT_GROUP,					// QuitGroup failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_QUIT_GROUP_ACK,				// QuitGroupAck failed, P: I_EIMCmd*; Use GetResult() to check, and GetErrorDesc() and GetXml() get data of xml
	EVENT_QUIT_GROUP_NOTICE,			// QuitGroupNotice, P: I_EIMCmd*; GetXml() get data of xml
	EVENT_MODIFY_MYSELF_INFO_RSLT,		// ModifyMyselfInfo result, P: Result
	EVENT_GET_EMPLOYEE_INFO_ACK,		// GetEmployeeInfoAck, P: PS_EmpInfo
	EVENT_GET_GROUP_INFO,				// Get group information, P: QSID
	EVENT_MODIFY_GROUP_REMARK_ACK,		// Modify group remark P: QSID
	EVENT_GET_VGROUP_INFO,				// Get regular group information, P: QSID
	EVENT_MODIFY_GROUP_NAME_ACK,		// Modify group name P:QSID
	EVENT_GET_GROUP_INFO_ACK,			// Get group information,P:QSID
	EVENT_GET_VGROUP_INFO_ACK,			// Get vgroup information,P:QSID
	EVENT_MODIFY_VGROUP_NAME_NOTICE,	// Modify vgroup name,P:QSID
	EVENT_MODIFY_VGROUP_MEMBER_NOTICE,	// Modify vgroup member,P:QSID
//	EVENT_UPDATE_SINGLE_TITLE,			// Update single session title by language change, P:C_eIMUISessionWnd*
	EVENT_UPDATE_SESSION_COUNTER,		// Update session counter, P:PS_SessionCounter
	EVENT_MODIFY_GROUP_MEMBER_FAILD_ACK,
	EVENT_TOP_SESSION,					// Top session, P: PS_UISessionInfo
	EVENT_UNINIT,						// Uninit client
						
	EVENT_DEPT_ADD,						// Add department, P: PS_DeptEmp
	EVENT_DEPT_DEL,						// Delete department, P: PS_DeptEmp
	EVENT_DEPT_MOD,						// Modify department, P: PS_DeptEmp
							
	EVENT_EMP_ADD,						// Add employee, P: PS_EmpInfo
	EVENT_EMP_DEL,						// Delete employee, P: PS_EmpInfo
	EVENT_EMP_MOD,						// Modify employee, P: PS_EmpInfo
	EVENT_EMP_MODS,						// Modify status employee, P: PS_EmpInfo
										
	EVENT_RELOAD_CONTACTS,				// Reload contacts, P: 0-Clear local contacts, and redownload it; 1-Only reload
	EVENT_CLEAR_EMPS_BEFORE,			// Before clear employees, P: NULL
	EVENT_CLEAR_EMPS_AFTER,				// After clear employees, P: NULL
								
	EVENT_CLEAR_DEPTS_BEFORE,			// Before clear departments, P: NULL
	EVENT_CLEAR_DEPTS_AFTER,			// After clear departments, P: NULL
	EVENT_LOAD_CONTS_END,				// Load contacts finished, P:NULL
//	EVENT_INIT_DATABASE,				// Initial database, P:0: global database,else is eid to initial
										
	EVENT_FILE_TRANSFER,				// File transfer, p: PS_FileTransInfo;
	EVENT_FILE_TRANSFER_ACTION,			// File transfer, p: PS_FileTransInfo;
	EVENT_ACTIVE_SESSION_MGR_WND,		// Active session manager window when exist session, P:NULL
	
	EVENT_URL_FILE_TRANSFER_ACTION,
	EVENT_URL_FILE_TRAN_RES,
	EVENT_SEND_RECALL_MSG_ACK,

	EVENT_SHOW_SYS_SETTING,				// Show setting window, P: SETTING_* of tab in "IeIMUISetting.h"
	EVENT_SERVERS_CHANGED,				// Server setting changed, P(Bit flags), see fellow SERVERS_TYPE_*
	EVENT_RTX_TOOL,						// Open RTX Tool
# define SERVERS_TYPE_IM				( 0x00000001 )  // IMSever
# define SERVERS_TYPE_P2P				( 0x00000002 )  // P2PServer
# define SERVERS_TYPE_FILE				( 0x00000004 )  // File Server
# define SERVERS_TYPE_IMAGE				( 0x00000008 )  // Image Server
# define SERVERS_TYPE_HEAD				( 0x00000010 )	// Head Server
# define SERVERS_TYPE_MEETING           ( 0x00000020 )  // meeting
# define SERVERS_TYPE_WEBAPP            ( 0x00000040 )  // webapp
# define SERVERS_TYPE_RESET				( 0x40000000 )	// Reset server
# define SERVERS_SWITCH_FILE_SVR		( 0x80000000 )	// Enable switch file server
# define SERVERS_TYPE_ALL				( SERVERS_TYPE_IM | SERVERS_TYPE_P2P | SERVERS_TYPE_FILE | SERVERS_TYPE_IMAGE | SERVERS_TYPE_HEAD |SERVERS_TYPE_MEETING | SERVERS_TYPE_WEBAPP)
# define SERVER_SWITCH_FILE_ALL			( SERVERS_SWITCH_FILE_SVR | (SERVERS_TYPE_ALL & ~SERVERS_TYPE_IM) )
# define SERVER_RESET_ALL				( SERVERS_TYPE_RESET | SERVERS_TYPE_ALL ) 

	EVENT_HOTKEYS_CHANGED,				// Hotkeys changed, P(Bit flags), see fellow HOTKEYS_TYPE_*
# define HOTKEYS_TYPE_SENDMSG			( 0x00000001 )	// Send message hotkeys changed
# define HOTKEYS_TYPE_SCREENSHORTS		( 0x00000002 )	// Screenshots hotkeys changed
# define HOTKEYS_TYPE_SEARCH			( 0x00000004 )  // Search hotkeys changed
# define HOTKEYS_TYPE_PICKUP_MSG		( 0x00000008 )	// Pick up message hotkeys changed
# define HOTKEYS_TYPE_ALL				( HOTKEYS_TYPE_SENDMSG | HOTKEYS_TYPE_SCREENSHORTS | HOTKEYS_TYPE_SEARCH | HOTKEYS_TYPE_PICKUP_MSG )

	EVENT_HOTKEYS_SEND_MODE,			 // Send Mode
# define SEND_MODE_ENTER                ( 0x00000001 )  // Send message by 'Enter'
# define SEND_MODE_CTRL_ENTER           ( 0x00000002 )  // Send message by 'Ctrl + Enter'

	EVENT_MY_CHANGED,					// My info changed, P(Bit flags), see fellow MY_TYPE_*
	EVENT_MY_CHANGED_LOGO_FAILD,
# define EVENT_MY_EMAIL					( 0x00000001 )	// Email changed
# define EVENT_MY_SEX					( 0x00000002 )	// Sex changed
# define EVENT_MY_TEL					( 0x00000004 )  // Search changed
# define EVENT_MY_PHONE					( 0x00000008 )	// Tel changed		
# define EVENT_MY_HOMETEL				( 0x0000000F )	// Phone changed	
# define EVENT_MY_EMGRTEL				( 0x00000010 )	// Emergency tel changed	
# define EVENT_MY_SIGNATURE				( 0x00000020 )	// Signature changed	
# define EVENT_MY_JOB					( 0x00000040 )	// Job changed	
# define EVENT_MY_LOGO					( 0x00000080 )	//	Header logo changed

	EVENT_BASIC_CHANGED,				// System basic info changed, P(Bit flags), see fellow BASIC_TYPE_*
# define EVENT_BASIC_AUTO_PROMPT		( 0x00000001 )	// Auto prompt
# define EVENT_BASIC_AUDIO_PROMPT		( 0x00000002 )	// Audio prompt
# define EVENT_BASIC_AUTO_RUN			( 0x00000004 )  // Auto run
# define EVENT_BASIC_AUTO_LOGIN			( 0x00000008 )	// Auto login		
# define EVENT_BASIC_MAX_SESSION		( 0x0000000F )	// Max sessions	
# define EVENT_BASIC_CLOSE_PROMPT		( 0x00000010 )	// Close prompt

	EVENT_TRANS_CHANGED,				// File transfer changed, P(Bit flags), see fellow EVENT_TRANS_*
# define EVENT_TRANS_AUTO_RECEIVE		( 0x00000001 )	// Auto receive file
# define EVENT_TRANS_RCV_LOCATION		( 0x00000002 )	// Receive file location
# define EVENT_TRANS_EXCEED_PROMPT		( 0x00000004 )  // Exceed prompt
# define EVENT_TRANS_EXCEED_VALUE		( 0x00000008 )	// Exceed value

	EVENT_RESP_CHANGED,					// Response changed, P(Bit flags), see fellow EVENT_RESP_*
# define EVENT_RESP_AUTO_LEAVE			( 0x00000001 )	// AutoLeave's unit is Minute, 0 to disable
# define EVENT_RESP_AUTO_ENABLE			( 0x00000002 )	// Enable: 0 to disable, else enable
# define EVENT_RESP_CHOSEN				( 0x00000004 )  // Chosen: current used item to auto response, begin with 0
# define EVENT_RESP_LEAVE_MSG			( 0x00000008 )	// Leave msg list changed

	EVENT_PWD_CHANGED,					// Password changed
	EVENT_READ_NEW_MSG,					// When read/remove the new message
	EVENT_READ_OFF_MSG,					// When read/remove the offline message
//	EVENT_JUMP2ORG,						// Jump to organization tree and highlight, P: qeid
//	EVENT_SHOW_EMP_INFO,				// Show employee detail information, P: QEID
//	EVENT_SHOW_SINGLE_RECORDS,			// Show single session records: P: QEID
//	EVENT_SHOW_SESSION_RECORDS,			// SHOW session records: P: QSID
//	EVENT_FONT_CHANGED,					// Font changed, P: PS_MsgFont
	EVENT_CREATE_SESSION_VIEW,			// Create session, P: PS_UISessionInfo
//	EVENT_CREATE_SESSION_FILE,			// Create sessin and send file, P: VectSessionMember*, TCHAR* pszFile
	EVENT_SEND_EMAIL,					// Send eMail, P: VectSessionMember*
	EVENT_SEND_BROADCAST,				// Send broadcast, P: VectSessionMember*
	EVENT_ADD2FREQUENT_DEPT,			// Add to frequent deptartment, P: VectSessionMember*
	EVENT_ADD2FREQUENT_CONTS,			// Add to frequent contacts, P: VectSessionMember*
	EVENT_SEND_MODIFY_GROUP_MEMBER,		// Modify group members, P: PS_ModifyGroupMember
	EVENT_NEWEST_MSG,					// Modity P: group id
//	EVENT_GET_ROBOT_MENU,				// Get iRobot menu succeed, P: TCHAR* is respone file path
//	EVENT_GET_ROBOT_TOPIC,				// Get iRobot topic succeed, P: PS_RobotTopic_
//	EVENT_WEB_APP,						// Web app, PS:PS_WebApp[*], Need update iRobot\WebApp.File by IME_ATTRIB_WEBAPPS value 

	EVENT_SET_TWINKLING,				// Set Twinkling
	EVENT_CREATE_SMSDLG,				// Create sms dialog P:*vector<S_SmsRecver>
//	EVENT_OPEN_SESSION_WND,				// Open session wnd event P: PQsid
//	EVENT_CLOSE_SESSION_WND,			// Close session wnd event P: PQsid
	EVENT_FRESH_CONTACT_TREE,			// Fresh contact tree P:NULL
//	EVENT_FRESH_ORGANIZATION_TREE,		// Fresh organization tree P:NULL
//	EVENT_DROP_EMP,						// Drop emp P: PS_TreeItemDropInfo
//	EVENT_CUT_SCREEN,					   
	EVENT_DOWNLOAD_EMP_HEAD_IMG,		// Download emp head P:QEID
	EVENT_UPDATE_HEAD_IMG,				// Update head P:QEID
	EVENT_UPDATE_SESSION_LOGO,			// Update session logo， P: PQsid
	EVENT_UPDATE_CONCERN_EMP,			// Update concern emp . P:QEID
	EVETN_CONCERN_PROMPT,				// Concern emp state . P:QEID
	EVENT_SET_ONLINE,					// Set online P:NULL
	EVENT_SET_OFFLINE,					// Set offline P:NULL
	EVENT_RECV_OFFLINE_MSG_DELAY,		// Offline delay  
	EVENT_CREATE_SMS_REPLY_WND,			// Create Sms Reply window P:PS_SmsRecInfo
	EVENT_SMS_SUBMIT_RLT,				// SMS to submit results
	EVENT_SMS_RECV_MSG,					// P:vector<S_SmsNotify>*
	EVENT_SMS_RECV_REPLY,				// P:S_SmsReply
	EVENT_CREATE_BRO_DLG,				// Create BRO window P:vector<S_BroRecver>*
	EVENT_DELETE_OFF_SESSION_MSG,		// Delete session off message event P: PQsid
	EVENT_DELETE_NEW_SESSION_MSG,		// Delete session new message event P: PQsid
	EVENT_CREATE_BRO_REPLY_DLG,			// Create BRO reply window P:UINT64*
	EVENT_BRO_NOTICE,					// BRO notice  P:PS_BroNotice
	EVENT_BRO_RLT,						// BRO notice  P:PS_BroNotice
	EVENT_ADD_DEF_GROUP,				// Add define group P:PS_AddDefineGroupInfo
	EVENT_UI_WAIT_OVERTIME,				// UI wait overtime P:Cmd
	EVENT_UI_WAIT_SUCCESS,				// UI wait success P:Cmd
	EVENT_SESSION_SHIELD,				// Shield session P:PQsid
	EVENT_ALERT_NOTICE,					// ALERT notice  use P:PS_BroNotice
	EVENT_CREATE_1TO100_WND,			// Create 1 to hundred window
	EVENT_SEND_1TO100_NOTICE,  
	EVENT_1TO100_READ_NOTICE,
	EVENT_RECEIPT_NOTICE,
	EVENT_CREATE_RECEIPT_WND,			// Create receipt list window
	EVENT_REPLACE_1TO100_TEXT,			// Replace 1 to 100 text, P:PS_1To100Text
	EVENT_REPLACE_WRITEDB_OK,
	EVENT_NEWMSG,

	//EVENT_MUTILMEDIALIST,
	//EVENT_MUTILMEDIALISTCLOSE,
	EVENT_VIDEO_REFURBISH,
	EVENT_VOICE_TO_TEXT,				// Voice to text, PS_VoiceToText
	EVENT_CHANGE_SESSION_TITLE,			// iRobot/Group SessionWnd'Title change
	//EVENT_SEND_ROBOT_TOPIC,				// 每天第一次登录发送小万主题, P:PS_RobotTopic
	//EVENT_MAINUI_ROBOT_ICON,
	EVENT_CHANGE_SESSION_STATE,
	EVENT_FILE_SAVEAS,
	EVENT_FILE_SAVEAS_RE,
	EVENT_UPDATE_COLLECT_WND,
	EVENT_SEND_MSG_FAILED,
	EVENT_FILE_RECEIVE_FINISH,
	//EVENT_P2P_NOTICE_CONFIRM,			// P2p Notice confirm, P:Cmd
	//EVENT_RDP_NOTICE_CONFIRM,
	//EVENT_ClOSE_RDP_NOTICEDLG,
	//EVENT_CLOUD_DISK,					// CloudDisk, P: PS_CloudDisk
	
	EVENT_CREATE_MEETING_BY_SESSION,		// Create meeting by session, P:PS_UISessionInfo
	//EVENT_SHOW_VOIP	,					// Show VOIP, P:NULL(no param yet)
	//EVENT_SHOW_MEETINGINITIATE,			// show meetinginitaate
	//EVENT_DOWNLOAD_EMP_HEAD_SMALL_IMG,	// download small head image
	//EVENT_CERATE_MEETING,				// createmeeting
	//EVENT_CREATE_MEETING_SUCCESS,		// create meeting success
	//EVENT_JOIN_MEETING,					// join meeting
	//EVENT_GET_LAUNCHSTRING,				// get launchString
	//EVENT_JOIN_MEETING_MAINUI,			// join meeting by mainui
	//EVENT_GET_LAUNCHSTRING_MAINUI,		// get pcode from mainui
	//EVENT_OPENJOINLOADINGWND,			// open joinmeeting loading
	//EVENT_CLOSEJOINLOADINGWND,			// close joinmeeting loading
	//EVENT_OPENMEETINGDETAIL,			// open meeting detail 
	//EVENT_CLOSEMEETINGDETAIL,			// close meeting detail
	//EVENT_MEETING_CANCEL,				// meeting cancel
	//EVENT_CREATE_MEETING_FAILD,			// create meeting success
	//EVENT_UPLOADMEETINGFILE,			// upload meeting file 
	//EVENT_GETCONFINFOSUCCESS,			// get confinfo success
	//EVENT_CHANGEMEETINGMEMBER,			// change meeting member
	//EVENT_DELETEMEETING,				// delete meeting
	//EVENT_MEETINGLIST_CLICKINFO,		// delete meeting
	//EVENT_OPENMEETINGCREATEDETAIL,		// opem meeting create detail
	//EVENT_CHANGE_MEETINGLEVEL,			// change meeting level
	//EVENT_ADD2COMMON_GROUP,				// Add to Common Group, P:&qsid
	//EVENT_CANCELMEETINGFAILD,			// cancel meeting FAILD
	//EVENT_CHANGEMEETINGBASICFAILD,		// change meeting basicinfo faild
	//EVENT_CHANGEMEETINGBASICSUCCESS,	// change meeting basicinfo success
	//EVENT_REGETMEETINGINFO,				// re get meeting info
	//EVENT_RECEIVEMEETING,				// receive meeting
	//EVENT_MEETINGREAD,					// meeting read
	//EVENT_MEETINGREMOVEFILE,			// meeting remove file
	//EVENT_MEETINGMEMBER_CHANGEFAILD,	// change meeting member faild
	//EVENT_MEETINGMEMBER_CHANGESUCCESS,	// change meeting member success
	//EVENT_MEETINGFILE_CHANGEFAILD,		// change meeting member faild
	//EVENT_MEETINGFILE_CHANGESUCCESS,	// change meeting member success
	//EVENT_FILEISEXIST,					// file is exist
	//EVENT_CREATEMEETINGBYMEMBER,		// create meeting by members
	//EVENT_MEETING_GETVIDEOURL,			// get refresh videourl
	//EVENT_MEETING_REFRESHVIDEOURL,		// refresh videourl 
	//EVENT_MEETING_REFRESHSHAREDOCURL,   // refresh sharedoc url
	//EVENT_MEETING_GETLAUNCHFAILD,		// GETLAUNCH FAILD
	//EVENT_MEETING_ACCOUNT_ACK,             
	//EVENT_MEETING_INFO_NOTICE,      
	//EVENT_MEETING_MBRINFO_NOTICE,   
	//EVENT_MEETING_FILEINFO_NOTICE,  
	//EVENT_MEETING_SENDTO_NOTICE,	
	//EVENT_MEETING_ACCEPT_NOTICE,	
	//EVENT_MEETING_MENU_INFO,		
	//EVENT_MEETING_REPLY_NOTICE,	
	//EVENT_MEETING_CHOOSE_MEETING,
	//EVENT_MEETING_REMARK_NOTICE,    
	//EVENT_MEETING_SHOWMORE_NOTICE,  
	//EVENT_JOIN_MEETING_BY_TAHOE,  
	//EVENT_GET_MEETINGROOM_LIST,  
	//EVENT_GET_MEETINGROOM_LIST_SUCCESS,		
	//EVENT_GET_MEETINGROOM_LIST_FAILD,	
	
	EVENT_LOADMAPIMAGE,					//地图图片

	//EVENT_OPEN_SSO_URL,				// P: TCHAR* pszUrl
	EVENT_TRANSLATE,					// Translator, P: PS_TranslateInfo
	EVENT_SAVEAUTOSTATES,               // AUTO STATE SAVE
	EVENT_UPDATED_MSG_RECORD,			// 消息记录更新了：P: PS_UpdatedMsgRecord
	EVENT_QUOTE_MSGID,					//引用回复的ID
	EVENT_DELCOLLECTMSG,
};

enum tagEIMUserEvent
{	//下面是第三方用户定义事件的开始///////////////////////////////////////////
	EVENT_USERS = 0x00008000,			// User define event start address

};

///////////////////////////////////////////////////////////////////////////////
#define CONNECT_IP_SIZE				( 128 )			// IP or Domain
typedef struct tagConnParam
{
	int		i32Port;					// Connect Port
	TCHAR	szIP[CONNECT_IP_SIZE];		// Connect IP or address
}S_ConnParam, *PS_ConnParam;
typedef const struct tagConnParam* PS_ConnParam_;

typedef struct tagCheckVersion
{
	int		i32Flag;			// 0: Non-Force update; 1: Force update; 2: Patch update, hightest bit is 1 to abort App when return( <0 )
	TCHAR	szUpdateURL[EIM_VERSION_URL_SIZE];			// Update URL
	TCHAR   szVerDesc[EIM_VERSION_URL_SIZE];			//ver des
	int     i32DeltaUpgrade;							//是否增量：1字节，1全量，2增量，没有为0
	union
	{
		DWORD u32Ver;			// Version
		struct
		{
			WORD	u16Rev;		// Revision version
			BYTE	u8Minor;	// Minor version
			BYTE	u8Major;	// Major version
		};
	};
}S_CheckVersion, *PS_CheckVersion;
typedef const struct tagCheckVersion* PS_CheckVersion_;

// Reference SENDMSGACK in protocol.h
typedef struct tagSendMsgRslt	
{
	DWORD	dwResult;
	UINT64	u64MsgID;
}S_SendMsgRslt, *PS_SendMsgRslt;

// EVENT_FILE_TRANSFER
#define FILE_ACTION_TO_OFFLINE	( 1 )	// Convert to offline file(Only by sender)
#define FILE_ACTION_TO_P2P		( 2 )	// Convert to p2p file(Only by sender)
#define FILE_ACTION_CANCEL		( 3 )	// Cancel send or receive
#define FILE_ACTION_REJECT		( 4 )	// Reject receive p2p file
#define FILE_ACTION_RECV		( 5 )	// Receive file
#define FILE_ACTION_RERECV		( 6 )	// Re-Receive file(Only offline file)
#define FILE_ACTION_UNREG		( 7 )	// Need unregister the event(for ImageCtrl)

#define FILE_STATUS_READY		( 0 )	// Ready
#define FILE_STATUS_START		( 1 )	// Start
#define FILE_STATUS_TRANSFERING	( 2 )	// Transfering
#define FILE_STATUS_CANCEL		( 3 )	// Cancel
#define FILE_STATUS_FINISHED	( 4 )	// Finished
#define FILE_STATUS_NO_FILE		( 5 )	// No file
#define FILE_STATUS_FAILED		( 6 )	// Failed(Read/Write/Net error)
#define FILE_STATUS_VERIFY_FAILED	( 7 )	// Verify failed

#define FILE_DIR_DOWNLOAD		( 0 )	// Download/Receive
#define FILE_DIR_UPLOAD			( 1 )	// Upload/Send

#define FILE_TRANS_TYPE_OFFLINE	( 0 )	// Offline file
#define FILE_TRANS_TYPE_P2P		( 1 )	// P2P file
#define FILE_TRANS_TYPE_CLOUD	( 3 )	// Cloud disk

#define FILE_SERVER_TYPE_FILE	( 0 )	// File server
#define FILE_SERVER_TYPE_IMAGE	( 1 )	// Image server
#define FILE_SERVER_TYPE_HEAD	( 2 )	// Emp Head server
#define FILE_SERVER_TYPE_HEADS	( 3 )	// Emp Small Head server

#define HEAD_IMAGE_BIG          ( 0 )   //BIG HEAD IMAGE
#define HEAD_IMAGE_SMALL        ( 1 )   //SMALL HEAD IMAGE

#define FILE_KEY_MAX_LEN	40
typedef struct tagFileTransInfo
{
	INT64	i64FileSize;		// File size(bytes)
	INT64	i64TransSize;		// Transfered size(bytes)
	UINT64	u64Fid;				// File identificatio 
	UINT64	u64FileId;			// Cloud's file_id, for preview
	DWORD	bitDirect:1;		// Direction, see FILE_DIR_*
	DWORD	bitIsImage:2;		// Direction, see FILE_SERVER_TYPE_*
	DWORD	bitIsModify:1;		// 0: Not modify the file path, else is modified.
	DWORD   bitSaveStatus:1;		// 1: save status
	DWORD	bitType:4;			// Transfer type, see: FILE_TRANS_TYPE_*
	DWORD	bitStatus:4;		// Transfer status[Engine==>UI], see: FILE_STATUS_*
	DWORD	bitAction:4;		// Transfer action[UI==>Engine], see: FILE_ACTION_*
	TCHAR	szFile[MAX_PATH];	// Local file path
	TCHAR	szKey[FILE_KEY_MAX_LEN + 1];	// file key
	DWORD	u32Eid;				// Emp Id
	DWORD	u32LoginEmpId;		// Login emp id
	DWORD   dwTimestamp;		// 时间戳（GetTickCount()）
}S_FileTransInfo, *PS_FileTransInfo;
typedef const tagFileTransInfo* PS_FileTransInfo_;

typedef struct tUrlFileTranInfo
{
    DWORD	bitAction:4;			// Action, see: FILE_ACTION_*
    TCHAR	szUrlFile[MAX_PATH];	// Local file path
    TCHAR	szSaveFile[MAX_PATH];	// Local file path
}T_UrlFileTranInfo;

typedef enum URL_FILE_DOWN_STATUS
{
    URL_DOWN_OK = 0,				// Download succeeded
    URL_DOWN_FAIL,					// Download failed
    URL_DOWN_DOWNING,				// Downloading
}eUrlDownStatus;

typedef struct tUrlFileTranRes
{
    eUrlDownStatus status;			// Status
    int  i32Progress;				// Progress
    TCHAR	szUrlFile[MAX_PATH];	// URL
    TCHAR	szSavePath[MAX_PATH];	// Local path
}T_UrlFileTranRes;

/*联系人操作结构*********************************/
typedef enum tagCustOpreType
{
	//联系人项
	eAdd,			// Add
	eDel,			// Delete
	eUpd,			// Update

	//联系人项人员
	eAddMember,		// Add 
	eDelMember,		// Delete
	eUpdMember,		// Update
}E_CustOperType,*PS_CustOperType;

//联系人操作数据结构
typedef struct tagCustomizeInfo
{
	DWORD	dwCustomizeId;							//最近联系人项ID
	int		iGroupId;								//组id
	int		iOperateType;							//操作类型
	DWORD	dwIdNum;								//成员Id数
	DWORD	adwId[EIM_CUSTOM_GTOUP_MEMBER_MAX];		//成员ID
	TCHAR	aszName[EIM_CUSTOM_GTOUP_NAME];			//组名
}S_CustomizeInfo,*PS_CustomizeInfo;

//#define TPE_WM_SIZE			0x00000001		// OnSize, Param: PS_SIZE
//typedef struct tagWMSize{
//	HWND	hFromWnd;
//	SIZE	sz;
//	UINT	type;
//}S_WM_SIZE, *PS_WM_SIZE;

//#define TPE_WM_CLOSE				0x00000002		// OnClose, Param: hFromWnd
//#define TPE_ABC_FILE_LIST			0x00000003		// Send list file path by ABC， receive by FileMerger
//#define TPE_MERGED_FILE			0x00000004		// Send merged file path by FileMerger，receive by SPIFlashWriter 
//#define TPE_HIDE2TASKBAR			0x00000005		// Hide main window to taskbar
//#define TPE_SET_FOCUS				0x00000006		// Set focus plugin window, Param: I_TPlugin*
//#define TPE_QUERY_MANAGER_STATUS	0x00000007		// Query manager status, Param: Status
//#define TPE_AFTER_INIT			0x00000008		// Init plugin finished, Param: Null	
//#define TPE_OPEN_EQUPROJECT       0x00000009      // Single line added by yongliu on 2012-9-24 18:09:11

typedef struct tagModifyGroupMember
{
	QSID	qsid;					// Session ID
	QEID	qeid;					// Modifer, optional
	DWORD	dwTimestamp;			// Modify timestamp, optional
	VectSessionMember* pvectAdd;	// To add member
	VectSessionMember* pvectDel;	// To delete member
}S_ModifyGroupMember, *PS_ModifyGroupMember;

typedef enum tagEnumSubscribeType
{
	eStatusGet,				// 状态拉取
	eStatusSubscribe,		// 状态订阅
}E_SubscribeType,PE_SubscribeType;

typedef enum tagEnumOper
{
	eOperDel=0,		// Add
	eOperAdd=1,		// Delete
	
}E_SubscribeOper,PE_SubscribeOper;

typedef struct tagSubscribeAction
{
	E_SubscribeType		eType;			// Type
	E_SubscribeOper		eOper;			// Operate
	int					iItemType;		//ITEMTYPE_*
	UINT64				u64Id;			//qeid OR qdid OR qsid
	DWORD				u32QeidNum;		//The number of qeid
//	PQEID				pQeids;			//Qeid arr
}S_SubscribeAction,*PS_SubscribeAction;

typedef struct tagSubscribeDBData
{
	BOOL bDelete;					// Is delete
	QEID eid_;						// 用户ID
	int	 createtime;				// 创建时间
	int  updatetime;				// 更新时间
	int  count;						// 引用计数
}S_SubscribeDBData, *PS_SubscribeDBData;

typedef struct tagDropInfo
{
	QSID	qsid;					//拖入的窗口会话ID
	QEID	qeid;					//拖入的人员Id
}S_TreeItemDropInfo,*PS_TreeItemDropInfo;

#define CREATE_SESSION_TYPE_QSID	( 1 )	// By QSID
#define CREATE_SESSION_TYPE_WNDINFO	( 2 )	// By PS_SessionWndInfo
#define CREATE_SESSION_TYPE_MEMBERS	( 3 )	// By VectSessionMember
//typedef struct tagCreateSession
//{
//	DWORD dwType;					// Type
//	union
//	{
//		QSID  qsid;					// Session ID
//		void* pSWndInfo;			// PS_SessionWndInfo
//		VectSessionMember* pvectSM;	// VectSessionMember
//
//	};
//}S_CreateSession, *PS_CreateSession;

typedef struct tagEmpHeadFlag
{
	QEID	qeid;			// Employee id
	DWORD	dwDownloadFlag; // See S_EmpInfo:dwLogoTime 
}S_EmpHeadFlag,*PS_EmpHeadFlag;

#define TEXT_LEN_1TO100	( 40 )
typedef struct tag1To100Text
{
	QMID qmid;						// Message Id
	TCHAR szText[TEXT_LEN_1TO100];	// Text
}S_1To100Text, *PS_1To100Text;
typedef const tag1To100Text* PS_1To100Text_; 

// eg:{"topic":"天气","statusCode":200}
typedef struct tagRobotTopic
{
	DWORD dwStatusCode;		// Status code
	QEID  qeidRobot;		// Robot qeid
	const TCHAR* pszTopic;	// Topic content
	const TCHAR* pszFile;	// Topic file
}S_RobotTopic, *PS_RobotTopic;
typedef const tagRobotTopic* PS_RobotTopic_; 

// WebApp =====================================================================
#define WEBAPP_CONFIG_MAX	( 5 )
typedef struct tagWebAppConfig
{		
	DWORD	dwAppId;				// 100, 
	DWORD	dwCheckTime;			// 300,
	DWORD	dwCompId;				// 5,
	BOOL	bRedNotice;				// 1,
	DWORD	dwSort;					// 排序号
	int		i32UpdateTime;			// 更新时间戳，当本地小于服务器时更新。
	DWORD	dwUpdateType;			// 1,				
	QEID	qeid;					// 员工ID
	const TCHAR* pszAppName;		// 待审批
	const TCHAR* pszAUrl;			// A Interface URL
	const TCHAR* pszDefaultPage;	// "http://ctx.wanda.cn/apperror/unknow_notice_app0.html"
	const TCHAR* pszLogoPath;		// "http://124.238.218.217:8090/USCService/organization.png",
	const TCHAR* pszErrorUnknownUrl;// "http://ctx.wanda.cn/apperror/vpn_notice.html",
	const TCHAR* pszErrorTimeoutA;	// "http://ctx.wanda.cn/apperror/timeout_notice.html",
	const TCHAR* pszErrorTouchA;	// "http://ctx.wanda.cn/apperror/error_notice.html",
	const TCHAR* pszErrorReturnA;	// "http://errorurl?errorcode=[errorcode]&errormessage=[errormessage]",
	const TCHAR* pszErrorOffline;	// "http://ctx.wanda.cn/apperror/offline_notice.html",
	const TCHAR* pszUndoListHomePage;// For Taheo webapp
}S_WebAppConfig, *PS_WebAppConfig;	// 
typedef const tagWebAppConfig* PS_WebAppConfig_; 

typedef struct tagWebAppARet
{
	DWORD	dwAppId;				// AppID
	DWORD	dwResult;				// Result
	BOOL	bRefreshA;				// 是否需要刷新A接口
	time_t	tmTimestamp;			// 当次刷新的时间戳

	DWORD	dwUndoNum;				// 待办数量P1（单位：个）
	DWORD	dwOutHealth;			// 刷新过载保护P6（单位：秒），上次刷新到本次刷新的时间间隔如果小于P5，则不请求A接口，不刷新B页面
	BOOL	bRefreshUndoList;		// 是否立即刷新待办列表B页面P2（单位：0？1）
	int		i32RefreshTimeInterval;	// 定时刷新时间间隔P3（单位：秒）, -1: Disable refresh
	BOOL	bActiveFefreshFlag;		// 是否进行激活刷新P4，标示PC客户端由非激活态->激活态（单位：0?1）
	BOOL	bNotNavigateUrl;		// 不导航到URL，主要用于由引擎发起的请求，以减少服务器SSO压力
	const TCHAR* pszUrl;			// 待办页面B的地址P5 (undolisthomepage:http://m.wanda.cn/wx/ctxHomePageWaitTodo.do)
}S_WebAppARet, *PS_WebAppARet;
typedef const tagWebAppARet* PS_WebAppARet_; 

//<?xml version="1.0" encoding="UTF-8"?><WebApps><WebApp  errorurl="http://ctx.wanda.cn/apperror/error_notice.html" errormessage="用户名或密码有误" errorcode="1"/></WebApps>
#define ERROR_URL_SIZE	( 2048 )
typedef struct tagWebAppARetError
{
	int		i32ErrorCode;				// 错误码
	DWORD	dwAppId;					// AppId
	TCHAR	szErrorUrl[ERROR_URL_SIZE];	// 错误URL
}S_WebAppARetError, *PS_WebAppARetError;
typedef const tagWebAppARetError* PS_WebAppARetError_; 

typedef struct tagWebAppUrl
{
	SIZE	sizeWindow;				// Window size
	TCHAR	szUrl[ERROR_URL_SIZE];	// URL
}S_WebAppUrl, *PS_WebAppUrl;
typedef const tagWebAppUrl* PS_WebAppUrl_;

typedef struct tagWebAppNotice
{
	DWORD dwAppId;					// AppId
	DWORD dwUndoNum;				// 待办条数
}S_WebAppNotice, *PS_WebAppNotice;
typedef const tagWebAppNotice* PS_WebAppNotice_; 

typedef struct tagWebAppGetNum
{
	DWORD dwAppId;					// AppId
	TCHAR szUrl[ERROR_URL_SIZE];	// URL
}S_WebAppGetNum, *PS_WebAppGetNum;
typedef const tagWebAppGetNum* PS_WebAppGetNum_; 


typedef struct tagHttpGet
{
	bool bPost;
	unsigned long long exID;
	tagEIMEvent receiveEvent;		//接收人的事件
	CHAR szUrl[ERROR_URL_SIZE];		// URL

	CHAR postBuffer[512];
	
} S_HttpGet;
typedef const tagHttpGet* PS_HttpGet_; 

typedef struct tagHttpDownload
{
	unsigned long long exID;
	TCHAR exStr[MAX_PATH];
	tagEIMEvent receiveEvent;		//接收人的事件
	CHAR szUrl[ERROR_URL_SIZE];		// URL
	TCHAR	szSavePath[MAX_PATH];	// Local path

} S_HttpDownload;
typedef const tagHttpDownload* PS_HttpDownload_; 


typedef struct tagHttpDownloadData
{
	unsigned long long exID;
	TCHAR exStr[MAX_PATH];
	tagEIMEvent receiveEvent;		//接收人的事件
	CHAR szUrl[ERROR_URL_SIZE];		// URL
	TCHAR	szSavePath[MAX_PATH];	// Local path

} S_HttpDownloadData;

typedef const tagHttpDownloadData* PS_HttpDownloadData_; 

typedef struct tagAsynLoadImage
{
	unsigned long long exID;
	TCHAR exStr[MAX_PATH];
	tagEIMEvent receiveEvent;		//接收人的事件
	TCHAR	szLoadPath[MAX_PATH];	// Local path

} S_AsynLoadImage;
typedef const tagAsynLoadImage* PS_AsynLoadImage_; 

typedef struct tagAsynLoadImageData
{
	unsigned long long exID;
	TCHAR exStr[MAX_PATH];
	TCHAR	szLoadPath[MAX_PATH];	// Local path
	void *pCImage ;					//CImage 

} S_AsynLoadImageData;
typedef const tagAsynLoadImageData* PS_AsynLoadImageData_; 


typedef struct tagHttpDownloadMapImage
{
	unsigned long long exID;
	TCHAR exStr[MAX_PATH];
	tagEIMEvent receiveEvent;		//接收人的事件
	CHAR szUrl[ERROR_URL_SIZE];		// URL
	TCHAR	szSavePath[MAX_PATH];	// Local path

} S_HttpDownloadMapImage;
typedef const tagHttpDownloadMapImage* PS_HttpDownloadMapImage_; 

typedef struct tagHttpDownloadMapImageData
{
	unsigned long long exID;
	TCHAR exStr[MAX_PATH];
	TCHAR	szLoadPath[MAX_PATH];	// Local path

} S_HttpDownloadMapImageData;
typedef const tagHttpDownloadMapImageData* PS_HttpDownloadMapImageData_; 

typedef enum tagWebAppType
{
	eWEBAPP_CONFIG = 0,				// 登录时下载的应用配置列表：S_WebAppConfig
	eWEBAPP_ARET,					// 通过配置中的A接口，获取到的结果：S_WebAppARet
	eWEBAPP_NOTICE,					// 通过广播下发的红点通知, BROADCASTNOTICE.cMsgType == 0 && BROADCASTNOTICE.cAllReply == 3
	eWEBAPP_REFRESH,				// WebApp 页面中的刷新A接口， 
	eWEBAPP_ARET_ERROR,				// A接口错误信息
	eWEBAPP_OPEN_URL,				// 打开URL
	eWEBAPP_GET_NUM,				// Get Number: PS_WebAppGetNum
	eWEBAPP_RET_NUM,				// Return Number: PS_WebAppNotice
}E_WebAppType, *PE_WebAppType;

typedef struct tagWebApps
{
	E_WebAppType eType;				// 数据类型
	DWORD		 dwCount;			// 数据笔数
	union
	{
		DWORD dwAppID;					// eWEBAPP_REFRESH
		PS_WebAppConfig psConfig;		// eWEBAPP_CONFIG
		PS_WebAppARet	psARet;			// eWEBAPP_ARET
		PS_WebAppNotice psNotice;		// eWEBAPP_NOTICE, eWEBAPP_RET_NUM
		PS_WebAppARetError	psARetErr;	// eWEBAPP_ARET_ERROR
		PS_WebAppUrl		psUrl;		// eWEBAPP_OPEN_URL
		PS_WebAppGetNum		psGetNum;	// eWEBAPP_GET_NUM
	};
}S_WebApps, *PS_WebApps;
typedef const tagWebApps* PS_WebApps_; 

typedef struct tagHttpGetData
{
	unsigned long long exID;
	std::string strData;
}S_HttpGetData, *PS_HttpGetData;
typedef const tagHttpGetData* PS_HttpGetData_; 

typedef struct tagSessionCounter
{
	QSID qsid;						// Session ID 
	DWORD dwNewCounter;				// New message counter, UINT_MAX flag for not changed
	DWORD dwOfflineCounter;			// Offline message counter, UINT_MAX flag for not changed
}S_SessionCounter, *PS_SessionCounter;
typedef const tagSessionCounter* PS_SessionCounter_; 

typedef enum tagMeetingInfoType
{
	TYPE_ADD_MEETING = 1,
	TYPE_MODIF_MEETING = 2,
	TYPE_DEL_MEETING = 3,
};

typedef struct tagMeetingSendtoInfo
{
	HWND	hParentHwnd;			// Handle of parent window
	char	szConfid[CONFID_LENGTH];// Conference id
}S_MeetingSendtoInfo, *PS_MeetingSendtoInfo;

typedef struct tagMeetingMenuInfo
{
	bool	bIsAccept;					// Accept flag
	int		Conftype;					// Conference type
	char	szConfid[CONFID_LENGTH];	// Conference id
	TCHAR	szConfTitle[256];			// Conference title
	LONGLONG	StartTime;				//会议开始时间，用于判断会议是否开始
	LONGLONG	EndTime;				//会议结束时间，用于判断会议是否结束
}S_MeetingMenuInfo, *PS_MeetingMenuInfo;


typedef struct tagCreateMeeting
{
	QEID ceratorId;					// Creater id
	union
	{
		VectSessionMember* pvectSM;	// VectSessionMember
	};
}S_CreateMeeting, *PS_CreateMeeting;

typedef enum tagCloudDiskType
{
	eCLOUD_DISK_HOME = 0,		// 打开个人网盘
	eCLOUD_DISK_OAUTH,			// 通过WEBVIEW访问，弹出亿方云授权界面
	eCLOUD_DISK_CODE,			// 取code
	eCLOUD_DISK_UPLOAD_FILE,	// 上传文件
	eCLOUD_DISK_DOWNLOAD,		// 下载文件
	eCLOUD_DISK_PREVIEW,		// 预览
	eCLOUD_DISK_PREVIEW_IN_WEB,	// 嵌入web页面预览
	eCLOUD_DISK_SAVEAS,			// 远端转存
	eCLOUD_DISK_GET_TOKEN,		// 取用户token，看是否存在，如果不存在，就需要弹出授权页
	eCLOUD_DISK_FILEPINFO,		// 根据file_id、file_name取文件信息
}E_CloudDiskType, PE_CloudDiskType;
typedef struct tagCloudDisk
{
	E_CloudDiskType eType;			// Type
	int		  i32Percent;			// Transfer percent
	uintptr_t hThread;				// Thread Handle
	char	  szUrl[2048];			// Base URL
	S_FileTransInfo sFInfo;			// File info
}S_CloudDisk, *PS_CloudDisk;
typedef const struct tagCloudDisk* PS_CloudDisk_;

typedef struct tagTranslateInfo
{
	POINT	ptMouse;				// Cursor position
	HWND	hParent;				// Parent handle
	LPCTSTR	lpszText;				// Text to translate
}S_TranslateInfo, *PS_TranslateInfo;
typedef const struct tagTranslateInfo* PS_TranslateInfo_;

typedef struct tagLoginStatus
{
	DWORD dwStatus;
	TCHAR szStatus[512];
}S_LoginStatus, *PS_LoginStatus;
typedef const struct tagLoginStatus* PS_LoginStatus_;

typedef struct tagUpdatedMsgRecord
{
	QSID sid;
	QMID mid;
}S_UpdatedMsgRecord, *PS_UpdatedMsgRecord;
typedef const struct tagLoginStatus* PS_UpdatedMsgRecord_;

typedef struct tagQuoteInfo
{
	char	szConfid[CONFID_LENGTH];// Conference id
	unsigned long long qsid;
} S_QuoteInfo, *PS_QuoteInfo;
typedef const struct tagQuoteInfo* PS_QuoteInfo_;

typedef struct tagHttpGetInfoIn
{
	unsigned long long exID;	//扩展
	const char *pUrl;

}S_HttpGetInfoIn, *PS_HttpGetInfoIn;

#pragma pack( pop )
#endif	// __EIM_EVENT_2013_12_17_BY_YFGZ__

