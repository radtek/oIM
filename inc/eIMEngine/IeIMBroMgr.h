// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_BRO_MGR_H
#define __EIMENGINE_BRO_MGR_H

#define BRO_RECV_ID_MAX		( 100 )		// Max size to recver id
#define BRO_TITLE_LEN		( 256 )     // title length
#define BRO_MSG_LEN			( 1024 )    // msg length

//������ͣ�77:��ͨ�㲥 78:�����㲥 88:��Ϣ֪ͨ
#define BRO_NORMAL_TYPE		( 77  )	
#define BRO_SUPER_TYPE		( 78  )
#define ALERT_TYPE			( 88  )

#define BRO_SUPER			( 1 )

#pragma pack(push, 8)

typedef enum tageBroType
{
	eBRO_BROCAST=0,
	//eBRO_SUPERBRO=1,
	eBRO_1To10T=1,
	eBRO_ALERT=2,
	eBRO_WEBAPP=3,	// WebApp
}E_BroType;

typedef struct tagBroInfo
{
	QMID  qMid;          //msgid
	int   i32timestamp;  //updatetime
	int   i32offmsg_flag;  //offline msg
	int   i32newmsg_flag;  //new msg
	int   i32flag;         //flag
	int   i32type;		//0���㲥  1��һ����Ӧ
	UINT32 u32SendID;   //��ϵͳ�㲥ʹ��
	const char* pszrecver; //receive
	const char* psztitle;  //title
}S_BroInfo, *PS_BroInfo;

typedef struct tagBroNotice
{
	QEID  qSendEid;   //������ID
	QEID  qRecvEid;	  //������ID
	QMID  qMid;       //msgid
	UINT8 u8Type;     //type
	UINT   i32Time;   //updatetime
	TCHAR tszTitle[BRO_TITLE_LEN];  //title
	TCHAR tszMsg[BRO_MSG_LEN];      //msg
}S_BroNotice, *PS_BroNotice;

#pragma pack( pop )

class __declspec(novtable) I_EIMBroMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Set
	//Description:	insert msg to db
	//Parameter:
	//	psBro   - bro info
	//  pszXml  -  xml info
	//Return:
    //    1
	//=============================================================================
	virtual int  Set(PS_BroInfo psBro, const TCHAR* pszXml) = 0;

};

#endif // __EIMENGINE_MSGMGR_HEADER_2014_07_10_YFGZ__