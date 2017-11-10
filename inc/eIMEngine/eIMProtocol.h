// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: eIMProtocol define the const and struct 
//   used reference config file "Protocol.config"
//
// Reference API:
//  I_EIMEngine::GetCmd(...)
//  I_EIMEngine::GetField(...)
//  I_EIMEngine::InRange(...)
// 
// Auth: yfgz
// Date:     2013/12/23 14:41:34 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_PROTOCOL_HEADER_2013_12_23_YFGZ__
#define __EIMENGINE_PROTOCOL_HEADER_2013_12_23_YFGZ__

#include <tchar.h>

#define FONT_ID_DEFAULT				( 3 )
#define FONT_SIZE_DEFAULT			( 11 )
#define FONT_MODE_DEFAULT			( 0 )
#define FONT_ID_DEFAULT_S			( _T("3") )
#define FONT_SIZE_DEFAULT_S		( _T("11") )
#define FONT_MODE_DEFAULT_S		( _T("0") )

#pragma region CmdKeywords
///////////////////////////////////////////////////////////////////////////////
// *** The define changed to NON-Unicode, if you want use as Unicode, can be as _T(PROTOCOL_ELEMENT_ROOT)
// Protocol.config file element and attribute 
// Element name 
#define PROTOCOL_ELEMENT_ROOT			  "eIMProtocol"	// Root 
#define PROTOCOL_ELEMENT_CMD			  "Cmd"			// Command
#define PROTOCOL_ELEMENT_GROUP	    "Group"			// Group of define for reuse
#define PROTOCOL_ELEMENT_FIELD	    "Field"			// Media-item of command
#define PROTOCOL_ELEMENT_REF			  "Ref"			// Media-item of command reference to group
  
// Element attributes 
#define PTOTOCOL_ATTRIB_VERSION	    "Version"		// Protocol config file version
#define PROTOCOL_ATTRIB_NAME			  "Name"			// Name
#define PROTOCOL_ATTRIB_ID				  "Id"			// Id
#define PROTOCOL_ATTRIB_ALIAS			  "Alias"			// Alias
#define PROTOCOL_ATTRIB_UNIT			  "Unit"			// Unit, map to E_PFieldType
#define PROTOCOL_ATTRIB_MIN				  "Min"			// Min value
#define PROTOCOL_ATTRIB_MAX				  "Max"			// Max value

// Field name value defined order by ASICC
#define FIELD_BUSINESS_ID						"BusinessId"
#define FIELD_ACK						        "Ack"
#define FIELD_ADDRESS							"Address"
#define FIELD_APPID								"AppId"
#define FIELD_AREA_ID 							"AreaId"
#define FIELD_AURL								"AUrl"
#define FIELD_AT								"At"					// 是否用户已经点了At
#define FIELD_BEGIN_TIME 						"BeginTime"
#define FIELD_BIRTHDAY							"Birthday"
#define FIELD_CHECKTIME							"CheckTime"
//Del #define FIELD_CREATER_ID					"CreaterId"				//Single line deleted by LongWQ on 2014/8/5 11:32:28, for: use "UserId"
#define FIELD_CO_ID								"CoId"
#define FIELD_CO_CODE							"CoCode"
#define FIELD_COLOR								"Color"
#define FIELD_CO_NAME 							"CoName"
#define FIELD_COUNT								"Count"
#define FIELD_CO_UPDATE_TIME 	 				"CoUpdateTime"
#define FIELD_DATA								"Data"
#define FIELD_DEPT_ID							"DeptId"
#define FIELD_DEPT_NAME							"DeptName"
#define FIELD_DEPT_TEL 							"DetpTel"
#define FIELD_DEPT_EMP_UPDATE_TIME				"DeptEmpUpdateTime"
#define FIELD_DEPT_UPDATE_TIME 					"DeptUpdateTime"
#define FIELD_DEPT_USER_UPDATE_TIME				"DeptUserUpdateTime"
#define FIELD_DETAIL							"Detail"
#define FIELD_DEVICE_TOKEN						"DeviceToken"
#define FIELD_DOWNLOAD_TYPE						"DownloadType"
#define FIELD_DOWNLOAD_PATH						"DownloadPath"
#define FIELD_DUTY								"Duty"
#define FIELD_EMAIL								"Email"
#define FIELD_EMP_UPDATE_TIME					"EmpUpdateTime"
#define FIELD_EMRG_TEL							"EmrgTel"
#define FIELD_END_TIME 							"EndTime"
#define FIELD_EST_TYME 							"EstTime"
#define FIELD_RDP_EXTRA							"Extra"
#define FIELD_FAX						        "Fax"
#define FIELD_FID						        "Fid"
#define FIELD_FILE								"File"
//#define FIELD_FILE_EXT						"FileExt"	// eg.: .jpg
#define FIELD_FILE_PSW							"FilePsw"
#define FIELD_FILE_PATH							"FilePath"
#define FIELD_FILE_ID							"file_id"
#define FIELD_GET_EMP_INFO_TYPE					"GetEmpInfoType"
#define FIELD_GROUP_ID							"GroupId"
#define FIELD_GROUP_ID2							"GroupId2"
#define FIELD_GROUP_NAME						"GroupName"
#define FIELD_GROUP_NOTE						"GroupNote"
#define FIELD_GROUP_TIME						"GroupTime"
#define FIELD_HOME_TEL							"HomeTel"
#define FIELD_HIREDATE							"Hiredate"
#define FIELD_HTTP_SESSION_ID					"HttpSessionID"
#define FIELD_ID 								"Id"
#define FIELD_INFO 								"Info"
#define FIELD_IS_DEPT							"IsDept"
#define FIELD_IS_GROUP							"IsGroup"
#define FIELD_IS_SEND							"IsSend"
#define FIELD_ITEM								"Item"
#define FIELD_KEY								"Key"
#define FIELD_LENGTH 							"Length"
#define FIELD_LOGO								"Logo"		//Old protocol
#define FIEID_LOGO_EX							"LogoEx"	//New protocol
#define FIELD_LOGIN_TYPE						"LoginType"
#define FIELD_LAST_UPDATE_TIME					"LastUpdateTime"
#define FIELD_MANUAL							"Manual"
//#define FIELD_MEMBERS_ID						"MembersId"
#define FIELD_MESSAGE							"Message"
#define FIELD_MODE								"Mode"
#define FIELD_MODIFY_TIME						"ModifyTime"
#define FIELD_MODIFY_TYPE						"ModifyType"
#define FIELD_MODIFY_ID							"ModifyId"
#define FIELD_MSGANNEX							"MsgAnnex"
#define FIELD_MSGANNEXACK						"MsgAnnexAck"
#define FIELD_MSGAUTO							"MsgAuto"
#define FIELD_MSGEMOT							"MsgEmot"
#define FIELD_MSGERROR							"MsgError"
#define FIELD_MSGFONT							"MsgFont"
#define FIELD_MSGMAP							"MsgMap"
#define FIELD_MSGUI								"MsgUI"
#define FIELD_MSGRICH							"MsgRich"
#define FIELD_MSGREPLYTO						"MsgReplyTo"
#define FIELD_MSGSECRET							"MsgSecret"
//#define FIELD_MSGGIF							"MsgGif"
#define FIELD_MSGIMAGE							"MsgImage"
#define FIELD_MSGLONG							"MsgLong"
#define FIELD_MSGLONGHEADER						"MsgText MsgLongTitle=\"1\" "
#define FIELD_MSGLONGTITLE						"MsgLongTitle"
#define FIELD_MSGP2P							"MsgP2P"
#define FIELD_MSGREDPACKET						"MsgRedPacket"
#define FIELD_MSGREDPACKETACT					"MsgRedPacketAction"
#define FIELD_MSGRDP							"MsgRdp"
#define FIELD_MSGVOICE							"MsgVoice"
#define FIELD_MSGVIDEO							"MsgVideo"
#define FIELD_MSGTEXT							"MsgText"
#define FIELD_MSGWEBAPP							"MsgWebApp"
#define FIELD_MSG						        "Msg"
#define FIELD_MSG_ID 							"MsgId"
#define FIELD_MSG_LEN							"MsgLen"
#define FIELD_MSG_SEQ							"MsgSeq"
#define FIELD_MSG_SYN_TYPE						"MsgSynType"
#define FIELD_MSG_TYPE							"MsgType"
#define FIELD_MSG_TOTAL							"MsgTotal"
#define FILED_MSG_RECALLMSGID					"RecallMsgId"
#define FIELD_MSG_BJSON							"MsgBJson"
#define FIELD_NAME 								"Name"
#define FILE_ORINAME							"OriName"
#define FIELD_NET_TYPE							"NetType"
#define FIELD_OFFICE_TEL						"OfficeTel"
#define FIELD_OFFLINE							"Offline"
#define FIELD_OFFLINE_MSG_COUNT 				"OfflineMsgCount"
#define FIELD_OFF_MSG_TOTAL						"OffMsgTotal"
#define FIELD_OFF_MSG_SEQ						"OffMsgSeq"
#define FIELD_PACKET_LENGTH						"PacketLength"
#define FIELD_PAGE_INDEX						"PageIndex"
#define FIELD_PARAMETER							"Parameter"
#define FIELD_PASSWORD							"Password"
#define FIELD_PHONE								"Phone"
#define FIELD_POSTAL_CODE						"PostalCode"
//#define FIELD_POST							"Post"				// use Duty
#define FIELD_PID						        "PId"
#define FIELD_PROF_ID							"ProfId"
#define FIELD_RANK_ID							"RankId"
#define FIELD_RECVER_ID							"RecverId"
#define FIELD_READ								"Read"
#define FIELD_REFUSE_TYPE						"RefuseType"
#define FIELD_RESULT							"Result"
//#define FIELD_RETURN							"Return"			// use result
#define FIELD_SEX						        "Sex"
#define FIELD_SENDER_ID 						"SenderId"
#define FIELD_SERIAL_NO 						"SerialNo"
#define FIELD_SCHEDULE_ID						"ScheduleID"
#define FIELD_SEND_TIME							"SendTime"
#define FIELD_SENDER_NAME						"SenderName"
#define FIELD_SERVER_TIME						"ServerTime"
#define FIELD_SIGN								"Sign"
#define FIELD_SIZE								"Size"
#define FIELD_SORT								"Sort"
#define FIELD_STATE								"State"
#define FIELD_STATUS							"Status"
#define FIELD_SUMMARY							"Summary"
#define FIELD_TEXT								"Text"
#define FIELD_TERM_TYPE							"TermType"
#define FIELD_TIME								"Time"
#define FIELD_TIMESTAMP							"Timestamp"
#define FIELD_TITLE								"Title"
#define FIELD_TOKEN								"Token"
#define FIELD_TYPE								"Type"
#define FIELD_UPDATE_TYPE						"UpdateType"
#define FIELD_USER								"User"
#define FIELD_USER_PURVIEW						"UserPurview"
#define FIELD_USER_CODE							"UserCode"
#define FIELD_USER_ID							"UserId"
#define FIELD_USERS_ID							"UsersId"
#define FIELD_USERS_COUNT						"UsersCount"
#define FIELD_USER_NAME							"UserName"
#define FIELD_USER_UPDATE_TIME					"UserUpdateTime"
#define FIELD_VIRT_GROUP_UPDATE_TIME			"VirtGroupUpdateTime"
#define CUSTOMIZE_ID							"CustomizeId"
#define	CUSTOM_GROUP_ID							"CustomGroupId"
#define CUSTOM_OPERA_TYPE						"CustomOperaType"
#define CUSTOM_MEMBERS							"CustomMembers"
#define CUSTOM_MEMNER_ID						"CustomMemberId"
#define CUSTOM_GROUP_NAME						"CustomGroupName"
#define SUBSCRIBE_OPERTYPE						"OperType"
#define SUBSCRIBE_EMPID							"EmpID"
#define SUBSCRIBE_GETSTATETYPE					"GetType"

