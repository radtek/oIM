// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMCmd is the engine function.
//   UI level use the XML string.
//   Each Command have two data:
//   1. Original data field, Engine internal used
//   2. XML string data value, UI level used
// 
//   The Database class is the friend class of the command
//
//	Thread execute the cmd order as:
//	SendThread or RecvThread => [DbThread] => [UIThread]
//
// Auth: yfgz
// Date:    2013/11/15 16:23:03 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_COMMAND_INTERFACE_HEADER_2013_12_25_YFGZ__
#define __EIMENGINE_COMMAND_INTERFACE_HEADER_2013_12_25_YFGZ__

#include "Public\Plugin.h"
#include "sqllite\I_SQLite.h"

#define INAME_EIMCMD				_T("SXIT.eIMCmd")				// I_EIMCmd interface name

// CMD flag define 
#define CMD_FLAG_NON				( 0x00000000 )					// None
#define CMD_FLAG_THREAD_DB			( 0x00000001 )					// Need in db thread to store
#define CMD_FLAG_THREAD_UI			( 0x00000002 )					// Need in UI thread to dispatch	
#define CMD_FLAG_THREAD_ALL			( CMD_FLAG_THREAD_DB | CMD_FLAG_THREAD_UI )
#define CMD_FLAG_THREAD_SEND		( 0x00000004 )					// Send cmd used by EVENT_SEND_CMD, in Send Thread
#define CMD_FLAG_THREAD_CURL		( 0x00000008 )					// Is Curl thread
#define CMD_FLAG_ABORT				( 0x00000100 )					// Abort cmd
#define CMD_FLAG_NO_FROMXML			( 0x00000200 )					// No FromXml
#define CMD_FLAG_NO_TOXML			( 0x00000400 )					// No ToXml
#define CMD_FLAG_NO_XML				(CMD_FLAG_NO_FROMXML | CMD_FLAG_NO_TOXML)
#define CMD_FLAG_NO_XML_DB			(CMD_FLAG_NO_XML | CMD_FLAG_THREAD_DB)
#define CMD_FLAG_MASK_ALL			( 0xFFFFFFFF )					// All flag's mask

// Msg flag
#define CMD_FLAG_MSG_TEXT			( 0x00010000 )					// Text message
#define CMD_FLAG_MSG_AUTO			( 0x00020000 )					// Auto response message
#define CMD_FLAG_MSG_IMAGE			( 0x00040000 )					// Image flag
#define CMD_FLAG_MSG_FILE			( 0x00080000 )					// File flag
#define CMD_FLAG_MSG_ANNEX			( 0x00100000 )					// Annex flag
#define CMD_FLAG_MSG_ANNEX_ACK		( 0x00200000 )					// Annex ack
#define CMD_FLAG_MSG_LONG			( 0x00400000 )					// Long message
#define CMD_FLAG_MSG_P2P			( 0x00800000 )					// P2P message
#define CMD_FLAG_MSG_VOICE			( 0x01000000 )					// Voice message
#define CMD_FLAG_MSG_VIDEO			( 0x02000000 )					// Video message
#define CMD_FLAG_MSG_EMOT			( 0x04000000 )					// Emot message
#define CMD_FLAG_MSG_WAIT_FILE		( 0x08000000 )					// Need wait to finished file transfer
#define CMD_FLAG_MSG_RDP			( 0x10000000 )					// RDP
#define CMD_FLAG_MSG_JSON			( 0x20000000 )					// JSON: WebApp, MapLocation, RedPacket
#define CMD_FLAG_MSG_MASK			( 0x0FFF0000 )					// Message flag mask
#define CMD_FLAG_TRANSFER			( CMD_FLAG_MSG_IMAGE | CMD_FLAG_MSG_FILE | CMD_FLAG_MSG_ANNEX | CMD_FLAG_MSG_LONG | CMD_FLAG_MSG_P2P | CMD_FLAG_MSG_VOICE | CMD_FLAG_MSG_VIDEO)	// File transfer flag mask
//#define CMD_FLAG_UPDATE_STATUS		( 0x00000000 )					// An specify mask to update file transfer status

