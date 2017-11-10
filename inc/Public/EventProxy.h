// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: EventProxy for ATL control, the control MUST be 
//  implement OnEvent funcion the same as of I_EIMPlugin
// 
// 
// Auth: LongWQ
// Date: 2015/9/6 21:25:49
// 
// History: 
//    2015/9/6  LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_EVENT_PROXY_2015_9_6_BY_YFGZ__
#define __EIM_EVENT_PROXY_2015_9_6_BY_YFGZ__

#include "Utils.h"

// Event proxy 
template< class T >
class C_EventProxy: public I_EIMPlugin
{
private:
	T* m_pControl;

public:
	C_EventProxy(): m_pControl(NULL){}

	IMPLEMENT_NON_PLUGIN_(C_EventProxy);
	ULONG Release(){return 1;}	
	BOOL  InitPlugin(I_EIMEventMgr* pEventMgr, LPCTSTR lpszConfigFile ){return TRUE;}
	BOOL  GetPluginInfo(PS_PluginDllInfo pPluginInfo){return TRUE;}
	void  FreePlugin(){}

	BOOL SetControl(T* pCtrl)
	{
		m_pControl = pCtrl;
		return !!m_pControl;
	}

	inline BOOL RegisterEvent(DWORD dwEventID, E_EIMEventType eType)
	{
		ASSERT_(m_pControl);
		if ( m_pControl )
			return hpi.RegisterEvent(dwEventID, this, eType);

		return FALSE;
	}

	inline BOOL UnregisterEvent(DWORD dwEventID)
	{
		if ( m_pControl )
			return hpi.UnregisterEvent(dwEventID, this);

		return FALSE;
	}

	void OnEvent(unsigned int u32Event,void* lpvParam)
	{
		if (m_pControl) 
			return m_pControl->OnEvent(u32Event, lpvParam);
	}
};

#endif	// __EIM_EVENT_PROXY_2015_9_6_BY_YFGZ__

