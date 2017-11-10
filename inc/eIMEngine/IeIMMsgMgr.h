// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMMsgMgr is the engine function DLL
// 
// Auth: yfgz
// Date:     2014/7/10 19:49:45 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_MSGMGR_HEADER_2014_07_10_YFGZ__
#define __EIMENGINE_MSGMGR_HEADER_2014_07_10_YFGZ__

#include "eIMEngine\IeIMSessionMgr.h"

// Return 0 to break Enumerate, szXml: <MsgUI />
typedef int (__stdcall* PFN_MsgCb)(QSID qsid, QMID qmid, QEID SenderID, DWORD dwTimestamp, LPCTSTR pszXml, void* pvUserData);
typedef int (__stdcall* PFN_DateCb)(DWORD dwDate, void* pvUserData);
typedef int (__stdcall* PFN_MsgInfoCb)(QEID SenderID, int type , DWORD dwTimestamp,  void* pvUserData);
typedef int (__stdcall* PFN_MsgIdCb)(QMID qmid, void* pvUserData);

typedef enum tagMsgOperate
{	// A [Date flag] can OR a [Contents flag], if Error combination, use the first appear flag by bit order
	// Date flag[Lowest byte]
	eMSG_OP_DATE		= 0x00000001,	// Equal the timestamp[ts == dwStart]
	eMSG_OP_GE_DATE		= 0x00000002,	// Be equal or greater than timestamp[ts >= dwStart]
	eMSG_OP_BETWEEN_DATE= 0x00000004,	// Between the timestamp[ts >= dwStart && ts <= dwEnd]
	eMSG_OP_LAST_MSG	= 0x00000008,	// Get last item of message of session[qsid]

	// Contents flag[second byte]
	eMSG_OP_FILE		= 0x00000100,	// As file
	eMsg_OP_IMAGE		= 0x00000200,	// As image
	eMSG_OP_TEXT		= 0x00000400,	// By content[szContent]
	eMSG_OP_CONTS		= 0x00000800,	// By contacts[szContacts]
	eMSG_OP_TEXT_CONTS	= (eMSG_OP_TEXT | eMSG_OP_CONTS),	// By content and contacts[szContent, szContacts]

	// Helper flag[Highest byte]
	eMSG_OP_NEW			= 0x01000000,	// Get the new(unreaded) msg
	eMSG_OP_OFFLINE		= 0x02000000,	// Get the offline msg
	eMSG_OP_NEW_OFFLINE = (eMSG_OP_NEW | eMSG_OP_OFFLINE)	// Get new and offline msg

}E_MsgOperate, *PE_MsgOperate;

#define MSG_CONTENT_MAX		( 128 )		// Max size to search content
#define MSG_CONTACTS_MAX	( 32  )		// Max size to search contacts
typedef struct tagMsgEnum
{
	E_MsgOperate	eOperate;			// Search operate
	E_SessionType	eType;				// Search session type(Reserved now)
	QSID			rQSID;				// Session ID, 0 to ignore it
	DWORD			dwOffset;			// Offset
	DWORD			dwLimit;			// Limit, 0 to ignore it
	DWORD			dwStart;			// Start date
	DWORD			dwEnd;				// End date
	TCHAR			szContent[MSG_CONTENT_MAX];		// Search content
	TCHAR			szContacts[MSG_CONTACTS_MAX];	// Search contacts
}S_MsgEnum, *PS_MsgEnum;

typedef struct tagMsg
{
	QMID			qmid;	// MsgId
	QSID			qsid;	// SessionId
	QEID			qeid;	// SenderId
	E_SessionType	eType;	// Session Type
	DWORD			dwTime;	// Send time
	DWORD			dwFlag;	// Flag
	UINT16			u161ToHundredFlag; //0:Normal 1:1 to hundred
}S_Msg, *PS_Msg;

