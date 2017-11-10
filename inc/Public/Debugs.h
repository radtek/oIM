// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Debug helper function
//   TRACE_		Used output trace information, like TRACE in MFC
//   ASSERT_		Used assert the expression in DEBUG version
//   VERIFY_		Used assert the expression in any version
// 
// 
// Auth: LongWQ
// Date: 2013/12/21 19:08:24
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_DEBUGS_2013_11_29_BY_YFGZ__
#define __EIM_DEBUGS_2013_11_29_BY_YFGZ__

#include <Windows.h>


///////////////////////////////////////////////////////////////////////////////
// A little LOG TRACE function
// Default log to output window of debugger, use __NO_LOG_TRACE_API__ to disable it
// Also can be output to STDOUT by __LOG_TRACE_STDOUT__
#ifndef NO_USE_LOG_TRACE
void __cdecl LogTrace( const TCHAR*	pszFormat, ... );
#  if defined(UNICODE) || defined(_UNICODE)
#    define TRACE_FMT "[%S L%06d] "
#  else
#    define TRACE_FMT "[%s L%06d] "
#  endif

#  define TRACE_(FMT, ...) do{ LogTrace( _T(TRACE_FMT)##FMT, __FUNCTION__, __LINE__, ##__VA_ARGS__); }while(0)
#else
#  define TRACE_(FMT, ...) 
#endif

#ifndef NO_USE_EIM_ASSERT 
#  if defined(DEBUG) || defined(_DEBUG)
#    define ASSERT_(exp) do{ if(!(exp)) { TRACE_(_T("Assert failed!")); DebugBreak(); } }while(0)
#  else
#    define ASSERT_(exp)
#  endif
#endif

#ifndef NO_USE_EIM_VERIFY
#  define VERIFY_(exp) do{ if(!(exp)) { TRACE_(_T("Verify failed!")); DebugBreak()} }while(0)
#endif


#include "Debugs.inl"

#endif	// __EIM_DEBUGS_2013_11_29_BY_YFGZ__

