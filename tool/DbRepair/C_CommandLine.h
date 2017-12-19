#pragma once
//#include "sqllite\I_SQLite.h"
#include "public\C_CommandLineBase.h"

#define args C_CommandLine::GetObject()
typedef struct tagAccountInfo
{
	DWORD	dwQeid;
	SStringT szAccount;
	SStringT szPath;
}S_AccountInfo, *PS_AccountInfo;

class C_CommandLine: public C_CommandLineBase
{
private:
	C_CommandLine(void);
	inline BOOL	Parse(LPWSTR* pszArgs, int nArgs);

public:
	friend class CMainDlg;
	~C_CommandLine(void);
	static C_CommandLine& GetObject();
	BOOL InitDb(S_AccountInfo& sAInfo);

private:
	S_AccountInfo m_AccountInfo;
	
};

