// Description: Plugin interface defined and helper class to use the plugin.
//   In oIM projects the DLL is implemented as NORMAL(PLUGIN_TYPE_NORMAL) 
//   function library that inherit from I_Unknown, or PLUGIN(PLUGIN_TYPE_PLUGIN)
//   library that inherit from I_Plugin that can be fire or response event.
//   It can not be registered when only fire the event by SEND(ET_SEND), 
//   but RECEIVE(ET_RECEIVE/ET_RECEIVE_SEND) event must be registered 
//   to response the event.
//
// Each DLL in oIM projects MUST be implemented the standard API of CreateInterface
// 
// DECALRE_PLUGIN_			Used to simplify the declare of base interface function of I_Unknown
// IMPLEMENT_NON_PLUGIN_	Used to simplify the implemented the interface function 
//
// C_PluginDll				Helper to use the DLL
// C_ComDll					Helper to use the COM
//
// Auth: LongWQ
// Date: 2013/12/21 18:49:00
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __PLUGIN_API_2013_10_23_BY_YFGZ__
#define __PLUGIN_API_2013_10_23_BY_YFGZ__

#include <windows.h>
#include <tchar.h>
#include <map>
#include <list>
#include <vector>
#include <utility>
#include <string>
#include <utility>
#include <algorithm>
#include "utils.h"

#define INAME_PLUGINMGR		_T("SXIT.PluginMgr.PluginMgr")	// I_PluginMgr interface name
#define INAME_EVENTMGR		_T("SXIT.PluginMgr.EventMgr")	// I_EventMgr interface name

#pragma pack( push, 8 )
///////////////////////////////////////////////////////////////////////////////
#define TOKEN_SIZE					( 32 + 1 )	// Token size, include NULL

#define PLUGIN_DESC_SIZE		( 64 )		// Plugin description size
#define PLUGIN_NAME_SIZE		( 64 )		// Plugin name size
#define PLUGIN_IID_SIZE			( 64 )		// Plugin IID size
#define PLUGIN_FILE_SIZE		( MAX_PATH )// Plugin's DLL file path size
#define PLUGIN_CONFIG_SIZE		( MAX_PATH )// Plugin's configuration file path size


template<class I>
class C_AutoRelease
{
	I * m_pI;
public:
	typedef I type;
	C_AutoRelease(I *pI) : m_pI(pI) {}
	~C_AutoRelease() { if(m_pI) m_pI->Release(); }
	operator I*() const { return m_pI; }
};

// define a Help Macro to auto release interface
#define AUTO_RELEASE_(ptrUnk) C_AutoRelease<I_Unknown> __auto_rel_##ptrUnk(ptrUnk)

// define the standard API function pointer type
typedef int (__stdcall* PFN_CreateInterface)(const TCHAR* pctszIID, void** ppvIObject);

// Help Macro function for Non-Plugin or no sub-interface to be create
#define IMPLEMENT_NON_PLUGIN_(clsname) \
	HRESULT clsname::QueryInterface(const TCHAR* pctszIID, void** ppvIObject) \
	{ \
		return E_NOTIMPL; \
	} \
	ULONG clsname::AddRef(void) \
	{ \
		return 1; \
	}

// Help Macro function for Plugin as reference counter
#define IMPLEMENT_PLUGIN_REF_(clsname, clsid, ref) \
	HRESULT clsname::QueryInterface(const TCHAR* pctszIID, void** ppvIObject) \
	{ \
		if (pctszIID == NULL || ppvIObject == NULL ) \
			return ERR_INVALID_POINTER; \
		\
		if( _tcsnicmp(pctszIID, clsid, _tcslen(clsid)) == 0 ) \
		{ \
			AddRef(); \
			*ppvIObject = this; \
			return ERR_NO_ERROR; \
		} \
		\
		return ERR_NOT_IMPL; \
	} \
	ULONG clsname::AddRef(void) \
	{ \
		return (ULONG)InterlockedIncrement(&ref); \
	} \
	ULONG clsname::Release( void ) \
	{ \
		if ( ref == 0 ) \
		{ \
			return 0; \
		} \
		LONG i32Ref = InterlockedDecrement(&ref); \
		if ( i32Ref == 0 ) \
		{ \
			delete this; \
			return 0; \
		} \
		\
		return i32Ref; \
	}

