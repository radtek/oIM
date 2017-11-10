#include "stdafx.h"
#include "C_eIMUIWaitOverMgr.h"
#include "eIMCmd.h"
#include "C_HelperCmd.h"
#include "C_XmlProtocol.h"
//#include "eIMEngine\C_CmdReflection.h"
#include "eIMPinyin\IeIMPinyin.h"
#include "C_eIMSaveData.h"
#include "eIMUICore/eIMUISettingUI.h"
#include "ImgColorToGray.h"
#include "C_MeetingMgr.h"

#include <Wininet.h>
#pragma comment(lib, "Wininet.lib")  

using namespace Cmd;
using namespace HelperCmd;

#define USE_LIBCURL_DOWNLOAD_CONTACTSDB 1

#define EIME_LOG_FILE_NAME			_T("eIMEngine")				// eIMEngine log file default name

// Engine Sub-Module
#define EIME_SUBMOD_CLIENT			_T("Client.dll")			// ClientAgent Dll
#define EIME_SUBMOD_LOGGER			_T("eIMLogger.dll")			// Logger Dll
#define EIME_SUBMOD_P2P				_T("eIMP2P.dll")			// eIMP2P Dll
#define EIME_SUBMOD_SQLITE3			_T("SQLite3.dll")			// SQLite3 dll 
#define EIME_SUBMOD_UPDATER			_T("eIMUpdater.dll");		// Updater dll
#define EIME_SUBMOD_IFILE			_T("eIMInternetFile.dll")	// eIMInternetFile dll
//#define EIME_SUBMODE_ZIP			_T("eIMZip.dll")			// eIMZip.dll
// Create thread
#define CREATE_TRHEAD_FLAG_MGR		( 1 << eTRHEAD_FLAG_MGR_IDX  )			// Create Manager thread
#define CREATE_THREAD_FLAG_SEND		( 1 << eTHREAD_FLAG_SEND_IDX )			// Create Send thread
#define CREATE_THREAD_FLAG_RECV		( 1 << eTHREAD_FLAG_RECV_IDX )			// Create Recv thread
#define CREATE_THREAD_FLAG_DISP		( 1 << eTHREAD_FLAG_DISP_IDX )			// Create Dispatch thread
#define CREATE_THREAD_FLAG_DB		( 1 << eTHREAD_FLAG_DB_IDX )			// Create InDb thread
#define CREATE_THREAD_FLAG_SUB		( 1 << eTHREAD_FLAG_STATESUB_IDX )		// Create state subscribe thread
#define CREATE_THREAD_FLAG_CURL		( 1 << eTHREAD_FLAG_CURL_IDX )			// Create state curl thread
#define CREATE_THREAD_FLAG_UI_WAIT	( 1 << eTHREAD_FLAG_UI_WAIT_OVERTIME )	// Create ui wait over time thread
#define CREATE_THREAD_FLAG_DOWNLOAD_URLFILE    (1 << eTHREAD_FLAG_DOWNLOAD_URLFILE)


#define CREATE_THREAD_FLAG_NO_MGR	( CREATE_THREAD_FLAG_SEND | CREATE_THREAD_FLAG_RECV | CREATE_THREAD_FLAG_DISP | CREATE_THREAD_FLAG_DB)
#define CREATE_THREAD_FLAG_ALL		( CREATE_TRHEAD_FLAG_MGR | CREATE_THREAD_FLAG_NO_MGR | CREATE_THREAD_FLAG_UI_WAIT | CREATE_THREAD_FLAG_CURL)

#define BASE_RECONN_DELAY_TIME		5
#define MAX_RECONN_DELAY_TIME		80
static S_FileTrans s_sFileTrans;

#define TYPE_COMCONTACT				1		// 常用联系人
#define TYPE_COMDEPT				2		// 常用部门
#define TYPE_ATTEN_EMP				3		// 关注联系人
#define TYPE_DEFAULT_COMCONTACT		6		// 关注联系人

#define HTTP_HEADER_AGENT			"User-Agent: upload 0"
#define HTTP_HEADER_AGENT_UPLOADFILE "User-Agent: upload 2"
#define HTTP_HEADER_ENV				"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)"
#define HTTP_HEADER_CONTENTTYPE		"Content-Type: image/jpeg"
#define HTTP_HEADER_CONTENTTYPE_UPLOADFILE	"Content-Type: multipart/form-data"
#define HTTP_HEADER_ACCEPT			"Accept: */*"
#define HTTP_HEADER_CACHECONTROL	"Cache-Control: no-cache"
//#define HTTP_HEADER_ACCEPTENCODING	"Accept-Encoding: gzip, deflate"
#define HTTP_HEADER_EXPECT			"Expect: 100-continue"
//#define HTTP_HEADER_USE_CHUNKED		"Transfer-Encoding: chunked"
#define HTTP_HEADER_KEEP_ALIVE		"Connection: Keep-Alive"

#define TIMER_WAIT_TYPE_LOGIN					1	// 登录超时类型

#define TIMER_WAIT_TYPE_LOGIN_SECOND_COUNT		10	// 登录超时时间

#define MEETING_VEDIO_POWER      0x40

extern "C" __declspec(dllexport) int __stdcall eIMCreateInterface(const TCHAR* pctszIID, void** ppvIObject)
{
	CHECK_NULL_RET_(pctszIID, EIMERR_INVALID_POINTER);
	CHECK_NULL_RET_(ppvIObject, EIMERR_INVALID_POINTER);

	if( _tcsnicmp(pctszIID, INAME_EIMENGINE, _tcslen(INAME_EIMENGINE)) == 0 )
	{
		*ppvIObject = &C_eIMEngine::GetObject();

		return EIMERR_NO_ERROR;
	}

	return EIMERR_NOT_IMPL;
}

HRESULT GetEngineInterface(LPCTSTR lpctszName, void** ppvIObj)
{
	HRESULT hr =  Eng.QueryInterface(lpctszName, ppvIObj); 
	if (SUCCEEDED(hr)) 
		return EIMERR_NO_ERROR;

	TRACE_(_T("Query interface \'%s\' filed!"), lpctszName); 
	return hr; 
}

C_eIMEngine::C_eIMEngine(void)
	: m_bInited( FALSE )
	, m_bIsUserOffline( FALSE )
	, m_eEngineStatus( eENGINE_STATUS_NO_LOGIN )
	, m_pIEventMgr( NULL )
	, m_pILogger( NULL )
	, m_pIClientAgent( NULL )
	, m_pIP2P( NULL )
	, m_pIContacts( NULL )
	, m_pIMsgMgr( NULL )
	, m_pIHistory( NULL )
	, m_pIPinyin( NULL )
	, m_pIBIFile( NULL )
	, m_pSubscribeMgr ( NULL )
	, m_DllClientAgent( ePLUGIN_TYPE_NORMAL, INAME_CLIENT_AGENT )
	, m_DllLogger( ePLUGIN_TYPE_NORMAL, INAME_EIMLOGGER )
	, m_DllP2P( ePLUGIN_TYPE_NORMAL, INAME_EIMP2P )
	, m_DllSQLite3( ePLUGIN_TYPE_NORMAL, INAME_SQLITE_DB )
	, m_DllIFile(ePLUGIN_TYPE_NORMAL, INAME_EIMBATCH_INTER_FILE)
	, m_iCheckDelay(0)
	, m_iWaitType(0)
	, m_iWaitSecond(0)
	, m_wRegularGroupCount(0)
	, m_pUIWaitOverMgr(NULL)
	, m_iSynRoamDataStep(0)
	, m_bOnlineHasSynInfo(FALSE)
    , m_pUrlFileMgr(NULL)
	//, m_pMeetingLaunchString(NULL)
	, m_OldProgressValue(0)
	, m_i32ReconnTimes(0)
{
	ZERO_STRUCT_(m_sLoginAck);
	ZERO_STRUCT_(m_sLoginEmpInfo);
	ZERO_STRUCT_(m_sUpdateContsInfo);
	ZERO_STRUCT_(m_sMeetingAccountInfo);
	for( int i32Index = 0; i32Index < COUNT_OF_ARRAY_(m_aThreads); i32Index++)
	{	// Init the thread data
		m_aThreads[i32Index].pThis		= this;
		m_aThreads[i32Index].dwFlag		= 1 << i32Index;
		m_aThreads[i32Index].hThread	= NULL;
		m_aThreads[i32Index].bExit		= FALSE;
		m_aThreads[i32Index].hTaskEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
		InitializeCriticalSectionAndSpinCount( &m_aThreads[i32Index].csTask,  9999 );
	}

	m_pSaveData = new C_eIMSaveData();
	s_sFileTrans.bExit = FALSE;
	InitializeCriticalSectionAndSpinCount( &s_sFileTrans.csMapFiles,  9999 );

	// Set the thread function address
	m_aThreads[eTRHEAD_FLAG_MGR_IDX ].pThreadFun = &C_eIMEngine::_DoMgrThread;
	m_aThreads[eTHREAD_FLAG_SEND_IDX].pThreadFun = &C_eIMEngine::_DoSendThread;
	m_aThreads[eTHREAD_FLAG_RECV_IDX].pThreadFun = &C_eIMEngine::_DoRecvThread;
	m_aThreads[eTHREAD_FLAG_DISP_IDX].pThreadFun = &C_eIMEngine::_DoDispThread;
	m_aThreads[eTHREAD_FLAG_DB_IDX  ].pThreadFun = &C_eIMEngine::_DoDbThread;
	m_aThreads[eTHREAD_FLAG_STATESUB_IDX].pThreadFun = &C_eIMEngine::_DoStateSubscribeThread;
	m_aThreads[eTHREAD_FLAG_CURL_IDX].pThreadFun = &C_eIMEngine::_DoCUrlThread;
	m_aThreads[eTHREAD_FLAG_UI_WAIT_OVERTIME].pThreadFun = &C_eIMEngine::_DoUIWaitOverTimeThread;
    m_aThreads[eTHREAD_FLAG_DOWNLOAD_URLFILE].pThreadFun = &C_eIMEngine::_DoDownloadUrlFileThread;
}


C_eIMEngine::~C_eIMEngine(void)
{
	for( int i32Index = 0; i32Index < COUNT_OF_ARRAY_(m_aThreads); i32Index++)
	{
		DeleteCriticalSection( &m_aThreads[i32Index].csTask );
		SAFE_CLOSE_HANDLE_(m_aThreads[i32Index].hTaskEvent);
		for ( ListTaskIt it = m_aThreads[i32Index].listTask.begin(); it != m_aThreads[i32Index].listTask.end(); it++ )
		{
			SAFE_RELEASE_INTERFACE_(*it);
		}
	}
	SAFE_DELETE_PTR_(m_pSaveData);
	
	DeleteCriticalSection( &s_sFileTrans.csMapFiles );

	m_DllClientAgent.Unload();
	m_DllP2P.Unload();
	m_DllLogger.Unload();
	m_DllSQLite3.Unload();
	m_DllIFile.Unload();
}

C_eIMEngine& C_eIMEngine::GetObject()
{
	static C_eIMEngine obj;
	return obj;
}

HRESULT C_eIMEngine::QueryInterface(	// Create sub-interface
	const TCHAR* pctszIID, 		
	void** ppvIObject
	)
{
	CHECK_NULL_RET_(m_pIEventMgr, EIMERR_NO_INIT);	// Not InitPlugin, or already FreePlugin
	CHECK_NULL_RET_(pctszIID, EIMERR_INVALID_POINTER);
	CHECK_NULL_RET_(ppvIObject, EIMERR_INVALID_POINTER);
	HRESULT hr = EIMERR_NO_ERROR;
	* ppvIObject = NULL;

//	EIMLOGGER_DEBUG_(m_pILogger, _T("IID:%s"), pctszIID);
	if( _tcsnicmp(pctszIID, INAME_CLIENT_AGENT, COUNT_OF_ARRAY_(INAME_CLIENT_AGENT)) == 0 )
	{	// I_ClientAgent
		if ( m_pIClientAgent == NULL )
		{
			EIMLOGGER_ERROR_(m_pILogger, _T("ClientAgent not initied"));
			hr = EIMERR_NO_INIT_PLUGIN;
		}
		else
			*ppvIObject = m_pIClientAgent;
	}
	else if(  _tcsnicmp(pctszIID, INAME_EIMP2P, COUNT_OF_ARRAY_(INAME_EIMP2P) - 1) == 0 )
	{	// I_EIMP2P
		hr = m_DllP2P.eIMCreateInterface(pctszIID, ppvIObject);
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_CONTACTS, COUNT_OF_ARRAY_(INAME_EIMENGINE_CONTACTS)) == 0 )
	{	// I_EIMContacts
		*ppvIObject = &C_eIMContacts::GetObject();
	}
	//else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_RECENT, COUNT_OF_ARRAY_(INAME_EIMENGINE_RECENT)) == 0 )
	//{	// I_EIMRecent
	//}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_MSGMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_MSGMGR)) == 0 )
	{	// I_EIMMsgMgr
		C_eIMMsgMgr* pObj = new C_eIMMsgMgr;
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_SESSIONMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_SESSIONMGR)) == 0 )
	{	// I_EIMSessionMgr
		C_eIMSessionMgr* pObj = new C_eIMSessionMgr;
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if (_tcsnicmp(pctszIID,INAME_EIMENGINE_MEETING , COUNT_OF_ARRAY_(INAME_EIMENGINE_MEETING)) == 0)
	{
		// I_MeetingMgr
		C_MeetingMgr* pObj = new C_MeetingMgr;
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_LOGIN_USER_MGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_LOGIN_USER_MGR)) == 0 )
	{	// I_EIMLoginUserMgr
		C_eIMLoginUserMgr* pObj = new C_eIMLoginUserMgr;
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_FILEMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_FILEMGR)) == 0 )
	{	// I_EIMFileMgr
		C_eIMFileMgr* pObj = new C_eIMFileMgr;	
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_SMSMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_SMSMGR)) == 0 )
	{	// I_EIMFileMgr
		C_eIMSMSMgr* pObj = new C_eIMSMSMgr;	
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	//else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_HISTORY, COUNT_OF_ARRAY_(INAME_EIMENGINE_HISTORY)) == 0 )
	//{	// I_EIMHistory
	//}
	else if( _tcsnicmp(pctszIID, INAME_EIMUI_PINYIN, COUNT_OF_ARRAY_(INAME_EIMUI_PINYIN)) == 0 )
	{	// I_EIMPinyin m_pIPinyin
		if ( m_pIPinyin )
		{
			m_pIPinyin->AddRef();
			*ppvIObject = m_pIPinyin;
		}
		else
		{
			EIMLOGGER_ERROR_(m_pILogger, _T("SXIT.Pinyin not initied"));
		}
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_PROTOCOL, COUNT_OF_ARRAY_(INAME_EIMENGINE_PROTOCOL)) == 0 )
	{	// I_EIMProtocol
		*ppvIObject = &C_XmlProtocol::GetObject();
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_EMOTICON, COUNT_OF_ARRAY_(INAME_EIMENGINE_EMOTICON)) == 0 )
	{	// I_eIMXmlEmoticon
		*ppvIObject = &C_XmlEmoticon::GetObject();
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMLOGGER, COUNT_OF_ARRAY_(INAME_EIMLOGGER)) == 0 )
	{	// I_EIMLogger
		m_pILogger->AddRef();
		*ppvIObject = m_pILogger;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMEVENTMGR, COUNT_OF_ARRAY_(INAME_EIMEVENTMGR)) == 0 )
	{	// I_EIMEventMgr
		m_pIEventMgr->AddRef();
		*ppvIObject = m_pIEventMgr;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE, COUNT_OF_ARRAY_(INAME_EIMENGINE)) == 0 )
	{	// I_EIMEngine
		AddRef();
		*ppvIObject = this;
	}
	else if( _tcsnicmp(pctszIID, INAME_SQLITE_DB,    COUNT_OF_ARRAY_(INAME_SQLITE_DB)) == 0 ||
			 _tcsnicmp(pctszIID, INAME_SQLITE_TABLE, COUNT_OF_ARRAY_(INAME_SQLITE_TABLE)) == 0 ||
			 _tcsnicmp(pctszIID, INAME_SQLITE_STMT,  COUNT_OF_ARRAY_(INAME_SQLITE_STMT)) == 0 )
	{	// I_SQLiteDatabase, I_SQLiteTable, I_SQLiteStmt
		hr = m_DllSQLite3.eIMCreateInterface(pctszIID, ppvIObject);
	}	
	else if( _tcsnicmp(pctszIID, INAME_EIMSINGLE_INTER_FILE, COUNT_OF_ARRAY_(INAME_EIMSINGLE_INTER_FILE)) == 0)
	{	// I_eIMSingleInternetFile
		hr = m_DllIFile.eIMCreateInterface(pctszIID, ppvIObject);
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMBATCH_INTER_FILE, COUNT_OF_ARRAY_(INAME_EIMBATCH_INTER_FILE)) == 0)
	{	// I_eIMBatchInternetFile
		if ( m_pIBIFile == NULL)
			hr = EIMERR_PLUGIN_INIT_FAIL;
		else
		{
			m_pIBIFile->AddRef();
			*ppvIObject = m_pIBIFile;
		}
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_SUBSCRIBE, COUNT_OF_ARRAY_(INAME_EIMENGINE_SUBSCRIBE)) == 0)
	{	// I_eIMStateSubscribeMgr
		*ppvIObject = &C_eIMStateSubscribeMgr::GetObject();
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_ROAMDATA, COUNT_OF_ARRAY_(INAME_EIMENGINE_ROAMDATA)) == 0 )
	{	// I_EIMRoamDataMgr
		C_eIMRoamDataMgr* pObj = new C_eIMRoamDataMgr;
		if ( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_BROMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_BROMGR)) == 0)
	{
		C_eIMBroMgr *pObj = new C_eIMBroMgr;
		if( pObj == NULL )
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
	else if(_tcsnicmp(pctszIID, INAME_EIMENGINE_MSGREAD, COUNT_OF_ARRAY_(INAME_EIMENGINE_MSGREAD)) == 0)
	{
		C_eIMMsgReadMgr *pObj = new C_eIMMsgReadMgr;
		if(NULL == pObj)
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
    else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_MSGCOLLECTMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_MSGCOLLECTMGR)) == 0 )
    {
        C_eIMMsgCollectMgr* pObj = new C_eIMMsgCollectMgr;
        if ( pObj == NULL )
            hr = EIMERR_OUT_OF_MEMORY;
        else
            *ppvIObject = pObj;
    }
	else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_IMPORT_MSG, COUNT_OF_ARRAY_(INAME_EIMENGINE_IMPORT_MSG)) == 0 )
	{	
		C_eIMImportMsgMgr *pObj = new C_eIMImportMsgMgr;
		if(NULL == pObj)
			hr = EIMERR_OUT_OF_MEMORY;
		else
			*ppvIObject = pObj;
	}
    else if( _tcsnicmp(pctszIID, INAME_EIMENGINE_MEETINGMGR, COUNT_OF_ARRAY_(INAME_EIMENGINE_MEETINGMGR)) == 0 )
    {	
        C_MeetingMgr *pObj = new C_MeetingMgr;
        if(NULL == pObj)
            hr = EIMERR_OUT_OF_MEMORY;
        else
            *ppvIObject = pObj;
    }
	else
	{
		hr = EIMERR_NOT_IMPL;
		return hr;
	}

	if (FAILED(hr))
		EIMLOGGER_ERROR_(m_pILogger, _T("QueryInterface[%s] failed, hr: 0x%08X, desc: %s"), pctszIID, hr, hpi.GetErrorLangInfo(hr, _T("Unknown error")));

	return hr;
}

ULONG	C_eIMEngine::AddRef(void)
{
	return 1;
}

ULONG	C_eIMEngine::Release(void)
{
	return 1;
}

inline BOOL C_eIMEngine::_CreateThread( DWORD dwFlag )
{
	DWORD dwExitCode = 0; 
	for( int i32Index = 0; i32Index < COUNT_OF_ARRAY_(m_aThreads) && m_aThreads[i32Index].bExit == FALSE; i32Index++)
	{
		uintptr_t& h = m_aThreads[i32Index].hThread;
		if ((dwFlag & ( 1 << i32Index)) &&	// Check create thread's flag
			(h == NULL ||					// Not created
			GetExitCodeThread((HANDLE)(h), &dwExitCode) && dwExitCode != STILL_ACTIVE))	// Thread exited
		{
			SAFE_CLOSE_HANDLE_(h);

			if ( eTHREAD_FLAG_DB_IDX == i32Index && m_sLoginEmpInfo.Id == 0)
			{
				EIMLOGGER_ERROR_(m_pILogger, _T("Not logined, can not start the database thread. Engine Status:%d"), m_eEngineStatus);
				continue;
			}

			h = _beginthreadex( NULL, 0, _StartThread,  &m_aThreads[i32Index], 0, &m_aThreads[i32Index].u32Tid ); 
			if ( h == NULL ) 
			{ 
				ASSERT_( FALSE ); 
				EIMLOGGER_ERROR_(m_pILogger, _T("Create thread \'%d\' failed!"), i32Index); 
				return FALSE; 
			} 
		}
	}

	return TRUE;
}

inline BOOL C_eIMEngine::_StopThread( )
{
	int	i32Count = 0;
	int i32Index = 0;
	MSG msg = { 0 };

	for ( i32Index = 0, i32Count = 0; i32Index < IME_MAX_THREADS; i32Index++)
	{
		m_aThreads[i32Index].bExit = TRUE;	// Set exit thread flag
	}

	Sleep(200);
	do
	{
		for ( i32Index = 0, i32Count = 0; i32Index < IME_MAX_THREADS; i32Index++)
		{
			DWORD dwExitCode = 0;
			S_WorkThreadData& WTD = m_aThreads[i32Index];	
			if ( WTD.hThread &&									// Valid
				 GetExitCodeThread((HANDLE)WTD.hThread, &dwExitCode) &&	// Succeeded
				 dwExitCode == STILL_ACTIVE )					// Thread is active
			{
				i32Count++;
			}
		}

		if ( i32Count > 0 && m_pIEventMgr )
		{
			Sleep(5);
			SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_EVENT_LOOP, (void*)EVENT_LOOP_PEEKQUENE);
		}
	}while(i32Count > 0 );
	return TRUE;
}

unsigned __stdcall C_eIMEngine::_StartThread( void* pvData )
{
	C_eIMEngine*	  pEngine = NULL;
	PS_WorkThreadData pWTD = static_cast< PS_WorkThreadData >( pvData );
	ASSERT_( pWTD );
	if ( pWTD == NULL ||									// Invalid thread parameter
		 pWTD->pThreadFun == NULL ||						// Invalid thread function pointer
		 pWTD->hTaskEvent == NULL ||						// Invalid thread task event handle
		!(pEngine = static_cast<C_eIMEngine*>(pWTD->pThis)))// Invalid this pointer
	{
		if ( pWTD )
			TRACE_( _T("Invalid parameter, pWTD:%p, pThreadFun:%p, pTaskEvent:%p, pThis:%p, exit!"),
				pWTD, pWTD->pThreadFun, pWTD->hTaskEvent, pEngine );

		return E_INVALIDARG;
	}

	try
	{
		DWORD dwRet = ((*pEngine).*pWTD->pThreadFun)(pWTD);
		SAFE_CLOSE_HANDLE_(pWTD->hThread);
		return dwRet;
	}
	catch(...)
	{
		EIMLOGGER_ERROR_(pEngine->m_pILogger, _T("Thread of [%d] throw abnormal"), pWTD->dwFlag);
	}
}

unsigned C_eIMEngine::_DoMgrThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );
	EIMLOGGER_INFO_(m_pILogger,  _T(" Entry _DoMgrThread(...)") );
	DWORD dwCheckTime = 0;
	DWORD dwBackupDatabaseTime = 0;
	const int i32WaitTimeMax = 5 * 60 * 20;	// 5min: 5 * 60 * (20 * 50)
	while( pWTD->bExit == FALSE )
	{
		// Sleeping
		int i32WaitTimes = i32WaitTimeMax;
		while( pWTD->bExit == FALSE && i32WaitTimes-- > 0 )
			Sleep( 50 );

		if ( dwCheckTime == 0 || dwCheckTime >= 1 * 60 * 60 * 1000 )	// 1h * 60min * 60s * 1000ms
		{	// Each 1hour check a time
			DWORD dwTimeNow = GetTickCount();
			_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_CHECK_TIME_REQ, (void*)&dwTimeNow);
			dwCheckTime = 1;				// Reset
		}
		dwCheckTime += i32WaitTimeMax * 50;

		_AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_FLUSHOUT_DATABASE, 0);		// Flush-out of database
		dwBackupDatabaseTime += 5;	// 5Min
		if ( dwBackupDatabaseTime >= 30 )
		{
			dwBackupDatabaseTime = 0;
			_AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_BACKUP_DATABASE, 0);		// Backup of database
		}

		// Create the other thread if not exist
		_CreateThread( CREATE_THREAD_FLAG_NO_MGR );	
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoMgrThread(...)") );
	return 0;
}

unsigned  C_eIMEngine::_DoSendThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );
	int			i32Ret;
	ListTask	listTask;
	I_EIMCmd*	pICmd = NULL;

	GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	EIMLOGGER_INFO_(m_pILogger, _T("Entry _DoSendThread(...)"));
	while( pWTD->bExit == FALSE )
	{
		if ( WAIT_OBJECT_0 != WaitForSingleObject(pWTD->hTaskEvent, 200) )
			continue;
		else
		{	// Get all command in list
			C_EIMAutoLock AutoLock(pWTD->csTask);
			pWTD->listTask.swap(listTask);
		}

		for(ListTaskIt it = listTask.begin(); it != listTask.end(); it++)
		{
			pICmd = *it;
			if ( pICmd )
			{
			//	if ( pWTD->bExit == FALSE )
			//		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_SEND_CMD_BEFORE, pICmd);
			
				i32Ret = pICmd->DoCmd(pILog, pICa, &pWTD->bExit);
			//	if ( pWTD->bExit == FALSE )
			//		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_SEND_CMD_AFTER, pICmd);
				if (SUCCEEDED(i32Ret))
				{	
					pICmd->SetFlag(~CMD_FLAG_THREAD_SEND, CMD_FLAG_THREAD_SEND);	// Clear flag
					if ( pICmd->GetFlag(CMD_FLAG_THREAD_DB) )		// Enqueue of db
						_AddCmd(eTHREAD_FLAG_DB_IDX, pICmd, TRUE);
					else if ( pICmd->GetFlag(CMD_FLAG_THREAD_UI) )	// Enqueue of Dispach UI without db
						_AddCmd(eTHREAD_FLAG_DISP_IDX, pICmd, TRUE);
				}
				else
				{
					SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_SEND_MSG_FAILED, pICmd);
				}
				
				pICmd->Release();

			}
		}

		listTask.clear();
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoSendThread(...)") );
	return 0;
}

