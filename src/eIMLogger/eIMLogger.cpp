#include "eIMLogger.h"
#include <assert.h> 
#include <string.h> 
#include <time.h> 
#include <process.h> 
#include <AtlConv.h>
#include <Lmcons.h>

#define IS_APPEND(FLAG)				(FLAG & EIMLOG_FLAG_APPEND)
#define CHECK_LOG_LEVEL_RETV_(L)	{if ( L < m_eLogLevel ) return;}

#define FMT_MSG_HEAD_SIZE		(80)							// Head size
#define FMT_DATA_MAX_SIZE		(8 * 1024)						// 8K
#define FMT_MSG_MAX_SIZE		(FMT_DATA_MAX_SIZE * 5 + 4096)	// ~44K

#pragma region Region_Log_Data
// Format Data Log help macro function
#define DISPLAY_CHAR(P,N)	((P[N] >= 32 && P[N] < 127) ? P[N] : _T('.'))

#define EC_LOG_DATA_FMT1	_T("%04d: %02X                                                 %c\n")                                                
#define DISPLAY_DATA1(P)	P[0x0]
#define DISPLAY_CHAR1(P)	DISPLAY_CHAR(P, 0x0)

#define EC_LOG_DATA_FMT2	_T("%04d: %02X %02X                                              %c%c\n")
#define DISPLAY_DATA2(P)	P[0x0], P[0x1]
#define DISPLAY_CHAR2(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1)

#define EC_LOG_DATA_FMT3	_T("%04d: %02X %02X %02X                                           %c%c%c\n")
#define DISPLAY_DATA3(P)	P[0x0], P[0x1], P[0x2]
#define DISPLAY_CHAR3(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2)

#define EC_LOG_DATA_FMT4	_T("%04d: %02X %02X %02X %02X                                        %c%c%c%c\n")
#define DISPLAY_DATA4(P)	P[0x0], P[0x1], P[0x2], P[0x3]
#define DISPLAY_CHAR4(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3)

#define EC_LOG_DATA_FMT5	_T("%04d: %02X %02X %02X %02X %02X                                     %c%c%c%c%c\n")
#define DISPLAY_DATA5(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4]
#define DISPLAY_CHAR5(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4)

#define EC_LOG_DATA_FMT6	_T("%04d: %02X %02X %02X %02X %02X %02X                                  %c%c%c%c%c%c\n")
#define DISPLAY_DATA6(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5]
#define DISPLAY_CHAR6(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5)

#define EC_LOG_DATA_FMT7	_T("%04d: %02X %02X %02X %02X %02X %02X %02X                               %c%c%c%c%c%c%c\n")
#define DISPLAY_DATA7(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6]
#define DISPLAY_CHAR7(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6)

#define EC_LOG_DATA_FMT8	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X                            %c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA8(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7]
#define DISPLAY_CHAR8(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7)

#define EC_LOG_DATA_FMT9	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X                       %c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA9(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8]
#define DISPLAY_CHAR9(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8)

#define EC_LOG_DATA_FMT10	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X                    %c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA10(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9]
#define DISPLAY_CHAR10(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9)

#define EC_LOG_DATA_FMT11   _T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X                 %c%c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA11(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9], P[0xA]
#define DISPLAY_CHAR11(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9), DISPLAY_CHAR(P, 0xA)

#define EC_LOG_DATA_FMT12	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X              %c%c%c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA12(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9], P[0xA], P[0xB]
#define DISPLAY_CHAR12(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9), DISPLAY_CHAR(P, 0xA), DISPLAY_CHAR(P, 0xB)

#define EC_LOG_DATA_FMT13	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X           %c%c%c%c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA13(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9], P[0xA], P[0xB], P[0xC]
#define DISPLAY_CHAR13(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9), DISPLAY_CHAR(P, 0xA), DISPLAY_CHAR(P, 0xB), \
							DISPLAY_CHAR(P, 0xC)

#define EC_LOG_DATA_FMT14	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X        %c%c%c%c%c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA14(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9], P[0xA], P[0xB], P[0xC], P[0xD]
#define DISPLAY_CHAR14(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9), DISPLAY_CHAR(P, 0xA), DISPLAY_CHAR(P, 0xB), \
							DISPLAY_CHAR(P, 0xC), DISPLAY_CHAR(P, 0xD)

