#include "StdAfx.h"
#include "Public\Utils.h"
#include "TinyXml\tinyxml-lib.h"
#include "eIMPluginMgr.h"
#include "eIMEventMgr.h"
#include "eIMUICore\C_HelpAPI.h"

// Registered Plugin configuration file element and attribute
// Root Element: <eIMPlugins>
#define NODE_ROOT							"eIMPlugins"
#define NODE_ROOT_VERSION_ATTR				"Version"
// Plugin Element: <Plugin>
#define NODE_PLUGIN							"Plugin"
#define NODE_PLUGIN_DESC_ATTR				"Description"
#define NODE_PLUGIN_NAME_ATTR				"Name"
#define NODE_PLUGIN_IID_ATTR				"IID"
#define NODE_PLUGIN_CLASS_ATTR				"Class"
#define NODE_PLUGIN_CLASS_APPMAN			"AppMan"
#define NODE_PLUGIN_CLASS_MAIN				"Main"
#define NODE_PLUGIN_CLASS_SESSION			"Session"
#define NODE_PLUGIN_CLASS_SETTING			"Setting"
#define NODE_PLUGIN_CLASS_SESSION_APPMAN	"SessionAppMan"
#define NODE_PLUGIN_ICON_ATTR				"Icon"
#define NODE_PLUGIN_FILE_ATTR				"File"
#define NODE_PLUGIN_CONFIG_ATTR				"Configuration"

extern "C" __declspec(dllexport) int __stdcall eIMCreateInterface(const TCHAR* pctszIID, void** ppvIObject)
{
	if( pctszIID == NULL || ppvIObject == NULL )
		return E_POINTER;

	if ( _tcsnicmp(pctszIID, INAME_EIMPLUGINMGR, _tcslen(INAME_EIMPLUGINMGR)) == 0 )
	{
		*ppvIObject = new C_EIMPluginMgr;
		if( *ppvIObject == NULL )
			return EIMERR_OUT_OF_MEMORY;

		return EIMERR_NO_ERROR;
	}
	else if ( _tcsnicmp(pctszIID, INAME_EIMEVENTMGR, _tcslen(INAME_EIMEVENTMGR)) == 0 )
	{	// C_EIMEventMgr is singleton
		*ppvIObject = &C_EIMEventMgr::GetObject();
		return EIMERR_NO_ERROR;
	}

	return EIMERR_NOT_IMPL;
}


//IMPLEMENT_PLUGIN_REF_(C_EIMPluginMgr, INAME_EIMPLUGINMGR, m_dwRef);
C_EIMPluginMgr::C_EIMPluginMgr()
	: m_dwRef( 1 )
{
}

C_EIMPluginMgr::~C_EIMPluginMgr(void)
{
}

HRESULT C_EIMPluginMgr::QueryInterface(const TCHAR* pctszIID, void** ppvIObject) 
{ 
	if (pctszIID == NULL || ppvIObject == NULL ) 
		return EIMERR_INVALID_POINTER; 

	if( _tcsnicmp(pctszIID, INAME_EIMPLUGINMGR, _tcslen(INAME_EIMPLUGINMGR)) == 0 ) 
	{ 
		AddRef(); 
		*ppvIObject = this; 
		return EIMERR_NO_ERROR; 
	} 

	for ( VectPluginDataIt it = m_vectPluginData.begin(); it != m_vectPluginData.end(); it++ )
	{
		if ( it->pPlguin && SUCCEEDED(it->pPlguin->QueryInterface(pctszIID, ppvIObject)) )
			return EIMERR_NO_ERROR;
	}

	return EIMERR_NOT_IMPL; 
} 

ULONG C_EIMPluginMgr::AddRef(void) 
{ 
	return (ULONG)InterlockedIncrement(&m_dwRef); 
} 

