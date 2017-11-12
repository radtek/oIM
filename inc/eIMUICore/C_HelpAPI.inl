inline HRESULT C_HelpApi::QueryInterface(LPCTSTR pctszIID, void** ppvIObject)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->QueryInterface(pctszIID, ppvIObject);
}

//inline DWORD C_HelpApi::GetUniqueId()
//{
//	CHECK_NULL_RET_( m_pIUICore, 0 );
//	return m_pIUICore->GetUniqueId();
//}
//
//inline QSID C_HelpApi::GetSessionId(QEID qeid, DWORD dwInitSeq)
//{
//	CHECK_NULL_RET_( m_pIUICore, 0 );
//	return m_pIUICore->GetSessionId(qeid, dwInitSeq);
//}
//
//inline QMID C_HelpApi::GetMsgId(QEID qeid, DWORD dwInitSeq)
//{
//	CHECK_NULL_RET_( m_pIUICore, 0 );
//	return m_pIUICore->GetMsgId(qeid, dwInitSeq);
//}

inline LPCTSTR C_HelpApi::GetFontFamily(int i32Index)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->GetFontFamily(i32Index);
}

inline LPCTSTR C_HelpApi::GetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDefault)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->GetErrorLangInfo(dwErrId, lpszDefault);
}

inline LPCTSTR C_HelpApi::SetErrorLangInfo(DWORD dwErrId, LPCTSTR lpszDesc)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->SetErrorLangInfo(dwErrId, lpszDesc);
}

inline QFID C_HelpApi::GetQfid(const char* pszKey)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->GetQfid(pszKey);
}

inline BOOL	C_HelpApi::UpdateLogo(QEID eid, LPCTSTR pszLogoTpl, LPCTSTR pszLogo)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->UpdateLogo(eid, pszLogoTpl, pszLogo);
}

inline int C_HelpApi::SendMsg(/*PS_UISessionInfo*/void* psUISInfo, LPCTSTR pszXml, DWORD dwFlag)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->SendMsg(psUISInfo, pszXml, dwFlag);
}

//=============================================================================
//Function:     eIMUnzip
//Description:	Unzip 
//
//Parameter:
//  pszZipFile		- Zip file path
//  pszUnzipFile 	- Unzip file path
//  pszPsw			- Password
//
//Return:
//		TRUE:	- Update succeed
//		FALSE:	- Update failed
//=============================================================================
inline BOOL C_HelpApi::Unzip(LPCTSTR pszZipFile, LPCTSTR pszUnzipFile, const char* pszPsw)
{
	//CHECK_NULL_RET_(pszZipFile, FALSE);
	//CHECK_NULL_RET_(pszUnzipFile, FALSE);

	//C_PluginDll dllZip(ePLUGIN_TYPE_NORMAL, INAME_EIMZIP);
	//I_eIMZip*	pIZip = NULL;

	//if ( dllZip.Load(_T("eIMZip.dll")) && SUCCEEDED(dllZip.eIMCreateInterface(INAME_EIMZIP, (void**)&pIZip)) )
	//{
	//	AUTO_RELEASE_(pIZip);
	//	BOOL bRet = (IMZIP_OK == pIZip->Decompression(pszZipFile, pszUnzipFile, COVER_BEFORE_FILE, pszPsw, NULL));
	//	TRACE_( _T("Unzip [%s ==> %s] %s"), pszZipFile, pszUnzipFile, bRet ? _T("Succeeded") : _T("Failed!") );
	//	return bRet;
	//}
	//
	//TRACE_( _T("Unzip [%s] failed!"), pszZipFile, pszUnzipFile);
	return FALSE;
}

//=============================================================================
//Function:     eIMCalCRC8
//Description:	Calculate CRC8
//
//Parameter:
//	pu8Data    	- Data
//	i32Len		- Data size
//	pchOut		- Return the CRC8 result
//
//Return:
//	TRUE	- Succeeded
//	FALSE	- Failed
//=============================================================================
inline BOOL C_HelpApi::CalCRC8(unsigned char *pu8Data,int i32Len,char* pchOut)
{
	if( pu8Data == NULL || pchOut == NULL || i32Len == 0) {ASSERT_(FALSE); return FALSE;}

	unsigned char i;
	unsigned char crc=0;
	while(i32Len-- != 0)
	{
		for(i=0x80; i!=0; i/=2)
		{
			if((crc&0x80)!=0)
			{
				crc*=2;
				crc^=0x07;
			} /* 余式CRC乘以2再求CRC */
			else
				crc*=2;
			if((*pu8Data&i)!=0)
				crc^=0x07; /* 再加上本位的CRC */
		}
		pu8Data++;
	}
	sprintf(pchOut,"%0x", crc);

	return TRUE;
}