#define EC_LOG_DATA_FMT15	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X     %c%c%c%c%c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA15(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9], P[0xA], P[0xB], P[0xC], P[0xD], P[0xE]
#define DISPLAY_CHAR15(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9), DISPLAY_CHAR(P, 0xA), DISPLAY_CHAR(P, 0xB), \
							DISPLAY_CHAR(P, 0xC), DISPLAY_CHAR(P, 0xD), DISPLAY_CHAR(P, 0xE)

#define EC_LOG_DATA_FMT16	_T("%04d: %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n")
#define DISPLAY_DATA16(P)	P[0x0], P[0x1], P[0x2], P[0x3], P[0x4], P[0x5], P[0x6], P[0x7], \
							P[0x8], P[0x9], P[0xA], P[0xB], P[0xC], P[0xD], P[0xE], P[0xF]
#define DISPLAY_CHAR16(P)	DISPLAY_CHAR(P, 0x0), DISPLAY_CHAR(P, 0x1), DISPLAY_CHAR(P, 0x2), DISPLAY_CHAR(P, 0x3), \
							DISPLAY_CHAR(P, 0x4), DISPLAY_CHAR(P, 0x5), DISPLAY_CHAR(P, 0x6), DISPLAY_CHAR(P, 0x7), \
							DISPLAY_CHAR(P, 0x8), DISPLAY_CHAR(P, 0x9), DISPLAY_CHAR(P, 0xA), DISPLAY_CHAR(P, 0xB), \
							DISPLAY_CHAR(P, 0xC), DISPLAY_CHAR(P, 0xD), DISPLAY_CHAR(P, 0xE), DISPLAY_CHAR(P, 0xF)

#pragma endregion  Region_Log_Data

#ifndef _NO_EIMCREATEINTERFACE_API_
// Define the EIMPLUGIN_API_ macro function to declare the standard API
extern "C" __declspec(dllexport) int __stdcall eIMCreateInterface(const TCHAR* pctszIID, void** ppvIObject)
{
	if( pctszIID == NULL || ppvIObject == NULL )
		return E_POINTER;

	if( _tcsnicmp(pctszIID, INAME_EIMLOGGER, _tcslen(INAME_EIMLOGGER)) == 0 )
	{
		*ppvIObject = new C_EIMLogger;
		if( *ppvIObject == NULL )
			return EIMERR_OUT_OF_MEMORY;

		return EIMERR_NO_ERROR;
	}

	return EIMERR_NOT_IMPL;
}
#endif

/////////////////////////////////////////////////////////////////////
IMPLEMENT_PLUGIN_REF_(C_EIMLogger, INAME_EIMLOGGER, m_ulRef);
C_EIMLogger::C_EIMLogger(void)
	: m_eLogLevel(EIMLOG_LEVEL_DEFAULT)
	, m_dwLogFlag(EIMLOG_FLAG_NOINIT)
	, m_dwPID(0)
	, m_dwWindowMsg(0)
	, m_fileLog(NULL)
	, m_hLogEvent(NULL)
	, m_hLogThread(NULL)
	, m_hExitEvent(NULL)
	, m_ulRef(1)
	, m_pfnLog(NULL)
	, m_pvUserData(NULL)
{
	m_szLogFile[0] = _T('\0');
	InitializeCriticalSectionAndSpinCount(&m_csMsgQuene, 99999);
}

C_EIMLogger::~C_EIMLogger(void)
{
	Uninit();
	DeleteCriticalSection(&m_csMsgQuene);
}