unsigned  C_eIMEngine::_DoRecvThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );

	int			i32Ret;
	BOOL		bUpdatedEmps = FALSE;	// for delay start _UpdateEmps
	DWORD		dwStartTime = GetTickCount();
	const DWORD dwDelayTime = GetAttributeInt(IME_ATTRIB_DOWNLOAD_EMPS_DELAY, 5) * 60 * 1000;	// 10min
	MESSAGE		msg;
	I_EIMCmd*	pICmd = NULL;
		
	GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	std::vector<I_EIMCmd*> vectReadMsgSyncNotice;	//	新消息已读通知，需要等到接收完离线消息后，才能处理。
	S_LoginStatus sLoginStatus = { 0 };
	DWORD dwCheckTime = GetTickCount();
	m_iCheckDelay = 0;				// 起始延时5秒
	BOOL bReConn  = FALSE;
	int AttriReconTimes = GetAttributeInt(IME_ATTRIB_RECONNECT_TIMES, IME_RECONNECT_TIMES_DEFAULT); //读取断线重连次数
	EIMLOGGER_INFO_(m_pILogger, _T("Entry _DoRecvThread(...)"));
	while( pWTD->bExit == FALSE )
	{
		i32Ret = m_pIClientAgent->GetMessage(&msg);
		bReConn  = FALSE;
		if (FAILED(i32Ret))
		{
			switch(i32Ret)
			{
			case EIMERR_INVALID_PARAMTER:
				{
				}
				break;
			case EIMERR_NOT_CONN:
				{
					bReConn = TRUE;
					if ((m_eEngineStatus == eENGINE_STATUS_ONLINE
						|| m_eEngineStatus == eENGINE_STATUS_LEAVE))
					{
						sLoginStatus.dwStatus = EIMERR_STATUS_OFFLINE;
						SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
					}
				}
				break;
			case EIMERR_KICK:
				{
					if (m_eEngineStatus != eENGINE_STATUS_LOGINING && m_eEngineStatus != eENGINE_STATUS_KICK)
					{
						sLoginStatus.dwStatus = EIMERR_KICK;
						SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
						bReConn = FALSE;
					}
				}
				break;
			case EIMERR_FORBIDDEN:
				{
					if (m_eEngineStatus != eENGINE_STATUS_LOGINING && m_eEngineStatus != eENGINE_STATUS_FORBIDDEN)
					{
						sLoginStatus.dwStatus = EIMERR_FORBIDDEN;
						SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
					}
				}
				break;
			}

			bUpdatedEmps = FALSE;
			dwStartTime  = GetTickCount();

			if ((AttriReconTimes > 0) && (m_i32ReconnTimes > AttriReconTimes) )
				bReConn = FALSE;

			// to do..要根据新机制重连接
			if (bReConn && m_eEngineStatus >= 0 && m_bIsUserOffline == FALSE)		// Already do Logined and disconnected, auto login again
			{
				m_i32ReconnTimes++;
				//DWORD dwFlag;
				//BOOL ret = InternetGetConnectedState(&dwFlag, 0);  
				//if (!ret)
				//{//网络未连接
				//	Sleep(500);
				//	m_iCheckDelay = 0;
				//	continue;
				//}
				
				// 增量延时重连机制 5s 10s 20s 40s 80s 80s……
				if(GetTickCount() - dwCheckTime  >= (DWORD)m_iCheckDelay *1000)
				{
					EIMLOGGER_INFO_(m_pILogger, _T("reconnect") );
					dwCheckTime = GetTickCount();
#if 0				// 暂时10S连接一次。。
					if(m_iCheckDelay == 0)
					{
						m_iCheckDelay = BASE_RECONN_DELAY_TIME;
					}else
					if(m_iCheckDelay < MAX_RECONN_DELAY_TIME)
					{
						m_iCheckDelay = m_iCheckDelay*2;
					}
#else
					if(m_iCheckDelay == 0)
					{
						m_iCheckDelay = BASE_RECONN_DELAY_TIME*2;
					}
#endif
					bReConn = FALSE;
					_Login(TRUE);	// Reconnect
				}
			}

			Sleep(100);
			continue;
		}

		if ( bUpdatedEmps == FALSE )
		{
			if ( GetTickCount() - dwStartTime >= dwDelayTime &&
				(m_eEngineStatus == eENGINE_STATUS_ONLINE || m_eEngineStatus == eENGINE_STATUS_LEAVE) )	// 5 Min
			{
				_UpdateEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime);
				bUpdatedEmps = TRUE;
			}
		}

		switch( i32Ret )
		{
		case EIMERR_EMPTY_MSG:		// no response
			//	EIMLOGGER_DEBUG_(m_pILogger, _T("No response") );
			if ( vectReadMsgSyncNotice.size() &&  GetAttributeInt(IME_ATTRIB_OFFMSG_FINISHED) == TRUE )
			{	// 取出一个来处理
				std::vector<I_EIMCmd*>::iterator it = vectReadMsgSyncNotice.begin();
				pICmd = *it;
				vectReadMsgSyncNotice.erase(it);
			}		
			else
			{
				Sleep(200);
				continue;
			}
		case EIMERR_NO_ERROR:		// get a response; 
			if ( pICmd == NULL )
				pICmd = CmdCreateInstance<I_EIMCmd*>( msg.wCmdID, msg.aszData);

			if ( pICmd )
			{
				if ( msg.wCmdID == eCMD_READ_MSG_SYNC_NOTICE && GetAttributeInt(IME_ATTRIB_OFFMSG_FINISHED) == FALSE )
				{	// 没有接收完离线消息，就需要先缓存新消息已读通知
					vectReadMsgSyncNotice.push_back(pICmd);
					pICmd = NULL;
					continue;
				}

			//	if ( pWTD->bExit == FALSE )
			//		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_RECV_CMD_BEFORE, pICmd);
				
				i32Ret = pICmd->DoCmd(pILog, pICa, &pWTD->bExit);
			//	if ( pWTD->bExit == FALSE )
			//		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_RECV_CMD_AFTER, pICmd);

				if (SUCCEEDED(i32Ret))
				{
					if ( pICmd->GetFlag(CMD_FLAG_THREAD_DB) )		// Enqueue of db
						_AddCmd(eTHREAD_FLAG_DB_IDX, pICmd, TRUE);
					else if ( pICmd->GetFlag(CMD_FLAG_THREAD_UI) )	// Enqueue of Dispatch UI without db
						_AddCmd(eTHREAD_FLAG_DISP_IDX, pICmd, TRUE);
				}
                else
                {
                    EIMLOGGER_ERROR_(m_pILogger, _T("cmd Deal fail CmdId:%d"), pICmd->GetCmdId());
                }

				pICmd->Release();
				pICmd = NULL;
			}
			else
			{
				 EIMLOGGER_WARN_(m_pILogger, _T("***** Cmd(%u) NOT be implemented!"), msg.wCmdID);
			}
			
			break;
		default:
			EIMLOGGER_WARN_(m_pILogger, _T("Error: 0x%08X,Info: %s"), i32Ret, hpi.GetErrorLangInfo(i32Ret, _T("")));
			// ASSERT_(FALSE);
			break;
		}
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoRecvThread(...)") );
	return 0;
}

void  C_eIMEngine::OnFileTransStatNotify(PS_FileTransNotify psFTN)
{
	CHECK_NULL_RETV_(psFTN);
	CHECK_NULL_RETV_(psFTN->psFileTrans);
	CHECK_NULL_RETV_(psFTN->psStat);

	MapFilesIt it = psFTN->psFileTrans->mapFiles.find(psFTN->psStat->Qfid);
	S_FileTransInfo sFileDummy = { 0 };
	BOOL bErase = FALSE;
	BOOL bExist = (it != psFTN->psFileTrans->mapFiles.end());
	if (bExist == FALSE )
	{
		sFileDummy.dwTimestamp = psFTN->psStat->dwTimestamp;
		sFileDummy.u64Fid	   = psFTN->psStat->Qfid;
		sFileDummy.i64FileSize = psFTN->psStat->i64TotalSize;
		sFileDummy.i64TransSize= psFTN->psStat->i64CompleteSize;
		_tcscpy(sFileDummy.szKey, psFTN->psStat->tszUploadKey);
		EIMLOGGER_WARN_(psFTN->psFileTrans->pILog, _T("Can not find file qfid:%llu, use dummy information to notify"), psFTN->psStat->Qfid);
		//ASSERT_(FALSE);
		return;
	}

	int i32Ratio = 0;
	S_FileTransInfo& sFile = (bExist ? it->second.sFile : sFileDummy);
	sFile.i64FileSize = psFTN->psStat->i64TotalSize;
	sFile.i64TransSize= psFTN->psStat->i64CompleteSize;
	sFile.dwTimestamp = psFTN->psStat->dwTimestamp;
	switch (psFTN->psStat->eTransStat)
	{
	case eInterFile_TaskWait:			//任务等待
		sFile.bitStatus = FILE_STATUS_READY;
		break;
	case eInterFile_TaskBegin:			//任务开始
		sFile.bitStatus = FILE_STATUS_START;
		break;
	case eInterFile_TaskTransing:		//任务传输中
		if ( sFile.i64FileSize > 0 && bExist )
		{
			i32Ratio = (int)((sFile.i64TransSize*100)/sFile.i64FileSize);
			if ( it->second.i32Ratio != i32Ratio)
			{
				sFile.bitStatus = FILE_STATUS_TRANSFERING;
				it->second.i32Ratio = i32Ratio;
				break;
			}
		}
		return;	//	Skip it
	case eInterFile_TaskEnd:			//任务完成
		bErase = TRUE;
		sFile.bitStatus = FILE_STATUS_FINISHED;
		if ( psFTN->psStat->tszUploadKey[0] && bExist)	// Upload
		{
			memcpy(sFile.szKey, psFTN->psStat->tszUploadKey, sizeof(sFile.szKey));
			it->second.pICmd->SetAttribute(psFTN->psStat->Qfid, psFTN->psStat->tszUploadKey);
		}

        if(sFile.bitDirect == FILE_DIR_UPLOAD && psFTN->psStat->tszUploadKey[0] == NULL)
        {
            sFile.bitStatus = FILE_STATUS_FAILED;
        }
		break;
	case eInterFile_TaskDeling:			//正在删除任务
	case eInterFile_TaskDel:			//任务被删除
		if ( sFile.bitDirect == FILE_DIR_DOWNLOAD )	// Upload DONT erase it for retry
			bErase = TRUE;

		sFile.bitStatus = FILE_STATUS_CANCEL;
		break;
	case eInterFile_NonTask:			//没有找到指定文件
		bErase = TRUE;
		sFile.bitStatus = FILE_STATUS_NO_FILE;
		break;
	case eInterFile_Failed:			//任务失败
		if ( sFile.bitDirect == FILE_DIR_DOWNLOAD )	// Upload DONT erase it for retry
			bErase = TRUE;
		sFile.bitStatus = FILE_STATUS_FAILED;
		break;
	case eInterFile_Verify_Failed:	// 文件验证失败
		bErase = TRUE;
		sFile.bitStatus = FILE_STATUS_VERIFY_FAILED;
		break;
	default:
		ASSERT_(FALSE);
		EIMLOGGER_WARN_(psFTN->psFileTrans->pILog, _T("Not dealwith of status: %u"), psFTN->psStat->eTransStat);
		break;
	}

	EIMLOGGER_DEBUG_(psFTN->psFileTrans->pILog, _T("ThreadID:%u, Timestamp:%u, Status:%u, qfid:%llu, Direct:%s, FileType:%s, ServerType:%s, Ratio:%u(%llu/%llu), File:%s, Key:%ls"),
		psFTN->dwThreadId, psFTN->psStat->dwTimestamp, sFile.bitStatus, sFile.u64Fid, 
		sFile.bitDirect == FILE_DIR_DOWNLOAD ? _T("Download") : _T("Upload"),
		sFile.bitType == FILE_TRANS_TYPE_OFFLINE ? _T("Offline") : _T("P2P"), 
		sFile.bitIsImage == FILE_SERVER_TYPE_FILE ? _T("File") : (sFile.bitIsImage == FILE_SERVER_TYPE_IMAGE ? _T("Image") : _T("HeadImage")),
		i32Ratio, sFile.i64TransSize, sFile.i64FileSize, sFile.szFile,  psFTN->psStat->tszUploadKey);

	SAFE_SEND_EVENT_(psFTN->psFileTrans->pIEMgr, EVENT_FILE_TRANSFER, &sFile);
	
	I_EIMCmd* pICmd = it->second.pICmd;
	if (pICmd && pICmd->GetCmdId() == eCMD_FILE_ACTION && pICmd->GetFlag(CMD_FLAG_THREAD_DB) )
	{
		if ( PS_FileTransInfo psData = (PS_FileTransInfo)pICmd->GetData() )
		{
			if ( sFile.bitDirect == FILE_DIR_DOWNLOAD &&
				 sFile.bitIsImage == FILE_SERVER_TYPE_FILE && // File
				 sFile.bitStatus == FILE_STATUS_FINISHED ) 
			{
				psData->dwTimestamp   = sFile.dwTimestamp;
				psData->bitStatus	  = sFile.bitStatus;
				psData->bitSaveStatus = 1;	// 保存下载完成状态：FILE_STATUS_FINISHED
			}
		}

		_AddCmd(eTHREAD_FLAG_DB_IDX, pICmd, TRUE);
	}

	if ( sFile.bitStatus == FILE_STATUS_FAILED && sFile.bitDirect == FILE_DIR_UPLOAD )
	{	// 上传图片、文件出错时，构造一条本地提示错误消息，并入库
		S_IFErrorInfo sIFEInfo;
		time_t tmNow = GetTimeNow();
		tm* ptmNow = localtime(&tmNow);
		const TCHAR* pszErrInfo = NULL;
		if ( sFile.bitIsImage == FILE_SERVER_TYPE_FILE )
			pszErrInfo = hpi.GetErrorLangInfo(EIM_ERR_UPLOAD_FILE_FAILED, _T("Upload file failed:"));
		else
			pszErrInfo = hpi.GetErrorLangInfo(EIM_ERR_UPLOAD_IMAGE_FAILED, _T("Upload image failed:"));

		_tcsncpy(sIFEInfo.szCmd, pICmd->GetXml(), COUNT_OF_ARRAY_(sIFEInfo.szCmd));
		_sntprintf(sIFEInfo.szErrorInfo, COUNT_OF_ARRAY_(sIFEInfo.szErrorInfo), _T("%02d:%02d:%02d %s%s"), 
			ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec, pszErrInfo, PathFindFileName(sFile.szFile));
		if (I_EIMCmd* pCmd = CmdCreateInstance<I_EIMCmd*>( eCMD_SEND_IMAGE_FILE_ERROR, &sIFEInfo))
			AddCmd(eTHREAD_FLAG_DISP_IDX, pCmd, FALSE);
	}

	if ( bErase && bExist )
	{
		SAFE_RELEASE_INTERFACE_( pICmd );
		psFTN->psFileTrans->mapFiles.erase(it);		// Remove it
	}
}

bool  __stdcall C_eIMEngine::_FileTransStatNotifyCb(PS_EIMStatNotify psStat, void *pvUserData)
{
	CHECK_NULL_RET_(psStat, false);
	if ( psStat->i64TotalSize > 0 )
		TRACE_(_T("Qfid:%llu, State:%d, %llu / %llu = %.2f%%, Key:%s"), psStat->Qfid, psStat->eTransStat, psStat->i64CompleteSize, psStat->i64TotalSize, (psStat->i64CompleteSize*100.0)/psStat->i64TotalSize, psStat->tszUploadKey);
	
	PS_FileTrans psFT = static_cast<PS_FileTrans>(pvUserData);
	ASSERT_(psFT);
	CHECK_NULL_RET_(psFT, false);
	if (psFT->bExit == TRUE || psFT->mapFiles.size() == 0)
	{
		TRACE_(_T("Application exiting, ignore file transfer status notify."));
		return false;
	}

	S_FileTransNotify sFTN;
	sFTN.dwThreadId  = ::GetCurrentThreadId();
	sFTN.psFileTrans = psFT;
	sFTN.psStat      = psStat;
	SAFE_SEND_EVENT_(psFT->pIEMgr, EVENT_ENGINE_FILE_TRANS, (void*)&sFTN);
	return true;
}

BOOL C_eIMEngine::_DoFileAction(I_EIMCmd* pICmd, I_EIMLogger* pILog)
{	// Special cmd
	if (pICmd->GetCmdId() != eCMD_FILE_ACTION)
		return FALSE;

	PS_FileTransInfo psFTI = (PS_FileTransInfo)pICmd->GetData();
	ASSERT_(psFTI);
	if ( psFTI == NULL )
		return TRUE;

	switch(psFTI->bitAction)
	{
	case FILE_ACTION_TO_OFFLINE:	// Convert to offline file(Only by sender)
		EIMLOGGER_DEBUG_(pILog, _T("Filetransfer action: ToOffline(%d), qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
		{
			C_EIMAutoLock al(s_sFileTrans.csMapFiles);
			MapFilesIt itF = s_sFileTrans.mapFiles.find(psFTI->u64Fid);
			BOOL bAdd2Map  = (itF == s_sFileTrans.mapFiles.end());
			if ( bAdd2Map == FALSE && itF->second.sFile.bitStatus <= FILE_STATUS_TRANSFERING )
			{
				EIMLOGGER_WARN_(pILog, _T("Filetransfer exist and working: ToOffline(%d), qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
				return TRUE;	// Is working...
			}

			S_FileInfo	sFileInfo = { 0 };
			sFileInfo.i32Ratio    = -1;
			if ( bAdd2Map == FALSE )
			{	// Already exist in quene
				sFileInfo = itF->second;
			}
			else
			{
				memcpy(&sFileInfo, psFTI, sizeof(S_FileTransInfo));
				sFileInfo.pICmd = pICmd;
			}

			AddFileTask(s_sFileTrans, sFileInfo, bAdd2Map);
			if ( bAdd2Map )
				SAFE_RELEASE_INTERFACE_(pICmd);
		}
		break;
	case FILE_ACTION_TO_P2P:		// Convert to p2p file(Only by sender)
		EIMLOGGER_DEBUG_(pILog, _T("Filetransfer action: ToP2P(%d), qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
		ASSERT_(FALSE);
		break;
	case FILE_ACTION_CANCEL:		// Cancel send or receive
		{
			EIMLOGGER_DEBUG_(pILog, _T("Filetransfer action: Cancel(%d), qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
			S_EIMQueryInfo sQInfo = { psFTI->u64Fid, psFTI->bitDirect == FILE_DIR_DOWNLOAD ? eInterFile_Download : eInterFile_Upload};
			if ( psFTI->bitType == FILE_TRANS_TYPE_OFFLINE )
			{
				if (eInterFile_NonTask == s_sFileTrans.pIBIFile->DelTransTask(sQInfo))
				{
					C_EIMAutoLock al(s_sFileTrans.csMapFiles);
					if ( psFTI->bitDirect == FILE_DIR_DOWNLOAD )	// Only erase when download
						s_sFileTrans.mapFiles.erase(psFTI->u64Fid);
					psFTI->dwTimestamp = ::GetTickCount();
					psFTI->bitStatus   = FILE_STATUS_CANCEL;
					SAFE_SEND_EVENT_(s_sFileTrans.pIEMgr, EVENT_FILE_TRANSFER, psFTI);
				}

				SAFE_RELEASE_INTERFACE_(pICmd);
			}
			else
				ASSERT_(FALSE);
		}
		break;
	case FILE_ACTION_REJECT:		// Reject receive p2p file
		EIMLOGGER_DEBUG_(pILog, _T("Filetransfer action: Reject(%d), qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
		ASSERT_(FALSE);
		break;
	case FILE_ACTION_RECV:			// Receive file
	case FILE_ACTION_RERECV:		// Re-Receive file(Only offline file)
			EIMLOGGER_DEBUG_(pILog, _T("Filetransfer action: Recv(%d), qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
			{
				C_EIMAutoLock al(s_sFileTrans.csMapFiles);
				MapFilesIt	itF = s_sFileTrans.mapFiles.find(psFTI->u64Fid);

				if ( itF != s_sFileTrans.mapFiles.end() && itF->second.sFile.bitStatus <= FILE_STATUS_TRANSFERING )
				{
					EIMLOGGER_WARN_(pILog, _T("File transfer action exist and working, action:%d, qfid:%llu, File: %s"), psFTI->bitAction, psFTI->u64Fid, psFTI->szFile);
					break;
				}
			}

			if ( psFTI->bitType == FILE_TRANS_TYPE_OFFLINE )
			{	// maybe is history...
				S_FileInfo sFileInfo = { 0 };
				sFileInfo.i32Ratio   = -1;
				memcpy(&sFileInfo, psFTI, sizeof(S_FileTransInfo));
				sFileInfo.pICmd	   = pICmd;
				AddFileTask(s_sFileTrans, sFileInfo, TRUE);
				SAFE_RELEASE_INTERFACE_(pICmd);
			}
			else
			{
				EIMLOGGER_ERROR_(pILog, _T("Filetransfer action bit type not offline type: qfid:%llu"), psFTI->u64Fid);
				ASSERT_(FALSE);
			}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

unsigned  C_eIMEngine::_DoDispThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );
	int			i32Ret;
	I_EIMCmd*	pICmd	 = NULL;
	ListTask	listTask;;
	ListTask	listFileTask;		// File task list

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IBIFILE_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
//	GET_IP2P_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

//	m_sFileTrans.pIP2P	= pIP2P;
	s_sFileTrans.bExit = FALSE;
	if ( s_sFileTrans.pILog == NULL )
		s_sFileTrans.pILog = m_pILogger;

	if ( s_sFileTrans.pIEMgr == NULL )
		s_sFileTrans.pIEMgr  = pIEMgr;

	if ( s_sFileTrans.pIEgn == NULL)
		s_sFileTrans.pIEgn	= this;

	if ( s_sFileTrans.pIBIFile == NULL )
	{	
		int i32FileTread = GetAttributeInt(IME_ATTRIB_FILE_TRANSFER_THREAD_NUM, 2);
		if ( i32FileTread < 1 )
			i32FileTread = 1;
		if ( i32FileTread > 12 )
			i32FileTread = 12;

		s_sFileTrans.pIBIFile = pIBIFile;
		pIBIFile->StatNotify(_FileTransStatNotifyCb, (void*)&s_sFileTrans);
		pIBIFile->Init(1, i32FileTread);
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Entry _DoDispThread(...)"));
	while( pWTD->bExit == FALSE )
	{
		for (ListTaskIt it = listFileTask.begin(); it != listFileTask.end();)
		{	// Check file transfer status
			pICmd = *it;

			if (pICmd->GetFlag( CMD_FLAG_MSG_WAIT_FILE))
			{
				it++;
				continue;
			}

			it = listFileTask.erase(it);
			// Loop to other thread that exist file transfer and finished
			if (pICmd->GetFlag(CMD_FLAG_THREAD_SEND) )		// Enqueue of Send thread
				_AddCmd(eTHREAD_FLAG_SEND_IDX, pICmd, TRUE);
			else if (pICmd->GetFlag(CMD_FLAG_THREAD_DB) )	// Enqueue of DB thread
				_AddCmd(eTHREAD_FLAG_DB_IDX, pICmd, TRUE);

			pICmd->Release();
		}		
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject(pWTD->hTaskEvent, 200 ))
			continue;

		// Dispatch task
		{	// Get all cmds in the list
			C_EIMAutoLock AutoLock(pWTD->csTask);
			pWTD->listTask.swap(listTask);
		}

		for (ListTaskIt it = listTask.begin(); it != listTask.end(); it++)
		{
			pICmd = *it;
			ASSERT_(pICmd);
			if ( pICmd == NULL)
				continue;

			if(!_DoFileAction(pICmd, pILog))
			{
				if ( pICmd->GetFlag( CMD_FLAG_MSG_WAIT_FILE ) )
				{
					pICmd->AddRef();
					listFileTask.push_back(pICmd);
				}

				i32Ret = pICmd->DoCmd(pILog, pIEMgr, &s_sFileTrans, &pWTD->bExit);
				pICmd->SetFlag(~CMD_FLAG_THREAD_UI, CMD_FLAG_THREAD_UI);	// Clear flag
				if ( SUCCEEDED(i32Ret) && !pICmd->GetFlag( CMD_FLAG_MSG_WAIT_FILE))
				{	// Loop to other thread that exist file transfer and finished
					if (pICmd->GetFlag(CMD_FLAG_THREAD_SEND) )		// Enqueue of Send thread
						_AddCmd(eTHREAD_FLAG_SEND_IDX, pICmd, TRUE);
					else if (pICmd->GetFlag(CMD_FLAG_THREAD_DB) )	// Enqueue of DB thread
						_AddCmd(eTHREAD_FLAG_DB_IDX, pICmd, TRUE);
				}

				pICmd->Release();
			}

		}

		listTask.clear();
	}
		
	s_sFileTrans.bExit = TRUE;
	pIBIFile->StatNotify(NULL, NULL);
	for ( MapFilesIt it = s_sFileTrans.mapFiles.begin(); it != s_sFileTrans.mapFiles.end(); )
	{
		ASSERT_( it->second.pICmd );
		if ( it->second.pICmd )
		{
			EIMLOGGER_WARN_(pILog, _T("Not dealwith file task:%s"), it->second.pICmd->GetXml());
			it->second.pICmd->Release();
		}

		it = s_sFileTrans.mapFiles.erase(it);
	}
	
	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoDispThread(...)") );
	return 0;
}

unsigned  C_eIMEngine::_DoDbThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );

	ListTask	listTask;
	I_EIMCmd*	pICmd = NULL;
	I_SQLite3*	pIDb = NULL;
	int			i32Ret = 0;

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	eIMStringA szDbFileA;
	if (FAILED((i32Ret = QueryInterface(INAME_SQLITE_DB, (void**)&pIDb))) ||
		GetDbFile(eDBTYPE_CONTS, szDbFileA) == NULL ||
		SQLITE_OK != (i32Ret = pIDb->Open(szDbFileA.c_str())) ||
		SQLITE_OK != pIDb->Key(DB_KEY, DB_KEY_LEN) ||
		GetDbFile(eDBTYPE_MSG, szDbFileA) == NULL ||
		SQLITE_OK != (i32Ret = pIDb->Attach(szDbFileA.c_str(), MSG_DB_AS_NAME, DB_KEY)) )
	{
		SAFE_RELEASE_INTERFACE_(pIDb);
		EIMLOGGER_ERROR_(m_pILogger, _T("Create database interface failed, Result: 0x%08X"), i32Ret);
		return -1;
	}

	AUTO_RELEASE_(pIDb);
	pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);
	EIMLOGGER_INFO_(m_pILogger, _T("Entry _DoDbThread(...)"));
	while( pWTD->bExit == FALSE )
	{
		if ( WAIT_OBJECT_0 != WaitForSingleObject(pWTD->hTaskEvent, 200 ))
			continue;

		{	// Get all cmds in the list
			C_EIMAutoLock AutoLock(pWTD->csTask);
			pWTD->listTask.swap(listTask);
		}

		for(ListTaskIt it = listTask.begin(); it != listTask.end(); it++)
		{
			pICmd = *it;
			if ( pICmd )
			{
				if ( pICmd->GetCmdId() >= EVENT_USERS )
					i32Ret = pICmd->DoCmd(pILog, (I_SQLite3*)NULL, &pWTD->bExit);
				else
					i32Ret = pICmd->DoCmd(pILog, pIDb, &pWTD->bExit);

				pICmd->SetFlag(~CMD_FLAG_THREAD_DB, CMD_FLAG_THREAD_DB);		// Clear flag
				if ( SUCCEEDED(i32Ret) && pICmd->GetFlag(CMD_FLAG_THREAD_UI) )	// Enqueue of Disp UI 
					_AddCmd(eTHREAD_FLAG_DISP_IDX, pICmd, TRUE);

				pICmd->Release();
			}
		}

		listTask.clear();
	}
		
	C_eIMSessionMgr smgr;
	smgr.Init(pIDb);
	smgr.Set(0, eSET_TYPE_CHECKPOINT, 0);
	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoDbThread(...)") );
	return 0;
}

unsigned C_eIMEngine::_DoStateSubscribeThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );
	int iTempCount = 0;
	C_eIMEngine* pthis = (C_eIMEngine*)pWTD->pThis;
	CHECK_NULL_RET_( pthis, 1 );

	// for test
	/*{
		pthis->m_sLoginAck.cPCSubscribeInterval = 1;
		pthis->m_sLoginAck.cPCGetStatusInterval = 1;
	}*/

	int iSubscribeInterval = pthis->m_sLoginAck.cPCSubscribeInterval * 60 * 10; // n个0.1秒
	while( pWTD->bExit == FALSE )
	{
		if(iTempCount++ > iSubscribeInterval)
		{
			// 状态订阅定时检查
			pthis->m_pSubscribeMgr->CheckStateSubscribeData(&pWTD->bExit);
			// 状态拉取定时检查
			pthis->m_pSubscribeMgr->CheckStateGetData(&pWTD->bExit);
			iTempCount = 0;
		}
		Sleep(100);
	}
	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoStateSubscribeThread(...)") );
	return 0;
}

unsigned C_eIMEngine::_DoUIWaitOverTimeThread(PS_WorkThreadData pWTD)
{
	CHECK_NULL_RET_( pWTD, 1 );
	int iTempCount = 0;
	C_eIMEngine* pthis = (C_eIMEngine*)pWTD->pThis;
	CHECK_NULL_RET_( pthis, 1 );

	while( pWTD->bExit == FALSE )
	{
		pthis->m_pUIWaitOverMgr->CheckWaitOverItem();
		Sleep(1000);
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoUIWaitOverTimeThread(...)") );
	return 0;
}

unsigned C_eIMEngine::_DoCUrlThread(PS_WorkThreadData pWTD)
{
	int			i32Ret;
	ListTask	listTask;
	I_EIMCmd*	pICmd = NULL;

	GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	EIMLOGGER_INFO_(m_pILogger, _T("Entry _DoCUrlThread(...)"));
	while( pWTD->bExit == FALSE )
	{
		if ( WAIT_OBJECT_0 != WaitForSingleObject(pWTD->hTaskEvent, 1000) )
			continue;
		else
		{	// Get all command in list
			C_EIMAutoLock AutoLock(pWTD->csTask);
			pWTD->listTask.swap(listTask);
		}

		for(ListTaskIt it = listTask.begin(); it != listTask.end(); it++)
		{
			pICmd = *it;
			if ( pICmd )
			{
				try{
					i32Ret = pICmd->DoCmd(pILog, pICa, &pWTD->bExit);
					if (SUCCEEDED(i32Ret))
					{	
						if ( pICmd->GetFlag(CMD_FLAG_THREAD_UI) )	// Enqueue of Dispach UI without db
							pICmd->DoCmd(pILog, pIEMgr, NULL, &pWTD->bExit);
					}
					pICmd->Release();
				}
				catch(...)
				{
					pICmd->Release();
				}
			}
		}

		listTask.clear();
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoCUrlThread(...)") );
	return 0;
}

unsigned C_eIMEngine::_DoDownloadUrlFileThread(PS_WorkThreadData pWTD)
{
    CHECK_NULL_RET_( pWTD, 1 );
	int iTempCount = 0;
	C_eIMEngine* pthis = (C_eIMEngine*)pWTD->pThis;
	CHECK_NULL_RET_( pthis, 1 );
  
	while( pWTD->bExit == FALSE )
	{
        T_UrlFileTranInfo info;
        if(pthis->GetFisrtDownUrlInfo(info))
        {         
             eIMStringA szUrl;
            ::eIMTChar2MByte(info.szUrlFile, szUrl);

            tUrlFileTranRes tUrlRes;
            BOOL bOk = SUCCEEDED(pthis->_cUrlDownload(szUrl.c_str(), info.szSaveFile));
            tUrlRes.i32Progress  = (bOk ? 100 : 0);
            tUrlRes.status = (bOk ? URL_DOWN_OK : URL_DOWN_FAIL);
            _tcsncpy(tUrlRes.szUrlFile, info.szUrlFile, COUNT_OF_ARRAY_(tUrlRes.szUrlFile));
            _tcsncpy(tUrlRes.szSavePath, info.szSaveFile, COUNT_OF_ARRAY_(tUrlRes.szSavePath));
            hpi.SendEvent(EVENT_URL_FILE_TRAN_RES, &tUrlRes);

            std::list<T_UrlFileTranInfo>::iterator it = m_urlFileDownLst.begin();
            while(it != m_urlFileDownLst.end())
            {
                eIMString szStopUrl(info.szUrlFile);
                eIMString szSavePath(info.szSaveFile);
                if( 0 == szStopUrl.compare( (*it).szUrlFile) 
                    && 0 == szSavePath.compare( (*it).szSaveFile) )
                {
                    m_urlFileDownLst.erase(it);
                    break;
                }
                ++it;
            }
        }
        else
        {
     		pthis->StopDownUrlThread();
            break;
        }
	}
	EIMLOGGER_INFO_(m_pILogger, _T("Exit _DoDownloadUrlFileThread(...)") );
    return 0;
}

bool C_eIMEngine::GetFisrtDownUrlInfo(T_UrlFileTranInfo& fileInfo )
{
    if( m_urlFileDownLst.empty())
    {
        return false;
    }
    
    std::list<T_UrlFileTranInfo>::iterator it = m_urlFileDownLst.begin();
    while(it != m_urlFileDownLst.end())
    {
        if((*it).bitAction == FILE_ACTION_CANCEL)
        {
            m_urlFileDownLst.erase(it);
            it = m_urlFileDownLst.begin();
            continue;
        }
        fileInfo.bitAction = (*it).bitAction;
        _tcsncpy( fileInfo.szSaveFile, (*it).szSaveFile, COUNT_OF_ARRAY_(fileInfo.szSaveFile) );
        _tcsncpy( fileInfo.szUrlFile, (*it).szUrlFile, COUNT_OF_ARRAY_(fileInfo.szUrlFile) );

        return true;
    }
    return false;
}

void C_eIMEngine::StopDownUrlThread()
{
	m_aThreads[eTHREAD_FLAG_DOWNLOAD_URLFILE].dwFlag		= 1 << eTHREAD_FLAG_DOWNLOAD_URLFILE;
    m_aThreads[eTHREAD_FLAG_DOWNLOAD_URLFILE].hThread	= NULL;
	m_aThreads[eTHREAD_FLAG_DOWNLOAD_URLFILE].bExit		= FALSE;
}

inline int C_eIMEngine::_AddCmd(E_ThreadIdx eThreadIdx, I_EIMCmd* pCmd, BOOL bAddRef)
{
	if ( pCmd == NULL )
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Invalid cmd interface pointer") );
		return EIMERR_INVALID_POINTER;
	}

	if ( bAddRef )
		pCmd->AddRef();

	if ( pCmd->GetFlag(CMD_FLAG_THREAD_CURL) )
		eThreadIdx = eTHREAD_FLAG_CURL_IDX;

	{	// Add to cmd list
		C_EIMAutoLock AutoLock(m_aThreads[eThreadIdx].csTask);
		m_aThreads[eThreadIdx].listTask.push_back(pCmd);
	}

	::SetEvent(m_aThreads[eThreadIdx].hTaskEvent);

	return EIMERR_NO_ERROR;
}

inline eIMString C_eIMEngine::_MakeSubSendCmd(const eIMString& szCmdAttr, const eIMString& szMsgUIAttr, const eIMString& szSubEl, const eIMString& szItemAttr )
{
	eIMString szCmd;

	szCmd = _T("<Cmd ");		// <Cmd
	szCmd+= szCmdAttr;			// Cmd attr
	szCmd+= _T(">");			// >

	szCmd+= _T("<MsgUI ");		// <MsgUI 
	szCmd+= szMsgUIAttr;		// MsgUI attr
	szCmd+= _T(">");			// >

	szCmd+= _T("<Message>");	// <Message>
	szCmd+= szSubEl;			// SubEl
	szCmd+= _T("</Message>");	// </Message>

	szCmd+= _T("</MsgUI>");		// </MsgUI>

	if(!szItemAttr.empty())
	{
		szCmd+= _T("<Item ");		// <Item 
		szCmd+= szItemAttr;		// Item attr
		szCmd+= _T("/>");			// />
	}

	szCmd+= _T("</Cmd>");		// </Cmd>

	return szCmd;
}

inline eIMString C_eIMEngine::MakeReciveSubSendCmd(const eIMString& szCmdAttr, const eIMString& szMsgUIAttr
                                                            , const eIMString& szSubEl, const eIMString& szItemAttr )
{
    eIMString szCmd;

    szCmd = _T("<Cmd ");		// <Cmd
    szCmd+= szCmdAttr;			// Cmd attr
    szCmd+= _T(">");			// >

    szCmd+= _T("<MsgUI ");		// <MsgUI 
    szCmd+= szMsgUIAttr;		// MsgUI attr
    szCmd+= _T(">");			// >

    szCmd+= _T("<Message>");	// <Message>
    szCmd+= szSubEl;			// SubEl
    szCmd+= _T("</Message>");	// </Message>

    szCmd+= _T("</MsgUI>");		// </MsgUI>

    if(!szItemAttr.empty())
    {
        szCmd+= szItemAttr;
    }

    szCmd+= _T("</Cmd>");		// </Cmd>

    return szCmd;
}

inline BOOL C_eIMEngine::_GetAttrString(TiXmlElement* pEl, eIMString& szAttr)
{
	CHECK_NULL_RET_(pEl, FALSE);
	eIMStringA szAttrA;
	for ( TiXmlAttribute* pAttr = pEl->FirstAttribute(); pAttr; pAttr = pAttr->Next())
	{
		pAttr->Print(NULL, 0, &szAttrA);
		szAttrA += " ";
	}

	::eIMUTF8ToTChar(szAttrA.c_str(), szAttr);
	return (szAttr.size() > 0 );
}

inline int C_eIMEngine::_AddCmd(E_ThreadIdx eThreadIdx, DWORD dwCmdId, const void* pvData)
{	// const TCHAR* const kCmdSendMsgFmt   = _T("<Cmd %s><MsgUI %s><Message>%s</Message></MsgUI></Cmd>");
	int				i32Ret = 0;
	I_EIMCmd*		pCmd = NULL;
	if ( eThreadIdx == eTHREAD_FLAG_SEND_IDX && dwCmdId == eCMD_SEND_MSG )
	{
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, (TCHAR*)pvData, dwCmdId, i32Ret);
		CHECK_NULL_RET_(pEl, EIMERR_INVALID_POINTER);

		eIMStringA oStr;
		oStr << *pEl;

		TiXmlHandle		h(pEl);
		eIMString		szCmdAttr;
		eIMString		szMsgUIAttr;
		eIMString		szSubAttr;
		eIMString		szCmd;
		eIMString		szCmdSub;
		TiXmlElement*	pSubEl = NULL;
		const char*		pszValue = NULL;
		int				i32SplitCount = 0;

		 _GetAttrString(pEl, szCmdAttr);
		 _GetAttrString(h.FirstChildElement(FIELD_MSGUI).ToElement(), szMsgUIAttr);
         eIMString szReceiveItem = GetReciveItems(h); 
		 while (pSubEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGANNEX).ToElement() )
		 {
			 _GetAttrString(pSubEl, szSubAttr);
			 szCmdSub = _T("<");
			 szCmdSub+= _T(FIELD_MSGANNEX);
			 szCmdSub+=_T(" ");
			 szCmdSub+= szSubAttr;
			 szCmdSub+= _T(" />");

             if(szReceiveItem.empty())
             {
                 eIMString szItemAttr;
                 _GetAttrString(h.FirstChildElement(FIELD_ITEM).ToElement(), szItemAttr);
                 szCmd     = _MakeSubSendCmd(szCmdAttr, szMsgUIAttr, szCmdSub, szItemAttr);
             }
             else
             {
                 szCmd = MakeReciveSubSendCmd(szCmdAttr, szMsgUIAttr, szCmdSub, szReceiveItem);
             }
			 
			 pCmd	   = CmdCreateInstance<I_EIMCmd*>( dwCmdId, szCmd.c_str() );
			 
			 SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_SEND_MSG, (void*)pCmd);
			 i32Ret = _AddCmd(eThreadIdx, pCmd);
			 if (FAILED(i32Ret) || FAILED(i32Ret = pCmd->GetResult()) )
			{
				szCmdAttr = pCmd->GetXml();
				EIMLOGGER_ERROR_(m_pILogger, _T("Add cmd[%s] failed, Ret:0x%08X, Desc:%s"), szCmdAttr.substr(0, 1024), i32Ret, hpi.GetErrorLangInfo(i32Ret, _T("Unknown error")) );
				return i32Ret;
			 }

			 h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement()->RemoveChild(pSubEl);
			 i32SplitCount++;
		 }

		 DWORD dwChildCount = 0;
		 BOOL  bNotShow = !!h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGAUTO).ToElement() |
						  !!h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGRDP).ToElement();

		 for ( pSubEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
			 i32SplitCount > 0 && pSubEl; pSubEl = pSubEl->NextSiblingElement() )
		 {
			 if (strcmp(FIELD_MSGFONT, pSubEl->Value()) == 0 )
				 continue;

			 dwChildCount++;
		 }

		 if ( dwChildCount || i32SplitCount == 0 )
		 {
			 eIMStringA oStr;
			 oStr << *pEl;
			 ::eIMUTF8ToTChar(oStr.c_str(), szCmd);
			 pCmd = CmdCreateInstance<I_EIMCmd*>( dwCmdId, szCmd.c_str());
			 if ( !bNotShow )
				 SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_SEND_MSG, (void*)pCmd);
		 }
		 else
			return EIMERR_NO_ERROR;
	}
	else
	{
		pCmd = CmdCreateInstance<I_EIMCmd*>( dwCmdId, pvData);
	}

	CHECK_NULL_RET_(pCmd, EIMERR_NO_ERROR);
	if (FAILED(i32Ret = pCmd->GetResult()) )
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Add cmd failed, Ret:0x%08X, Desc:%s"), i32Ret, hpi.GetErrorLangInfo(i32Ret, _T("Unknown error")) );
		SAFE_RELEASE_INTERFACE_(pCmd);
		return i32Ret;
	}

	return _AddCmd(eThreadIdx, pCmd);
}

C_eIMSaveData* C_eIMEngine::GetEIMSaveDataPtr()
{
	return m_pSaveData;
}

DWORD C_eIMEngine::GetTimeNow()
{	// SvrNow + (CurrentTime - VerifySysTime)
	DWORD dwTimeNow = GetTickCount();
	DWORD dwRet = 0;
	if ( dwTimeNow < m_sLastUpdateTTs.dwTTSysNow )
	{	//  49.7 days overflow
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_CHECK_TIME_REQ, (void*)&dwTimeNow);
		dwRet = m_sLastUpdateTTs.dwTTSvrNow + ((0xFFFFFFFFll + dwTimeNow) - m_sLastUpdateTTs.dwTTSysNow) / 1000;
	}
	else
		dwRet = m_sLastUpdateTTs.dwTTSvrNow + (dwTimeNow - m_sLastUpdateTTs.dwTTSysNow)/1000;

	if ( dwRet == 0 )
	{
		dwRet = (DWORD)time(NULL);
		EIMLOGGER_ERROR_(m_pILogger, _T("dwTTSvrNow:%u, dwTTSysNow:%u, dwTimeNow:%u, CorrectTo: %u"), m_sLastUpdateTTs.dwTTSvrNow, m_sLastUpdateTTs.dwTTSysNow, dwTimeNow, dwRet);
	}
//  EIMLOGGER_DEBUG_(m_pILogger, _T("NowTime dwTTSvrNow:%u, dwTTSysNow:%u, dwTimeNow:%u, CorrectTo: %u"), m_sLastUpdateTTs.dwTTSvrNow, m_sLastUpdateTTs.dwTTSysNow, dwTimeNow, dwRet);
	return dwRet;
}

