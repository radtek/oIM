#ifndef I_EIM_UPDATER
#define I_EIM_UPDATER
#include "eIMUpdData.h"

#define INAME_EIM_UPDATER _T("SXIT.EIMUpdater")

#define UPDATEDB_FLAG_GLOBAL  (0x00000001)   //DB Global Flag

class __declspec(novtable) I_eIMUpdater : public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Updater
	//Description:	Update
	//Parameter:
	//  sUpdateInfo   - Update information
	//   
	//Return:
	//  HRESULT
	//=============================================================================
	virtual int Updater(const S_EIMUpdaterInfo &sUpdateInfo) = 0;
	
	//=============================================================================
	//Function:     SetNewFunIntroPath
	//Description:	Set release notice path by extention function
	//Parameter:
	//	infoPath   - release notice path
	//   
	//Return:
	//  None
	//=============================================================================
	virtual void SetNewFunIntroPath(eIMString& infoPath) = 0;
};

#endif