ULONG C_EIMPluginMgr::Release( void ) 
{ 
	if ( m_dwRef == 0 ) 
	{ 
		ASSERT_(FALSE);
		return 0; 
	} 
	LONG i32Ref = InterlockedDecrement(&m_dwRef); 
	if ( i32Ref == 0 ) 
	{ 
		delete this; 
		return 0; 
	} 

	return i32Ref; 
}
//=============================================================================
//Function:     _GetPluginClass
//Description:	Convert the plugin class string to value
//
//Parameter:
//	pszClass    - Plugin class
//
//Return:
//     Plugin class value         
//=============================================================================
E_PluginClass C_EIMPluginMgr::_GetPluginClass(const char* pszClass)
{
	if ( pszClass == NULL || pszClass[0] == _T('\0') )
		return ePLUGIN_CLASS_NONE;

	if ( !_stricmp( NODE_PLUGIN_CLASS_APPMAN, pszClass) )
		return ePLUGIN_CLASS_APPMAN;

	if ( !_stricmp( NODE_PLUGIN_CLASS_MAIN, pszClass) )
		return ePLUGIN_CLASS_MAIN;

	if ( !_stricmp( NODE_PLUGIN_CLASS_SESSION, pszClass) )
		return ePLUGIN_CLASS_SESSION;

	if ( !_stricmp( NODE_PLUGIN_CLASS_SETTING, pszClass) )
		return ePLUGIN_CLASS_SETTING;

	if ( !_stricmp( NODE_PLUGIN_CLASS_SESSION_APPMAN, pszClass) )
		return ePLUGIN_CLASS_SESSION_APPMAN;

	return ePLUGIN_CLASS_NONE;
}

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
int C_EIMPluginMgr::LoadPlugins(				
	const TCHAR*	lpszPluginConfigFile
	)
{
#define GET_ATTR_STR_(VARS, NAME, SIZE) \
	{ \
		const TCHAR* pszAttr = pPluginElement->Attribute( NAME ); \
		if ( pszAttr ) \
			_tcsncpy(VARS, pszAttr, SIZE ); \
	}

	I_EIMLogger* pILog = hpi.UIGetLogger();
	TiXmlDocument xmlDoc;
	if ( !TiXmlLoadFile(lpszPluginConfigFile, xmlDoc))
	{
		eIMString szErrDesc;

		::eIMUTF8ToTChar(xmlDoc.ErrorDesc(), szErrDesc);
		EIMLOGGER_ERROR_( pILog, _T("Load plugin config file [%s] failed: ErrorId:%d(%d,%d) %s!"),
			lpszPluginConfigFile, xmlDoc.ErrorId(), xmlDoc.ErrorRow(), xmlDoc.ErrorCol(), szErrDesc.c_str() );
		return -1;
	}

	TiXmlElement* pRootElement = xmlDoc.RootElement();
	if ( pRootElement == NULL || strcmp(pRootElement->Value(), NODE_ROOT))
	{
		ASSERT_( FALSE );
		EIMLOGGER_ERROR_( pILog, _T("Invalid plugin configuration file [%s] failed!"), lpszPluginConfigFile );
		return -1;
	}

	const TCHAR*	pszAttr = NULL;
	eIMString		szPluginFile;
	eIMString		szPluginIID;
	eIMString		szConfig;
	eIMString		szValue;
	PI_EIMPlugin	pIEIMPlugin = NULL;
	S_EIMPluginData sPluginData;
	for ( TiXmlElement* pPluginElement = pRootElement->FirstChildElement();
		  pPluginElement; 
		  pPluginElement = pPluginElement->NextSiblingElement() )
	{
		::eIMUTF8ToTChar(pPluginElement->Attribute(NODE_PLUGIN_DESC_ATTR), szValue);
		C_PluginDll PluginDll( ePLUGIN_TYPE_PLUGIN, szValue.c_str(), FALSE );
		
		::eIMUTF8ToTChar(pPluginElement->Attribute( NODE_PLUGIN_FILE_ATTR ),  szPluginFile);
		::eIMUTF8ToTChar(pPluginElement->Attribute( NODE_PLUGIN_IID_ATTR ),   szPluginIID );
		::eIMUTF8ToTChar(pPluginElement->Attribute( NODE_PLUGIN_CONFIG_ATTR), szConfig );

		if ( !szConfig.empty() )
			::eIMReplaceToken(szConfig);

		if ( ::eIMReplaceToken(szPluginFile) > 0 && 
			 PluginDll.Load(szPluginFile.c_str()) &&
			 SUCCEEDED(PluginDll.eIMCreateInterface( szPluginIID.c_str(), (void**)&pIEIMPlugin)) &&
			 pIEIMPlugin->InitPlugin(&C_EIMEventMgr::GetObject(), szConfig.c_str()) )
		{
			memcpy( &sPluginData, PluginDll.GetPluginInfo(), sizeof(S_EIMPluginData) );

			sPluginData.hWnd		 = NULL;
			sPluginData.pPlguin		 = pIEIMPlugin;
			sPluginData.ePluginClass = _GetPluginClass( pPluginElement->Attribute(NODE_PLUGIN_CLASS_ATTR) );

			::eIMUTF8ToTChar(pPluginElement->Attribute(NODE_PLUGIN_NAME_ATTR), szValue);
			_tcsncpy(sPluginData.szPluginName, szValue.c_str(), EIM_PLUGIN_NAME_SIZE -1);

			::eIMUTF8ToTChar(pPluginElement->Attribute(NODE_PLUGIN_IID_ATTR), szValue);
			_tcsncpy(sPluginData.szPluginIID, szValue.c_str(), EIM_PLUGIN_IID_SIZE -1);

			::eIMUTF8ToTChar(pPluginElement->Attribute(NODE_PLUGIN_CONFIG_ATTR), szValue);
			_tcsncpy(sPluginData.szPluginConfig, szValue.c_str(), EIM_PLUGIN_CONFIG_SIZE -1);

			pPluginElement->Attribute( NODE_PLUGIN_ICON_ATTR, &sPluginData.i32Icon );
			m_vectPluginData.push_back( sPluginData );

			EIMLOGGER_INFO_( pILog, _T("Load plugin [%s] succeeded!"), szPluginFile.c_str() );
		}
		else
		{
			EIMLOGGER_ERROR_( pILog, _T("Load plugin [%s] failed!"), szPluginFile.c_str() );
		}
	}

	return m_vectPluginData.size();
}