int C_eIMEngine::Command(const TCHAR*	pszXml, E_ThreadIdx eThreadId)
{
	CHECK_NULL_RET_(pszXml, EIMERR_INVALID_POINTER);
	DWORD dwId = 0;
	int i32Ret = EIMERR_NO_ERROR;
	if (!(m_eEngineStatus == eENGINE_STATUS_ONLINE || m_eEngineStatus == eENGINE_STATUS_LEAVE))
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Not login, status:%d"), m_eEngineStatus);
		i32Ret = (m_eEngineStatus == eENGINE_STATUS_OFFLINE) ? EIMERR_OFFLINE_MSG : EIMERR_NOT_LOGIN;
	}
	else
	{
		DWORD  dwTag = MAKE4CC(pszXml[0], pszXml[1], pszXml[2], pszXml[3]);
		const TCHAR* pszId = _tcsstr(pszXml, _T(PROTOCOL_ATTRIB_ID));	// Find "Id"
		if ( dwTag == CMD_TAG && pszId)
		{	// Is "<Cmd"
			pszId += _tcslen(_T(PROTOCOL_ATTRIB_ID));
			while (*pszId == _T(' ') || *pszId == _T('=') || *pszId == '\"' || *pszId == '\'')
				pszId++;	// Skip space and equal sign

			dwId = ::Str2Int(pszId, 0);	// Get cmd's id
			i32Ret = _AddCmd(eThreadId, dwId, pszXml);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pILogger, _T("Unknown command: %s"), pszXml );
			i32Ret = EIMERR_BAD_CMD;
		}
	}

	if (FAILED(i32Ret))
	{
		eIMStringA		szValueA;
		TiXmlDocument	xmlDoc;

		::eIMTChar2UTF8(pszXml, szValueA);
		xmlDoc.Parse( szValueA.c_str() );

		TiXmlHandle	h(xmlDoc.RootElement());
		TiXmlElement* pMsgEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement();
		if ( pMsgEl )
		{
			TiXmlElement* pSubEl = NULL;

			while ( pSubEl = pMsgEl->FirstChildElement() )
			{
				pMsgEl->RemoveChild(pSubEl);
			}

			eIMStringA szErrorA;
			::eIMTChar2UTF8(hpi.GetErrorLangInfo(i32Ret, NULL), szErrorA);
			TiXmlText	  ErrorText(szErrorA.c_str());
			TiXmlElement  ErrorEl(FIELD_MSGERROR);
			
			ErrorText.SetCDATA(true);
			ErrorEl.InsertEndChild(ErrorText);
			AddFontElement(eSESSION_TYPE_ERROR, TRUE, h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement());
			h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement()->InsertEndChild(ErrorEl);

			eIMString szError;
			szErrorA.clear();
			szErrorA << *xmlDoc.RootElement();
			::eIMUTF8ToTChar(szErrorA.c_str(), szError);

			I_EIMCmd* pCmd = CmdCreateInstance<I_EIMCmd*>( eCMD_SEND_MSG, szError.c_str());
			CHECK_NULL_RET_(pCmd, EIMERR_BAD_CMD);
			if ( pCmd )
			{
				SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_SEND_MSG, (void*)pCmd);
				SAFE_RELEASE_INTERFACE_(pCmd);
			}
		}
 	}

	return i32Ret;
}

// Attribute /////////////////////////////////////////////////////////////////////
//=============================================================================
//Function:     GetAttributeStr
//Description:	Get string attribute
//
//Parameter:
//	pszName       - Attribute Name
//	pszDefault    - Default value
//
//Return:
//		NULL		If pszName is invaild
//		pszDefault	If not exist
//		Non-Null	The attribute value
//=============================================================================
const TCHAR* C_eIMEngine::GetAttributeStr(
	const TCHAR*	pszName,
	const TCHAR*	pszDefault
	)
{
	if ( pszName == NULL || pszName[0] == _T('\0') )
		return NULL;

	MapAttribStrIt it = m_mapAttrStr.find( pszName );
	if ( it == m_mapAttrStr.end() )
		return pszDefault;

	return it->second.c_str();
}

//=============================================================================
//Function:     SetAttributeStr
//Description:	Set string attribute
//
//Parameter:
//	pszName     - Attribute name
//	pszValue    - Attribute string value
//
//Return:
//		NULL		If pszName is invalid, or no old value
//		Non-Null	The old value
//=============================================================================
const BOOL C_eIMEngine::SetAttributeStr(
	const TCHAR*	pszName,
	const TCHAR*	pszValue
	) 
{
	if ( pszName == NULL || pszName[0] == _T('\0') )
		return FALSE;

	MapAttribStrIt it = m_mapAttrStr.find( pszName );
	if ( it == m_mapAttrStr.end() )
	{	// No exist
		if ( pszValue )	// Not NULL to add it, else not need to delete
			m_mapAttrStr.insert( MapAttribStr::value_type(pszName, pszValue) );
	}
	else
	{	// Exist
		if ( pszValue == NULL || pszValue[0] == _T('\0') )
			m_mapAttrStr.erase( it );	// Delete
		else
			it->second = pszValue;		// Modify
	}

	return TRUE;
}

//=============================================================================
//Function:     GetAttributeInt
//Description:	Get integer attribute
//
//Parameter:
//	pszName       - Attribute name
//	i32Default    - Default value
//
//Return:
//		NULL		If name is invalid
//		i32Default	If not exist
//		Non-Null	The attribute value
//=============================================================================
const int C_eIMEngine::GetAttributeInt(
	const TCHAR*	pszName,
	int				i32Default
	) 
{
	const TCHAR* pszValue = GetAttributeStr(pszName);
	return Str2Int( pszValue, i32Default);
}

//=============================================================================
//Function:     SetAttributeInt
//Description:	Set integer attribute
//
//Parameter:
//	pszName     - Attribute name
//	i32Value    - Attribute value
//
//Return:
//		0		If name is invalid, or no old value
//		Non-0	The old value
//=============================================================================
const BOOL C_eIMEngine::SetAttributeInt(
	const TCHAR*	pszName,
	const int		i32Value
	)
{
	TCHAR szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
	return SetAttributeStr(pszName, IntToStr(i32Value, szBuf, FALSE));
}

inline BOOL	C_eIMEngine::_IsInit()
{
	return m_bInited;
}
	
inline int C_eIMEngine::_Login(BOOL bReconn)
{
	if ( eENGINE_STATUS_LOGINING == m_eEngineStatus )
	{
		EIMLOGGER_WARN_(m_pILogger, _T("Already is logging, skip the login action."));
		return EIMERR_NO_ERROR;
	}

	SetAttributeInt(IME_ATTRIB_RECONNECT_TIMES, hpi.UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_RECONNECTTIMES,-1));
	// 登录超时处理

	S_Login sLogin;
	
	SetAttributeInt(IME_ATTRIB_OFFMSG_FINISHED, 0);
	memset(&sLogin, 0, sizeof( S_Login));
	sLogin.bReconn = bReconn;
	sLogin.cMode   = GetAttributeInt( IME_ATTRIB_ACCESS_MODE, CMNET ) == CMNET ? CMNET : CMWAP;
	sLogin.sLogin.cLoginType = TERMINAL_PC;
	
	eIMStringA szValueA;
	::eIMTChar2UTF8(GetAttributeStr(IME_ATTRIB_USER_NAME), szValueA);
	if (szValueA.empty())
	{
		TRACE_(_T("Empty login name"));
		return EIMERR_INVALIDUSER;
	}

	strncpy(sLogin.sLogin.tAccount.value, szValueA.c_str(), COUNT_OF_ARRAY_(sLogin.sLogin.tAccount.value));
	szValueA.clear();

	eIMString szPsw = GetAttributeStr(IME_ATTRIB_USER_PSW);
	if ( szPsw == LOGIN_USER_DUMMY_PSW )
	{
		C_eIMLoginUserMgr umgr;
		if ( umgr.Init() &&	umgr.Get(GetAttributeStr(IME_ATTRIB_USER_NAME), szPsw) )
			SetAttributeStr(IME_ATTRIB_USER_PSW, szPsw.c_str());
	}

	::eIMTChar2UTF8(szPsw.c_str(), szValueA );
	memcpy(sLogin.sLogin.aszPassword, szValueA.c_str(), PASSWD_MAXLEN);
	szValueA.clear();

//	sLogin.sLogin.aszDeviceToken	// Not used
	sLogin.asSvr[0].i32Port = GetAttributeInt(IME_ATTRIB_SERVER_PORT);
	sLogin.asSvr[1].i32Port = GetAttributeInt(IME_ATTRIB_SERVER_PORTB);
	const TCHAR* pszIP = GetAttributeStr(IME_ATTRIB_SERVER_IP);
	if ( pszIP )
	{
		_tcsncpy(sLogin.asSvr[0].szIP, pszIP, CONNECT_IP_SIZE - 1);
		pszIP = GetAttributeStr(IME_ATTRIB_SERVER_IPB);
		if ( pszIP )
			_tcsncpy(sLogin.asSvr[1].szIP, pszIP, CONNECT_IP_SIZE - 1);
	}
	else
	{
		pszIP = GetAttributeStr(IME_ATTRIB_SERVER_IPB);
		if ( pszIP )
			_tcsncpy(sLogin.asSvr[0].szIP, pszIP, CONNECT_IP_SIZE - 1);
	}

	UpdateLoginState(eENGINE_STATUS_LOGINING);
	SetAttributeInt(IME_UPDATE_CONTACTS_TYPE, 0);
	SetAttributeInt(IME_GET_DATA_LIST_TYPE, 0);
	SetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO, 1);
	SetAttributeInt(IME_RELOAD_CONTACTS, 0);

	// Send Login cmd
	if ( GetAttributeInt(IME_ATTRIB_ENABLE_SSO) )
		return _AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_SSO_LOGIN, &sLogin);	// SSO login
	else
		return _AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_LOGIN, &sLogin);		// Normal login
}

inline BOOL C_eIMEngine::_LoadProtocolConfigFile()
{	// As: _T("$(Exe)\\Protocol.config")
	eIMString szProtocolFile = PATH_TOKEN_EXE_FILE_(PROTOCOL_CONFIG_FILE);
	eIMReplaceToken( szProtocolFile );

	if ( FAILED(C_XmlProtocol::GetObject().Open( szProtocolFile.c_str(), NULL)) )
		return FALSE;

	return TRUE;
}

//BOOL C_eIMEngine::_ModifyPath(BOOL bGlobal, QEID qeid, BOOL bBkupdb)

BOOL C_eIMEngine::_InitDb(BOOL bGlobal, QEID qeid, BOOL bBkupdb)
{
	BOOL		  bRet		   = FALSE;
	eIMString	  szDllUpdater = PATH_TOKEN_EXE_FILE_(EIME_SUBMOD_UPDATER);
	eIMString	  szUpdateFile = PATH_TOKEN_EXE_FILE_(UPDATE_CONFIG_FILE);
	C_PluginDll   DllUpdater(ePLUGIN_TYPE_NORMAL, INAME_EIM_UPDATER );
	I_eIMUpdater* pIUpdater = NULL;

	::eIMReplaceToken(szUpdateFile);
	::eIMReplaceToken(szDllUpdater);
	if (DllUpdater.Load(szDllUpdater.c_str()) &&
		SUCCEEDED(DllUpdater.eIMCreateInterface(INAME_EIM_UPDATER, (void**)&pIUpdater)))
	{
		AUTO_RELEASE_(pIUpdater);
		if ( bBkupdb )
		{
			int		i32Index = 0;
			TCHAR	szValue[XML_VALUE_SIZE_MAX];
			TCHAR	szPath[MAX_PATH];
			C_XmlConfiger cfgr;

			if ( FAILED(cfgr.Open(szUpdateFile.c_str())) )
				return FALSE;

			do
			{
				szValue[0] = _T('\0');
				_stprintf(szPath, _T("\\database[%d]"), i32Index++);
				if (!cfgr.GetAttributeStr(szPath, _T("file"), szValue, XML_VALUE_SIZE_MAX, NULL))
					break;

				if ( StrStrI(szValue, USERS_DB_FILE) )
					_stprintf(szValue, _T("%s~1\\%s"), _T("$(LoginId)"), USERS_DB_FILE);
				else if( StrStrI(szValue, CONTACTS_DB_FILE) )
					_stprintf(szValue, _T("%s~1\\%s"), _T("$(LoginId)"), CONTACTS_DB_FILE);
				else if( StrStrI(szValue, MSG_DB_FILE) )
					_stprintf(szValue, _T("%s~1\\%s"), _T("$(LoginId)"), MSG_DB_FILE);
				else
					szValue[0] = _T('\0');

				if ( szValue[0] )
					cfgr.SetAttributeStr(szPath, _T("file"), szValue);
			}while(1);

			eIMString  szBkupUpdateFile;
			::eIMGetFullPath(PATH_TOKEN_LOGINID, PATH_TYPE_CONTACTS_TMP, UPDATE_CONFIG_FILE, szBkupUpdateFile, FALSE);
			cfgr.Save(szBkupUpdateFile.c_str());
			szUpdateFile = szBkupUpdateFile;
		}

		S_EIMUpdaterInfo sUpdaterInfo;

		memset(&sUpdaterInfo, 0, sizeof(S_EIMUpdaterInfo));
		sUpdaterInfo.eUpdaterType		= eUPDATER_DB;
		
		int i32Language = hpi.UIGetAttributeInt(PATH_SETTING_BASIC, SETTING_BASIC_ATTR_LANGUAGE, (int)eSYSSETTING_ENGLISH);
		if(eSYSSETTING_CHINESE == i32Language)
			sUpdaterInfo.eLanguage			= eUPDATER_CN;
		else
			sUpdaterInfo.eLanguage			= eUPDATER_EN;
			
		sUpdaterInfo.seIMUdpDb.u8PswLen = DB_KEY_LEN;
		memcpy(sUpdaterInfo.seIMUdpDb.szPsw, DB_KEY, sUpdaterInfo.seIMUdpDb.u8PswLen);
		sUpdaterInfo.seIMUdpDb.dwFlag	= bGlobal ? UPDATEDB_FLAG_GLOBAL : 0;
		sUpdaterInfo.seIMUdpDb.i32DbVer = GetAttributeInt(APP_ATTR_DATABASE_VERSION);
		_tcsncpy(sUpdaterInfo.seIMUdpDb.szXmlPath, szUpdateFile.c_str(), COUNT_OF_ARRAY_(sUpdaterInfo.seIMUdpDb.szXmlPath) -1);

		bRet = SUCCEEDED(pIUpdater->Updater(sUpdaterInfo));
	}

	DllUpdater.Unload();
	return bRet;
}