//召回消息应答
typedef struct _SendReCallMsgAck
{
    bool	result;		// Result
    UINT8	cType;		// Type
	UINT64	dwMsgID;	// MsgId
	QSID	qSessionID;	// SessionId
	QEID	eidSender;	// Sender
	DWORD	dwTime;		// Send time
}SENDRECALLMSGACK;

//批量导入使用
typedef struct tagMsgInfo
{
	tagMsgInfo()
	{
		memset(&sMsg, 0, sizeof(S_Msg));
		memset(szXml, 0, TOKEN_MAX_SIZE);
	}
	S_Msg sMsg;						// Msg
	char  szXml[TOKEN_MAX_SIZE];	// XML
}S_MsgInfo, *PS_MsgInfo;

// 广播消息
#define MAX_BRO_RECEIVER_LEN		1024
#define MAX_BRO_TITLE_LEN			1024
#define MAX_BRO_TEXT_LEN			2048
#define SHOW_TIME_LEN				50
typedef struct tagBroMsg
{
	QMID	qmid;								// MsgId
	DWORD	dwUpdateTime;						// Update time
	TCHAR	szShowTime[SHOW_TIME_LEN];			// Show time
	TCHAR	szReceiver[MAX_BRO_RECEIVER_LEN];	// Receivers
	TCHAR	szTitle[MAX_BRO_TITLE_LEN];			// Title
	TCHAR	szMsg[MAX_BRO_TEXT_LEN];			// Messsage
}S_BroMsg, *PS_BroMsg;
typedef int (__stdcall* PFN_BroCb)(PS_BroMsg pBroMsg, void* pvUserData);

// 系统广播
typedef struct tagMsgNoticeMsg
{
	QMID	qmid;						// MsgId
	DWORD	dwUpdateTime;				// Update time
	TCHAR	szShowTime[SHOW_TIME_LEN];	// Show time
	TCHAR	szTitle[MAX_BRO_TITLE_LEN];	// Title
	TCHAR	szMsg[MAX_BRO_TEXT_LEN];	// Message
}S_MsgNoticeMsg, *PS_MsgNoticeMsg;
typedef int (__stdcall* PFN_MsgNoticeCb)(PS_MsgNoticeMsg pMsgNoticeMsg, void* pvUserData);

typedef int (__stdcall* PFN_RecordsAll)(void* pvRetData, void* pvUserData);

