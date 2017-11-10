// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_MSG_READ_MGR_H
#define __EIMENGINE_MSG_READ_MGR_H

#pragma pack(push, 8)

#define MSG_READ_FLAG_NO_READ	0
#define MSG_READ_FLAG_READ		1

#define MSG_RECEIPT  0
#define MSG_1TO100   1

typedef struct tagMsgRead
{
	QMID			qmid;			// MsgId
	QEID			qeidRecver;		// RecverId
	DWORD			dwTime;			// Send time
	DWORD			dwFlag;			// Flag		0:NoRead, 1:Read

}S_MsgRead,*PS_MsgRead;

//一呼百应界面
typedef struct tag1To100Read
{
	QMID qmid;				//Qmid
	QSID qsid;				//用作一呼百应通知时查找对应会话窗口
	QEID qeidSender;		//Msg sender
	bool bIsSend;			//是否发送者 1：是， 0：不是
}S_1To100Read, *PS_1To100Read;

#pragma pack( pop )

typedef int (__stdcall* PFN_1to100READ)(PS_MsgRead psMsgRead, void* pvUserData);

class __declspec(novtable) I_EIMMsgReadMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     GetHasMsgReadCount
	//Description:	get msg count of been read
	//Parameter:
	//	qmid   - msgid
	//Return:
	//   int  -  count
	//=============================================================================
	virtual int	GetHasMsgReadCount( QMID qmid ) = 0;
	
	//=============================================================================
	//Function:     GetMsgReadCount
	//Description:	get all the count of t_msg_read
	//Parameter:
	//	qmid   - msgid
	//Return:
	//   int  -  count
	//=============================================================================
	virtual int	GetMsgReadCount( QMID qmid ) = 0;

	//=============================================================================
	//Function:     GetMsgRead
	//Description:	get msg read info
	//Parameter:
	//	qmid   - msgid
	//  PFN_1to100READ
	//Return:
	//   1
	//=============================================================================
	virtual int GetMsgRead(QMID qmid, PFN_1to100READ pfnRead, void* pvUserData) = 0;

	//=============================================================================
	//Function:     Set
	//Description:	insert msg to t_msg_read
	//Parameter:
	//Return:
	//   1
	//=============================================================================
	virtual int	Set(PS_MsgRead psMsgRead) = 0;

	//=============================================================================
	//Function:     DeleteMsgRead
	//Description:	delete msg from t_msg_read
	//Parameter:
	//   delMsgIdLst  -  msgid list
	//Return:
	//   1
	//=============================================================================
    virtual int DeleteMsgRead(const std::list<QMID>& delMsgIdLst) = 0;
};

#endif // __EIMENGINE_MSGMGR_HEADER_2014_07_10_YFGZ__