#ifndef C_EIM_SMS_MGR_H
#define C_EIM_SMS_MGR_H

#include "eIMEngine/IeIMSMSMgr.h"

class C_eIMSMSMgr: public I_EIMSMSMgr
{
private:
	DWORD		m_dwRef;
	I_SQLite3*	m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMSMSMgr)

	C_eIMSMSMgr(void);
	~C_eIMSMSMgr(void);

	virtual BOOL Init(I_SQLite3* pIContsDb = NULL);
	virtual BOOL Uninit();

	virtual QSID Get(QMID qmid);
	virtual BOOL Get(QMID qmid, S_Msg& sMsg, eIMStringA& pszXmlA);
	virtual int  Set(PS_Sms psMsg, const TCHAR* pszXml);	// pszXml is <Message />
	//////////////////////////////////////////////////////////////////////////////

	virtual DWORD Enum(PS_SMSEnum psCond, PFN_CountCb pfnCountCb, void*	pvUserData);

	virtual DWORD GetCount(QSID QeidOrPhone, int iType);

	virtual DWORD Enum( PS_SMSEnum psCond,PFN_SMSMsgCb pfnMsgCb,void* pvUserData);
};

#endif