inline void C_HelpApi::SendEvent(DWORD dwEventId, void* pvParam)
{
	CHECK_NULL_RETV_( m_pIUICore );
	return m_pIUICore->SendEvent(dwEventId, pvParam);
}

inline BOOL C_HelpApi::RegisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin, E_EIMEventType eType)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->RegisterEvent(dwEventID, pPlugin, eType);
}

inline BOOL C_HelpApi::UnregisterEvent(DWORD dwEventID, I_EIMPlugin* pPlugin)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UnregisterEvent(dwEventID, pPlugin);
}

inline void C_HelpApi::UICenterWindow(HWND hwnd)
{
	CHECK_NULL_RETV_( m_pIUICore );
	return m_pIUICore->UICenterWindow(hwnd);
}

inline HWND C_HelpApi::UIGetMainWnd()
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->UIGetMainWnd();
}

inline HWND C_HelpApi::UIGetPopupWnd()
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->UIGetPopupWnd();
}

//=============================================================================
//Function:     eIMSetActiveWindowForInput
//Description:	Set a active window for input
//
//Parameter:
//  hWnd	- Handle of window 
//
//Return:
//	The prev-acitive handoe of window
//=============================================================================
inline HWND C_HelpApi::UISetActiveWindowForInput(HWND hWnd)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->UISetActiveWindowForInput(hWnd);
}

// 重要提醒：如果不是在eIMUICore中调用，一定要先初始化回调函数，如：sInfo.pfnCallback = InvaliteCallback;
// 说明：默认回调函数实现在邀请接口头文件中，是内联函数，只要使用了，内存申请就会在调用者模块中，不会有内存问题。
inline BOOL C_HelpApi::ShowInviteWnd(PS_InviteInfo psInfo)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->ShowInviteWnd(psInfo);
}

// 重要提醒：如果不是在eIMUICore中调用，一定要先初始化回调函数，如：sInfoEx.pfnCallbackEx = InvaliteCallbackEx;
// 说明：默认回调函数实现在邀请接口头文件中，是内联函数，只要使用了，内存申请就会在调用者模块中，不会有内存问题。
inline BOOL C_HelpApi::ShowInviteWndEx(PS_InviteInfoEx psInfoEx)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->ShowInviteWndEx(psInfoEx);
}

//inline BOOL C_HelpApi::ShowScreenshot(LPCTSTR pszSaveFile)
//{
//	CHECK_NULL_RET_( m_pIUICore, FALSE );
//	return m_pIUICore->ShowScreenshot(pszSaveFile);
//}

inline BOOL C_HelpApi::UIUpdateHotkey(DWORD dwType, UINT u32Key, UINT u32Modifier)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UIUpdateHotkey(dwType, u32Key, u32Modifier);
}

inline LPCTSTR C_HelpApi::UIGetLanguage(int i32Lang, BOOL bErrorInfo)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->UIGetLanguage(i32Lang, bErrorInfo);
}

inline int C_HelpApi::UIGetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect )
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->UIGetAttributeStr(lpszPath, lpszAttrName, pszValue, i32Size, bEnableRedirect);
}

inline LPCTSTR C_HelpApi::UIGetAttributeStrEx( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPTSTR pszDefault, BOOL bEnableRedirect )
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->UIGetAttributeStrEx(lpszPath, lpszAttrName, pszDefault, bEnableRedirect);
}

inline BOOL C_HelpApi::UISetAttributeStr( LPCTSTR lpszPath, LPCTSTR lpszAttrName, LPCTSTR lpszValue )
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UISetAttributeStr(lpszPath, lpszAttrName, lpszValue);
}

inline int C_HelpApi::UIGetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Default, BOOL bEnableRedirect )
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->UIGetAttributeInt(lpszPath, lpszAttrName, i32Default, bEnableRedirect);
}