#define SMS_MSG							        "SmsMsg"
#define SMS_RECV_PHONE							"SmsPhone"
#define FIELD_SMS_TEXT							"SmsText"
#define FIELD_SMS_WND_ID						"SmsWndID"
#define FILED_BATCH_NO							"SmsBatchNO"  //批号
#define FILED_SMS_TOTAL							"SmsTotal"	  //每批短信数量
#define FIELD_RECVER_NAME						"RecvName"
#define FIELD_RECV_UCODE						"RecvUcode"
    
#define FILED_BRO_TITLE							"Title"
#define FILED_BRO_WND_ID						"BroWndID"
#define FILED_BRO_MSG							"BroMsg"
#define FILED_BRO_MSG_TEXT						"BroText"
#define FILED_BRO_RECV							"BroRecver"
#define FILED_BRO_ISDEPT						"IsDept"

#define FILED_MSG_READ_TYPE						"ReadType"
#define FILED_MSG_FRONT							"MsgFront"
#define FILED_ALERT_MSG							"AlertMsg"
#define FILED_ALERT_MSG_TEXT					"AlertText"
  
#define FILED_IS_1_TO_HUNDRED					"Is1To100"
#define FILED_IS_RECEIVE_NUM					"ReceNum"
  
#define FILED_IS_1To10T							"Is1To10T"
#define FILE_1To10T_MSG_SRC_ID					"OneTo10TMsgSrcId"
#define FILED_IS_SELF_SEND						"IsSelfSend"
#define FIELD_COLLECT_TIME						"CollectTime"
#define FIELD_COLLECT_UPDATE_TYPE				"CollectUpdateType"

#define FILED_RECEIVEFILE_ACK					"IsFileReceiveAck"
// Robot ======================================================
#define ROBOT_RES_ROOT_BEGIN					"<robotResponse>"
#define ROBOT_RES_ROOT_END						"</robotResponse>"
#define ROBOT_BODY								"Body"
#define ROBOT_SUGGESTED_QUESTIONS_RSP			"getSuggestedQuestionsResponse"
#define ROBOT_SUGGESTED_QUESTIONS				"suggestedQuestions"
#define ROBOT_ORGINQUESTION						"orginquestion"
#define FILED_ROBOT_IS_TOPIC					"IsRobotTopic"
#define ROBOT_TOPIC_CONTENT						"topic"
#define ROBOT_TOPIC_STATUS_CODE					"statusCode"

