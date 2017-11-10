#include "StdAfx.h"
#include "eIMEventMgr.h"
#include "Public\Utils.h"

#define WM_QUEUE_EVENT	(WM_USER + 0x3FF)	// Define the queuer event(at the last user event)


IMPLEMENT_PLUGIN_SINGLETON_(C_EIMEventMgr, INAME_EIMEVENTMGR);
C_EIMEventMgr::C_EIMEventMgr(void)
	: m_pwndDummy( NULL )
	, m_bLoopEvent( TRUE )
{
	InitializeCriticalSectionAndSpinCount( &m_csEvent,  9999 );
}

C_EIMEventMgr::~C_EIMEventMgr(void)
{
	if ( m_pwndDummy )
	{
		m_pwndDummy->Destroy();
		delete m_pwndDummy;
		m_pwndDummy = NULL;
	}

	Release();
	DeleteCriticalSection( &m_csEvent );
}

C_EIMEventMgr& C_EIMEventMgr::GetObject()
{
	static C_EIMEventMgr obj;
	if ( obj.m_pwndDummy == NULL )
	{
		obj.m_pwndDummy = new C_eIMEventWnd(&obj);
		if ( obj.m_pwndDummy )
			obj.m_pwndDummy->Create();
	}

	return obj;
}


//=============================================================================
//Function:     RegisterEvent
//Description:	Register event to the event manager,
//
//Parameter:
//	dwEventID    - Event ID, User event id start from TPE_USER_EVENT
//	pPlugin      - Plugin handle, which will receive or send registered event
//	eType        - Event type, see E_TEventType
//
//Return:
//      TRUE	succeed, else
//		FALSE
//=============================================================================
BOOL C_EIMEventMgr::RegisterEvent(
		DWORD			dwEventID,
		I_EIMPlugin*	pPlugin,
		E_EIMEventType	eType
		)
{
	ASSERT_( pPlugin );
	if ( pPlugin == NULL )
		return FALSE;

	C_EIMAutoLock al(m_csEvent);
	S_EIMEventTypeData	sETData = {eType, pPlugin };
	MapEventIt			it = m_mapEvents.find( dwEventID );
	if ( it == m_mapEvents.end() )
	{
		VectPlugin	vectPlugin;
	
		vectPlugin.push_back( sETData );
		m_mapEvents[dwEventID] = vectPlugin;
	}
	else
	{
		for (VectPluginIt it_ = it->second.begin(); it_ != it->second.end(); it_++ )
		{
			if ( it_->pPlugin == pPlugin )
				return TRUE;
		}

		it->second.push_back( sETData );
	}

	return TRUE;
}

//=============================================================================
//Function:     UnregisterEvent
//Description:	Unregister event from event manager
//
//Parameter:
//	dwEventID    - Event ID 
//	pPlugin      - Plugin handle
//
//Return:
//      TRUE	succeed, else
//		FALSE
//=============================================================================
BOOL C_EIMEventMgr::UnregisterEvent(
		DWORD			dwEventID,
		I_EIMPlugin*	pPlugin	
		)
{
	ASSERT_( pPlugin );
		
	C_EIMAutoLock al(m_csEvent);
	MapEventIt it = m_mapEvents.find( dwEventID );
	if ( pPlugin == NULL || it == m_mapEvents.end() )
	{
		TRACE_(_T("Event[%d] not exist"), dwEventID);
		return FALSE;
	}

	for (VectPluginIt it_ = it->second.begin(); it_ != it->second.end(); it_++ )
	{
		if ( it_->pPlugin == pPlugin )
		{
			it->second.erase( it_ );
			if (it->second.size() == 0)
				m_mapEvents.erase(it);

			return TRUE;
		}
	}

	return FALSE;
}

//=============================================================================
//Function:     SendEvent
//Description:	 Send a event(like as function call, not post!)
//
//Parameter:
//	dwEventID    - Event ID
//	ppvParam     - Parameter of event 
//
//Return:
//              None
//=============================================================================
void C_EIMEventMgr::SendEvent(
		DWORD	dwEventID,
		void*	ppvParam
		)
{
	if ( dwEventID != EVENT_EVENT_LOOP )
	{
		if ( m_bLoopEvent == FALSE )
		{
			TRACE_(_T("Event loop already disabled, EventId:%u, Param:0x%08X"), dwEventID, ppvParam);
			return;
		}

		CHECK_NULL_RETV_( m_pwndDummy );
		m_pwndDummy->QueueEvent(dwEventID, ppvParam);
		return;
	}

	switch((DWORD)ppvParam)
	{
	case EVENT_LOOP_DISABLE:
		m_bLoopEvent = FALSE;
		if ( m_pwndDummy )
			m_pwndDummy->QueueEvent(dwEventID, ppvParam);
		TRACE_(_T("Disable event loop"));
		break;
	case EVENT_LOOP_ENABLE:
		m_bLoopEvent = TRUE;
		if ( m_pwndDummy )
			m_pwndDummy->QueueEvent(dwEventID, ppvParam);

		TRACE_(_T("Enable event loop"));
		break;
	case EVENT_LOOP_PEEKQUENE:
		PeekQueneMessage(TRUE);
		break;
	case EVENT_LOOP_PEEKMESSAGE:
		PeekQueneMessage(FALSE);
		break;
	default:
		break;
	}
}

