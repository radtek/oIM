// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: IEIMLogger is a Multi-Thread Logger, for safe, speed, and simple.
//   It can log as FILE, STDOUT, HWND and DebugView at the same time. have multi
//   head format to config(EIMLOG_FLAG_HEAD_*); can open as append or create log
//   file mode; can auto switch a new log file when EIMLOG_FLAG_NEW_4G is set, 
// 
// Lead message head(EIMLOG_FLAG_HEAD_*):
//	[PPPPPP.TTTTTT YYYY-MM-DD HH:MM:SS.ddd][LEVEL]
// Simple format:
//  [TTTTTT HH:MM:SS]
///////////////////////////////////////////////////////////////////////////////
// Usage:
// // 1. Create a logger
// IeIMLogger* g_pobjLogger = NULL;
// 
// if(eIMCreateInterface(INAME_EIMLOGGER, (void**)&g_pobjLogger)))
// {
//		// 2. Set logger parameter, can't modify later, unless UnInit() and Init()
//		g_pobjLogger->Init(_T("c:\\test.log"));								
//		
//		// 3. Use EIMLOGGER_* Macro to do log
//		// 3.1 Use EIMLOGGER_*0 Macro, not log function and line information
//		EIMLOGGER_LOG0(  g_pobjLogger, eLLTrace, _T("This is a test"));
//		EIMLOGGER_DATA0( g_pobjLogger, _T("This is a test:%s"), _T("Data"));
//		EIMLOGGER_DEBUG0(g_pobjLogger, _T("This is a test:%s"), _T("Debug"));
//		EIMLOGGER_TRACE0(g_pobjLogger, _T("This is a test:%s"), _T("Trace"));
//		EIMLOGGER_INFO0( g_pobjLogger, _T("This is a test:%s"), _T("Info"));
//		EIMLOGGER_WARN0( g_pobjLogger, _T("This is a test:%s"), _T("Warn"));
//		EIMLOGGER_ERROR0(g_pobjLogger, _T("This is a test:%s"), _T("Error"));
//		EIMLOGGER_FATAL0(g_pobjLogger, _T("This is a test:%s"), _T("Fatal"));
//
//		// 3.2 Use EIMLOGGER_* Macro, log function and line information
//		EIMLOGGER_LOG(  g_pobjLogger, eLLTrace, _T("This is a test"));
//		EIMLOGGER_DATA( g_pobjLogger, _T("This is a test:%s"), _T("Data"));
//		EIMLOGGER_DEBUG(g_pobjLogger, _T("This is a test:%s"), _T("Debug"));
//		EIMLOGGER_TRACE(g_pobjLogger, _T("This is a test:%s"), _T("Trace"));
//		EIMLOGGER_INFO( g_pobjLogger, _T("This is a test:%s"), _T("Info"));
//		EIMLOGGER_WARN( g_pobjLogger, _T("This is a test:%s"), _T("Warn"));
//		EIMLOGGER_ERROR(g_pobjLogger, _T("This is a test:%s"), _T("Error"));
//		EIMLOGGER_FATAL(g_pobjLogger, _T("This is a test:%s"), _T("Fatal"));
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

#ifndef __IEIMLOGGER_HEADER_2013_06_30_YFGZ__
#define __IEIMLOGGER_HEADER_2013_06_30_YFGZ__

#include "public\plugin.h"

#define INAME_EIMLOGGER			_T("SXIT.EIMLogger")			// eIMLogger interface name
#define INAME_RICHEDIT_LOG		_T("SXIT.EIMLogger.Richedit")	// Richedit.Log(Not realy exist, only define here for richedit's logger interface, because it create in mainui)
#define INAME_FILESTORAGE_LOG	_T("SXIT.EIMLogger.FileTrans")	// FileStorage.Log(Not realy exist, only define here for richedit's logger interface, because it create in mainui)

/////////////////////////////////////////////////////////////////
#define EIMLOG_WINDOW_MSG_NAME	_T("eIMLogger.Window.Msg.Yfgz")	// Log to Window global message

#define EIMLOG_FLAG_NOINIT			0x00000000		// Not initial
#define EIMLOG_FLAG_OUT_FILE		0x00000001		// Log to file
#define EIMLOG_FLAG_OUT_STDOUT		0x00000002		// Log to standard out, the console screen
#define EIMLOG_FLAG_OUT_HWND		0x00000004		// Log to a window
#define EIMLOG_FLAG_OUT_DEBUGVIEW	0x00000008		// Log to debugview, use OutputDebugString API
#define EIMLOG_FLAG_APPEND			0x00000010		// Open log file as append mode, otherwise empty it
#define EIMLOG_FLAG_NEW_4G			0x00000020		// Open a new log file when content greate than 2GB
#define EIMLOG_FLAG_HEAD_PID		0x00000040		// Log PID(Program ID)
#define EIMLOG_FLAG_HEAD_MS			0x00000100		// Log millisecond
#define EIMLOG_FLAG_HEAD_DATE		0x00000200		// Log date
#define EIMLOG_FLAG_HEAD_LEVEL		0x00000400		// Log level
#define EIMLOG_FLAG_WORKING			0x00000800		// Log thread is working
#define EIMLOG_FLAG_HEAD_DAY		0x00001000		// Log day 
#define EIMLOG_FLAG_DEFAULT			(EIMLOG_FLAG_OUT_FILE | EIMLOG_FLAG_HEAD_MS | EIMLOG_FLAG_OUT_DEBUGVIEW | EIMLOG_FLAG_APPEND)	// Default log type
#define EIMLOG_FLAG_DEFAULT_FULL	(EIMLOG_FLAG_DEFAULT | EIMLOG_FLAG_HEAD_DAY | EIMLOG_FLAG_HEAD_LEVEL)