// WebApp ======================================================
#define WEBAPP_UNDOLISTHOMEPAGE				"undolisthomepage"
#define WEBAPP_APPHOMEPAGE					"apphomepage"
//# define WEBAPP_APPHOMEPAGE_DEFAULT		"http://m.wanda.cn/ctxHomePageWaitTodo.do"
#define WEBAPP_DEFAULT_PAGE					"defaultpage"
//# define WEBAPP_DEFAULT_PAGE_URL			"http://ctx.wanda.cn/apperror/unknow_notice_app0.html"
#define WEBAPP_APPID						"appid"
#define WEBAPP_APPNAME						"appname"
#define WEBAPP_CHECKTIME					"checktime"
#define WEBAPP_COMPID						"compid"
#define WEBAPP_LOGOPATH						"logopath"
#define WEBAPP_REDNOTICE					"rednotice"
#define WEBAPP_SORT							"sort"
#define WEBAPP_TOTAL						"total"
#define WEBAPP_UPDATETIME					"updatetime"
#define WEBAPP_UPDATETYPE					"updatetype"
#define WEBAPP_USERID						"userid"

#define WEBAPP_RESULT						"result"
# define WEBAPP_DISABLE						( -200 )	// Disable webapp

#define WEBAPP_ERROR_UNKNOWURL				"error-unknowurl"	
//# define WEBAPP_ERROR_UNKNOWNURL_DEFAULT	"http://ctx.wanda.cn/apperror/vpn_notice.html"

#define WEBAPP_ERROR_TIMEOUTA				"error-timeoutA"	
//# define WEBAPP_ERROR_TIMEOUT_DEFAULT		"http://ctx.wanda.cn/apperror/timeout_notice.html"

#define WEBAPP_ERROR_TOUCHA					"error-touchA"		
//# define WEBAPP_ERROR_TOUCHA_DEFAULT		"http://ctx.wanda.cn/apperror/error_notice.html"

#define WEBAPP_ERROR_OFFLINE				"offline-error"
//# define WEBAPP_ERROR_OFFLINE_DEFAULT		"http://ctx.wanda.cn/apperror/offline_notice.html"

#define WEBAPP_ERROR_RETURNA				"error-returnA"		
# define WEBAPP_ERROR_RETURNA_DEFAULT		"?errorcode=[errorcode]&errormessage=[errormessage]"
#define WEBAPP_ERROR_CODE					"[errorcode]"
#define WEBAPP_ERROR_MSG					"[errormessage]"

#pragma endregion 

///////////////////////////////////////////////////////////////////////////////
#pragma region CmdIdDefine
// The fellow Id define MUST BE the same as "Protocol.config"
// Group Id, High bit is 1 
#define GROUP_RESULT_ID						( 0x80000001 )	// General field: 0x8000000*
#define GROUP_RETURN_ID						( 0x80000002 )

#define GROUP_SCHEDULEID_ID					( 0x80000010 )	// ID field: 0x8000001*
#define GROUP_COID_ID						( 0x80000011 )
#define GROUP_USERID_ID						( 0x80000012 )
#define GROUP_USERSID_ID					( 0x80000013 )
#define GROUP_MSGID_ID						( 0x80000014 )
#define GROUP_SENDERID_ID					( 0x80000015 )
#define GROUP_RECVERID_ID					( 0x80000016 )
#define GROUP_GROUPID_ID					( 0x80000017 )
#define GROUP_DEPTID_ID						( 0x80000018 )
#define GROUP_RANKID_ID						( 0x80000019 )
#define GROUP_PROFID_ID						( 0x8000001A )
#define GROUP_AREAID_ID						( 0x8000001B )

#define GROUP_TIMESTAMP_ID					( 0x80000021 )	// Time: 0x8000002*
#define GROUP_SEND_TIME_ID					( 0x80000022 )
#define GROUP_LAST_UPDATE_TIME_ID			( 0x80000023 )

#define GROUP_STATUS_ID						( 0x80000030 )	// Other simplify: 0x8000003* ~ 0x8000007*
#define GROUP_LOGIN_TYPE_ID					( 0x80000031 )
#define GROUP_MSG_SYN_TYPE_ID				( 0x80000032 )
#define GROUP_SERIAL_NO_ID					( 0x80000033 )
#define GROUP_GROUP_NAME_ID					( 0x80000034 )
#define GROUP_USER_NAME_ID					( 0x80000035 )
#define GROUP_USER_CODE_ID					( 0x80000036 )
#define GROUP_PASSWORD_ID					( 0x80000037 )
#define GROUP_MODIFY_TYPE_ID				( 0x80000038 )
#define GROUP_GET_EMP_INFO_TYPE_ID			( 0x80000039 )
#define GROUP_MSG_TYPE_ID					( 0x8000003A )
#define GROUP_PACKET_LENGTH_ID				( 0x8000003B )
#define GROUP_LOGO_ID						( 0x8000003C )
#define GROUP_SEX_ID						( 0x8000003D )
#define GROUP_UPDATE_TYPE_ID				( 0x8000003E )

#define GROUP_MESSAGE_ID					( 0x80000060 ) // message
#define GROUP_MSG_FONT_ID					( 0x80000061 )	
#define GROUP_MSG_TEXT_ID					( 0x80000062 )
#define GROUP_MSG_ANNEX_ID					( 0x80000063 )
#define GROUP_MSG_ANNEX_ACK_ID				( 0x80000064 )
#define GROUP_MSG_IMAGE_ID					( 0x80000065 )
#define GROUP_MSG_EMOT_ID					( 0x80000066 )
#define GROUP_MSG_VOICE_ID					( 0x80000067 )
#define GROUP_MSG_VIDEO_ID					( 0x80000068 )
#define GROUP_MSG_P2P_ID					( 0x80000069 )
#define GROUP_MSG_LONG_ID					( 0x8000006A )
#define GROUP_MSG_AUTO_ID					( 0x8000006B )
#define GROUP_MSG_UI_ID						( 0x8000006F )

#define GROUP_USER_STATE_ID					( 0x80000080 )	// Other complex: 0x8000008*
#define GROUP_EMP_PURVIEW_ID				( 0x80000081 )
#define GROUP_EMPLOYEE_ID					( 0x80000082 )
#define GROUP_BROADCAST_RECEIVER_ID			( 0x80000083 )
#define GROUP_BROADCAST_ID					( 0x80000084 )
#define GROUP_SCHEDULE_ID					( 0x80000085 )
#define GROUP_MSG_ID						( 0x80000086 )
#define GROUP_MODIFY_GROUP_MEMBER_ID		( 0x80000087 )
#define GROUP_MODIFY_GROUP_ID				( 0x80000088 )
#define GROUP_CREATE_GROUP_ID				( 0x80000089 )
#define GROUP_CO_INFO_ID					( 0x8000008A )
#define GROUP_DEPT_INFO_ID					( 0x8000008B )
#define GROUP_DEPT_USER_ID					( 0x8000008C )
#pragma endregion 

