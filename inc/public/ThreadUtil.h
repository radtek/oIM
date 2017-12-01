// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Thread helper function
// 
// C_AutoLock is a helper class for multi-thread control
// Usage:
// {	// Left brace to begin the C_AutoLock scope
//		C_AutoLock AutoLock(m_csKernel);
//
//		// Do any multi-thread resource here...
//
//	}	// Right brace to end the C_AutoLock scope, release critical section
// 
// Auth: LongWQ
// Date: 2013/12/21 19:14:07
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __THREAD_UTIL_2017_12_01_BY_YFGZ__
#define __THREAD_UTIL_2017_12_01_BY_YFGZ__


class C_AutoLock
{
private:
	CRITICAL_SECTION& m_csMsgQuene;

public:
	C_AutoLock(CRITICAL_SECTION& csMsgQuene)
		: m_csMsgQuene(csMsgQuene)
	{
		EnterCriticalSection(&m_csMsgQuene);
	}

	~C_AutoLock()
	{
		LeaveCriticalSection(&m_csMsgQuene);
	}
};

class C_Lock
{
private:
	CRITICAL_SECTION m_cs;

public:
	C_Lock()
	{
		InitializeCriticalSection(&m_cs);
	}

	~C_Lock()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}
};

#endif	// __THREAD_UTIL_2017_12_01_BY_YFGZ__