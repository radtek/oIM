#pragma once
#include <vector>


class C_eIMEventWnd;
class C_EIMEventMgr: public I_EIMEventMgr
{
	DECALRE_PLUGIN_(C_EIMEventMgr)
	
	C_EIMEventMgr(void);

public:
	static C_EIMEventMgr& GetObject();
	~C_EIMEventMgr(void);	
	
	void SendQueueEvent(				// Send a event(like as function call, not post!)
		DWORD		dwEventID, 			// Event ID
		void*        ppvParam			// Parameter 
		);
	
public:
	virtual BOOL RegisterEvent(			// Register event to the event manager, succeed if return TRUE
		DWORD			dwEventID, 		// Event ID, User event id start from TPE_USER_EVENT
		I_EIMPlugin*	pPlugin,		// Plugin handle, which will receive or send registered event
		E_EIMEventType	eType			// Event type, see E_TEventType
		);

	virtual BOOL UnregisterEvent(		// Unregister event from event manager, succeed if return TRUE
		DWORD			dwEventID,		// Event ID 
		I_EIMPlugin*	pPlugin			// Plugin handle
		);

	virtual void SendEvent(				// Send a event(like as function call, not post!)
		DWORD		dwEventID, 		// Event ID
		void*        ppvParam			// Parameter 
		);

	virtual void PeekQueneMessage(BOOL bPeekQuene=TRUE);

protected:
	typedef struct tagEIMEVENT_TYPE_DATA
	{
		E_EIMEventType	eType;
		I_EIMPlugin*	pPlugin;
	}S_EIMEventTypeData, *PS_EIMEventTypeData;

	typedef std::vector< S_EIMEventTypeData >			VectPlugin;
	typedef std::vector< S_EIMEventTypeData >::iterator	VectPluginIt;
	
	// < EventId, VectPlugin >
	typedef std::map< DWORD, VectPlugin >				MapEvent;
	typedef std::map< DWORD, VectPlugin >::iterator		MapEventIt;

	CRITICAL_SECTION m_csEvent;			
	MapEvent		m_mapEvents;
	C_eIMEventWnd*	m_pwndDummy;
	BOOL			m_bLoopEvent;	//TRUE:Enable loop event, else disable
};

class C_eIMEventWnd
{
private:
	HWND			m_hWnd;
	BOOL			m_bLoopEvent;	//TRUE:Enable loop event, else disable
	I_EIMEventMgr*	m_pIEventMgr;

	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
public:
	C_eIMEventWnd(I_EIMEventMgr * pIEventMgr);

	BOOL	Create();
	BOOL	Destroy();
	BOOL	QueueEvent(DWORD dwEventID, void* ppvParam );
	void	SendQueueEvent(DWORD dwEventID, void* ppvParam);

};