#if defined( _DEBUG ) || defined( DEBUG )
# define EIMLOG_LEVEL_DEFAULT eLLDEBUG		
#else
# define EIMLOG_LEVEL_DEFAULT eLLINFO
#endif

#pragma pack( push, 8 )
typedef enum tagEIMLogLevel
{
	eLLDATA = 0,						// Data
	eLLDEBUG,							// Debug
	eLLTRACE,							// Trace
	eLLINFO,							// Information
	eLLWARN,							// Warning
	eLLERROR,							// Error
	eLLFATAL,							// Fatal
}E_EIMLogLevel, *PE_EIMLogLevel;

typedef struct tagEIMLogMsg
{
	DWORD			dwSize;				// This struct size
	E_EIMLogLevel	eLogLevel;			// Log level
	DWORD			dwTID;				// Thread Id
	DWORD			dwPID;				// Program Id
	SYSTEMTIME		sTime;				// Time
	char			szMsg[1];			// Message
}S_EIMLogMsg, *PS_EIMLogMsg;
typedef const struct tagEIMLogMsg* PS_EIMLogMsg_;
#pragma pack( pop )

typedef void (__stdcall* PFN_eIMLog)(PS_EIMLogMsg_ psMsg, void* pvUserData);

class __declspec(novtable) I_EIMLogger: public I_EIMUnknown
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
		const TCHAR*	lpszLogFile = NULL,					
		E_EIMLogLevel	eLogLevel   = EIMLOG_LEVEL_DEFAULT,	
		DWORD			dwLogFlag   = EIMLOG_FLAG_DEFAULT_FULL,	
		PFN_eIMLog		pfnLog      = NULL,					
		void*			pvUserData  = NULL
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
	virtual E_EIMLogLevel	GetLogLevel() = 0;	

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
	virtual void Log  (E_EIMLogLevel eLogLevel, const TCHAR* pszFormat, ...) = 0;

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
typedef I_EIMLogger*		PI_EIMLogger;
typedef const I_EIMLogger*	PI_EIMLogger_;


// Help Macro API, set NO_USE_EIMLOGGER to disable it***************************
// EIMLOGGER_*0: No __FUNCTION__, __LINE__
#ifdef  NO_USE_EIMLOGGER
// Turn off of the log function 
#  define EIMLOGGER_LOG0_(  PLOG, LEVEL, FORMAT, ...)
#  define EIMLOGGER_DATA0_( PLOG, DATA, SIZE, FORMAT, ...)
#  define EIMLOGGER_DEBUG0_(PLOG, FORMAT, ...)
#  define EIMLOGGER_TRACE0_(PLOG, FORMAT, ...)
#  define EIMLOGGER_INFO0_( PLOG, FORMAT, ...)
#  define EIMLOGGER_WARN0_( PLOG, FORMAT, ...)
#  define EIMLOGGER_ERROR0_(PLOG, FORMAT, ...)
#  define EIMLOGGER_FATAL0_(PLOG, FORMAT, ...)

#  define EIMLOGGER_LOG_(  PLOG, LEVEL, FORMAT, ...)
#  define EIMLOGGER_DATA_( PLOG, DATA, SIZE, FORMAT, ...)
#  define EIMLOGGER_DEBUG_(PLOG, FORMAT, ...)
#  define EIMLOGGER_TRACE_(PLOG, FORMAT, ...)
#  define EIMLOGGER_INFO_( PLOG, FORMAT, ...)
#  define EIMLOGGER_WARN_( PLOG, FORMAT, ...)
#  define EIMLOGGER_ERROR_(PLOG, FORMAT, ...)
#  define EIMLOGGER_FATAL_(PLOG, FORMAT, ...)
#else
// Log as simplest 
#  define EIMLOGGER_LOG0_(  PLOG, LEVEL, FORMAT, ...)	do{ if(PLOG) (PLOG)->Log  (LEVEL, FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_DATA0_( PLOG, DATA,SIZE,FORMAT,...)	do{ if(PLOG) (PLOG)->Data (DATA, SIZE, FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_DEBUG0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Debug(FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_TRACE0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Trace(FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_INFO0_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Info (FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_WARN0_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Warn (FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_ERROR0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Error(FORMAT, __VA_ARGS__);}while(0)
#  define EIMLOGGER_FATAL0_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Fatal(FORMAT, __VA_ARGS__);}while(0)

# if defined(UNICODE) || defined(_UNICODE)
#   define EIMLOGGER_HEAD_FUNC_FMT "(%S L%06d) "
# else
#   define EIMLOGGER_HEAD_FUNC_FMT "(%s L%06d) "
# endif

// Log as rich information 
#  define EIMLOGGER_LOG_(  PLOG, LEVEL, FORMAT, ...)	do{ if(PLOG) (PLOG)->Log  (LEVEL, _T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_DATA_( PLOG, DATA,SIZE,FORMAT, ...)	do{ if(PLOG) (PLOG)->Data (DATA, SIZE, _T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_DEBUG_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Debug(_T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_TRACE_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Trace(_T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_INFO_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Info (_T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_WARN_( PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Warn (_T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_ERROR_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Error(_T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#  define EIMLOGGER_FATAL_(PLOG, FORMAT, ...)			do{ if(PLOG) (PLOG)->Fatal(_T(EIMLOGGER_HEAD_FUNC_FMT)##FORMAT, __FUNCTION__, __LINE__, ##__VA_ARGS__ );}while(0)
#endif




#endif // __IEIMLOGGER_HEADER_2013_06_30_YFGZ__