// CmdId, Range is [0x00, 0x200), CmdId also is EventId, see "inc\public\Events.h"
typedef enum tagCmdId
{
    eCMD_START		= 21,				// Start CmdId of ClientAgent's protocol CmdId
	eCMD_LOGIN		= eCMD_START,		// ( 21  ) Alias=_登录"
    eCMD_LOGIN_ACK,						// ( 22  )  Alias="登录应答" 
    eCMD_LOGOUT,						// ( 23  )  Alias="退出"
    eCMD_LOGOUT_ACK,					// ( 24  ) Alias="退出应答"
    eCMD_NOTICE_STATE,					// ( 25  ) Alias="员工在线状态变化通知"
    eCMD_NOTICE_STATE_ACK,				// ( 26  ) Alias="员工在线状态变化通知应答"
    eCMD_ALIVE,							// ( 27  ) Alias="心跳"
    eCMD_ALIVE_ACK,						// ( 28  ) Alias="心跳应答"
    eCMD_MODIFY_INFO,					// ( 29  ) Alias="修改本人资料"
    eCMD_MODIFY_INFO_ACK,				// ( 30  ) Alias="修改本人资料应答"

    eCMD_MODIFY_INFO_NOTICE,			// ( 31  ) Alias="资料修改通知"
    eCMD_MODIFY_INFO_NOTICE_ACK,		// ( 32  ) Alias="资料修改通知应答"
    eCMD_GET_CO_INFO,					// ( 33  ) Alias="获取企业信息"
    eCMD_GET_CO_INFO_ACK,				// ( 34  ) Alias="获取企业信息应答"
    eCMD_GET_CO_INFO_NOTICE,			// ( 35  ) Alias="企业信息更新通知" 
    eCMD_GET_CO_INFO_NOTICE_ACK,		// ( 36  ) Alias="企业信息更新通知应答" 
    eCMD_GET_DEPT_LIST,					// ( 37  ) Alias="获取组织构架" 
    eCMD_GET_DEPT_LIST_ACK,				// ( 38  ) Alias="获取组织构架应答" 
    eCMD_GET_DEPT_LIST_NOTICE,			// ( 39  ) Alias="组织构架更新通知" 
										               
    eCMD_GET_DEPT_LIST_NOTICE_ACK,		// ( 40  ) Alias="组织构架更新通知应答" 
    eCMD_GET_USER_LIST,					// ( 41  ) Alias="获取员工列表" 
    eCMD_GET_USER_LIST_ACK,				// ( 42  ) Alias="获取员工列表应答" 
    eCMD_GET_DEPT_USER,					// ( 43  ) Alias="获取部门员工信息" 
    eCMD_GET_DEPT_USER_ACK,				// ( 44  ) Alias="获取部门员工信息应答" 
    eCMD_GET_EMPLOYEE_INFO,				// ( 45  ) Alias="获取员工详细信息" 
    eCMD_GET_EMPLOYEE_INFO_ACK,			// ( 46  ) Alias="获取员工详细信息应答" 
    eCMD_CREATE_GROUP,					// ( 47  ) Alias="创建聊天群组" 
    eCMD_CREATE_GROUP_ACK,				// ( 48  ) Alias="创建聊天群组应答" 
    eCMD_CREATE_GROUP_NOTICE,			// ( 49  ) Alias="创建群组通知" 
                                                   
    eCMD_CREATE_GROUP_NOTICE_ACK,		// ( 50  ) Alias="创建群组通知应答" 
    eCMD_MODIFY_GROUP,					// ( 51  ) Alias="修改群组资料" 
    eCMD_MODIFY_GROUP_ACK,				// ( 52  ) Alias="修改群组资料应答" 
    eCMD_MODIFY_GROUP_NOTICE,			// ( 53  ) Alias="修改群组资料通知" 
    eCMD_MODIFY_GROUP_NOTICE_ACK,		// ( 54  ) Alias="修改群组资料通知应答" 
    eCMD_GET_GROUP_INFO,				// ( 55  ) Alias="获取群组信息" 
    eCMD_GET_GROUP_INFO_ACK,			// ( 56  ) Alias="获取群组信息应答" 
    eCMD_MODIFY_GROUP_MEMBER,			// ( 57  ) Alias="添加/删除群组成员" 
    eCMD_MODIFY_GROUP_MEMBER_ACK,		// ( 58  ) Alias="添加/删除群组成员应答" 
    eCMD_MODI_MEMBER_NOTICE,			// ( 59  ) Alias="群组成员变化通知" 
                                                   
    eCMD_MODI_MEMBER_NOTICE_ACK,		// ( 60  ) Alias="群组成员变化通知应答" 
    eCMD_SEND_MSG,						// ( 61  ) Alias="发送消息" 
    eCMD_SEND_MSG_ACK,					// ( 62  ) Alias="发送消息应答" 
    eCMD_MSG_NOTICE,					// ( 63  ) Alias="消息通知" 
    eCMD_MSG_NOTICE_ACK,				// ( 64  ) Alias="消息通知应答" 
    eCMD_GETUSERSTATE,					// ( 65  ) Alias="获取员工在线状态" 
    eCMD_GET_USER_STATE_ACK,			// ( 66  ) Alias="获取员工在线状态应答" 
    eCMD_MODIFY_EMPLOYEE,				// ( 67  ) Alias="修改多个用户资料" 
    eCMD_MODIFY_EMPLOYEE_ACK,			// ( 68  ) Alias="修改多个用户资料应答" 
    eCMD_SEND_BROADCAST,				// ( 69  ) Alias="发送广播" 
                                                   
    eCMD_SEND_BROADCAST_ACK,			// ( 70  ) Alias="发送广播应答" 
    eCMD_BROADCAST_NOTICE,				// ( 71  ) Alias="广播通知" 
    eCMD_BROADCAST_NOTICE_ACK,			// ( 72  ) Alias="广播通知应答" 
    eCMD_MSG_READ,						// ( 73  ) Alias="消息已读" 
    eCMD_MSG_READ_ACK,					// ( 74  ) Alias="消息已读应答" 
    eCMD_MSG_READ_NOTICE,				// ( 75  ) Alias="消息已读通知" 
    eCMD_MSG_READ_NOTICE_ACK,			// ( 76  ) Alias="消息已读通知应答" 
    eCMD_GET_USERS_SIMPLE_LIST,			// ( 77  ) Alias="获取员工简要信息列表" 
    eCMD_GET_USERS_SIMPLE_LIST_ACK,		// ( 78  ) Alias="获取员工简要信息列表应答" 
    eCMD_MSG_NOTICE_CONFIRM,			// ( 79  ) Alias="消息通知已接收确认" 
                                                   
    eCMD_MSG_NOTICE_CONFIRM_ACK,		// ( 80  ) Alias="消息通知已接收确认应答" 
    eCMD_REGULAR_GROUP_UPDATE_REQ,		// ( 81  ) Alias="客户端请求更新群组信息" 
    eCMD_REGULAR_GROUP_UPDATE_RSP,		// ( 82  ) Alias="客户端请求更新群组信息应答" 
    eCMD_CHECK_TIME_REQ,				// ( 83  ) Alias="服务器时间校验请求" 
    eCMD_CHECK_TIME_RSP,				// ( 84  ) Alias="服务器时间校验应答" 
    eCMD_GET_OFFLINE_REQ,				// ( 85  ) Alias="离线消息请求" 
    eCMD_GET_OFFLINE_RSP,				// ( 86  ) Alias="离线消息请求应答" 
    eCMD_REFUSE_GROUP_MSG_REQ,			// ( 87  ) Alias="停止群组推送/消息" 
    eCMD_REFUSE_GROUP_MSG_RSP,			// ( 88  ) Alias="停止群组推送/消息应答" 
    eCMD_QUIT_GROUP,					// ( 89  ) Alias="主动退群请求" 
                                                   
    eCMD_QUIT_GROUP_ACK,				// ( 90  ) Alias="主动退群请求应答" 
    eCMD_QUIT_GROUPNOTICE,				// ( 91  ) Alias="主动退群通知" 
    eCMD_QUIT_GROUPNOTICE_ACK,			// ( 92  ) Alias="主动退群通知应答" 
    eCMD_MODIFY_SELF_INFO,				// ( 93  ) Alias="本人信息变更通知联系人请求" 
    eCMD_MODIFY_SELF_INFO_ACK,			// ( 94  ) Alias="本人信息变更通知联系人请求应答" 
    eCMD_MODIFY_SELF_INFO_NOTICE,		// ( 95  ) Alias="用户信息变更通知" 
    eCMD_MODIFY_SELF_INFO_NOTICE_ACK,	// ( 96  ) Alias="用户信息变更通知应答" 
    eCMD_ECWX_SYNC_REQ,					// ( 97  ) Alias="网信客户端同步公众账号请求" 
    eCMD_ECWX_SYNC_RSP,					// ( 98  ) Alias="网信客户端同步公众账号响应" 
    eCMD_ECWX_SMSG_REQ,					// ( 99  ) Alias="网信客户端向公众账号上行消息请求" 
										               
    eCMD_ECWX_SMSG_RSP,					// ( 100 ) Alias="网信客户端向公众账号推送消息响应" 
    eCMD_ECWX_PACC_NOT,					// ( 101 ) Alias="公众平台下行消息至网信客户端通知" 
    eCMD_CREATE_SCHEDULE,				// ( 102 ) Alias="创建日程提醒" 
    eCMD_CREATE_SCHEDULE_ACK,			// ( 103 ) Alias="创建日程提醒应答" 
    eCMD_CREATE_SCHEDULE_NOTICE,		// ( 104 ) Alias="创建日程提醒通知" 
    eCMD_CREATE_SCHEDULE_NOTICE_ACK,	// ( 105 ) Alias="创建日程提醒通知应答" 
    eCMD_GET_DATA_LIST_TYPE,			// ( 106 ) Alias="获取人员信息下载方式请求" 
    eCMD_GET_DATA_LIST_TYPE_ACK,		// ( 107 ) Alias="获取人员信息下载方式请求应答" 
    eCMD_COMP_LAST_TIME_NOTICE,			// ( 108 ) Alias="服务端发起企业相关最后更新时间的通知" 

    eCMD_DELETE_SCHDULE,				// ( 109 ) 删除日程提醒 
    eCMD_DELETE_SCHDULE_ACK,			// ( 110 ) 删除日程提醒应答
    eCMD_DELETE_SCHDULE_NOTICE,			// ( 111 ) 删除日程提醒通知 
    eCMD_DELETE_SCHDULE_NOTICE_ACK,		// ( 112 ) 删除日程提醒通知应答 
    
    eCMD_GROUP_PUSH_FLAG,				// ( 113 ) ios群组消息推送 
    eCMD_GROUP_PUSH_FLAG_ACK,			// ( 114 ) ios群组消息推送修改应答 
    
    eCMD_IOS_BACKGROUND_REQ,			// ( 115 ) IOS转入后台请求 
    eCMD_IOS_BACKGROUND_ACK,			// ( 116 ) IOS转入后台应答
    
    eCMD_GET_USER_RANK_REQ,				// ( 117 ) 获取级别(员工所属)请求 
    eCMD_GET_USER_RANK_ACK,				// ( 118 ) 获取级别(员工所属)应答 
    
    eCMD_GET_USER_PROFE_REQ,			// ( 119 ) 获取业务(员工所属)请求 
    eCMD_GET_USER_PROFE_ACK,			// ( 120 ) 获取业务(员工所属)应答 
    
    eCMD_GET_USER_AREA_REQ,				// ( 121 ) 获取地域(员工所属)请求 
    eCMD_GET_USER_AREA_ACK,				// ( 122 ) 获取地域(员工所属)应答 
    
    eCMD_GET_SPECIAL_LIST = 130, 		// ( 130 ) 获取特殊用户列表
    eCMD_GET_SPECIAL_LIST_ACK,			// ( 131 ) 获取特殊用户列表应答
    eCMD_MODI_SPECIAL_LIST_NOTICE,		// ( 132 ) 通知变更通知
    eCMD_MODI_SPECIAL_LIST_NOTICE_ACK,	// ( 133 ) 收到应答

	eCMD_CREATEREGULARGROUPNOTICE = 135,		//135 固定组创建通知
	eCMD_CREATEREGULARGROUPNOTICEACK,			//136 固定组创建通知应答
	eCMD_DELETEREGULARGROUPNOTICE,				//137 固定组删除通知
	eCMD_DELETEREGULARGROUPNOTICEACK,			//138 固定组删除通知应答
	eCMD_GULARGROUPMEMBERCHANGENOTICE,			//149 固定组成员变化通知
	eCMD_GULARGROUPMEMBERCHANGENOTICEACK,		//140 固定组成员变化通知应答
	eCMD_GULARGROUPNAMECHANGENOTICE,			//141 固定组名称变化通知
	eCMD_GULARGROUPNAMECHANGENOTICEACK,			//142 固定组名称变化通知应答
	eCMD_GULARGROUPPROCLAMATIONCHANGENOTICE,	//143 固定组公告变化通知
	eCMD_GULARGROUPPROCLAMATIONCHANGENOTICEACK,	//144 固定组公告变化通知应答

	eCMD_MODIDEPTUSER = 145,				// 修改员工部门资料					145
	eCMD_MODIDEPTUSERACK,					// 修改员工部门资料应答				146	
	eCMD_MODIDEPTUSERNOTICE,				// 修改员工部门资料通知				147
	eCMD_MODIDEPTUSERNOTICEACK,				// 修改员工部门资料通知应答			148
	eCMD_MODIDEPINFO,						// 修改组织架构资料	                149
	eCMD_MODIDEPINFOACK,					// 修改组织架构资料应答				150
	eCMD_MODIDEPINFONOTICE,					// 修改组织架构资料通知           	151
	eCMD_MODIDEPINFONOTICEACK,				// 修改组织架构资料通知应答			152

	eCMD_GETTOTALLISTTYPE=157,				// 获取全量信息下载方式请求			157
	eCMD_GETTOTALLISTTYPEACK,				// 获取全量信息下载方式请求应答		158
	eCMD_GET_HEAD_ICON_ADD_LIST_REQ,		//获取头像变化用户列表请求159
	eCMD_GET_HEAD_ICON_ADD_LIST_RSP,		//应答

	eCMD_ROAMINGDATASYN = 175,				//175	漫游数据同步请求
	eCMD_ROAMINGDATASYNACK,					//176	漫游数据同步请求应答
	eCMD_ROAMINGDATAMODI,					//177	漫游数据增加、删减请求
	eCMD_ROAMINGDATAMODIACK,				//178	漫游数据增加、删减请求应答
	eCMD_ROAMINGDATAMODINOTICE,				//179	漫游数据增加、删减通知

	eCMD_SUBSCRIBERREQ,						//180	临时订阅请求
	eCMD_SUBSCRIBERACK,						//181	临时订阅应答
	eCMD_GET_STATUS_REQ=182,				//182   获取状态请求
	eCMD_NOTICESTATE_ALL,					//183   全量状态通知

	eCMD_SEND_SMS,							//184   发送短信

	eCMD_READ_MSG_SYNC_REQ =186,			//186	已读消息同步请求(通知另一端消息已经在本端查看过了)
	eCMD_READ_MSG_SYNC_NOTICE,				//187	已读消息同步通知
	eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE,	// 188 新创建群组通知  去除旧的
	eCMD_GULARGROUP_PROTOCOL2_CREATEACK,	//189 固定组协议2创建应答
	eCMD_ROBOTSYNCREQ,						//190 机器人同步请求
	eCMD_ROBOTSYNCRSP,						//191 机器人同步应答

	eCMD_CONTACTS_UPDATE_NOTICE,			//192 通讯录全量更新通知
	eCMD_CONTACTS_UPDATE_NOTICE_RSP,		//193 通讯录全量更新通知应答

	eCMD_DEPTSHOWCONFIG_REQ = 211,			// 211 获取部门显示配置请求	
	eCMD_DEPTSHOWCONFIG_ACK = 212,			// 212 获取部门显示配置应答

 //   eCMD_MEETING_INFO_NOTICE     = 213,     //会议基本信息通知
 //   eCMD_MEETING_MBRINFO_NOTICE  = 214,     // 会议成员信息通知
 //   eCMD_MEETING_FILEINFO_NOTICE = 215,     // 会议附件信息通知
 //   eCMD_MEETING_USERINFO_NOTICE = 216,     // 会议账号信息通知
 //   eCMD_MEETING_MSG_NOTICE      = 217,     // 会议提醒消息
 //   eCMD_GET_MEETING_ACCOUNT_INFO = 218,    // 获取全时帐号信息
	//eCMD_GET_MEETING_LEVEL_INFO = 219,      // 会议级别通知
	//eCMD_MEETING_REMARKS_NOTICE = 220,       // 会议备注提醒

    // Engine internal cmd
    eCMD_INT_START		= 0x100,			// (256) Start CmdId of Engine internal CmdId
    eCMD_LOAD_CONTACTS	= eCMD_INT_START,	// (256) 加载通讯录
	eCMD_MODIFY_MYSELF_INFO,				// (257) 修改个人信息
	eCMD_FILE_ACTION,						// (258) 文件传输控制
	eCMD_CUSTOMIZE_ACTION,					// (259) 用户自定义
	eCMD_UPDATE_TIMESTAMP,					// (260) 更新时间戳
	eCMD_SUBSCRIBE_ACTION,					// (261) 订阅
	eCMD_UPDATE_EMP_HEAD_FLAG,				// (262) 更新头像的时间
	eCMD_ADD_OVERTIME_SENDMSG,				// (263) 添加发送消息超时失败的记录
	eCMD_FLUSHOUT_DATABASE,					// (264) 刷新数据库（写入DB文件）
	eCMD_BACKUP_DATABASE,					// (265) 备份数据库
	eCMD_SSO_LOGIN,							// (266) SSO 登录
	eCMD_GET_IROBOT_MENU,					// (267) Get iRobot menu
	eCMD_GET_IROBOT_TOPIC,					// (268) Get iRobot topic
	eCMD_WEBAPP_CONFIG,						// (269) Web app's config
	eCMD_WEBAPP_AINTERFACE,					// (270) Web app's A Interface req
	eCMD_SEND_MSG_NOTICE_CONFIRM,			// (271) 发送消息通知已接收确认" 
	//eCMD_MEETING_CREATE,                    // (272) 创建会议
	//eCMD_GET_LAUNCHSTRING,                  // (273) 获取会议吊起串
 //   eCMD_GET_MEETING_ACCOUNT,               // (274) 获取全时帐号信息
 //   eCMD_MEETING_SYNC,                      // (275) 会议同步
 //   eCMD_MEETING_CANCEL,                    // (276) 取消会议
 //   eCMD_MEETING_MEMBERCHANGE,              // (277) 修改会议成员
	//eCMD_MEETING_CHANGE_LEVEL,              // (278) 修改会议级别
	//eCMD_MEETING_REGETMEETINGINFO,          // (279) 重新获取会议信息
	//eCMD_MEETING_RECEIVEMEETING,            // (280) 接受会议邀请
	//eCMD_MEETING_MEETINGREAD,               // (281) 会议已读
	eCMD_SMS_REQ,							// (282）
	eCMD_SMS_ACK,							// (283)
	eCMD_SEND_IMAGE_FILE_ERROR,				// (284) 发送图片、文件出错（本地提示与入库）
//	eCMD_MEETING_GETVIDEOURL,               // (285) 获取会议录制的视频
//	eCMD_WEBAPP_GET_NUM,					// (286) WebApp's get unread number
//	eCMD_MEETING_GET_ROOMLIST,				// (287) 泰禾获取可用会议室列表
	eCMD_GET_USERVIEWLEVEL,					// (288) 获取当前用户等可查看哪些等级的用户手机号
	eCMD_HTTP_GET,							// (299) http get 
	eCMD_HTTP_DOWNLOAD,						// 下载命令
	eCMD_ASYN_LOADIMAGE,					//异步加载图片
	eCMD_HTTP_DOWNLOAD_MAPIMAGE				//下载地图图片


}E_CmdId, *PE_CmdId;