// Help Macro function for Plugin as singleton
#define IMPLEMENT_PLUGIN_SINGLETON_(clsname, clsid) \
	HRESULT clsname::QueryInterface(const TCHAR* pctszIID, void** ppvIObject) \
	{ \
		if( _tcsnicmp(pctszIID, clsid, _countof(clsid)) == 0) \
		{ \
			AddRef(); \
			*ppvIObject = this; \
			return ERR_NO_ERROR; \
		} \
		\
		return ERR_NOT_IMPL; \
	} \
	ULONG clsname::AddRef(void) \
	{ \
		return 1; \
	} \
	ULONG clsname::Release(void) \
	{ \
		return 1; \
	}

// Help Macro function decare the standard plugin interface
#define DECALRE_PLUGIN_(clsname) \
	public: \
	virtual HRESULT QueryInterface(const TCHAR* pctszIID, void** ppvIObject); \
	virtual ULONG	AddRef(void); \
	virtual ULONG	Release(void);	

///////////////////////////////////////////////////////////////////////////////
class __declspec(novtable) I_Unknown
{
public:
	//=============================================================================
	//Function:     QueryInterface
	//Description:	Create the interface 
	//
	//Parameter:
	//	pctszIID		- The interface name string to be query, is GUID string of COM,
	//					  or function name string, format as: "Company.Module[.Function]"
	//	ppvIObject		- A pointer to the interface pointer identified by pctszIID. If the 
	//					  object does not support this interface, ppv is set to NULL. 
	//
	//Return:
	//	S_OK			- Returns a pointer to the current interface if successful
	//	E_NOINTERFACE	- otherwise.
	//=============================================================================
	virtual HRESULT QueryInterface(	// Create sub-interface
		const TCHAR* pctszIID, 		
		void** ppvIObject
		) = 0;

	//=============================================================================
	//Function:     AddRef
	//Description:	increments the reference count for an interface on an object. 
	//				It should be called for every new copy of a pointer to an interface on a given object. 
	//
	//Return:
	//	Returns an integer from 1 to n, the value of the new reference count.       
	//=============================================================================
	virtual ULONG	AddRef(void) = 0;

	//=============================================================================
	//Function:     Release
	//Description:	Decrements the reference count for the calling interface on a object. 
	//				If the reference count on the object falls to 0, the object is freed from memory.
	//
	//Return:
	//    Returns the resulting value of the reference count          
	//=============================================================================
	virtual ULONG	Release(void) = 0;	
};
typedef I_Unknown*		PI_Unknown;
typedef const I_Unknown*	PI_Unknown_;

///////////////////////////////////////////////////////////////////////////////
#ifndef GUID_DEFINED
	#define GUID_DEFINED
	typedef struct _GUID {
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[ 8 ];
	} GUID;
#endif

// Tool plugin event type
typedef enum tagEventType
{
	ET_RECEIVE 		= 1,				// Only Receive event
	ET_SEND			= 2, 				// Only Send event
	ET_RECEIVE_SEND	= 4,				// Send and Receive event
}E_EventType, *PE_EventType;
typedef tagEventType*		PE_EventType;
typedef const tagEventType*	PE_EventType_;

class I_PluginMgr;
class I_EventMgr;
class I_Plugin;

// oIM plugin event manager interface
class I_EventMgr: public I_Unknown
{
public:
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
	virtual BOOL RegisterEvent(
		DWORD		dwEventID,
		I_Plugin*	pPlugin,
		E_EventType	eType
		) = 0;

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
	virtual BOOL UnregisterEvent(
		DWORD		dwEventID,
		I_Plugin*	pPlugin
		) = 0;

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
	virtual void SendEvent(
		DWORD			dwEventID,
		void*			ppvParam
		) = 0;

//	virtual void PeekQueneMessage(BOOL bPeekQuene=TRUE) = 0;
};
typedef I_EventMgr*			PI_EventMgr;
typedef const I_EventMgr*	PI_EventMgr_;


