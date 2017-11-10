#ifndef EIMENGINE_IMPORT_MSGMGR_HEADER_H
#define EIMENGINE_IMPORT_MSGMGR_HEADER_H


typedef int (__stdcall* PFN_RecordsAll)(void* pvRetData, void* pvUserData);

class __declspec(novtable) I_EIMImportMsgMgr : public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     ImportMsg
	//Description:	Import message
	//
	//Parameter:
	//	lpszFullPath  - The backup message path
	//	i32DbType     - Database type
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL  ImportMsg(LPCTSTR lpszFullPath, int i32DbType) = 0;
};

#endif