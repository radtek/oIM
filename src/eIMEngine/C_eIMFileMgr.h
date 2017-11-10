#ifndef __EIMENGINE_FILE_MANAGER_HEADER_2014_08_22_YFGZ__
#define __EIMENGINE_FILE_MANAGER_HEADER_2014_08_22_YFGZ__

#pragma once
#include "stdafx.h"
#include "eIMEngine\IeIMFileMgr.h"

#define FILE_KEY_MAX	( 32 + 1 )
typedef struct tagOfflineFile		// Offline file 
{
	QFID	Qfid;					// File id(Unique)
	QMID	Qmid;					// Message id
	UINT64	u64Size;				// File size
	QEID	eidSender;				// Sender id
	DWORD	dwTimestamp;			// Message time of the file
	DWORD	dwFlag;					// Flag 
	char	szKey[FILE_KEY_MAX];	// File key, Key[6] + ext 
	char	szName[MAX_PATH];		// File name(+ext)
	char	szLocalPath[MAX_PATH];	// Local file full path
}S_OffFile, *PS_OffFile;

#define TOKEN_SIZE_MAX	( 32 + 1 )
typedef struct tagP2pFile			// P2p file
{
	QFID	Qfid;					// File id(Unique)
	QMID	Qmid;					// Message id
	UINT64	u64Size;				// File size
	QEID	eidRecver;				// Receiver id
	QEID	eidSender;				// Sender id
	DWORD	dwTimestamp;			// Message time of the file
	DWORD	dwFlag;					// Flag 
	char	szToken[TOKEN_SIZE_MAX];// Token
	char	szName[MAX_PATH];		// File name(+ext)
	char	szLocalPath[MAX_PATH];	// Local file full path
}S_P2pFile, *PS_P2pFile;

class C_eIMFileMgr: public I_EIMFileMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMMsgMgr)

	C_eIMFileMgr(void);
	~C_eIMFileMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	BOOL	Get(QFID Qfid, PS_OffFile psOffFile);
	BOOL	Get(QFID Qfid, PS_P2pFile psP2pFile);
	int		Set(PS_OffFile psOffFile);
	int		Set(PS_P2pFile psP2pFile);
	BOOL	UpdateFlag(QFID Qfid, DWORD dwFlag, BOOL bSet, BOOL bP2p = FALSE);

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
		BOOL	bP2p = FALSE);

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
		);

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
		);

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
		);

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate the file info
	//
	//Parameter:
	//  sEnumInfo				- TRUE OR FALSE
	//  pfnFileMsgDataCb		- file msg info callback function
	//	pvUserData				- User parameter passed to callback function
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD Enum(
		S_FilesEnum&		sEnumInfo, 
		PFN_FileMsgDataCb	pfnFileMsgDataCb,
		void*				pvUserData
		);

	//=============================================================================
	//Function:     GetCount
	//Description:	Get the count of the file info
	//
	//Parameter:
	//  sEnumInfo				- S_FilesEnum info
	//Return:
	//		Return the count             
	//=============================================================================
	virtual DWORD GetCount(S_FilesEnum&	sEnumInfo);

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
		BOOL	bP2p = FALSE);
};

#endif	// __EIMENGINE_FILE_MANAGER_HEADER_2014_08_22_YFGZ__