typedef enum tagPluginClass		// Define the plugin Class
{
	ePLUGIN_CLASS_NONE			 = 0x0000,		// Unknown
	ePLUGIN_CLASS_APPMAN		 = 0x0001,		// Little application class
	ePLUGIN_CLASS_MAIN			 = 0x0002,		// Main window tab's class
	ePLUGIN_CLASS_SESSION		 = 0x0004,		// Session class
	ePLUGIN_CLASS_SETTING		 = 0x0008,		// Setting class
	ePLUGIN_CLASS_SESSION_APPMAN = ( ePLUGIN_CLASS_APPMAN | ePLUGIN_CLASS_SESSION ),		// Little application and session class
	ePLUGIN_CLASS_ALL			 = ( ePLUGIN_CLASS_APPMAN | ePLUGIN_CLASS_MAIN | ePLUGIN_CLASS_SESSION | ePLUGIN_CLASS_SETTING | ePLUGIN_CLASS_SESSION_APPMAN ),		// All plugin class
}E_PluginClass, *PE_PluginClass;

typedef std::vector< I_Plugin* >			VectPlugins;	// for Enum Plugins
typedef std::vector< I_Plugin* >::iterator	VectPluginsIt;	// for Enum Plugins

// C_PluginDll is a helper class for register PLUGIN library and operator the standard API
typedef enum tagPluginType		// Define the plugin type
{
	ePLUGIN_TYPE_NONE	= 0x0000,		// Unknown type
	ePLUGIN_TYPE_NORMAL	= 0x0001,		// Nomral function DLL, implementation interface I_Unknown, eg.: ClientAgent, RUDP, Logger
	ePLUGIN_TYPE_PLUGIN	= 0x0002,		// Plugin DLL, implementation interface I_Plugin
}E_PluginType, *PE_PluginType;
typedef const tagPluginType* PE_PluginType_;

typedef struct tagPluginDllInfo
{
	DWORD				dwSize;					// Size of this struct
	E_PluginType		ePluginType;			// Type of plugin
	E_PluginClass		ePluginClass;			// Class of plugin
	HMODULE				hmodPluginDll;			// Handle of plugin
	PFN_CreateInterface	pfnCreateInterface;		// API 
	int		i32Icon;							// Icon
	TCHAR	szPluginIID[PLUGIN_IID_SIZE];		// IID
	TCHAR	szPluginDesc[PLUGIN_DESC_SIZE];		// Description
	TCHAR	szPluginName[PLUGIN_NAME_SIZE];		// Name
	TCHAR	szPluginDll[PLUGIN_FILE_SIZE];		// Plugin DLL file 
	TCHAR	szPluginConfig[PLUGIN_CONFIG_SIZE];	// Plugin configuration file 
}S_PluginDllInfo, *PS_PluginDllInfo;
typedef const tagPluginDllInfo* PS_PluginDllInfo_;

class I_Logger;
// Plugin manager interface
class I_PluginMgr: public I_Unknown
{
public:
	//=============================================================================
	//Function:     LoadPlugins
	//Description:	Load all plugins that registered in configuration file
	//
	//Parameter:
	//	lpszPluginConfigFile    - Plugin Registered configuration file
	//
	//Return:
	//		-1		Load the configuration failed
	//		>=0		The count of loaded plugin
	//=============================================================================
	virtual int	LoadPlugins(				
		const TCHAR*	lpszPluginConfigFile
		) = 0;

	//=============================================================================
	//Function:     UnloadPlugins
	//Description:	Unload all plugins
	//
	//Return:
	//      None        
	//=============================================================================
	virtual void UnloadPlugins() = 0;		


	//=============================================================================
	//Function:     GetParrentWnd
	//Description:	Get the parrent HWND
	//
	//Parameter:
	//	pPlugin               - Plugin interface pointer
	//	bCreateTemplateDlg    - Reserved
	//
	//Return:
	//    The parrent HWND          
	//=============================================================================
	virtual HWND GetParrentWnd(
		I_Plugin*	pPlugin,			
		bool		bCreateTemplateDlg	
		) = 0;	

