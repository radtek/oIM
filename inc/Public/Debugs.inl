#include <stdio.h>
//=============================================================================
//Function:     LogTrace
//Description:	Log variable parameter information
//
//Parameter:
//	pszFormat    - Format, referance printf 
//	argptr       - variable parameter
//
//Return:
//              None
//=============================================================================
inline void __cdecl LogTrace( const TCHAR*	pszFormat, ... )
{	
	if( pszFormat == NULL)							// Parameter error
		return;	

	const int	i32LogTraceSize = 2 * 1024;			// 2KB log buffer
	TCHAR		szLogTraceBuf[i32LogTraceSize + 2];	// Log trace buffer

	va_list args;
	va_start(args, pszFormat);
	int i32Ret = _vsntprintf( szLogTraceBuf, i32LogTraceSize, pszFormat, args );
	va_end(args);

	if(i32Ret < 0)
	{	// Truncated
		szLogTraceBuf[i32LogTraceSize + 0] = _T('\n');
		szLogTraceBuf[i32LogTraceSize + 1] = _T('\0');
	}
	else
	{
		szLogTraceBuf[i32Ret + 0] = _T('\n');
		szLogTraceBuf[i32Ret + 1] = _T('\0');
	}

#ifndef __NO_LOG_TRACE_API__
	OutputDebugString( szLogTraceBuf );
#endif

#ifdef __LOG_TRACE_STDOUT__
	_tprintf(szLogTraceBuf);
#endif
}