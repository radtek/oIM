﻿#pragma once

#include "utilities-def.h"

#include <windows.h>
namespace SOUI
{
    void UTILITIES_API STraceA(LPCSTR pstrFormat, ...);
    void UTILITIES_API STraceW(LPCWSTR pstrFormat, ...);
}//end of namespace SOUI

#define STRACEW SOUI::STraceW
#define STRACEA SOUI::STraceA

//#ifdef _UNICODE
//#define STRACE STRACEW
//#else
//#define STRACE STRACEA
//#endif

#ifndef NO_USE_LOG_TRACE
# if defined(UNICODE) || defined(_UNICODE)
#   define STRACE(FMT, ...) do{ STRACEW( _T("[%S L%06d] ")##FMT, __FUNCTION__, __LINE__, ##__VA_ARGS__); }while(0)
# else
#   define STRACE(FMT, ...) do{ STRACEA( _T("[%s L%06d] ")##FMT, __FUNCTION__, __LINE__, ##__VA_ARGS__); }while(0)
# endif
#else
#  define STRACE(FMT, ...) 
#endif