	//=============================================================================
	//Function:     Enums
	//Description:	Enumerate the specified type plugin
	//
	//Parameter:
	//	vectPlugins     - Return enumerated result
	//	ePluginClass    - Class of plugin to beenumerated
	//
	//Return:
	//     The enumerated count         
	//=============================================================================
	virtual DWORD Enums(
		VectPlugins&	vectPlugins,		
		E_PluginClass	ePluginClass
		) = 0;

	//=============================================================================
	//Function:     DllPreTranslateMessage
	//Description:	Route PreTranslateMessage
	//
	//Parameter:
	//	pMsg    - Windows message
	//
	//Return:
	//		TRUE	if the message was translated and should not be dispatched;
	//		FALSE	if the message was not translated and should be dispatched.        
	//=============================================================================
	virtual BOOL DllPreTranslateMessage(
		MSG* pMsg
		) = 0;	
};
typedef I_PluginMgr*			PI_PluginMgr;
typedef const I_PluginMgr*	PI_PluginMgr_;

// Tool plugin interface
class I_Plugin: public I_Unknown
{
public:
	//=============================================================================
	//Function:     InitPlugin
	//Description:	Initial plugin
	//
	//Parameter:
	//	pPluginMgr    	- Tool manager interface
	//  pEventMgr		- Event manager interface
	//	lpszConfigFile	- Config file path, for plugin[set] manager only now
	//Return:
	//		TRUE	- Succeeded
	//		FALSE	- Failed 
	//=============================================================================
	virtual BOOL InitPlugin(
//Del 	I_PluginMgr*pPluginMgr,	//Single line deleted by lwq on 2013/11/26 9:03:54
		I_EventMgr*	pEventMgr,
		LPCTSTR		lpszConfigFile
		) = 0;
		
	//=============================================================================
	//Function:     OnEvent
	//Description:	Event response function
	//
	//Parameter:
	//	u32Event    	- Event Id
	//  lpvParam		- Event parameter
	//Return:
	//	None
	//=============================================================================
	virtual void OnEvent(
		unsigned int		u32Event,
		void*				lpvParam
		) = 0;
	
	//=============================================================================
	//Function:     GetPluginInfo
	//Description:	Get plugin information
	//
	//Parameter:
	//	pPluginInfo    	- Return plugin information
	//   
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL GetPluginInfo(
		PS_PluginDllInfo	pPluginInfo	
		) = 0;
		
	//=============================================================================
	//Function:     FreePlugin
	//Description:	Free plugin
	//
	//=============================================================================
	virtual void FreePlugin() = 0;		// Free plugin
};
typedef I_Plugin*		PI_Plugin;
typedef const I_Plugin*	PI_Plugin_;


class C_PluginDll
{
private:
	BOOL			m_bAutoUnload;
	S_PluginDllInfo m_sPluginDllInfo;
	C_PluginDll() : m_bAutoUnload(TRUE)
	{}

public:
	C_PluginDll(S_PluginDllInfo* psPluginDllInfo) 
		: m_bAutoUnload( TRUE )
	{
		if ( psPluginDllInfo )
			memcpy( &m_sPluginDllInfo, psPluginDllInfo, sizeof(S_PluginDllInfo) );
		else
			memset( &m_sPluginDllInfo, 0, sizeof(S_PluginDllInfo) );
	}

	C_PluginDll(E_PluginType ePluginType, const TCHAR* pszDescription, BOOL bAutoUnload = TRUE) 
		: m_bAutoUnload( bAutoUnload )
	{
		memset( &m_sPluginDllInfo, 0, sizeof(m_sPluginDllInfo));
		m_sPluginDllInfo.ePluginType = ePluginType;
		if( pszDescription )
			_tcsncpy(m_sPluginDllInfo.szPluginDesc, pszDescription, PLUGIN_DESC_SIZE);
	}

	~C_PluginDll()
	{
		if ( m_bAutoUnload )
			Unload();
	}

	//=============================================================================
	//Function:     GetPluginInfo
	//Description:	Get the loaded plugin information
	//
	//Return:
	//	Return plugin information
	//=============================================================================
	const PS_PluginDllInfo GetPluginInfo()
	{
		return &m_sPluginDllInfo;
	}