//=============================================================================
//Function:     UnloadPlugins
//Description:	Unload all plugins
//
//Return:
//      None        
//=============================================================================
void C_EIMPluginMgr::UnloadPlugins()				
{
	I_EIMLogger* pILog = hpi.UIGetLogger();
	VectPluginDataIt it = m_vectPluginData.begin();
	for ( ; it != m_vectPluginData.end(); it++ )
	{
		if ( it->pPlguin )
			it->pPlguin->FreePlugin();

		C_PluginDll PluginDll( &(*it) );
		PluginDll.Unload();
	
		EIMLOGGER_INFO_( pILog, _T("Unloaded plugin [%s] succeeded!"), it->szPluginName );
	}

	m_vectPluginData.clear();
}

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
DWORD C_EIMPluginMgr::Enums(
	VectPlugins&	vectPlugins,		
	E_PluginClass	ePluginClass
	)
{
	vectPlugins.clear();
	VectPluginDataIt it = m_vectPluginData.begin();
	for ( ; it != m_vectPluginData.end(); it++ )
	{
		if ( it->pPlguin && (ePluginClass & it->ePluginClass) )
		{
			vectPlugins.push_back( it->pPlguin );
		}
	}

	return vectPlugins.size();
}

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
HWND C_EIMPluginMgr::GetParrentWnd(I_EIMPlugin* pPlugin, bool bCreateTemplateDlg)
{
	//PS_EIMPluginData pPlugin = FindPlugin(lpszPluginFile);
	//if(pPlugin)
	//	return pPlugin->hWnd;

	return NULL;
}

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
BOOL C_EIMPluginMgr::DllPreTranslateMessage(MSG* pMsg)
{
	//std::vector<PS_EIMPluginData>::iterator it= m_vectPluginData.begin();
	//for(; it != m_vectPluginData.end(); it++)
	//{
	////	if((*it)->pfnDllPreTranslateMessage && (*it)->pfnDllPreTranslateMessage(pMsg))
	////		return TRUE;
	//}

	return FALSE;
}

