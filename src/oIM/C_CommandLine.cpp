#include "StdAfx.h"
#include "C_CommandLine.h"
#include <Winerror.h>
#include <com-loader.hpp>

C_CommandLine& C_CommandLine::GetObject()
{
	static C_CommandLine s_CommandLine;
	return s_CommandLine;
}

C_CommandLine::C_CommandLine(void)
	: C_CommandLineBase()
{
}

C_CommandLine::~C_CommandLine(void)
{
}

inline BOOL C_CommandLine::Parse(LPWSTR* pszArgs, int nArgs)
{
	int i32Index = 0;
	for ( ; i32Index < nArgs; i32Index++ )
	{
		LPWSTR pszArg = pszArgs[i32Index];
		LPWSTR pszNext=(i32Index + 1 < nArgs) ? pszArgs[i32Index + 1] : NULL;
		if ( IsOption(pszArg, _T("lang")) )
		{
			SetLang(GetParamValue(pszArg, pszNext, i32Index));
		}
		else if ( IsOption(pszArg, _T("scale")) )
		{
			SetScale(GetParamValueInt(pszArg, pszNext, i32Index));
		}
		else if ( IsOption(pszArg, _T("taskbar")) )
		{
			m_bShowInTaskbar = GetParamValueInt(pszArg, pszNext, i32Index);
		}
		else if ( IsOption(pszArg, _T("h")) || IsOption(pszArg, _T("?")) )
		{
			return FALSE;
		}
	}

	return TRUE;
}