	//=============================================================================
	//Function:     Load
	//Description:	Load the plugin
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	BOOL Load(LPCTSTR lpszPluginDll)
	{
		if ( lpszPluginDll == NULL || lpszPluginDll[0] == _T('\0') )
		{
			SASSERT( FALSE );
			return FALSE;
		}

		_tcsncpy(m_sPluginDllInfo.szPluginDll, lpszPluginDll, MAX_PATH);

		if ( m_sPluginDllInfo.hmodPluginDll == NULL )
		{
			m_sPluginDllInfo.hmodPluginDll = LoadLibrary(m_sPluginDllInfo.szPluginDll);
			if ( !m_sPluginDllInfo.hmodPluginDll )
			{
				STRACE( _T("Load DLL [%s] failed!"), lpszPluginDll);
				return FALSE;
			}
		}

		if ( m_sPluginDllInfo.pfnCreateInterface == NULL )
		{
			m_sPluginDllInfo.pfnCreateInterface = (PFN_CreateInterface)GetProcAddress(m_sPluginDllInfo.hmodPluginDll, "CreateInterface");
			if ( !m_sPluginDllInfo.pfnCreateInterface )
			{
				STRACE( _T("Query CreateInterface API failed of [%s]!"), lpszPluginDll );
				return FALSE;
			}
		}

		return TRUE;
	}

	//=============================================================================
	//Function:     Unload
	//Description:	Unloadoad the plugin
	//
	//=============================================================================
	void Unload()
	{
		if(m_sPluginDllInfo.hmodPluginDll)
		{
			try{
				FreeLibrary(m_sPluginDllInfo.hmodPluginDll);
			}catch(...){
				STRACE(_T("*****Unload plugin '%s' occur exception"), m_sPluginDllInfo.szPluginDll);
			}
		}

		m_sPluginDllInfo.hmodPluginDll			= NULL;
		m_sPluginDllInfo.pfnCreateInterface	= NULL;
	}
	//=============================================================================
	//Function:     CreateInterface
	//Description:	Create the specified interface.
	//
	//Parameter:
	//	pctszIID    - ID will to be created interface, maybe GUID when is COM, or
	//				  function tag, format: "Company.Module[.Function]"��eg.: "SXIT.RUDP.P2P"
	//  ppvIObject  - Return the the object interface pointer.
	//Return:
	//��S_OK			Succeeded
 	//	E_INVALIDARG 	Parameter error
 	//	E_POINTER		Pointer error
 	//	E_OUTOFMEMORY	No memory 
	//=============================================================================
	int CreateInterface( const TCHAR* pctszIID, void** ppvIObject )
	{
		if ( ppvIObject == NULL )
			return ERR_INVALID_POINTER;

		*ppvIObject = NULL;
		int i32Ret = ERR_NO_ERROR;
		if( m_sPluginDllInfo.pfnCreateInterface )
		{
			i32Ret = m_sPluginDllInfo.pfnCreateInterface(pctszIID, ppvIObject);
			STRACE(_T("Create interface [%s] from [%s] %s"), pctszIID, m_sPluginDllInfo.szPluginDll, SUCCEEDED(i32Ret) ? _T("succeed") : _T("failed"));
			return i32Ret;
		}

		STRACE(_T("Create interface [%s] failed that CreateInterface is invalid"), pctszIID);
		return ERROR_DLL_INIT_FAILED;
	}
};

// C_ComDll is a helper class for Register COM Library and operator the standard API
class C_ComDll
{
private:
	typedef HRESULT (__stdcall* PFN_DllRegisterServer	)(void);
	typedef HRESULT (__stdcall* PFN_DllUnregisterServer	)(void);
	typedef HRESULT (__stdcall* PFN_DllCanUnloadNow		)(void);
	typedef HRESULT (__stdcall* PFN_DllGetClassObject	)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

	TCHAR						m_szComDll[MAX_PATH + 1];
	HMODULE						m_hmodComDll;

