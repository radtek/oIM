// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMPlayer is to support the interface
//  of play music or video
// 
// 
// Auth: HuDongwen
// Date: 2014/5/29 14:00:47
// 
// History: 
//    2014/5/29 HuDongwen 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIM_PLAYER_HEADER_2014_05_29__
#define __IEIM_PLAYER_HEADER_2014_05_29__

#include "public\plugin.h"
#include "Public\StrUtil.h"

#define INAME_EIMUI_PLAYER	_T("SXIT.Player")	// I_EIMPlayer interface name

class __declspec(novtable) I_EIMPlayer: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Play
	//Description:	Play the media
	//
	//Parameter:
	//	szPath			-Media path
	//	hwndDrawable	-Windows handle where the media player should
	//				render its video output
	//
	//Return:
	//	TRUE		-SUCCEEDED
	//	FALSE		-FAILED
	//              
	//=============================================================================
	BOOL Play( const eIMString& szPath, HWND hwndDrawable = 0 );

	//=============================================================================
	//Function:     Stop
	//Description:	Stop the media
	//
	//
	//Return:
	//	TRUE		-SUCCEEDED
	//	FALSE		-FAILED
	//              
	//=============================================================================
	BOOL Stop();

	//=============================================================================
	//Function:     Pause
	//Description:	Pause the media
	//
	//
	//Return:
	//	TRUE		-SUCCEEDED
	//	FALSE		-FAILED
	//              
	//=============================================================================
	BOOL Pause();

};

#endif // __IEIM_PLAYER_HEADER_2014_05_29__