bool C_EIMLogger::_OpenLogFile(
	BOOL	bNewFile, 
	DWORD	dwLogFlag
	)
{
	float		fDummy    = 0.00f;
	int			i32Size   = MAX_PATH;
	TCHAR		szExt[20] = { _T(".log") };
	SYSTEMTIME	sTime	  = { 0 };
	
	GetLocalTime(&sTime);	
	if(bNewFile && m_szLogFile[0])
	{	// New a logfile, append a time string
		TCHAR*	pszFind = _tcsrchr(m_szLogFile, _T('.'));
		if(pszFind)
		{
			_tcsncpy(szExt, pszFind, 20 -1);
			pszFind[0] = _T('\0');
		}

		i32Size -= _tcslen(m_szLogFile);
		_GetDateTimeString(&m_szLogFile[MAX_PATH - i32Size], i32Size, sTime, eDTTIME_MS_, _T("_"), szExt);
	}
	
	if(m_szLogFile[0] == _T('\0'))	// Get a default log file 
		_GetDateTimeString(m_szLogFile, i32Size, sTime, eDTTIME_MS_, _T("ECLogger_"), szExt);

	m_fileLog = _tfopen(m_szLogFile, (IS_APPEND(dwLogFlag) && bNewFile == FALSE ) ? _T("at") : _T("wt"));
	if(m_fileLog)
	{	// Write a new log head line
		DWORD dwSize = UNLEN + 1;
		TCHAR szUser[UNLEN + 1] = { 0 };
		OSVERSIONINFOEX sOsVer = {sizeof( OSVERSIONINFOEX ), 0 };
		LANGID SysLID  = GetSystemDefaultLangID();
		LANGID UserLID = GetUserDefaultLangID();
		USES_CONVERSION;

		GetVersionEx((OSVERSIONINFO*)&sOsVer);
		GetUserName(szUser, &dwSize);
		fprintf(m_fileLog, "================ %04d-%02d-%02d %02d:%02d:%02d.%03d (%s, %u.%u.%u %s(%u.%u), Platform:%u, Product:%u, SuiteMask:0x%04X)SLID:0x%04x ULID:%04x================\n", 
			sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds,
			T2A(szUser), sOsVer.dwMajorVersion, sOsVer.dwMinorVersion, sOsVer.dwBuildNumber, T2A(sOsVer.szCSDVersion),
			sOsVer.wServicePackMajor, sOsVer.wServicePackMinor, sOsVer.dwPlatformId, sOsVer.wProductType, sOsVer.wSuiteMask,
			SysLID, UserLID);
		fflush(m_fileLog);
		return true;
	}

	return false;
}

//=============================================================================
//Function:     _FormatData
//Description:	Format eLLData, 16 byte data each line, and display the ASCII code
//
//Parameter:
//	pu8Data          - Data to format
//	i32Size          - Data size
//	pszMsgBuf        - Output the formated string
//	i32MsgBufSize    - Buffer size 
//
//Return:
//		The formated data size       
//=============================================================================
int C_EIMLogger::_FormatData(
	const BYTE*		pu8Data, 
	int				i32Size, 
	TCHAR*			pszMsgBuf, 
	int				i32MsgBufSize
	)
{
	int i32Ret		 = 0;
	int i32Line		 = 0;
	int i32LineCount = i32Size / 16;			// Display 16 data each line
	int i32StartPos  = 0;
	const BYTE* pu8LineStart = pu8Data; 
	for(; i32Ret >= 0 && i32StartPos < i32MsgBufSize && i32Line < i32LineCount; i32Line++, pu8LineStart += 16)
	{
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32MsgBufSize - i32StartPos, EC_LOG_DATA_FMT16, i32Line, DISPLAY_DATA16(pu8LineStart), DISPLAY_CHAR16(pu8LineStart));

		if(i32Ret >= 0)
			i32StartPos += i32Ret;
	}

	if(i32Ret < 0 || i32Line < i32LineCount)
	{
		pszMsgBuf[i32MsgBufSize - 2] = _T('\n');
		pszMsgBuf[i32MsgBufSize - 1] = _T('\0');
		return i32MsgBufSize - 1;
	}

	int i32Count =  i32MsgBufSize - i32StartPos;
	switch(i32Size % 16)
	{
	case 1:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT1,  i32Line, DISPLAY_DATA1(pu8LineStart),  DISPLAY_CHAR1(pu8LineStart));
		break;
	case 2:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT2,  i32Line, DISPLAY_DATA2(pu8LineStart),  DISPLAY_CHAR2(pu8LineStart));
		break;
	case 3:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT3,  i32Line, DISPLAY_DATA3(pu8LineStart),  DISPLAY_CHAR3(pu8LineStart));
		break;
	case 4:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT4,  i32Line, DISPLAY_DATA4(pu8LineStart),  DISPLAY_CHAR4(pu8LineStart));
		break;
	case 5:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT5,  i32Line, DISPLAY_DATA5(pu8LineStart),  DISPLAY_CHAR5(pu8LineStart));
		break;
	case 6:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT6,  i32Line, DISPLAY_DATA6(pu8LineStart),  DISPLAY_CHAR6(pu8LineStart));
		break;
	case 7:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT7,  i32Line, DISPLAY_DATA7(pu8LineStart),  DISPLAY_CHAR7(pu8LineStart));
		break;
	case 8:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT8,  i32Line, DISPLAY_DATA8(pu8LineStart),  DISPLAY_CHAR8(pu8LineStart));
		break;
	case 9:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT9,  i32Line, DISPLAY_DATA9(pu8LineStart),  DISPLAY_CHAR9(pu8LineStart));
		break;
	case 10:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT10, i32Line, DISPLAY_DATA10(pu8LineStart), DISPLAY_CHAR10(pu8LineStart));
		break;
	case 11:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT11, i32Line, DISPLAY_DATA11(pu8LineStart), DISPLAY_CHAR11(pu8LineStart));
		break;
	case 12:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT12, i32Line, DISPLAY_DATA12(pu8LineStart), DISPLAY_CHAR12(pu8LineStart));
		break;
	case 13:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT13, i32Line, DISPLAY_DATA13(pu8LineStart), DISPLAY_CHAR13(pu8LineStart));
		break;
	case 14:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT14, i32Line, DISPLAY_DATA14(pu8LineStart), DISPLAY_CHAR14(pu8LineStart));
		break;
	case 15:
		i32Ret = _sntprintf(&pszMsgBuf[i32StartPos], i32Count, EC_LOG_DATA_FMT15, i32Line, DISPLAY_DATA15(pu8LineStart), DISPLAY_CHAR15(pu8LineStart));
		break;
	default:
		i32Ret = 0;
		break;
	}

	if(i32Ret < 0)
	{
		pszMsgBuf[i32MsgBufSize - 2] = _T('\n');
		pszMsgBuf[i32MsgBufSize - 1] = _T('\0');
		return i32MsgBufSize - 1;
	}

	return (i32StartPos + i32Ret);
}

