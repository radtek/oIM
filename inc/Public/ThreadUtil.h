// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Thread helper function
// 
// C_EIMAutoLock is a helper class for multi-thread control
// Usage:
// {	// Left brace to begin the C_EIMAutoLock scope
//		C_EIMAutoLock AutoLock(m_csKernel);
//
//		// Do any multi-thread resource here...
//
//	}	// Right brace to end the C_EIMAutoLock scope, release critical section
// 
// Auth: LongWQ
// Date: 2013/12/21 19:14:07
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_THREAD_UTIL_2013_10_23_BY_YFGZ__
#define __EIM_THREAD_UTIL_2013_10_23_BY_YFGZ__


class C_EIMAutoLock
{
private:
	CRITICAL_SECTION& m_csMsgQuene;

public:
	C_EIMAutoLock(CRITICAL_SECTION& csMsgQuene)
		: m_csMsgQuene(csMsgQuene)
	{
		EnterCriticalSection(&m_csMsgQuene);
	}

	~C_EIMAutoLock()
	{
		LeaveCriticalSection(&m_csMsgQuene);
	}
};

class C_EIMLock
{
private:
	CRITICAL_SECTION m_cs;

public:
	C_EIMLock()
	{
		InitializeCriticalSection(&m_cs);
	}

	~C_EIMLock()
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

#endif	// __EIM_THREAD_UTIL_2013_10_23_BY_YFGZ__