///////////////////////////////////////////////////////////////////////////////
#define PROTOCOL_NAME_VALUE_SIZE			( 32 )
#define PROTOCOL_ID_INVALID					( ePFTYPE_UNKNOWN )			// Invalid Id

#pragma pack( push, 8 )

// Copy from server's <imp.h> L603
// for SENDMSG.cType, MSGNOTICE.cMsgType
typedef enum tagImMsgType
{
	eIM_MSGTYPE_TEXT = 0,			//普通文本消息
	eIM_MSGTYPE_IMG  = 1,			//普通图片消息
	eIM_MSGTYPE_VOICE = 2,			//普通语音消息
	eIM_MSGTYPE_VIDEO = 3,			//普通视频消息
	eIM_MSGTYPE_FILE	= 4,		//普通文件消息
	eIM_MSGTYPE_P2P	= 5,			//P2P捂手消息
	eIM_MSGTYPE_FILERECVED = 6,		//文件接收回复消息
	eIM_MSGTYPE_LONGTEXT = 7,		//长文本消息
	eIM_MSGTYPE_PCAUTO = 8,			//PC自动回复消息
	
	eIM_MSGTYPE_PUSH	= 10,		//公众平台推送消息
	
	eIM_MSGTYPE_BROADCAST = 20,		//广播消息
	eIM_MSGTYPE_SMS_SEND = 21,		//短信发送
	eIM_MSGTYPE_SMS_RECEIPT = 22,	//短信回执
	eIM_MSGTYPE_SMS_RSP = 23,		//平台的短信回复
	eIM_MSGTYPE_SMS_PUSH = 24,		//万达平台的IM消息
	eIM_MSGTYPE_SMS_ALERT = 25,		//万达平台的IM提醒
	eIM_MSGTYPE_ROBOT_SUGGEST = 26,	//机器人智能输入提示
	
	eIM_MSGTYPE_MSG_RECALL = 50,	//消息召回
	eIM_MSGTYPE_GROUP_ANNO= 51,		//群公告
	
	eIM_JSON = 101,				// 应用推送通知，JSON格式（检查是否有msgtype）
	eIM_FILE_DOWNLOADED = 9998,	// 检查文件已经成功下载
	eIM_AT_MSG = 9999,			// @消息已经提示（用户点了@提示）

}S_ImMsgType, *PS_ImMsgType;

