// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_Logger is a Multi-Thread Logger, for safe, speed, and simple.
//   It can log as FILE, STDOUT, HWND and DebugView at the same time. have multi
//   head format to config(LOG_FLAG_HEAD_*); can open as append or create log
//   file mode; can auto switch a new log file when LOG_FLAG_NEW_4G is set, 
// 
// Lead message head(LOG_FLAG_HEAD_*):
//	[PPPPPP.TTTTTT YYYY-MM-DD HH:MM:SS.ddd][LEVEL]
// Simple format:
//  [TTTTTT HH:MM:SS]
///////////////////////////////////////////////////////////////////////////////
// Usage:
// // 1. Create a logger
// I_Logger* g_pobjLogger = NULL;
// 
// if(CreateInterface(INAME_LOGGER, (void**)&g_pobjLogger)))
// {
//		// 2. Set logger parameter, can't modify later, unless UnInit() and Init()
//		g_pobjLogger->Init(_T("c:\\test.log"));								
//		
//		// 3. Use LOGGER_* Macro to do log
//		// 3.1 Use LOGGER_*0 Macro, not log function and line information
//		LOGGER_LOG0(  g_pobjLogger, eLLTrace, _T("This is a test"));
//		LOGGER_DATA0( g_pobjLogger, _T("This is a test:%s"), _T("Data"));
//		LOGGER_DEBUG0(g_pobjLogger, _T("This is a test:%s"), _T("Debug"));
//		LOGGER_TRACE0(g_pobjLogger, _T("This is a test:%s"), _T("Trace"));
//		LOGGER_INFO0( g_pobjLogger, _T("This is a test:%s"), _T("Info"));
//		LOGGER_WARN0( g_pobjLogger, _T("This is a test:%s"), _T("Warn"));
//		LOGGER_ERROR0(g_pobjLogger, _T("This is a test:%s"), _T("Error"));
//		LOGGER_FATAL0(g_pobjLogger, _T("This is a test:%s"), _T("Fatal"));
//
//		// 3.2 Use EIMLOGGER_* Macro, log function and line information
//		LOGGER_LOG(  g_pobjLogger, eLLTrace, _T("This is a test"));
//		LOGGER_DATA( g_pobjLogger, _T("This is a test:%s"), _T("Data"));
//		LOGGER_DEBUG(g_pobjLogger, _T("This is a test:%s"), _T("Debug"));
//		LOGGER_TRACE(g_pobjLogger, _T("This is a test:%s"), _T("Trace"));
//		LOGGER_INFO( g_pobjLogger, _T("This is a test:%s"), _T("Info"));
//		LOGGER_WARN( g_pobjLogger, _T("This is a test:%s"), _T("Warn"));
//		LOGGER_ERROR(g_pobjLogger, _T("This is a test:%s"), _T("Error"));
//		LOGGER_FATAL(g_pobjLogger, _T("This is a test:%s"), _T("Fatal"));
//		
//		// 3.3 Call IEIMLogger function direct, DONT recomand£¬use up Macro you can ON/OFF function
//
//		// 4. Release Logger
//		g_pobjLogger->UnInit();	
//		g_pobjLogger->Release();
//		g_pobjLogger = NULL;		
//	}
// Auth: yfgz
// Date: 2013/7/4 9:27:21
// 
// History: 
//    2013/7/4 yfgz Finished first version
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __ILOGGER_HEADER_2017_06_30_YFGZ__
#define __ILOGGER_HEADER_2017_06_30_YFGZ__

#include <Windows.h>
#include <list>
#include "unknown\obj-ref-i.h"

#define INAME_LOGGER			_T("SXIT.Logger")			// Logger interface name

/////////////////////////////////////////////////////////////////
#define LOG_WINDOW_MSG_NAME	_T("Logger.Window.Msg.Yfgz")	// Log to Window global message

