#pragma once

#include "Logger\I_Logger.h"
#include "Public\Utils.h"
#include "vld\vld.h"

class C_Logger: public I_Logger
{
private:
	long m_lRef;		// Reference count of Cmd
	typedef enum tagDateTimeFmt
	{
		eDTFULL = 0,	// Format:YYYY-MM-DD HH:MM:SS
		eDTFULL_MS,		// Format:YYYY-MM-DD HH:MM:SS.ddd
		eDTDATE,		// Format:YYYY-MM-DD
		eDTDAY,			// Format:DD
		eDTTIME,		// Format:HH:MM:SS
		eDTTIME_MS,		// Format:HH:MM:SS.ddd
		eDTFULL_,		// Format:YYYY-MM-DD HH_MM_SS
		eDTFULL_MS_,	// Format:YYYY-MM-DD HH_MM_SS_ddd
		eDTTIME_,		// Format:HH_MM_SS
		eDTTIME_MS_,	// Format:HH_MM_SS_ddd
	}E_DateTimeFmt, *PE_DateTimeFmt;

	typedef std::list<PS_LogMsg> ListLogMsg;
	FILE*		m_fileLog;
	HANDLE		m_hLogThread;
	HANDLE		m_hLogEvent;
	HANDLE		m_hExitEvent;
	DWORD		m_dwLogFlag;
	DWORD		m_dwWindowMsg;
	DWORD		m_dwPID;
	E_LogLevel	m_eLogLevel;
	TCHAR		m_szLogFile[MAX_PATH];
	ListLogMsg	m_lstLogMsg;
	CRITICAL_SECTION m_csMsgQuene;
	PFN_Log		m_pfnLog;
	void*		m_pvUserData;
private:
	int			_FormatData(
		const BYTE* pu8Data, 
		int			i32Size, 
		TCHAR*		pszMsgBuf,
		int			i32MsgBufSize
		);

	void		_LogV(
		E_LogLevel	eLogLevel, 
		const TCHAR*pszFormat, 
		va_list		argptr, 
		const BYTE*	pu8Data = NULL, 
		int			i32Size = 0
		);

	unsigned	_DoLog();

	TCHAR*		_GetLevelString(
		E_LogLevel	eLogLeve
		);

	TCHAR*		_GetLogHeadInfo(
		TCHAR*		pszMsgBuf, 
		int&		i32Size, 
		E_LogLevel	eLogLeve, 
		DWORD		dwTID, 
		SYSTEMTIME*	psTime
		);

	TCHAR*		_GetDateTimeString(
		TCHAR*			pszMsgBuf, 
		int&			i32Size, 
		SYSTEMTIME&		sTime,
		E_DateTimeFmt	eFormat,
		TCHAR*			pszStartWith = NULL, 
		TCHAR*			pszEndWith = NULL
		);

	bool		_OpenLogFile(
		BOOL			bNewFile, 
		DWORD			dwLogFlag
		);

	static unsigned __stdcall _LogThreadProc( void * );

public:
	C_Logger(void);
	~C_Logger(void);

public:
	virtual long AddRef(void); 
	virtual long Release(void);
	virtual	void OnFinalRelease();

	virtual bool Init(	// Initial log object
		const TCHAR*lpszLogFile = NULL,					// Log file, default is: {AppPath}/logs/{AppName}-YYYYMMDD.log
		E_LogLevel	eLogLevel   = LOG_LEVEL_DEFAULT,	// Log level
		DWORD		dwLogFlag   = LOG_FLAG_DEFAULT,	// Log Flag
		PFN_Log		pfnLog      = NULL,
		void*		pvUserData  = NULL
		);

	virtual bool Uninit();

	// Log property 
	virtual bool		 IsInited();		// Inited ?
	virtual int			 GetLogFlag();		// Log flag, reference LOG_FLAG_*
	virtual E_LogLevel	 GetLogLevel();		// Log level, reference E_LogLevel
	virtual const TCHAR* GetLogFile();		// Log file

public:
	//Log function: input variable parameter 
	virtual void Log  (E_LogLevel eLogLevel, const TCHAR* pszFormat, ...);
	virtual void Data (const BYTE* pu8Data, int i32Size, const TCHAR* pszFormat, ...);
	virtual void Debug(const TCHAR* pszFormat, ...);
	virtual void Trace(const TCHAR* pszFormat, ...);
	virtual void Info (const TCHAR* pszFormat, ...);
	virtual void Warn (const TCHAR* pszFormat, ...);
	virtual void Error(const TCHAR* pszFormat, ...);
	virtual void Fatal(const TCHAR* pszFormat, ...);	


};
