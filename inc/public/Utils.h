// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Utils.h header file, used for eIM projects to help simplify
//   the work, it included all sub helper function or class.
//  
//  Plugin.h		- Plugin helper, defined the Plugin interface and helper class to use the plugin
//  Debugs.h		- Debug helper Macro or function
//  Errors.h		- Error code defined
//  StrUtil.h		- String helper function
//  PathUtils.h		- Path helper function
//  ThreadUtil.h	- Thread helper function
//  Configer.h		- INI configuration helper class
//  XmlConfiger.h	- XML configuration helper dlass
// 
// Auth: LongWQ
// Date: 2013/12/21 18:39:26
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////


#ifndef __OIM_UTIL_API_2017_11_22_BY_YFGZ__
#define __OIM_UTIL_API_2017_11_22_BY_YFGZ__

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include "TypeDefs.h"
#include "Errors.h"
#include "trace.h"


#if 0
# include "vld\vld.h"
#endif

// Count of array
#define COUNT_OF_ARRAY_( ARY )	( sizeof( ARY ) / sizeof( ARY[0] ) )
#define ZERO_ARRAY_( ARY )		do{ memset( (ARY), 0, sizeof(ARY) ); }while(0)
#define ZERO_STRUCT_( ST )		do{ memset(&(ST), 0, sizeof(ST) );  }while(0)

///////////////////////////////////////////////////////////////////////////////
// Free memory
#define SAFE_DELETE_PTR_( PTR )	do{ if( PTR ) { delete ( PTR );	( PTR ) = NULL; } }while(0)
#define SAFE_FREE_PTR_( PTR )	do{ if( PTR ) { free( PTR );	( PTR ) = NULL; } }while(0)

// Close handle
#define SAFE_CLOSE_FILE_(F)		do{ if(F) { fclose(F); (F) = NULL; }}while(0)
#define SAFE_CLOSE_HANDLE_(H)	do{ if(H) { ::CloseHandle((HANDLE)H); (H) = NULL; }}while(0)
#define SAFE_DELETE_OBJECT_(H)	do{ if(H) { ::DeleteObject((HGDIOBJ)H); (H) = NULL; }}while(0)
#define SAFE_DELETE_DC_(H)		do{ if(H) { ::DeleteDC((HDC)H); (H) = NULL; }}while(0)

// Release plugin interface, based I_EIMPlugin
#define SAFE_RELEASE_PLUGIN_(P) 	do{ if(P){(P)->FreePlugin(); (P)->Release(); (P)=NULL;}}while(0)

// Release interface, based I_EIMUnknown 
#define SAFE_RELEASE_INTERFACE_(P) 	do{ if(P){(P)->Release(); (P)=NULL;}}while(0)

// Safe send event(PEventMgr, Event, Param) 
#define SAFE_SEND_EVENT_(PEMGR, E, P) do {if (PEMGR) (PEMGR)->SendEvent((E), (P)); }while(0)

// Color reference[0xaabbggrr]
#define RGBA_(r,g,b,a)          ( (COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(b))<<16) | (((DWORD)(BYTE)(a))<<24)) )

// Check NULL pointer and return
#define CHECK_NULL_RET_(P, R)		do { if( !(P) ) { STRACE(_T("Parameter ") _T(#P) _T(" is NULL")); return (R); }}while(0)
#define CHECK_NULL_RETV_(P)			do { if( !(P) ) { STRACE(_T("Parameter ") _T(#P) _T(" is NULL")); return; }}while(0)
#define CHECK_FAILED_RET_(R,RET)	do { if( FAILED(R) ) { STRACE(_T("Result: 0x%08X"), (R)); return (RET); } }while(0)
#define CHECK_NULL_CONTINUE_(P)		do { if ( !(P) ) { ASSERT_(P); continue; } }while(0)

///////////////////////////////////////////////////////////////////////////////
// Bit operate
#define TEST_BIT_(VALUE, BITN) 	( ((VALUE) >> (BITN)) & 1 )
#define CLEAR_BIT_(VALUE, BITN)	( (VALUE) & ~(1 << (BITN)) )
#define GET_BIT_(VALUE, BITN)	TEST_BIT_(VALUE, BITN)
#define SET_BIT_(VALUE, BITN)	( (VALUE) | (1 << (BITN)) )

// Make four char to DWORD, char auto converter to uppercase
#define MAKE4CC(ch0, ch1, ch2, ch3)  ((DWORD)(BYTE)(toupper(ch3)) | ((DWORD)(BYTE)(toupper(ch2)) << 8) | ((DWORD)(BYTE)(toupper(ch1)) << 16) | ((DWORD)(BYTE)(toupper(ch0)) << 24 ))

///////////////////////////////////////////////////////////////////////////////
// Max, Mid, Min
#define MIN_(a, b) 				( (a) < (b) ? (a) : (b) )
#define MAX_(a, b) 				( (a) > (b) ? (a) : (b) )
#define MID_(a, b, c)			( (a) < (b) ? ((b) < (c) ? (b) : MIN_(a, c)) : ((b) > (c) ? (b) : MAX_(a, c)))

// swap two integer variable's value
#define SWAP_(a, b)             do{ a^=b; b^=a; a^=b; }while(0)

///////////////////////////////////////////////////////////////////////////////
// Compare V1 and V2, if V1 > V2 return RET_GT, else if V1 < V2 return RET_LS
#define CMP_NEQ_RET_(V1, V2, RET_GT, RET_LS) \
	do{ \
		if ( (V1) > (V2) ) return (RET_GT); \
		if ( (V1) < (V2) ) return (RET_LS); \
	}while(0)

// File API 
#if defined(UNICODE) || defined(_UNICODE)
# define _tfopen    _wfopen
#else
# define _tfopen    fopen
#endif 



#endif	// __OIM_UTIL_API_2017_11_22_BY_YFGZ__