#define LOG_FLAG_NOINIT			0x00000000		// Not initial
#define LOG_FLAG_OUT_FILE		0x00000001		// Log to file
#define LOG_FLAG_OUT_STDOUT		0x00000002		// Log to standard out, the console screen
#define LOG_FLAG_OUT_HWND		0x00000004		// Log to a window
#define LOG_FLAG_OUT_DEBUGVIEW	0x00000008		// Log to debugview, use OutputDebugString API
#define LOG_FLAG_APPEND			0x00000010		// Open log file as append mode, otherwise empty it
#define LOG_FLAG_NEW_4G			0x00000020		// Open a new log file when content greate than 2GB
#define LOG_FLAG_HEAD_PID		0x00000040		// Log PID(Program ID)
#define LOG_FLAG_HEAD_MS		0x00000100		// Log millisecond
#define LOG_FLAG_HEAD_DATE		0x00000200		// Log date
#define LOG_FLAG_HEAD_LEVEL		0x00000400		// Log level
#define LOG_FLAG_WORKING		0x00000800		// Log thread is working
#define LOG_FLAG_HEAD_DAY		0x00001000		// Log day 
#define LOG_FLAG_DEFAULT		(LOG_FLAG_OUT_FILE | LOG_FLAG_HEAD_MS | LOG_FLAG_OUT_DEBUGVIEW | LOG_FLAG_APPEND)	// Default log type
#define LOG_FLAG_DEFAULT_FULL	(LOG_FLAG_DEFAULT | LOG_FLAG_HEAD_DAY | LOG_FLAG_HEAD_LEVEL)

#if defined( _DEBUG ) || defined( DEBUG )
# define LOG_LEVEL_DEFAULT eLLDEBUG		
#else
# define LOG_LEVEL_DEFAULT eLLINFO
#endif

#pragma pack( push, 8 )
typedef enum tagLogLevel
{
	eLLDATA = 0,						// Data
	eLLDEBUG,							// Debug
	eLLTRACE,							// Trace
	eLLINFO,							// Information
	eLLWARN,							// Warning
	eLLERROR,							// Error
	eLLFATAL,							// Fatal
}E_LogLevel, *PE_LogLevel;

typedef struct tagLogMsg
{
	DWORD			dwSize;				// This struct size
	E_LogLevel	eLogLevel;			// Log level
	DWORD			dwTID;				// Thread Id
	DWORD			dwPID;				// Program Id
	SYSTEMTIME		sTime;				// Time
	char			szMsg[1];			// Message
}S_LogMsg, *PS_LogMsg;
typedef const struct tagLogMsg* PS_LogMsg_;
#pragma pack( pop )

typedef void (__stdcall* PFN_Log)(PS_LogMsg_ psMsg, void* pvUserData);

class __declspec(novtable) I_Logger: public IObjRef
{
public:
	//=============================================================================
	//Function:     Init
	//Description:	Init ECLogger, must be call once and only once, unless called UnInit
	//
	//Parameter:
	//	lpszLogFile    - Log file, default is: {AppPath}/logs/{AppName}-YYYYMMDD.log
	//	eLogLevel      - Log Level
	//	dwLogFlag      - Log Flag
	//  pfnLog		   - Log Callback function
	//  pvUserData	   - Callback function user's data
	//
	//Return:
	//      true  - succeed
	//		false - failed
	//=============================================================================
	virtual bool Init(	// Initial log object
		const TCHAR*lpszLogFile = NULL,					
		E_LogLevel	eLogLevel   = LOG_LEVEL_DEFAULT,	
		DWORD		dwLogFlag   = LOG_FLAG_DEFAULT_FULL,	
		PFN_Log		pfnLog      = NULL,					
		void*		pvUserData  = NULL
		) = 0;

	//=============================================================================
	//Function:     UnInit
	//Description:	Uninited the ECLogger, then you can Release() or Init() again
	//
	//Return:
	//      true  - succeed
	//		false - failed
	//=============================================================================
	virtual bool Uninit()  = 0;

	// Log property 
	//=============================================================================
	//Function:     IsInited
	//Description:	Check initial status
	//
	//Return:
	//      true  - Initialed
	//		false - Not initial
	//=============================================================================
	virtual bool			IsInited()    = 0;	
	
	//=============================================================================
	//Function:     GetLogFlag
	//Description:	Get log flag
	//
	//Return:
	//      return LOG_FLAG_*
	//=============================================================================
	virtual int				GetLogFlag()  = 0;

	//=============================================================================
	//Function:     GetLogLevel
	//Description:	Get log level
	//
	//Return:
	//      return E_LogLevel
	//=============================================================================
	virtual E_LogLevel	GetLogLevel() = 0;	

	//=============================================================================
	//Function:     GetLogFile
	//Description:	Get log file path
	//
	//Return:
	//      return log file path
	//=============================================================================
	virtual const TCHAR*	GetLogFile()  = 0;	

public:
	//Log function: input variable parameter 

	//=============================================================================
	//Function:     Log
	//Description:	Write the log information like printf function
	//Parameter:
	//	eLogLevel   - Log level
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Log  (E_LogLevel eLogLevel, const TCHAR* pszFormat, ...) = 0;