typedef enum tagProtocolFieldType
{
    ePFType_PID = 0x80000000,	// Pseudo Id flag
    ePFTYPE_UNKNOWN = -1,		// Unknown
    ePFTYPE_REF = 0,			// Reference group
    ePFTYPE_I8,					// I8, Number
    ePFTYPE_I16,				// I16, Number
    ePFTYPE_I32,				// I32, Number
    ePFTYPE_I64,				// I64, Number
    ePFTYPE_U8,					// U8, Number
    ePFTYPE_U16,				// U16, Number
    ePFTYPE_U32,				// U32, Number
    ePFTYPE_U64,				// U64, Number
    ePFTYPE_FLOAT,				// Float, Number
    ePFTYPE_DOUBLE,				// Double, Number
    ePFTYPE_ASCII,				// Ascii, String
    ePFTYPE_UTF8,				// UTF8, String
    ePFTYPE_UTF16,				// UTF16, String
}E_PFieldType, *PE_PFieldType;
typedef const enum tagProtocolFieldType* PE_PFieldType_;

typedef struct tagProtocolField
{
    E_PFieldType	 eFieldType;	// Field type, or Pseudo ID
    union {
        unsigned __int64 u64Min;	// Min value or count
        double			 dMin;
    };
    union {
        unsigned __int64 u64Max;	// Max value or count
        double			 dMax;
    };
    TCHAR			 szName[PROTOCOL_NAME_VALUE_SIZE + 1];	// Field name
}S_PField, *PS_PField;
typedef const struct tagProtocolField* PS_PField_;