//=============================================================================
//Function:     _LogV
//Description:	Log variable parameter information
//
//Parameter:
//	eLogLevel    - Log level
//	pszFormat    - Format, referance printf 
//	argptr       - variable parameter
//
//Return:
//              None
//=============================================================================
void C_EIMLogger::_LogV(
	E_EIMLogLevel	eLogLevel,
	const TCHAR*	pszFormat,
	va_list			argptr, 
	const BYTE*		pu8Data, 
	int				i32Size
	)
{	
	if( eLogLevel < m_eLogLevel ||									// Skip this log message
		m_dwLogFlag == EIMLOG_FLAG_NOINIT ||						// Not initial or uninited
		!(m_dwLogFlag & EIMLOG_FLAG_WORKING))						// Log thread not working
		return;	

	S_EIMLogMsg	sLogMsg;											// Log message information, temporary

	sLogMsg.dwTID     = GetCurrentThreadId();						// Get current thread Id
	sLogMsg.dwPID	  = m_dwPID;									// Set Process Id
	sLogMsg.eLogLevel = eLogLevel;									// Set Log level
	GetLocalTime(&sLogMsg.sTime);									// Get current datetime

	int		i32Ret     = 0;
	int		i32MsgSize = FMT_MSG_MAX_SIZE;
	TCHAR	szMsgBuf[FMT_MSG_MAX_SIZE];
	_GetLogHeadInfo(szMsgBuf, i32MsgSize, eLogLevel, sLogMsg.dwTID, &sLogMsg.sTime);	// format head information

	i32Ret = _vsntprintf_s(&szMsgBuf[i32MsgSize], FMT_MSG_MAX_SIZE - i32MsgSize, _TRUNCATE, pszFormat, argptr);
	if(i32Ret < 0)
	{	// Truncated
		szMsgBuf[FMT_MSG_MAX_SIZE - 2] = _T('\n');
		szMsgBuf[FMT_MSG_MAX_SIZE - 1] = _T('\0');
		i32MsgSize = FMT_MSG_MAX_SIZE - 1;
	}
	else
	{
		i32MsgSize += i32Ret;
		if(i32MsgSize == FMT_MSG_MAX_SIZE)
			i32MsgSize = FMT_MSG_MAX_SIZE - 2; // Truncate

		szMsgBuf[i32MsgSize + 0] = _T('\n');
		szMsgBuf[i32MsgSize + 1] = _T('\0');
		i32MsgSize++;

		i32MsgSize += _FormatData(pu8Data, i32Size, &szMsgBuf[i32MsgSize], FMT_MSG_MAX_SIZE - i32MsgSize);
	}

	if(m_dwLogFlag & EIMLOG_FLAG_OUT_DEBUGVIEW)
 		OutputDebugString(szMsgBuf);

	// Convert Unicode to char
	USES_CONVERSION;
	char* pszMsg   = T2A(szMsgBuf);
	i32MsgSize	   = strlen(pszMsg);
	sLogMsg.dwSize = sizeof(S_EIMLogMsg) + i32MsgSize;				// Message structure size in byte
	PS_EIMLogMsg psEIMLogMsg  = (PS_EIMLogMsg)malloc(sLogMsg.dwSize);	// Malloc buffer
	if(psEIMLogMsg)
	{
		memcpy(psEIMLogMsg, &sLogMsg, sizeof(S_EIMLogMsg));			// Copy from temporary information
		memcpy(psEIMLogMsg->szMsg, pszMsg, i32MsgSize + 1);			// Copy message content
	
		if ( m_pfnLog )
			m_pfnLog(psEIMLogMsg, m_pvUserData);
		
		{
			C_EIMAutoLock Lock(m_csMsgQuene);
			m_lstLogMsg.push_back(psEIMLogMsg);						// Message enqueue
		}


		SetEvent(m_hLogEvent);										// New Log Message arived
	}
}

