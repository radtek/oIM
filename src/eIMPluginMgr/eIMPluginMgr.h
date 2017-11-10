#pragma once
#include "Public\Utils.h"
#include "eIMLogger\IeIMLogger.h"

class C_EIMPluginMgr: public I_EIMPluginMgr
{
	DECALRE_PLUGIN_(C_EIMPluginMgr)
public:
	C_EIMPluginMgr();
	~C_EIMPluginMgr(void);
	
public:
	virtual int	LoadPlugins(				// Load plugins
		const TCHAR*	lpszPluginConfigFile
		);

	virtual void UnloadPlugins();			// Unload plugins
		
	virtual HWND GetParrentWnd(
		I_EIMPlugin*	pPlugin,			// Get the parent window's handle
		bool			bCreateTemplateDlg	// true = Create parent dialog if is need
		);	
	
	virtual DWORD Enums(
		VectPlugins&	vectPlugins,		// 	Return the result of enumerated//	Caller empty it if needed at first
		E_PluginClass	ePluginClass
		);

	virtual BOOL DllPreTranslateMessage(	// PreTranslateMessage
		MSG* pMsg
		);	
	
protected:
	typedef struct tagEIMPluginData: public S_PluginDllInfo
	{
		HWND			hWnd;								// Plugin's parent window handle
		I_EIMPlugin*	pPlguin;							// Plugin interface
	}S_EIMPluginData, *PS_EIMPluginData;	

	typedef std::vector< S_EIMPluginData >				VectPluginData;
	typedef std::vector< S_EIMPluginData >::iterator	VectPluginDataIt;

	DWORD			m_dwRef;
	VectPluginData	m_vectPluginData;

	E_PluginClass _GetPluginClass(const char* pszClass);

};