typedef struct tagProtocolCmd
{
    int			i32Count;		// Field count
    int			i32Id;			// Id
    TCHAR		szName[PROTOCOL_NAME_VALUE_SIZE + 1];	// Cmd name
    S_PField	sField[1];		// Variable-length arrays
}S_PCmd, *PS_PCmd;
typedef const struct tagProtocolCmd* PS_PCmd_;


// XML format
const TCHAR* const kCmdEnd	   = _T("</Cmd>");

// ***Msg***===================================================================
// MsgUI: kMsgUiBegin + <Message /> + kMsgUiEnd
// Type: E_SessionType
// eg.: <MsgUI SenderId=2 SenderName=\"my\" SendTime=123 Type=1 IsSend=1 >
//			<Message />
//		</MsgUI>
const TCHAR* const kMsgUiBegin		= _T("<MsgUI SenderId=%u SenderName=\"%s\" SendTime=%u Type=%u IsSend=%u>");
const TCHAR* const kMsgUiEnd		= _T("</MsgUI>");

const TCHAR* const kMsgError		= _T("<MsgUI SenderId=%u SenderName=\"%s\" SendTime=%u Type=%u IsSend=%u><Message><MsgError Text=\"%s\" /></Message></MsgUI>");

// CmdSendMsg: kCmdSendMsgBegin + <Message /> + kCmdSendMsgEnd
// eg.: <Cmd Id=61 RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u>"
//			<MsgUI Type=%u>
//				<Message />
//			</MsgUI>
//		</Cmd>
const TCHAR* const kCmdSendSingleMsgBeginVir		= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u GroupId2=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u><MsgUI Type=%u>");
const TCHAR* const kCmdSendSingleMsgBegin			= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u><MsgUI Type=%u>");
const TCHAR* const kCmdSendSingleMsgBeginType		= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u Type=%u><MsgUI Type=%u>");
const TCHAR* const kCmdSendSingleMsgBeginAddKeyType = _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u theme=\"%s\" value=\"%s\" Type=%u><MsgUI Type=%u>");
const TCHAR* const kCmdSendSingleMsgRobotBegin		= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u theme=\"%s\" value=\"%s\"><MsgUI Type=%u>");
const TCHAR* const kCmdAutoResponse					= _T("<Cmd Id=61 RecverId=%u GroupId=%I64u IsGroup=0 Read=0><MsgUI Type=0><Message><MsgAuto><![CDATA[%s]]></MsgAuto></Message></MsgUI></Cmd>");
const TCHAR* const kCmdSendMsgBegin					= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u><MsgUI Type=%u>");
const TCHAR* const kCmdItemEmpID					= _T("<Item EmpID=%u />");
const TCHAR* const kCmdSendMsgUIEnd					= _T("</MsgUI>");
const TCHAR* const kCmdSendMsgCmdEnd				= _T("</Cmd>");
const TCHAR* const kCmdSendMsgEnd					= _T("</MsgUI></Cmd>");

const TCHAR* const kCmdSendFileReceiveAck			= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u IsFileReceiveAck=1><MsgUI Type=%u>");

const TCHAR* const kCmdSendReCallMsg				= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Type=%u RecallMsgId=%lld><MsgUI Type=%u>");
const TCHAR* const kCmdSendSingleRobotTopic			= _T("<Cmd Id=61 BusinessId=%u RecverId=%u GroupId=%I64u IsGroup=%u Read=%u Is1To100=%u Is1To10T=%u theme=\"%s\" value=\"%s\" IsRobotTopic=%u><MsgUI Type=%u>");
const TCHAR* const kCmdSendRobotFirstOpen			= _T("<Cmd Id=61 BusinessId=1 RecverId=%u GroupId=%I64u IsGroup=0 Read=0 Is1To100=0 Is1To10T=0 IsRobotTopic=1 theme=\"%s\" value=\"%s\"><MsgUI Type=0>");
const TCHAR* const iRobotAllKey						= _T("490E7B19-FFC8-4341-B064-54534D81D141");
const TCHAR* const iRobotAllTheme					= _T("全部");
// MsgRich: <MsgRich>[<MsgUI></MsgUI>]...<MsgRich>
const TCHAR* const kMsgUIsToMsgRich	= _T("<MsgRich>%s</MsgRich>");

// CmdSendMsgAck
const TCHAR* const kCmdSendMsgAck	= _T("<Cmd Id=62 Result=%u MsgId=%I64u  GroupId=%I64u/>");
// CmdModifySelfInfoEx
//const TCHAR* const kCmdModifySelfInfoEx = _T("<Cmd Id=257 UserName=\"%s\" UserCode=\"%s\" Password=\"%s\" Sex=%d Birthday=%u Duty=\"%s\" OfficeTel=\"%s\" Phone=\"%s\" HomeTel=\"%s\" EmrgTel=\"%s\" Email=\"%s\" Hiredate=%u Logo=\"%s\" Sign=\"%s\" LoginType=%d MsgSynType=%d />");

// ***Group & VirtualGroup***=================================================================
// CreateGroup: kCmdCreateGroupBegin + kUserId(s) + kCmdEnd
// eg.: <Cmd Id=47 UserId=2 GroupId=1111222233334444 GroupName="test" TimeStamp=1111 >
//			<UserId=1 />
//			<UserId=4 />
//		</Cmd>
const TCHAR* const kCmdCreateGroupBegin		  = _T("<Cmd Id=47 UserId=%u GroupId=%I64u GroupName=\"%s\" Timestamp=%u >");
const TCHAR* const kUserId = _T("<Item UserId=%u />");

// CreateGroupAck: kCmdCreateGroupAckBegin + kUsersId(s) + kCmdEnd
// Fmt: see CreateGroup
// Notify UI when failed
//const TCHAR* const kCmdCreateGroupAckBegin    = _T("<Cmd Id=48 UserId=%u GroupId=%I64u GroupName=\"%s\" Timestamp=%u >");

// CreateGroupNotice: kCmdCreateGroupNoticeBegin + kUsersId(s) + kCmdEnd
//Fmt: see CreateGroup
// Used by engine only
//const TCHAR* const kCmdCreateGroupNoticeBegin = _T("<Cmd Id=49 UserId=%u GroupId=%I64u GroupName=\"%s\" Timestamp=%u >");

// ModifyGroup: kCmdModifyGroupBegin + kCmdModifyGroupItem + kCmdEnd
// eg.: <Cmd Id=51 UserId=2 GroupId=1111222233334444 TimeStamp=123456 >
//			<Item Type=0 Data="Group new name />
//			<Item Type=1 Data="Group new note />
//		</Cmd>
const TCHAR* const kCmdModifyGroupBegin       = _T("<Cmd Id=51 UserId=%u GroupId=%I64u Timestamp=%u >");
const TCHAR* const kCmdModifyGroupItem        = _T("<Item Type=%u Data=\"%s\" />");
#define MODIFY_GROUP_TYPE_NAME	( 0 )	// 0: group name
#define MODIFY_GROUP_TYPE_NOTE	( 1 )	// 1: group note 

