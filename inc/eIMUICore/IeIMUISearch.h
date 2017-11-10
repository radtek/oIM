// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMUISearch is the Search ui interface
// 
// Auth: hdw
// Date:  2014/7/18
// 
// History: 
//   
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIMUI_SEARCH_HEADER_2014_07_18_HDW__
#define __IEIMUI_SEARCH_HEADER_2014_07_18_HDW__

#include "public\plugin.h"
#include "Public\XmlConfiger.h"
#include "eIMConfigConst.h"

#define INAME_EIMUI_SEARCH					_T("SXIT.EIMUI.SEARCH")	// I_EIMUISearch interface name

class __declspec(novtable) I_EIMUISearch: public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     Show
	//Description:	Show the search window
	//
	//Parameter:
	//		i32Wnd	Which window id
	//
	//Return:
	//		TRUE	Succeeded
	//		FALSE	Failed
	//=============================================================================
	virtual BOOL Show() = 0;
};




#endif // __IEIMUI_SEARCH_HEADER_2014_07_18_HDW__