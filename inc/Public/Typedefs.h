// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Common type define 
// 
// 
// 
// Auth: LongWQ
// Date: 18:34 2014/4/15
// 
// History: 
//    
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_COMMON_TYPEDEFS_2014_04_15_BY_YFGZ__
#define __EIM_COMMON_TYPEDEFS_2014_04_15_BY_YFGZ__

#include <vector>
#include <string>

////////////////// string ///////////////////////////////////////////////////////////
#if defined(UNICODE) || defined(_UNICODE)
#  define eIMString std::wstring
#else
#  define eIMString std::string
#endif

#define eIMStringA	std::string
#define eIMStringW	std::wstring

typedef int                 BOOL,   *PBOOL;
typedef unsigned char       BYTE,   *PBYTE;
typedef unsigned short      WORD,   *PWORD;
typedef unsigned long       DWORD,  *PDWORD;
typedef float               FLOAT,  *PFLOAT;
typedef signed int          INT,    *PINT;

///////////////////////////////////////////////////////////////////////////////
#define TIME_BASE_SECOND	(1397600000U)		// 1397619216: ~2014/4/16 08:00


typedef DWORD				QEID, *PQEID;		// Employee ID
typedef DWORD               QDID, *PQDID;		// Department ID
typedef DWORD				QGID, *PQGID;		// Group ID
typedef unsigned __int64	QFID, *PQFID;		// File ID
typedef unsigned __int64	QSID, *PQSID;		// Session ID
typedef unsigned __int64	QMID, *PQMID;		// Message ID

typedef std::vector<eIMString>             RechEditMessage;       //Rechedit message
typedef RechEditMessage::iterator			RechEditMessageIt;	// Rechedit message vector's iterator

//typedef std::vector<eIMStringA>              VectStringMember;     //Session member's name
//typedef VectStringMember::iterator          VectStringMemberIt;   //VectStringMember's iterator

typedef std::vector<QEID>					VectSessionMember;		// Session member vector
typedef VectSessionMember::iterator			VectSessionMemberIt;	// Session member vector's iterator
typedef VectSessionMember::const_iterator	VectSessionMemberIt_;	// Session member vector's const iterator	
// Must be match TERMINAL_TYPE in ClientAgent/protocol.h
typedef enum tagIMDevType
{
	eDEV_TYPE_ANDROID = 1,	// Android device
	eDEV_TYPE_IOS,			// IOS device
	eDEV_TYPE_PC,			// PC device
}S_DevType, *PS_DevType;


#endif	// __EIM_COMMON_TYPEDEFS_2014_04_15_BY_YFGZ__