//=============================================================================
//Function:     _GetLevelString
//Description:	Get Log level string descript
//
//Parameter:
//	eLogLeve    - Log level
//
//Return:
//              
//=============================================================================
TCHAR* C_EIMLogger::_GetLevelString(E_EIMLogLevel eLogLeve)
{
	switch(eLogLeve)
	{
	case eLLDATA:		
		return _T("Data");
	case eLLDEBUG:		
		return _T("Debug");
	case eLLTRACE:		
		return _T("Trace");
	case eLLINFO:		
		return _T("Info");
	case eLLWARN:	
		return _T("Warn");
	case eLLERROR:	
		return _T("Error");
	case eLLFATAL:	
		return _T("Fatal");
	default:
		assert(0);	
		return _T(" -- ");
	}
}

TCHAR* C_EIMLogger::_GetDateTimeString(
	TCHAR*				pszMsgBuf, 
	int&				i32Size, 
	SYSTEMTIME&			sTime,
	E_EIMDateTimeFmt	eFormat,
	TCHAR*				pszStartWith,
	TCHAR*				pszEndWith)
{
	if(pszMsgBuf == NULL)
		return NULL;

#define _WITH_(P) ((P) ? (P) : _T(""))

	int i32Ret = 0;

	switch(eFormat)
	{
	default:
	case eDTFULL:
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%04d-%02d-%02d %02d:%02d:%02d%s"), _WITH_(pszStartWith), sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond, _WITH_(pszEndWith));
		break;
	case eDTFULL_MS: 
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%04d-%02d-%02d %02d:%02d:%02d.%03d%s"), _WITH_(pszStartWith), sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds, _WITH_(pszEndWith));
		break;
	case eDTDATE:	
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%04d-%02d-%02d%s"), _WITH_(pszStartWith), sTime.wYear, sTime.wMonth, sTime.wDay, _WITH_(pszEndWith));
		break;
	case eDTDAY:	
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%02d%s"), _WITH_(pszStartWith), sTime.wDay, _WITH_(pszEndWith));
		break;
	case eDTTIME:	
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%02d:%02d:%02d%s"), _WITH_(pszStartWith), sTime.wHour, sTime.wMinute, sTime.wSecond, _WITH_(pszEndWith));
		break;
	case eDTTIME_MS:
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%02d:%02d:%02d.%03d%s"), _WITH_(pszStartWith), sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds, _WITH_(pszEndWith));
		break;
	case eDTFULL_:
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%04d-%02d-%02d %02d_%02d_%02d%s"), _WITH_(pszStartWith), sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond, _WITH_(pszEndWith));
		break;
	case eDTFULL_MS_: 
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%04d-%02d-%02d %02d_%02d_%02d_%03d%s"), _WITH_(pszStartWith), sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds, _WITH_(pszEndWith));
		break;
	case eDTTIME_:
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%02d_%02d_%02d%s"), _WITH_(pszStartWith), sTime.wHour, sTime.wMinute, sTime.wSecond, _WITH_(pszEndWith));
		break;
	case eDTTIME_MS_:
		i32Ret = _sntprintf(pszMsgBuf, i32Size, _T("%s%02d_%02d_%02d_%03d%s"), _WITH_(pszStartWith), sTime.wHour, sTime.wMinute, sTime.wSecond, sTime.wMilliseconds, _WITH_(pszEndWith));
		break;
	}

	if(i32Ret < 0)
	{
		i32Size   = 0;
		pszMsgBuf = NULL;
	}
	else
		i32Size = i32Ret;

	return pszMsgBuf;
}