inline BOOL C_HelpApi::UISetAttributeInt( LPCTSTR lpszPath, LPCTSTR lpszAttrName, int i32Value )
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UISetAttributeInt(lpszPath, lpszAttrName, i32Value);
}

//inline int C_HelpApi::UIGetPortraitMode()
//{
//    return UIGetAttributeInt(PATH_FUNC, FUNC_PORTRAIT_MODE, PORTRAIT_MODE_DEFAULT );
//}

inline int C_HelpApi::UIGetText( LPCTSTR lpszPath, LPTSTR pszValue, int i32Size, BOOL bEnableRedirect )
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->UIGetText(lpszPath, pszValue, i32Size, bEnableRedirect);
}

inline BOOL C_HelpApi::UISetText( LPCTSTR lpszPath, LPCTSTR lpszValue )
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UISetText(lpszPath, lpszValue);
}

inline BOOL C_HelpApi::UISaveConfig()
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UISaveConfig();
}

inline LPCTSTR C_HelpApi::UIGetAppVersion(BOOL bTrimBuildVer)
{
	CHECK_NULL_RET_( m_pIUICore, _T("3.0.0") );
	return m_pIUICore->UIGetAppVersion(bTrimBuildVer);
}

inline LPCTSTR C_HelpApi::UIGetSkinPath(int* pi32DpiRatio)
{
	CHECK_NULL_RET_( m_pIUICore, _T("") );
	return m_pIUICore->UIGetSkinPath(pi32DpiRatio);
}

inline BOOL C_HelpApi::UIGetMsgSummary(LPCTSTR pszXmlMsg, LPTSTR pszMsgSummary, int i32Size, BOOL bPrefixName)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->UIGetMsgSummary(pszXmlMsg, pszMsgSummary, i32Size, bPrefixName);
}

inline I_EIMLogger* C_HelpApi::UIGetLogger(BOOL bAddRef)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->UIGetLogger(bAddRef);
}

inline DWORD C_HelpApi::GetPurview(DWORD u32Type, DWORD dwId)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->GetPurview(u32Type, dwId);
}

inline int C_HelpApi::GetLoginState()
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->GetLoginState();
}

inline DWORD C_HelpApi::GetTimeNow()
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->GetTimeNow();
}

inline LPCTSTR C_HelpApi::GetAttributeStr(LPCTSTR pszName, LPCTSTR pszDefault)
{
	CHECK_NULL_RET_( m_pIUICore, NULL );
	return m_pIUICore->GetAttributeStr(pszName, pszDefault);
}

inline int C_HelpApi::GetAttributeInt(LPCTSTR pszName, int i32Default)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->GetAttributeInt(pszName, i32Default);
}

inline int C_HelpApi::AddCmd(const void* pvData, DWORD dwCmdId, DWORD dwThreadId)
{
	CHECK_NULL_RET_( m_pIUICore, EIMERR_NO_INTERFACE );
	return m_pIUICore->AddCmd(pvData, dwCmdId, dwThreadId);
}

inline int C_HelpApi::AddCmd(void* pICmd, DWORD dwThreadId, BOOL bAddRef)
{
	CHECK_NULL_RET_( m_pIUICore, EIMERR_NO_INTERFACE );
	return m_pIUICore->AddCmd(pICmd, dwThreadId, bAddRef);
}

inline int C_HelpApi::Command(LPCTSTR pszXml, DWORD dwThreadId)
{
	CHECK_NULL_RET_( m_pIUICore, EIMERR_NO_INTERFACE );
	return m_pIUICore->Command(pszXml, dwThreadId);
}

inline BOOL	C_HelpApi::IsTypeOfId(QEID qeid, E_TypeId eTypeId)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->IsTypeOfId(qeid, eTypeId);
}

inline BOOL C_HelpApi::IsFace(LPCTSTR pszFace)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->IsFace(pszFace);
}

inline int C_HelpApi::UIMessageBox(HWND hwndParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, LPCTSTR pszSkinFile)
{
	CHECK_NULL_RET_( m_pIUICore, 0 );
	return m_pIUICore->UIMessageBox(hwndParent, lpText, lpCaption, uType, pszSkinFile);
}

inline BOOL C_HelpApi::VerifyFile(LPCTSTR pszFile, LPCTSTR pszFileKey)
{
	CHECK_NULL_RET_( m_pIUICore, FALSE );
	return m_pIUICore->VerifyFile(pszFile, pszFileKey);
}