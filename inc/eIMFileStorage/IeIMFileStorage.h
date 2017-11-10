#ifndef __I_EIMFILE_STORAGE_2013_11_12
#define __I_EIMFILE_STORAGE_2013_11_12

#include "public\plugin.h"
#include "eIMFileStorage\FileStorageUtil.h"

#define INAME_EIMFILESTORAGE		_T("SXIT.EIMFileStorage")		// EIMInternetFile interface name

class __declspec(novtable) I_eIMFileStorage : public I_EIMUnknown
{
public:
	/*virtual int Download(TCHAR *pszFileName, TCHAR *pszServerPath, TCHAR *pszLocalPath, UINT64 Qfid, E_EIMEncrpt eEncrpty=eInterFile_NoEncrpt) = 0;

	virtual int Upload(TCHAR *pszFileName, TCHAR *pszServerPath, TCHAR *pszLocalPath, int i32FileType, UINT64 Qfid, E_EIMEncrpt eEncrpty=eInterFile_NoEncrpt) = 0;*/

	//=============================================================================
	//Function:     Download
	//Description:	Download a file
	//Parameter:
	//	psFileInfo   - File information
	//   
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int Download(PS_EIMFileInfo psFileInfo) = 0;

	//=============================================================================
	//Function:     Upload
	//Description:	Upload a file
	//Parameter:
	//	psFileInfo   - File information
	//   
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int Upload(PS_EIMFileInfo psFileInfo) = 0;

	//=============================================================================
	//Function:     Stop
	//Description:	Stop the task
	//
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int Stop() = 0;

	//=============================================================================
	//Function:     GetTaskState
	//Description:	Get task state
	//Parameter:
	//	sStat   - Return the state
	//   
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int GetTaskState(S_EIMStatNotify &sStat) = 0;

	//=============================================================================
	//Function:     StatNotify
	//Description:	Set the notify callback 
	//Parameter:
	//	pfn_stat   - State notify callback
	//  pvUserData - User data
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int StatNotify(PFN_StatNotify pfn_stat, void *pvUserData) = 0;

	//=============================================================================
	//Function:     GetTextByVoice
	//Description:	Get the text result by voice to text
	//Parameter:
	//	psFileInfo   - voice file information
	//  pszResult  	 - Result
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int GetTextByVoice(PS_EIMFileInfo psFileInfo, eIMString* pszResult) = 0;
};

#endif