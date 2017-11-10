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

#ifndef __EIMENGINE_SMSMGR_H
#define __EIMENGINE_SMSMGR_H

#include "eIMEngine\IeIMSessionMgr.h"

#define SMS_CONTENT_LEN	1500

typedef enum tagSMSOperate
{	// A [Date flag] can OR a [Contents flag], if Error combination, use the first appear flag by bit order
	// Date flag[Lowest byte]
	eSMS_OP_DATE		= 0x00000001,	// Equal the timestamp[ts == dwStart]
	eSMS_OP_GE_DATE		= 0x00000002,	// Be equal or greater than timestamp[ts >= dwStart]
	eSMS_OP_BETWEEN_DATE= 0x00000004,	// Between the timestamp[ts >= dwStart && ts <= dwEnd]

	// Contents flag[second byte]
	eSMS_OP_TEXT		= 0x00000400,	// By content[szContent]
	eSMS_OP_CONTS		= 0x00000800,	// By contacts[szContacts]
	eSMS_OP_TEXT_CONTS	= (eSMS_OP_TEXT | eSMS_OP_CONTS),	// By content and contacts[szContent, szContacts]

	// Helper flag[Highest byte]
	eSMS_OP_NEW			= 0x01000000,	// Get the new(unreaded) SMS
	eSMS_OP_OFFLINE		= 0x02000000,	// Get the offline SMS
	eSMS_OP_NEW_OFFLINE = (eSMS_OP_NEW | eSMS_OP_OFFLINE)	// Get new and offline SMS

}E_SMSOperate, *PE_SMSOperate;

#define SMS_CONTENT_MAX		( 128 )		// Max size to search content
#define SMS_CONTACTS_MAX	( 32  )		// Max size to search contacts
typedef struct tagSMSEnum
{
	E_SMSOperate	eOperate;			// Search operate
	int 			iType;				// id type 0  eid   1 number
	QSID			rQSID;				// eid or phone number
	DWORD			dwOffset;			// Offset
	DWORD			dwLimit;			// Limit, 0 to ignore it
	DWORD			dwStart;			// Start date
	DWORD			dwEnd;				// End date
	TCHAR			szContent[SMS_CONTENT_MAX];		// Search content
	TCHAR			szContacts[SMS_CONTACTS_MAX];	// Search contacts
}S_SMSEnum, *PS_SMSEnum;


typedef struct tagSms
{
	QMID			qmid;			// MsgId
	UINT64			eidorPhone;		// SenderId
	int				iType;			// 数据类型：短信
	DWORD			dwTime;			// 
	int				i32newmsg_flag;	// New message flag
	int				i32offmsg_flag;	// Offline message flag
	DWORD			dwFlag;			// Flag
	int				i32eidOrPhoneType;	//0:eid, 1:phone
}S_Sms, *PS_Sms;

typedef struct tagSmsSendRlt
{
	int    i32Result;	// HRESULT
	QEID   qEid;		//发送时：接收者EID，接收时：发送者EID
	DWORD  dwWndID;		// Window Id
	UINT64 u64Phone;	// Phone 
}S_SmsSendRlt, *PS_SmsSendRlt;
typedef struct tagSmsNotify
{
	UINT64 u64Phone;	// Phone
	TCHAR  szDesc[256];	// Description
}S_SmsNotify, *PS_SmsNotify;

//收到对方回复短信
typedef struct tagSmsReply
{
	QMID   qMID;		// MsgId
	QEID   qEid;		// 发送时：接收者EID，接收时：发送者EID
	UINT64 u64Phone;	// Phone
	DWORD  dwTimer;		// Time
	TCHAR  szContent[SMS_CONTENT_LEN];	// Content
}S_SmsReply, *PS_SmsReply;


typedef struct tagSmsListInfo
{
	QSID QeidOrPhone;	// 0:eid, 1:phone
	DWORD dwTimestamp;	// Time
	int iCount;			// Count
	int iType;			// Type
	QMID qmid;			// MsgId
}S_SmsListInfo, *PS_SmsListInfo;

// Return 0 to break Enumerate, szXml: <MsgUI />
typedef int (__stdcall* PFN_SMSMsgCb)(S_Sms &rMsg, LPCTSTR pszXml, void* pvUserData);
typedef int (__stdcall* PFN_DateCb)(DWORD dwDate, void* pvUserData);

// 因为不使用搜索出来的COUNT
//typedef int (__stdcall* PFN_CountCb)(QSID QeidOrPhone, int iCount, DWORD dwTimestamp, int iType, void* pvUserData);
typedef int (__stdcall* PFN_CountCb)(S_SmsListInfo& rInfo, void* pvUserData);


class __declspec(novtable) I_EIMSMSMgr: public I_EIMUnknown
{
public:

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the SMS by parameters
	//
	//Parameter:
	//	psCond      - Condition to enum
	//	pfnMsgCb    - Msg callback function
	//	pvUserData	- User parameter passed to callback function
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum(PS_SMSEnum psCond, PFN_CountCb pfnMsgCb, void*	pvUserData) = 0;

	//=============================================================================
	//Function:     GetCount
	//Description:	Get the count of SMS
	//
	//Parameter:
	//	psCond      - Parameters
	
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD GetCount(QSID QeidOrPhone, int iType) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the SMS by parameters
	//
	//Parameter:
	//	psCond      - Condition to enum
	//	pfnMsgCb    - Msg callback function
	//	pvUserData	- User parameter passed to callback function
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum( PS_SMSEnum psCond,PFN_SMSMsgCb pfnMsgCb,void*	  pvUserData) = 0;

};

#endif // __EIMENGINE_MSGMGR_HEADER_2014_07_10_YFGZ__