BOOL C_eIMEngine::InitPlugin(		// Initial plugin, succeed if return TRUE
//Del 	I_EIMPluginMgr*	pPluginMgr,	// Tool manager interface	//Single line deleted by lwq on 2013/11/26 9:03:54
	I_EIMEventMgr*	pEventMgr,		// Event manager interface
	LPCTSTR			lpszConfigFile	// Config file path
	)
{
	_InitDb(TRUE, 0);
	m_bInited = FALSE;
	I_EIMUnknown* pUnk = NULL;		// Dummy only for initial p2p env
	CHECK_NULL_RET_(pEventMgr, FALSE);
	// Load sub-function module...
	if (_LoadProtocolConfigFile() && 
		m_DllSQLite3.Load(EIME_SUBMOD_SQLITE3) &&		// Load SQLite3
		m_DllClientAgent.Load(EIME_SUBMOD_CLIENT) &&	// ClientAgent
		SUCCEEDED(m_DllClientAgent.eIMCreateInterface(INAME_CLIENT_AGENT, (void**)&m_pIClientAgent)) &&
		m_DllLogger.Load(EIME_SUBMOD_LOGGER) &&			// eIMLogger
		SUCCEEDED(m_DllLogger.eIMCreateInterface(INAME_EIMLOGGER, (void**)&m_pILogger)) &&
	//	m_DllP2P.Load(EIME_SUBMOD_P2P) &&
	//	SUCCEEDED(m_DllP2P.eIMCreateInterface(INAME_EIMP2P, (void**)&pUnk)) &&
		SUCCEEDED(::eIMPinyinCreateInterface(INAME_EIMUI_PINYIN, (void**)&m_pIPinyin)) &&
		m_DllIFile.Load(EIME_SUBMOD_IFILE) && 
		SUCCEEDED(m_DllIFile.eIMCreateInterface(INAME_EIMBATCH_INTER_FILE, (void**)&m_pIBIFile)) )
	{
		I_SQLite3* pIDb = NULL;
		if (SUCCEEDED(m_DllSQLite3.eIMCreateInterface(INAME_SQLITE_DB, (void**)&pIDb)))
		{
			AUTO_RELEASE_(pIDb);
			pIDb->Initialize();		// Init SQLite3 DLL
		}

		m_pIEventMgr = pEventMgr;	// Save the event manager interface
		m_pIEventMgr->AddRef();
		m_pIContacts = &C_eIMContacts::GetObject();
		C_eIMContacts::GetObject().SetEventMgr( pEventMgr );
		m_pSubscribeMgr = &C_eIMStateSubscribeMgr::GetObject();

		m_pUIWaitOverMgr = &C_UIWaitOverMgr::GetObject();
		m_pUIWaitOverMgr->Init(pEventMgr);
  //    C_eIMUrlFileMgr::GetObject().Init();	// 不能在这儿初始化，因为还没有登录。
        m_pUrlFileMgr = &C_eIMUrlFileMgr::GetObject();

		m_pIEventMgr->RegisterEvent(eCMD_DEPTSHOWCONFIG_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_GET_DATA_LIST_TYPE_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_RECV_OFFLINE_MSG, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_ENGINE_FILE_TRANS, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_RELOAD_CONTACTS, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_LOGIN_STATUS, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_GROUP_INFO, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_VGROUP_INFO, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_SEND_CMD, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_CLEAR_EMPS_AFTER, this, eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_INIT_DATABASE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_SEND_CMD_BEFORE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_SEND_CMD_AFTER, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_RECV_CMD_BEFORE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_RECV_CMD_AFTER, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_DEPT_INFO_END, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_DEPT_EMPS_END, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_EMPS_INFO_END, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_SPECIAL_LIST_END, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_FILE_TRANSFER_ACTION, this, eIMET_RECEIVE);
        m_pIEventMgr->RegisterEvent(EVENT_URL_FILE_TRANSFER_ACTION, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_GET_EMPSAVATAR_LIST_END, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_LOAD_CONTS_END, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_FILE_TRANSFER,this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(EVENT_UI_WAIT_OVERTIME,this, eIMET_RECEIVE);
		

		m_pIEventMgr->RegisterEvent(eCMD_LOGIN_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_LOGOUT_ACK, this, eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(eCMD_MODIFY_INFO_NOTICE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_GET_EMPLOYEE_INFO_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_CREATE_GROUP_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_BROADCAST_NOTICE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_MSG_READ, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_MSG_READ_NOTICE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_MODIFY_SELF_INFO_NOTICE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_COMP_LAST_TIME_NOTICE, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_GET_CO_INFO_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_GET_USER_STATE_ACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_ROAMINGDATASYNACK, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_ROBOTSYNCRSP, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_CHECK_TIME_RSP, this, eIMET_RECEIVE);
		m_pIEventMgr->RegisterEvent(eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE, this, eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_CERATE_MEETING, this, eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_GET_LAUNCHSTRING,this,eIMET_RECEIVE);
  //      m_pIEventMgr->RegisterEvent(EVENT_MEETING_ACCOUNT_ACK,this,eIMET_RECEIVE);
  //      m_pIEventMgr->RegisterEvent(EVENT_DELETEMEETING,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_CHANGEMEETINGMEMBER,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_CHANGE_MEETINGLEVEL,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_REGETMEETINGINFO,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_RECEIVEMEETING,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_MEETINGREAD,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_MEETING_GETVIDEOURL,this,eIMET_RECEIVE);
		//m_pIEventMgr->RegisterEvent(EVENT_GET_MEETINGROOM_LIST,this,eIMET_RECEIVE);

		m_bInited = TRUE;				// Already initialed the eIMEngine, at last
	}

	return m_bInited;
}

void C_eIMEngine::FreePlugin()		// Free plugin
{
	SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_EVENT_LOOP, EVENT_LOOP_DISABLE);	// Disable event loop

	m_pIEventMgr->UnregisterEvent(eCMD_DEPTSHOWCONFIG_ACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_GET_DATA_LIST_TYPE_ACK, this);
	m_pIEventMgr->UnregisterEvent(EVENT_RECV_OFFLINE_MSG, this);
	m_pIEventMgr->UnregisterEvent(EVENT_ENGINE_FILE_TRANS, this);
	m_pIEventMgr->UnregisterEvent(EVENT_RELOAD_CONTACTS, this);
	m_pIEventMgr->UnregisterEvent(EVENT_LOGIN_STATUS, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_GROUP_INFO, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_VGROUP_INFO, this);
	m_pIEventMgr->UnregisterEvent(EVENT_SEND_CMD, this);
	m_pIEventMgr->UnregisterEvent(EVENT_CLEAR_EMPS_AFTER, this);
	//m_pIEventMgr->UnregisterEvent(EVENT_INIT_DATABASE, this);
	m_pIEventMgr->UnregisterEvent(EVENT_SEND_CMD_BEFORE, this);
	m_pIEventMgr->UnregisterEvent(EVENT_SEND_CMD_AFTER, this);
	m_pIEventMgr->UnregisterEvent(EVENT_RECV_CMD_BEFORE, this);
	m_pIEventMgr->UnregisterEvent(EVENT_RECV_CMD_AFTER, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_DEPT_INFO_END, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_DEPT_EMPS_END, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_EMPS_INFO_END, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_SPECIAL_LIST_END, this);
	m_pIEventMgr->UnregisterEvent(EVENT_FILE_TRANSFER_ACTION, this);
    m_pIEventMgr->UnregisterEvent(EVENT_URL_FILE_TRANSFER_ACTION, this);
	m_pIEventMgr->UnregisterEvent(EVENT_GET_EMPSAVATAR_LIST_END, this);
	m_pIEventMgr->UnregisterEvent(EVENT_LOAD_CONTS_END, this);
	m_pIEventMgr->UnregisterEvent(EVENT_FILE_TRANSFER,this);
	m_pIEventMgr->UnregisterEvent(EVENT_UI_WAIT_OVERTIME,this);
    	
	m_pIEventMgr->UnregisterEvent(eCMD_LOGIN_ACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_LOGOUT_ACK, this);
	//m_pIEventMgr->UnregisterEvent(eCMD_MODIFY_INFO_NOTICE, this);
	m_pIEventMgr->UnregisterEvent(eCMD_GET_EMPLOYEE_INFO_ACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_CREATE_GROUP_ACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_BROADCAST_NOTICE, this);
	m_pIEventMgr->UnregisterEvent(eCMD_MSG_READ, this);
	m_pIEventMgr->UnregisterEvent(eCMD_MSG_READ_NOTICE, this);
	m_pIEventMgr->UnregisterEvent(eCMD_MODIFY_SELF_INFO_NOTICE, this);
	m_pIEventMgr->UnregisterEvent(eCMD_COMP_LAST_TIME_NOTICE, this);
	m_pIEventMgr->UnregisterEvent(eCMD_GET_CO_INFO_ACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_GET_USER_STATE_ACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_ROAMINGDATASYNACK, this);
	m_pIEventMgr->UnregisterEvent(eCMD_ROBOTSYNCRSP, this);
	m_pIEventMgr->UnregisterEvent(eCMD_CHECK_TIME_RSP, this);
	m_pIEventMgr->UnregisterEvent(eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE, this);
	//m_pIEventMgr->UnregisterEvent(EVENT_CERATE_MEETING,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_GET_LAUNCHSTRING,this);
 //   m_pIEventMgr->UnregisterEvent(EVENT_MEETING_ACCOUNT_ACK,this);
 //   m_pIEventMgr->UnregisterEvent(EVENT_DELETEMEETING,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_CHANGEMEETINGMEMBER,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_CHANGE_MEETINGLEVEL,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_REGETMEETINGINFO,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_RECEIVEMEETING,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_MEETINGREAD,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_MEETING_GETVIDEOURL,this);
	//m_pIEventMgr->UnregisterEvent(EVENT_GET_MEETINGROOM_LIST,this);

	m_bInited = FALSE;
	_StopThread();

	if ( m_sLoginAck.uUserId )
	{
		C_eIMContactsMgr cmgr;
		cmgr.SetTimestamp(&m_sLastUpdateTTs, m_sLoginAck.uUserId);
	}
	EIMLOGGER_DEBUG_(m_pILogger, _T("ClientAgent UnInit start"));
	m_pIClientAgent->UnInit(1); 
	EIMLOGGER_DEBUG_(m_pILogger, _T("ClientAgent UnInit end"));
	m_mapAttrStr.clear();
//Del 	m_pIBIFile->DelAllTransTask();	//Single line deleted by LongWQ on 2014/11/19 19:57:48
	SAFE_RELEASE_INTERFACE_(m_pIClientAgent);
	SAFE_RELEASE_INTERFACE_(m_pIContacts);
	SAFE_RELEASE_INTERFACE_(m_pIP2P);
	SAFE_RELEASE_INTERFACE_(m_pIHistory);
	SAFE_RELEASE_INTERFACE_(m_pIPinyin);
	SAFE_RELEASE_INTERFACE_(m_pIMsgMgr);
	SAFE_RELEASE_INTERFACE_(m_pIBIFile);
	SAFE_RELEASE_INTERFACE_(m_pIEventMgr);

	C_eIMStateSubscribeMgr::GetObject().Uninit();
    C_eIMUrlFileMgr::GetObject().Uninit();
	m_pUIWaitOverMgr->UnInit();

	SAFE_RELEASE_INTERFACE_(m_pSubscribeMgr);
	SAFE_RELEASE_INTERFACE_(m_pUrlFileMgr);

	C_eIMContacts::GetObject().SetEventMgr( NULL );
	C_XmlProtocol::GetObject().Close();
	C_XmlEmoticon::GetObject().Close();
	
	I_SQLite3* pIDb = NULL;
	if (SUCCEEDED(QueryInterface(INAME_SQLITE_DB, (void**)&pIDb)))
	{
		AUTO_RELEASE_(pIDb);
		pIDb->Shutdown();		// Release SQLite3 DLL
	}

	EIMLOGGER_DEBUG_(m_pILogger, _T("Exit engine"));
	Sleep(250);
	SAFE_RELEASE_INTERFACE_(m_pILogger);
}

BOOL C_eIMEngine::GetPluginInfo(	// Get plugin information, succeed if return TRUE
	PS_PluginDllInfo	pPluginInfo	// Return plugin information
	)
{
	CHECK_NULL_RET_(pPluginInfo, FALSE);
	if( pPluginInfo->dwSize == sizeof(S_PluginDllInfo))
	{
		memset(pPluginInfo, 0, sizeof(S_PluginDllInfo));
		pPluginInfo->ePluginClass = ePLUGIN_CLASS_NONE;
		pPluginInfo->ePluginType  = ePLUGIN_TYPE_PLUGIN;
		pPluginInfo->i32Icon	  = -1;

		_tcsncpy( pPluginInfo->szPluginDesc, INAME_EIMENGINE, COUNT_OF_ARRAY_( pPluginInfo->szPluginDesc) -1);
		_tcsncpy( pPluginInfo->szPluginIID,  INAME_EIMENGINE, COUNT_OF_ARRAY_( pPluginInfo->szPluginIID ) -1);
		_tcsncpy( pPluginInfo->szPluginName, INAME_EIMENGINE, COUNT_OF_ARRAY_( pPluginInfo->szPluginName) -1);

		return TRUE;
	}

	return TRUE;
}

void C_eIMEngine::OnEvent(			// Event response function
	unsigned int	u32Event, 		// Event ID
	void*			lpvParam		// Event parameter
	)
{
	switch( u32Event )
	{
	case eCMD_GET_DATA_LIST_TYPE_ACK:
		OnGetDataListTypeAck((GETDATALISTTYPEACK*)lpvParam);
		break;
	case EVENT_RECV_OFFLINE_MSG:
		SetAttributeInt(IME_ATTRIB_OFFMSG_FINISHED, 1);
		break;
	case EVENT_ENGINE_FILE_TRANS:
		OnFileTransStatNotify((PS_FileTransNotify)lpvParam);
		break;
	case EVENT_RELOAD_CONTACTS:
		OnReloadContacts((int)lpvParam);
		break;
	case EVENT_GET_GROUP_INFO:
		OnGetGroupInfo(*(QSID*)lpvParam);
		break;
	case EVENT_GET_VGROUP_INFO:
		DoGetVGroupInfo(*(DWORD*)lpvParam);
		break;
	case EVENT_LOGIN_STATUS:
		OnLoginStatus((PS_LoginStatus)lpvParam);
		break;
	case EVENT_SEND_CMD:
		OnSendCmd((I_EIMCmd*)lpvParam);
		break;
	case EVENT_CLEAR_EMPS_AFTER:
		OnClearEmpsAfter();
		break;
	case EVENT_SEND_CMD_BEFORE:
		break;
	case EVENT_SEND_CMD_AFTER:
		break;
	case EVENT_RECV_CMD_BEFORE:
		break;
	case EVENT_RECV_CMD_AFTER:
		break;
	case EVENT_GET_DEPT_INFO_END:
		OnGetDeptListEnd((int)lpvParam);
		break;
	case EVENT_GET_DEPT_EMPS_END:
		OnGetDeptEmpsEnd((int)lpvParam);
		break;
	case EVENT_GET_EMPS_INFO_END:
		OnGetEmpsEnd((int)lpvParam, GetAttributeInt(IME_ATTRIB_FORCE_RELOAD2EMPS));
		break;
	case EVENT_GET_EMPSAVATAR_LIST_END:
		OnGetUserIconListEnd((int)lpvParam);
		break;
	case EVENT_LOAD_CONTS_END:
		OnLoadContactEnd();
		break;
	case EVENT_FILE_TRANSFER:
		OnFileTransfer((S_FileTransInfo*)lpvParam);
		break;
	case EVENT_FILE_TRANSFER_ACTION:
		OnFileTransferAction((PS_FileTransInfo)lpvParam);
		break;
	case eCMD_LOGIN_ACK:
		OnLoginAck((LOGINACK*)lpvParam);
		break;
	case eCMD_LOGOUT_ACK:
		OnLogoutAck((LOGOUTACK*)lpvParam);
		break;
	//case eCMD_MODIFY_INFO_NOTICE:
	//	OnModifyInfoNotice((MODIINFONOTICE*)lpvParam);
	//	break;
	case eCMD_GET_EMPLOYEE_INFO_ACK:
		OnGetEmployeeAck(PS_EmpInfo(lpvParam));
		break;
	case eCMD_CREATE_GROUP_ACK:
		OnCreateGroupAck((CREATEGROUPACK*)lpvParam);
		break;
	case eCMD_BROADCAST_NOTICE:
		OnBroadcastNotice((BROADCASTNOTICE*)lpvParam);
		break;
	case eCMD_MSG_READ:
		OnMsgRead((MSGREAD*)lpvParam);
		break;
	case eCMD_MSG_READ_NOTICE:
		OnMsgReadNotice((BROADCASTNOTICE*)lpvParam);
		break;
	case eCMD_MODIFY_SELF_INFO_NOTICE:
		OnCmdModifySelfInfoNotice((RESETSELFINFONOTICE*)lpvParam);
		break;
	case eCMD_GET_USER_STATE_ACK:
		OnGetUserStateAck((GETUSERSTATELISTACK*)lpvParam);
		break;
	case eCMD_ROAMINGDATASYNACK:
		OnSynRoamDataAck((ROAMDATASYNCACK*)lpvParam);
		break;
	case eCMD_ROBOTSYNCRSP:
		OnGetRobotInfoRsp();
		break;
	case eCMD_COMP_LAST_TIME_NOTICE:
		OnCoLastTimeNotice((COMPLASTTIMENOTICE*)lpvParam);
		break;
	case eCMD_CHECK_TIME_RSP:
		OnCheckTimeRsp((CHECK_TIME_RESP*)lpvParam);
		break;
	case eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE:
		{
			m_wRegularGroupCount += 1;
			if((int)m_wRegularGroupCount >= GetAttributeInt(IME_ATTRIB_OFF_GROUP_COUNT))
			{
				if(m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime < m_sLoginAck.tTimeStamp.dwRegularGroupUpdateTime)
				{
					EIMLOGGER_DEBUG_(m_pILogger, _T("Get regular group finished"));
					m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime = m_sLoginAck.tTimeStamp.dwRegularGroupUpdateTime;	// Update the timestamps
					UpdateTimsstamp();
					C_eIMSaveData* pSaveData = GetEIMSaveDataPtr();
					if(pSaveData)
					{
						pSaveData->ClearGroupIDUpdatingData();
					}
				}
			}
		}
		break;
	case EVENT_UI_WAIT_OVERTIME:
		{
			const TCHAR * pszMsg = ((I_EIMCmd*)lpvParam)->GetXml();

			if(NULL == pszMsg || pszMsg[0] == _T('\0') )
			{
				TRACE_(_T("Empty"));
				return;
			}

			eIMStringA strMsgA;
			TiXmlDocument xml;
			::eIMTChar2UTF8(pszMsg, strMsgA);
			xml.Parse(strMsgA.c_str());
			TiXmlElement* xmlCmd = xml.RootElement();
			CHECK_NULL_RETV_(xmlCmd);

			QMID qmid = 0;
			if (xmlCmd->QueryValueAttribute(FIELD_MSG_ID,&qmid))
			{
				TRACE_(_T("Get QSID failed"));
				return;
			}
            int i32Value = 0;
            xmlCmd->QueryValueAttribute(FIELD_TYPE,&i32Value);
            if(eIM_MSGTYPE_MSG_RECALL == i32Value)
            {
                ::MessageBox(NULL,_T("撤回消息超时!"),_T(" 撤回应答"),0);
            }
            else
            {
                xmlCmd->QueryValueAttribute(FILED_ROBOT_IS_TOPIC,&i32Value);
                if(1 == i32Value)
                {
                    break;
                }
			    _AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_ADD_OVERTIME_SENDMSG, &qmid);
            }
		}
		break;
    case EVENT_URL_FILE_TRANSFER_ACTION:
		OnUrlFileTransferAction((T_UrlFileTranInfo*)lpvParam);
		break;
	//case EVENT_CERATE_MEETING:
	//	OnCreateMeeting((S_Meeting_create*)lpvParam);
	//	break;
	//case EVENT_GET_LAUNCHSTRING:
	//	GetLaunchString((S_GetLaunchString*)lpvParam);
	//	break;
	//case EVENT_CHANGEMEETINGMEMBER:
	//	OnChangeMeetingMember((S_MeetingMemberChange*)lpvParam);
	//	break;
	//case EVENT_REGETMEETINGINFO:
	//	OnReGetMeetingInfo((S_ReGetMeetingInfo*)lpvParam);
	//	break;
	//case EVENT_CHANGE_MEETINGLEVEL:
	//	OnChangeMeetingLevel((S_MeetingLevel*)lpvParam);
	//	break;
	//case EVENT_RECEIVEMEETING:
	//	OnReceiveMeeting((S_ReceiveMeeting*)lpvParam);
	//	break;
	//case EVENT_MEETINGREAD:
	//	OnMeetingRead((S_MeetingRead*)lpvParam);
	//	break;
	//case EVENT_DELETEMEETING:
	//	OnDeleteMeeting((S_MeetingCancel*)lpvParam);
	//	break;
 //   case EVENT_MEETING_ACCOUNT_ACK:
 //       OnSaveMeetingAccountInfo((confUserInfoNotice*)lpvParam);
 //       break;
	//case EVENT_MEETING_GETVIDEOURL:
	//	OnGetMeetingVideoUrl((S_MeetingGetVideo*)lpvParam);
	//	break;
	//case EVENT_GET_MEETINGROOM_LIST:
	//	OnGetMeetingRoomlist((S_Meeting_GetRoomlist*)lpvParam);
	//	break;
	case eCMD_DEPTSHOWCONFIG_ACK:
		OnGetDeptShowAck((GETDEPTSHOWCONFIGACK*)lpvParam);
        break;
	default:
		EIMLOGGER_WARN_(m_pILogger, _T("Not dealwith event: 0x%08X"), u32Event);
		break;
	}
}

void C_eIMEngine::_UpdateRoamData()
{
	EIMLOGGER_INFO_(m_pILogger, _T("SynRoamData!!!"));
	ROAMDATASYNC rSyn;
	rSyn.dwUserid = m_sLoginAck.uUserId;
	rSyn.dwCompid = m_sLoginAck.dwCompID;
	rSyn.cTerminalType = TERMINAL_PC;
	rSyn.cRequestType = TYPE_COMCONTACT;
	// 同步常用联系人
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalCommonContactUpdateTime
		|| m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime == 0
		|| m_sLoginAck.tTimeStamp.dwPersonalCommonContactUpdateTime == 0)
	{
		rSyn.dwUpdatetime = 0;//m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime;
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_ROAMINGDATASYN, &rSyn);
	}
	else
	{
		m_iSynRoamDataStep++;
	}

	// 默认常用联系人
	rSyn.cRequestType = TYPE_DEFAULT_COMCONTACT;
#if 1
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime < m_sLoginAck.tTimeStamp.dwGlobalCommonContactUpdateTime
		|| m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime == 0 
		|| m_sLoginAck.tTimeStamp.dwGlobalCommonContactUpdateTime == 0)	// fix:南航版本，服务器改错时间了为0xfffffff,导致再也无法更新，
	{
		rSyn.dwUpdatetime = m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime;
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_ROAMINGDATASYN, &rSyn);
	}
	else
	{
		m_iSynRoamDataStep++;
	}
#else
	rSyn.dwUpdatetime = 0;// 默认常用联系人每次直接拉取
	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_ROAMINGDATASYN, &rSyn);
#endif

	// 同步常用部门
	rSyn.cRequestType = TYPE_COMDEPT;
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalCommonDeptUpdateTime
		|| m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime == 0 
		|| m_sLoginAck.tTimeStamp.dwPersonalCommonDeptUpdateTime == 0)
	{
		rSyn.dwUpdatetime = 0;//m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime;
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_ROAMINGDATASYN, &rSyn);
	}
	else
	{
		m_iSynRoamDataStep++;
	}

	// 同步关注联系人
	rSyn.cRequestType = TYPE_ATTEN_EMP;
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAttentionUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalAttentionUpdateTime
		|| m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime == 0
		|| m_sLoginAck.tTimeStamp.dwPersonalAttentionUpdateTime == 0)
	{
		rSyn.dwUpdatetime = 0;//m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAttentionUpdateTime;
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_ROAMINGDATASYN, &rSyn);
	}
	else
	{
		m_iSynRoamDataStep++;
	}

	//// 同步机器人信息
	//if(m_sLastUpdateTTs.dwRobotInfoUpdatetime < m_sLoginAck.dwRobotInfoUpdatetime
	//	|| m_sLastUpdateTTs.dwRobotInfoUpdatetime == 0
	//	|| m_sLoginAck.dwRobotInfoUpdatetime == 0)
	//{
	//	SetAttributeInt(IME_ATTRIB_GET_ROBOT_PAGE_COUNT, 0);	// 同步前页面计数清0
	//	ROBOTSYNCREQ rRobotReq;
	//	rRobotReq.dwCompID = m_sLoginAck.dwCompID;
	//	rRobotReq.cTerminal = TERMINAL_PC;
	//	rRobotReq.dwUserID = m_sLoginAck.uUserId;
	//	rRobotReq.dwTimestamp = m_sLastUpdateTTs.dwRobotInfoUpdatetime;
	//	rRobotReq.cReqType = 0;
	//	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_ROBOTSYNCREQ, &rRobotReq);
	//	EIMLOGGER_DEBUG_(m_pILogger, _T("Start get robot info"));
	//}
	//else
	//{
	//	EIMLOGGER_DEBUG_(m_pILogger, _T("no need update robot info.  timestamp:%u  acktimestamp:%u"), 
	//		m_sLastUpdateTTs.dwRobotInfoUpdatetime, m_sLoginAck.dwRobotInfoUpdatetime);
	//	m_iSynRoamDataStep++;
	//}

	if(m_iSynRoamDataStep >= 5)
	{
		SAFE_SEND_EVENT_(m_pIEventMgr,  EVENT_FRESH_CONTACT_TREE, (void*)0);
	}
}

void C_eIMEngine::OnClearEmpsAfter()
{
	PS_EmpInfo_ psEmpInfo = m_pIContacts->GetEmp(m_sLoginAck.uUserId);
	if ( psEmpInfo )
	{
		//memcpy(m_sLoginAck.sEmp.qdidPid, psEmpInfo->qdidPid, sizeof(m_sLoginAck.sEmp.qdidPid));
	}
}

void C_eIMEngine::OnReloadContacts(int i32Type)
{
	EIMLOGGER_INFO_(m_pILogger, _T("Reload contacts type:%u)"), i32Type);
	C_eIMContacts*  pConts  = static_cast<C_eIMContacts*>(m_pIContacts);
	CHECK_NULL_RETV_(pConts);

	switch(i32Type)
	{
	case IME_UPDATE_CONTS_TYPE_LOCAL:		
		{
			C_eIMContactsMgr cmgr;
			cmgr.SetReloadContactsInfo((DWORD)time(NULL), i32Type, 0, 0);
			pConts->SetLoadEmpTimestamp(0);
			pConts->SetLoadContactTimestamp(0, TRUE);
			pConts->SetLoadContactTimestamp(0, FALSE);

			return _LoadContact();
		}
	case IME_UPDATE_CONTS_TYPE_NEXT_LOGIN:	// Already update the information by cmd
		break;	
	case IME_UPDATE_CONTS_TYPE_SERVER_UI:
		SetAttributeInt(IME_UPDATE_CONTACTS_TYPE, 0);
		SetAttributeInt(IME_GET_DATA_LIST_TYPE, 0);
		SetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO, 1);
		SetAttributeInt(IME_RELOAD_CONTACTS, 0);
	case IME_UPDATE_CONTS_TYPE_SERVER:
		if (!(m_sUpdateContsInfo.dwType == IME_UPDATE_CONTS_TYPE_NOW || m_sUpdateContsInfo.dwTime || GetTickCount() - m_sUpdateContsInfo.dwTime > 10 * 60 * 1000))
		{
			EIMLOGGER_INFO_(m_pILogger, _T("Skip this time user refreash contacts, m_sUpdateContsInfo.dwTime:%u, timediff:%u(need greater than 10 minute)"), GetTickCount() - m_sUpdateContsInfo.dwTime > 10 * 60 * 1000);
			break;
		}
	case IME_UPDATE_CONTS_TYPE_NOW:
		{
			SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_CLEAR_DEPTS_BEFORE, NULL );
			m_sLastUpdateTTs.dwTTShowDeptConfig = 0;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime = 0;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime = 0;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = 0;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime = 0;

			pConts->SetLoadEmpTimestamp(0);
			pConts->SetLoadContactTimestamp(0, TRUE);
			pConts->SetLoadContactTimestamp(0, FALSE);

			m_sUpdateContsInfo.dwId		= (DWORD)time(NULL);
			m_sUpdateContsInfo.dwType	= i32Type;
			m_sUpdateContsInfo.dwElapse =(i32Type == IME_UPDATE_CONTS_TYPE_NOW ? m_sUpdateContsInfo.dwElapse : 0);
			m_sUpdateContsInfo.dwFlag	= IME_UPDATE_CONTS_FLAG_FINISH;
			m_sUpdateContsInfo.dwTime	= GetTickCount();

			C_eIMContactsMgr cmgr;
			cmgr.Clear();
			cmgr.SetReloadContactsInfo(m_sUpdateContsInfo.dwId, m_sUpdateContsInfo.dwType, m_sUpdateContsInfo.dwElapse, m_sUpdateContsInfo.dwFlag);
			
			_UpdateDepts(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime);
			break;
		}
	default:
		EIMLOGGER_INFO_(m_pILogger, _T("Unknown type:%u"), i32Type);
		break;
	}
}

//void C_eIMEngine::OnSyncId()
//{
//	DWORD tmSeq = GetTimeNow() - TIME_BASE_SECOND;
//	hpi.GetSessionId(0, tmSeq);
//	hpi.GetMsgId(0, tmSeq);
//
//	C_eIMMsgMgr mmgr;
//	S_IMID qmid = { 0 };
//	
//	qmid.qmid = mmgr.GetLastId();
//	if ( qmid.qmid )
//	{
//		EIMLOGGER_INFO_(m_pILogger, _T("Update SeqID to %u of QSID, QMID"), qmid.Seq);
//		hpi.GetSessionId(0, qmid.Seq);
//		hpi.GetMsgId(0, qmid.Seq);
//	}
//}

void C_eIMEngine::OnGetGroupInfo(QSID qsid)
{
	GETGROUPINFO sGetGroupInfo = { 0 };
	GET_QSID_ASTR_(qsid, sGetGroupInfo.aszGroupID);
	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_GROUP_INFO, &sGetGroupInfo);
}

void C_eIMEngine::DoGetVGroupInfo(DWORD dwTime)
{
	REGULAR_GROUP_UPDATE_REQ sReq;
	sReq.dwUserID = m_sLoginAck.uUserId;
	sReq.dwRegularTime  = m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime;
	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_REGULAR_GROUP_UPDATE_REQ, &sReq);
}

void C_eIMEngine::OnLoginStatus(PS_LoginStatus psStatus)
{
	CHECK_NULL_RETV_(psStatus);
	BOOL bClearData = FALSE;
	switch(psStatus->dwStatus)
	{
	case EIMERR_STATUS_ONLINE:
		EIMLOGGER_INFO_(m_pILogger, _T("Online"));
		UpdateLoginState(eENGINE_STATUS_ONLINE);
		break;
	case EIMERR_STATUS_ONLEAVE:
		EIMLOGGER_INFO_(m_pILogger, _T("Onleave"));
		UpdateLoginState(eENGINE_STATUS_LEAVE);
		break;
	case EIMERR_KICK:
		EIMLOGGER_INFO_(m_pILogger, _T("Kicked"));
		UpdateLoginState(eENGINE_STATUS_KICK);
		m_pIClientAgent->Disconnect();
		bClearData = TRUE;
		break;
	case EIMERR_FORBIDDEN:
		EIMLOGGER_INFO_(m_pILogger, _T("Forbidden"));
		UpdateLoginState(eENGINE_STATUS_FORBIDDEN);
		m_pIClientAgent->Disconnect();
		bClearData = TRUE;
		break;
	case EIMERR_STATUS_OFFLINE:
		EIMLOGGER_INFO_(m_pILogger, _T("Offline"));
		UpdateLoginState(eENGINE_STATUS_OFFLINE);
		m_pIClientAgent->Disconnect();
		bClearData = TRUE;
		break;
	case EIMERR_LOGIN_OVERTIME:
		EIMLOGGER_INFO_(m_pILogger, _T("Login overtime"));
		UpdateLoginState(eENGINE_STATUS_OFFLINE);
		m_pIClientAgent->Disconnect();
		bClearData = TRUE;
		break;
	default:
		if (FAILED(psStatus->dwStatus))
		{
			EIMLOGGER_ERROR_(m_pILogger, _T("Login failed, ret=0x%08X"), psStatus->dwStatus);
			UpdateLoginState(eENGINE_STATUS_OFFLINE);
			m_pIClientAgent->Disconnect();
			bClearData = TRUE;
		}
		break;
	}

	if(bClearData)
	{
		C_eIMSaveData* pSaveData = GetEIMSaveDataPtr();
		if(pSaveData)
		{
			pSaveData->ClearGroupIDReq();
			pSaveData->ClearGroupIDUpdatingData();
		}
	}
}

