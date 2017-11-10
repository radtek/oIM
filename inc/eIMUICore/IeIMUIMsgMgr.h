// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMUIMsgMgr is the Message Manager ui interface
// 
// Auth: yfgz
// Date:  16:24 2014/5/6
// 
// History: 
//   
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIMUI_MESSAGE_MANAGER_HEADER_2014_05_06_YFGZ__
#define __IEIMUI_MESSAGE_MANAGER_HEADER_2014_05_06_YFGZ__

#include "public\plugin.h"
#include "Public\Typedefs.h"
#include "eIMEngine/IeIMSessionMgr.h"

#define INAME_EIMUI_MSGMGR					_T("SXIT.EIMUI.MSGMGR")	// I_EIMUIMsgMgr interface name

#pragma region MessageManagerConst
#define EIM_UI_MSGMGR_MGR					( 0x00000000 )
#define EIM_UI_MSGMGR_UNREAD				( 0x00000001 )
#define EIM_UI_MSGMGR_OFFLINE				( 0x00000002 )

class __declspec(novtable) I_EIMUIMsgMgr: public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     Show
	//Description:	Show the setting window
	//
	//Parameter:
	//	i32Wnd     	- Which window ID 
	//Return:
	//		1	Changed the setting 
	//		0	No change the setting
	//=============================================================================
	virtual int Show(
		int	i32Wnd,
		BOOL bShow = TRUE
		) = 0;
	//=============================================================================
	//Function:     MsgWndSelectSession
	//Description:	msg center select qsid session
	//
	//Parameter:
	//	qsid     	- session ID
	//  eType       - session type
	//Return:
	//		0
	//=============================================================================
	virtual int MsgWndSelectSession(QSID qsid, E_SessionType eType) = 0;

	//=============================================================================
	//Function:     CloseWnd
	//Description:	Close the window
	//
	//Parameter:
	//
	//Return:
	//		0
	//=============================================================================
	virtual int CloseWnd() = 0;
};




#endif // __IEIMUI_MESSAGE_MANAGER_HEADER_2014_05_06_YFGZ__