// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMFileMgr is the engine function DLL
// 
// Auth: yfgz
// Date:    2014/08/20 20:43:03 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_FILE_MANAGER_HEADER_2014_08_20_YFGZ__
#define __EIMENGINE_FILE_MANAGER_HEADER_2014_08_20_YFGZ__

#include "Public\Plugin.h"

// Return 0 to break Enumerate
typedef int (__stdcall* PFN_OffFileCb)(DWORD dwTimestamp, LPCTSTR lpszFile, QEID qeidSender, void* pvUserData);
typedef int (__stdcall* PFN_P2pFileCb)(DWORD dwTimestamp, LPCTSTR lpszFile, QEID qeidSender, QEID qeidReceiver, void* pvUserData);

typedef enum tagDelFileFlag
{
	eDEL_RECORDS = 0x00080000,	// Only delete records(MUST be equal to DB_FLAG_DELETED for unified management)
	eDEL_FILE	 = 0x00000002,	// Only delete file
	eDEL_IMAGE	 = 0x00000004,	// Only delete image
	eDEL_FILE_IMAGE = eDEL_FILE | eDEL_IMAGE,
	eDEL_FILE_IMAGE_RECORDS = eDEL_FILE_IMAGE | eDEL_RECORDS,
}E_DelFileFlag, *PE_DelFileFlag;


#define FILE_KEYLEN_MAX		( 30 )		// File key max len
typedef struct tagFileMsgInfo
{
	QFID			fid;						// File Id
	QMID			msgid;						// MsgId
	TCHAR			szName[MAX_PATH];			// File name
	TCHAR			szLocalPath[MAX_PATH];		// Local file path
	QEID			qSenderID;					// QEID of sender
	UINT64			u64FileSize;				// File size
	DWORD			datatime;					// Timestamp
	TCHAR			szKey[FILE_KEYLEN_MAX];		// File Key
	TCHAR			szToken[FILE_KEYLEN_MAX];	// File token
}S_FileMsgInfo, *PS_FileMsgInfo;
typedef int (__stdcall* PFN_FileMsgDataCb)(S_FileMsgInfo& rFileMsgInfo, void* pvUserData);

typedef struct tagFilesEnum
{
	BOOL			bIsSender;						// Send flag
	QEID			qeid;							// QEID
	E_MsgOperate    eOperate;						// just use date oper
	DWORD			dwOffset;						// Offset
	DWORD			dwLimit;						// Limit, 0 to ignore it
	DWORD			dwStart;						// Start date
	DWORD			dwEnd;							// End date
	TCHAR			szContent[MSG_CONTENT_MAX];		// Search content
	TCHAR			szContacts[MSG_CONTACTS_MAX];	// Search contacts
}S_FilesEnum, *PS_FilesEnum;

class __declspec(novtable) I_EIMFileMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     GetCount
	//Description:	Get the message count by date range
	//
	//Parameter:
	//	dwStart    - Start date, 0 to min value
	//	dwEnd      - End date, 0xFFFFFFFF to max value
	//	bP2p	   - TRUE: is P2P file, else is offline file, 
	//
	//Return:
	//		Return the count of the message
	//=============================================================================
	virtual DWORD GetCount(
		DWORD	dwStart, 
		DWORD	dwEnd,
		BOOL	bP2p = FALSE) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the offline files by timestamp
	//
	//Parameter:
	//	dwStart		 - Start date, 0 to min value
	//	dwEnd		 - End date, 0xFFFFFFFF to max value
	//	pfnOffFileCb - Offline file callback function
	//	pvUserData	 - User parameter passed to callback function

	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum( 
		DWORD	  dwStart,
		DWORD	  dwEnd,
		PFN_OffFileCb pfnOffFileCb,
		void*	  pvUserData
		) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the p2p files by timestamp
	//
	//Parameter:
	//	dwStart		 - Start date, 0 to min value
	//	dwEnd		 - End date, 0xFFFFFFFF to max value
	//	pfnP2pFileCb - P2p file callback function 
	//	pvUserData   - User parameter passed to callback function
	//
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum(
		DWORD			dwStart,
		DWORD			dwEnd,
		PFN_P2pFileCb	pfnP2pFileCb,
		void*			pvUserData
		) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the file info
	//
	//Parameter:
	//  bIsSendFile				- TRUE OR FALSE
	//  pfnFileMsgDataCb		- file msg info callback function
	//	pvUserData				- User parameter passed to callback function
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum(
		BOOL				bIsSendFile, 
		PFN_FileMsgDataCb	pfnFileMsgDataCb,
		void*				pvUserData
		) = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the file info
	//
	//Parameter:
	//  sEnumInfo				- S_FilesEnum info
	//  pfnFileMsgDataCb		- file msg info callback function
	//	pvUserData				- User parameter passed to callback function
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum(
		S_FilesEnum&		sEnumInfo, 
		PFN_FileMsgDataCb	pfnFileMsgDataCb,
		void*				pvUserData
		) = 0;

	//=============================================================================
	//Function:     GetCount
	//Description:	Get the count of the file info
	//
	//Parameter:
	//  sEnumInfo				- S_FilesEnum info
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD GetCount(S_FilesEnum&	sEnumInfo) = 0;


	//=============================================================================
	//Function:     Delete
	//Description:	Delete the offline/p2p file records, can be include files
	//
	//Parameter:
	//	dwStart		- Start date, 0 to min value
	//	dwEnd		- End date, 0xFFFFFFFF to max value
	//	eFlag		- Delete flag
	//	bP2p		- TRUE: is P2P file, else is offline file, 
	//
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- FALSE
	//=============================================================================
	virtual BOOL Delete( 
		DWORD	dwStart,
		DWORD	dwEnd,
		E_DelFileFlag	eFlag,
		BOOL	bP2p = FALSE) = 0;

};

#endif // __EIMENGINE_FILE_MANAGER_HEADER_2014_08_20_YFGZ__