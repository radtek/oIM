#include "trace.h"
#include <stdio.h>

#define MAX_TRACEBUF    502

#include "souicoll.h"

namespace SOUI
{
void  STraceA(LPCSTR pstrFormat, ...)
{
    char szBuffer[MAX_TRACEBUF] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    int len = _vscprintf( pstrFormat, args ) // _vscprintf doesn't count
        + 1; // terminating '\0'
    if(len<MAX_TRACEBUF-1)
    {
        vsprintf_s(szBuffer, ARRAYSIZE(szBuffer)-1, pstrFormat, args);
        strcat(szBuffer, "\n");
        ::OutputDebugStringA(szBuffer);
    }
    va_end(args);
}

void  STraceW(LPCWSTR pstrFormat, ...)
{
    wchar_t szBuffer[MAX_TRACEBUF] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    int len = _vscwprintf( pstrFormat, args ) // _vscprintf doesn't count
        + 1; // terminating '\0'
    if(len<MAX_TRACEBUF-1)
    {
        vswprintf_s(szBuffer, ARRAYSIZE(szBuffer)-1, pstrFormat, args);
        wcscat(szBuffer, L"\n");
        ::OutputDebugStringW(szBuffer);
    }
    va_end(args);
}

}