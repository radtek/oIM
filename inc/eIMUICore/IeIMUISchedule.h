// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
// 
// 
// 
// Auth: hdp
// Date: 2013/12/2 11:22:40
// 
// History: 
//    
//    
//    
/////////////////////////////////////////////////////////////////////////////
#ifndef __IEIMSCHEDULE_2013_11_05_BY_HDP__
#define __IEIMSCHEDULE_2013_11_05_BY_HDP__


#include <windows.h>
#include <tchar.h>

#include "public\plugin.h"

#define INAME_EIMSCHEDULE		_T("SXIT.EIMSchedule")	// eIMSchedule interface name


class __declspec(novtable) I_eIMSchedule : public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     StartSchedule
	//Description:	Show schedule dialog
	//
	//Parameter:
	//  hParent		- Parent window
	//					
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int StartSchedule(HWND hParent = NULL) = 0; 
};


#endif