void C_eIMEngine::OnSendCmd(I_EIMCmd* pICmd)
{
	CHECK_NULL_RETV_(pICmd);
	DWORD dwFlag = pICmd->GetFlag();
	if (dwFlag & CMD_FLAG_THREAD_SEND)
		_AddCmd(eTHREAD_FLAG_SEND_IDX, pICmd, TRUE);
	else if (dwFlag & CMD_FLAG_THREAD_DB)
		_AddCmd(eTHREAD_FLAG_DB_IDX, pICmd, TRUE);
	else if (dwFlag & CMD_FLAG_THREAD_UI)
		_AddCmd(eTHREAD_FLAG_DISP_IDX, pICmd, TRUE);
	else
		ASSERT_(FALSE);
}

inline void C_eIMEngine::_InitUserEnv()
{
	//TCHAR* pszPath[] = 
	//{
	//	PATH_TOKEN_LOGINID_FILE_(PATH_TYPE_EMOTICON) _T("\\"),
	//	PATH_TOKEN_LOGINID_FILE_(PATH_TYPE_SKIN) _T("\\"),
	//	PATH_TOKEN_LOGINID_FILE_(PATH_TYPE_FACE) _T("\\"),
	//};

	//eIMString szPath;
	//for( int i32Index = 0; i32Index < COUNT_OF_ARRAY_(pszPath); i32Index++)
	//{
	//	szPath = pszPath[i32Index];
	//	::eIMReplaceToken(szPath);
	//	::eIMMakeDir(szPath.c_str());
	//}
}


BOOL C_eIMEngine::InitEnv(LOGINACK* pLoginAck)
{
	TCHAR szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
	SetEnvironmentVariable(IME_ATTRIB_USER_ID, IntToStr(pLoginAck->uUserId, szBuf, FALSE, FALSE));	// Set UID as environment variable
	SetEnvironmentVariable(IME_ATTRIB_CO_ID,   IntToStr(pLoginAck->dwCompID, szBuf, FALSE, FALSE));	// Set COID as environment variable

	SetAttributeInt(IME_ATTRIB_USER_ID, pLoginAck->uUserId);
	SetAttributeInt(IME_ATTRIB_CO_ID,   pLoginAck->dwCompID);

	// 服务器时间
	CHECK_TIME_RESP sRsp = { 0 };
	sRsp.dwSerial = GetTickCount();
	sRsp.timeNow  = pLoginAck->tTimeStamp.nServerCurrentTime;		// Set the server times
	OnCheckTimeRsp(&sRsp);

	m_sLoginEmpInfo.Id     = pLoginAck->uUserId;
	m_sLoginEmpInfo.dwCoId = pLoginAck->dwCompID; 

	_InitUserEnv();
	_InstallConcatsDb(pLoginAck->uUserId, pLoginAck->aszContactPath, "wd150119yfgz");
	return _IsInit();
}

void C_eIMEngine::OnLoginAck(LOGINACK* pLoginAck)
{
	m_iSynRoamDataStep = 0;
	m_bOnlineHasSynInfo = FALSE;
	S_LoginStatus sLoginStatus = { 0 };
	if ( pLoginAck == NULL || pLoginAck->ret != RESULT_SUCCESS || pLoginAck->uUserId == 0)
	{
		if (pLoginAck)
			EIMLOGGER_ERROR_(m_pILogger, _T("Invalid parameter, qeid:%u, result:%u, ErrDesc: %s"), pLoginAck->uUserId, pLoginAck->ret, hpi.GetErrorLangInfo(pLoginAck->ret, NULL));
		
		m_bIsUserOffline = TRUE;
		sLoginStatus.dwStatus = EIMERR_STATUS_OFFLINE;
		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
		return;
	}

	if (!_IsInit()) 
		return;

	m_bIsUserOffline = FALSE;
	m_iCheckDelay = 0;
	m_dwLoginTimes++;


	_InitDb(FALSE, pLoginAck->uUserId);
	// Check and backup 
	C_CmdBackupDatabase cmdBkup(0);
	cmdBkup.DoCmd(m_pILogger, NULL);


	_CreateThread( CREATE_THREAD_FLAG_DB );	// Start DbThread after login succeeded

	C_eIMLoginUserMgr umgr;
	if (umgr.Init())
	{
		DWORD dwFlag = GetAttributeInt(IME_ATTRIB_LOGIN_TYPE) & LOGIN_USER_FLAG_LOGIN_TYPE_MASK;
		dwFlag <<=16;	// Login type
		dwFlag  |= GetAttributeInt(IME_ATTRIB_REMEMBER_PSW) ? LOGIN_USER_FLAG_REMEMBER_PSW : 0;
		dwFlag  |= GetAttributeInt(IME_ATTRIB_AUTO_LOGIN) ? LOGIN_USER_FLAG_AUTO_LOGIN : 0;

		eIMString szName = GetAttributeStr(IME_ATTRIB_USER_NAME); 
		eIMString szPsw  = GetAttributeStr(IME_ATTRIB_USER_PSW);
		if (szPsw == LOGIN_USER_DUMMY_PSW)
			umgr.Get(szName.c_str(), szPsw);

		umgr.Set(pLoginAck->uUserId, szName.c_str(), szPsw.c_str(), dwFlag);
	}

	C_eIMContactsMgr cmgr;
	if (cmgr.Init())
	{
		cmgr.GetTimestamp(&m_sLastUpdateTTs, pLoginAck->uUserId);
		if ( FAILED(cmgr.LoadEmpInfo(pLoginAck->uUserId, m_sLoginEmpInfo)))	// Load from db
		{
			eIMString szValue;
			m_sLoginEmpInfo.pszCode = GetAttributeStr( IME_ATTRIB_USER_NAME );
			m_sLoginEmpInfo.bitSex  = pLoginAck->sex;

			eIMUTF8ToTChar(pLoginAck->tCnUserName.value, szValue);
			m_sLoginEmpInfo.pszName = eIMAddString_(szValue.c_str());
			if ( pLoginAck->tEnUserName.len > 0)
			{
				eIMUTF8ToTChar(pLoginAck->tEnUserName.value, szValue);
				m_sLoginEmpInfo.pszEnName = eIMAddString_(szValue.c_str());
			}
		}

		if ( hpi.UIGetAttributeInt(PATH_FUNC, FUNC_ATTR_CLEAR_LOGO_TIME) )
		{
			hpi.UISetAttributeInt(PATH_FUNC, FUNC_ATTR_CLEAR_LOGO_TIME, 0);
			m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime = 0;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAvatarUpdateTime = 0;
			cmgr.Clear(CLEAR_FLAG_LOGO_TIME);
		}	
		
		// 检查启动服务器的通讯录更新命令（上次登录未完成）
		S_UpdateContsInfo	sInfo;
		if ( cmgr.GetReloadContactsInfo(&sInfo) && sInfo.dwFlag == IME_UPDATE_CONTS_FLAG_NOT_START)
		{
			if ( sInfo.dwType == IME_UPDATE_CONTS_TYPE_NEXT_LOGIN )
			{
				SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_RELOAD_CONTACTS, (void*)IME_UPDATE_CONTS_TYPE_NOW);	// switch type NOW
			}
			else
			{	// Start update contacts after IME_RESTART_UPDATE_DELAY
				CONTACTSUPDATENOTICE  sUpdate = { 0 };
				sUpdate.cTerminalType = TERMINAL_PC;
				sUpdate.cUpdateType   = IME_UPDATE_CONTS_TYPE_NOW;
				sUpdate.dwCount		  = sInfo.dwElapse;
				sUpdate.dwTimeStampe  = sInfo.dwId;
				sUpdate.dwUserID	  = pLoginAck->uUserId;

				I_EIMCmd* pICmd = CmdCreateInstance<I_EIMCmd*>(eCMD_CONTACTS_UPDATE_NOTICE, &sUpdate );	ASSERT_(pICmd);
				C_ContactUpdateWaitOverItem* pWaitItem = new C_ContactUpdateWaitOverItem(IME_RESTART_UPDATE_DELAY);		ASSERT_(pWaitItem);
				if ( pICmd && pWaitItem )
				{
					pICmd->SetBusinessId(eCMD_CONTACTS_UPDATE_NOTICE);
					pWaitItem->SetData(pICmd);
					pWaitItem->SetStartTime(GetTickCount());
					C_UIWaitOverMgr::GetObject().AddWaitOverItem(pWaitItem);
				}
			}
		}
	}

	if ( GetAttributeInt(IME_FAST_LOAD_CONTACTS) &&
		 m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime &&  m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime &&  m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime )
	{	// 非首次登录时， 先加载通讯录，使用户可以先正常使用。
		_LoadContact();
	}

	ZERO_STRUCT_(m_sLoginAck);
	memcpy(&m_sLoginAck,pLoginAck,sizeof(LOGINACK));
	SetAttributeInt(IME_FILE_TRAN_NEW_MODEL, GetPurview(FILE_TRANS_MODE_RESUME));

	// 获取企业信息
	if ( m_sLoginAck.tTimeStamp.dwCompUpdateTime > m_sLastUpdateTTs.rUpdateTimeInfo.dwCompUpdateTime 
		|| m_sLastUpdateTTs.rUpdateTimeInfo.dwCompUpdateTime == 0 )
	{	// Get company information
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_CO_INFO, NULL);
	}


	if ( GetAttributeInt(IME_ATTRIB_LOGIN_TYPE) == IME_LOGIN_TYPE_ONLIVE)
	{
		m_eEngineStatus = eENGINE_STATUS_ONLINE;
		Logout(2, 0);	// Onlive
	}
	else
	{
		m_bIsUserOffline = FALSE;
		sLoginStatus.dwStatus = EIMERR_STATUS_ONLINE;
		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
	}

	// 更新个人信息
	_UpdatePersonalInfo(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalInfoUpdateTime);

	// 更新组织架构信息
	GETDATALISTTYPEPARAMETET sGetDataList = { 0 };
	sGetDataList.cUpdataTypeDept		  = 1;
	sGetDataList.cUpdataTypeUser		  = 1;
	sGetDataList.cUpdataTypeDeptUser	  = 1;
	sGetDataList.dwLastUpdateTimeDept	  = m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime;
	sGetDataList.dwLastUpdateTimeUser	  = m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime;
	sGetDataList.dwLastUpdateTimeDeptUser = m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime;
	// 如果3个时音戳都小于本地时间戳 则不获取组织架构的下载类型，直接使用数据包方式
	if(m_sLoginAck.tTimeStamp.dwDeptUpdateTime < m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime
		&& m_sLoginAck.tTimeStamp.dwUserUpdateTime < m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime
		&& m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime < m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime)
	{
		_UpdateDepts(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime);
	}
	else
	{
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_DATA_LIST_TYPE, (void*)&sGetDataList);
	}
}

void C_eIMEngine::OnLogoutAck(LOGOUTACK* lpvParam)
{	
}

BOOL C_eIMEngine::Emp2EmpInfo(const EMPLOYEE* psEmp, PS_EmpInfo psEmpInfo, QDID qdid)
{
	CHECK_NULL_RET_(psEmp, FALSE);
	CHECK_NULL_RET_(psEmpInfo, FALSE);

//	eIMStringA	   szPinyinA;
//	eIMStringA	   szSearchPyA;
	eIMString	   szValue;
//	m_pIPinyin->GetUtf8Pinyins(psEmp->tUserInfo.aszCnUserName,  szPinyinA, szSearchPyA);

	psEmpInfo->dwSize			= sizeof(S_EmpInfo);
	psEmpInfo->Id				= psEmp->tUserInfo.dwUserID;
	psEmpInfo->bitSex			= IME_GET_SEX_(psEmp->tUserInfo.cSex);
	psEmpInfo->bitLoginType		=(psEmp->tUserExtend.cLoginType & IME_EMP_TERM_MASK);
	psEmpInfo->bitMsgSyncType	=(psEmp->tUserExtend.cMsgsynType & IME_EMP_SYNC_MASK);
	psEmpInfo->bitPurview		=(psEmp->tUserExtend.wPurview & IMP_EMP_PURVIEW_MASK);
	psEmpInfo->bitStatus		=(psEmp->tUserExtend.cStatus & IME_EMP_STATUS_MASK);
	psEmpInfo->dwBirthday		= psEmp->tUserExtend.dwBirth;
	psEmpInfo->dwCoId			= psEmp->tUserExtend.dwCompID;
	psEmpInfo->dwHiredate		= psEmp->tUserInfo.dwHiredate;
//	psEmpInfo->bitRef			= 1;	// Auto calc

	psEmpInfo->qdidPid[0]		= qdid;
	psEmpInfo->bitIntegrity		= IME_EMP_INTEGRITY_FULL;
	psEmpInfo->pszNamePinyin	= NULL; // (char*)szPinyinA.c_str();			// DONT care the buffer
	psEmpInfo->pszNamePinyinS   = NULL;	// (char*)szSearchPyA.c_str();	// DONT care the buffer

	psEmpInfo->pszName		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszCnUserName, szValue), szValue.c_str()));
	psEmpInfo->pszEnName	= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszEnUserName, szValue), szValue.c_str()));

	psEmpInfo->pszDuty		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszPost, szValue), szValue.c_str()));
	psEmpInfo->pszSign		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserExtend.aszSign, szValue), szValue.c_str()));
	psEmpInfo->pszCode		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszUserCode, szValue), szValue.c_str()));
	psEmpInfo->pszTel		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszTel, szValue), szValue.c_str()));
	psEmpInfo->pszPhone		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszPhone, szValue), szValue.c_str()));
	psEmpInfo->pszHomeTel	= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserExtend.aszHomeTel, szValue), szValue.c_str()));
	psEmpInfo->pszEmrgTel	= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserExtend.aszEmergencyphone, szValue), szValue.c_str()));
	psEmpInfo->pszEmail		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszEmail, szValue), szValue.c_str()));

	psEmpInfo->pszAddress   = eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszAdrr, szValue), szValue.c_str()));
	psEmpInfo->pszPostalCode= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszPostcode, szValue), szValue.c_str()));
	psEmpInfo->pszFax		= eIMAddString_((::eIMUTF8ToTChar(psEmp->tUserInfo.aszFax, szValue), szValue.c_str()));

	C_eIMContactsMgr cmgr;
	cmgr.Set(psEmp);

	return FALSE;
}

void C_eIMEngine::OnCreateGroupAck(CREATEGROUPACK* lpsAck)
{
	CHECK_NULL_RETV_(lpsAck);
	
	if ( lpsAck->result == RESULT_GROUPEXIST )
	{	// Existed group, get it
		QSID qsid= GroupId2Qsid(lpsAck->aszGroupID);
		OnGetGroupInfo(qsid);
	}
}

void C_eIMEngine::OnBroadcastNotice(BROADCASTNOTICE* psNotice)
{
}

void C_eIMEngine::OnMsgRead(MSGREAD* psRead)
{

}

void C_eIMEngine::OnMsgReadNotice(BROADCASTNOTICE* psNotice)
{
}

void C_eIMEngine::OnCmdModifySelfInfoNotice(RESETSELFINFONOTICE* psNotice)
{
	CHECK_NULL_RETV_(psNotice);
	if ( psNotice->cModiType == 100 )
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_EMPLOYEE_INFO, &psNotice->dwUserID);
}

void C_eIMEngine::OnGetUserStateAck(GETUSERSTATELISTACK* psAck)
{
	if ( psAck == NULL || psAck->result != RESULT_SUCCESS )
		return;

	if ( psAck->wCurrPage == 0 )
	{	// Reach at the end pate of GetUserState
		EIMLOGGER_INFO_(m_pILogger, _T("Get state end!!!"));
	}
}

// 同步漫游数据应答
void C_eIMEngine::OnSynRoamDataAck(ROAMDATASYNCACK* psAck)
{
	EIMLOGGER_INFO_(m_pILogger, _T("SynRoamData ack save time!!!"));
	CHECK_NULL_RETV_(psAck);
	switch(psAck->cResponseType)
	{
	case TYPE_COMCONTACT:
		{
			if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalCommonContactUpdateTime)
			{
				m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime = m_sLoginAck.tTimeStamp.dwPersonalCommonContactUpdateTime;	
				UpdateTimsstamp();
			}
			m_iSynRoamDataStep++;
		}
		break;
	case TYPE_COMDEPT:
		{
			if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalCommonDeptUpdateTime)
			{
				m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime = m_sLoginAck.tTimeStamp.dwPersonalCommonDeptUpdateTime;	
				UpdateTimsstamp();
			}
			m_iSynRoamDataStep++;
		}
		break;
	case TYPE_ATTEN_EMP:
		{
			if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAttentionUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalAttentionUpdateTime)
			{
				m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAttentionUpdateTime = m_sLoginAck.tTimeStamp.dwPersonalAttentionUpdateTime;	
				UpdateTimsstamp();
			}
			m_iSynRoamDataStep++;
		}
	case TYPE_DEFAULT_COMCONTACT:
		{
			if(m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime < m_sLoginAck.tTimeStamp.dwGlobalCommonContactUpdateTime)
			{
				m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime = m_sLoginAck.tTimeStamp.dwGlobalCommonContactUpdateTime;	
				UpdateTimsstamp();
			}
			m_iSynRoamDataStep++;
		}
		break;
	default:
		break;
	}
	// need update ui
	if(m_iSynRoamDataStep >= 5)
	{
		SAFE_SEND_EVENT_(m_pIEventMgr,  EVENT_FRESH_CONTACT_TREE, (void*)0);
	}

}

// 同步机器人数据完成
void C_eIMEngine::OnGetRobotInfoRsp()
{
	// 更新时间戳 
	if(m_sLastUpdateTTs.dwRobotInfoUpdatetime < m_sLoginAck.dwRobotInfoUpdatetime)
	{
		m_sLastUpdateTTs.dwRobotInfoUpdatetime = m_sLoginAck.dwRobotInfoUpdatetime;	
		UpdateTimsstamp();
	}

	EIMLOGGER_DEBUG_(m_pILogger, _T("Get robot info finished"));
	C_eIMStateSubscribeMgr::GetObject().UpdateRobotStateOnline();
	m_iSynRoamDataStep++;
	if(m_iSynRoamDataStep >= 5)
	{
		SAFE_SEND_EVENT_(m_pIEventMgr,  EVENT_FRESH_CONTACT_TREE, (void*)0);
	}
}

void C_eIMEngine::OnGetDeptListEnd(int i32Result)
{
	if (FAILED(i32Result))
	{
		EIMLOGGER_WARN_(m_pILogger, _T("Result: 0x%08X"), i32Result);
		return;
	}
	EIMLOGGER_INFO_(m_pILogger, _T("Get dept list end!!!"));
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime < m_sLoginAck.tTimeStamp.dwDeptUpdateTime)
	{
		m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime = m_sLoginAck.tTimeStamp.dwDeptUpdateTime;	// Update the timestamps
		UpdateTimsstamp();
	}
    
    int RankId = hpi.UIGetAttributeInt(PATH_SETTING_BASIC, SETTING_BASIC_ATTR_RANKID);
    if (RankId )
    {
        std::wstringstream ss;
        ss<<m_sLoginAck.uUserId;
        eIMString strUserId = ss.str();
        eIMString strConfigName = _T("RandID_");
        strConfigName = strConfigName + strUserId;
        int rankidlevel    = hpi.UIGetAttributeInt(PATH_SETTING_RANKIDLEVEL,  strConfigName.data(), 0);
        if (m_sLoginAck.cMobileMaxSendFileSize != rankidlevel)
        {
            m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = 0;
            hpi.UISetAttributeInt(PATH_SETTING_RANKIDLEVEL, strConfigName.data(), m_sLoginAck.cMobileMaxSendFileSize);
			hpi.UISaveConfig();
        }
    }

	_UpdateDeptEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime);
}

void C_eIMEngine::OnGetDeptEmpsEnd(int i32Result)
{
	if (FAILED(i32Result))
	{
		EIMLOGGER_WARN_(m_pILogger, _T("Result: 0x%08X"), i32Result);
		return;
	}
	EIMLOGGER_INFO_(m_pILogger, _T("Get dept emps end!!!"));
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime < m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime)
	{
		m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime;	// Update the timestamps
		UpdateTimsstamp();
	}

	int i32VInfo = GetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO);
	if ( i32VInfo >= 0 && !GetAttributeInt(IME_DISABLE_DEPTUSER_VISABLE))
	{
		GETDEPTSHOWCONFIGREQ sConfigReq = { 0 };

		sConfigReq.dwUserID = m_sLoginAck.uUserId;
		sConfigReq.dwCompID = m_sLoginAck.dwCompID;
		sConfigReq.cTerminal= TERMINAL_PC;
		sConfigReq.dwTimestamps = i32VInfo ? 0 : m_sLastUpdateTTs.dwTTShowDeptConfig;

		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_DEPTSHOWCONFIG_REQ, &sConfigReq);

		return;	// Need wait to get dept user show config 
	}

	// 同步个人漫游数据
	_UpdateRoamData();

	// 加载刷新组织架构界面
	if ( m_sUpdateContsInfo.dwTime )
		_UpdateEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime);	
	else
		_LoadContact();
	// 更新人员信息
//Del 	_UpdateEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime);	//Single line deleted by lwq on 2014/10/15 16:39:49 延迟启动

	// 获取用户状态
	//_GetEmpState();//延迟获取

}

void C_eIMEngine::OnGetEmpsEnd(int i32Result, BOOL bForceReload)
{
	if (FAILED(i32Result))
	{
		EIMLOGGER_WARN_(m_pILogger, _T("Result: 0x%08X"), i32Result);
		return;
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Get emps end!!!"));
	
	if ( bForceReload )
	{	// To troggle reload contacts
		C_eIMContacts*  pConts  = static_cast<C_eIMContacts*>(m_pIContacts);
		pConts->SetLoadContactTimestamp(0, TRUE);
		pConts->SetLoadContactTimestamp(0, FALSE);
	}
	_LoadContact();
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime < m_sLoginAck.tTimeStamp.dwUserUpdateTime)
	{
		m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime = m_sLoginAck.tTimeStamp.dwUserUpdateTime;	// Update the timestamps
		UpdateTimsstamp();
	}

	//// 获取用户状态
	//_GetEmpState();

	//// 最新联系人状态订阅
	//m_pSubscribeMgr->FirstSubscribeRecentContact();
	//_CreateThread( CREATE_THREAD_FLAG_SUB );	// Start state subscribe after first subscribe ack

	//// 获取离线消息
	//_GetOfflineMsg();
}

// 获取用户头像修改列表结构
void C_eIMEngine::OnGetUserIconListEnd(int i32Result)					
{
	if (FAILED(i32Result))
	{
		EIMLOGGER_WARN_(m_pILogger, _T("Result: 0x%08X"), i32Result);
		return;
	}

	EIMLOGGER_INFO_(m_pILogger, _T("Get user icon list end!!!"));
	if(m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime < m_sLoginAck.tTimeStamp.dwOthersAvatarUpdateTime)
	{
		m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime = m_sLoginAck.tTimeStamp.dwOthersAvatarUpdateTime;	// Update the timestamps
		UpdateTimsstamp();
	}
}

// 加载组织架构结束
void C_eIMEngine::OnLoadContactEnd()									
{
	m_i32ReconnTimes = 0;

	if ( m_sUpdateContsInfo.dwTime )
	{
		m_sUpdateContsInfo.dwTime = 0;
		C_eIMContactsMgr cmgr;
		cmgr.SetReloadContactsInfo(m_sUpdateContsInfo.dwId, m_sUpdateContsInfo.dwType, m_sUpdateContsInfo.dwElapse, IME_UPDATE_CONTS_FLAG_FINISH);
	}

	SetAttributeInt(IME_ATTRIB_CONTACTS_LOADED, 1);
	// 获取其他人头像时间戳信息

	_UpdateOthersAvatorTS(m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime);
	if(m_bOnlineHasSynInfo == FALSE)
	{
		m_bOnlineHasSynInfo = TRUE;
		// 更新固定群组信息
		_UpdateRegularGroupInfo(m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime);

		// 获取离线消息
		_GetOfflineMsg();

		// 获取用户状态
		_GetEmpState();
		C_eIMStateSubscribeMgr::GetObject().UpdateRobotStateOnline();

		m_pSubscribeMgr->FirstSubscribeRecentContact();
		_CreateThread( CREATE_THREAD_FLAG_SUB );	// Start state subscribe after first subscribe ack


		// 获取虚拟组信息
		_GetVirtualGroupMsg();

		//DownloadFileRequest(IME_ATTRIB_ROBOT_MENU_URL, IROBOT_MENU_CONFIG_FILE, eCMD_GET_IROBOT_MENU);	// Get Robot menu
		//DownloadFileRequest(IME_ATTRIB_ROBOT_TOPIC_URL, IROBOT_TOPIC_FILE, eCMD_GET_IROBOT_TOPIC);		// Get Robot topic
        SendAllMsgReadError();

        DWORD CollectTime = 0;
		C_eIMMsgCollectMgr MsgCollectMgr;
        MsgCollectMgr.GetCollectTime(m_sLoginEmpInfo.Id, CollectTime);
        _AddCmd(eTHREAD_FLAG_SEND_IDX, CMD_FAVORITE_SYNC_REQ, &CollectTime);
	//	DownloadFileRequest(IME_ATTRIB_WEBAPP_URL, IROBOT_WEBAPP_FILE, eCMD_WEBAPP_CONFIG);				// Get WebApp Config
	}
}

void C_eIMEngine::_GetVirtualGroupMsg()
{
	VIRTUAL_GROUP_INFO_REQ virtualGroup;

	I_EIMSessionMgr* smgr = NULL;
	if (FAILED(QueryInterface(INAME_EIMENGINE_SESSIONMGR,(void**)&smgr)))
		return ;
	AUTO_RELEASE_(smgr);
	UINT32 virtualGroupUpdatetime = 0;
	smgr->Get(&virtualGroupUpdatetime);

	virtualGroup.cTerminalType = TERMINAL_PC;
	virtualGroup.dwCompID = GetPurview(GET_LOGIN_COID);
	virtualGroup.dwUserID = GetPurview(GET_LOGIN_QEID);
	virtualGroup.dwTimestamp = virtualGroupUpdatetime;
	_AddCmd(eTHREAD_FLAG_SEND_IDX, CMD_VIRTUAL_GROUP_REQ, (void*)&virtualGroup);
}

//void C_eIMEngine::OnCreateMeeting(S_Meeting_create* meetingStartInfo)
//{
//	S_Meeting_create Meeting_CreateJson ;
//	memcpy(&Meeting_CreateJson,meetingStartInfo,sizeof(S_Meeting_create));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX, eCMD_MEETING_CREATE,(void*)&Meeting_CreateJson);
//}
//
//void C_eIMEngine::GetLaunchString(S_GetLaunchString* getLaunchString)
//{
//	S_GetLaunchString sgetLaunchString;
//	memcpy(&sgetLaunchString,getLaunchString,sizeof(S_GetLaunchString));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_GET_LAUNCHSTRING,(void*)&sgetLaunchString);
//}
//
//void C_eIMEngine::OnChangeMeetingMember(S_MeetingMemberChange* changeMeetingMember)
//{
//	S_MeetingMemberChange sMeetingMemberChnage ;
//	memcpy(&sMeetingMemberChnage,changeMeetingMember,sizeof(S_MeetingMemberChange));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_MEMBERCHANGE,(void*)&sMeetingMemberChnage);
//}
//
//void C_eIMEngine::OnReGetMeetingInfo(S_ReGetMeetingInfo* sregetMeetingInfo)
//{
//	S_ReGetMeetingInfo sRegetMeetingInfo ;
//	memcpy(&sRegetMeetingInfo,sregetMeetingInfo,sizeof(S_ReGetMeetingInfo));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_REGETMEETINGINFO,(void*)&sRegetMeetingInfo);
//}
//
//void C_eIMEngine::OnChangeMeetingLevel(S_MeetingLevel* changeMeetingLevel)
//{
//	S_MeetingLevel sMeetingLevel;
//	memcpy(&sMeetingLevel,changeMeetingLevel,sizeof(S_MeetingLevel));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_CHANGE_LEVEL,(void*)&sMeetingLevel);
//}
//
//void C_eIMEngine::OnReceiveMeeting(S_ReceiveMeeting* sreceiveMeeting)
//{
//	S_ReceiveMeeting sReceiveMeeting;
//	memcpy(&sReceiveMeeting,sreceiveMeeting,sizeof(S_ReceiveMeeting));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_RECEIVEMEETING,(void*)&sReceiveMeeting);
//}
//
//void C_eIMEngine::OnMeetingRead(S_MeetingRead* smeetingRead)
//{
//	S_MeetingRead sMeetingRead;
//	memcpy(&sMeetingRead,smeetingRead,sizeof(S_MeetingRead));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_MEETINGREAD,(void*)&sMeetingRead);
//}
//
//void C_eIMEngine::OnGetMeetingVideoUrl(S_MeetingGetVideo* sMeetingGetVideo)
//{
//	S_MeetingGetVideo s_MeetingGetVideo;
//	memcpy(&s_MeetingGetVideo,sMeetingGetVideo,sizeof(S_MeetingGetVideo));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_GETVIDEOURL,(void*)&s_MeetingGetVideo);
//}
//
//void C_eIMEngine::OnDeleteMeeting(S_MeetingCancel* meetingCancel)
//{
//	S_MeetingCancel sMeetingCancel ;
//	memcpy(&sMeetingCancel,meetingCancel,sizeof(S_MeetingCancel));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_CANCEL,(void*)&sMeetingCancel);
//}
//
//void C_eIMEngine::OnGetMeetingRoomlist(S_Meeting_GetRoomlist* GetRoomlistInfo)
//{
//	S_Meeting_GetRoomlist sGetRoomlist;
//	memcpy(&sGetRoomlist,GetRoomlistInfo,sizeof(S_Meeting_GetRoomlist));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX,eCMD_MEETING_GET_ROOMLIST,(void*)&sGetRoomlist);
//}
//
//const char* C_eIMEngine::getLaunchString()
//{
//	if (m_pMeetingLaunchString)
//		return m_pMeetingLaunchString;
//	else
//		return "";	
//}

