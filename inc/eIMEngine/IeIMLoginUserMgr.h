// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMSessionMgr is the engine function to 
//	 operate the sessions
// 
// Auth: yfgz
// Date:     2014/7/2 13:53:05 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_LOGIN_USER_MANAGER_HEADER_2014_07_15_YFGZ__
#define __EIMENGINE_LOGIN_USER_MANAGER_HEADER_2014_07_15_YFGZ__
#include "Public\Plugin.h"

#define LOGIN_USER_FLAG_REMEMBER_PSW		( 0x00000001 )	// Remember psw
#define LOGIN_USER_FLAG_AUTO_LOGIN			( 0x00000002 )	// Auto login
#define LOGIN_USER_FLAG_LOGIN_TYPE_MASK		( 0x00FF0000 )	// Login type mask
#define LOGIN_USER_DUMMY_PSW				_T("*********")	// Dummy password for auto login that already remembered psw

// Enumerator callback, retorn 0 to break 
typedef int (__stdcall* PFN_LoginUserCb)(const TCHAR* pszName, QEID qeid, DWORD dwFlag, void* pvUserData);

class __declspec(novtable) I_EIMLoginUserMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Delete
	//Description:	Delete the login user information
	//
	//Parameter:
	//	pszName    - The login account name
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL Delete( 
		const TCHAR* pszName
		) = 0;

	//=============================================================================
	//Function:     GetDbVer
	//Description:	Get Database version
	//
	//Parameter:
	//	pszName    - The login account name
	//
	//Return:
	//	Version
	//=============================================================================
	virtual DWORD GetDbVer(
		const TCHAR* pszName
		) = 0;

	//=============================================================================
	//Function:     SetDbVer
	//Description:	Set Database version
	//
	//Parameter:
	//	pszName		- The login account name
	//	dwVer		- Version
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL SetDbVer(
		const TCHAR* pszName,
		DWORD dwVer) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enum the logined users
	//
	//Parameter:
	//	pfnCb		- Callback
	//	pvUserData	- User data
	//
	//Return:
	//	Count
	//=============================================================================
	virtual DWORD Enum( 
		PFN_LoginUserCb pfnCb,
		void*		    pvUserData
		) = 0;

	//=============================================================================
	//Function:     UpdateFlag
	//Description:	Update flag
	//
	//Parameter:
	//	dwFlag		- Flag
	//	bSet		- TRUE is set, else is clear
	//
	//Return:
	//	TRUE	- Succeeded
	//	FALSE	- Failed
	//=============================================================================
	virtual BOOL UpdateFlag(QEID qeid, 
		DWORD dwFlag, 
		BOOL bSet = TRUE
		) = 0;

	//=============================================================================
	//Function:     Set
	//Description:	Set login information
	//
	//Parameter:
	//	qeid		- Employee id
	//	pszName		- The login account name
	//	pszPsw		- Password
	//  dwFlag		- Flag
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int  Set(QEID qeid, 
		const TCHAR* pszName, 
		const TCHAR* pszPsw, 
		DWORD dwFlag
		) = 0;

	virtual QEID Get(const TCHAR* pszName) = 0;
};

#endif // __EIMENGINE_LOGIN_USER_MANAGER_HEADER_2014_07_15_YFGZ__