	PFN_DllRegisterServer		m_pfnDllRegisterServer;
	PFN_DllUnregisterServer		m_pfnDllUnregisterServer;
	PFN_DllCanUnloadNow			m_pfnDllCanUnloadNow;
	PFN_DllGetClassObject		m_pfnDllGetClassObject;

public:
	C_ComDll()
		: m_hmodComDll(NULL)
		, m_pfnDllRegisterServer(NULL)
		, m_pfnDllUnregisterServer(NULL)
		, m_pfnDllCanUnloadNow(NULL)
		, m_pfnDllGetClassObject(NULL)
	{
		m_szComDll[0] = _T('\0');
	}

	//=============================================================================
	//Function:     Load
	//Description:	Load the plugin
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	BOOL Load(LPCTSTR lpszComDll)
	{
		if(lpszComDll == NULL || lpszComDll[0] == _T('\0'))
			return FALSE;

		memset(m_szComDll, 0, sizeof(m_szComDll));
		_tcsncpy(m_szComDll, lpszComDll, MAX_PATH);

		m_hmodComDll = LoadLibrary(m_szComDll);
		if(!m_hmodComDll)
		{
			STRACE(_T("LoadLibrary: %s failed\n"), m_szComDll);
			return FALSE;
		}

		m_pfnDllRegisterServer		 = (PFN_DllRegisterServer	)GetProcAddress(m_hmodComDll, "DllRegisterServer"	);
		m_pfnDllUnregisterServer	 = (PFN_DllUnregisterServer	)GetProcAddress(m_hmodComDll, "DllUnregisterServer"	);
		m_pfnDllCanUnloadNow		 = (PFN_DllCanUnloadNow		)GetProcAddress(m_hmodComDll, "DllCanUnloadNow"		);
		m_pfnDllGetClassObject		 = (PFN_DllGetClassObject	)GetProcAddress(m_hmodComDll, "DllGetClassObject"	);

		return (m_pfnDllRegisterServer	&&
				m_pfnDllUnregisterServer &&
				m_pfnDllCanUnloadNow &&
				m_pfnDllGetClassObject);
	}

	//=============================================================================
	//Function:     Unload
	//Description:	Unloadoad the plugin
	//
	//=============================================================================
	void Unload()
	{
		if(m_hmodComDll)
			FreeLibrary(m_hmodComDll);

		m_hmodComDll				 = NULL;

		m_pfnDllRegisterServer		 = NULL;
		m_pfnDllUnregisterServer	 = NULL;
		m_pfnDllCanUnloadNow		 = NULL;
		m_pfnDllGetClassObject		 = NULL;
	}
	
	//=============================================================================
	//Function:     DllRegisterServer
	//Description:	Register the COM 
	//
	//=============================================================================
	HRESULT DllRegisterServer(void)
	{
		if(m_pfnDllRegisterServer)
			return m_pfnDllRegisterServer();

		return ERROR_DLL_INIT_FAILED;
	}

	//=============================================================================
	//Function:     DllUnregisterServer
	//Description:	Unregister the COM
	//
	//=============================================================================
	HRESULT DllUnregisterServer(void)
	{
		if( m_pfnDllUnregisterServer )
			return m_pfnDllUnregisterServer();
		
		return ERROR_DLL_INIT_FAILED;
	}

	//=============================================================================
	//Function:     DllCanUnloadNow
	//Description:	Check whether the COM can unload now
	//
	//=============================================================================
	HRESULT DllCanUnloadNow(void)
	{
		if( m_pfnDllCanUnloadNow )
			return m_pfnDllCanUnloadNow();

		return ERROR_DLL_INIT_FAILED;
	}

	//=============================================================================
	//Function:     DllGetClassObject
	//Description:	Get the class object
	//
	//Parameter:
	//	rclsid  - ClassId
	//  riid  	- REFIID
	//	ppv		- Return the object
	//Return:
	//  HRESULT
	//=============================================================================
	HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
	{
		if ( ppv == NULL )
			return ERR_INVALID_POINTER;

		*ppv = NULL;
		if( m_pfnDllGetClassObject )
			return m_pfnDllGetClassObject(rclsid, riid, ppv);

		return ERROR_DLL_INIT_FAILED;
	}
};

#pragma pack( pop )

#endif	// __PLUGIN_API_2013_10_23_BY_YFGZ__