	//=============================================================================
	//Function:     Data
	//Description:	Write the data level log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Data (const BYTE* pu8Data, int i32Size, const TCHAR* pszFormat, ...) = 0;	// i32Size < 8KB
	//=============================================================================
	//Function:     Log
	//Description:	Write the log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Debug(const TCHAR* pszFormat, ...) = 0;
	
	//=============================================================================
	//Function:     Log
	//Description:	Write the trace level log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Trace(const TCHAR* pszFormat, ...) = 0;

	//=============================================================================
	//Function:     Log
	//Description:	Write the info level log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Info (const TCHAR* pszFormat, ...) = 0;

	//=============================================================================
	//Function:     Log
	//Description:	Write the  warning level log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Warn (const TCHAR* pszFormat, ...) = 0;

	//=============================================================================
	//Function:     Log
	//Description:	Write the error level log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Error(const TCHAR* pszFormat, ...) = 0;
	
	//=============================================================================
	//Function:     Log
	//Description:	Write the fatal level log information like printf function
	//Parameter:
	//  pszFormat	- Format like printf
	//=============================================================================
	virtual void Fatal(const TCHAR* pszFormat, ...) = 0;	

};
typedef I_Logger*		PI_Logger;
typedef const I_Logger*	PI_Logger_;


// Help Macro API, set NO_USE_EIMLOGGER to disable it***************************
// EIMLOGGER_*0: No __FUNCTION__, __LINE__
#ifdef  NO_USE_LOGGER
// Turn off of the log function 
#  define LOGGER_LOG0_(  PLOG, LEVEL, FORMAT, ...)
#  define LOGGER_DATA0_( PLOG, DATA, SIZE, FORMAT, ...)
#  define LOGGER_DEBUG0_(PLOG, FORMAT, ...)
#  define LOGGER_TRACE0_(PLOG, FORMAT, ...)
#  define LOGGER_INFO0_( PLOG, FORMAT, ...)
#  define LOGGER_WARN0_( PLOG, FORMAT, ...)
#  define LOGGER_ERROR0_(PLOG, FORMAT, ...)
#  define LOGGER_FATAL0_(PLOG, FORMAT, ...)

#  define LOGGER_LOG_(  PLOG, LEVEL, FORMAT, ...)
#  define LOGGER_DATA_( PLOG, DATA, SIZE, FORMAT, ...)
#  define LOGGER_DEBUG_(PLOG, FORMAT, ...)
#  define LOGGER_TRACE_(PLOG, FORMAT, ...)
#  define LOGGER_INFO_( PLOG, FORMAT, ...)
#  define LOGGER_WARN_( PLOG, FORMAT, ...)
#  define LOGGER_ERROR_(PLOG, FORMAT, ...)
#  define LOGGER_FATAL_(PLOG, FORMAT, ...)
#else
// Log as simplest 
#  define LOGGER_LOG0_(  PLOG, LEVEL, FORMAT, ...)	do{ if(PLOG) (PLOG)->Log  (LEVEL, FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_DATA0_( PLOG, DATA,SIZE,FORMAT,...)	do{ if(PLOG) (PLOG)->Data (DATA, SIZE, FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_DEBUG0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Debug(FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_TRACE0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Trace(FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_INFO0_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Info (FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_WARN0_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Warn (FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_ERROR0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Error(FORMAT, __VA_ARGS__);}while(0)
#  define LOGGER_FATAL0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Fatal(FORMAT, __VA_ARGS__);}while(0)

# if defined(UNICODE) || defined(_UNICODE)
#   define LOGGER_HEAD_FUNC_FMT "(%S L%06d) "
# else
#   define LOGGER_HEAD_FUNC_FMT "(%s L%06d) "
# endif

// Log as rich information 
#  define LOGGER_LOG_(  PLOG, LEVEL, FORMAT, ...)	do{ if(PLOG) (PLOG)->Log  (LEVEL, _T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_DATA_( PLOG, DATA,SIZE,FORMAT, ...)	do{ if(PLOG) (PLOG)->Data (DATA, SIZE, _T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_DEBUG_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Debug(_T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_TRACE_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Trace(_T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_INFO_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Info (_T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_WARN_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Warn (_T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_ERROR_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Error(_T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define LOGGER_FATAL_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Fatal(_T(LOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#endif




#endif // __ILOGGER_HEADER_2017_06_30_YFGZ__