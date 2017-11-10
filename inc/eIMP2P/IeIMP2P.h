// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMP2P is the engine function DLL
// 
// Auth: yfgz
// Date:     2014/1/15 15:53:32 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_P2P_HEADER_2014_01_10_YFGZ__
#define __EIMENGINE_P2P_HEADER_2014_01_10_YFGZ__

#include "eIMP2P\rudp.h"
#include "eIMP2P\ec2ec.h"
#include "Public\Plugin.h"
#include "Public\Errors.h"
#include "vld\vld.h"

// P2P Interface name MUST BE as format: INAME_P2P + _T('.') + SessionToken, eg.:
// SXIT.P2P.00000227000001900000000252D49D52
#define INAME_EIMP2P			_T("SXIT.EIMP2P")	// eIMP2P interface name(original name is rudp)

#define P2PSTATUS_NOT_CONNECT	( 0 )			// Not connected
#define P2PSTATUS_CONNECTING	( 1 )			// Connecting
#define P2PSTATUS_CONNECTED		( 2 )			// Connected

#define PPP_EVENT_POLL_PROGRESS	( 0xFF )

typedef struct tagP2PChannel
{
	DWORD	u32Type;				// Message ID, see PPP_EVENT_*, PPP_EVAL_*
	DWORD	u32SenderId;			// Sender ID
	DWORD	u32ReceiverId;			// Receiver ID
	DWORD	u32Flag;				// Bit0: 1 is send; 0 is recv
	UINT64	u64FileSize;			// Message Value if have.
	UINT64	u64Value;				// Value, by u32Type
	TCHAR	szToken[TOKEN_SIZE];	// Session Token
	TCHAR	szFileKey[TOKEN_SIZE];	// FileKey
	TCHAR	szFile[MAX_PATH];		// File path
}S_P2PChannel, *PS_P2PChannel;
typedef const struct tagP2PChannel* PS_P2PChannel_;

typedef bool ( __stdcall *PFN_P2PCallback)( PS_P2PChannel_ psP2PInfo, void* pvUserData );


class __declspec(novtable) I_eIMP2P: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Init
	//Description:	Inital P2P server setting, MUST be call it once before do any action
	//
	//Parameter:
	//	pszServerIP      - P2P Server IP
	//	i32ServerPort    - P2P Server Port
	//	i32UdpPort       - Local UDP Port 
	//	pfnP2pCb         - Callback function to notificed
	//	pvUserData       - User define data
	//
	//Return:
	//    true	- Succeeded
	//	  false	- Failed
	//=============================================================================
	virtual bool Init(
		const TCHAR*	pszServerIP,	// P2P Server IP
		int				i32ServerPort,	// P2P Server Port
		PFN_P2PCallback pfnP2pCb,		// Callback for event
		void*			pvUserData		// User datas
		) = 0;

	//=============================================================================
	//Function:     Start
	//Description:	Start the connect, after Init(...), you can call it to
	//	start the P2P connect, each single session need at least call it once
	//  to send or receive file
	//
	//	After start the connect, you can send or receive files, regardless of
	//  it client or server
	//
	//  If any endpoint STOPPED, the other endpoint MUST be STOP too, after it
	//  need call Start(...) again, so, you need check the connect status by 
	//  GetConnectStatus(), you need Stop() when is not P2PSTATUS_NOT_CONNECT
	//
	//Parameter:
	//	bClient    - TRUE as client, is the sender that first time call it
	//			   - FALSE as server, is the receiver that first time call it.
	//
	//Return:
	//      true	- Succeeded
	//		false	- Failed
	//=============================================================================
	virtual bool Start(
		bool		 bClient			// Whether start as client
		) = 0;

	//=============================================================================
	//Function:     GetStatus
	//Description:	Get connect status
	//
	//
	//Return:
	//      P2PSTATUS_NOT_CONNECT	- Not connect
	//		P2PSTATUS_CONNECTING	- Is connecting
	//		P2PSTATUS_CONNECTED		- Connected
	//=============================================================================
	virtual unsigned int GetConnectStatus() = 0;

	//=============================================================================
	//Function:     SendFile
	//Description:	Send file
	//
	//Parameter:
	//	pszFile       - Local file to be send
	//	pszFileKey    - File key to identified the file
	//
	//Return:
	//    true	- Succeeded
	//	  false	- Failed
	//=============================================================================
	virtual bool SendFile(		// 发送文件
		const TCHAR* pszFile,	// 待发送文件全路径
		const TCHAR* pszFileKey	// 文件传输FileKey
		) = 0;				

	//=============================================================================
	//Function:     RecvFile
	//Description:	Receive file
	//
	//Parameter:
	//	pszFile       - Local file to be save
	//	pszFileKey    - File key to identified the file
	//	bResume       - Breakpoint resume
	//
	//Return: 
	//    true	- Succeeded
	//	  false	- Failed
	//=============================================================================
	virtual bool RecvFile(		// 接收文件
		const TCHAR* pszFile,	// 待接收文件的全路径
		const TCHAR* pszFileKey,// 文件传输FileKey
		bool		 bResume	// 是否断点续传
		) = 0;								

	//=============================================================================
	//Function:     Cancel
	//Description:	Cancel the transfer file
	//
	//Parameter:
	//	pszFileKey    - File key to identified the file to recevie or send
	//	u32Reason     -
	//
	//Return:
	//    true	- Succeeded
	//	  false	- Failed
	//=============================================================================
	virtual bool Cancel(	// 取消文件传输
		const TCHAR* pszFileKey,// 文件传输FileKey
		unsigned int u32Reason	// 取消的原因, 参看 PPP_EVAL_FILE_*
		) = 0;					

	//=============================================================================
	//Function:     EnableSecurity
	//Description:	Enable security transfer
	//
	//Parameter:
	//	pszKey    - Key
	//
	//Return:
	//     true		- Succeeded
	//	   false	- Failed
	//=============================================================================
	virtual bool EnableSecurity(// 启用安全传输
		const TCHAR* pszKey		// 安全密钥
		) = 0;					

	//=============================================================================
	//Function:     Reject
	//Description:	Reject the file
	//
	//Parameter:
	//	pszFileKey    - File key to identified the file to recevie 
	//	u32Reason     - PPP_EVAL_FILE_RSEC
	//				  - PPP_EVAL_FILE_RITV
	//
	//Return:
	//     true		- Succeeded
	//	   false	- Failed
	//=============================================================================
	virtual bool Reject(			// 拒绝接收文件
		const TCHAR* pszFileKey,// 文件传输FileKey
		unsigned int u32Reason	// 取消的原因, 参看 PPP_EVAL_FILE_*
		) = 0;

	//=============================================================================
	//Function:     Stop
	//Description:	Stop all task and disconnect
	//
	//
	//Return:
	//     true		- Succeeded
	//	   false	- Failed
	//=============================================================================
	virtual bool Stop() = 0;	

	//=============================================================================
	//Function:     SetRetryTimes
	//Description:	Set retry times
	//
	//	i32Times    - The times to retry when connect failed, it will be set to INT_MAX 
	//				  when less then 0, 
	//
	//Return:
	//     The old retry times
	//=============================================================================
	virtual int	SetRetryTimes(
		int i32Times
		) = 0;	


};

#endif // __EIMENGINE_P2P_HEADER_2014_01_10_YFGZ__