//=============================================================================
//Function:     _GetLogHeadInfo
//Description:	Get format log head information by user setting
//
//Parameter:
//	pszMsgBuf    - Output formated log head information
//	i32Size      - Message buffer size
//	eLogLeve     - Log level
//	dwTID        - Thread Id
//	psTime       - Date time
//
//Return:
//     formated log head information
//=============================================================================
TCHAR* C_EIMLogger::_GetLogHeadInfo(
	TCHAR*			pszMsgBuf, 
	int&			i32Size, 
	E_EIMLogLevel	eLogLeve,
	DWORD			dwTID, 
	SYSTEMTIME*		psTime
	)
{	// Full:    [P12345.T33222 2013-07-01 23:19:22.123][Debug]
	// Simple:  [T33222 23:19:22]
#define CHECK_RET(POS,RET) \
	{ \
		if(RET < 0) \
		{ \
			pszMsgBuf[0] = _T('\0'); \
			i32Size = 0; \
			return NULL; \
		} \
		POS += RET; \
		RET  = 0; \
	}

	if(pszMsgBuf == NULL || psTime == NULL)
		return NULL;

	int i32Ret = 0;
	int i32Pos = 0;
	if(m_dwLogFlag & EIMLOG_FLAG_HEAD_PID)
		i32Ret = _sntprintf(&pszMsgBuf[i32Pos], i32Size, _T("[P%06u.T%06u "), m_dwPID, dwTID);
	else
		i32Ret = _sntprintf(&pszMsgBuf[i32Pos], i32Size, _T("[T%06u "), dwTID);

	CHECK_RET(i32Pos, i32Ret);
	if(m_dwLogFlag & EIMLOG_FLAG_HEAD_DATE)
	{
		i32Ret =  i32Size - i32Pos;
		_GetDateTimeString(&pszMsgBuf[i32Pos], i32Ret, *psTime, eDTDATE, NULL, _T(" "));
	}
	else if (m_dwLogFlag & EIMLOG_FLAG_HEAD_DAY)
	{
		i32Ret =  i32Size - i32Pos;
		_GetDateTimeString(&pszMsgBuf[i32Pos], i32Ret, *psTime, eDTDAY, NULL, _T(" "));
	}

	CHECK_RET(i32Pos, i32Ret);
	i32Ret =  i32Size - i32Pos;
	_GetDateTimeString(&pszMsgBuf[i32Pos], i32Ret, *psTime, (m_dwLogFlag & EIMLOG_FLAG_HEAD_MS) ? eDTTIME_MS : eDTTIME, NULL, _T("]"));

	CHECK_RET(i32Pos, i32Ret);
	if(m_dwLogFlag & EIMLOG_FLAG_HEAD_LEVEL)
		i32Ret = _sntprintf(&pszMsgBuf[i32Pos], i32Size - i32Pos, _T("[%-5s]"),	_GetLevelString(eLogLeve));

	CHECK_RET(i32Pos, i32Ret);
	i32Size = i32Pos;

	return pszMsgBuf;
}

//=============================================================================
//Function:     _LogThreadProc
//Description:	Log thread function stub
//
//Parameter:
//	 pParam   - Parameter
//
//Return:
//              
//=============================================================================
unsigned __stdcall C_EIMLogger::_LogThreadProc(void* pParam)
{
	C_EIMLogger* pLogger = static_cast<C_EIMLogger*>(pParam);
	if(pLogger)
		return pLogger->_DoLog();

	return 1;
}