void C_EIMEventMgr::SendQueueEvent(DWORD dwEventID, void* ppvParam )
{
	MapEventIt it = m_mapEvents.find( dwEventID );
	if ( it == m_mapEvents.end() )
	{
	//	ASSERT_(FALSE);
		return;
	}

	for ( VectPluginIt it_ = it->second.begin(); it_ != it->second.end(); it_++ )
	{
		ASSERT_( it_->pPlugin );
		if ( it_->pPlugin && (( it_->eType == eIMET_RECEIVE ) || ( it_->eType == eIMET_RECEIVE_SEND )) )
		{
			try
			{
				it_->pPlugin->OnEvent( dwEventID, ppvParam );
			}
			catch(...)
			{
				TRACE_(_T("Occur except"));
			}
		}
	}
}


void C_EIMEventMgr::PeekQueneMessage(BOOL bPeekQuene)
{
//	TRACE_(_T("Peek event quene message[%d] begin"), bPeekQuene);
	UINT wMsgFilterMin = bPeekQuene ? WM_QUEUE_EVENT : 0;
	UINT wMsgFilterMax = bPeekQuene ? WM_QUEUE_EVENT : 0;
	MSG msg = { 0 };									
	while(::PeekMessage(&msg, NULL, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
	{
		if(msg.message == WM_QUIT)	
			break;

		::DispatchMessage(&msg);
	}	

	if(msg.message == WM_QUIT)	
		::PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
	
//	TRACE_(_T("Peek event quene message[%d] end"), bPeekQuene);
}


//C_eIMEventWnd////////////////////////////////////////////////////////////////

static HINSTANCE	g_hInst = ::GetModuleHandle(NULL);
static const TCHAR	m_szWindowClass[32] = _T("eIMEventWnd");

C_eIMEventWnd::C_eIMEventWnd(I_EIMEventMgr * pIEventMgr)
	: m_hWnd( NULL )
	, m_bLoopEvent( TRUE )
{
	ASSERT_( pIEventMgr != NULL );
	if ( pIEventMgr )
	{
		pIEventMgr->AddRef();
		m_pIEventMgr = pIEventMgr;
	}
}

BOOL C_eIMEventWnd::Create()
{
	ASSERT_( m_hWnd == NULL );

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), 0 };
	if ( !GetClassInfoEx(g_hInst, m_szWindowClass, &wcex) )
	{	// Check and register the dummy window
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)C_eIMEventWnd::WndProc;
		wcex.hInstance		= g_hInst;
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszClassName	= m_szWindowClass;

		if( !RegisterClassEx(&wcex) )
		{
			TRACE_( _T("Error: register class '%s' is failed!"), m_szWindowClass );
			return FALSE;
		}
	}

	m_hWnd = ::CreateWindow(m_szWindowClass, NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL, g_hInst, NULL);
	if( m_hWnd == NULL )
	{
		TRACE_(_T("Error: event queuer window create failed!") );
		return FALSE;
	}

	SetWindowLongPtr( m_hWnd, GWLP_USERDATA, (LONG_PTR)this );
	m_bLoopEvent = TRUE;
	return TRUE;
}

BOOL C_eIMEventWnd::Destroy()
{
	m_bLoopEvent = FALSE;
	ASSERT_( m_hWnd != NULL );
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);

		if ( !GetClassLong(m_hWnd, GCW_ATOM) )
			UnregisterClass( m_szWindowClass, g_hInst );

		m_hWnd = NULL;
	}

	SAFE_RELEASE_INTERFACE_(m_pIEventMgr);
	return TRUE;
}

BOOL C_eIMEventWnd::QueueEvent( DWORD dwEventID, void* ppvParam )
{
	if ( m_bLoopEvent )
	{
		ASSERT_( m_hWnd && ::IsWindow(m_hWnd) );
		CHECK_NULL_RET_( m_hWnd, FALSE );
		if ( ::IsWindow(m_hWnd) )
			return SUCCEEDED( SendMessage(m_hWnd, WM_QUEUE_EVENT, dwEventID, (LPARAM)ppvParam) );
	}

	TRACE_(_T("Event window handle is invalid, hWmd:%p, EventId:%u, Param:0x%08X"), m_hWnd, dwEventID, ppvParam);
	return FALSE;
}

void C_eIMEventWnd::SendQueueEvent(DWORD dwEventID, void* ppvParam)
{
	C_EIMEventMgr* pEntMgr = static_cast<C_EIMEventMgr*>(m_pIEventMgr);
	CHECK_NULL_RETV_( pEntMgr );
	if ( dwEventID == EVENT_EVENT_LOOP )
	{
		if ((DWORD)ppvParam == EVENT_LOOP_DISABLE)
			m_bLoopEvent = FALSE;
		else if ((DWORD)ppvParam == EVENT_LOOP_ENABLE)
			m_bLoopEvent = TRUE;
	}

	if ( m_bLoopEvent )
		pEntMgr->SendQueueEvent(dwEventID, ppvParam);
}

LRESULT CALLBACK C_eIMEventWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	C_eIMEventWnd* pThis = NULL;

	switch( message ) 
	{
	case WM_QUEUE_EVENT:
		pThis = reinterpret_cast<C_eIMEventWnd *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		ASSERT_( pThis != NULL );
		CHECK_NULL_RET_( pThis, EIMERR_INVALID_POINTER );
		pThis->SendQueueEvent(wParam, (void*)lParam);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}