class __declspec(novtable) I_EIMMsgMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     GetCount
	//Description:	Get the message count of qsid by date range, for calcate the pages
	//
	//Parameter:
	//  qsid	   - Session ID	
	//	dwStart    - Start date, 0 to min value
	//	dwEnd      - End date, 0xFFFFFFFF to max value
	//
	//Return:
	//		Return the count of the message
	//=============================================================================
	virtual DWORD GetCount(
		QSID	qsid,
		DWORD	dwStart, 
		DWORD	dwEnd) = 0;

	//=============================================================================
	//Function:     GetCount
	//Description:	Get the message count
	//
	//Parameter:
	//	psCond      - Parameters
	
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD GetCount(PS_MsgEnum psCond);

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the session by parameters
	//
	//Parameter:
	//	psCond      - Parameters
	//	pfnMsgCb    - Msg callback function
	//	pvUserData	- User parameter passed to callback function
	//  bRecent		- get recent msg or not  (ASC OR desc)
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum( 
		PS_MsgEnum psCond,
		PFN_MsgCb pfnMsgCb,
		void*	  pvUserData,
		BOOL	  bRecent = FALSE
		) = 0;

	//=============================================================================
	//Function:     SearchMsgCount
	//Description:	Get message count by condition
	//
	//Parameter:
	//	psCond      - Parameters
	//
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD SearchMsgCount(PS_MsgEnum psCond) = 0;

	//=============================================================================
	//Function:     SearchMsg
	//Description:	Search message by condition
	//
	//Parameter:
	//	psCond      - Parameters
	//  pfnMsgCb	- Callback function
	//  pvUserData	- User data
	//
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD SearchMsg(PS_MsgEnum psCond, PFN_MsgCb pfnMsgCb, void* pvUserData) = 0;

	//=============================================================================
	//Function:     Get
	//Description:	Get a message by MsgId
	//
	//Parameter:
	//	psCond      - Parameters
	//  pfnMsgCb	- Callback function
	//  pvUserData	- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL Get(QMID qmid, PFN_MsgCb pfnMsgCb,   void*	 pvUserData) = 0;

 	//=============================================================================
	//Function:     Get
	//Description:	Get a message by MsgId
	//
	//Parameter:
	//	psCond      - Parameters
	//  pfnMsgCb	- Callback function
	//  pvUserData	- User data
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
   virtual BOOL Get(QMID qmid, PFN_MsgInfoCb pfnMsgCb,   void*	 pvUserData) = 0;

	//=============================================================================
	//Function:     EnumBroMsg
	//Description:	Enumerate the broadcast message
	//
	//Parameter:
	//  psCond		- Condition
	//	pfnBroMsgCb - Callback function
	//	pvUserData - User parameter passed to callback function
	//
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD EnumBroMsg(PS_SessionEnum	psCond,PFN_BroCb pfnBroMsgCb, void* pvUserData) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the session's date list by date range, for QQ like DateCtrl of msg
	//
	//Parameter:
	//  qsid	   - Session ID	
	//	dwStart    - Start date, 0 to min value
	//	dwEnd      - End date, 0xFFFFFFFF to max value
	//	pfnDateCb  - Date callback function 
	//	pvUserData - User parameter passed to callback function
	//
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD EnumMsgNoticeMsg(PS_MsgEnum psCond, PFN_MsgNoticeCb pfnMsgNoticeCb, void* pvUserData) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the session's date list by date range, for QQ like DateCtrl of msg
	//
	//Parameter:
	//  qsid	   - Session ID	
	//	dwStart    - Start date, 0 to min value
	//	dwEnd      - End date, 0xFFFFFFFF to max value
	//	pfnDateCb  - Date callback function 
	//	pvUserData - User parameter passed to callback function
	//
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum(
		QSID		qsid, 
		DWORD		dwStart,
		DWORD		dwEnd,
		PFN_DateCb	pfnDateCb,
		void*		pvUserData
		) = 0;

	//=============================================================================
	//Function:     Delete
	//Description:	Delete the message records
	//
	//Parameter:
	//	qsid		- Session Id to be deleted
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
	virtual BOOL Delete( QSID qsid) = 0;

	//=============================================================================
	//Function:     DeleteMsgByMsgID
	//Description:	Delete the message records by MsgID
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
	virtual BOOL DeleteMsgByMsgID( QMID qmid) = 0;

	//=============================================================================
	//Function:     DeleteSMSByID
	//Description:	Delete the SMS message records by MsgID
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
	virtual BOOL DeleteSMSByID( QMID qmid) = 0;

	//=============================================================================
	//Function:     DeleteBoardcastByID
	//Description:	Delete the broadcast message records by MsgID
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
	virtual BOOL DeleteBoardcastByID(QMID qmid) = 0;

	//=============================================================================
	//Function:     DeleteMsgNoticeByID
	//Description:	Delete the message notice records by MsgID
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
	virtual BOOL DeleteMsgNoticeByID(QMID qmid) = 0;

 	//=============================================================================
	//Function:     DeleteMsg
	//Description:	Delete the message records by time
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
   virtual BOOL DeleteMsg(QSID qSid, DWORD dwDelTime) = 0;

 	//=============================================================================
	//Function:     IsMsgIDExist
	//Description:	Check Message Id whether exist
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Exist
	//		FALSE	- Not exist
	//=============================================================================
	virtual BOOL IsMsgIDExist(QMID qmid) = 0;

 	//=============================================================================
	//Function:     IsSMSIDExist
	//Description:	Check SMS Message Id whether exist
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Exist
	//		FALSE	- Not exist
	//=============================================================================
	virtual BOOL IsSMSIDExist(QMID qmid) = 0;

 	//=============================================================================
	//Function:     IsBroIDExist
	//Description:	Check broadcast Message Id whether exist
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Exist
	//		FALSE	- Not exist
	//=============================================================================
	virtual BOOL IsBroIDExist(QMID qmid) = 0;

 	//=============================================================================
	//Function:     IsMsgNoticeIDExist
	//Description:	Check Message Notice Id whether exist
	//
	//Parameter:
	//	qmid		- Msg Id to be deleted
	//
	//Return:
	//		TRUE	- Exist
	//		FALSE	- Not exist
	//=============================================================================
	virtual BOOL IsMsgNoticeIDExist(QMID qmid) = 0;

	//=============================================================================
	//Function:     UpdateFlagByQSID
	//Description:	Update msg flag by qsid
	//
	//Parameter:
	//	qsid      - Special the qsid
	//	dwFlag    - Db falg
	//	bSet      - The vaule of flag
	//
	//Return: If Db succeed , return TURE. else 
	//              
	//=============================================================================
	virtual BOOL UpdateFlagByQSID(
		QSID qsid, 
		DWORD dwFlag, 
		BOOL bSet = TRUE) = 0;

	//=============================================================================
	//Function:     UpdateFlag
	//Description:	Update msg flag by qmid
	//
	//Parameter:
	//	qsid      - Special the qmid
	//	dwFlag    - Db falg
	//	bSet      - The vaule of flag
	//
	//Return: If Db succeed , return TURE. else 
	//              
	//=============================================================================
	virtual BOOL UpdateFlag(
		QMID qmid, 
		DWORD dwFlag, 
		BOOL bSet = TRUE) = 0;

	//=============================================================================
	//Function:     UpdateType
	//Description:	Update msg type
	//
	//Parameter:
	//	qsid      - Special the qmid
	//	dwType    - type
	//
	//Return:  
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL UpdateType(QMID qmid, DWORD dwType) = 0;

	//=============================================================================
	//Function:     UpdateMsg2ReCallType
	//Description:	Update recall message information
	//
	//Parameter:
	//	sRecallInfo      - Recall information
	//
	//Return:  
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL UpdateMsg2ReCallType(SENDRECALLMSGACK sRecallInfo ) = 0;

	//=============================================================================
	//Function:     ReplaceSet
	//Description:	Update message information
	//
	//Parameter:
	//	pszData      - The data
	//
	//Return:  
	//	HRESULT
	//=============================================================================
	virtual int  ReplaceSet(void* pszData) = 0;

	//=============================================================================
	//Function:     GetRecordsBySid
	//Description:	Get message information by session
	//
	//Parameter:
	//	qSid		- Session ID
	// pfnRecords	- Callback function
	// pvUserData	- User data
	//
	//Return:  
	//	Message count
	//=============================================================================
	virtual DWORD GetRecordsBySid(QSID qSid,  PFN_RecordsAll pfnRecords, void* pvUserData) = 0;

	//=============================================================================
	//Function:     Get
	//Description:	Get sessin ID by MsgId
	//
	//Parameter:
	//	qmid		- message ID
	//
	//Return:  
	//	Session Id
	//=============================================================================
    virtual QSID Get(QMID qmid) = 0;

	//=============================================================================
	//Function:     EnumMsgId
	//Description:	Enum message Id
	//
	//Parameter:
	//	qsid		- Session ID
	//  dwTime		- Time
	//  pfnMsgIdCb	- Callback
	//	pvUserData	- User data
	//
	//Return:  
	//	Message count
	//=============================================================================
    virtual DWORD EnumMsgId(QSID qsid, DWORD dwTime,  PFN_MsgIdCb pfnMsgIdCb, void* pvUserData) = 0;
};

#endif // __EIMENGINE_MSGMGR_HEADER_2014_07_10_YFGZ__