//=============================================================================
//Function:     _DoLog
//Description:	Log thread do work really at here
//
//
//Return:
//              
//=============================================================================
unsigned C_EIMLogger::_DoLog()
{
	__int64		i64LogFileSize = 0;
	HANDLE		arrhEvent[2] = { m_hExitEvent, m_hLogEvent };
	ListLogMsg	lstLogMsg;

	while(true)
	{
		DWORD dwRet = WaitForMultipleObjects(2, arrhEvent, FALSE, 200);
		switch(dwRet)
		{
		case WAIT_OBJECT_0 + 0:
			m_dwLogFlag &= ~EIMLOG_FLAG_WORKING;
			return 0;
		case WAIT_OBJECT_0 + 1:
			{
				C_EIMAutoLock Lock(m_csMsgQuene);
				m_lstLogMsg.swap(lstLogMsg);
			}

			if((m_dwLogFlag & EIMLOG_FLAG_OUT_FILE) && m_fileLog)
			{
				_fseeki64(m_fileLog, 0, SEEK_END);
				i64LogFileSize = _ftelli64(m_fileLog);
			}

			while(lstLogMsg.size() > 0)
			{
				PS_EIMLogMsg psECLogMsg = lstLogMsg.front();
				
				lstLogMsg.pop_front();
				assert(psECLogMsg);
				if(psECLogMsg == NULL)
					continue;

				int i32MsgSize = psECLogMsg->dwSize - sizeof(S_EIMLogMsg);
				if((m_dwLogFlag & EIMLOG_FLAG_OUT_FILE) && m_fileLog)
				{
					i64LogFileSize += i32MsgSize;
					if(i64LogFileSize > 0xFFFFFFFFll)
					{
						i64LogFileSize = 0;
						if(_OpenLogFile(TRUE, m_dwLogFlag))
							fwrite(psECLogMsg->szMsg, 1, i32MsgSize, m_fileLog);
						else
						{	// Can not log to file, redirect to DebugView
							m_dwLogFlag &= ~EIMLOG_FLAG_OUT_FILE;	// Clear ECLOG_FLAG_OUT_FILE
							m_dwLogFlag |= EIMLOG_FLAG_OUT_DEBUGVIEW;// Set ECLOG_FLAG_OUT_DEBUGVIEW
						}
					}
					else
						fwrite(psECLogMsg->szMsg, 1, i32MsgSize, m_fileLog);
				}


//Del 				//============ Begin === 2013/11/27 15:52:37 deleted by lwq ======
//Del 				//Reason: Move to _LogV for immediate display
//Del 				if(m_dwLogFlag & EIMLOG_FLAG_OUT_DEBUGVIEW)
//Del 					OutputDebugStringA(psECLogMsg->szMsg);
//Del 				//============ End ===== 2013/11/27 15:52:37 deleted by lwq ======

				if(m_dwLogFlag & EIMLOG_FLAG_OUT_HWND)
					SendMessage( HWND_BROADCAST, m_dwWindowMsg, 0, (LPARAM)psECLogMsg);

				if(m_dwLogFlag & EIMLOG_FLAG_OUT_STDOUT)
					printf("%s", psECLogMsg->szMsg);

				free(psECLogMsg);
			} // while(lstLogMsg.size() > 0)
			
			if((m_dwLogFlag & EIMLOG_FLAG_OUT_FILE) && m_fileLog)
				fflush(m_fileLog);

			break;	//case WAIT_OBJECT_0 + 1:
		}
	}
}

