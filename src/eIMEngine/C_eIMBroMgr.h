#ifndef C_EIM_BRO_MGR_H
#define C_EIM_BRO_MGR_H

#include "eIMEngine/IeIMBroMgr.h"

class C_eIMBroMgr: public I_EIMBroMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMSMSMgr)

	C_eIMBroMgr(void);
	~C_eIMBroMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	virtual int  Set(PS_BroInfo psBro, const TCHAR* pszXml);

	virtual int SetAlert(PS_BroInfo psBro, const TCHAR* pszXml);
	//////////////////////////////////////////////////////////////////////////////

	/*virtual DWORD Enum(PS_SMSEnum psCond, PFN_CountCb pfnCountCb, void*	pvUserData);

	virtual DWORD GetCount(QSID QeidOrPhone, int iType);

	virtual DWORD Enum( PS_SMSEnum psCond,PFN_BroMsgCb pfnMsgCb,void* pvUserData);*/
};

#endif