//void C_eIMEngine::DownloadFileRequest(const TCHAR* pszUrl, const TCHAR* pszFileName, int i32CmdId)		
//{
//	const TCHAR* pszRobotRequest = GetAttributeStr(pszUrl);
//	CHECK_NULL_RETV_(pszRobotRequest);
//
//	eIMString szFile;
//	S_CurlDownload sGetRobot = { 0 };
//
//	::eIMGetFullPath(PATH_TOKEN_LOGINID, PATH_TYPE_CONTACTS_TMP, pszFileName, szFile, FALSE);
//	sGetRobot.qeid			= m_sLoginEmpInfo.Id;
//	sGetRobot.qcid			= m_sLoginEmpInfo.dwCoId;
//	sGetRobot.qdid			= m_sLoginEmpInfo.qdidPid[0];
//	sGetRobot.pszUserCode	= GetAttributeStr( IME_ATTRIB_USER_NAME );
//	sGetRobot.dwLocal		= (DWORD)time(NULL);
//	sGetRobot.dwServer		= GetTimeNow();
//	sGetRobot.pszUrlFmt	= pszRobotRequest;
//	_tcsncpy(sGetRobot.szFile, szFile.c_str(), COUNT_OF_ARRAY_(sGetRobot.szFile));
//	_AddCmd(eTHREAD_FLAG_CURL_IDX, i32CmdId, (void*)&sGetRobot);
//}


// 获取用户资料
void C_eIMEngine::OnGetEmployeeAck(PS_EmpInfo pEmpInfo)		
{
	if(pEmpInfo->Id == m_sLoginAck.uUserId)
	{
		pEmpInfo->bitVisible = 1;	// Myself alwayse visable
		memcpy(pEmpInfo->qdidPid, m_sLoginEmpInfo.qdidPid, sizeof(m_sLoginEmpInfo.qdidPid));
		SetLoginEmp(pEmpInfo); 
		C_eIMContacts::GetObject().SetEmps(*pEmpInfo, eIMECSET_ADD);

		// 更新个人头像
		_UpdatePersonalAvatar(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAvatarUpdateTime);

		SAFE_SEND_EVENT_(m_pIEventMgr,  EVENT_MODIFY_MYSELF_INFO_RSLT, (void*)0);

		if(m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalInfoUpdateTime < m_sLoginAck.tTimeStamp.dwPersonalInfoUpdateTime)
		{
			m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalInfoUpdateTime = m_sLoginAck.tTimeStamp.dwPersonalInfoUpdateTime;
			UpdateTimsstamp();
		}

		EIMLOGGER_INFO_(m_pILogger, _T("Login emp ok"));
	}

}

void C_eIMEngine::OnGetDeptShowAck(GETDEPTSHOWCONFIGACK* psAck)
{
	CHECK_NULL_RETV_(psAck);
	m_sLastUpdateTTs.dwTTShowDeptConfig = psAck->dwUpdateTime;
}

void C_eIMEngine::OnSaveMeetingAccountInfo(confUserInfoNotice* pAck)
{
    if (pAck)
    {
		memcpy(&m_sMeetingAccountInfo, pAck, sizeof(confUserInfoNotice));
		//SAFE_SEND_EVENT_(m_pIEventMgr,EVENT_INIT_MEETING,(void*)m_sMeetingAccountInfo.strConfUserId);
		
		char tempTip[1024];
		eIMString szTempTip;
		snprintf(tempTip,COUNT_OF_ARRAY_(tempTip),"ReceiveConfUserInfo dwUserId:%u strConfUserId:%s strConfUserCode:%s strConfPwd:%s acctType:%d cConfLang:%d cUpdateType:%d ",
			m_sMeetingAccountInfo.dwUserId,m_sMeetingAccountInfo.strConfUserId,m_sMeetingAccountInfo.strConfUserCode,m_sMeetingAccountInfo.strConfPwd,(int)m_sMeetingAccountInfo.acctType,(int)m_sMeetingAccountInfo.cConfLang,(int)m_sMeetingAccountInfo.cUpdateType);
		eIMUTF8ToTChar(tempTip,szTempTip);
		EIMLOGGER_INFO_(m_pILogger,_T("%s"),szTempTip.c_str());
    }
}

void C_eIMEngine::OnUrlFileTransferAction(T_UrlFileTranInfo* pTUrlFileInfo)
{
    if (m_eEngineStatus == eENGINE_STATUS_ONLINE || m_eEngineStatus == eENGINE_STATUS_LEAVE )
	{
        switch(pTUrlFileInfo->bitAction)
        {
        case FILE_ACTION_RECV:
            {
                
                std::list<T_UrlFileTranInfo>::iterator itr = m_urlFileDownLst.begin();
                while(itr != m_urlFileDownLst.end())
                {
                   if(  0 == _tcscmp(pTUrlFileInfo->szUrlFile, (*itr).szUrlFile )
                       && 0 == _tcscmp(pTUrlFileInfo->szSaveFile, (*itr).szSaveFile ) )
                   {
                       return;
                   }
                    ++itr;
                }
                T_UrlFileTranInfo fileInfo = *pTUrlFileInfo;
                m_urlFileDownLst.push_back(fileInfo);
                _CreateThread(CREATE_THREAD_FLAG_DOWNLOAD_URLFILE);
            
            }
            break;
        case FILE_ACTION_CANCEL:
            {
                std::list<T_UrlFileTranInfo>::iterator it = m_urlFileDownLst.begin();
                while(it != m_urlFileDownLst.end())
                {
                    eIMString szStopUrl(pTUrlFileInfo->szUrlFile);
                    if( 0 == szStopUrl.compare( (*it).szUrlFile) && 0 == _tcscmp(pTUrlFileInfo->szSaveFile, (*it).szSaveFile ))
                    {
                        (*it).bitAction = FILE_ACTION_CANCEL;
                        break;
                    }
                    ++it;
                }
            }
            break;
        default:
            break;
        }
    }
}

void C_eIMEngine::OnFileTransferAction(PS_FileTransInfo psFTI)
{
	CHECK_NULL_RETV_(psFTI);
	if (m_eEngineStatus == eENGINE_STATUS_ONLINE || m_eEngineStatus == eENGINE_STATUS_LEAVE )
	{
		int i32Ret = _AddCmd(eTHREAD_FLAG_DISP_IDX, eCMD_FILE_ACTION, psFTI);
		if ( SUCCEEDED(i32Ret) )
		{
			EIMLOGGER_DEBUG_(m_pILogger, _T("Add file transfer action of qfid:%llu, file:%s succeed"), psFTI->u64Fid, psFTI->szFile);
		}
		else
		{
			EIMLOGGER_ERROR_(m_pILogger, _T("Add file transfer action of qfid:%llu, file:%s failed, error: 0x%08x"), psFTI->u64Fid, psFTI->szFile, i32Ret);
			if ( psFTI->bitAction )
			{
				S_FileTransInfo sFTI;
				psFTI->dwTimestamp = ::GetTickCount();
				psFTI->bitStatus   = FILE_STATUS_FAILED;
				memcpy(&sFTI, psFTI, sizeof(S_FileTransInfo));
				sFTI.dwTimestamp++;	// Newest;
				SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_FILE_TRANSFER, &sFTI);
			}
		}
	}
	else
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Not login, can not send or receive file, qfid:%llu, file:%s"), psFTI->u64Fid, psFTI->szFile);
		if ( psFTI->bitAction )
		{
			S_FileTransInfo sFTI;
			psFTI->dwTimestamp = ::GetTickCount();
			psFTI->bitStatus   = FILE_STATUS_FAILED;
			memcpy(&sFTI, psFTI, sizeof(S_FileTransInfo));
			sFTI.dwTimestamp++;	// Newest;
			SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_FILE_TRANSFER, &sFTI);
		}
	}
}

void C_eIMEngine::OnFileTransfer( S_FileTransInfo* psFTI )
{
	CHECK_NULL_RETV_(psFTI);
	if(psFTI->bitIsImage == FILE_SERVER_TYPE_FILE)
	{
		EIMLOGGER_DATA_( m_pILogger, NULL, 0, _T("Timestamp:%u, FileSize:%llu, TransSize:%llu, qfid:%llu, Status:%u, Action:%u, Direct:%s, Type:File, Modify:%u, TransType:%s, File:%s, Key:%s"),
			psFTI->dwTimestamp,
			psFTI->i64FileSize,
			psFTI->i64TransSize,
			psFTI->u64Fid, 
			psFTI->bitStatus, 
			psFTI->bitAction,
			psFTI->bitDirect == FILE_DIR_DOWNLOAD ? _T("Download") : _T("Upload"),
			psFTI->bitIsModify,
			psFTI->bitType == FILE_TRANS_TYPE_OFFLINE ? _T("Offline") : _T("P2P"), 
			psFTI->szFile,  
			psFTI->szKey);
	}
	else if(psFTI->bitIsImage == FILE_SERVER_TYPE_IMAGE)
	{
		EIMLOGGER_DATA_( m_pILogger, NULL, 0, _T("Timestamp:%u, FileSize:%llu, TransSize:%llu, qfid:%llu, Status:%u, Action:%u, Direct:%s, Type:Image, Modify:%u,  TransType:%s, File:%s, Key:%s"),
			psFTI->dwTimestamp, 
			psFTI->i64FileSize,
			psFTI->i64TransSize,
			psFTI->u64Fid, 
			psFTI->bitStatus, 
			psFTI->bitAction,
			psFTI->bitDirect == FILE_DIR_DOWNLOAD ? _T("Download") : _T("Upload"),
			psFTI->bitIsModify,
			psFTI->bitType == FILE_TRANS_TYPE_OFFLINE ? _T("Offline") : _T("P2P"), 
			psFTI->szFile,  
			psFTI->szKey);
	}
	else if(psFTI->bitIsImage == FILE_SERVER_TYPE_HEAD || psFTI->bitIsImage == FILE_SERVER_TYPE_HEADS)
	{
		if(psFTI->bitStatus == FILE_STATUS_FINISHED)
		{
			if (hpi.UIGetPortraitMode() == PORTRAIT_MODE_NEW)
			{
				TCHAR szPath[TOKEN_MAX_SIZE] = { 0 };
				eIMGetGrayImgPath(szPath, _countof(szPath));
				if(!BitmapColorToGray(psFTI->szFile,szPath))
					return;
			}

			if ( PS_EmpInfo psEmpInfo = (PS_EmpInfo)C_eIMContacts::GetObject().GetEmp(psFTI->u32Eid) )
			{	// 必须先更新通讯录中的状态，以避免自己的头像更新形成死循环
				psEmpInfo->dwLogoTime = IME_EMP_LOGO_NO_DOWNLOAD;
				C_eIMContacts::GetObject().SetEmps(*psEmpInfo,eIMECSET_MODS_UPDATE_LOGO,FALSE);
			}

		//	hpi.UpdateLogo(psFTI->u32Eid, kLogoTplSmall, NULL);	// 生成小头像（不生成小头像，从后台下载）

			if( psFTI->u32Eid == GetPurview(GET_LOGIN_QEID) )
			{
				m_sLoginEmpInfo.dwLogoTime = IME_EMP_LOGO_NO_DOWNLOAD;
				EIMLOGGER_DATA_( m_pILogger, NULL, 0, _T("(**Head img**) Download finish login Id:[%u],State:[%d]"), psFTI->u32Eid, psFTI->bitStatus);
				SAFE_SEND_EVENT_(m_pIEventMgr,EVENT_MY_CHANGED,(void*)EVENT_MY_LOGO);
			}
			else
			{
				EIMLOGGER_DATA_( m_pILogger, NULL, 0, _T("(**Head img**) Download finish others Id:[%u],State:[%d]"), psFTI->u32Eid, psFTI->bitStatus);
				SAFE_SEND_EVENT_(m_pIEventMgr,EVENT_UPDATE_HEAD_IMG,(void*)psFTI->u32Eid);
			}

			if ( psFTI->bitIsImage == FILE_SERVER_TYPE_HEADS )
			{	// 只有小头像，才设置已经下载标识（大头像，始终下载：《不想大改》）
				S_EmpHeadFlag sEmpHeadFlag;

				sEmpHeadFlag.qeid = psFTI->u32Eid;
				sEmpHeadFlag.dwDownloadFlag = IME_EMP_LOGO_NO_DOWNLOAD;		
				_AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_UPDATE_EMP_HEAD_FLAG, &sEmpHeadFlag);
			}
		}
	}
}

// 更新已经保存的时间戳
void C_eIMEngine::UpdateTimestamp(I_SQLite3* pDB)								
{
	C_eIMContactsMgr cmgr;
	cmgr.SetTimestamp(&m_sLastUpdateTTs, m_sLoginAck.uUserId);
}

// 更新个人信息
void C_eIMEngine::_UpdatePersonalInfo(DWORD dwTime)
{
	if (m_sLoginAck.tTimeStamp.dwPersonalInfoUpdateTime >dwTime || dwTime == 0 )
	{	
		// 要向服务器请求个人信息
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_EMPLOYEE_INFO, &m_sLoginAck.uUserId);
	}
	else
	{
		S_EmpInfo rInfo;
		memset(&rInfo, 0, sizeof(rInfo));
		C_eIMContactsMgr cmgr;
		cmgr.LoadEmpInfo(m_sLoginAck.uUserId, rInfo);
		rInfo.bitLoginType = IME_EMP_TERM_PC;
		rInfo.bitSex = m_sLoginAck.sex;	ASSERT_(rInfo.bitSex == 0 || rInfo.bitSex == 1);
		rInfo.bitStatus = GetAttributeInt(IME_ATTRIB_LOGIN_TYPE);
		rInfo.dwCoId = m_sLoginAck.dwCompID;
		OnGetEmployeeAck(&rInfo);
	}
}

// 更新个人头像信息
void C_eIMEngine::_UpdatePersonalAvatar(DWORD dwTime)
{
	if (m_sLoginAck.tTimeStamp.dwPersonalAvatarUpdateTime > dwTime || dwTime == 0 )
	{	
		//引擎登陆用户头像信息修改
		m_sLoginEmpInfo.dwLogoTime = IME_EMP_LOGO_DOWNLOAD;

		//内存组织架构登陆用户头像信息修改
		S_EmpInfo sUpdateEmpInfo;
		ZERO_STRUCT_(sUpdateEmpInfo);
		sUpdateEmpInfo.Id = m_sLoginEmpInfo.Id;
		sUpdateEmpInfo.dwLogoTime = IME_EMP_LOGO_DOWNLOAD;
		C_eIMContacts::GetObject().SetEmps(sUpdateEmpInfo,eIMECSET_MODS_UPDATE_LOGO,FALSE);

		//数据库登陆用户头像信息修改
		S_EmpHeadFlag sEmpHeadFlag;
		ZERO_STRUCT_(sEmpHeadFlag);
		sEmpHeadFlag.qeid = m_sLoginEmpInfo.Id;
		sEmpHeadFlag.dwDownloadFlag = IME_EMP_LOGO_DOWNLOAD;
		_AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_UPDATE_EMP_HEAD_FLAG, &sEmpHeadFlag);

		m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAvatarUpdateTime = dwTime;
		UpdateTimsstamp();
	}
}

void C_eIMEngine::_UpdateDepts(DWORD dwTime)
{
	if ( m_sLoginAck.tTimeStamp.dwDeptUpdateTime > dwTime || dwTime == 0 )
	{
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_DEPT_LIST, &dwTime);
	}
	else
		OnGetDeptListEnd(EIMERR_NO_ERROR);
}

void C_eIMEngine::_UpdateDeptEmps( DWORD dwTime)
{
	if ( m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime > dwTime || dwTime == 0 )
	{
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_DEPT_USER, &dwTime);
	}
	else
	{
		OnGetDeptEmpsEnd(EIMERR_NO_ERROR);
	}
}

// 获取固定组信息
void C_eIMEngine::_UpdateRegularGroupInfo(DWORD dwTime)
{
	if ( m_sLoginAck.tTimeStamp.dwRegularGroupUpdateTime > dwTime || dwTime == 0 )
	{
		REGULAR_GROUP_UPDATE_REQ sReq;
		sReq.dwUserID = m_sLoginAck.uUserId;
		sReq.dwRegularTime  = m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime;
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_REGULAR_GROUP_UPDATE_REQ, &sReq);
	}
}

void C_eIMEngine::UpdateTimsstamp()
{
	_AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_UPDATE_TIMESTAMP, NULL);	// Start load contacts
}

void C_eIMEngine::_UpdateEmps( DWORD dwTime)
{
	EIMLOGGER_DEBUG_(m_pILogger, _T("m_sLoginAck.tTimeStamp.dwUserUpdateTime:%u, dwTime:%u"), m_sLoginAck.tTimeStamp.dwUserUpdateTime, dwTime);
	if ( m_sLoginAck.tTimeStamp.dwUserUpdateTime > dwTime || dwTime == 0 )
	{
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_USER_LIST, &dwTime);
	}
	else
	{
		OnGetEmpsEnd(EIMERR_NO_ERROR, FALSE);
	}
}

// 加载联系人
void C_eIMEngine::_LoadContact()
{
	_AddCmd(eTHREAD_FLAG_DB_IDX, eCMD_LOAD_CONTACTS, NULL);	// Start load contacts
}

// 获取用户状态
void C_eIMEngine::_GetEmpState()					
{
	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_STATUS_REQ, NULL);
}

// 更新其他人头像时间戳信息
void C_eIMEngine::_UpdateOthersAvatorTS(DWORD dwTime)
{
	if (m_sLoginAck.tTimeStamp.dwOthersAvatarUpdateTime > dwTime || dwTime == 0 )
	{	
		EIMLOGGER_DEBUG_( m_pILogger, _T("Get others head img srv:[%u],loca:[%u]"), m_sLoginAck.tTimeStamp.dwOthersAvatarUpdateTime,dwTime);
		_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_HEAD_ICON_ADD_LIST_REQ, &dwTime);
		//  要向服务器请求其他人头像时间戳
	}
	else
	{
		OnGetUserIconListEnd(EIMERR_NO_ERROR);
	}
}

// 获取离线消息
void C_eIMEngine::_GetOfflineMsg()					
{
	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_OFFLINE_REQ, NULL);
}

void C_eIMEngine::OnCoLastTimeNotice(COMPLASTTIMENOTICE* psNotice)
{
	CHECK_NULL_RETV_(psNotice);
	if ( psNotice->dwCompID != m_sLoginAck.dwCompID )
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Company ID not equal, current cid=%u, notice cid=%u"), m_sLoginAck.dwCompID, psNotice->dwCompID);
		return;
	}

	// Update timestamp when needed
	TUpdateTimeStamp& tt = m_sLoginAck.tTimeStamp;	// User short name
	if ( psNotice->dwDeptUpdateTime > tt.dwDeptUpdateTime )
		tt.dwDeptUpdateTime =  psNotice->dwDeptUpdateTime;

	if ( psNotice->dwDeptUserUpdateTime > tt.dwDeptUserUpdateTime )
		tt.dwDeptUserUpdateTime = psNotice->dwDeptUserUpdateTime;

	if ( psNotice->dwUserUpdateTime > tt.dwUserUpdateTime )
		tt.dwUserUpdateTime = psNotice->dwUserUpdateTime;

	if ( psNotice->vgts > tt.dwRegularGroupUpdateTime )
		tt.dwRegularGroupUpdateTime = psNotice->vgts;
	
	GETDATALISTTYPEPARAMETET sGetDataList = { 0 };
	sGetDataList.cUpdataTypeDept		  = 1;
	sGetDataList.cUpdataTypeUser		  = 1;
	sGetDataList.cUpdataTypeDeptUser	  = 1;
	sGetDataList.dwLastUpdateTimeDept	  = m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime;
	sGetDataList.dwLastUpdateTimeUser	  = m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime;
	sGetDataList.dwLastUpdateTimeDeptUser = m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime;
	_AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_GET_DATA_LIST_TYPE, (void*)&sGetDataList);
}

void C_eIMEngine::OnCheckTimeRsp(CHECK_TIME_RESP* psRsp)
{
	CHECK_NULL_RETV_(psRsp);
	EIMLOGGER_DEBUG_(m_pILogger, _T("timeNow:%u, Serial:%u"), psRsp->timeNow, psRsp->dwSerial);
	m_sLastUpdateTTs.dwTTSvrNow = psRsp->timeNow;
	m_sLastUpdateTTs.dwTTSysNow = psRsp->dwSerial;
}

//=============================================================================
//Function:     Login
//Description:	Connect to server and login
//Notes: Before Login, you can change fellow attribute:
//  IME_ATTRIB_SERVER_IP	
//  IME_ATTRIB_SERVER_PORT	
//  IME_ATTRIB_SERVER_IPB	
//  IME_ATTRIB_SERVER_PORTB
//  IME_ATTRIB_LOG_FILE	
//  IME_ATTRIB_LOG_LEVEL	
//  IME_ATTRIB_LOG_FLAG	
//
//Parameter:
//	pszName    - Login Name, Id, or email and so on
//	pszPsw     - Password
//
//Return:
//		0		Return Succeeded     
//=============================================================================
int C_eIMEngine::Login(
	const TCHAR* pszName, 
	const TCHAR* pszPsw
	)
{
	ZERO_STRUCT_(m_sLoginAck);
	TCHAR* psz = FONT_ID_DEFAULT_S;
	if ( !_IsInit() )
		return EIMERR_NO_INIT;

	if ( pszName == NULL || pszName[0] == _T('\0') )
	{
		EIMLOGGER_INFO_(m_pILogger, _T("Invalid login name!"));
		return EIMERR_NO_USER;
	}
	
	m_i32ReconnTimes = 0;
	// Save Login UserName & UserPsw
	SetEnvironmentVariable(IME_ATTRIB_USER_NAME, pszName);	// Set as environment variable
	SetAttributeStr( IME_ATTRIB_USER_NAME, pszName );
	SetAttributeStr( IME_ATTRIB_USER_PSW,  pszPsw );

	// Initial eIMEngine log file
	eIMString szLogFileDef;
	eIMGetLogFile(szLogFileDef, _T("Engine"), NULL);

	eIMString	szLogFile  = GetAttributeStr( IME_ATTRIB_LOG_FILE);
	DWORD		dwLogFlag  = GetAttributeInt( IME_ATTRIB_LOG_FLAG,  EIMLOG_FLAG_DEFAULT_FULL );
	DWORD		dwLogLevel = GetAttributeInt( IME_ATTRIB_LOG_LEVEL, EIMLOG_LEVEL_DEFAULT );

	if ( szLogFile.empty() )
		szLogFile = szLogFileDef;

	m_pILogger->Init( szLogFile.c_str(), (E_EIMLogLevel)dwLogLevel, dwLogFlag);
	EIMLOGGER_INFO_(m_pILogger, _T("AppVersion: %s"), hpi.UIGetAppVersion(FALSE));
	// DbThread Started after LoginAck OK in C_eIMEngine::OnLoginAck
	if ( !_CreateThread( CREATE_THREAD_FLAG_ALL & ~CREATE_THREAD_FLAG_DB) )
		return FALSE;

	m_bIsUserOffline = FALSE;
	return _Login( FALSE );
}

int	C_eIMEngine::Logout(int nStatus, BYTE cManual)
{
	if ( m_sLoginAck.uUserId == 0 )	// Not login
	{
		if(nStatus == eENGINE_STATUS_OFFLINE || nStatus == eENGINE_STATUS_QUIT)
			m_bIsUserOffline = TRUE;

		return EIMERR_NO_ERROR;
	}

	// 去除离线时重复登录
	if(m_eEngineStatus == eENGINE_STATUS_LOGINING
		&& (nStatus == eENGINE_STATUS_ONLINE || nStatus == eENGINE_STATUS_LEAVE))
	{
		return EIMERR_NO_ERROR;
	}
	// 离线时要登录
	if(m_eEngineStatus != eENGINE_STATUS_LEAVE && m_eEngineStatus != eENGINE_STATUS_ONLINE 
		&& (nStatus == eENGINE_STATUS_ONLINE || nStatus == eENGINE_STATUS_LEAVE))
	{
		if(nStatus == eENGINE_STATUS_ONLINE)
		{
			SetAttributeInt(IME_ATTRIB_LOGIN_TYPE, IME_LOGIN_TYPE_ONLINE);
		}
		else
		{
			SetAttributeInt(IME_ATTRIB_LOGIN_TYPE, IME_LOGIN_TYPE_ONLIVE);
		}
		m_bIsUserOffline = FALSE;
		return _Login(TRUE);
	}
	// 主动离线标识
	if(nStatus == eENGINE_STATUS_OFFLINE || nStatus == eENGINE_STATUS_QUIT)
	{
		m_bIsUserOffline = TRUE;
	}

	// 更新时间戳
	C_eIMContactsMgr cmgr;
	cmgr.SetTimestamp(&m_sLastUpdateTTs, m_sLoginAck.uUserId);

	// 切换状态
	LOGOUT sLogout  = { 0 };
	sLogout.cStatus = nStatus;
	sLogout.cManual = cManual;

	return _AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_LOGOUT, &sLogout);
}

PS_EmpInfo_ C_eIMEngine::GetLoginEmp()
{
	if (PS_EmpInfo_ psEmp = m_pIContacts->GetEmp(m_sLoginEmpInfo.Id))
		return psEmp;	// fix: 首次登录时，无法显示部门，性别

	return &m_sLoginEmpInfo;
}

void C_eIMEngine::SetLoginEmp(PS_EmpInfo_ pLoginEmpInfo)
{
#define ASSIGN_(N)	m_sLoginEmpInfo.N = pLoginEmpInfo->N ?  pLoginEmpInfo->N : m_sLoginEmpInfo.N;

	ASSIGN_(dwSize			);
	ASSIGN_(bitSex			);
	ASSIGN_(bitStatus		);
	ASSIGN_(bitMsgSyncType	);
	ASSIGN_(bitLoginType	);
//	ASSIGN_(bitRef			);
	ASSIGN_(bitPurview		);
//	ASSIGN_(bitIntegrity	);
	ASSIGN_(bitFixedOrders	);
	ASSIGN_(bitConcern		);
	ASSIGN_(bitVisible		);
//	ASSIGN_(qdidPid			);
	ASSIGN_(Id				);
	ASSIGN_(dwBirthday		);
	ASSIGN_(dwHiredate		);
	ASSIGN_(dwRankId		);
	ASSIGN_(dwProfId		);
	ASSIGN_(dwAeraId		);
	ASSIGN_(dwUpdateTime	);
	ASSIGN_(dwLogoTime		);	
	ASSIGN_(pszName			);
	ASSIGN_(pszEnName		);
	ASSIGN_(pszNamePinyin	);
	ASSIGN_(pszNamePinyinS	);
	ASSIGN_(pszDuty			);
	ASSIGN_(pszSign			);
	ASSIGN_(pszCode			);
	ASSIGN_(pszTel			);
	ASSIGN_(pszPhone		);
	ASSIGN_(pszHomeTel		);
	ASSIGN_(pszEmrgTel		);
	ASSIGN_(pszEmail		);
	ASSIGN_(pszAddress		);
	ASSIGN_(pszPostalCode	);
	ASSIGN_(pszFax			);

	eIMString szPsw = GetAttributeStr(IME_ATTRIB_USER_PSW);
	if ( szPsw == LOGIN_USER_DUMMY_PSW )
	{
		C_eIMLoginUserMgr cmgr;
		if ( cmgr.Init() )
			cmgr.Get(GetAttributeStr(IME_ATTRIB_USER_NAME), szPsw);
	}

	//_UpdateDeptEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime);

	//_UpdateEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime);

	// Load contacts
	//_LoadContact();
}

