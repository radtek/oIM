#pragma once

#include "eIMEngine\IeIMLoginUserMgr.h"

class C_eIMLoginUserMgr: public I_EIMLoginUserMgr
{
private: 
	DWORD	   m_dwRef;
	I_SQLite3* m_pIDb;

public:
	DECALRE_PLUGIN_(C_eIMLoginUserMgr)

	C_eIMLoginUserMgr(void);
	~C_eIMLoginUserMgr(void);

	static int __stdcall LoginUserCb(const TCHAR* pszName, DWORD dwFlag, void* pvUserData);
	BOOL Init(I_SQLite3* pIDb = NULL);
	BOOL Uninit();
	BOOL UpdateFlag(QEID qeid, DWORD dwFlag, BOOL bSet = TRUE);
	int  Set(QEID qeid, const TCHAR* pszName, const TCHAR* pszPsw, DWORD dwFlag);
	BOOL Get(const TCHAR* pszName, eIMString& szPsw);
	bool DES(eIMStringA& szDataA, bool bEncrypt = true, const char* pszKey="yf~gz@16\t03\a11");

	///////////////////////////////////////////////////////////////////////////
	virtual BOOL  Delete(const TCHAR* pszName);
	virtual DWORD GetDbVer(const TCHAR* pszName);
	virtual BOOL  SetDbVer(const TCHAR* pszName, DWORD dwVer);
	virtual DWORD Enum(PFN_LoginUserCb pfnCb, void*	pvUserData);
	virtual QEID  Get(const TCHAR* pszName);

};