//=============================================================================
//Function:     Init
//Description:	Init ECLogger, must be call once and only once, unless called UnInit
//
//Parameter:
//	lpszLogFile    - Log file
//	eLogLevel      - Log Level
//	dwLogFlag      - Log Flag
//
//Return:
//      true  - succeed
//		false - failed
//=============================================================================
bool C_EIMLogger::Init(
	const TCHAR*	lpszLogFile, 
	E_EIMLogLevel	eLogLevel, 
	DWORD			dwLogFlag,
	PFN_eIMLog		pfnLog,
	void*			pvUserData
	)
{
	if(m_dwLogFlag)
	{	
		if ( _tcsicmp(lpszLogFile, m_szLogFile) )
		{	// Not the same log file
			Uninit();
		}
		else
		{
			OutputDebugString(_T("ECLogger already initialed\n"));
			return true;
		}
	}

	m_pfnLog	 = pfnLog;
	m_pvUserData = pvUserData;

	if(lpszLogFile)
		dwLogFlag |= EIMLOG_FLAG_OUT_FILE;

	if(dwLogFlag & EIMLOG_FLAG_OUT_FILE)
	{
		if( _sntprintf(m_szLogFile, MAX_PATH, lpszLogFile ? lpszLogFile : _T("\0")) < 0 ||
			_OpenLogFile(FALSE, dwLogFlag) == false)
			return false;
	}

	m_hLogEvent  = CreateEvent(NULL, FALSE, FALSE, NULL);	// Create Log event, auto reset
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	// Create exit thread event, manual reset
	m_hLogThread = (HANDLE)_beginthreadex(NULL, 0, _LogThreadProc, (LPVOID)this, 0, NULL);
	if(m_hLogEvent == NULL || m_hLogThread == NULL || m_hExitEvent == NULL)
	{
		Uninit();
		return false;
	}
	
	if(dwLogFlag & EIMLOG_FLAG_OUT_HWND)
		m_dwWindowMsg = RegisterWindowMessage(EIMLOG_WINDOW_MSG_NAME);

	m_dwPID		= GetCurrentProcessId();					// Get current process Id
	m_eLogLevel = eLogLevel;								// Set log level
	m_dwLogFlag = dwLogFlag | EIMLOG_FLAG_WORKING;			// Set log flag

	return true;
}

//=============================================================================
//Function:     UnInit
//Description:	Uninited the ECLogger, then you can Release() or Init() again
//
//
//Return:
//      true  - succeed
//		false - failed
//=============================================================================
bool C_EIMLogger::Uninit()
{
	m_eLogLevel = EIMLOG_LEVEL_DEFAULT;
	m_dwLogFlag = EIMLOG_FLAG_NOINIT;

	DWORD dwExitCode = 0;
	if(m_hLogThread && GetExitCodeThread(m_hLogThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
	{
		if(m_hExitEvent)
		{
			SetEvent(m_hExitEvent);
			WaitForSingleObject(m_hLogThread, INFINITE);		// Wait until thread exit
		}
		else
			TerminateThread(m_hLogThread, 1);
	}

	SAFE_CLOSE_FILE_(m_fileLog);
	SAFE_CLOSE_HANDLE_(m_hLogEvent);
	SAFE_CLOSE_HANDLE_(m_hExitEvent);
	SAFE_CLOSE_HANDLE_(m_hLogThread);

	return true;
}

// Log property 
bool C_EIMLogger::IsInited()
{
	return (m_dwLogFlag != EIMLOG_FLAG_NOINIT);
}

int C_EIMLogger::GetLogFlag()
{
	return m_dwLogFlag;
}

E_EIMLogLevel C_EIMLogger::GetLogLevel()
{
	return m_eLogLevel;
}

const TCHAR* C_EIMLogger::GetLogFile()
{
	return m_szLogFile;
}

//Log function: input variable parameter 
void C_EIMLogger::Log(E_EIMLogLevel eLogLevel, const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLogLevel);
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLogLevel, pszFormat, args);
	va_end(args);
}

void C_EIMLogger::Data(const BYTE* pu8Data, int i32Size, const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLLDATA);
	if( i32Size > FMT_DATA_MAX_SIZE)
		i32Size = FMT_DATA_MAX_SIZE;

	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLDATA, pszFormat, args, pu8Data, i32Size);
	va_end(args);
}

void C_EIMLogger::Debug(const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLLDEBUG);
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLDEBUG, pszFormat, args);
	va_end(args);
}

void C_EIMLogger::Trace(const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLLTRACE);
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLTRACE, pszFormat, args);
	va_end(args);
}

void C_EIMLogger::Info(const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLLINFO);
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLINFO, pszFormat, args);
	va_end(args);
}

void C_EIMLogger::Warn(const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLLWARN);
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLWARN, pszFormat, args);
	va_end(args);
}

void C_EIMLogger::Error(const TCHAR* pszFormat, ...)
{
	CHECK_LOG_LEVEL_RETV_(eLLERROR);
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLERROR, pszFormat, args);
	va_end(args);
}

void C_EIMLogger::Fatal(const TCHAR* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	_LogV(eLLFATAL, pszFormat, args);
	va_end(args);
}