eIMString C_eIMEngine::AddFontElement(E_SessionType eSType, BOOL bSend, TiXmlElement* pParentEl )
{
	eIMString  szMsgFont;
	if ( bSend )
	{
		if (eSType == eSESSION_TYPE_1TO_HUNDRED)
			szMsgFont = GetAttributeStr(MSGUI_FONT_1TOH, _T(MSGUI_FONT_SEND_DEFAULT));
		else if (eSType == eSESSION_TYPE_1TO_TEN_THOUSAND)
			szMsgFont = GetAttributeStr(MSGUI_FONT_1TOT, _T(MSGUI_FONT_SEND_DEFAULT));
		else if (eSType == eSESSION_TYPE_ERROR)
			szMsgFont = GetAttributeStr(MSGUI_FONT_ERROR, _T(MSGUI_FONT_ERROR_DEFAULT));
		else if ( eSType == eSESSION_TYPE_MSGAUTO)
			szMsgFont = GetAttributeStr(MSGUI_FONT_MSGAUTO, _T(MSGUI_FONT_MSGAUTO_DEFAULT));
		else
			szMsgFont = GetAttributeStr(MSGUI_FONT_SEND, _T(MSGUI_FONT_SEND_DEFAULT));
	}
	else
		szMsgFont = GetAttributeStr(MSGUI_FONT_RECV, _T(MSGUI_FONT_RECV_DEFAULT));

	if ( pParentEl )
	{
		eIMStringA szMsgFontA;
		TiXmlDocument xml;

		::eIMTChar2UTF8(szMsgFont.c_str(), szMsgFontA);
		xml.Parse(szMsgFontA.c_str());
		TiXmlElement* pFistChildEl = pParentEl->FirstChildElement();
		if ( pFistChildEl )
			pParentEl->InsertBeforeChild(pFistChildEl, *xml.RootElement());
		else
			pParentEl->InsertEndChild(*xml.RootElement());
	}

	return szMsgFont;
}
 
int __stdcall C_eIMEngine::_EmpCb(PS_EmpInfo_ psEmpInfo, void* pvUserData, DWORD dwPyOrder)
{
	if ( dwPyOrder == 100 )
	{
		*(QEID*)pvUserData = psEmpInfo->Id;
		return 0;
	}

	return 1;
}

DWORD C_eIMEngine::GetPurview(DWORD u32Type)
{
	DWORD u32Ret = 0;

	switch ( u32Type )
	{
		// m_sLoginAck.*** ==============================
	case FILE_TRANS_MODE_RESUME:
		u32Ret = !(m_sLoginAck.wPCMaxSendFileSize == 50);
		break;
	case GET_PERSONAL_DISPLAY:
		u32Ret = m_sLoginAck.dwPersonalDisplay;
		break;
	case GET_PERSONAL_EDIT:				
		u32Ret = m_sLoginAck.dwPersonalEdit;
		break;
	case GET_DEPT_USER_LANGUAGE_DISPLAY:
		u32Ret = m_sLoginAck.cDeptUserLanguageDisplay;
		break;
	case GET_LOGIN_SEX:
		u32Ret = IME_GET_SEX_(m_sLoginEmpInfo.bitSex);
		break;
	case GET_STATUS_INTERVAL:
		u32Ret = m_sLoginAck.cPCGetStatusInterval;
		break;
	case SUBSCRIBE_INTERVAL:
		u32Ret = m_sLoginAck.cPCSubscribeInterval;
		break;
	case TEMP_SUBSCRIBE_MAX_NUM:
		u32Ret = m_sLoginAck.wPCTempSubscribeMaxNum;
		break;
	case GET_STATUS_MAX_NUM:
		u32Ret = m_sLoginAck.wGetStatusMaxNum;
		break;
	case GET_ALIVE_MAX_INTERVAL:
		u32Ret = m_sLoginAck.wPCAliveMaxInterval;
		break;
	case GET_MSG_SYNC_TYPE:
		u32Ret = m_sLoginAck.cMsgSynType;
		break;
	case SEND_SMS_MAX_LENGTH:
		u32Ret = m_sLoginAck.wPCSMSMaxLength;
		break;
	case GET_UPLOAD_RECENT_CONTACT:
		u32Ret = m_sLoginAck.wPCUploadRecentContact;
		break;
	case GET_HTTP_SESSIONID:
		u32Ret = m_sLoginAck.dwSessionID;
		break;
	case SEND_SMS_SPLIT_LENGTH:
		u32Ret = m_sLoginAck.wPCSMSSplitLength;
		break;
	case GET_USER_OFFLINE_FLAG:
		u32Ret = m_bIsUserOffline;
		break;
	case GET_MODIFY_PERSONAL_AUDIT_PERIOD:
		u32Ret = m_sLoginAck.cModifyPersonalAuditPeriod;
		break;
		// m_sLoginEmpInfo.*** ==========================
	case GET_LOGIN_QEID:
		u32Ret = m_sLoginEmpInfo.Id;
		break;
	case GET_LOGIN_COID:
		u32Ret = m_sLoginEmpInfo.dwCoId;
		break;
	case GET_LOGIN_TERM:
		u32Ret = m_sLoginEmpInfo.bitLoginType;
		break;
	case GET_LOGIN_STATUS:
		u32Ret = m_eEngineStatus;
		break;
	case GET_LOGIN_QDID:
		u32Ret = m_sLoginEmpInfo.qdidPid[0];
		break;
		// LOGINACK.tTimeStamp.*** ================
	case GET_UTS_CO:								
		u32Ret = m_sLoginAck.tTimeStamp.dwCompUpdateTime;	
		break;					
	case GET_UTS_DEPT:								
		u32Ret = m_sLoginAck.tTimeStamp.dwDeptUpdateTime;	
		break;			
	case GET_UTS_USER:								
		u32Ret = m_sLoginAck.tTimeStamp.dwUserUpdateTime;	
		break;			
	case GET_UTS_DEPT_USER:							
		u32Ret = m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime;
		break;			
	case GET_UTS_PERSONAL_INFO:						
		u32Ret = m_sLoginAck.tTimeStamp.dwPersonalInfoUpdateTime;	
		break;	
	case GET_UTS_PERSONAL_COMMON_CONTACT:			
		u32Ret = m_sLoginAck.tTimeStamp.dwPersonalCommonContactUpdateTime;
		break;
	case GET_UTS_PERSONAL_COMMON_DEPT:				
		u32Ret = m_sLoginAck.tTimeStamp.dwPersonalCommonDeptUpdateTime;	
		break;
	case GET_UTS_PERSONAL_ATTENTION:				
		u32Ret = m_sLoginAck.tTimeStamp.dwPersonalAttentionUpdateTime;	
		break;
	case GET_UTS_GLOBAL_COMMON_CONTACT:				
		u32Ret = m_sLoginAck.tTimeStamp.dwGlobalCommonContactUpdateTime;
		break;	
	case GET_UTS_OTHERS_AVATAR:						
		u32Ret = m_sLoginAck.tTimeStamp.dwOthersAvatarUpdateTime;	
		break;	
	case GET_UTS_PERSONAL_AVATAR:					
		u32Ret = m_sLoginAck.tTimeStamp.dwPersonalAvatarUpdateTime;	
		break;	
	case GET_UTS_REGULAR_GROUP:						
		u32Ret = m_sLoginAck.tTimeStamp.dwRegularGroupUpdateTime;	
		break;	
	case GET_UTS_USER_RANK:							
		u32Ret = m_sLoginAck.tTimeStamp.dwUserRankUpdateTime;	
		break;		
	case GET_UTS_USER_PRO:							
		u32Ret = m_sLoginAck.tTimeStamp.dwUserProUpdateTime;
		break;				
	case GET_UTS_USER_AREA:							
		u32Ret = m_sLoginAck.tTimeStamp.dwUserAreaUpdateTime;	
		break;		
	case GET_UTS_SPECIAL_LIST:						
		u32Ret = m_sLoginAck.tTimeStamp.dwSpecialListUpdatetime;
		break;			
	case GET_UTS_SPECIAL_WHITE_LIST:				
		u32Ret = m_sLoginAck.tTimeStamp.dwSpecialWhiteListUpdatetime;	
		break;
	case GET_UTS_SERVER_CURRENT_TIME:							
		u32Ret = m_sLoginAck.tTimeStamp.nServerCurrentTime;	
		break;

		// m_sLastUpdateTTs.rUpdateTimeInfo.*** ================
	case GET_LTS_CO:								
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwCompUpdateTime;	
		break;					
	case GET_LTS_DEPT:								
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime;		
		break;		
	case GET_LTS_USER:								
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime;		
		break;		
	case GET_LTS_DEPT_USER:							
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime;	
		break;		
	case GET_LTS_PERSONAL_INFO:						
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalInfoUpdateTime;		
		break;
	case GET_LTS_PERSONAL_COMMON_CONTACT:			
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonContactUpdateTime;
		break;
	case GET_LTS_PERSONAL_COMMON_DEPT:				
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalCommonDeptUpdateTime;	
		break;
	case GET_LTS_PERSONAL_ATTENTION:				
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAttentionUpdateTime;	
		break;
	case GET_LTS_GLOBAL_COMMON_CONTACT:				
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwGlobalCommonContactUpdateTime;	
		break;
	case GET_LTS_OTHERS_AVATAR:						
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwOthersAvatarUpdateTime;		
		break;
	case GET_LTS_PERSONAL_AVATAR:					
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwPersonalAvatarUpdateTime;	
		break;	
	case GET_LTS_REGULAR_GROUP:						
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwRegularGroupUpdateTime;	
		break;	
	case GET_LTS_USER_RANK:							
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwUserRankUpdateTime;	
		break;		
	case GET_LTS_USER_PRO:							
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwUserProUpdateTime;	
		break;			
	case GET_LTS_USER_AREA:							
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwUserAreaUpdateTime;	
		break;		
	case GET_LTS_SPECIAL_LIST:						
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwSpecialListUpdatetime;
		break;			
	case GET_LTS_SPECIAL_WHITE_LIST:				
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.dwSpecialWhiteListUpdatetime;	
		break;
	case GET_LTS_SERVER_CURRENT_TIME:							
		u32Ret = m_sLastUpdateTTs.rUpdateTimeInfo.nServerCurrentTime;	
		break;

		// Purview ======================================
	case CREATE_SESSION_PURVIEW:
//	case GET_GROUP_MAX_MEMBER_NUM:
		if ( m_sLoginAck.wPurview & CREATE_SESSION_PURVIEW )
			u32Ret = m_sLoginAck.mPurview[0].dwParameter;
		
		if ( u32Ret == 0 )
			u32Ret = m_sLoginAck.wGroupMaxMemberNum ? m_sLoginAck.wGroupMaxMemberNum : CREATE_SESSION_MAX_DEFAULT;
		break;
	case SEND_FILE_PURVIEW:
		if ( m_sLoginAck.wPurview & SEND_FILE_PURVIEW )
			u32Ret = m_sLoginAck.mPurview[1].dwParameter;

		if ( u32Ret == 0 )
			u32Ret = SEND_FILE_MAX_DEFAULT;
		break;
	case SEND_P2P_FILE_PURVIEW:
		u32Ret = (m_sLoginAck.wPurview & u32Type);
		break;
	case SEND_ALL_BROADCAST_PURVIEW:
		if (m_sLoginAck.wPurview & u32Type)
			u32Ret = (m_sLoginAck.mPurview[3].dwParameter ? m_sLoginAck.mPurview[3].dwParameter : SEND_BROADCAST_MAX_DEFAULT);
		break;
	case SEND_DEPT_BROADCAST_PURVIEW:
		if (m_sLoginAck.wPurview & u32Type)
			u32Ret = (m_sLoginAck.mPurview[4].dwParameter ? m_sLoginAck.mPurview[4].dwParameter : SEND_BROADCAST_MAX_DEFAULT);
		break;
	case SEND_SMS_PURVIEW:
		if ( m_sLoginAck.wPurview & SEND_SMS_PURVIEW )
			u32Ret = (m_sLoginAck.mPurview[5].dwParameter ? m_sLoginAck.mPurview[5].dwParameter : SEND_SMS_MAX_DEFAULT);
		break;
	//case CREATE_MEETING_MAXNUM:
	case SEND_1TO1H_RESP_PURVIEW:
		if ( m_sLoginAck.wPurview & 0x00000040 )
		{
			////字段复用惹的麻烦
			//if (hpi.UIGetAttributeInt(PATH_MAINUI_VOIP, VOIP_SHOWMAIN) == 1)
			//	u32Ret = (m_sLoginAck.mPurview[6].dwParameter ? m_sLoginAck.mPurview[6].dwParameter : CREATE_MEETING_MAX_DEFAULT);
			//else
				u32Ret = (m_sLoginAck.wPurview & SEND_1TO1H_RESP_PURVIEW ? 1 : 0); //m_sLoginAck.mPurview[6].dwParameter服务端始终返回0，因此给固定值
		}
		break;
	case CREATE_MEETING_PURVIEW:
		u32Ret = (m_sLoginAck.wPurview & 0x00000040 ? 1 : 0);
		break;
	case SEND_1TOTH_RESP_PURVIEW:
		if ( m_sLoginAck.wPurview & SEND_1TOTH_RESP_PURVIEW )
			u32Ret = (m_sLoginAck.mPurview[7].dwParameter ? m_sLoginAck.mPurview[7].dwParameter : SEND_1TOTH_RESP_DEFAULT);
		break;
	case SEND_KAPOK_PURVIEW:	// Kapok Tong Fei(Only Mobile used)
		if ( m_sLoginAck.wPurview & SEND_KAPOK_PURVIEW )
			u32Ret = (m_sLoginAck.mPurview[8].dwParameter ? m_sLoginAck.mPurview[8].dwParameter : 0);
		break;
    case SEND_READMSG_PURVIEW:
        u32Ret = (m_sLoginAck.wPurview & SEND_READMSG_PURVIEW  ? 1 : 0);
        break;
    case SEND_RECALLMSG_PURVIEW:
        u32Ret = (m_sLoginAck.wPurview & SEND_RECALLMSG_PURVIEW ? 1 : 0);
        break;
	case CALL_TO_MEETING:
		u32Ret = (m_sLoginAck.wPurview & CALL_TO_MEETING ? 1 : 0);
		break;;
	case INVITE_TO_MEETING:
		u32Ret = (m_sLoginAck.wPurview & INVITE_TO_MEETING ? 1 : 0);
		break;
    case GET_RECALL_TIME:
        u32Ret = m_sLoginAck.tTimeStamp.dwUserRankUpdateTime;
        break;
	case GET_PCONF_USER_INFO:
		u32Ret = (DWORD)&m_sMeetingAccountInfo;
		break;
	case GET_PLOGIN_EMP_INFO:
		u32Ret = (DWORD)&m_sLoginEmpInfo;
		break;
	default:
		EIMLOGGER_WARN_(m_pILogger, _T("Unknown purview type: 0x%08X"), u32Type);
		break;
	}

	TRACE_(_T("type:0x%08x, value:%d"), u32Type, u32Ret);
	return u32Ret;
}

void C_eIMEngine::UpdateLoginState(E_EngineStatus eState)
{
	m_eEngineStatus = eState;

	S_EmpInfo sEmpInfo;
	ZERO_STRUCT_(sEmpInfo);

	sEmpInfo.Id = m_sLoginEmpInfo.Id;

	if (eState == eENGINE_STATUS_ONLINE)
	{
		sEmpInfo.bitStatus = IME_EMP_STATUS_ONLINE;
	}
	else if (eState == eENGINE_STATUS_LEAVE)
	{
		sEmpInfo.bitStatus = IME_EMP_STATUS_LEAVE;
	}
	else
	{
		sEmpInfo.bitStatus = IME_EMP_STATUS_OFFLINE;
	}

	sEmpInfo.bitLoginType = IME_EMP_TERM_PC;
	m_sLoginEmpInfo.bitStatus = sEmpInfo.bitStatus;
	m_sLoginEmpInfo.bitLoginType = sEmpInfo.bitLoginType ;
	
	if (m_sLoginEmpInfo.Id  == 0 )
		return;

	C_eIMContacts::GetObject().SetEmps(sEmpInfo,eIMECSET_MODS,TRUE);
	if ( sEmpInfo.bitStatus == IME_EMP_STATUS_OFFLINE )
	{
		C_eIMContacts::GetObject().UpdateEmpsState(eIMECSET_MODS, IME_EMP_STATUS_OFFLINE);
	}
}

PS_UpdateTTs_ C_eIMEngine::GetUpdateTimestamp()
{
	return &m_sLastUpdateTTs;
}

QSID C_eIMEngine::NewID(QEID eidRecver, QEID eidSender)
{
	if ( eidRecver )
	{	// 是创建单聊的会话ID
		if ( eidSender == 0 )
			eidSender = m_sLoginEmpInfo.Id;

		typedef union tagIMID
		{
			QMID qmid;				// Msg ID
			struct
			{
				DWORD 	Seq:32;		// Unique serial number, not use time, it too less than a unique seq
				DWORD	Rev:5;		// Reserved, 0x3; 0x2: single session
				DWORD	qeid:24;	// QEID
				DWORD	Type:3;		// Terminate type, PC=3
			};
		}S_IMID;

		S_IMID sID;
		sID.Type = eDEV_TYPE_PC;
		sID.Rev  = eDEV_TYPE_PC;
		if ( eidSender > eidRecver )
		{
			sID.qeid   = eidSender;
			sID.Seq    = eidRecver;
		}
		else
		{
			sID.qeid   = eidRecver;
			sID.Seq    = eidSender ;
		}

		return sID.qmid;
	}

	if ( m_pIClientAgent )
		return m_pIClientAgent->PackMsgId();

	ASSERT_(FALSE);
	return 0;
}

BOOL C_eIMEngine::Unzip(const char* pszZipFile, const char* pszUnzipFile, const char* pszPsw)
{
	eIMString szZipFile;
	eIMString szUnzipFile;

	::eIMMByte2TChar(pszZipFile, szZipFile);
	::eIMMByte2TChar(pszUnzipFile, szUnzipFile);

	return hpi.Unzip(szZipFile.c_str(), szUnzipFile.c_str(), pszPsw);
}


// Download constacts files--begin///////////////////////////////////////////////////////////////////
typedef struct tagParseDepts
{
	I_SQLite3* pIDb;
	I_EIMLogger* pILogger;
	VectDeptInfo vectDeptInfo;
}S_ParseDepts, *PS_ParseDepts;

typedef struct tagParseUserDept
{
	I_SQLite3* pIDb;
	I_EIMLogger* pILogger;
	VectUserDept vectUserDept;
}S_ParseUserDepts, *PS_ParseUserDepts;

typedef struct tagParseUsers
{
	I_SQLite3* pIDb;
	I_EIMLogger* pILogger;
	VectUserInfo vectUserInfo;
}S_ParseUsers, *PS_ParseUsers;

bool __stdcall C_eIMEngine::_ParseContDeptCallback(PS_ContsData_ psCData, void* pvUserData)
{	// 部门ID[0]|父部门ID[1]|部门中文名称[2]|部门英文名称[3]|更新类型[4]|部门排序[5]|部门电话[6]|最后更新时间[7]|所属公司[8]
	PS_ParseDepts psDeptInfo = static_cast<PS_ParseDepts>(pvUserData);
	CHECK_NULL_RET_(psDeptInfo, false);

	if ( psCData )
	{
		eIMString szName;
		psDeptInfo->vectDeptInfo.push_back(psCData->sDeptInfo);
		EIMLOGGER_DATA_(psDeptInfo->pILogger, NULL, 0, _T("%u|%u|%s|%S|%u|%u|%S|%u|%u"),
			psCData->sDeptInfo.dwCompID,
			psCData->sDeptInfo.dwDeptID,
			(::eIMUTF8ToTChar(psCData->sDeptInfo.szCnDeptName, szName), szName.c_str()),
			psCData->sDeptInfo.szEnDeptName,
			psCData->sDeptInfo.wUpdate_type,
			psCData->sDeptInfo.wSort,
			psCData->sDeptInfo.aszDeptTel,
			psCData->sDeptInfo.dwUpdateTime,
			psCData->sDeptInfo.dwPID);
	}

	if ( psCData == NULL || // Finished
		 psDeptInfo->vectDeptInfo.size() >= CONTS_CACHESIZE_MAX )
	{
		C_eIMContactsMgr cmgr;

		cmgr.Init(psDeptInfo->pIDb);
		bool bRet = SUCCEEDED(cmgr.Set(psDeptInfo->vectDeptInfo));
		psDeptInfo->vectDeptInfo.clear();
		return bRet;
	}

	return true;
}

bool __stdcall C_eIMEngine::_ParseContUserDeptCallback(PS_ContsData_ psCData, void* pvUserData)
{	// 部门ID[0]|用户ID[1]|用户工号[2]|用户中文名[3]|用户英文名[4]|性别[5]|排序[6]|更新类型[7]|最后更新时间[8]|本人级别[9]|本人业务[10]|本人地域[11]
	PS_ParseUserDepts psUserDept= static_cast<PS_ParseUserDepts>(pvUserData);
	CHECK_NULL_RET_(psUserDept, false);

	if ( psCData )
	{
		eIMString	szName;
		psUserDept->vectUserDept.push_back(psCData->sDeptUser);
		EIMLOGGER_DATA_(psUserDept->pILogger, NULL, 0, _T("%u|%u|%S|%s|%S|%u|%u|%u|%u|%u|%u|%u"),
			psCData->sDeptUser.dwDeptID,
			psCData->sDeptUser.dwUserID,
			psCData->sDeptUser.aszUserCode,
			(::eIMUTF8ToTChar(psCData->sDeptUser.aszCnUserName, szName), szName.c_str()),
			psCData->sDeptUser.aszEnUserName,
			psCData->sDeptUser.cSex,
			psCData->sDeptUser.wSort,
			psCData->sDeptUser.wUpdate_type,
			psCData->sDeptUser.dwUpdateTime,
			psCData->sDeptUser.cRankID,
			psCData->sDeptUser.cProfessionalID,
			psCData->sDeptUser.dwAreaID);
	}

	if ( psCData == NULL || // Finished
		 psUserDept->vectUserDept.size() >= CONTS_CACHESIZE_MAX )
	{
		C_eIMContactsMgr cmgr;

		cmgr.Init(psUserDept->pIDb);
		bool bRet = SUCCEEDED(cmgr.Set(psUserDept->vectUserDept));
		psUserDept->vectUserDept.clear();
		return bRet;
	}

	return true;
}

bool __stdcall C_eIMEngine::_ParseContUsersCallback(PS_ContsData_ psCData, void* pvUserData)
{	// 用户ID[0]|用户工号[1]|用户中文名[2]|用户英文名[3]|用户邮箱[4]|性别[5]|出生日期[6]|职务[7]|办公电话[8]|手机号码[9]|传真[10]|联系地址[11]|邮编[12]|入职时间[13]|个性签名[14]|更新类型[15]|更新时间[16]
	PS_ParseUsers psUsers = static_cast<PS_ParseUsers>(pvUserData);
	CHECK_NULL_RET_(psUsers, false);

	if ( psCData )
	{
		eIMString	szName;
		eIMString	szDuty;
		eIMString	szAddr;
		eIMString	szSign;
		psUsers->vectUserInfo.push_back(psCData->sUserInfo);
		EIMLOGGER_DATA_(psUsers->pILogger, NULL, 0, _T("%u|%S|%s|%S|%S|%u|%u|%s|%S|%S|%S|%s|%S|%u|%s|%u|%u"),
			psCData->sUserInfo.dwUserID,
			psCData->sUserInfo.aszUserCode,
			(::eIMUTF8ToTChar(psCData->sUserInfo.aszCnUserName, szName), szName.c_str()),
			psCData->sUserInfo.aszEnUserName,
			psCData->sUserInfo.aszEmail,
			psCData->sUserInfo.cSex,
			psCData->sUserInfo.dwBirth,
			(::eIMUTF8ToTChar(psCData->sUserInfo.aszPost, szDuty), szDuty.c_str()),
			psCData->sUserInfo.aszTel,
			psCData->sUserInfo.aszPhone,
			psCData->sUserInfo.aszFax,
			(::eIMUTF8ToTChar(psCData->sUserInfo.aszAdrr, szAddr), szAddr.c_str()),
			psCData->sUserInfo.aszPostcode,
			psCData->sUserInfo.dwHiredate,
			(::eIMUTF8ToTChar(psCData->sUserInfo.aszSign, szSign), szSign.c_str()),
			psCData->sUserInfo.wUpdate_type,
			psCData->sUserInfo.dwUpdateTime);
	}

	if ( psCData == NULL || // Finished
		 psUsers->vectUserInfo.size() >= CONTS_CACHESIZE_MAX )
	{
		C_eIMContactsMgr cmgr;

		cmgr.Init(psUsers->pIDb);
		bool bRet = SUCCEEDED(cmgr.Set(psUsers->vectUserInfo));
		psUsers->vectUserInfo.clear();
		return bRet;
	}

	return true;
}

BOOL C_eIMEngine::_GetContsPathFile(const char* pszUrl, eIMStringA& szZipFileA, eIMStringA& szUnzipDir, eIMStringA& szUnzipFileName, eIMString& szZipFile)
{
	CHECK_NULL_RET_(pszUrl, FALSE);
	eIMString	szUrl;
	TCHAR* pszFileName = NULL;

	// Convert to TCHAR
	::eIMUTF8ToTChar(pszUrl, szUrl);
	if ( !::UrlIs(szUrl.c_str(), URLIS_URL) )
		return FALSE;

	pszFileName = ::PathFindFileName(szUrl.c_str());
	CHECK_NULL_RET_(pszFileName, FALSE);

	// Get local zipfile path
	CHECK_NULL_RET_(pszFileName, FALSE);
	CHECK_NULL_RET_(pszFileName[0], FALSE);
	::eIMGetFullPath(PATH_TOKEN_LOGINID, PATH_TYPE_CONTACTS_TMP, pszFileName, szZipFile, FALSE);
	::eIMTChar2MByte(szZipFile.c_str(), szZipFileA);

	// Get unzip path(not include file)
	eIMString szFile = PATH_TOKEN_LOGINID_FILE_(PATH_TYPE_CONTACTS_TMP);
	szFile += _T("\\");
	::eIMReplaceToken(szFile);
	::eIMTChar2MByte(szFile.c_str(), szUnzipDir);

	TCHAR* pszFileNameExt = ::PathFindExtension(pszFileName);
	CHECK_NULL_RET_(pszFileNameExt, FALSE);
	if (_tcsicmp(pszFileNameExt, _T(".zip")) )
		return FALSE;	// Not is ".zip"

	*pszFileNameExt = _T('\0');	// Remove ext '.zip'
	::eIMTChar2MByte(pszFileName, szUnzipFileName);

	if ( ::PathFileExistsA((szUnzipDir + szUnzipFileName).c_str()) )
		::DeleteFileA((szUnzipDir + szUnzipFileName).c_str());

	return TRUE;
}

BOOL C_eIMEngine::DownloadContactFiles(GETDATALISTTYPEACK* pAck, I_SQLite3* pIDb)
{	// Working in DBThread
	CHECK_NULL_RET_(pAck, FALSE);
	CHECK_NULL_RET_(m_pIClientAgent, FALSE);

	eIMString  szZipFile;
	eIMStringA szZipFileA;
	eIMStringA szUnzipDirA;
	eIMStringA szUnzipFileNameA;
	DWORD dwDownloadContsFile = hpi.UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_CONTACT_FILE, 0xFFFFFFFF);

	if ( pAck->cDownLoadTypeDept == 0 && (dwDownloadContsFile & CONTACT_UPDATE_TYPE_DEPT) &&
		_GetContsPathFile((const char*)pAck->strDownLoadPathDept, szZipFileA, szUnzipDirA, szUnzipFileNameA, szZipFile) )
	{	// Depts
		if (pAck->cUpdataTypeDept == 0 )
		{
			C_eIMContactsMgr cmgr;
			cmgr.Init(pIDb);
			cmgr.Clear(CLEAR_FLAG_DEPT);

			m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime = 0;
			UpdateTimestamp(pIDb);
		}

		S_ParseDepts sParseDepts;
		sParseDepts.pIDb = pIDb;
		sParseDepts.pILogger = m_pILogger;
		if (SUCCEEDED(_cUrlDownload((const char*)pAck->strDownLoadPathDept, szZipFile.c_str(), 3)) &&
			Unzip(szZipFileA.c_str(),  szUnzipDirA.c_str(), (const char*)pAck->strFilePwdDept) &&
			SUCCEEDED(m_pIClientAgent->ParseContactsFile((szUnzipDirA + szUnzipFileNameA).c_str(), CONTACT_UPDATE_TYPE_DEPT, _ParseContDeptCallback, (void*)&sParseDepts)) )
		{
			//m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime = pAck->dwLastUpdateTimeDept;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime = m_sLoginAck.tTimeStamp.dwDeptUpdateTime;
			UpdateTimestamp(pIDb);
		}
	}

	if (pAck->cDownLoadTypeDeptUser == 0 && (dwDownloadContsFile & CONTACT_UPDATE_TYPE_DEPTUSER) && 
		_GetContsPathFile((const char*)pAck->strDownLoadPathDeptUser, szZipFileA, szUnzipDirA, szUnzipFileNameA, szZipFile))
	{	// DeptUsers
		if ( pAck->cUpdataTypeDeptUser == 0 )
		{
			C_eIMContactsMgr cmgr;
			cmgr.Init(pIDb);
			cmgr.Clear(CLEAR_FLAG_DEPT_EMPS);

			m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = 0;
			UpdateTimestamp(pIDb);
		}

		S_ParseUserDepts sUserDepts;
		sUserDepts.pIDb = pIDb;
		sUserDepts.pILogger = m_pILogger;
		if (SUCCEEDED(_cUrlDownload((const char*)pAck->strDownLoadPathDeptUser, szZipFile.c_str(), 3)) &&
			Unzip(szZipFileA.c_str(),  szUnzipDirA.c_str(), (const char*)pAck->strFilePwdDeptUser) &&
			SUCCEEDED(m_pIClientAgent->ParseContactsFile((szUnzipDirA + szUnzipFileNameA).c_str(), CONTACT_UPDATE_TYPE_DEPTUSER, _ParseContUserDeptCallback, (void*)&sUserDepts)) )
		{
			//m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = pAck->dwLastUpdateTimeDeptuser;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime;
			UpdateTimestamp(pIDb);
		}
	}

	if (pAck->cDownLoadTypeUser == 0 && (dwDownloadContsFile & CONTACT_UPDATE_TYPE_USER) &&
		_GetContsPathFile((const char*)pAck->strDownLoadPathUser, szZipFileA, szUnzipDirA, szUnzipFileNameA, szZipFile))
	{	// Users
		if ( pAck->cUpdataTypeUser == 0 )
		{
			C_eIMContactsMgr cmgr;
			cmgr.Init(pIDb);
			cmgr.Clear(CLEAR_FLAG_EMPS);

			m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime = 0;
			UpdateTimestamp(pIDb);
		}

		S_ParseUsers sParseUsers;
		sParseUsers.pIDb = pIDb;
		sParseUsers.pILogger = m_pILogger;
		if (SUCCEEDED(_cUrlDownload((const char*)pAck->strDownLoadPathUser, szZipFile.c_str(), 3)) &&
			Unzip(szZipFileA.c_str(), szUnzipDirA.c_str(), (const char*)pAck->strFilePwdUser) &&
			SUCCEEDED(m_pIClientAgent->ParseContactsFile((szUnzipDirA + szUnzipFileNameA).c_str(), CONTACT_UPDATE_TYPE_USER, _ParseContUsersCallback, (void*)&sParseUsers)) )
		{
			//m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime = pAck->dwLastUpdateTimeUser;
			m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime = m_sLoginAck.tTimeStamp.dwUserUpdateTime;
			UpdateTimestamp(pIDb);
		}
	}

	return TRUE;
}

