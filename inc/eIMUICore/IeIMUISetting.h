// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMUISetting is the Setting ui interface
// 
// Auth: yfgz
// Date:  10:37 2014/5/6
// 
// History: 
//   
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIMUI_SETTING_HEADER_2014_05_06_YFGZ__
#define __IEIMUI_SETTING_HEADER_2014_05_06_YFGZ__

#include "public\plugin.h"
#include "Public\XmlConfiger.h"
#include "eIMConfigConst.h"

#define INAME_EIMUI_SETTING					_T("SXIT.EIMUI.SETTING")	// I_EIMUISetting interface name

#pragma region SettingConst 
// Main tab define 
#define SETTING_MAIN_TAB_MY					( 0x01000000 )	// Person setting tab
#define SETTING_MAIN_TAB_SYSTEM				( 0x02000000 )	// System setting tab
#define SETTING_MAIN_TAB_MASK				( 0xffff0000 )	// Tab mask
#define SETTING_MAIN_PAGE_MASK				( 0x0000ffff )	// page mask

// My tab's sub-tab
#define SETTING_MY_BASIC_INFO				( SETTING_MAIN_TAB_MY | 0x000001 )
#define SETTING_MY_CONTACT_INFO				( SETTING_MAIN_TAB_MY | 0x000002 )
#define SETTING_MY_DETAIL_INFO				( SETTING_MAIN_TAB_MY | 0x000003 )
#define SETTING_MY_PSW						( SETTING_MAIN_TAB_MY | 0x000004 )
#define SETTING_MY_HOTKEYS					( SETTING_MAIN_TAB_MY | 0x000005 )
#define SETTING_MY_REPLY					( SETTING_MAIN_TAB_MY | 0x000006 )

// System tab's sub-tab
#define SETTING_SYS_BASIC					( SETTING_MAIN_TAB_SYSTEM | 0x000001 )
#define SETTING_SYS_SOUND					( SETTING_MAIN_TAB_SYSTEM | 0x000002 )
#define SETTING_SYS_FILE_TRANSFER			( SETTING_MAIN_TAB_SYSTEM | 0x000003 )
#define SETTING_SYS_PROXY					( SETTING_MAIN_TAB_SYSTEM | 0x000004 )
#define SETTING_SYS_SERVERS					( SETTING_MAIN_TAB_SYSTEM | 0x000005 )

class __declspec(novtable) I_EIMUISetting: public I_EIMPlugin
{
public:
	//=============================================================================
	//Function:     SetAttribute
	//Description:	Set attribute
	//
	//Parameter:
	//	i32NameId  	- Id of the attribute name
	//	i32Value	- Attribute value
	//Return:
	//		1	Old value
	//=============================================================================
	virtual int SetAttribute(
		int i32Type, 
		int i32Value
		) = 0;

	//=============================================================================
	//Function:     Uninit
	//Description:	Set configer to load and save the config
	//
	//Parameter:
	//Return:
	//		TRUE	Succeed
	//		FALSE	Failed
	//=============================================================================
	virtual BOOL Uninit() = 0;

	//=============================================================================
	//Function:     Show
	//Description:	Show the setting window
	//
	//Parameter:
	//	i32SettingType   - SETTING_MAIN_TAB_MY or SETTING_MAIN_TAB_SYSTEM
	//  i32SelectedItem	 -Selected items
	//	bLogin		     - If logined, show all tab, else show owned tab only.
	//Return:
	//		1	Changed the setting 
	//		0	No change the setting
	//=============================================================================
	virtual int Show(
		int i32SettingType, 
		int i32SelectedItem,
		bool bLogined = true
		) = 0;
};




#endif // __IEIMUI_SETTING_HEADER_2014_05_06_YFGZ__