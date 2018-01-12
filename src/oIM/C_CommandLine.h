#pragma once
#include "public\C_CommandLineBase.h"

typedef UINT64 QFID;
typedef UINT64 QSID;
typedef UINT64 QMID;

#define args C_CommandLine::GetObject()

class C_CommandLine: public C_CommandLineBase
{
private:
	C_CommandLine(void);
	inline BOOL	Parse(LPWSTR* pszArgs, int nArgs);

public:
	friend class CMainDlg;
	~C_CommandLine(void);
	static C_CommandLine& GetObject();

private:
};

