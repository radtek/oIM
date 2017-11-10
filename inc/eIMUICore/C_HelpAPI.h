 // This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIM_GLOBAL_API_2017_04_10_BY_YFGZ__
#define __EIM_GLOBAL_API_2017_04_10_BY_YFGZ__

#include "IeIMUICore.h"
#include "Public\Utils.h"
#include "Public\XmlLang.h"
#include "eIMLogger\IeIMLogger.h"
#include "eIMUICore\IeIMUIInviteWnd.h"

#define hpi C_HelpApi::GetObject()

#define HOTKEY_COUNT			( 3 )	// Hotkey count
#define HOTKEY_KEY_RESTORE		( 0xFFFFFFFF )	// Restore to default
#define HOTKEY_KEY_DISABLE		( 0 )	// Disable hotkey
#define HOTKEY_TYPE_UNKNOWN		( -1 )	// Unknown
#define HOTKEY_TYPE_SCREENSHOT	( 0 )	// Screenshot
#define HOTKEY_TYPE_SEARCH		( 1 )	// Search emps
#define HOTKEY_TYPE_EXTRACTMSG	( 2 )	// Pickup message

class C_HelpApi
{
private:
	C_HelpApi(): m_pIUICore(NULL){}
	I_EIMUICore* m_pIUICore;

public:
	~C_HelpApi()
	{
		SAFE_RELEASE_INTERFACE_(m_pIUICore);
	}

	static C_HelpApi& GetObject()
	{
		static C_HelpApi h;
		if ( h.m_pIUICore == NULL )
		{
			if ( PFN_eIMCreateInterface pfn = (PFN_eIMCreateInterface)::GetProcAddress(NULL, "eIMCreateInterface") )
			{
				if ( SUCCEEDED(pfn(INAME_EIMUI_CORE, (void**)&h.m_pIUICore)) )
					return h;
			}
		}

		return h;
	}

	inline HRESULT QueryInterface(LPCTSTR pctszIID, void** ppvIObject);
	// HelpApi ///////////////////////////////////////////////////////////////////////////
	//inline DWORD	GetUniqueId();
	//inline QSID		GetSessionId(QEID qeid, DWORD dwInitSeq = 0);
	//inline QMID		GetMsgId(QEID qeid, DWORD dwInitSeq = 0);
	inline LPCTSTR	GetFontFamily(int i32Index);
	inline LPCTSTR	GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault=NULL);
	inline LPCTSTR	SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc);
	inline QFID		GetQfid(const char* pszKey=NULL);
	inline BOOL		Unzip(LPCTSTR pszZipFile, LPCTSTR pszUnzipFile, const char* pszPsw=NULL);
	inline BOOL		CalCRC8(unsigned char *pu8Data,int i32Len,char* pchOut);
	inline BOOL		UpdateLogo(QEID eid, LPCTSTR pszLogoTpl, LPCTSTR pszLogo);
	inline int   	SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag);

	inline void		SendEvent(DWORD dwEventId, void* pvParam);
	inline BOOL		RegisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin, E_EIMEventType eType);
	inline BOOL		UnregisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin);
	 
	inline void		UICenterWindow(HWND hwnd);
	inline HWND		UIGetMainWnd();
	inline HWND		UIGetPopupWnd();
	inline HWND		UISetActiveWindowForInput(HWND hWnd);
	inline int		UIMessageBox(HWND hwndParent, LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uType = MB_OK, LPCTSTR pszSkinFile = NULL);

	inline BOOL		ShowInviteWnd(PS_InviteInfo psInfo);
	inline BOOL		ShowInviteWndEx(PS_InviteInfoEx psInfoEx);
	//inline BOOL		ShowScreenshot(LPCTSTR pszSaveFile);

	inline BOOL		UIUpdateHotkey(DWORD dwType, UINT u32Key, UINT u32Modifier);
	inline LPCTSTR	UIGetLanguage(int i32Lang=-1, BOOL bErrorInfo = FALSE);
	inline int		UIGetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect=TRUE );
	inline LPCTSTR	UIGetAttributeStrEx( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszDefault=NULL, BOOL bEnableRedirect=TRUE );
	inline BOOL		UISetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue );
	inline int		UIGetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default=0, BOOL bEnableRedirect=TRUE );
	inline BOOL		UISetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value );
	inline int		UIGetText( LPCTSTR lpszPath, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect=TRUE );
	inline BOOL		UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue );
	inline BOOL		UISaveConfig();
	inline LPCTSTR	UIGetAppVersion(BOOL bTrimBuildVer = TRUE);
	inline LPCTSTR  UIGetSkinPath(int* pi32DpiRatio = NULL);

	inline I_EIMLogger* UIGetLogger(BOOL bAddRef=FALSE);
	inline BOOL		UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName=FALSE);
	inline int		UIGetPortraitMode();

	inline DWORD	GetPurview(DWORD u32PurType, DWORD dwId=0);
	inline int		GetLoginState();
	inline DWORD	GetTimeNow();
	inline LPCTSTR	GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault = NULL);
	inline int		GetAttributeInt(LPCTSTR pszName, int i32Default = 0);
	inline int		AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId =1);
	inline int		AddCmd(I_EIMCmd* pCmd, DWORD dwThreadId = 1, BOOL bAddRef = FALSE);
	inline int 		Command(LPCTSTR pszXml, DWORD dwThreadId = 1);
	inline BOOL		IsTypeOfId(QEID qeid, E_TypeId eTypeId);
	inline BOOL		IsFace(LPCTSTR pszFace);
	inline BOOL		VerifyFile(LPCTSTR pszFile, LPCTSTR pszFileKey);
};

#include "C_HelpAPI.inl"

#endif  // __EIM_GLOBAL_API_2017_04_10_BY_YFGZ__