void C_eIMEngine::OnGetDataListTypeAck(GETDATALISTTYPEACK* pAck)
{
	CHECK_NULL_RETV_(pAck);
	//if ( pAck->result != RESULT_SUCCESS )
	//{
	//	EIMLOGGER_ERROR_(m_pILogger, _T("Failed(0x%08X)"), m_pIClientAgent->GetErrorCode(pAck->result));
	//	return;
	//}

	// Server NOT return the timestamp yet, only use the loginack's timestamp
	//m_sLoginAck.tTimeStamp.dwDeptUpdateTime		= pAck->dwLastUpdateTimeDept;
	//m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime	= pAck->dwLastUpdateTimeDeptuser;
	//m_sLoginAck.tTimeStamp.dwUserUpdateTime		= pAck->dwLastUpdateTimeUser;
	// 下面功能，服务器有问题，不能开放。
	//if (pAck->cUpdataTypeDept == 0 && pAck->cDownLoadTypeDept == 1 )
	//{
	//	C_eIMContactsMgr cmgr;
	//	cmgr.Clear(CLEAR_FLAG_DEPT);
	//	m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime = 0;
	//}

	//if (pAck->cUpdataTypeDeptUser == 0 && pAck->cDownLoadTypeDeptUser == 1 )
	//{
	//	C_eIMContactsMgr cmgr;
	//	cmgr.Clear(CLEAR_FLAG_DEPT_EMPS);
	//	m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime = 0;
	//}

	//if (pAck->cUpdataTypeUser == 0 && pAck->cDownLoadTypeUser == 1 )
	//	m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime = 0;

	_UpdateDepts(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime);
	//if ( m_sLoginAck.tTimeStamp.dwDeptUpdateTime > m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime )
	//{	// Update departments
	//	_UpdateDepts(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUpdateTime);
	//}
	//else if ( m_sLoginAck.tTimeStamp.dwDeptUserUpdateTime > m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime )
	//{	// Update deptuser
	//	_UpdateDeptEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwDeptUserUpdateTime);
	//}
	//else if ( m_sLoginAck.tTimeStamp.dwUserUpdateTime > m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime )
	//{	// Update user
	//	_UpdateEmps(m_sLastUpdateTTs.rUpdateTimeInfo.dwUserUpdateTime);
	//}
	//else
	//{
	//	OnGetDeptEmpsEnd(EIMERR_NO_ERROR);
	//}
}


size_t C_eIMEngine::_cUrlWrite(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if(stream)
    {
        return fwrite(ptr, size, nmemb, stream);
    }
	return 0;
} 

size_t C_eIMEngine::_write_data(void* data, size_t size, size_t nmemb, void* content)  
{
	long totalSize = size*nmemb;  
	std::string* symbolBuffer = (std::string*)content;  
	if(symbolBuffer)  
	{  
		symbolBuffer->append((char *)data, ((char*)data)+totalSize);  
	}  
	return totalSize;  
}

size_t C_eIMEngine::_cUrlRead(void *ptr, size_t size, size_t nmemb, void *userp)
{
	TRACE_(_T("_cUrlRead(size:%u) "), size);
	if ( FILE* pflRead = (FILE*)userp )
		return fread(ptr, size, nmemb, pflRead);

	return 0;
} 

int C_eIMEngine::_cUrlStatus(TCHAR *progress_data, curl_off_t llDnldTotal, curl_off_t llDnldNow, curl_off_t /*fUpldTotal*/, curl_off_t /*fUpldNow*/)
{
	if ( llDnldTotal > 0ll)
	{
		C_eIMEngine& eng = Eng;
		if ( !eng._IsInit() )
			return 1;

		if( eng.IsStopDownUrl(progress_data) )
        {
            //返回非0值 ,停止下载
            return -1;
        }
        //发送文件接收进度事件
        int iProgress = (int)(llDnldNow*100.0/llDnldTotal);
		eng.SendUrlDownProgress(progress_data, iProgress);
	}

	return 0;
}

int C_eIMEngine::_cUrlUploadStatus(int* pnUserData, curl_off_t llDnldTotal, curl_off_t llDnldNow, curl_off_t fUpldTotal, curl_off_t fUpldNow)
{
	if ( fUpldTotal > 0ll)
	{
		//发送文件接收进度事件
		int iProgress = (int)(fUpldNow*100.0/fUpldTotal);
		TRACE_(_T("Progress:%d(%llu / %llu) "), iProgress, fUpldNow, fUpldTotal);
		if ( iProgress >= 100 && pnUserData && *pnUserData == -1)
			return 1;
	}
	return 0;
}

size_t HeaderCallback(char *pszBuffer, size_t size, size_t nitems, int *pnUserData)
{
	if ( pszBuffer && pnUserData) 
	{
		int nLen = 0;
		if ( sscanf(pszBuffer, "Content-Length: %u", &nLen) == 1 && nLen == *pnUserData )
			*(int*)pnUserData = -1;
		int i = 0;
	}

	  return nitems * size;
}

void C_eIMEngine::SendUrlDownProgress(TCHAR *pUrl, int iProgress)
{
    tUrlFileTranRes tUrlRes;
    tUrlRes.i32Progress  = iProgress;
    tUrlRes.status = URL_DOWN_DOWNING;
	if ( pUrl )
		_tcsncpy(tUrlRes.szSavePath, pUrl, COUNT_OF_ARRAY_(tUrlRes.szSavePath));
	SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_URL_FILE_TRAN_RES, &tUrlRes);
}

bool C_eIMEngine::IsStopDownUrl(TCHAR* pszUrl)
{
	CHECK_NULL_RET_(pszUrl, false);

    std::list<T_UrlFileTranInfo>::iterator it = m_urlFileDownLst.begin();
    while(it != m_urlFileDownLst.end())
    {
        if( _tcsicmp(pszUrl, it->szSaveFile) == 0 )
        {
            if(FILE_ACTION_CANCEL == it->bitAction)
            {
                return true;
            }
        }
        ++it;
    }
    return false;
}

int C_eIMEngine::_cUrlDownload(const char* pszUrl, const TCHAR* pszSaveAs, int i32Retry, DWORD dwTimeout)
{
	CHECK_NULL_RET_(pszUrl, EIMERR_CURLE_FAILED_INIT);
	CHECK_NULL_RET_(pszSaveAs, EIMERR_CURLE_FAILED_INIT);

	char szProg    = 0;
	FILE* pflWrite = _tfopen(pszSaveAs, _T("wb"));
	CURL *pCurl    = curl_easy_init();
	char szErrBuf[CURL_ERROR_SIZE] = { 0 };

	if ( pflWrite == NULL || pCurl == NULL )
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Initial for download[%S] failed, pflWrite:%p[errno:%u, %S], pCurl:%p"), pszUrl, pflWrite, errno, pszSaveAs, pCurl);
		SAFE_CLOSE_FILE_( pflWrite ) ;
		::DeleteFile(pszSaveAs);
		::RemoveDirectory(pszSaveAs);

		if (pCurl )
			curl_easy_cleanup(pCurl);
		
		return EIMERR_CURLE_FAILED_INIT;
	}

	curl_easy_setopt(pCurl, CURLOPT_URL, pszUrl);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pflWrite);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, _cUrlWrite);
	curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, FALSE);
	curl_easy_setopt(pCurl, CURLOPT_XFERINFOFUNCTION, _cUrlStatus);
    //curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(pCurl, CURLOPT_MAXREDIRS, 10);
	curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, TRUE);
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, pszSaveAs);
	curl_easy_setopt(pCurl, CURLOPT_ERRORBUFFER, szErrBuf);
	//curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, dwTimeout); 
    curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, dwTimeout); 
    
	EIMLOGGER_DEBUG_(m_pILogger, _T("Begin download[%S] to [%s]"), pszUrl, pszSaveAs);
	int i32Result= EIMERR_NO_ERROR;
	CURLcode res = (curl_easy_perform(pCurl));
	if ( res == CURLE_OK )
	{
		EIMLOGGER_DEBUG_(m_pILogger, _T("Finished download[%S] to [%s]"), pszUrl, pszSaveAs);
		
		// Only CURL is CURLE_OK can be get the response code
		long lStatus = 200;
		curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &lStatus);			// Get status code
		if ( lStatus != 200 )
		{
			i32Result = EIMERR_HTTP_ERROR_(lStatus);
			EIMLOGGER_ERROR_(m_pILogger, _T("Curl http(s) error, response code: %d"), lStatus);
		}
	}
	else
	{
		i32Result = EIMERR_CURL_ERROR_(res);
		EIMLOGGER_ERROR_(m_pILogger, _T("Download[%S] to [%s] failed, errorcode: 0x%08x, Curl error info:%S"), pszUrl, pszSaveAs, i32Result, szErrBuf);
	}

	fclose(pflWrite);
    pflWrite = NULL;
	curl_easy_cleanup(pCurl);

	if ( FAILED(i32Result) && i32Retry > 0)
		return _cUrlDownload(pszUrl, pszSaveAs, i32Retry - 1);

	return i32Result;
}

//用于以POST 方式上传JSON数据
//int C_eIMEngine::_cUrlMeeting(const char* pszUrl,char* cjson,char* confid /* = null*/,enumcurlMeetingType i32Retry/* =eMeeting_default */, DWORD dwTimeout/* =30 */)
//{
//	CURL *pCurl = NULL;  
//	CURLcode res;  
//	// In windows, this will init the winsock stuff  
//	curl_global_init(CURL_GLOBAL_ALL);  
//
//	// get a curl handle  
//	pCurl = curl_easy_init();  
//
//	int i32Result= EIMERR_NO_ERROR;
//	if (NULL != pCurl)   
//	{  
//		/*char szJsonData[1024];  
//		memset(szJsonData, 0, sizeof(szJsonData));  
//		std::string strJson = "{";  
//		strJson += "\"user_name\" : \"test\",";  
//		strJson += "\"password\" : \"test123\"";  
//		strJson += "}";  
//		strcpy(szJsonData, strJson.c_str()); */
//		
//		// 设置连接等待超时时间为10秒 
//		curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,10);
//		// 设置超时时间默认为30秒 
//		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, dwTimeout); 
//		curl_easy_setopt(pCurl, CURLOPT_URL, pszUrl);   
//
//		// 设置http发送的内容类型为JSON  
//		curl_slist *plist = curl_slist_append(NULL,   
//			"Content-Type:application/json;charset=UTF-8");  
//		curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);  
//
//		//设置以POST方式上传
//		curl_easy_setopt(pCurl, CURLOPT_POST,				1L);
//		// 设置要POST的JSON数据  
//		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS,cjson);
//
//		// 设置回调函数  
//		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, _write_data); 
//		//设置写数据  
//		std::string strData;   
//		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&strData); 
//		
//		// Perform the request, res will get the return code   
//		res = curl_easy_perform(pCurl);  
//		if ( res == CURLE_OK )
//		{
//			eIMString tip;
//			eIMUTF8ToTChar(pszUrl,tip);
//			EIMLOGGER_DEBUG_(m_pILogger, _T("Finished _cUrlMeeting [%s]"), tip.c_str());
//
//			// Only CURL is CURLE_OK can be get the response code
//			long lStatus = 200;
//			curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &lStatus);			// Get status code
//			if (lStatus == 200)
//			{
//				Data_Maneger(strData,confid,i32Retry);
//			}
//			else
//			{
//				SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_CREATE_MEETING_FAILD, NULL);
//				i32Result = EIMERR_HTTP_ERROR_(lStatus);
//				EIMLOGGER_ERROR_(m_pILogger, _T("MeetingCreate Faild Status: %d"), lStatus);
//			}
//		}
//		else
//		{
//			SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_CREATE_MEETING_FAILD, NULL);
//			i32Result = EIMERR_CURL_ERROR_(res);
//			EIMLOGGER_ERROR_(m_pILogger, _T("MeetingCreate Faild ret is not ok: %d"), res);
//		}
//		// always cleanup  
//		curl_easy_cleanup(pCurl); 
//		curl_global_cleanup();
//	}
//	return i32Result;
//}

//用于上传修改的头像
int C_eIMEngine::_cUrlUpload(const char* pszUrl, const TCHAR* pszFile ,S_UploadMeetingFile* sUploadFile,int i32Retry, DWORD dwTimeout)
{
	eIMStringA szUrlA;
	struct _stat sStat = { 0 };
	FILE*	 pflRead = _tfopen(pszFile, _T("rb"));				CHECK_NULL_RET_(pflRead, false);
	CURLcode ret     = curl_global_init(CURL_GLOBAL_DEFAULT);	CHECK_NULL_RET_(ret == CURLE_OK, false);
	CURL*	 pCurl   = curl_easy_init();						CHECK_NULL_RET_(pCurl, false);
	struct curl_slist *psHeaders = NULL;
	S_UploadMeetingFile sUploadMeetingFile;

	_tstat(pszFile, &sStat);	// Get file size
	int nUserData = sStat.st_size;
	
	psHeaders = curl_slist_append(psHeaders, HTTP_HEADER_CONTENTTYPE);

	psHeaders = curl_slist_append(psHeaders, HTTP_HEADER_ACCEPT);
	psHeaders = curl_slist_append(psHeaders, HTTP_HEADER_KEEP_ALIVE);

	curl_easy_setopt(pCurl, CURLOPT_URL,				pszUrl);/* First set the URL that is about to receive our POST. */
	
	/* tell it to "upload" to the URL */ 
	curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER,			psHeaders);		// Header
	
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER,		0);				// SECURITY_FLAG_IGNORE_REVOCATION?
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST,		0L);			// No sso verify
	//curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT,		60); 
	curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS,			FALSE);			// Need show progress
	curl_easy_setopt(pCurl, CURLOPT_XFERINFODATA,	(void*)&nUserData);
	curl_easy_setopt(pCurl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(pCurl, CURLOPT_HEADERDATA,		(void*)&nUserData);
	
	curl_easy_setopt(pCurl, CURLOPT_POST,				1L);			/* Now specify we want to POST data */
	curl_easy_setopt(pCurl, CURLOPT_READFUNCTION,		_cUrlRead);	/* we want to use our own read function */
	curl_easy_setopt(pCurl, CURLOPT_XFERINFOFUNCTION,	_cUrlUploadStatus);   // Status function

	curl_easy_setopt(pCurl, CURLOPT_READDATA,			pflRead);		/* pointer to pass to our read function */
	curl_easy_setopt(pCurl, CURLOPT_VERBOSE,			1L);			/* get verbose debug output please */
	//curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS,			&sStat);	/* Set the expected POST size. If you want to POST large amounts of data, consider CURLOPT_POSTFIELDSIZE_LARGE */
	curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE,		sStat.st_size);
	/* Perform the request, res will get the return code */
	ret = curl_easy_perform(pCurl);

	/* always cleanup */
	curl_easy_cleanup(pCurl);
	fclose(pflRead);

	/* Check for errors */
	if(ret == CURLE_OK || ret == CURLE_ABORTED_BY_CALLBACK)
	{
		EIMLOGGER_DEBUG_(m_pILogger, _T("_cUrlUpload Get CURLE_OK"));
	}
	else
	{
		EIMLOGGER_DEBUG_(m_pILogger, _T("_cUrlUpload failed  %d  fileName:%s"),ret,pszFile);
	}
	return 0;
}

LONGLONG C_eIMEngine::GetFileSize(const TCHAR* pszFile)
{
	struct __stat64	statFile = { 0 };

	if (_tstat64(pszFile, &statFile) < 0 || statFile.st_size == 0)
		statFile.st_size = 0;

	return statFile.st_size;
}

BOOL C_eIMEngine::GetFileName(const TCHAR* pszFile,eIMString& szName)
{
	CHECK_NULL_RET_(pszFile, FALSE);
	const TCHAR* pszName = _tcsrchr(pszFile, _T('\\'));

	if ( pszName == NULL )
		pszName = _tcsrchr(pszFile, _T('/'));

	if ( pszName )
		szName = pszName + 1;

	return !szName.empty();
}

BOOL C_eIMEngine::_InstallConcatsDb(QEID qeid, const char* pszDownloadUrl, const char* pszPsw)
{
	BOOL bDownloadContsDb = hpi.UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_CONTACTDB, 1);
	BOOL bDisableProxy    = hpi.UIGetAttributeInt(PATH_ENGINE, ENGINE_ATTR_DOWNLOAD_CONTACTDB_DISABLE_PROXY, 1);
	eIMStringA	szDbFileA;
	eIMString	szDbFile;
	eIMString	szSrcDbFile = PATH_TOKEN_EXE_FILE_(CONTACTS_DB_FILE);

	if (!GetDbFile(eDBTYPE_CONTS, szDbFileA))						// Get current logined user contacts.db file path
	{
		EIMLOGGER_ERROR_(m_pILogger, _T("Get contacts.db file failed")); 
		return FALSE;
	}

	::eIMUTF8ToTChar(szDbFileA.c_str(), szDbFile);
	if ( ::eIMPathFileExists(szDbFile.c_str()) )
	{
		EIMLOGGER_DEBUG_(m_pILogger, _T("Contacts database [%s] already exist, not need initialized"), szDbFile.c_str());
		return FALSE;
	}

	EIMLOGGER_DEBUG_(m_pILogger, _T("DownloadContsDb:%d, DownloadUrl:%S"), bDownloadContsDb, pszDownloadUrl);
	if ( bDownloadContsDb && pszDownloadUrl && *pszDownloadUrl)
	{
		eIMString		szZipDbFile;
		eIMStringA		szZipDbFileA;
		S_LoginStatus	sLoginStatus = { 0 };

		::eIMGetFullPath(PATH_TOKEN_LOGINID, PATH_TYPE_CONTACTS_TMP, _T("contacts.db.zip"), szZipDbFile, FALSE);
		::eIMTChar2MByte(szZipDbFile.c_str(), szZipDbFileA);
		sLoginStatus.dwStatus = EIMERR_DOWNLOADING_DB;
		SAFE_SEND_EVENT_(m_pIEventMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
		if (FAILED(_cUrlDownload(pszDownloadUrl, szZipDbFile.c_str())))
			return FALSE;

		if ( ::eIMPathFileExists(szZipDbFile.c_str()) )
		{	// Download succeeded
			eIMString szUnzipDir = PATH_TOKEN_LOGINID;
			::eIMReplaceToken(szUnzipDir);
			::eIMTChar2MByte(szUnzipDir.c_str(), szDbFileA);
			if ( Unzip(szZipDbFileA.c_str(), szDbFileA.c_str(), pszPsw) )
			{
				EIMLOGGER_DEBUG_(m_pILogger, _T("Unzip downloaded contacts database file [%s] succeeded"), szZipDbFile.c_str());
				C_eIMContactsMgr cmgr;
				S_UpdateTTs sUpdateTTs = { 0 };

				_InitDb(FALSE, GetPurview(GET_LOGIN_QEID));	// need init it at first
				cmgr.GetTimestamp(&sUpdateTTs, 0);		// 0 Is a specify QEID 
				cmgr.SetTimestamp(&sUpdateTTs, qeid);	// Update qeid 0 as current logined user's timestamp
				return TRUE;
			}
			else
			{
				EIMLOGGER_ERROR_(m_pILogger, _T("Unzip downloaded contacts database file [%s] failed"), szZipDbFile.c_str());
				::DeleteFile(szDbFile.c_str());
			}
		}
		else
		{
			EIMLOGGER_ERROR_(m_pILogger, _T("Downloaded contacts database file [%s] failed"), szZipDbFile.c_str());
		}
	}
	else
	{
		::eIMReplaceToken(szSrcDbFile);									// Get the installed contacts.db
		if ( ::eIMPathFileExists(szSrcDbFile.c_str()) && !::eIMPathFileExists(szDbFile.c_str()))
		{
			EIMLOGGER_DEBUG_(m_pILogger, _T("Copy contacts.db from installed, file:%s"), szSrcDbFile.c_str());
			if ( ::CopyFile(szSrcDbFile.c_str(), szDbFile.c_str(), TRUE) )	// Copy if not exist in user folder
			{
				_InitDb(FALSE, GetPurview(GET_LOGIN_QEID));	// need init it at first
				C_eIMContactsMgr cmgr;
				S_UpdateTTs sUpdateTTs = { 0 };

				cmgr.GetTimestamp(&sUpdateTTs, 0);		// 0 Is a specify QEID 
				cmgr.SetTimestamp(&sUpdateTTs, qeid);	// Update qeid 0 as current logined user's timestamp
				return TRUE;
			}
		}
		EIMLOGGER_DEBUG_(m_pILogger, _T("Not copy contacts.db from installed when it already existed"));
	}

	return FALSE;
}


void C_eIMEngine::AddFontElement(E_SessionType eSType, BOOL bSend, void* pParentEl)
{
	if(pParentEl)
	{
		TiXmlElement* pEl = (TiXmlElement*)pParentEl;
		AddFontElement(eSType, bSend, pEl);
	}
}

BOOL C_eIMEngine::IsFace(const TCHAR* pszFace)
{
	return  C_XmlEmoticon::GetObject().IsExistEmot(pszFace);
}

void C_eIMEngine::SetUrlFileInfo(const S_UrlFileInfo& sUrlInfo)
{
    m_pUrlFileMgr->Set(sUrlInfo);
}

BOOL C_eIMEngine::GetLocalPath(const S_UrlFileInfo& sUrlInfo, PFN_GetLocalPath pfnGetLocalPath, void* pvUserData)
{
    return m_pUrlFileMgr->GetLocalPath(sUrlInfo, pfnGetLocalPath, pvUserData);
}

void C_eIMEngine::SendAllMsgReadError()
{
    C_eIMMsgReadErrorMgr cMsgReadError;
    std::list<eIMString> MsgReadLst;
    cMsgReadError.GetAllMsgReads(C_eIMEngine::GetAllMsgReadError, &MsgReadLst);
    std::list<eIMString>::iterator it = MsgReadLst.begin();
    while(it != MsgReadLst.end())
    {
        _AddCmd(eTHREAD_FLAG_SEND_IDX, eCMD_MSG_READ, (*it).c_str());
        ++it;
    }
}

int     __stdcall C_eIMEngine::GetAllMsgReadError(LPCTSTR pszXml, void *pvUserData)
{
    std::list<eIMString>* pList = (std::list<eIMString>*)pvUserData;
    pList->push_back(pszXml);
    return 1;
}

inline eIMString C_eIMEngine::GetReciveItems(const TiXmlHandle& h)
{
    eIMString szReceiveItem;
    TiXmlElement* pSubEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGANNEX).ToElement();
    if(pSubEl)
    {
        TiXmlElement* pCmdNode = h.ToElement();
        int iRead = 0;
        if(pCmdNode)
        {
            pCmdNode->Attribute(FIELD_READ, &iRead);
        }
        if(1 == iRead)
        {
            TiXmlElement* pItemNode = h.FirstChildElement(FIELD_ITEM).ToElement();
            while(pItemNode)
            {
                const char*pNodeName = pItemNode->Value();
                eIMStringA szItemName(FIELD_ITEM);
                if( szItemName == pNodeName)
                {
                    eIMString szItemAttr;
                    _GetAttrString(pItemNode, szItemAttr);
                    szReceiveItem+= _T("<Item ");
                    szReceiveItem+= szItemAttr;	
                    szReceiveItem+= _T("/>");
                }
                pItemNode = pItemNode->NextSiblingElement();
            }
        }   
    }
    return szReceiveItem;
}

void C_eIMEngine::DelCollectMsg(const std::list<UINT64>& CollectIdLst)
{
    int iTotal = CollectIdLst.size();
    if(0 >= iTotal)
    {
        return;
    }
    std::list<UINT64>::const_iterator it = CollectIdLst.begin();
    while(it != CollectIdLst.end())
    {
        FAVORITE_MODIFY_REQ sModifyCollect;
        sModifyCollect.cOperType = 3;
        sModifyCollect.stFavoriteBatch.wNum = 0;
        sModifyCollect.wTotalNum = 0;
        sModifyCollect.wTotalNum = iTotal;
        for (int i=0; i<  MAX_BATCH_OPERA_NUM && it != CollectIdLst.end(); ++i, ++it)
        {
            sModifyCollect.stFavoriteBatch.ddwMsgID[i] = (*it);
            ++sModifyCollect.stFavoriteBatch.wNum;
            ++sModifyCollect.wCurNum;
        } 
        _AddCmd(eTHREAD_FLAG_SEND_IDX, CMD_FAVORITE_MODIFY_REQ, &sModifyCollect);
    }
}


int C_eIMEngine::SaveNeedUpdateRead(S_UpdateReadMsg readMsg)
{
    if(m_needUpdateReadMap.empty())
    {
        m_needUpdateReadMap.push_back(readMsg);
    }
    else
    {
        m_needUpdateReadMap.remove(readMsg);
        m_needUpdateReadMap.push_back(readMsg);
    }
    return 0;
}

bool C_eIMEngine::GetUpdateRead(QSID qSid, std::list<S_UpdateReadMsg>& readMsgLst)
{
    bool bUpdate = false;
    if(m_needUpdateReadMap.empty())
    {
        return bUpdate;
    }
    std::list<S_UpdateReadMsg>::iterator itr = m_needUpdateReadMap.begin();
    while(itr != m_needUpdateReadMap.end())
    {
        QSID itemSid = itr->qsid;
        if(itemSid == qSid)
        {
            readMsgLst.push_back((*itr));
            bUpdate = true;
        }
        ++itr;
    }

   
    std::list<S_UpdateReadMsg>::iterator delitr = readMsgLst.begin();
    while(delitr != readMsgLst.end())
    {
        m_needUpdateReadMap.remove((*delitr));
        ++delitr;
    }

    return bUpdate;
}

bool C_eIMEngine::isNeedUpdateGroup(QMID qmid)
{
    if(m_needUpdateReadMap.empty())
    {
        return false;
    }

    std::list<S_UpdateReadMsg>::iterator itr = m_needUpdateReadMap.begin();
    while(itr != m_needUpdateReadMap.end())
    {
        QMID itemMid = itr->qmid;
        if(itemMid == qmid)
        {
            return true;
        }
        ++itr;
    }
    return false;
}

void C_eIMEngine::SaveReadMsgNotice(S_ReadMsgNotice ReadMsgNotice)
{
    m_msgReadNoticeLst.push_back(ReadMsgNotice);
}

void C_eIMEngine::GetReadMsgNotice(QMID qmid,  std::list<S_ReadMsgNotice>& msgReadNoticeLst)
{
    if (m_msgReadNoticeLst.empty())
    {
        return;
    }
    std::list<S_ReadMsgNotice>::iterator itr = m_msgReadNoticeLst.begin();
    while(itr != m_msgReadNoticeLst.end())
    {
        S_ReadMsgNotice sData = (*itr);
        if (qmid == sData.dwMsgID)
        {
            msgReadNoticeLst.push_back(sData);
        }
        ++itr;
    }

    std::list<S_ReadMsgNotice>::iterator delitr = msgReadNoticeLst.begin();
    while(delitr != msgReadNoticeLst.end())
    {
        if (m_msgReadNoticeLst.empty())
        {
            break;
        }
        m_msgReadNoticeLst.remove((*delitr));
        ++delitr;
    }

}


// Download constacts files--end///////////////////////////////////////////////////////////////////