// ModifyGroupAck: kCmdModifyGroupAckBegin + kCmdModifyGroupItem(s) + kCmdEnd
//const TCHAR* const kCmdModifyGroupAckBegin    = _T("<Cmd Id=52 UserId=%u GroupId=%I64u Timestamp=%u >");

// ModifyGroupNotice: kCmdModifyGroupNoticeBegin + kCmdModifyGroupItem + kCmdEnd
//const TCHAR* const kCmdModifyGroupNoticeBegin = _T("<Cmd Id=53 UserId=%u GroupId=%I64u GroupName=\"%s\" Timestamp=%u >");

// ModifyGroupNoticeAck: 
// Used by engine only
//const TCHAR* const kCmdModifyGroupNoticeAck  = _T("<Cmd Id=54 UserId=%u GroupId=%I64u />");

// GetGroupInfo:
// Used by engine only
//const TCHAR* const kCmdGetGroupInfo = _T("<Cmd = Id=55 UserId=%u GroupId=%I64u />");

// GetGroupInfoAck: kCmdGetGroupInfoAckBegin + kUsersId + kCmdEnd
//const TCHAR* const kCmdGetGroupInfoAckBegin		= _T("<Cmd Id=56 UserId=%u GroupId=%I64u GroupName=\"%s\" Timestamp=%u GroupNote=\"%s\" Result=%u >");

// ModifyGroupMember: kCmdModifyGroupMemberBegin + kCmdModifyGroupMemberItem(s) + kCmdEnd
// eg.: <Cmd Id=57 UserId=2 GroupId=1111222233334444 Timestamp=123456 >
//			<Item Type=0 UserId=3 />
//			<Item Type=1 UserId=4 />
//		</Cmd>
const TCHAR* const kCmdModifyGroupMemberBegin	= _T("<Cmd Id=57 UserId=%u GroupId=%I64u Timestamp=%u >");
const TCHAR* const kCmdModifyGroupMemberItem	= _T("<Item Type=%u UserId=%u />");
#define MODIFY_GROUP_MEMBER_TYPE_ADD	( 0 )	// 0: Add member
#define MODIFY_GROUP_MEMBER_TYPE_DEL	( 1 )	// 1: Del member 

// ModifyGroupMemberAck: kCmdModifyGroupMemberAckBegin + kCmdModifyGroupMemberItem(s) + kCmdEnd
const TCHAR* const kCmdModifyGroupMemberAckBegin= _T("<Cmd Id=58 UserId=%u GroupId=%I64u Timestamp=%u >");

// ModiMemberNotice: kCmdModiMemberNoticeAckBegin + kCmdModifyGroupMemberItem + kCmdEnd
const TCHAR* const kCmdModiMemberNoticeBebin	= _T("<Cmd Id=59 UserId=%u GroupId=%I64u Timestamp=%u >");

// ModiMemberNoticeAck
// Used by engine only
//const TCHAR* const kCmdModiMemberNoticeAck	= _T("<Cmd Id=60 UserId=%u GroupId=%I64u >");

// QuitGroup
const TCHAR* const kCmdQuitGroup		= _T("<Cmd Id=89 UserId=%u GroupId=%I64u />");

// QuitGroupAck
const TCHAR* const kCmdQuitGroupAck		= _T("<Cmd Id=90 GroupId=%I64u />");

// QuitGroupNotice
const TCHAR* const kCmdQuitGroupNotice	= _T("<Cmd Id=91 UserId=%u GroupId=%I64u Timestamp=%u />");

// QuitGroupNoticeAck
// Used by engine only
//const TCHAR* const kCmdQuitGroupNoticeAck= _T("<Cmd Id=92 Result=%u />");

// VirgroupUpdateReq
// Used by engine only
//const TCHAR* const kCmdVirgroupUpdateReq = _T("<Cmd Id=81 UserId=%u Timestamp=%u />");

// VirgroupUpdateRsp: kCmdVirgroupUpdateRspBegin + kUsersId(s) + kCmdEnd
// Used by engine only
//const TCHAR* const kCmdVirgroupUpdateRspBegin = _T("<Cmd Id=82 GroupId=%I64u GroupName=\"%s\" Timestamp=%u >");

const TCHAR* const kCmdCustomAction	= _T("<Cmd Id=259 CustomizeId=%u CustomGroupId=%I64u CustomOperaType=%u CustomGroupName=\"%s\" >");
const TCHAR* const kMemberId = _T("<Item CustomMemberId=%u />");

const TCHAR* const kCmdCustomActionEnd = _T("</Cmd>");

// subscribe  kCmdSubscribeAction + kMemberId ... +kAllCmdEnd
const TCHAR* const kCmdSubscribeAction = _T("<Cmd Id=180 OperType=%u>");
const TCHAR* const kCmdGetStatesAction = _T("<Cmd Id=182 GetType=%d>");
const TCHAR* const kSubcribeMemberId = _T("<Item EmpID=%u />");

// Subscribe action command
const TCHAR* const kCmdSubscribeActionEx = _T("<Cmd Id=261 Type=%u Opera=%d ItemID=%I64u ItemType=%d EmpsNum=%d >");
const TCHAR* const kSubscribeEmpId = _T("<Item SubscribeEmpId=%u />");

// Message read 
const TCHAR* const kCmdMsgRead = _T("<Cmd Id=73 MsgId=%I64u RecverId=%u ReadType=%d />");


const TCHAR* const kAllCmdEnd = _T("</Cmd>");

// Robot question 
const char* const kRobotQuestionBegin = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><robotRequest><key><theme>";
const char* const kRobotQuestionTheme = "</theme><value>";
const char* const kRobotQuestionValue = "</value></key><question><![CDATA[";
const char* const kRobotQuestionEnd   = "]]></question></robotRequest>";

// WebApp
const TCHAR* const kCmdWebAppRefresh = _T("<Cmd Id=270 AppId=%u CheckTime=%u AUrl=\"%s\" error-returnA=\"%s\" />");
const TCHAR* const kCmdWebAppGetNum  = _T("<Cmd Id=286 AppId=%u AUrl=\"%s\" />");

// Rdp: kCmdSendSingleMsgBegin + kMsgRdp + kCmdSendMsgEnd
const TCHAR* const kMsgRdp = _T("<MsgRdp GroupId=%I64u Type=%u Token=%u SenderId=%u RecverId=%u />");

// P2p: kCmdSendSingleMsgBegin + kMsgP2p + kCmdSendMsgEnd
const TCHAR* const kMsgP2p = _T("<MsgP2P GroupId=%I64u Fid=%I64u Size=%u Key=\"%s\" Name=\"%s\" File=\"%s\" />");

// P2P Notice confirm
const TCHAR* const kSendMsgNoticeConfirm = _T("<Cmd Id=271 RecverId=%u><MsgRdp GroupId=%I64u Type=%u Token=%u SenderId=%u RecverId=%u Extra=\"%s\" /></Cmd>");
const TCHAR* const kCmdSendCollectMsgBegin = _T("<Cmd Id=208 BusinessId=1 CollectUpdateType=%d SenderId=%u GroupId=%I64u IsGroup=%u >");

#endif // __EIMENGINE_PROTOCOL_HEADER_2013_12_23_YFGZ__