class __declspec(novtable) I_EIMCmd: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     GetCmdId
	//Description:	Get command id
	//
	//Return:
	//		Return command id       
	//=============================================================================
	virtual int			GetCmdId() = 0;

	//=============================================================================
	//Function:     GetResult
	//Description:	Get the result of execute the command
	//
	//Return:
	//		HRESULT
	//=============================================================================
	virtual int			GetResult() = 0;

	//=============================================================================
	//Function:     GetErrorDesc
	//Description:	Get the error description
	//
	//Return:
	//		Error description
	//=============================================================================
	virtual const TCHAR*GetErrorDesc(int i32Error=0) = 0;

	//=============================================================================
	//Function:     GetXml
	//Description:	Get the xml formation of the command
	//
	//Return:
	//		The xml formation of the command if exist
	//=============================================================================
	virtual const TCHAR*GetXml() = 0;

	//=============================================================================
	//Function:     GetData
	//Description:	Get the Data of the command
	//
	//Return:
	//		The Data of the command
	//=============================================================================
	virtual const void* GetData() = 0;

	//=============================================================================
	//Function:     DoCmd
	//Description:	Execute the command with the server by ClientAgent, in _DoSendThread
	//
	//Parameter:
	//	pILog     - Logger interface
	//  pICa	  - ClientAgent interface
	//	pbAbort	  - Abort flag, TRUE to Abort the command
	//Return:
	//		Return HRESULT         
	//=============================================================================
	virtual int			DoCmd(I_EIMLogger* pILog, void* pICa,			BOOL* pbAbort = NULL) = 0;	// Only can be call it by engine

	//=============================================================================
	//Function:     DoCmd
	//Description:	Execute the command with the SQLite database in _DoDbThread to save data
	//
	//Parameter:
	//	pILog     - Logger interface
	//  pIDb	  - SQLite3 database interface
	//	pbAbort	  - Abort flag, TRUE to Abort the command
	//Return:
	//		Return HRESULT         
	//=============================================================================
	virtual int			DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,		BOOL* pbAbort = NULL) = 0;	// Only can be call it by engine

	//=============================================================================
	//Function:     DoCmd
	//Description:	Execute the command with the SQLite database in _DoDbThread to save data
	//
	//Parameter:
	//	pILog     - Logger interface
	//  pIEMgr	  - Event manager interface
	//  pIBIFile  - Batch File transfer interface
	//	pbAbort	  - Abort flag, TRUE to Abort the command
	//Return:
	//		Return HRESULT         
	//=============================================================================
	virtual int			DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pIBIFile, BOOL* pbAbort = NULL) = 0;		// Only can be call it by engine

	//=============================================================================
	//Function:     GetFlag
	//Description:	Get the flag
	//
	//Parameter:
	//	dwMask		- Mask to be get the flag
	//
	//Return:
	//		The Value
	//=============================================================================
	virtual DWORD		GetFlag(DWORD dwMask = CMD_FLAG_MASK_ALL) = 0;

	//=============================================================================
	//Function:     SetFlag
	//Description:	Set the flag
	//
	//Parameter:
	//	dwFlag		- Flag to be set
	//	dwMask		- Mask to be set the flag
	//
	//Return:
	//		The result
	//=============================================================================
	virtual DWORD		SetFlag(DWORD dwFlag, DWORD dwMask = CMD_FLAG_MASK_ALL) = 0;

	//=============================================================================
	//Function:     GetAttribute
	//Description:	Get the Attribute
	//
	//Parameter:
	//	dwAttrId	- Attribute Id
	//
	//Return:
	//		The attribute
	//=============================================================================
	virtual TCHAR*		GetAttribute(UINT64 dwAttrId) = 0;

	//=============================================================================
	//Function:     SetAttribute
	//Description:	Set the Attribute
	//
	//Parameter:
	//	dwAttrId	- Attribute Id
	//	pszValue	- The value of attribute
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL		SetAttribute(UINT64 dwAttrId, TCHAR* pszValue) = 0;

	//=============================================================================
	//Function:     SetBusinessId
	//Description:	Set Business Id for timeout
	//
	//Parameter:
	//	dwBusinessId	- Business Id
	//
	//Return:
	//	None
	//=============================================================================
	virtual void SetBusinessId(DWORD dwBusinessId) = 0;

	//=============================================================================
	//Function:     GetBusinessId
	//Description:	Get Business Id
	//
	//Return:
	//	Business Id
	//=============================================================================
	virtual DWORD GetBusinessId() = 0;

	//=============================================================================
	//Function:     SetBusinessTypeId
	//Description:	Set SetBusiness Type Id for timeout
	//
	//Parameter:
	//	u64BusinessTypeId	- Business type Id
	//
	//Return:
	//	None
	//=============================================================================
	virtual void SetBusinessTypeId(UINT64 u64BusinessTypeId) = 0;

	//=============================================================================
	//Function:     GetBusinessTypeId
	//Description:	Get Business type Id
	//
	//Return:
	//	Business type Id
	//=============================================================================
	virtual UINT64 GetBusinessTypeId() = 0;
};


#endif // __EIMENGINE_COMMAND_INTERFACE_HEADER_2013_12_25_YFGZ__