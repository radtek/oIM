#include "stdafx.h"
#include "eIMCmd.h"
#include "C_XmlProtocol.h"
//#include "eIMEngine\C_CmdReflection.h"
#include "C_eIMSaveData.h"
#include "eIMUpdater\UpdaterError.h"

#define  BOM_UTF8					(0x00EFBBBF)
#define  IS_BOM_UTF8(P)				( MAKELONG(MAKEWORD((P)[2], (P)[1]), MAKEWORD((P)[0], 0)) == BOM_UTF8 )


#define  ROAM_TYPE_COM_CONTACT		1	//	常用联系人
#define  ROAM_TYPE_COM_DEPT			2	//	常用部门
#define  ROAM_TYPE_CONCERN_CONTACT	3	//	关注联系人
#define  ROAM_TYPE_DEFINE_GROUP		4	//  自定义组
#define  ROAM_TYPE_DEFINE_GROUP_MEM	5	//  自定义组成员变化
#define  ROAM_TYPE_DEFAULT_COMCONTACT    6	//默认常用联系人

#define  ROAM_OPER_TYPE_ADD			1	// 新增
#define  ROAM_OPER_TYPE_DELETE		2	// 删除
#define EIME_SUBMOD_UPDATER			_T("eIMUpdater.dll");		// Updater dll
//#define  SET_DESC_WHEN_FAIL_(R, D)	{ if (FAILED(R)){ ::eIMUTF8ToTChar((D), m_szErrorDesc); ::eIMSetErrorLangInfo((R), (m_szErrorDesc.c_str()));}}


namespace Cmd
{
// Helper Macro to Db failed rollback and convert the errorcode
#define ROLLBACK_FAILED_RET_(PDB, RET) \
	if (RET != SQLITE_OK) \
	{ \
		(PDB)->Rollback(); \
		RET = EIMERR_DB_ERROR(RET); \
		return RET; \
	}

// Helper Macro to simple string TRACE result 
#define CMD_TRACE_STR_(S, R) \
	if (FAILED(R)) \
		EIMLOGGER_ERROR_( pILog, _T("[%s] failed, result: 0x%08X, desc:%s"), S.c_str(), (R), GetErrorDesc()); \
	else \
		EIMLOGGER_DEBUG_( pILog, _T("[%s] succeeded"), S.c_str()); 

// Helper Macro to simple TRACE result
#define CMD_TRACE_(R) \
	if (FAILED(R)) \
	EIMLOGGER_ERROR_( pILog, _T("Failed, result: 0x%08X, desc:%s"), (R), GetErrorDesc()); \
	else \
		EIMLOGGER_DEBUG_( pILog, _T("Succeeded"));

#define CHECK_FAILED_SEND_EVENT_RET_(E,P) \
	if (FAILED(m_i32Result)) \
	{ \
		GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR); \
		SAFE_SEND_EVENT_(pIEMgr, (E), (P) ); \
		return m_i32Result; \
	}

//	static UINT	s_u32OffMsgCount = 0;
	static VectUserDept s_vectUserDept;
	static VectUserInfo s_vectUserInfo;
	static VectDeptInfo s_vectDeptInfo;
#define CLEAR_VECTOR_(V, T) \
	{ \
		T vectTmp; \
		V.swap(vectTmp); \
	}

	BOOL CheckAndSetFileAttribute( TiXmlElement* pEl, const char* pszUtf8FilePath, const char* pszKey )
	{
		CHECK_NULL_RET_(pEl, FALSE);
		CHECK_NULL_RET_(pszUtf8FilePath, FALSE);
		
		eIMStringA		szValueA;
		struct __stat64	statFile = { 0 };
		char			szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
	
		if ( !pEl->Attribute(FIELD_SIZE) )
		{
			eIMString szValue;
			::eIMUTF8toWByte(pszUtf8FilePath, szValue);
		//	if (!_stat64(szValueA.c_str(), &statFile) )	// Receive the image not have the size
			_tstat64(szValue.c_str(), &statFile);		// When failed ,is 0
			pEl->SetAttribute(FIELD_SIZE, GET_QSID_ASTR_(statFile.st_size, szBuf));

		}

		if ( !pEl->Attribute(FIELD_NAME) )
		{
			const char* pszPos = strrchr(pszUtf8FilePath, '\\');
			if ( pszPos == NULL )
				pszPos = strrchr(pszUtf8FilePath, '/');

			if ( pszPos == NULL )
			{
				ASSERT_(FALSE);
				return FALSE;
			}

			pEl->SetAttribute(FIELD_NAME, pszPos + 1);
		}

		if ( !pEl->Attribute(FIELD_FID) )
			pEl->SetAttribute(FIELD_FID, GET_QSID_ASTR_(hpi.GetQfid(pszKey), szBuf));	// Add UID

		return TRUE;
	}

    BOOL SetFileMetaData(TiXmlElement* pEl, FILE_META& sFileMeta,int& i32Index)
    {
        const char* pszValue  = pEl->Attribute(FIELD_KEY);
        if ( pszValue )
        {
            eIMString szData;
            i32Index			   = sizeof( FILE_META );
            sFileMeta.dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
            sFileMeta.dwFileSize = htonl(sFileMeta.dwFileSize);
            strncpy(sFileMeta.aszURL, pszValue, COUNT_OF_ARRAY_(sFileMeta.aszURL));
            strncpy(sFileMeta.aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(sFileMeta.aszFileName));
            return TRUE;
        }
        return FALSE;
    }

	UINT64  htonl64(UINT64  host)   
	{   

		UINT64   ret = 0;   
		unsigned long   high,low;
		low   =   host & 0xFFFFFFFF;
		high   =  (host >> 32) & 0xFFFFFFFF;
		low   =   htonl(low);   
		high   =   htonl(high);   
		ret   =   low;
		ret   <<= 32;   
		ret   |=   high;   
		return   ret;   
	}

	UINT64  ntohl64(UINT64   host)   
	{   
		UINT64   ret = 0;   
		unsigned long   high,low;
		low   =   host & 0xFFFFFFFF;
		high   =  (host >> 32) & 0xFFFFFFFF;
		low   =   ntohl(low);   
		high   =   ntohl(high);   
		ret   =   low;
		ret   <<= 32;   
		ret   |=   high;   
		return   ret;   
	}

    bool __stdcall StatNotifyCB(PS_EIMStatNotify psStat, void *pvUserData)
    {
        CHECK_NULL_RET_(psStat, FALSE);
        TRACE_(_T("TickCount:%u, Qfid:%llu, Stat:%d, Size: %lld/%lld"), GetTickCount(), psStat->Qfid, psStat->eTransStat, psStat->i64CompleteSize, psStat->i64TotalSize);
        if ( psStat->eTransStat == eInterFile_TaskEnd )
        {
            BOOL* pbFinished = (BOOL*)pvUserData;
            *pbFinished = TRUE;
        }

        return true;
    }

    const BOOL WaitMsgLong(QFID qfid, const TCHAR* pszLocalFile, const TCHAR* pszKey)
    {
        CHECK_NULL_RET_(pszLocalFile, FALSE);
        CHECK_NULL_RET_(pszKey, FALSE);
        GET_ISIFILE_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
        S_EIMFileInfo sFile  = { 0 };
		C_eIMEngine&  eng = Eng;

        const TCHAR* pszNamePos = _tcsrchr(pszLocalFile, _T('\\'));
        if (pszNamePos == NULL)
            pszNamePos = _tcsrchr(pszLocalFile, _T('/'));

        CHECK_NULL_RET_(pszNamePos, FALSE);

        _tcsncpy(sFile.tszFileName, pszNamePos + 1, COUNT_OF_ARRAY_(sFile.tszFileName) -1);
        _tcsncpy(sFile.tszLocalPath, pszLocalFile, COUNT_OF_ARRAY_(sFile.tszLocalPath) -1);
        sFile.tszLocalPath[pszNamePos - pszLocalFile] = _T('\0');

        sFile.eDirec	= eInterFile_Download;
        sFile.Qfid		= qfid;
        sFile.eFileType = eInterFile_File;
        sFile.ePriority = eInterFile_Normal;
        sFile.eEncrpt   = eInterFile_Encrpt;

        if(eng.GetPurview(FILE_TRANS_MODE_RESUME))
        {
            sFile.eTransWay = eInterFile_Resuming;
            _tcsncpy(sFile.tszFileKey, pszKey, COUNT_OF_ARRAY_(sFile.tszFileKey));
            _sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszUpServerUrl), _T("%s"), 
                eng.GetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD_BT));
        }
        else
        {
            sFile.eTransWay = eInterFile_NonResuming;
            _sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszUpServerUrl), _T("%skey=%s"), 
                eng.GetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD), pszKey);
        }

        BOOL bFinished = FALSE;
        pISIFile->StatNotify(StatNotifyCB, (void*)&bFinished);
        return SUCCEEDED(pISIFile->TaskTrans(sFile));
    }
	void ParseRobotMsgType(const eIMString& strRobotXml, int& iType);
    int ChangeMsgType2CollectType(const eIMString& szMsgUI);
	//C_CmdLogin///////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdLogin, eCMD_LOGIN);
	int	C_CmdLogin::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		GET_IEGN_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_BEFORE, NULL);
		CMD_ABORT_RET_(pbAbort);
		S_LoginStatus sLoginStatus = { 0 };

		LogInfo(pILog);	// 先写登录信息日志，以便查错
		if ( !pICa->GetStatus(CA_STATUS_INITED) )
		{	// Initial ClientAgent
			eIMStringA szLogFileA;
			eIMStringA szRsaKeyFileA;
			
			if ( !GetRsaKeyFile(szRsaKeyFileA) )
			{
				EIMLOGGER_ERROR_(pILog, _T("Copy public rsa public key to [%S] failed!"), szRsaKeyFileA.c_str() );
				sLoginStatus.dwStatus = m_i32Result;
				SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
				return m_i32Result;
			}

			m_i32Result	= pICa->Init( m_sData.cMode == CMNET ? CMNET : CMWAP, (char*)szRsaKeyFileA.c_str(), GetLogFile(szLogFileA) );
			CMD_ABORT_RET_(pbAbort);
			if ( FAILED(m_i32Result) )
			{
				EIMLOGGER_ERROR_(pILog, _T("Init client agent: Failed, result: 0x%08X, desc:%s, access mode as:(%s)"), 
					m_i32Result, GetErrorDesc(), GetMode(m_sData.cMode));
				sLoginStatus.dwStatus = m_i32Result;
				SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
				return m_i32Result;
			}

			EIMLOGGER_INFO_(pILog, _T("Init client agent: Succeeded, access mode as: %s"), GetMode(m_sData.cMode));
		}

		CMD_ABORT_RET_(pbAbort);
		static char szVersion[20] ={0};	
		if ( szVersion[0] == '\0' )
		{
			eIMStringA	szAppVerA;
			::eIMTChar2MByte(hpi.UIGetAppVersion(), szAppVerA);
			strcpy(szVersion, szAppVerA.c_str());
		}

		if ( !pICa->GetStatus(CA_STATUS_CONNECT) )
		{	// Connect
			eIMStringA	szIPA;

			LOGINACCESSACK* psAck = NULL;
			for ( int i32Index = 0; i32Index < COUNT_OF_ARRAY_( m_sData.asSvr ); i32Index++ )
			{
				if ( m_sData.asSvr[i32Index].szIP[0] == _T('\0') )
				{
					EIMLOGGER_WARN_(pILog, _T("Server of index[%d] is invalid"), i32Index);
					continue;
				}

				::eIMTChar2MByte( m_sData.asSvr[i32Index].szIP, szIPA );
				char szIP[MAXCHAR]="";
				strcpy(szIP, szIPA.c_str());

				do {
					m_i32Result = pICa->Connect(szIP, m_sData.asSvr[i32Index].i32Port, m_sData.sLogin.tAccount.value, szVersion);
					CMD_ABORT_RET_(pbAbort);
					if ( FAILED(m_i32Result) && (psAck = pICa->GetLoginAccessAck()) )
					{
						//SET_DESC_WHEN_FAIL_(m_i32Result, psAck->tRetDesc.value);	// This error desc not integrity,eg.: EIMERR_GETHOSTNAME_SOCKET
						if ( m_i32Result == EIMERR_SYSTEM_OVERLOAD )
						{
							int			i32RetryTime = psAck->iTryTime;
							eIMString	szOverloadInfo = hpi.GetErrorLangInfo(EIMERR_SYSTEM_OVERLOAD, _T("Over the overload protected(200/s)\nAuto reconect after %ds"));
							TCHAR		szOverloadInfoPrompt[256] = { 0 };
						//	m_i32Result = pICa->Disconnect();	// Can'not connect succeed when not disconnect

							do
							{
								if ( pIEgn->GetPurview(GET_USER_OFFLINE_FLAG) && pIEgn->GetPurview(GET_LOGIN_STATUS) != eENGINE_STATUS_LOGINING )	// User abort logining
								{
									EIMLOGGER_INFO_(pILog, _T("User abort auto reconnect"));
									return m_i32Result;
								}

								_sntprintf(szOverloadInfoPrompt, COUNT_OF_ARRAY_(szOverloadInfoPrompt), szOverloadInfo.c_str(), i32RetryTime);
								SAFE_SEND_EVENT_(pIEMgr, EVENT_OVERLOAD_INFO, (void*)szOverloadInfoPrompt );
								EIMLOGGER_DEBUG_(pILog, _T("%s"), szOverloadInfoPrompt);
								Sleep(1000);
							}while(i32RetryTime-- > 0);
							SAFE_SEND_EVENT_(pIEMgr, EVENT_OVERLOAD_INFO, (void*)hpi.GetErrorLangInfo(EIMERR_STATUS_LOGINING, _T("Logining...")));
						}
					}
				} while(m_i32Result == EIMERR_SYSTEM_OVERLOAD);

				if (FAILED(m_i32Result))
				{	// 账号和密码相关错误，立即终止登录
					if ( m_i32Result == EIMERR_NO_USER || 
						m_i32Result == EIMERR_INVALID_PSW ||
						m_i32Result == EIMERR_INVALIDUSER ||
						m_i32Result == EIMERR_SSO_USER_OR_PSW ||
						m_i32Result == EIMERR_FORBIDDEN || 
						m_i32Result == EIMERR_SSO_USER_FORBID ||
						m_i32Result == EIMERR_SSO_USER_EXPIRE ||
						m_i32Result == EIMERR_SSO_USER_OR_PSW_EMPTY )
						break;
				}

				// 检查失败 重连接一次
				switch(m_i32Result)
				{
				case EIMERR_SOCKETFD_SOCKET:
				case EIMERR_GETHOSTNAME_SOCKET:
				case EIMERR_CONNECT_TIMEOUT_SOCKET:
				case EIMERR_SOCKET_GETOPT_SOCKET:
				case EIMERR_SENDSOCKET_EWOULDBLOCK_SOCKET:
				case EIMERR_RECV_DATA_SOCKET:

				case EIMERR_SENDSOCKET_SOCKET:
				case EIMERR_RECV_TIMEOUT_SOCKET:
				case EIMERR_SOCKETCLOSE_SOCKET:

				case EIMERR_GETHOSTNAME_SERVICE_SOCKET:
				case EIMERR_CONNECT_SERVICE_TIMEOUT_SOCKET:
					EIMLOGGER_ERROR_(pILog, _T("Init client agent[Server:%s, Port:%d]: Failed! result: 0x%08X, desc:%s ---- DO reconnect"),
						m_sData.asSvr[i32Index].szIP, m_sData.asSvr[i32Index].i32Port,m_i32Result,GetErrorDesc());
						m_i32Result = pICa->Connect(szIP, m_sData.asSvr[i32Index].i32Port, m_sData.sLogin.tAccount.value, szVersion);
					break;
				}

				if (FAILED(m_i32Result))
				{
					EIMLOGGER_ERROR_(pILog, _T("Init client agent[Server:%s, Port:%d]: Failed! result: 0x%08X, desc:%s"),
						m_sData.asSvr[i32Index].szIP, m_sData.asSvr[i32Index].i32Port,m_i32Result,GetErrorDesc());
					continue;
				}

				EIMLOGGER_INFO_(pILog, _T("Init client agent[Server:%s, Port:%d]: Succeeded."),
					m_sData.asSvr[i32Index].szIP, m_sData.asSvr[i32Index].i32Port);

				if ( pIEgn->GetAttributeInt(IME_ATTRIB_SWITCH_FILE_SERVER) )
				{	// If enabled switch file server, to switch it(First is 99BILL)
					int i32Param = SERVER_SWITCH_FILE_ALL;
					if (i32Index == 0) 
						i32Param &= ~SERVERS_SWITCH_FILE_SVR;

					SAFE_SEND_EVENT_(pIEMgr, EVENT_SERVERS_CHANGED, (void*)i32Param);
				}

				break;			
			}

			if ( FAILED(m_i32Result) )
			{
				sLoginStatus.dwStatus = m_i32Result;
				SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sLoginStatus );
				return m_i32Result;
			}
		}
		else
		{
			EIMLOGGER_INFO_( pILog, _T("connect ok! do not send login packet again"));
		//	return m_i32Result;
		}

		CMD_ABORT_RET_(pbAbort);
		
		if ( !CheckUpdate(pILog, pICa, pIEgn, pIEMgr) )
			return m_i32Result;

		// Login
		eIMString szName;
		::eIMUTF8ToTChar( m_sData.sLogin.tAccount.value, szName);
		CMD_ABORT_RET_(pbAbort);
		m_i32Result = pICa->Login(m_sData.sLogin.tAccount.value, m_sData.sLogin.aszPassword, TERMINAL_PC, szVersion, NULL);
		if ( FAILED(m_i32Result) )
		{
			sLoginStatus.dwStatus = m_i32Result;
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sLoginStatus );
		}

		CMD_TRACE_STR_(szName, m_i32Result);
		return m_i32Result;
	}

	inline BOOL C_CmdLogin::LogInfo(I_EIMLogger* pILog)
	{
		char szMd5A[33] = { 0 };
		eIMString szMd5;
		eIMString szInfo;

		::eIMUTF8ToTChar( m_sData.sLogin.tAccount.value, szInfo);
		szInfo = _T("[") + szInfo;
		Create32Md5((unsigned char*) m_sData.sLogin.aszPassword, strlen(m_sData.sLogin.aszPassword), (unsigned char*)szMd5A); // calc md5
		eIMUTF8ToTChar(szMd5A, szMd5);
		szInfo += _T(":");
		szInfo += szMd5;
		szInfo += _T("]");
		EIMLOGGER_INFO_(pILog, _T("%s"), szInfo.c_str());

		return TRUE;
	}

	const char*  C_CmdLogin::GetRsaKeyFile(eIMStringA& szRsaKeyFileA)
	{	// Init RSA public key
		eIMString szRsaKey=PATH_TOKEN_APPDATA_FILE_(_T("rsa_public.key"));
		
		::eIMReplaceToken(szRsaKey);
		if ( !eIMPathFileExists(szRsaKey.c_str()) )
		{	// Not exist, copy to dest
			eIMString szRsaKeySrc=PATH_TOKEN_EXE_FILE_( _T("rsa_public.key"));
			::eIMReplaceToken(szRsaKeySrc);
			if ( !CopyFile(szRsaKeySrc.c_str(), szRsaKey.c_str(), FALSE) )
			{
				m_i32Result = EIMERR_NO_RSA_KEY;
				return NULL;
			}
		}

		::eIMTChar2MByte(szRsaKey.c_str(), szRsaKeyFileA);
		return szRsaKeyFileA.c_str();
	}

	const char* C_CmdLogin::GetLogFile(eIMStringA& szLogFileA)
	{
		eIMString  szLogFile;

		eIMGetLogFile(szLogFile, _T("Client_"), NULL);
		::eIMTChar2MByte(szLogFile.c_str(), szLogFileA);
		eIMStringA::size_type nPos = szLogFileA.rfind('_');
		if ( nPos != eIMStringA::npos )
			szLogFileA[nPos] = '\0';	// 因为Client会再追加后面的日期部份
		 
		return szLogFileA.c_str();
	}

	const TCHAR* C_CmdLogin::GetMode(int i32Mode)
	{
		if ( m_sData.cMode == CMNET ) 
			return _T("CMNET");
		else
			return _T("CMWAP");
	}

	const TCHAR* C_CmdLogin::GetUpdateType(int i32Type)
	{
		switch ( i32Type )
		{
		case 1:
			return _T("Force update");
		case 2:
			return _T("Silence update");
		case 3:
			return _T("Patch update");
		default:
			return _T("None update");
		}
	}

	bool C_CmdLogin::CheckUpdate(I_EIMLogger* pILog, I_ClientAgent* pICa, I_EIMEngine* pIEgn, I_EIMEventMgr* pIEMgr)
	{
		// Check new version
		char	szVer[20] = { 0 };
		char	szURL[EIM_VERSION_URL_SIZE] = { 0 };
		S_CheckVersion	sCheckVer = { 0 };
		LOGINACCESSACK sLoginAck;
		memset(&sLoginAck, 0, sizeof(LOGINACCESSACK));
		if(!pICa->GetVersionInfo(&sLoginAck))
		{
			DWORD dwVer[4] = { 0 };
			sscanf( sLoginAck.szLatestVer, "%d.%d.%d", &dwVer[0], &dwVer[1], &dwVer[2] );
			sCheckVer.u8Major= (BYTE)dwVer[0];
			sCheckVer.u8Minor= (BYTE)dwVer[1];
			sCheckVer.u16Rev = (WORD)dwVer[2];

			sCheckVer.i32Flag = int(sLoginAck.UpgradeType);
			//sCheckVer.i32Flag = 1; //test

			eIMString	szURL_, szDesc_, szVer_;
			eIMMByte2TChar( sLoginAck.tUpgradeFileUrl.value,    szURL_ );
			eIMMByte2TChar(sLoginAck.tLatestVerDesc.value,      szDesc_);
			_tcsncpy( sCheckVer.szUpdateURL,   szURL_.c_str(),  EIM_VERSION_URL_SIZE -1);
			_tcsncpy( sCheckVer.szVerDesc,     szDesc_.c_str(), EIM_VERSION_DESC_SIZE -1);

			EIMLOGGER_INFO_( pILog, _T("Check version succeeded! Update type:%d(%s), Version:%d.%d.%d, URL:%s"),
				sCheckVer.i32Flag,
				GetUpdateType(sCheckVer.i32Flag),
				sCheckVer.u8Major, sCheckVer.u8Minor, sCheckVer.u16Rev,
				sCheckVer.szUpdateURL );

			SAFE_SEND_EVENT_(pIEMgr, EVENT_CHECK_VERSION, (void*)&sCheckVer);

			eIMString szOldVer, szNewVer;
			//::eIMTChar2Mbyte(pIEgn->GetAttributeStr(IME_ATTRIB_APP_VERSION), szOldVerA);
			if(hpi.UIGetAppVersion())
				szOldVer = hpi.UIGetAppVersion();

			eIMMByte2TChar(sLoginAck.szLatestVer, szNewVer);
			eIMString szSkipVer = pIEgn->GetAttributeStr(IME_ATTRIB_SKIP_VERSION, _T(""));
			if ( szSkipVer.empty() || szSkipVer.compare(szNewVer) != 0 )
			{
				eIMString szExePath = PATH_TOKEN_EXE;
				::eIMReplaceToken(szExePath);
				eIMString	  szDllUpdater = szExePath + _T("\\") + EIME_SUBMOD_UPDATER;
				C_PluginDll   DllUpdater(ePLUGIN_TYPE_NORMAL, INAME_EIM_UPDATER );
				I_eIMUpdater* pIUpdater = NULL;
				S_EIMUdpSetup sUdpSetup;
				S_EIMUpdaterInfo sUpdInfo;
				if( sCheckVer.i32Flag && 
					eIMCompareVersion(szNewVer.c_str(), szOldVer.c_str()) > 0)
				{
					if (DllUpdater.Load(szDllUpdater.c_str()) &&
						SUCCEEDED(DllUpdater.eIMCreateInterface(INAME_EIM_UPDATER, (void**)&pIUpdater)))
					{
						memset(&sUdpSetup, 0, sizeof(S_EIMUdpSetup));

						_tcsncpy( sUdpSetup.szDownloadUrl,		szURL_.c_str(),				UPD_DOWN_URL_LEN -1);
						_tcsncpy( sUdpSetup.szMainProgramName,  _T("eim.exe"),				UPD_FILE_NAME_LEN -1);
						_tcsncpy( sUdpSetup.szVerDesc,			szDesc_.c_str(),			UPD_DOWN_URL_LEN -1);

						sUpdInfo.eLanguage = eUPDATER_CN;
						sUpdInfo.eUpdaterType = eUPDATER_SETUP;
						sUpdInfo.seIMUdpSetup = sUdpSetup;

						eIMString		szTxtFile;
						eIMStringA     UpdateTipA;
						eIMString SaveFilePath = ::PathFindFileName(szDesc_.c_str());
						::eIMGetFullPath(PATH_TOKEN_APPDATA, _T("updater\\"),SaveFilePath.c_str(), szTxtFile, FALSE);
						::eIMTChar2MByte(szDesc_.c_str(), UpdateTipA);
						
						Eng._cUrlDownload(UpdateTipA.c_str(),szTxtFile.c_str());  //download pcupdatenotie.txt
						pIUpdater->SetNewFunIntroPath(szTxtFile);
						int i32Ret = pIUpdater->Updater(sUpdInfo);
						if(i32Ret != 0)
						{
							if ( UPD_ERR_CANCEL_UPGRADE == i32Ret && (sCheckVer.i32Flag & 0xFF) != 1  )
								pIEgn->SetAttributeStr(IME_ATTRIB_SKIP_VERSION, szNewVer.c_str());
							else
							{
								if ( m_sData.bReconn == FALSE )
									SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_RET_USERENTER, (void*)EXIT_APP_UPDATE);
								EIMLOGGER_INFO_( pILog, _T("Updater failed, result: 0x%08X, desc:%s"), i32Ret, GetErrorDesc());
								DllUpdater.Unload();
								return false;
							}
						}
						else
						{
							SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_RET_USERENTER, (void*)EXIT_APP_UPDATE);
							SAFE_SEND_EVENT_(pIEMgr, EVENT_EXIT_APP, (void*)EXIT_APP_UPDATE);
							pICa->Disconnect();
							m_i32Result = EIMERR_EXIT_FOR_UPDATE;
							return false;
						}
					}
					else
					{
						EIMLOGGER_INFO_( pILog, _T("pIUpdater pointer is invalid"));
					}
				}
			}
		}
		else
		{
			EIMLOGGER_INFO_( pILog, _T("Check version failed!") );
		}

		return true;
	}

	//C_CmdLoginAck////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdLoginAck, eCMD_LOGIN_ACK);
	int	C_CmdLoginAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode((RESULT)m_sData.ret);	// Save the result
	//	SET_DESC_WHEN_FAIL_(m_i32Result, m_sData.tRetDesc.value);
		EIMLOGGER_LOG_(pILog, SUCCEEDED(m_i32Result) ? eLLDEBUG : eLLERROR,
			_T("cid:%u, qeid:%u, sex:%u, display:%u, edit:%u, lang:%u, CompUpdateTime:%u, DeptUpdateTime:%u, UserUpdateTime:%u, DeptUserUpdateTime:%u,")
			_T("PersonalInfoUpdateTime:%u, PersonalCommonContactUpdateTime:%u, PersonalCommonDeptUpdateTime:%u,")
			_T("PersonalAttentionUpdateTime:%u, GlobalCommonContactUpdateTime:%u, OthersAvatarUpdateTime:%u,")
			_T("PersonalAvatarUpdateTime:%u, RegularGroupUpdateTime:%u, UserRankUpdateTime:%u, UserProUpdateTime:%u,")
			_T("UserAreaUpdateTime:%u, SpecialListUpdatetime:%u, SpecialWhiteListUpdatetime:%u,")
			_T("wPurview:0x%08x{(%d:%d), (%d:%d), (%d:%d), (%d:%d), (%d:%d), (%d:%d), (%d:%d), (%d:%d), (%d:%d), (%d:%d)}"),
			m_sData.dwCompID, m_sData.uUserId, m_sData.sex, m_sData.dwPersonalDisplay, m_sData.dwPersonalEdit, m_sData.cDeptUserLanguageDisplay,
			m_sData.tTimeStamp.dwCompUpdateTime,				m_sData.tTimeStamp.dwDeptUpdateTime, 
			m_sData.tTimeStamp.dwUserUpdateTime,				m_sData.tTimeStamp.dwDeptUserUpdateTime,
			m_sData.tTimeStamp.dwPersonalInfoUpdateTime,		m_sData.tTimeStamp.dwPersonalCommonContactUpdateTime, 
			m_sData.tTimeStamp.dwPersonalCommonDeptUpdateTime,	m_sData.tTimeStamp.dwPersonalAttentionUpdateTime,
			m_sData.tTimeStamp.dwGlobalCommonContactUpdateTime, m_sData.tTimeStamp.dwOthersAvatarUpdateTime,	
			m_sData.tTimeStamp.dwPersonalAvatarUpdateTime,		m_sData.tTimeStamp.dwRegularGroupUpdateTime, 
			m_sData.tTimeStamp.dwUserRankUpdateTime,			m_sData.tTimeStamp.dwUserProUpdateTime,			 
			m_sData.tTimeStamp.dwUserAreaUpdateTime,			m_sData.tTimeStamp.dwSpecialListUpdatetime, 
			m_sData.tTimeStamp.dwSpecialWhiteListUpdatetime,	m_sData.wPurview,	
			m_sData.mPurview[0].dwID, m_sData.mPurview[0].dwParameter, m_sData.mPurview[1].dwID, m_sData.mPurview[1].dwParameter,
			m_sData.mPurview[2].dwID, m_sData.mPurview[2].dwParameter, m_sData.mPurview[3].dwID, m_sData.mPurview[3].dwParameter,
			m_sData.mPurview[4].dwID, m_sData.mPurview[4].dwParameter, m_sData.mPurview[5].dwID, m_sData.mPurview[5].dwParameter,
			m_sData.mPurview[6].dwID, m_sData.mPurview[6].dwParameter, m_sData.mPurview[7].dwID, m_sData.mPurview[7].dwParameter,
			m_sData.mPurview[8].dwID, m_sData.mPurview[8].dwParameter, m_sData.mPurview[9].dwID, m_sData.mPurview[9].dwParameter);

		return EIMERR_NO_ERROR;
	}

	int	C_CmdLoginAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		C_eIMEngine& eng = Eng;
		// By C_eIMEngine to dealwith the data of LOGINACK
		if ( m_sData.ret == RESULT_SUCCESS )
			eng.InitEnv(&m_sData);

		if ( m_sData.tAuthToken.len > 0 )
		{	// SSO Token for WanDA's WebApp
			typedef struct tagDesUserInfo
			{
				char username[128];
				char password[64];
			}S_DesUserInfo, *PS_DesUserInfo;

			PS_DesUserInfo psToken = (PS_DesUserInfo)(m_sData.tAuthToken.value);
			eIMString szName;
			eIMString szPsw;
			eIMString szTimeStamp;

			::eIMUTF8ToTChar(psToken->username, szName);
			::eIMUTF8ToTChar(psToken->password, szPsw);
			EIMLOGGER_INFO_( pILog, _T("SSO Token UserName:%s, PSW:%s"), szName.c_str(), szPsw.c_str());

			eng.SetAttributeStr(IME_ATTRIB_SM_USER, szName.c_str());
			eng.SetAttributeStr(IME_ATTRIB_SM_KEY, szPsw.c_str());
			if ( szName.size() > 0 && szPsw.size() > 0)
			{
				eIMString szSSOParams;
				C_UrlHelper<TCHAR> url1(URL_X_SPACE_FLAG);
				C_UrlHelper<TCHAR> url2(URL_X_SPACE_FLAG);

				szSSOParams  = _T("?param1=");
				szSSOParams += url1.Encode(szName.c_str());
				szSSOParams += _T("&param2=");
				szSSOParams += url2.Encode(szPsw.c_str());
				szSSOParams += _T("&param3=");
				szSSOParams += GetMd5Param3();

				eng.SetAttributeStr(IME_ATTRIB_SSO_PARAMS, szSSOParams.c_str());
			}
		}

		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_AFTER, (void*)m_i32Result);

		return EIMERR_NO_ERROR;
	}

	eIMString C_CmdLoginAck::GetMd5Param3()
	{
		int		i32Index = 0;
		char	szCalcMd5[256]	= { 0 };
		char	szMd5[33]		= { 0 };
		const TCHAR* pszLoginUserName = Eng.GetAttributeStr(IME_ATTRIB_USER_NAME);
		while( pszLoginUserName[i32Index] )
		{
			szCalcMd5[i32Index] = tolower(pszLoginUserName[i32Index]);
			i32Index++;
		}

		strcat(szCalcMd5, "wdwx");	
		eIMString szParam3;
		//	snprintf(szCalcMd5, COUNT_OF_ARRAY_(szCalcMd5), "%Swdwx%S", url1.GetUrl(), url2.GetUrl());	// Make string to calc md5, format as: MD5(param1+"wdwx"+param2),
		Create32Md5((unsigned char*)szCalcMd5, strlen(szCalcMd5), (unsigned char*)szMd5);			// calc md5
		::eIMUTF8ToTChar(szMd5, szParam3);

		return szParam3;
	}

	//C_CmdLogout//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdLogout, eCMD_LOGOUT);
	int	C_CmdLogout::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		if(m_sData.cStatus == eENGINE_STATUS_OFFLINE || m_sData.cStatus == eENGINE_STATUS_QUIT)
		{
			m_i32Result = pICa->Disconnect();
		}
		else
		{
			m_i32Result = pICa->Logout(m_sData.cStatus, m_sData.cManual);
		}
		if (FAILED(m_i32Result))
		{
			EIMLOGGER_ERROR_( pILog, _T("UserID: %d, Status: %d, LoginType: %d, Manual: %d, Logout failed, result: 0x%08X, desc:%s"), 
				m_sData.dwUserID, m_sData.cStatus, m_sData.cLoginType, m_sData.cManual , m_i32Result, GetErrorDesc());
		}
		else
		{
			EIMLOGGER_INFO_( pILog, _T("UserID: %d, Status: %d, LoginType: %d, Manual: %d, Logout succeeded"), 
			m_sData.dwUserID, m_sData.cStatus, m_sData.cLoginType, m_sData.cManual , m_i32Result);
		}

		return m_i32Result;
	}

	int C_CmdLogout::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		if (FAILED(m_i32Result))
		{
			S_LoginStatus sLoginStatus = { 0 };

			sLoginStatus.dwStatus = m_i32Result;
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
		}

		return m_i32Result;
	}

	//C_CmdLogoutAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdLogoutAck, eCMD_LOGOUT_ACK);
	int	C_CmdLogoutAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if (FAILED(m_i32Result))
			EIMLOGGER_ERROR_( pILog, _T("Logout failed, status:%d, result: 0x%08X, desc:%s"), m_sData.cStatus, m_i32Result, GetErrorDesc());
		else
			EIMLOGGER_DEBUG_( pILog, _T("Logout succeeded, status:%d"), m_sData.cStatus);

		return m_i32Result;
	}

	int	C_CmdLogoutAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort )
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		S_LoginStatus sLoginStatus = { m_i32Result };

		if (SUCCEEDED(m_i32Result))
		{
			if (m_sData.cStatus == 1)
				sLoginStatus.dwStatus = EIMERR_STATUS_ONLINE;
			else if (m_sData.cStatus == 2)
				sLoginStatus.dwStatus = EIMERR_STATUS_ONLEAVE;
			else if (m_sData.cStatus == 0)
				sLoginStatus.dwStatus = EIMERR_STATUS_OFFLINE;
		}		

		SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sLoginStatus);
		return m_i32Result;
	}
	
	//C_CmdNoticeState//////////////////////////////////////////////////////////////////
	//REGISTER_CMDCLASS(C_CmdNoticeState, eCMD_NOTICE_STATE);// 去除了此模式使用C_CmdTGetStatusRsq来应答
	int	C_CmdNoticeState::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdNoticeState::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

		int				 i32Ret = 0;
		UINT32			 u32StartPos = 0;
		USERSTATUSNOTICE sNotice;
		S_EmpInfo		 sEmp = { 0 };
		C_eIMContacts&	 obj = C_eIMContacts::GetObject();

		while(SUCCEEDED(i32Ret = pICa->ParseUserStatusSetNotice(m_sData.strPacketBuff, &u32StartPos, &sNotice)))
		{
			PARSE_FINISHED_BREAK_(i32Ret);
			sEmp.Id			  = sNotice.dwUserID;
			sEmp.bitStatus	  =(sNotice.cStatus & IME_EMP_STATUS_MASK);
			sEmp.bitLoginType =(sNotice.cLoginType & IME_EMP_TERM_MASK);

			EIMLOGGER_DEBUG_(pILog, _T("Update user[%d]'s status, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
			if (!obj.SetEmps(sEmp, eIMECSET_MODS, TRUE))
				EIMLOGGER_ERROR_(pILog, _T("Update user[%d]'s status failed, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
		}

		if (FAILED(i32Ret) && i32Ret != EIMERR_PARSE_FINISHED)
			EIMLOGGER_ERROR_(pILog, _T("Package error, result: 0x%08X, desc:%s"), i32Ret, GetErrorDesc());

		return m_i32Result;
	}

	//- eCMD_NOTICE_STATE_ACK Not used
	//- eCMD_ALIVE Not used
	//- eCMD_ALIVE_ACK Not used
	//C_CmdModifyInfo//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyInfo, eCMD_MODIFY_INFO);
	int	C_CmdModifyInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->ModiInfo(m_sData.cModiType, m_sData.cLen, (char*)m_sData.aszModiInfo);
		CMD_TRACE_STR_(m_szXml, m_i32Result);

		return m_i32Result;
	}

	//C_CmdModifyInfoAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyInfoAck, eCMD_MODIFY_INFO_ACK);
	int	C_CmdModifyInfoAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdModifyInfoAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_INFO_ACK, (void*)m_i32Result);
		return m_i32Result;
	}

	//C_CmdModifyInfoNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyInfoNotice, eCMD_MODIFY_INFO_NOTICE);
	int	C_CmdModifyInfoNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		EIMLOGGER_DEBUG_(pILog, _T("UserID: %d, ModiType: %d"), m_sData.dwUserID, m_sData.cModiType);
		return m_i32Result;
	}

	int	C_CmdModifyInfoNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		CHECK_NULL_RET_(pIDb, EIMERR_INVALID_PARAM);
		CHECK_FAILED_RET_(m_i32Result, m_i32Result);

		C_eIMEngine& eng = Eng;
		C_eIMContactsMgr cmgr;
		cmgr.Init(pIDb);
		m_i32Result = cmgr.Set(&m_sData.sEmployee);


		S_EmpInfo	   sEmp = { 0 };
		C_eIMContacts& obj = C_eIMContacts::GetObject();
		PS_DeptInfo_   psDeptInfo = obj.GetEmpDept( m_sData.dwUserID );

		if (psDeptInfo && 
			eng.Emp2EmpInfo(&m_sData.sEmployee, &sEmp, psDeptInfo->Id) )
			obj.SetEmps(sEmp, eIMECSET_ADD, TRUE);

		return m_i32Result;
	}

	int	C_CmdModifyInfoNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, (void*)m_i32Result);
		return m_i32Result;
	}

	//- ECMD_MODIFY_INFO_NOTICE_ACK Not Used
	//C_CmdGetCoInfo//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetCoInfo, eCMD_GET_CO_INFO);
	int	C_CmdGetCoInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetCompInfo();
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetCoInfoAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetCoInfoAck, eCMD_GET_CO_INFO_ACK);
	int	C_CmdGetCoInfoAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		if ( SUCCEEDED(m_i32Result))
		{
			eIMString szValue;
			S_CoInfo  sCoInfo = { 0 };

			sCoInfo.dwId      = m_sData.sCompInfo.dwCompID;
			sCoInfo.dwEstTime = m_sData.sCompInfo.dwEstablish_time;
			memcpy(sCoInfo.au8Code, m_sData.sCompInfo.aszCompCode, sizeof(sCoInfo.au8Code));
			memcpy(sCoInfo.au8Logo, m_sData.sCompInfo.aszLogo, sizeof(sCoInfo.au8Logo));

			::eIMUTF8ToTChar((char*)m_sData.sCompInfo.aszCompName, szValue);
			_tcsncpy(sCoInfo.szName, szValue.c_str(), IME_CO_NAME_SIZE -1);

			C_eIMContacts::GetObject().SetCoInfo(&sCoInfo);
		}

		return m_i32Result;
	}

	//- eCMD_GET_CO_INFO_NOTICE Not Used
	//- eCMD_GET_CO_INFO_NOTICE_ACK Not Used
	//C_CmdGetDeptInfo//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetDeptList, eCMD_GET_DEPT_LIST);
	int	C_CmdGetDeptList::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetDeptInfo(m_sData, TERMINAL_PC);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetDeptListAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetDeptListAck, eCMD_GET_DEPT_LIST_ACK);
	int	C_CmdGetDeptListAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if (FAILED(m_i32Result)) 
		{
			EIMLOGGER_ERROR_( pILog, _T("Failed, result: 0x%08X, desc:%s"), m_i32Result, GetErrorDesc()); 
		}
		else 
		{
			if ( pILog && pILog->GetLogLevel() == eLLDATA )
				EIMLOGGER_DATA_( pILog, (BYTE*)&m_sData, m_sData.nPacketLen, _T("Length:%d, Page:%d, Count:%d, succeeded"), m_sData.nPacketLen, m_sData.wCurrPage, m_sData.wCurrNum); 
			else
				EIMLOGGER_DEBUG_( pILog, _T("Length:%d, Page:%d, Count:%d, succeeded"), m_sData.nPacketLen, m_sData.wCurrPage, m_sData.wCurrNum); 
		}

		return m_i32Result;
	}

	int	C_CmdGetDeptListAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		CHECK_FAILED_RET_(m_i32Result, m_i32Result);
	
		// Cached
		UINT32	 u32StartPos = 0;
		DEPTINFO sDeptInfo   = { 0 };
		if (s_vectDeptInfo.capacity() < CONTS_CAPACITY_MAX)
			s_vectDeptInfo.reserve(CONTS_CAPACITY_MAX);

		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		while(SUCCEEDED(m_i32Result = pICa->ParseDeptInfo(m_sData.strPacketBuff, &u32StartPos, &sDeptInfo)) )
		{
			PARSE_FINISHED_BREAK_(m_i32Result);
			s_vectDeptInfo.push_back(sDeptInfo);
		}

		if ( m_sData.wCurrPage && s_vectDeptInfo.size() < CONTS_CACHESIZE_MAX )	// Not reach at end
		{
			return EIMERR_NO_ERROR;
		}
		else
		{
			C_eIMContactsMgr cmgr;

			cmgr.Init(pIDb);
			m_i32Result = cmgr.Set(s_vectDeptInfo);
			s_vectDeptInfo.clear();

			if ( m_sData.wCurrPage == 0 )	// Last package, to start DeptEmp
				CLEAR_VECTOR_(s_vectDeptInfo, VectDeptInfo);

			return m_i32Result;
		}
	}

	int	C_CmdGetDeptListAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		
		if (  m_sData.wCurrPage == 0 )
		{
			S_LoginStatus sStatus = { EIMERR_UPDATE_DEPT_OK };
			_tcsncpy(sStatus.szStatus, GetErrorDesc(EIMERR_UPDATE_DEPT_OK), COUNT_OF_ARRAY_(sStatus.szStatus));
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_DEPT_INFO_END, (void*)m_i32Result);
		}
		else
		{
			S_LoginStatus sStatus = { EIMERR_UPDATE_DEPT };
			_sntprintf(sStatus.szStatus, COUNT_OF_ARRAY_(sStatus.szStatus), GetErrorDesc(EIMERR_UPDATE_DEPT), m_sData.wCurrPage);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
		}

		return m_i32Result;
	}
	
	//- eCMD_GET_DEPT_LIST_NOTICE Not used
	//- eCMD_GET_DEPT_LIST_NOTICE_ACK Not used
	//C_CmdGetUserList//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetUserList, eCMD_GET_USER_LIST);
	int	C_CmdGetUserList::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetUserList(m_sData, TERMINAL_PC);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetUserListAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetUserListAck, eCMD_GET_USER_LIST_ACK);

	int	C_CmdGetUserListAck::DoCmd(I_EIMLogger* pILog, void* pvCa,	BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if (FAILED(m_i32Result)) 
		{
			EIMLOGGER_ERROR_( pILog, _T("Failed, result: 0x%08X, desc:%s"), m_i32Result, GetErrorDesc()); 
		}
		else 
		{
			if ( pILog && pILog->GetLogLevel() == eLLDATA )
				EIMLOGGER_DATA_( pILog, (BYTE*)&m_sData, m_sData.nPacketLen, _T("Length:%d, Page:%d, Count:%d, succeeded"), m_sData.nPacketLen, m_sData.wCurrPage, m_sData.wCurrNum); 
			else
				EIMLOGGER_DEBUG_( pILog, _T("Length:%d, Page:%d, Count:%d, succeeded"), m_sData.nPacketLen, m_sData.wCurrPage, m_sData.wCurrNum); 
		}

		return m_i32Result;
	}

	int	C_CmdGetUserListAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		CHECK_FAILED_RET_(m_i32Result, m_i32Result);

		// Cached
		UINT32	 u32StartPos = 0;
		USERINFO sUserInfo = { 0 };
		if (s_vectUserInfo.capacity() < CONTS_CAPACITY_MAX)
			s_vectUserInfo.reserve(CONTS_CAPACITY_MAX);

		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		while(SUCCEEDED(m_i32Result = pICa->ParseUserInfo(m_sData.strPacketBuff, &u32StartPos, &sUserInfo)) )
		{
			PARSE_FINISHED_BREAK_(m_i32Result);
			s_vectUserInfo.push_back(sUserInfo);
		}

		if ( m_sData.wCurrPage && s_vectUserInfo.size() < CONTS_CACHESIZE_MAX )	// Reach at end or < 4K
		{
			return m_i32Result;
		}
		else
		{
			C_eIMContactsMgr cmgr;

			cmgr.Init(pIDb);
			m_i32Result = cmgr.Set(s_vectUserInfo);
			s_vectUserInfo.clear();

			if ( m_sData.wCurrPage == 0 )
			{	// Last package, to start DeptEmp
				CLEAR_VECTOR_(s_vectUserInfo, VectUserInfo);
				EIMLOGGER_DEBUG_( pILog, _T("Add emps to db end")); 
			}

			return m_i32Result;
		}
	}

	int	C_CmdGetUserListAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		if (  m_sData.wCurrPage == 0 )
		{
			S_LoginStatus sStatus = { EIMERR_UPDATE_EMP_OK };
			_tcsncpy(sStatus.szStatus, GetErrorDesc(EIMERR_UPDATE_EMP_OK), COUNT_OF_ARRAY_(sStatus.szStatus));
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_EMPS_INFO_END, (void*)m_i32Result);
		}
		else
		{
			S_LoginStatus sStatus = { EIMERR_UPDATE_EMP };
			_sntprintf(sStatus.szStatus, COUNT_OF_ARRAY_(sStatus.szStatus), GetErrorDesc(EIMERR_UPDATE_EMP), m_sData.wCurrPage);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
		}

		return m_i32Result;
	}

	//C_CmdGetDeptEmps//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetDeptEmps, eCMD_GET_DEPT_USER);
	int	C_CmdGetDeptEmps::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetUserDept(m_sData, TERMINAL_PC);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetDeptEmpsAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetDeptEmpsAck, eCMD_GET_DEPT_USER_ACK);
	int C_CmdGetDeptEmpsAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if (FAILED(m_i32Result)) 
		{
			EIMLOGGER_ERROR_( pILog, _T("Failed, result: 0x%08X, desc:%s"), m_i32Result, GetErrorDesc()); 
		}
		else 
		{
			if ( pILog && pILog->GetLogLevel() == eLLDATA )
				EIMLOGGER_DATA_( pILog, (BYTE*)&m_sData, m_sData.nPacketLen, _T("Length:%d, Page:%d, Count:%d, succeeded"), m_sData.nPacketLen, m_sData.wCurrPage, m_sData.wCurrNum); 
			else
				EIMLOGGER_DEBUG_( pILog, _T("Length:%d, Page:%d, Count:%d, succeeded"), m_sData.nPacketLen, m_sData.wCurrPage, m_sData.wCurrNum); 
		}

		return m_i32Result;
	}

	int	C_CmdGetDeptEmpsAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		CHECK_FAILED_RET_(m_i32Result, m_i32Result);

		// Cached 
		UINT32	 u32StartPos = 0;
		USERDEPT sUserDept = { 0 };
		if (s_vectUserDept.capacity() < CONTS_CAPACITY_MAX)
			s_vectUserDept.reserve( CONTS_CAPACITY_MAX);

		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		while(SUCCEEDED(m_i32Result = pICa->ParseDeptUserInfo(m_sData.strPacketBuff, &u32StartPos, &sUserDept)) )
		{
			PARSE_FINISHED_BREAK_(m_i32Result);
			s_vectUserDept.push_back(sUserDept);
		}

		if ( m_sData.wCurrPage  && s_vectUserDept.size() < CONTS_CACHESIZE_MAX)	// Not reach at end
		{
			return EIMERR_NO_ERROR;
		}
		else
		{
			C_eIMContactsMgr cmgr;

			cmgr.Init(pIDb);
			m_i32Result = cmgr.Set(s_vectUserDept);
			s_vectUserDept.clear();

			if ( m_sData.wCurrPage == 0 )
			{	// Last package, to start DeptEmp
				CLEAR_VECTOR_(s_vectUserDept, VectUserDept);	// Clear cache memory
			}

			return m_i32Result;
		}
	}

	int	C_CmdGetDeptEmpsAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		if (  m_sData.wCurrPage == 0 )
		{
			S_LoginStatus sStatus = { EIMERR_UPDATE_DEPTEMP_OK };
			_tcsncpy(sStatus.szStatus, GetErrorDesc(EIMERR_UPDATE_DEPTEMP_OK), COUNT_OF_ARRAY_(sStatus.szStatus));
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_DEPT_EMPS_END, (void*)m_i32Result);
		}
		else
		{
			S_LoginStatus sStatus = { EIMERR_UPDATE_DEPTEMP };
			_sntprintf(sStatus.szStatus, COUNT_OF_ARRAY_(sStatus.szStatus), GetErrorDesc(EIMERR_UPDATE_DEPTEMP), m_sData.wCurrPage);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOGIN_STATUS, &sStatus);
		}

		return m_i32Result;
	}


	//C_CmdGetEmpInfo//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetEmpInfo, eCMD_GET_EMPLOYEE_INFO);
	int C_CmdGetEmpInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetEmployee(m_sData, 0);	//0：手动触发(默认) 1：后台主动触发(目前不关心这个类型)
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	BOOL C_CmdGetEmpInfo::FromXml( const TCHAR* pszXml )
	{
		TiXmlDocument xmlDoc;
		TiXmlElement* pEl = ParseXmlCmdHeader(xmlDoc, pszXml, GetCmdId(), m_i32Result);
		CHECK_NULL_RET_(pEl, FALSE);

		if (!pEl->Attribute(FIELD_USER_ID, (int*)&m_sData))
			m_i32Result = EIMERR_BAD_CMD;

		return m_i32Result;
	}


	//C_CmdGetEmpInfoAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetEmpInfoAck, eCMD_GET_EMPLOYEE_INFO_ACK);
	int C_CmdGetEmpInfoAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}
	
	int	C_CmdGetEmpInfoAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		CHECK_FAILED_RET_(m_i32Result, m_i32Result);

		C_eIMEngine&	 eng = Eng;
		C_eIMContacts&	 obj = C_eIMContacts::GetObject();
	//	C_eIMContactsMgr cmgr;

		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		UINT32 uiPos = 0;
		EMPLOYEE rInfo;
		if(FAILED(pICa->ParseEmployee(m_sData.strPacketBuff, &uiPos, &rInfo)))
		{
			EIMLOGGER_ERROR_(pILog,_T("prase emploee error  empid=%u"), m_sData.dwUserID);
		}
		
		if(m_sData.dwUserID == eng.GetPurview(GET_LOGIN_QEID))
		{
			ZERO_STRUCT_(m_sEmpInfo);
			eng.Emp2EmpInfo(&rInfo, &m_sEmpInfo, 0);
		}
		else
		{
			ZERO_STRUCT_(m_sEmpInfo);
			PS_DeptInfo_   psDeptInfo = obj.GetEmpDept( m_sData.dwUserID);

			if ( psDeptInfo && eng.Emp2EmpInfo(&rInfo, &m_sEmpInfo, psDeptInfo->Id) )
				obj.SetEmps(m_sEmpInfo, eIMECSET_ADD, TRUE);
		}

		return m_i32Result;
	}

	int	C_CmdGetEmpInfoAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr,  m_i32CmdId, &m_sEmpInfo);
		return m_i32Result;
	}
	
	//C_CmdCreateGroup//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCreateGroup, eCMD_CREATE_GROUP);
	int C_CmdCreateGroup::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		if (SUCCEEDED(m_i32Result))
		{
			m_i32Result = pICa->CreateGroup(m_sData.aszGroupID, m_sData.aszGroupName, strlen(m_sData.aszGroupName), (char*)m_sData.aUserID, m_sData.wUserNum, m_sData.dwTime);
			QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
			SetBusinessTypeId(qsid);
			C_CreateGroupWaitOverItem* pBaseWaitItem = new C_CreateGroupWaitOverItem();
			pBaseWaitItem->SetData(this);
			pBaseWaitItem->SetStartTime(GetTickCount());
			C_UIWaitOverMgr::GetObject().AddWaitOverItem(pBaseWaitItem);
		}

		if (FAILED(m_i32Result)) 
			EIMLOGGER_ERROR_( pILog, _T("Failed, %s, result: 0x%08X, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc()); 
		else 
			EIMLOGGER_DEBUG_( pILog, _T("Succeeded, %s"), m_szXml.c_str());

		return m_i32Result;
	}

	int C_CmdCreateGroup::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMSessionMgr smgr;
		C_eIMMsgMgr mmgr;

		eIMString		szValue;
		S_SessionInfo	sSInfo = { 0 };

		smgr.Init(pIDb);
		mmgr.Init(pIDb);

		sSInfo.dwCreateTime = 0;
		sSInfo.dwSUpdateTime = m_sData.dwTime;
		sSInfo.eType		= eSESSION_TYPE_MULTI;
		sSInfo.qsid			= GroupId2Qsid(m_sData.aszGroupID);
		sSInfo.CreaterId	= m_sData.dwUserID;
		SET_UTF8_2TVALUE_(m_sData.aszGroupName, sSInfo.szTitle);

		DWORD dwCount = mmgr.GetCount(sSInfo.qsid,0,0xFFFFFFFF);
		if( dwCount == 0 )
		{
			sSInfo.dwUpdateTime = 0;
			sSInfo.eFlag		= (E_SEFlag)(eSEFLAG_DELETE | eSEFLAG_MULTI);
		}
		else
		{
			sSInfo.eFlag		= (E_SEFlag)(eSEFLAG_MULTI);
			sSInfo.dwUpdateTime = mmgr.GetSessionLastMsgTime(sSInfo.qsid);
		}

		smgr.Set(&sSInfo);
		m_i32Result = smgr.Set(sSInfo.qsid, (PQEID)m_sData.aUserID, m_sData.wUserNum, TRUE, TRUE);

		return m_i32Result;
	}

	int	C_CmdCreateGroup::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		CHECK_FAILED_SEND_EVENT_RET_(EVENT_CREATE_GROUP, (void*)this);
		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdCreateGroup::FromXml( const TCHAR* pszXml )
	{
		int				i32Value;
		const char*		pszValue = NULL;
		eIMStringA		szValueA;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);
		
		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;

		m_sData.dwUserID = pEl->Attribute(FIELD_USER_ID, &i32Value) ? i32Value : eng.GetPurview(GET_LOGIN_QEID);
		m_sData.dwTime   = pEl->Attribute(FIELD_TIMESTAMP, &i32Value) ? i32Value : eng.GetTimeNow();
		
		pszValue = pEl->Attribute(FIELD_GROUP_ID);
		SET_GROUP_ID_(pszValue, m_sData.aszGroupID);
		SET_UTF8_VALUE_(pEl->Attribute(FIELD_GROUP_NAME), m_sData.aszGroupName);

		for( pEl = pEl->FirstChildElement(); pEl && m_sData.wUserNum < MAXNUM_PAGE_USERID; pEl = pEl->NextSiblingElement() )
		{
			if ( pEl->Attribute(FIELD_USER_ID, &i32Value ) )
				m_sData.aUserID[m_sData.wUserNum++] = i32Value;
		}

		m_szXml = pszXml;
		return TRUE;
	}

	//C_CmdCreateGroupAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCreateGroupAck, eCMD_CREATE_GROUP_ACK);
	int C_CmdCreateGroupAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);

		QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
		
		if (FAILED(m_i32Result)) 
			EIMLOGGER_ERROR_( pILog, _T("Failed, %s, result: 0x%08X, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc()); 
		else 
			EIMLOGGER_DEBUG_( pILog, _T("Succeeded, %s"), m_szXml.c_str());

		C_UIWaitOverMgr::GetObject().DelWaitOverItem(eWaitType_Group,qsid);

		return EIMERR_NO_ERROR;
	}

	int C_CmdCreateGroupAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMSessionMgr smgr;
		smgr.Init(pIDb);

		if ( SUCCEEDED(m_i32Result) || m_i32Result == EIMERR_GROUP_EXISTED )
		{	// Create group succeeded, clear the eSEFLAG_DELETE flag
			QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
			smgr.UpdateFlag(qsid, eSEFLAG_DELETE, FALSE);

			S_SessionInfo sSessionInfo;
			ZERO_STRUCT_(sSessionInfo);
			BOOL bHas = smgr.Get(qsid,&sSessionInfo);
			if(bHas)
			{
				sSessionInfo.dwCreateTime = m_sData.dwTime;
				sSessionInfo.dwSUpdateTime= m_sData.dwTime;
				smgr.Set(&sSessionInfo);
			}
		}

		return EIMERR_NO_ERROR;
	}

	int	C_CmdCreateGroupAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
	//	SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		SAFE_SEND_EVENT_(pIEMgr,EVENT_CREATE_GROUP_ACK, (void*)this);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdCreateGroupAck::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		//if ( SUCCEEDED(m_i32Result) )
		{
			TiXmlElement El(PROTOCOL_ELEMENT_CMD);
			El.SetAttribute(PROTOCOL_ATTRIB_ID, m_i32CmdId);
			SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
			El.SetAttribute(FIELD_GROUP_NAME,	m_sData.aszGroupName);
			El.SetAttribute(FIELD_TIMESTAMP,	m_sData.dwTime);
			El.SetAttribute(FIELD_COUNT, m_sData.wUserNum);
			for ( int n = 0; n < m_sData.wUserNum; n++ )
			{
				TiXmlElement e(FIELD_ITEM);
				e.SetAttribute(FIELD_USER_ID, m_sData.aUserID[n]);
				El.InsertEndChild(e);
			}

			ConvertToXml(El, m_szXml);
			return m_szXml.c_str();
		}

		return NULL;
	}

	//C_CmdCreateGroupNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCreateGroupNotice, eCMD_CREATE_GROUP_NOTICE);
	int C_CmdCreateGroupNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int C_CmdCreateGroupNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		C_eIMSessionMgr smgr;
		C_eIMMsgMgr mmgr;

		ZERO_STRUCT_(m_sSInfo);
		smgr.Init(pIDb);
		mmgr.Init(pIDb);
		m_sSInfo.qsid			= GroupId2Qsid(m_sData.aszGroupID);
		if ( smgr.Get(m_sSInfo.qsid, &m_sSInfo) )
		{
			m_sSInfo.eFlag = (E_SEFlag)(m_sSInfo.eFlag | eSEFLAG_MULTI);	
		}
		else
		{
			m_sSInfo.eFlag	= (E_SEFlag)( eSEFLAG_MULTI);	
		}
		m_sSInfo.dwCreateTime = m_sData.dwTime;
		m_sSInfo.dwSUpdateTime= m_sData.dwTime;
		m_sSInfo.eType		  = eSESSION_TYPE_MULTI;
		m_sSInfo.CreaterId	  = m_sData.dwUserID;
		SET_UTF8_2TVALUE_(m_sData.aszGroupName, m_sSInfo.szTitle);

		DWORD dwCount = mmgr.GetCount(m_sSInfo.qsid,0,0xFFFFFFFF);
		if( dwCount == 0 )
			m_sSInfo.dwUpdateTime = 0;
		else
			m_sSInfo.dwUpdateTime = mmgr.GetSessionLastMsgTime(m_sSInfo.qsid);

		smgr.Set(&m_sSInfo);
		m_i32Result = smgr.Set(m_sSInfo.qsid, (PQEID)m_sData.aUserID, m_sData.wUserNum, TRUE, TRUE);
		EIMLOGGER_DEBUG_(pILog, _T("Creater:%d, Qsid:%llu, Title:%s, CreateTime:%d, Num:%d"),
			m_sSInfo.CreaterId, m_sSInfo.qsid, m_sSInfo.szTitle, m_sSInfo.dwCreateTime, m_sData.wUserNum);
		C_eIMSaveData* pSaveData = Eng.GetEIMSaveDataPtr();
		if(pSaveData)
		{
			pSaveData->DelGroupIDReq(m_sSInfo.qsid);
		}


		return m_i32Result;
	}

	int	C_CmdCreateGroupNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_GROUP_NOTICE, &m_sSInfo.qsid);
		return EIMERR_NO_ERROR;
	}

	//- eCMD_CREATE_GROUP_NOTICE_ACK,
	//C_CmdModifyGroup//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyGroup, eCMD_MODIFY_GROUP);
	int C_CmdModifyGroup::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		if ( m_sData.cType == MODIFY_GROUP_TYPE_NAME && SUCCEEDED(m_i32Result) && m_sData.aszData[0] )
		{
			m_i32Result = pICa->ModiGroup(m_sData.aszGroupID, m_sData.aszData, m_sData.cType, m_sData.dwTime);
		}
	
		//if (SUCCEEDED(m_i32Result) && m_sModifyNote.aszData[0])
			//m_i32Result = pICa->ModiGroup(m_sModifyNote.aszGroupID, m_sModifyNote.aszData, m_sModifyNote.cType, m_sModifyNote.dwTime);
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int	C_CmdModifyGroup::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		if ( m_sData.cType == MODIFY_GROUP_TYPE_NOTE )
		{
			eIMString		szValue;
			::eIMUTF8ToTChar(m_sData.aszData, szValue);

			C_eIMSessionMgr smgr;
			smgr.Init(pIDb);
			smgr.SetRemark(GroupId2Qsid(m_sData.aszGroupID), szValue.c_str());
		}
		else if ( m_sData.cType == MODIFY_GROUP_TYPE_NAME )
		{
			eIMString		szValue;
			::eIMUTF8ToTChar(m_sData.aszData, szValue);

			C_eIMSessionMgr smgr;
			smgr.Init(pIDb);
			smgr.SetTitle(GroupId2Qsid(m_sData.aszGroupID), szValue.c_str());
		}
		else
		{
			ASSERT_(FALSE);
		}
		return EIMERR_NO_ERROR;
	}

	int	C_CmdModifyGroup::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		if ( m_sData.cType == MODIFY_GROUP_TYPE_NOTE )
		{
			QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
			SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_GROUP_REMARK_ACK, (void*)&qsid);
		}
		else if(m_sData.cType == MODIFY_GROUP_TYPE_NAME)
		{
			if(SUCCEEDED(m_i32Result) && m_sData.aszData[0])
				CHECK_FAILED_SEND_EVENT_RET_(EVENT_MODIFY_GROUP, (void*)this);
		}
		
		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdModifyGroup::FromXml( const TCHAR* pszXml )
	{
		int				i32Value;
		const char*		pszValue = NULL;
		eIMStringA		szValueA;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);
		
		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;
		m_sData.dwUserID = pEl->Attribute(FIELD_USER_ID,   &i32Value) ? i32Value : eng.GetPurview(GET_LOGIN_QEID);
		m_sData.dwTime   = pEl->Attribute(FIELD_TIMESTAMP, &i32Value) ? i32Value : eng.GetTimeNow();
		
		pszValue = pEl->Attribute(FIELD_GROUP_ID);
		SET_GROUP_ID_(pszValue, m_sData.aszGroupID);
		
		ASSERT_(pEl);
		for (pEl = pEl->FirstChildElement(); pEl; pEl = pEl->NextSiblingElement())
		{	// If more than once of the same type item, the last exist is valid
			pszValue = pEl->Attribute(FIELD_TYPE, &i32Value);
			ASSERT_(pszValue);
			if (i32Value == MODIFY_GROUP_TYPE_NAME)
			{
				m_sData.cType	 = MODIFY_GROUP_TYPE_NAME;
			}
			else if(i32Value == MODIFY_GROUP_TYPE_NOTE)
			{
				m_sData.cType	 = MODIFY_GROUP_TYPE_NOTE;
			}
			else
			{
				ASSERT_(FALSE);
			}
			SET_UTF8_VALUE_(pEl->Attribute(FIELD_DATA), m_sData.aszData);
			break;
		}

		m_szXml = pszXml;
		return m_i32Result;
	}

	//C_CmdModifyGroupAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyGroupAck, eCMD_MODIFY_GROUP_ACK);
	int C_CmdModifyGroupAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int C_CmdModifyGroupAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		C_eIMSessionMgr smgr;

		::eIMUTF8ToTChar(m_sData.aszData, szValue);
		smgr.Init(pIDb);
		smgr.SetTitle(GroupId2Qsid(m_sData.aszGroupID), szValue.c_str());

		return EIMERR_NO_ERROR;
	}

	int	C_CmdModifyGroupAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		//CHECK_FAILED_SEND_EVENT_RET_(EVENT_MODIFY_GROUP_ACK, (void*)this);
		QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_GROUP_NAME_ACK, (void*)&qsid);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdModifyGroupAck::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if ( SUCCEEDED(m_i32Result) )
		{
			TiXmlElement El(PROTOCOL_ELEMENT_CMD);
			TiXmlElement ItemEl(FIELD_ITEM);

			El.SetAttribute(PROTOCOL_ATTRIB_ID, m_i32CmdId);
			SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
			El.SetAttribute(FIELD_TIMESTAMP, m_sData.dwTime);
			
			ItemEl.SetAttribute(FIELD_TYPE, m_sData.cType);
			ItemEl.SetAttribute(FIELD_DATA, m_sData.aszData);

			El.InsertEndChild(ItemEl);

			ConvertToXml(El, m_szXml);
			return m_szXml.c_str();
		}

		return NULL;
	}
	
	//C_CmdModifyGroupNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyGroupNotice, eCMD_MODIFY_GROUP_NOTICE);
	int C_CmdModifyGroupNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int C_CmdModifyGroupNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		C_eIMSessionMgr smgr;

		::eIMUTF8ToTChar((char*)m_sData.aszData, szValue);
		smgr.Init(pIDb);
		smgr.SetTitle(GroupId2Qsid(m_sData.aszGroupID), szValue.c_str());

		return EIMERR_NO_ERROR;
	}

	int	C_CmdModifyGroupNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_GROUP_NOTICE, &qsid);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdModifyGroupNotice::ToXml()
	{
		TiXmlElement El(PROTOCOL_ELEMENT_CMD);
		TiXmlElement ItemEl(FIELD_ITEM);

		El.SetAttribute(FIELD_ID, m_i32CmdId);
		El.SetAttribute(FIELD_USER_ID, m_sData.dwUserID);
		SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
		El.SetAttribute(FIELD_TIMESTAMP, m_sData.dwTime);

		ItemEl.SetAttribute(FIELD_TYPE, m_sData.cType);
		ItemEl.SetAttribute(FIELD_DATA, (char*)m_sData.aszData);
		
		El.InsertEndChild(ItemEl);
		ConvertToXml(El, m_szXml);
		
		return m_szXml.c_str();
	}

	//C_CmdGetGroupInfo//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetGroupInfo, eCMD_GET_GROUP_INFO);
	int C_CmdGetGroupInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		m_i32Result = pICa->GetGroupInfo(m_sData.aszGroupID);
		if (FAILED(m_i32Result)) 
			EIMLOGGER_ERROR_( pILog, _T("Get qsid:%llu Failed, result: 0x%08X, desc:%s"), GroupId2Qsid(m_sData.aszGroupID), m_i32Result, GetErrorDesc()); 
		else 
			EIMLOGGER_DEBUG_( pILog, _T("Get qsid:%llu Succeeded"), GroupId2Qsid(m_sData.aszGroupID));

		return m_i32Result;
	}

	//C_CmdGetGroupInfoAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetGroupInfoAck, eCMD_GET_GROUP_INFO_ACK);
	int C_CmdGetGroupInfoAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int C_CmdGetGroupInfoAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		S_SessionInfo	sSInfo = { 0 };
		C_eIMSessionMgr smgr;
		C_eIMEngine& eng = Eng;

		smgr.Init(pIDb);
		sSInfo.qsid			= GroupId2Qsid(m_sData.aszGroupID);
		if ( smgr.Get(sSInfo.qsid, &sSInfo) )
		{
			sSInfo.eFlag = (E_SEFlag)(sSInfo.eFlag | eSEFLAG_MULTI);	
		}
		else
		{
			sSInfo.eFlag	= (E_SEFlag)( eSEFLAG_MULTI);	
		}

		sSInfo.dwCreateTime = m_sData.dwTime;
		sSInfo.dwSUpdateTime= m_sData.dwTime;
		sSInfo.eType		= eSESSION_TYPE_MULTI;
		sSInfo.CreaterId	= m_sData.dwCreaterID;
		SET_UTF8_2TVALUE_((const char*)m_sData.aszGroupName, sSInfo.szTitle);
		SET_UTF8_2TVALUE_((const char*)m_sData.aszGroupNote, sSInfo.szRemark);

		m_i32Result = smgr.Set(&sSInfo);
		if (SUCCEEDED(m_i32Result))
		{
			EIMLOGGER_ERROR_(pILog, _T("update groupinfo faild,qsid:%lld"), sSInfo.qsid);
		}
		else
		{
			EIMLOGGER_DEBUG_(pILog, _T("update groupinfo success,qsid:%lld"), sSInfo.qsid);
		}   
		m_i32Result = smgr.Set(sSInfo.qsid, (PQEID)m_sData.aUserID, m_sData.wNum, TRUE, TRUE);    
        
		std::list<S_UpdateReadMsg> msgReadLst;
        if( eng.GetUpdateRead(sSInfo.qsid, msgReadLst) )
        {
			QEID qeidLogin = eng.GetPurview(GET_LOGIN_QEID);
			std::list<S_UpdateReadMsg>::iterator itrItem = msgReadLst.begin();
            while(itrItem != msgReadLst.end())
            {
                S_UpdateReadMsg msgRead = (*itrItem);
                C_eIMMsgReadMgr readmgr;
                readmgr.Init(pIDb);
                S_SessionEmps sSessionEmp;
                sSessionEmp.self_qeid = qeidLogin;
                smgr.EnumMember(sSInfo.qsid, GetEmpByQSID, (void*)&sSessionEmp);
                readmgr.AddMsgReadEmps(msgRead.qmid, sSessionEmp.vecMember);

                eng.GetReadMsgNotice(msgRead.qmid, m_msgReadNoticeLst);
                std::list<S_ReadMsgNotice>::iterator itr = m_msgReadNoticeLst.begin();
                while(itr != m_msgReadNoticeLst.end())
                {
                    S_ReadMsgNotice sData = (*itr);
                    readmgr.UpdateFlagAndReadTime(sData.dwMsgID,sData.dwSenderID, MSG_READ_FLAG_READ, sData.dwTime);
                    ++itr;
                    EIMLOGGER_DEBUG_(pILog, _T("get groupinfo ok write readmsgdb msgid:%llu sederid:%u msgtype:%u\n"), sData.dwMsgID, sData.dwSenderID,sData.cMsgType);
                }

                ++itrItem;
            }
            
        }
		return m_i32Result;
	}

	int	C_CmdGetGroupInfoAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		//CHECK_FAILED_SEND_EVENT_RET_(EVENT_MODIFY_GROUP_ACK, (void*)this);
		QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
		SAFE_SEND_EVENT_(pIEMgr,EVENT_GET_GROUP_INFO_ACK,(void*)&qsid);

        std::list<S_ReadMsgNotice>::iterator itr = m_msgReadNoticeLst.begin();
        while(itr != m_msgReadNoticeLst.end())
        {
            S_ReadMsgNotice sData = (*itr);
            ++itr;
            if(MSG_RECEIPT == sData.cMsgType)
            {
				S_1To100Read sMsgRead = { 0 };
                sMsgRead.bIsSend    = true;  
                sMsgRead.qeidSender = sData.dwSenderID;
                sMsgRead.qmid       = sData.dwMsgID;
                sMsgRead.qsid       = GroupId2Qsid(m_sData.aszGroupID);
                SAFE_SEND_EVENT_(pIEMgr, EVENT_RECEIPT_NOTICE, (void*)&sMsgRead);
            }
            else
            {
				S_1To100Read sMsgRead = { 0 };
                sMsgRead.bIsSend    = true; 
                sMsgRead.qeidSender = sData.dwSenderID;
                sMsgRead.qmid       = sData.dwMsgID;
                sMsgRead.qsid       = GroupId2Qsid(m_sData.aszGroupID);
                SAFE_SEND_EVENT_(pIEMgr, EVENT_1TO100_READ_NOTICE, (void*)&sMsgRead);
            }

        }
        
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdGetGroupInfoAck::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CHECK_FAILED_RET_(m_i32Result, NULL);

		TiXmlElement El(PROTOCOL_ELEMENT_CMD);
		TiXmlElement ItemEl(FIELD_ITEM);

		El.SetAttribute(FIELD_ID,		  m_i32CmdId);
		El.SetAttribute(FIELD_USER_ID,	  m_sData.dwCreaterID);
		SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
		El.SetAttribute(FIELD_TIMESTAMP,  m_sData.dwTime);
		El.SetAttribute(FIELD_GROUP_NAME, (char*)m_sData.aszGroupName);
		El.SetAttribute(FIELD_GROUP_NOTE, (char*)m_sData.aszGroupNote);

		for (int i32Index = 0; i32Index < m_sData.wNum; i32Index++)
		{
			ItemEl.SetAttribute(FIELD_USER_ID, m_sData.aUserID[i32Index]);
			El.InsertEndChild(ItemEl);
		}

		ConvertToXml(El, m_szXml);
		return m_szXml.c_str();
	}

	//C_CmdModifyGroupMember//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyGroupMember, eCMD_MODIFY_GROUP_MEMBER);
	int C_CmdModifyGroupMember::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		if (SUCCEEDED(m_i32Result) && m_sData.wNum > 0)
			m_i32Result = pICa->ModiMember(m_sData.aszGroupID, (char*)m_sData.aUserID, m_sData.wNum, m_sData.cOpType, m_sData.dwTime);

		if (SUCCEEDED(m_i32Result) && m_sDelMember.wNum > 0)
			m_i32Result = pICa->ModiMember(m_sDelMember.aszGroupID, (char*)m_sDelMember.aUserID, m_sDelMember.wNum, m_sDelMember.cOpType, m_sDelMember.dwTime);

		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int	C_CmdModifyGroupMember::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		CHECK_FAILED_SEND_EVENT_RET_(EVENT_MODIFY_GROUP_MEMBER, (void*)this);
		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdModifyGroupMember::FromXml( const TCHAR* pszXml )
	{
		int				i32Add;
		int				i32Value;
		const char*		pszValue = NULL;
		eIMStringA		szValueA;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);
		
		CHECK_NULL_RET_(pEl, FALSE);
		ZERO_STRUCT_(m_sDelMember);
		ResetData();
		C_eIMEngine& eng = Eng;

		m_sData.cOpType  = MODIFY_GROUP_MEMBER_TYPE_ADD;
		m_sData.dwUserID = pEl->Attribute(FIELD_USER_ID, &i32Value) ? i32Value : eng.GetPurview(GET_LOGIN_QEID);
		m_sData.dwTime   = pEl->Attribute(FIELD_TIMESTAMP, &i32Value) ? i32Value : eng.GetTimeNow();
		
		pszValue = pEl->Attribute(FIELD_GROUP_ID);
		SET_GROUP_ID_(pszValue, m_sData.aszGroupID);
	
		memcpy(&m_sDelMember, &m_sData, sizeof(MODIMEMBER));
		m_sDelMember.cOpType = MODIFY_GROUP_MEMBER_TYPE_DEL;
		for(pEl = pEl->FirstChildElement(); pEl && m_sData.wNum < MAXNUM_PAGE_USERID; pEl = pEl->NextSiblingElement() )
		{
			if ( pEl->Attribute(FIELD_USER_ID, &i32Value ) )
			{
				pszValue = pEl->Attribute(FIELD_TYPE, &i32Add);
				ASSERT_(pszValue);
				if (i32Add == MODIFY_GROUP_MEMBER_TYPE_ADD)
					m_sData.aUserID[m_sData.wNum++] = i32Value;
				else
					m_sDelMember.aUserID[m_sDelMember.wNum++] = i32Value;
			}
		}

		m_szXml = pszXml;
		return TRUE;
	}

	//C_CmdModifyGroupMemberAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyGroupMemberAck, eCMD_MODIFY_GROUP_MEMBER_ACK);
	int C_CmdModifyGroupMemberAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
		{
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());
			SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_GROUP_MEMBER_FAILD_ACK, (void*)this);
		}
			
		return m_i32Result;
	}

	int C_CmdModifyGroupMemberAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result) )
		{
			C_eIMSessionMgr smgr;

			smgr.Init(pIDb);
			m_i32Result = smgr.Set(GroupId2Qsid(m_sData.aszGroupID), eSET_TYPE_SUTIME, m_sData.dwTime);
			m_i32Result = smgr.Set(GroupId2Qsid(m_sData.aszGroupID), (PQEID)m_sData.aUserID, m_sData.wNum, m_sData.cOpType == 0, FALSE);
		}

		return m_i32Result;
	}			

	int	C_CmdModifyGroupMemberAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr,EVENT_MODIFY_GROUP_MEMBER_ACK, (void*)this);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdModifyGroupMemberAck::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		m_i32Result = pICa->GetErrorCode(m_sData.result);

		//ResetData();
		TiXmlElement El(PROTOCOL_ELEMENT_CMD);
		TiXmlElement ItemEl(FIELD_ITEM);

		El.SetAttribute(FIELD_ID,		  m_i32CmdId);
		SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
		El.SetAttribute(FIELD_TIMESTAMP,  m_sData.dwTime);
		ItemEl.SetAttribute(FIELD_TYPE,	  m_sData.cOpType);
		El.SetAttribute(FIELD_MODIFY_ID,  Eng.GetPurview(GET_LOGIN_QEID) );

		for (int i32Index = 0; i32Index < m_sData.wNum; i32Index++)
		{
			ItemEl.SetAttribute(FIELD_USER_ID, m_sData.aUserID[i32Index]);
			El.InsertEndChild(ItemEl);
		}

		ConvertToXml(El, m_szXml);
		return m_szXml.c_str();

	}

	//C_CmdModifyGroupMemberNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyGroupMemberNotice, eCMD_MODI_MEMBER_NOTICE);
	int C_CmdModifyGroupMemberNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int C_CmdModifyGroupMemberNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMSessionMgr smgr;

		smgr.Init(pIDb);
		m_i32Result = smgr.Set(GroupId2Qsid(m_sData.aszGroupID), eSET_TYPE_SUTIME, m_sData.dwTime);
		m_i32Result = smgr.Set(GroupId2Qsid(m_sData.aszGroupID), (PQEID)m_sData.aUserID, m_sData.wNum, m_sData.cOpType == 0, FALSE);

		//如果通知删除的人是自己，则删除会话信息
		QEID qeidLoginId = Eng.GetLoginEmp()->Id;
		if(m_sData.cOpType == 1)
		{
			for (int i32Index = 0;i32Index < m_sData.wNum; i32Index++)
			{
				if(m_sData.aUserID[i32Index] == qeidLoginId)
				{
					QSID qsid  = GroupId2Qsid((char*)m_sData.aszGroupID);
					smgr.Remove(qsid);
					break;
				}
			}
		}

		return m_i32Result;
	}

	int	C_CmdModifyGroupMemberNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, EVENT_MODIFY_GROUP_MEMBER_NOTICE, (void*)this);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdModifyGroupMemberNotice::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		m_i32Result = EIMERR_NO_ERROR;

		TiXmlElement El(PROTOCOL_ELEMENT_CMD);
		TiXmlElement ItemEl(FIELD_ITEM);

		El.SetAttribute(FIELD_ID,		  m_i32CmdId);
		SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
		El.SetAttribute(FIELD_TIMESTAMP,  m_sData.dwTime );
		ItemEl.SetAttribute(FIELD_TYPE,	  m_sData.cOpType);
		El.SetAttribute(FIELD_MODIFY_ID,  m_sData.dwModiID );
		
		for (int i32Index = 0; i32Index < m_sData.wNum; i32Index++)
		{
			ItemEl.SetAttribute(FIELD_USER_ID, m_sData.aUserID[i32Index]);
			El.InsertEndChild(ItemEl);
		}

		ConvertToXml(El, m_szXml);
		return m_szXml.c_str();

	}

	//- eCMD_MODI_MEMBER_NOTICE_ACK,	
	REGISTER_CMDCLASS(C_CmdSendMsg, eCMD_SEND_MSG);
	int C_CmdSendMsg::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		if ( GetFlag(CMD_FLAG_MSG_WAIT_FILE) )
		{	// Need wait file transfer finished
			SetFlag(CMD_FLAG_THREAD_UI, CMD_FLAG_THREAD_UI);	// Shift to UI thread 
			SetFlag(~CMD_FLAG_THREAD_DB, CMD_FLAG_THREAD_DB);	// Shield DB thread temp
			return EIMERR_NO_ERROR;
		}

		CONVERT_TO_ICA_RET_(pvCa);
		if ( m_sData.cType == eIM_MSGTYPE_TEXT && !m_szRobotBeginA.empty() )
		{	// To robot xml		
			int i32XmlLen = m_szRobotBeginA.size() + strlen(kRobotQuestionEnd);
			int i32MoveLen = i32XmlLen + m_sData.dwMsgLen > COUNT_OF_ARRAY_(m_sData.aszMessage) ? COUNT_OF_ARRAY_(m_sData.aszMessage) - i32XmlLen : m_sData.dwMsgLen;

			memmove(m_sData.aszMessage + 10 + m_szRobotBeginA.size(), &m_sData.aszMessage[10], i32MoveLen);
			memcpy(m_sData.aszMessage + 10, m_szRobotBeginA.c_str(), m_szRobotBeginA.size());
			memcpy(m_sData.aszMessage + i32MoveLen + i32XmlLen - strlen(kRobotQuestionEnd) - 1, kRobotQuestionEnd, strlen(kRobotQuestionEnd));
			m_sData.dwMsgLen += i32XmlLen;
		}

		m_i32Result = pICa->SendSMSEx( &m_sData );

		if(SUCCEEDED(m_i32Result) && m_sData.cType != eIM_MSGTYPE_ROBOT_SUGGEST)
		{
			SetBusinessTypeId(m_sData.dwMsgID);
			C_MsgWaitOverItem* pBaseWaitItem = new C_MsgWaitOverItem();
			pBaseWaitItem->SetData(this);
			pBaseWaitItem->SetStartTime(GetTickCount());
			C_UIWaitOverMgr::GetObject().AddWaitOverItem(pBaseWaitItem);
		}

		CMD_TRACE_STR_(m_szXml.substr(0, 1024), m_i32Result);

		return m_i32Result;
	}

	int	C_CmdSendMsg::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
        if(eIM_MSGTYPE_MSG_RECALL == m_sData.cType)
        {
            //召回消息不入库
            return EIMERR_NO_ERROR;
        }
		const char*		pszValue;
		S_OffFile		sOffFile = { 0 };
		S_P2pFile		sP2pFile = { 0 };
		C_eIMMsgMgr		mmgr;
		C_eIMSessionMgr smgr;
		C_eIMFileMgr	fmgr;
		C_eIMMsgReadMgr MsgReadMgr;
		
		mmgr.Init(pIDb);
		smgr.Init(pIDb);
		fmgr.Init(pIDb);
		MsgReadMgr.Init(pIDb);

		TiXmlHandle h(m_xmlDoc.RootElement());
		TiXmlElement* pEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
		for(;pEl;pEl=pEl->NextSiblingElement())
		{
			pszValue = pEl->Value();
			ASSERT_(pszValue);
			if ( pszValue == NULL )
				continue;

			if (/*_stricmp(pszValue, FIELD_MSGIMAGE ) == 0 ||*/ // No image
				_stricmp(pszValue, FIELD_MSGANNEX ) == 0 ||
				/*_stricmp(pszValue, FIELD_MSGLONG ) == 0  ||*/ // No Long msg
				_stricmp(pszValue, FIELD_MSGVOICE ) == 0)
			{
				pszValue			 = pEl->Attribute(FIELD_FID);
				sOffFile.Qfid		 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0;

				pszValue			 = pEl->Attribute(FIELD_SIZE);
				sOffFile.u64Size	 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0;

				sOffFile.Qmid		 = m_sData.dwMsgID;
				sOffFile.eidSender   = m_sData.dwUserID;
				sOffFile.dwTimestamp = m_sData.nSendTime;
				sOffFile.dwFlag		 = 0;

				pszValue = pEl->Attribute(FIELD_KEY);
				SET_UTF8_VALUE_(pszValue, sOffFile.szKey);

				pszValue = pEl->Attribute(FIELD_NAME);
				SET_UTF8_VALUE_(pszValue, sOffFile.szName);

				pszValue = pEl->Attribute(FIELD_FILE_PATH);
				SET_UTF8_VALUE_(pszValue, sOffFile.szLocalPath);

				fmgr.Set(&sOffFile);
			}
			else if( _stricmp(pszValue, FIELD_MSGP2P ) == 0 )
			{	
				pszValue			 = pEl->Attribute(FIELD_FID);
				sOffFile.Qfid		 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0;

				pszValue			 = pEl->Attribute(FIELD_SIZE);
				sP2pFile.u64Size	 = _strtoui64(pszValue, NULL, 0);

				sP2pFile.Qmid		 = m_sData.dwMsgID;
				sP2pFile.eidSender	 = m_sData.dwUserID;	// Maybe not right here...
				sP2pFile.eidRecver	 = m_sData.dwRecverID;
				sP2pFile.dwTimestamp = m_sData.nSendTime;
				sP2pFile.dwFlag		 = 0;

				pszValue = pEl->Attribute(FIELD_TOKEN);
				SET_UTF8_VALUE_(pszValue, sP2pFile.szToken);

				pszValue = pEl->Attribute(FIELD_NAME);
				SET_UTF8_VALUE_(pszValue, sP2pFile.szName);

				pszValue = pEl->Attribute(FIELD_FILE_PATH);
				SET_UTF8_VALUE_(pszValue, sP2pFile.szLocalPath);

				fmgr.Set(&sP2pFile);
			}
		}
		if (m_sData.cIsGroup == 5)
		{
			QSID qsid = m_sMsg.qsid;
			smgr.GetSidByVirGroupid(qsid,m_sData.dwRecverID,eSESSION_TYPE_VIRGROUP);
			m_sMsg.qsid = qsid;
		}

		S_SessionInfo sSessionInfo;
		ZERO_STRUCT_(sSessionInfo);
		BOOL bHasSession = smgr.Get(m_sMsg.qsid,&sSessionInfo);
		if( bHasSession && (sSessionInfo.eFlag & eSEFLAG_DELETE) ) 
			smgr.UpdateFlag(m_sMsg.qsid, eSEFLAG_DELETE, FALSE);

		m_i32Result = smgr.Set(m_sMsg.qsid, eSET_TYPE_UTIME, m_sData.nSendTime);
		m_i32Result = mmgr.Set(&m_sMsg, m_szMsgUI.c_str(), FALSE);
		if ( FAILED(m_i32Result) )
		{
			EIMLOGGER_ERROR_(pILog, _T("QMID: %llu, QSID:%llu, Time:%d, Msg:%s"), m_sMsg.qmid, m_sMsg.qsid, m_sMsg.dwTime, m_szMsgUI.c_str());
			return m_i32Result;
		}

		//Add to msg read tabel if the msg is read or allrepaly msg.
		if(m_sData.cRead == 1 || m_sData.cAllReply == 1 )
		{
			m_i32Result = MsgReadMgr.AddMsgReadEmps(m_sData.dwMsgID,m_MsgEmps);
			if ( FAILED(m_i32Result) )
			{
				EIMLOGGER_ERROR_(pILog, _T("Add to msg_read tabel [QMID: %llu] "), m_sMsg.qmid);
				return m_i32Result;
			}
		}

		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdSendMsg::SetAttribute(UINT64 U64AttrId, TCHAR* pszValue)
	{
		const char* pszQfid = NULL;
		QFID		Qfid = 0;
		DWORD		dwCountWaiting = 0;
		TiXmlHandle h(m_xmlDoc.RootElement());
		TiXmlElement* pMsgEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
		ASSERT_(pMsgEl);
		ASSERT_(pszValue);
		CHECK_NULL_RET_(pMsgEl, FALSE);
		CHECK_NULL_RET_(pMsgEl, FALSE);

		for( ;pMsgEl; pMsgEl = pMsgEl->NextSiblingElement() )
		{
			pszQfid = pMsgEl->Attribute(FIELD_FID);
			if (pszQfid == NULL)
				continue;

			Qfid = _strtoui64(pszQfid, NULL, 0);
			if(Qfid == U64AttrId)
			{
				eIMStringA szValueA;
				::eIMTChar2UTF8(pszValue, szValueA);
				pMsgEl->SetAttribute(FIELD_KEY, szValueA.c_str());
			}
			else if (pMsgEl->Attribute(FIELD_KEY) == NULL)
				dwCountWaiting++;
		}

		if ( dwCountWaiting == 0 )
		{	// All file transfer finished
			SetFlag(0, CMD_FLAG_MSG_WAIT_FILE);	// Clear wait flag
			SetFlag(CMD_FLAG_THREAD_SEND, CMD_FLAG_THREAD_SEND);
			SetFlag(CMD_FLAG_THREAD_DB, CMD_FLAG_THREAD_DB);
			SetFlag(0, CMD_FLAG_THREAD_UI);

			// Update <MsgUI /> and send buffer of message
			GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
			TiXmlHandle h(m_xmlDoc.RootElement());
			m_sData.dwMsgLen = MSG_MAXLEN;
			Xml2Msg(h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement(), m_sData.aszMessage, m_sData.dwMsgLen, pIProt, m_sMsg.qeid);
			ConvertToXml(*h.FirstChildElement(FIELD_MSGUI).ToElement(), m_szMsgUI);
			ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
		}

		return TRUE;
	}

	int	C_CmdSendMsg::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		if ( GetFlag( CMD_FLAG_MSG_WAIT_FILE) )
		{
			const TCHAR*	ptszValue= NULL;
			const char*		pszValue = NULL;
			const char*		pszPos   = NULL;
			BOOL			bIsImage = FALSE;
			eIMString		szValue;
			eIMStringA		szValueA;
			S_FileInfo		sFileInfo = { 0 };
			PS_FileTrans	psFileTrans = static_cast<PS_FileTrans>(pvBIFile);
			TiXmlHandle h(m_xmlDoc.RootElement());
			TiXmlElement* pMsgEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
			ASSERT_(pMsgEl);

			sFileInfo.pICmd = this;
			sFileInfo.sFile.bitDirect = FILE_DIR_UPLOAD;
			sFileInfo.sFile.bitStatus = FILE_STATUS_READY;
			sFileInfo.sFile.bitType	  = FILE_TRANS_TYPE_OFFLINE;
			C_eIMEngine& eng = Eng;

			while (pMsgEl)
			{
				bIsImage = FALSE;
				pszValue = pMsgEl->Value();
				ASSERT_(pszValue);
				if ( (bIsImage = (_stricmp(pszValue, FIELD_MSGIMAGE) == 0))  ||
					 _stricmp(pszValue, FIELD_MSGANNEX) == 0 ||
					 _stricmp(pszValue, FIELD_MSGLONG ) == 0  ||
					 _stricmp(pszValue, FIELD_MSGVOICE ) == 0 ||
                     _stricmp(pszValue, FIELD_MSGVIDEO ) == 0 )
				{
					sFileInfo.i32Ratio			= -1;
					sFileInfo.sFile.u64Fid		= _strtoui64(pMsgEl->Attribute(FIELD_FID), NULL, 0);
					sFileInfo.sFile.i64FileSize = _strtoi64(pMsgEl->Attribute(FIELD_SIZE), NULL, 0);
					sFileInfo.sFile.bitIsImage  = bIsImage;
					sFileInfo.sFile.u32LoginEmpId = eng.GetPurview(GET_LOGIN_QEID);

					pszValue = pMsgEl->Attribute(FIELD_FILE_PATH);	ASSERT_(pszValue);
					SET_UTF8_2TVALUE_(pszValue, sFileInfo.sFile.szFile); 
					m_i32Result = AddFileTask(*psFileTrans, sFileInfo, TRUE);
				}
				else if(_stricmp(pszValue, FIELD_MSGP2P ) == 0 )
				{	//...
					ASSERT_(FALSE);
				}

				pMsgEl = pMsgEl->NextSiblingElement();
			}

			if ( FAILED(m_i32Result) )
				return m_i32Result;
		}

		ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);

		SAFE_SEND_EVENT_(pIEMgr,EVENT_NEWMSG,&m_sData.dwMsgID);
        if(m_sData.cRead == 1)
        {
		    SAFE_SEND_EVENT_(pIEMgr, EVENT_REPLACE_WRITEDB_OK, &m_sData.dwMsgID);
        }
		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdSendMsg::FromXml( const TCHAR* pszXml )
	{
		GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		C_eIMEngine& eng = Eng;

		int				i32Value = 0;
		long long		i64Value = 0ll;
		const char*		pszValue = NULL;
		eIMStringA		szValueA;
		TiXmlElement*	pEl = ParseXmlCmdHeader(m_xmlDoc, pszXml, m_i32CmdId, m_i32Result);
	
		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		ZERO_STRUCT_(m_sMsg);

		if( pEl->Attribute( FIELD_BUSINESS_ID,&i32Value ) ) 
			m_dwBusinessId = i32Value;
		else
			m_dwBusinessId = 0;

		m_sMsg.qeid = eng.GetPurview(GET_LOGIN_QEID);
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		if ( pEl->Attribute( FIELD_MSG_ID) == NULL )		// Check and set QMID
		{
			//m_sMsg.qmid		= ::eIMGetMsgId(m_sMsg.qeid);
			m_sMsg.qmid = eng.NewID();	// By CA
			m_sData.dwMsgID = m_sMsg.qmid;
			pEl->SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sMsg.qmid, szBuf) );
		}
		else
		{
			m_sData.dwMsgID = GroupId2Qsid(pEl->Attribute( FIELD_MSG_ID));
			m_sMsg.qmid	 = m_sData.dwMsgID;
		}

		pEl->Attribute( FIELD_RECVER_ID,&i32Value ); m_sData.dwRecverID = i32Value; 
		pEl->Attribute( FIELD_IS_GROUP, &i32Value ); m_sData.cIsGroup   = i32Value; 
		pEl->Attribute( FIELD_READ,	    &i32Value ); m_sData.cRead      = i32Value; 
		pEl->Attribute( FIELD_MSG_TOTAL,&i32Value ); m_sData.nMsgTotal  = i32Value; 
		pEl->Attribute( FIELD_MSG_SEQ,  &i32Value ); m_sData.nMsgSeq    = i32Value; 
		pEl->Attribute( FIELD_MSG_LEN,  &i32Value ); m_sData.dwMsgLen   = i32Value;

		if (pEl->Attribute( FIELD_TYPE,		&i32Value ))
		{
			m_sData.cType = i32Value; 
			if ( m_sData.cType == eIM_MSGTYPE_ROBOT_SUGGEST )	// Not write database
				m_dwFlag &= ~(CMD_FLAG_THREAD_DB | CMD_FLAG_THREAD_UI);	
            else if( m_sData.cType == eIM_MSGTYPE_MSG_RECALL )  //召回
            {
                pszValue = pEl->Attribute( FILED_MSG_RECALLMSGID);
                eIMString strReCallID;
                ::eIMMByte2TChar(pszValue, strReCallID);
                m_sData.dwSrcMsgID   = Str2Int64(strReCallID.c_str(), 0);
                m_dwFlag &= ~(CMD_FLAG_THREAD_DB |  CMD_FLAG_THREAD_UI);
            }
		}
		else
			m_sData.cType = 0xFF;	// Flag of no this attribute

		if(pEl->Attribute(FILED_ROBOT_IS_TOPIC, &i32Value) && 1==i32Value)
		{
			m_dwFlag &= ~(CMD_FLAG_THREAD_DB | CMD_FLAG_THREAD_UI);	
		}

		if ( m_sData.cIsGroup )
		{
			pszValue = pEl->Attribute(FIELD_GROUP_ID);	ASSERT_(pszValue);
			m_sMsg.qsid = GroupId2Qsid(pszValue);
			ASSERT_(m_sMsg.qsid);
			SET_UTF8_VALUE_(pszValue, m_sData.aszGroupID);
		
			// Check group created or not
			C_eIMSessionMgr smgr;
			S_SessionInfo sSessionInfo = { 0 };
			if (!smgr.Get(m_sMsg.qsid, &sSessionInfo) ||
				(sSessionInfo.eFlag & eSEFLAG_DELETE) )
				Sleep(200);	// Wait 200ms

			if (!smgr.Get(m_sMsg.qsid, &sSessionInfo) ||
				(sSessionInfo.eFlag & eSEFLAG_DELETE) )
				m_i32Result = EIMERR_CREATE_GROUP_FAIL;
		}
		else
		{
			if ( (pszValue = pEl->Attribute(FIELD_GROUP_ID)) )
			{
				m_sMsg.qsid = GroupId2Qsid(pszValue);
			}
			else
			{	// No QSID of single session, generate it
				C_eIMSessionMgr smgr;
				S_SessionInfo	sSInfo = { 0 };

				smgr.Get(m_sData.dwUserID, m_sData.dwRecverID, &sSInfo);
				m_sMsg.qsid = sSInfo.qsid;
				pEl->SetAttribute(FIELD_GROUP_ID, GET_QSID_ASTR_(m_sMsg.qsid, szBuf));
			}
		}

		const char* pszRobotKey = pEl->Attribute("value");
		const char* pszRobotTheme = pEl->Attribute("theme");
		if ( pszRobotKey && pszRobotTheme )
		{
			m_szRobotBeginA = kRobotQuestionBegin;
			m_szRobotBeginA+= pszRobotTheme;
			m_szRobotBeginA+= kRobotQuestionTheme;
			m_szRobotBeginA+= pszRobotKey;
			m_szRobotBeginA+= kRobotQuestionValue;
		}

		//一呼万应优先
		int i321To100 = 0;
		int i321To10T = 0;
		pEl->Attribute( FILED_IS_1_TO_HUNDRED, &i321To100);
		pEl->Attribute( FILED_IS_1To10T,	   &i321To10T );
		if(!!i321To10T)
		{
			m_sData.dwSrcMsgID = m_sMsg.qsid;
			m_sData.cAllReply = eSESSION_TYPE_1TO_TEN_THOUSAND;
		}
		else if(!!i321To100)
			m_sData.cAllReply = eSESSION_TYPE_1TO_HUNDRED;
		else
			m_sData.cAllReply = 0;

		// <Item Emps> ============================================================
        m_MsgEmps.reserve(500);
		for(TiXmlElement* pItemEmp = pEl->FirstChildElement(FIELD_ITEM); pItemEmp ; pItemEmp = pItemEmp->NextSiblingElement(FIELD_ITEM) )
		{
			if ( pItemEmp->Attribute(SUBSCRIBE_EMPID, &i32Value ) )
				m_MsgEmps.push_back(i32Value);
		}

		// <MsgUI /> ==========================================================
		pEl = pEl->FirstChildElement(FIELD_MSGUI);	
		if ( pEl == NULL || _stricmp(pEl->Value(), FIELD_MSGUI) )
		{
			TRACE_(_T("Error SendMsg format[%s]"), pszXml);
			m_i32Result = EIMERR_BAD_CMD;
			return FALSE;
		}

		if ( pEl->Attribute( FIELD_SENDER_ID ) == NULL )		// Check and set QEID
			pEl->SetAttribute(FIELD_SENDER_ID, (int)m_sMsg.qeid);

		if ( pEl->Attribute( FIELD_SEND_TIME) == NULL )		// Check and set SendTime
		{
			m_sMsg.dwTime = eng.GetTimeNow();
			pEl->SetAttribute(FIELD_SEND_TIME, (int)m_sMsg.dwTime);
		}

		//if ( pEl->Attribute( FIELD_SENDER_NAME ) == NULL )	// Check and Set SenderName
		//{
		//	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		//	::eIMTChar2UTF8(pIConts->GetEmpName(m_sMsg.qeid), szValueA);
		//	pEl->SetAttribute(FIELD_SENDER_NAME, szValueA.c_str());
		//}

		if ( pEl->Attribute( FIELD_USER_CODE ) == NULL )	// Check and Set SenderName
		{
			GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
			PS_EmpInfo_ psEmpInfo = pIConts->GetEmp(m_sMsg.qeid);
			ASSERT_(psEmpInfo);
			if ( psEmpInfo )
			{
				::eIMTChar2UTF8(psEmpInfo->pszCode, szValueA);
				pEl->SetAttribute(FIELD_USER_CODE, szValueA.c_str());
			}
		}

		if ( pEl->Attribute( FIELD_IS_SEND ) == NULL )		// Check and Set IsSend
			pEl->SetAttribute(FIELD_IS_SEND, 1);

		m_sMsg.dwFlag= 0;	// Maybe set m_sData.cMsgType...
		m_sMsg.eType = eSESSION_TYPE_SINGLE;
		if ( pEl->Attribute( FIELD_TYPE ) == NULL )			// Check and Set SessionType
		{
			S_SessionInfo	sInfo = { 0 };
			C_eIMSessionMgr smgr;
			
			if (smgr.Get(GroupId2Qsid(pEl->Attribute(FIELD_GROUP_ID)), &sInfo))
			{
				m_sMsg.eType = sInfo.eType;
				pEl->SetAttribute(FIELD_TYPE, sInfo.eType);
			}
			else 
				pEl->SetAttribute(FIELD_TYPE, m_sMsg.eType);// Default as single session
		}

		//加入此字段，判断是否一呼百应字段
		if(pEl->Attribute(FILED_IS_1_TO_HUNDRED) == NULL)
			pEl->SetAttribute(FILED_IS_1_TO_HUNDRED, m_sData.cAllReply);

		if(pEl->Attribute(FILED_IS_RECEIVE_NUM) == NULL)
			pEl->SetAttribute(FILED_IS_RECEIVE_NUM, m_MsgEmps.size());

		//发送时如果是一呼百就，加入MSGID字段，RE显示需要此字段
		if(NULL==pEl->Attribute(FIELD_MSG_ID) && (eSESSION_TYPE_1TO_HUNDRED==m_sData.cAllReply || 1==m_sData.cRead))
		{
			char szMsgBuf[CONVERT_TO_STRING_SIZE] = { 0 };
			pEl->SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sData.dwMsgID, szMsgBuf));
		}

		pEl->Attribute( FIELD_SEND_TIME, (int*)&m_sData.nSendTime);
		pEl->Attribute( FIELD_SENDER_ID, (int*)&m_sData.dwUserID);

		if(NULL==pEl->Attribute(FIELD_READ))
			pEl->SetAttribute(FIELD_READ, m_sData.cRead);

		// <MsgUI><Message /></MsgUI> =========================================
		TiXmlHandle h(pEl);
		TiXmlElement*	pMessageEl = h.FirstChildElement(FIELD_MESSAGE).ToElement();	
		if ( pMessageEl == NULL )
		{
			TRACE_(_T("Error SendMsg format[%s]"), pszXml);
			m_i32Result = EIMERR_BAD_CMD;
			return FALSE;
		}

		m_sData.dwMsgLen = MSG_MAXLEN;
		if ( !Xml2Msg(pMessageEl, m_sData.aszMessage, m_sData.dwMsgLen, pIProt, m_sMsg.qeid) )
		{
			m_i32Result = EIMERR_BAD_CMD;
			return FALSE;
		}

		if (m_sData.cType == 0xFF)
		{
			if ( m_dwFlag & CMD_FLAG_MSG_VOICE )
				m_sData.cType = eIM_MSGTYPE_VOICE;		// 语音
			else if ( m_dwFlag & CMD_FLAG_MSG_VIDEO )
				m_sData.cType = eIM_MSGTYPE_VIDEO;		// 视频
			else if ( m_dwFlag & CMD_FLAG_MSG_ANNEX)
				m_sData.cType = eIM_MSGTYPE_FILE;		// 附件
			else if ((m_dwFlag & CMD_FLAG_MSG_P2P) || (m_dwFlag & CMD_FLAG_MSG_RDP))
				m_sData.cType = eIM_MSGTYPE_P2P;		// P2P/RDP 文件, 复用类型5，由XML内容进行区分
			else if ( m_dwFlag & CMD_FLAG_MSG_ANNEX_ACK )
				m_sData.cType = eIM_MSGTYPE_FILERECVED;	// 附件应答(离线文件)
			else if ( m_dwFlag & CMD_FLAG_MSG_LONG )
				m_sData.cType = eIM_MSGTYPE_LONGTEXT;	// 长消息
			else if ( m_dwFlag & CMD_FLAG_MSG_AUTO )
				m_sData.cType = eIM_MSGTYPE_PCAUTO;		// 自动回复消息
			else
				m_sData.cType = eIM_MSGTYPE_TEXT;		// 聊天内容(表情和文字和图片)
		}

		if (GetFlag(CMD_FLAG_TRANSFER))
			SetFlag(CMD_FLAG_MSG_WAIT_FILE, CMD_FLAG_MSG_WAIT_FILE);
		else
			SetFlag(0, CMD_FLAG_MSG_WAIT_FILE);

		// <MsgUI><MsgFont /></MsgUI> =========================================
		if ( h.FirstChildElement(FIELD_MSGUI).ToElement() == NULL )
		{	// Need add it
			eng.AddFontElement(m_sMsg.eType, TRUE, pEl);
		}

		// Need convert to string because may be add some attribute
		ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
		ConvertToXml(*pEl, m_szMsgUI);

		m_i32Result = EIMERR_NO_ERROR;
		return TRUE;
	}



	char* C_CmdSendMsg::Xml2Msg( TiXmlElement* pEl, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol, QEID qeid )
	{
#define CHECK_BUF_SIZE_( NEED, TOTAL, RET ) \
	do{ \
		if ( (NEED) > (TOTAL) ) \
		{ \
			ASSERT_(FALSE); \
			TRACE_( _T("Message out of buffer[%d > %d]!"), (NEED), (TOTAL) ); \
			return (RET); \
		} \
	}while( 0 )

		int				i32Ret   = 0;
		int				i32Index = 0;
		const char*		pszValue = NULL;
		const char*		pszAttr  = NULL;
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };


		for ( pEl = pEl->FirstChildElement(); pEl; pEl = pEl->NextSiblingElement() )
		{	// Enumerate each child element
			pszValue = pEl->Value();
			ASSERT_( pszValue );
			CHECK_NULL_RET_(pszValue, NULL);

			if ( _stricmp(pszValue, FIELD_MSGFONT ) == 0 )
			{	// MsgFont
				DWORD	   dwColor   = strtol( pEl->Attribute(FIELD_COLOR), NULL, 0 );
				S_FONTINFO sFontInfo = { 0 };
				
				pszAttr = pEl->Attribute(FIELD_ID);
				sFontInfo.u8Id   = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_ID_DEFAULT;

				pszAttr = pEl->Attribute(FIELD_MODE);
				sFontInfo.u8Mode = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_MODE_DEFAULT;

				pszAttr = pEl->Attribute(FIELD_SIZE);
				sFontInfo.u8Size = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_SIZE_DEFAULT;

				sFontInfo.au8Color[0] = (BYTE)GetRValue(dwColor);	// R
				sFontInfo.au8Color[1] = (BYTE)GetGValue(dwColor);	// G
				sFontInfo.au8Color[2] = (BYTE)GetBValue(dwColor);	// B

				CHECK_BUF_SIZE_( i32Index + (int)sizeof(S_FONTINFO), u16Size, NULL );
				memcpy( &pszMsg[i32Index], &sFontInfo, sizeof(S_FONTINFO) );
				i32Index += sizeof(S_FONTINFO);
			}
			else if ( _stricmp(pszValue, FIELD_MSGTEXT ) == 0 )
			{	// MsgText
				SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
				pszAttr = pEl->GetText();
				i32Ret  = strlen(pszAttr);

				ASSERT_( i32Ret );
				CHECK_BUF_SIZE_( i32Index + i32Ret, u16Size, NULL );
				memcpy( &pszMsg[i32Index], pszAttr, i32Ret );
				i32Index += i32Ret;
			}
			else if ( _stricmp(pszValue, FIELD_MSGEMOT ) == 0 )	// [/wx][/han]
			{	// MsgEmot
				SetFlag(CMD_FLAG_MSG_EMOT, CMD_FLAG_MSG_EMOT);
				pszValue = pEl->Attribute(FIELD_NAME);
				if ( pszValue )
				{
					i32Ret = _snprintf( &pszMsg[i32Index], u16Size - i32Index - 1, "[%s]", pszValue );
					if ( i32Ret < 0 )
					{
						TRACE_( _T("Message out of buffer!")); 
						return NULL;
					}
					
					i32Index += i32Ret;
				}
			}
			else if ( _stricmp(pszValue, FIELD_MSGIMAGE) == 0 ) 	// [#m6R73a.jpg]
			{	// MsgImage
				SetFlag(CMD_FLAG_MSG_IMAGE, CMD_FLAG_MSG_IMAGE);
				CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
				pszValue =  pEl->Attribute(FIELD_KEY);
				if( pszValue )
				{
					const char* pszFileExt =  strrchr(pEl->Attribute(FIELD_NAME), '.');
					i32Ret = _snprintf( &pszMsg[i32Index], u16Size - i32Index - 1, "[#%s%s]", pszValue, pszFileExt);
					if ( i32Ret < 0 )
					{
						TRACE_( _T("Message out of buffer!")); 
						return NULL;
					}

					i32Index += i32Ret;
				}
			}
			else if ( _stricmp(pszValue, FIELD_MSGANNEX ) == 0 )	// 8751084#mMvyAn#depot_tools.zip
			{	// MsgAnnex, Format: Size + "#" + Key + "#" + File
				SetFlag(CMD_FLAG_MSG_ANNEX, CMD_FLAG_MSG_ANNEX);
				CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
				pszValue = pEl->Attribute(FIELD_KEY);
				if ( pszValue )
				{
					eIMString szData;
					ASSERT_( pEl->Attribute(FIELD_SIZE) );
					ASSERT_( pEl->Attribute(FIELD_NAME));

					i32Index			   = sizeof( FILE_META );
					FILE_META* psFileMeta  = (FILE_META*)pszMsg;
					psFileMeta->dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
					strncpy(psFileMeta->aszURL, pszValue, COUNT_OF_ARRAY_(psFileMeta->aszURL));
					strncpy(psFileMeta->aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(psFileMeta->aszFileName));
					psFileMeta->dwFileSize = htonl(psFileMeta->dwFileSize);
				}
			}
			else if ( _stricmp(pszValue, FIELD_MSGANNEXACK ) == 0 )	// 对方已成功接收了您发送的离线文件 "depot_tools.zip" (8.35 M)。
			{	// MsgAnnexAck
				SetFlag(CMD_FLAG_MSG_ANNEX_ACK, CMD_FLAG_MSG_ANNEX_ACK);

				const char* pText = pEl->Attribute(FIELD_ACK);
				i32Ret = sprintf( &pszMsg[i32Index], "%s", pEl->Attribute(FIELD_ACK) );
				ASSERT_(i32Ret > 0);
				i32Index += i32Ret;
			}
			else if ( _stricmp(pszValue, FIELD_MSGAUTO) == 0 )		// [自动回复] 现在忙
			{	// MsgAuto
				SetFlag(CMD_FLAG_MSG_AUTO, CMD_FLAG_MSG_AUTO);
				i32Ret = sprintf( pszMsg, "%s", pEl->GetText() );
				ASSERT_(i32Ret > 0);
				i32Index += i32Ret;
			}
			else if ( _stricmp(pszValue, FIELD_MSGLONG ) == 0 )
			{	// MsgLong, format:FileSize + Key + Name + Summery
				SetFlag(CMD_FLAG_MSG_LONG, CMD_FLAG_MSG_LONG);
				CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
				pszValue = pEl->Attribute(FIELD_KEY);
				if ( pszValue )
				{
					eIMString szData;
					ASSERT_( pEl->Attribute(FIELD_SIZE) );
					ASSERT_( pEl->Attribute(FIELD_NAME));

					FILE_META* psFileMeta  = (FILE_META*)&pszMsg[i32Index];
					psFileMeta->dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
					psFileMeta->dwFileSize = htonl(psFileMeta->dwFileSize);
					strncpy(psFileMeta->aszURL, pszValue, COUNT_OF_ARRAY_(psFileMeta->aszURL));
					strncpy(psFileMeta->aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(psFileMeta->aszFileName));
					
					i32Index += sizeof( FILE_META );
				}
			}
			else if ( _stricmp(pszValue, FIELD_MSGP2P ) == 0 )			// 0000EEF9000009910000004D52C108BB0000EEF9000009910000004C52C108BA|12286640|Setup.exe
			{	// MsgP2P, Format:  Key + Token + "|" + Size + "|" + Name
				eIMString szData;
				SetFlag(CMD_FLAG_MSG_P2P, CMD_FLAG_MSG_P2P);
				CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
				//i32Ret = sprintf( &pszMsg[i32Index], "%s%s|%s|%s", 
				//	pEl->Attribute( FIELD_SIZE ),
				//	pEl->Attribute( FIELD_KEY ),
				//	pEl->Attribute( FIELD_TOKEN ),
				//	pEl->Attribute( FIELD_NAME ));

				ASSERT_(i32Ret > 0);
				i32Index += i32Ret;
			}
			else if ( _stricmp(pszValue, FIELD_MSGRDP ) == 0 )	
			{
				eIMStringA oStr;
				SetFlag(CMD_FLAG_MSG_RDP, CMD_FLAG_MSG_RDP);
				oStr << *pEl ;
				strcpy(pszMsg, oStr.c_str());
				i32Index += oStr.size();					
			}
			else if ( _stricmp(pszValue, FIELD_MSGVOICE ) == 0 
                ||  _stricmp(pszValue, FIELD_MSGVIDEO ) == 0 )
			{	
                if(_stricmp(pszValue, FIELD_MSGVOICE ) == 0 )
				    SetFlag(CMD_FLAG_MSG_VOICE, CMD_FLAG_MSG_VOICE);
                else
                    SetFlag(CMD_FLAG_MSG_VIDEO, CMD_FLAG_MSG_VIDEO);

				CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
				pszValue = pEl->Attribute(FIELD_KEY);
				if ( pszValue )
				{
					eIMString szData;
					ASSERT_( pEl->Attribute(FIELD_SIZE) );
					ASSERT_( pEl->Attribute(FIELD_NAME));

					i32Index			   = sizeof( FILE_META );
					FILE_META* psFileMeta  = (FILE_META*)pszMsg;
					psFileMeta->dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
					psFileMeta->dwFileSize = htonl(psFileMeta->dwFileSize);
					strncpy(psFileMeta->aszURL, pszValue, COUNT_OF_ARRAY_(psFileMeta->aszURL));
					strncpy(psFileMeta->aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(psFileMeta->aszFileName));
				}
			}
			else if (_stricmp(pszValue, FIELD_MSGERROR ) == 0 )
			{	// Only for loop back UI to prompt 
			}
			else
			{	// Error type of message...
				ASSERT_(FALSE);
			}

		}

		u16Size = (WORD)i32Index + 1;
		return pszMsg;
	}

    QSID C_CmdSendMsg::GetSessionId()
    {
        return m_sMsg.qsid;
    }

	//C_CmdSendMsgAck///////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdSendMsgAck, eCMD_SEND_MSG_ACK);
	int C_CmdSendMsgAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
        //获取发送接
        I_EIMCmd* pSendInterface = C_UIWaitOverMgr::GetObject().GetWaitOverItemCmd(m_sData.dwMsgID);
        if(pSendInterface && eCMD_SEND_MSG == pSendInterface->GetCmdId())
        {
            C_CmdSendMsg* pSendMsg =  (C_CmdSendMsg*)pSendInterface;
            SENDMSG* pSendData = (SENDMSG*)pSendMsg->GetData();
            if(pSendData)
            {
                if(eIM_MSGTYPE_MSG_RECALL == pSendData->cType)
                {
                    m_sSendData.cType      = pSendData->cType;
                    m_sSendData.dwMsgID    = pSendData->dwSrcMsgID;
                    m_sSendData.qSessionID = pSendMsg->GetSessionId();
                    m_sSendData.result	   = SUCCEEDED(m_sData.result);
					m_sSendData.eidSender  = pSendData->dwUserID;
					m_sSendData.dwTime     = pSendData->nSendTime;
                }
            }
            pSendInterface->Release();
        }
		if (FAILED(m_i32Result))
		{
			C_UIWaitOverMgr::GetObject().DelSetFailedItem(eWaitType_Msg,m_sData.dwMsgID, this);
			EIMLOGGER_ERROR_( pILog, _T("MsgId: %llu %s"), m_sData.dwMsgID, m_szXml.c_str());
		}
		else
		{
			C_UIWaitOverMgr::GetObject().DelWaitOverItem(eWaitType_Msg,m_sData.dwMsgID);
			EIMLOGGER_DEBUG_( pILog, _T("MsgId: %llu"), m_sData.dwMsgID);
		}

		return m_i32Result;
	}

    int	C_CmdSendMsgAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
    {
        if(eIM_MSGTYPE_MSG_RECALL == m_sSendData.cType && m_sSendData.result)
        {
            C_eIMMsgMgr		mmgr;
		    mmgr.Init(pIDb);
            //修改撤回消息的消息类型
            mmgr.UpdateMsg2ReCallType(m_sSendData);
        }
        return EIMERR_NO_ERROR;
    }

	int	C_CmdSendMsgAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		return m_i32Result;
	}
	
	const TCHAR* C_CmdSendMsgAck::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if ( FAILED(m_i32Result) )
		{
			TCHAR szXml[256];
			C_eIMMsgMgr mmgr;

			_sntprintf(szXml, COUNT_OF_ARRAY_(szXml), kCmdSendMsgAck, m_i32Result, m_sData.dwMsgID, mmgr.Get(m_sData.dwMsgID));
			m_szXml = szXml;
			return m_szXml.c_str();
		}

		return NULL;
	}

	//C_CmdMsgNotice///////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdMsgNotice, eCMD_MSG_NOTICE);
	int C_CmdMsgNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_bGetGroupInfo = FALSE;
		m_bConstraintDb = FALSE;
		ZERO_STRUCT_(m_sMsg);
		ZERO_STRUCT_(m_sReply);

		//m_i32Result = pICa->SendMsgNoticeAck(m_sData.dwMsgID, m_sData.dwNetID);
		EIMLOGGER_DEBUG_(pILog, _T("rsv msg QMID: %llu i32Result:%d"),m_sData.dwMsgID, m_i32Result);
		return m_i32Result;
	}

	int	C_CmdMsgNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		EIMLOGGER_DEBUG_(pILog, _T("db process QMID: %llu MsgTyp:%d"),m_sData.dwMsgID, m_sData.cMsgType);
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		C_eIMEngine& eng = Eng;
		if(eIM_MSGTYPE_SMS_RECEIPT == m_sData.cMsgType)
		{

		}
        else if (eIM_MSGTYPE_MSG_RECALL == m_sData.cMsgType)
        {   //处理消息被召回
			m_sMsg.qmid = m_sData.dwSrcMsgID;
			m_sMsg.qeid = m_sData.dwSenderID;
			m_sMsg.dwTime = m_sData.dwSendTime;
			if ( m_sData.aszGroupID[0] == 0 )
			{
				S_SessionInfo  sSInfo = { 0 };
				C_eIMSessionMgr smgr;

				smgr.Init(pIDb);
				smgr.Get(m_sData.dwSenderID, m_sData.dwRecverID, &sSInfo);
				m_sMsg.qsid = sSInfo.qsid;
			}
			else
				m_sMsg.qsid = GroupId2Qsid((char*)m_sData.aszGroupID);

            DealMsgRecalled2Db(pIDb);
        }
		else if(eIM_MSGTYPE_SMS_RSP == m_sData.cMsgType)
		{
			SMS_client sClient = { 0 };
			memcpy(&sClient, (char *)(&m_sData.aszMessage[10]), sizeof(SMS_client));

			memset(&m_sReply, 0, sizeof(S_SmsReply));
			m_sReply.qMID = m_sData.dwMsgID;
			m_sReply.qEid = m_sData.dwSenderID;
			m_sReply.dwTimer = m_sData.dwSendTime;//ntohl(sClient.dwTime);
			m_sReply.u64Phone = ntohl64(sClient.dwSenderMobile);
			eIMString szTxt;
			eIMUTF8ToTChar((const char *)sClient.aszMSgContent, szTxt);
			_tcscpy(m_sReply.szContent, szTxt.c_str());

		//	const char*		pszValue;
			C_eIMSMSMgr		smsgr;

			S_Sms sSms;
			sSms.dwFlag			= 0;
			sSms.dwTime			= m_sReply.dwTimer;
			sSms.i32newmsg_flag = 0;
			sSms.i32offmsg_flag = 0;
			sSms.iType			= eSESSION_TYPE_SMS;
			sSms.qmid			= m_sData.dwMsgID;
			if(0 == m_sData.dwSenderID)
			{
				sSms.eidorPhone	       = m_sReply.u64Phone;
				sSms.i32eidOrPhoneType = 1;
			}
			else
			{
				sSms.eidorPhone			= m_sData.dwSenderID;
				sSms.i32eidOrPhoneType  = 0;
			}

			smsgr.Init(pIDb);
			ToXmlForSms(pIDb);
			smsgr.Set(&sSms, m_szMsgUI.c_str());
		}
		else
		{
			const char*		pszValue;
			S_OffFile		sOffFile = { 0 };
			S_P2pFile		sP2pFile = { 0 };
			C_eIMMsgMgr		mmgr;
			C_eIMSessionMgr smgr;
			C_eIMFileMgr	fmgr;

			mmgr.Init(pIDb);
			smgr.Init(pIDb);
			fmgr.Init(pIDb);
			ToXml(pIDb);
            if( !m_szXml.empty() )
            {
                CMD_TRACE_STR_(m_szXml.substr(0, 1024), m_i32Result);
            }
			if (!GetFlag(CMD_FLAG_THREAD_DB))
            {
                EIMLOGGER_DEBUG_(pILog, _T("cmd has not deal db"),m_sMsg.qmid);
				return EIMERR_NO_ERROR;
            }

			TiXmlHandle h(m_xmlDoc.RootElement());
			TiXmlElement* pEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
			for(;pEl; pEl = pEl->NextSiblingElement() )
			{
				pszValue = pEl->Value();
				ASSERT_(pszValue);
				if ( pszValue == NULL )
					continue;

				if (/*_stricmp(pszValue, FIELD_MSGIMAGE ) == 0 ||*/	// 图片不入库
					_stricmp(pszValue, FIELD_MSGANNEX ) == 0 )
				{
					pszValue			 = pEl->Attribute(FIELD_FID);
					sOffFile.Qfid		 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0; ASSERT_(pszValue);

					pszValue			 = pEl->Attribute(FIELD_SIZE);
					sOffFile.u64Size	 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0;// ASSERT_(pszValue);

					sOffFile.Qmid		 = m_sData.dwMsgID;
					sOffFile.eidSender   = m_sData.dwSenderID;
					sOffFile.dwTimestamp = m_sData.dwSendTime;
					sOffFile.dwFlag		 = 0;

					pszValue = pEl->Attribute(FIELD_KEY);
					SET_UTF8_VALUE_(pszValue, sOffFile.szKey);

					pszValue = pEl->Attribute(FIELD_NAME);
					SET_UTF8_VALUE_(pszValue, sOffFile.szName);

					pszValue = pEl->Attribute(FIELD_FILE_PATH);
					SET_UTF8_VALUE_(pszValue, sOffFile.szLocalPath);

					fmgr.Set(&sOffFile);
				}
				else if( _stricmp(pszValue, FIELD_MSGP2P ) == 0 )
				{	
					pszValue			 = pEl->Attribute(FIELD_FID);
					sOffFile.Qfid		 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0; ASSERT_(pszValue);

					pszValue			 = pEl->Attribute(FIELD_SIZE);
					sP2pFile.u64Size	 = pszValue ? _strtoui64(pszValue, NULL, 0) : 0;// ASSERT_(pszValue);

					sOffFile.Qmid		 = m_sData.dwMsgID;
					sP2pFile.eidSender	 = m_sData.dwSenderID;	// Maybe not right here...
					sP2pFile.eidRecver	 = m_sData.dwRecverID;

					sP2pFile.dwTimestamp = m_sData.dwSendTime;
					sP2pFile.dwFlag		 = 0;

					pszValue = pEl->Attribute(FIELD_TOKEN);
					SET_UTF8_VALUE_(pszValue, sP2pFile.szToken);

					pszValue = pEl->Attribute(FIELD_NAME);
					SET_UTF8_VALUE_(pszValue, sP2pFile.szName);

					pszValue = pEl->Attribute(FIELD_FILE_PATH);
					SET_UTF8_VALUE_(pszValue, sP2pFile.szLocalPath);

					fmgr.Set(&sP2pFile);
				}
			}

			S_SessionInfo sSInfo = { 0 };
			BOOL bExist = smgr.Get(m_sMsg.qsid, &sSInfo);
			if ((m_sMsg.eType == eSESSION_TYPE_MULTI || m_sMsg.eType == eSESSION_TYPE_RGROUP) &&
				(bExist == FALSE || /*sSInfo.dwCreateTime < m_sData.dwGroupTime ||*/ sSInfo.dwSUpdateTime < m_sData.dwGroupTime))
			{
				C_eIMSaveData* pSaveData = eng.GetEIMSaveDataPtr();
				CHECK_NULL_RET_(pSaveData, 0);
				BOOL bHasReq = pSaveData->IsGroupIDHasReq(m_sMsg.qsid);
				EIMLOGGER_DEBUG_(pILog, _T("Type: %u, CreateTime:%u, SUpdateTime: %u, GroupTime: %u, HasReq:%d"), m_sMsg.eType, sSInfo.dwCreateTime, sSInfo.dwSUpdateTime, m_sData.dwGroupTime, bHasReq);
				if(bHasReq == FALSE || sSInfo.dwSUpdateTime < m_sData.dwGroupTime )
				{
					pSaveData->AddGroupIDReq(m_sMsg.qsid);
					m_bGetGroupInfo = TRUE;
					if ( bHasReq == FALSE )
					{
						// Make a dummy multi-session information for UI display session
						QEID qeidLogin = eng.GetPurview(GET_LOGIN_QEID); 
						QEID aQeid[2]  = { m_sData.dwSenderID, m_sData.dwRecverID };
						smgr.Set(m_sMsg.qsid, aQeid, 2, TRUE, bExist==FALSE);		// Add default multi-session member

						sSInfo.dwCreateTime = 1;
						sSInfo.qsid  = m_sMsg.qsid;
						sSInfo.eType = m_sMsg.eType;
					//	sSInfo.dwSUpdateTime = m_sData.dwGroupTime -1;
						smgr.Set(&sSInfo);
					}
				}
			}

			//更新一呼万应会话表
			if(!bExist && eSESSION_TYPE_1TO_TEN_THOUSAND==m_sData.cAllReply)
			{
				QEID qeidLogin = eng.GetPurview(GET_LOGIN_QEID); 
				QEID aQeid[2]  = { m_sData.dwSenderID, m_sData.dwRecverID };
				smgr.Set(m_sMsg.qsid, aQeid, 2, TRUE, bExist==FALSE);

				sSInfo.dwCreateTime = m_sData.dwSendTime;
				sSInfo.qsid  = m_sMsg.qsid;
				sSInfo.eType = m_sMsg.eType;
				sSInfo.eFlag = eSEFLAG_1TOTH;
				sSInfo.dwCreateTime = m_sData.dwSendTime;
				sSInfo.CreaterId = m_sData.dwSenderID;
				smgr.Set(&sSInfo);
			}

			m_sMsg.dwFlag = m_sData.cOffline ? DB_FLAG_MSG_OFFLINE : DB_FLAG_MSG_NEW;
			m_i32Result = mmgr.Set(&m_sMsg, m_szMsgUI.c_str(), FALSE);
			if ( FAILED(m_i32Result) )
			{
				if ( m_i32Result == EIMERR_SQLITE_CONSTRAINT )
				{
					EIMLOGGER_WARN_(pILog, _T("Constraint QMID: %llu, QSID:%llu, Time:%d, Msg:%s, error:0x%08X, desc:%s"),
						m_sMsg.qmid, m_sMsg.qsid, m_sMsg.dwTime, m_szMsgUI.c_str(),	m_i32Result, GetErrorDesc());
					m_bConstraintDb = TRUE;
					m_i32Result     = EIMERR_NO_ERROR;
				}
				else
					EIMLOGGER_ERROR_(pILog, _T("QMID: %llu, QSID:%llu, Time:%d, Msg:%s, error:0x%08X, desc:%s"),
					m_sMsg.qmid, m_sMsg.qsid, m_sMsg.dwTime, m_szMsgUI.c_str(),	m_i32Result, GetErrorDesc());

				//		if ( m_sData.cOffline ) 
				//			s_u32OffMsgCount++;

				//如果是一呼万应不发送SendMsgNoticeAck
				if(m_sData.cAllReply != eSESSION_TYPE_1TO_TEN_THOUSAND)
					pICa->SendMsgNoticeAck(m_sData.dwMsgID, m_sData.dwNetID);
                
				return m_i32Result;
			}		

			smgr.Set(m_sMsg.qsid, eSET_TYPE_UTIME, m_sData.dwSendTime);
			if ( m_sData.cOffline ) 
			{
				//		s_u32OffMsgCount++;
				smgr.Set(m_sMsg.qsid, eSET_TYPE_OFF_MSG, 1);	// Add 1
				//smgr.Set(m_sMsg.qsid, eSET_TYPE_FLAG,eSEFLAG_OFFLINE | (1 << m_sMsg.eType));//上行正确设置了FLAG
				//m_sMsg.dwFlag = DB_FLAG_MSG_OFFLINE;
				if ( m_sData.nOffMsgTotal == /*s_u32OffMsgCount*/m_sData.nOffMsgSeq )
					smgr.Set(m_sMsg.qsid, eSET_TYPE_CHECKPOINT, 0);	// Sync db connection data
			}
			else
			{
				//如果是一呼万应不递增未读消息
				if(m_sData.cAllReply != eSESSION_TYPE_1TO_TEN_THOUSAND)
					smgr.Set(m_sMsg.qsid, eSET_TYPE_NEW_MSG, 1);	// Add 1
				//smgr.Set(m_sMsg.qsid, eSET_TYPE_FLAG, eSEFLAG_NEW | ( 1 << m_sMsg.eType));//上行正确设置了FLAG
				//m_sMsg.dwFlag = DB_FLAG_MSG_NEW;
			}

			if( sSInfo.eFlag & eSEFLAG_DELETE ) 
				smgr.UpdateFlag(m_sMsg.qsid, eSEFLAG_DELETE, FALSE);

			if(eSESSION_TYPE_1TO_HUNDRED == m_sData.cAllReply || 1==m_sData.cRead)
			{
				QEID qeidLogin = eng.GetPurview(GET_LOGIN_QEID);
				if(qeidLogin == m_sData.dwSenderID)
				{
					S_SessionEmps sSessionEmp;
					sSessionEmp.self_qeid = qeidLogin;
					smgr.EnumMember(m_sMsg.qsid, GetEmpByQSID, (void*)&sSessionEmp);
					
					C_eIMMsgReadMgr readmgr;
					readmgr.Init(pIDb);
                    if(0 == sSessionEmp.vecMember.size() || m_bGetGroupInfo)
                    {
                        S_UpdateReadMsg saveData;
                        saveData.qsid = m_sMsg.qsid;
                        saveData.qmid = m_sMsg.qmid;
                        eng.SaveNeedUpdateRead(saveData);
                    }
                    else
					    readmgr.AddMsgReadEmps(m_sMsg.qmid, sSessionEmp.vecMember);
				}
                else
                {
                    C_eIMMsgReadErrorMgr cMsgReadError;
                    cMsgReadError.Init(pIDb);
                    cMsgReadError.Set(m_sData.dwMsgID, m_sData.dwSenderID);
                }
			}
		}
		//如果是一呼万应不发送SendMsgNoticeAck
		if(m_sData.cAllReply != eSESSION_TYPE_1TO_TEN_THOUSAND)
			m_i32Result = pICa->SendMsgNoticeAck(m_sData.dwMsgID, m_sData.dwNetID);

	    EIMLOGGER_DEBUG_(pILog, _T("db process OK QMID: %llu end"),m_sMsg.qmid);

		return EIMERR_NO_ERROR;
	}

	int	C_CmdMsgNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		if(eIM_MSGTYPE_SMS_RECEIPT == m_sData.cMsgType)
		{
			int i32Len = sizeof(SMStoclient);
			int i32MsgLen = strlen((const char*)(&m_sData.aszMessage[10]));
			//if(strlen((const char*)m_sData.aszMessage) >= i32Len+3*sizeof(UINT32))
			{
				SMSbatchtoclient sBatch = { 0 };

				memcpy(&sBatch, (char*)(&m_sData.aszMessage[10]), sizeof(SMSbatchtoclient));
				UINT32 u32FailNum = ntohl(sBatch.dwFailNum);
				if(u32FailNum <= 0)
					return EIMERR_NO_ERROR;
				vector<S_SmsNotify> vecInfo;

				int i32Start = 10 + sizeof(SMSbatchtoclient);
				for(int i32Index=0; i32Index<(int)u32FailNum; i32Index++)
				{
					SMStoclient sClient = { 0 };
					memcpy(&sClient, (char*)(&m_sData.aszMessage[i32Start]), sizeof(SMStoclient));
					i32Start += sizeof(SMStoclient);
					
					UINT8 u8Type = /*ntohl*/(sClient.cResultType);  //服务端没有做转换
					if(0 == u8Type)
						continue;

					S_SmsNotify sNotify = { 0 };
					sNotify.u64Phone = ntohl64(sClient.dwPhoneNum);

					eIMString szDesc;
					eIMUTF8ToTChar((const char*)sClient.aszDescribe, szDesc);
					_tcscpy(sNotify.szDesc, szDesc.c_str());
					vecInfo.push_back(sNotify);
				}
				if(!vecInfo.empty())
					SAFE_SEND_EVENT_(pIEMgr, EVENT_SMS_RECV_MSG, (void*)&vecInfo);
			}
		}
		else if(eIM_MSGTYPE_SMS_RSP == m_sData.cMsgType)
		{
			SAFE_SEND_EVENT_(pIEMgr, EVENT_SMS_RECV_REPLY, (void*)&m_sReply);
		}
        else if (eIM_MSGTYPE_MSG_RECALL == m_sData.cMsgType)
        {
            //处理消息被召回,通知界面更新
            SENDRECALLMSGACK sSendData;
            sSendData.cType		= eIM_MSGTYPE_MSG_RECALL;
			sSendData.dwMsgID	= m_sMsg.qmid;
			sSendData.qSessionID= m_sMsg.qsid;
			sSendData.eidSender = m_sMsg.qeid;
			sSendData.dwTime	= m_sMsg.dwTime;
            sSendData.result	= true;
            pIEMgr->SendEvent(EVENT_SEND_RECALL_MSG_ACK, &sSendData);
            
			//因为修改了消息类型，所以不会走else中的这部分代码就不会发送离线消息弹窗提醒了 by-fxy
			if ( m_sData.cOffline &&m_sData.nOffMsgTotal == /*s_u32OffMsgCount*/m_sData.nOffMsgSeq )	// Receive offline message finished
			{
				//	Sleep(3000);	//3s
				EIMLOGGER_DEBUG_(pILog, _T("Receive offline message finished, seq:%d"), m_sData.nOffMsgSeq);
				SAFE_SEND_EVENT_(pIEMgr, EVENT_RECV_OFFLINE_MSG, NULL);
			}
        }
		else
		{
			if ( m_bGetGroupInfo)
			{
				if(m_sMsg.eType == eSESSION_TYPE_MULTI)
				{
                    EIMLOGGER_DEBUG_(pILog, _T("local has not group info groupid:%llu"), m_sMsg.qsid);
					SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_GROUP_INFO, (void*)&m_sMsg.qsid);
				}
				else if(m_sMsg.eType == eSESSION_TYPE_RGROUP)
				{
                    EIMLOGGER_DEBUG_(pILog, _T("local has not vgroup info grouptime:%lu"), m_sMsg.dwTime);
					SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_VGROUP_INFO, (void*)&m_sMsg.dwTime);
				}
			}


			if ( m_sData.cOffline &&m_sData.nOffMsgTotal == /*s_u32OffMsgCount*/m_sData.nOffMsgSeq )	// Receive offline message finished
			{
				//	Sleep(3000);	//3s
				EIMLOGGER_DEBUG_(pILog, _T("Receive offline message finished, seq:%d"), m_sData.nOffMsgSeq);
				SAFE_SEND_EVENT_(pIEMgr, EVENT_RECV_OFFLINE_MSG, NULL);
			}

			if ( SUCCEEDED( m_i32Result ) && m_bConstraintDb == FALSE &&				// Except the Constraint or error
				 Eng.GetAttributeInt(IME_ATTRIB_CONTACTS_LOADED) )	// Contacts loaded
			{
				SAFE_SEND_EVENT_(pIEMgr, EVENT_RECV_MSG, (void*)this);
			}
			else
			{
		//		EIMLOGGER_DEBUG_(pILog, _T("UI process NOT OK QMID: %llu"),m_sMsg.qmid);
			}
		}
	//	EIMLOGGER_DEBUG_(pILog, _T("UI process OK QMID: %llu"),m_sMsg.qmid);

		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdMsgNotice::ToXml(I_SQLite3* pIDb)
	{
 		GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		C_eIMEngine& eng = Eng;
		
		m_bGetGroupInfo = FALSE;
		m_bConstraintDb = FALSE;

		char		 szBuf[CONVERT_TO_STRING_SIZE];
		eIMString	 szValue;
		eIMStringA	 szValueA;
		TiXmlElement MsgEl( PROTOCOL_ELEMENT_CMD);
		TiXmlElement MsgUiEl(FIELD_MSGUI);

		// Set attribute
		MsgEl.SetAttribute( FIELD_ID,		 eCMD_MSG_NOTICE);
		MsgEl.SetAttribute( FIELD_RECVER_ID, (int)m_sData.dwRecverID );
		MsgEl.SetAttribute( FIELD_MSG_ID,    GET_QSID_ASTR_(m_sData.dwMsgID, szBuf) );
		MsgEl.SetAttribute( FIELD_IS_GROUP,  m_sData.cIsGroup	);
		MsgEl.SetAttribute( FIELD_TYPE,      m_sData.cMsgType	);
		MsgEl.SetAttribute( FIELD_MSG_TOTAL, m_sData.nMsgTotal	);
		MsgEl.SetAttribute( FIELD_MSG_SEQ,   m_sData.nMsgSeq	);
		MsgEl.SetAttribute( FIELD_MSG_LEN,   m_sData.dwMsgLen	);

		if ( m_sData.cMsgType == eIM_MSGTYPE_ROBOT_SUGGEST )	// Not write database
			m_dwFlag &= ~(CMD_FLAG_THREAD_DB );	

		if ( m_sData.cOffline )
		{
			MsgEl.SetAttribute(FIELD_OFF_MSG_TOTAL, m_sData.nOffMsgTotal);
			MsgEl.SetAttribute(FIELD_OFF_MSG_SEQ,   m_sData.nOffMsgSeq );
		}

		m_sMsg.dwTime= m_sData.dwSendTime;
		m_sMsg.qmid  = m_sData.dwMsgID;
		m_sMsg.qeid  = m_sData.dwSenderID;
		m_sMsg.eType = eSESSION_TYPE_SINGLE;
		m_sMsg.dwFlag= 0;	// Maybe set m_sData.cMsgType...
		if ( m_sData.cIsGroup == 1 )
		{
			m_sMsg.eType = eSESSION_TYPE_MULTI;
			m_sMsg.qsid  = GroupId2Qsid((char*)m_sData.aszGroupID);
			MsgEl.SetAttribute(FIELD_GROUP_TIME, m_sData.dwGroupTime);
			SET_GROUP_ID_ATTR_(m_sData.aszGroupID, MsgEl);
			MsgUiEl.SetAttribute( FIELD_TYPE, eSESSION_TYPE_MULTI);
		}
		else if ( m_sData.cIsGroup == 2 )
		{
			m_sMsg.eType = eSESSION_TYPE_RGROUP;
			m_sMsg.qsid  = GroupId2Qsid((char*)m_sData.aszGroupID);
			MsgEl.SetAttribute(FIELD_GROUP_TIME, m_sData.dwGroupTime);
			SET_GROUP_ID_ATTR_(m_sData.aszGroupID, MsgEl);
			MsgUiEl.SetAttribute( FIELD_TYPE, eSESSION_TYPE_RGROUP);
		}
		/*else if (m_sData.cAllReply == eSESSION_TYPE_1TO_HUNDRED || m_sData.cAllReply == eSESSION_TYPE_1TO_TEN_THOUSAND)
		{
			m_sMsg.eType = (E_SessionType)m_sData.cAllReply;
			m_sMsg.qsid  = GroupId2Qsid((char*)m_sData.aszGroupID);
			SET_GROUP_ID_ATTR_(m_sData.aszGroupID, MsgEl);
			MsgUiEl.SetAttribute( FIELD_TYPE, m_sData.cAllReply);
		}*/
		else
		{
			C_eIMSessionMgr smgr;
			smgr.Init(pIDb);
			S_SessionInfo  sSInfo = { 0 };

			if(eSESSION_TYPE_1TO_TEN_THOUSAND == m_sData.cAllReply)
			{
				m_sMsg.qsid = m_sData.dwSrcMsgID;
				m_sMsg.eType = eSESSION_TYPE_1TO_TEN_THOUSAND;
			}
			else
			{
				QSID qsid = 0;
				if (m_sData.aszGroupID[0] != 0)
				{
					qsid = m_sData.aszGroupID[0] - '0';
					for( int i32Index = 1; i32Index < 20 && m_sData.aszGroupID[i32Index]; i32Index++)
					{
						qsid *= 10;
						qsid += m_sData.aszGroupID[i32Index] - '0';
					}
				}
				
				smgr.Get(m_sData.dwSenderID, m_sData.dwRecverID, &sSInfo, qsid);
				m_sMsg.qsid = sSInfo.qsid;
			}

			MsgUiEl.SetAttribute( FIELD_TYPE, eSESSION_TYPE_SINGLE);
			MsgEl.SetAttribute( FIELD_GROUP_ID, GET_QSID_ASTR_(m_sMsg.qsid, szBuf) );
			memcpy(m_sData.aszGroupID, szBuf, sizeof(m_sData.aszGroupID));
		}

		//m_sMsg.u161ToHundredFlag = m_sData.cAllReply;
		//::eIMTChar2UTF8(pIConts->GetEmpName(m_sData.dwSenderID), szValueA);

		//if( ( m_sData.cRead == 1 || m_sData.cAllReply == 1 ) && objEgn.GetPurview(GET_LOGIN_QEID) != m_sMsg.qeid )
		QEID selfQeid = eng.GetPurview(GET_LOGIN_QEID);
		if(m_sData.cAllReply == eSESSION_TYPE_1TO_HUNDRED)
		{
			MsgUiEl.SetAttribute( FIELD_READ, 0 );
			MsgUiEl.SetAttribute(FILED_IS_1_TO_HUNDRED, m_sData.cAllReply);
			MsgUiEl.SetAttribute( FIELD_MSG_ID,    GET_QSID_ASTR_(m_sData.dwMsgID, szBuf) );
		}
		else if(eSESSION_TYPE_1TO_TEN_THOUSAND == m_sData.cAllReply)
		{
			MsgUiEl.SetAttribute(FIELD_READ, 0);
			MsgUiEl.SetAttribute(FILED_IS_1To10T, 1);
			MsgUiEl.SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sData.dwMsgID, szBuf));
		}
		else if(m_sData.cRead == 1  /*&& selfQeid != m_sMsg.qeid*/)
		{
			MsgUiEl.SetAttribute( FIELD_READ, 1 );
			MsgUiEl.SetAttribute( FIELD_MSG_ID,    GET_QSID_ASTR_(m_sData.dwMsgID, szBuf) );
		}
		else
		{
			MsgUiEl.SetAttribute( FIELD_READ, 0 );
		}

		MsgUiEl.SetAttribute( FIELD_AT, 0 );	// 还没有点@消息（@提示栏）
		
// Mask it for: can not auto download picture/audio..
//		if(selfQeid != m_sMsg.qeid)
			MsgUiEl.SetAttribute( FIELD_IS_SEND, 0 );
//		else
//			MsgUiEl.SetAttribute( FIELD_IS_SEND, 1 );

		//判断是由谁发送的，提供给一呼万应与回执界面显示使用
		//1、表示自己发送 2，表示别人发送
		if(selfQeid != m_sMsg.qeid)
			MsgUiEl.SetAttribute(FILED_IS_SELF_SEND, 2);
		else
			MsgUiEl.SetAttribute(FILED_IS_SELF_SEND, 1);

		//MsgUiEl.SetAttribute( FIELD_SENDER_NAME, szValueA.c_str());	// The name maybe is wrong when the contacts changed but not received the change
		MsgUiEl.SetAttribute( FIELD_SEND_TIME, (int)m_sData.dwSendTime );
		MsgUiEl.SetAttribute( FIELD_SENDER_ID, (int)m_sData.dwSenderID );

		if ( const TCHAR* pszName = pIConts->GetEmpName(m_sData.dwSenderID) )
		{	// 处理离职人员无法显示名字问题。
			eIMStringA szNameA;

			eIMTChar2UTF8(pszName, szNameA);
			MsgUiEl.SetAttribute( FIELD_SENDER_NAME, szNameA.c_str());
		}

		TiXmlElement e_(FIELD_MESSAGE);
		if (!Msg2Element(e_, eng) )
		{
			GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
			EIMLOGGER_ERROR_(pILog, _T("Msg2Element error QMID: %llu"),m_sMsg.qmid);
			return NULL;
		}

		if ( m_sMsg.qeid == selfQeid )
			eng.AddFontElement(m_sMsg.eType, TRUE, &MsgUiEl);
		else
			eng.AddFontElement(m_sMsg.eType, FALSE, &MsgUiEl);

		MsgUiEl.InsertEndChild(e_);
		MsgEl.InsertEndChild(MsgUiEl);

		m_xmlDoc.Clear();
		m_xmlDoc.InsertEndChild(MsgEl);

		ConvertToXml( MsgEl, m_szXml );
		ConvertToXml( MsgUiEl, m_szMsgUI);

		return m_szXml.c_str();
	}

	const TCHAR* C_CmdMsgNotice::ToXmlForSms(I_SQLite3* pIDb)
	{
		GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		C_eIMEngine& eng = Eng;

		/*m_bGetGroupInfo = FALSE;
		m_bConstraintDb = FALSE;*/

		char		 szBuf[CONVERT_TO_STRING_SIZE];
		eIMString	 szValue;
		eIMStringA	 szValueA;
		TiXmlElement MsgEl( PROTOCOL_ELEMENT_CMD);
		TiXmlElement MsgUiEl(FIELD_MSGUI);

		// Set attribute
		MsgEl.SetAttribute( FIELD_ID,		 eCMD_MSG_NOTICE);
		MsgEl.SetAttribute( FIELD_RECVER_ID, (int)m_sData.dwRecverID );
		MsgEl.SetAttribute( FIELD_MSG_ID,    GET_QSID_ASTR_(m_sData.dwMsgID, szBuf) );
		MsgEl.SetAttribute( FIELD_TYPE,      m_sData.cMsgType	);
		MsgEl.SetAttribute( FIELD_READ,      m_sData.cRead		);
		MsgEl.SetAttribute( FIELD_MSG_TOTAL, m_sData.nMsgTotal	);

		m_sMsg.dwTime= m_sMsg.dwTime;
		m_sMsg.qmid  = m_sData.dwMsgID;
		m_sMsg.qeid  = m_sData.dwSenderID;
		m_sMsg.eType = eSESSION_TYPE_SINGLE;
		m_sMsg.dwFlag= eIM_MSGTYPE_TEXT;	// Maybe set m_sData.cMsgType...

		PS_EmpInfo_ psSendInfo = pIConts->GetEmp(m_sData.dwSenderID);
		PS_EmpInfo_ psRecvInfo = pIConts->GetEmp(m_sData.dwRecverID);

		TCHAR szPhone[64] = {0};
		Int64ToStr(m_sReply.u64Phone, szPhone, FALSE);
		eIMStringA szPhoneA;
		eIMTChar2UTF8((const TCHAR *)szPhone, szPhoneA);
		if(psSendInfo)
		{
			::eIMTChar2UTF8(pIConts->GetEmpName(m_sData.dwSenderID), szValueA);
			eIMStringA szCodeA;
			eIMTChar2UTF8(psSendInfo->pszCode, szCodeA);
			MsgUiEl.SetAttribute( FIELD_USER_CODE,	 szCodeA.c_str());
			MsgUiEl.SetAttribute(SMS_RECV_PHONE,	 szPhoneA.c_str());
			MsgUiEl.SetAttribute( FIELD_SENDER_NAME, szValueA.c_str());
			MsgUiEl.SetAttribute( FIELD_SENDER_ID,	 (int)m_sData.dwSenderID );
		}
		else
		{
			MsgUiEl.SetAttribute( FIELD_USER_CODE,	 szPhoneA.c_str());
			MsgUiEl.SetAttribute(SMS_RECV_PHONE,	 szPhoneA.c_str());
			MsgUiEl.SetAttribute( FIELD_SENDER_NAME, szPhoneA.c_str());
			MsgUiEl.SetAttribute( FIELD_SENDER_ID,	 (int)m_sData.dwSenderID );
		}

		if(psRecvInfo)
		{
			eIMStringA szNameA, szCodeA;
			eIMTChar2UTF8(pIConts->GetEmpName(psRecvInfo), szNameA);
			eIMTChar2UTF8(psRecvInfo->pszCode, szCodeA);
			MsgUiEl.SetAttribute(FIELD_RECVER_NAME,	 szNameA.c_str());
			MsgUiEl.SetAttribute(FIELD_RECV_UCODE,	 szCodeA.c_str());
			MsgUiEl.SetAttribute(FIELD_RECVER_ID,	 psRecvInfo->Id);
		}
		else
		{
			PS_EmpInfo_ psLogin = eng.GetLoginEmp();
			eIMStringA szNameA, szCodeA;
			eIMTChar2UTF8(pIConts->GetEmpName(psLogin), szNameA);
			eIMTChar2UTF8(psLogin->pszCode, szCodeA);
			MsgUiEl.SetAttribute(FIELD_RECVER_NAME,	 szNameA.c_str());
			MsgUiEl.SetAttribute(FIELD_RECV_UCODE,	 szCodeA.c_str());
			MsgUiEl.SetAttribute(FIELD_RECVER_ID,	 psRecvInfo->Id);
		}
		
		MsgUiEl.SetAttribute( FIELD_SEND_TIME,	 (int)m_sData.dwSendTime );
		MsgUiEl.SetAttribute( FIELD_IS_SEND,	 0 );
		MsgUiEl.SetAttribute( FIELD_TYPE,		 eSESSION_TYPE_SMS);

		TiXmlElement e_(SMS_MSG);
		if (!Msg2Element(e_, eng) )
			return FALSE;

		eng.AddFontElement(m_sMsg.eType, FALSE, &MsgUiEl);
		MsgUiEl.InsertEndChild(e_);
		MsgEl.InsertEndChild(MsgUiEl);

		m_xmlDoc.Clear();
		m_xmlDoc.InsertEndChild(MsgEl);

		ConvertToXml( MsgEl, m_szXml );
		ConvertToXml( MsgUiEl, m_szMsgUI);

		return m_szXml.c_str();
	}

	// 红包：{"userId" : "10", "type" : "redPacket", "redPacketId" : "ZkZN\/1\/BGbMV\/MQ+Obj7\/VKmPYp\/FUWbKLrh6Hee7i0=", "greeting" : "恭喜发财，大吉大利！"}
	// 地图: {"location":{"address":"中国广东省深圳市南山区海天二路","latitude":"22.528234","longitude":"113.944366"},"type":"location"}
	// 轻应用：{"apptype":2, "subtype":"待阅", "title":"请查阅管理员提交的流程：测试测试", "content":"请查阅管理员提交的流程：测试测试",
	//          "url":  "http://oa01.tahoecn.com:8080/ekp/km/review/km_review_main/kmReviewMain.do?method=view&fdId=15aa674923dda80d90fb06c4448a85e1",
	//          "sender":"流程管理","sendtime":1488851357}
	// 定向回复：{"type":"replyTo", "msgId":"4611687014204689924","content":"@史素萍 sdfsafsafsdafsdafds", "userId":7}
	BOOL  C_CmdMsgNotice::IsTypeOfMsg(const char* pszMsg, const char* pszType, const char* pszValue)
	{
		BOOL bRet = FALSE;
		if (cJSON* pjsonRoot = cJSON_Parse(pszMsg))
		{
			if ( cJSON* pItem = cJSON_GetObjectItem(pjsonRoot, pszType) )
			{
				bRet = TRUE;
				if ( pszValue && pItem->type == cJSON_String )
					bRet &= (stricmp(pItem->valuestring, pszValue) == 0);
			}

			cJSON_Delete(pjsonRoot);
		}

		return bRet;
	}

	const BOOL C_CmdMsgNotice::Msg2Element(TiXmlElement& MsgElement, C_eIMEngine& objEgn)
	{
		GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);

#define GET_FILE_PATH_(FILE, DIR, KEY) \
	fid = hpi.GetQfid(KEY); \
	::eIMUTF8ToTChar(FILE, szValue); \
	::eIMGetFullPath(PATH_TOKEN_LOGINID_DOC, DIR, szValue.c_str(), szValue); \
	::eIMTChar2UTF8(szValue.c_str(), szValueA);

#define GET_FILE_PATH_NAME(DIR) \
	do{ pos = DIR.rfind("/");\
    if (pos == eIMStringA::npos) {\
        pos = DIR.rfind("\\");\
        if (pos == eIMStringA::npos) {szFileNameA = DIR; break;} }\
    pos += 1;\
	szFileNameA = DIR.substr(pos, DIR.length() - pos);break;}while(1)

		char			szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		QFID			fid;
		eIMString		szValue;
		eIMStringA		szValueA;
		eIMStringA		szFileNameA;
		unsigned long pos = 0;
		TiXmlElement    MsgFontElement( FIELD_MSGFONT );
		switch ( m_sData.cMsgType )	// ERCR_TYPE
		{
		case eIM_MSGTYPE_TEXT: 	// ERCR_DETAILS = 0,		//聊天内容(表情和文字和图片)
		case eIM_MSGTYPE_ROBOT_SUGGEST:	// Intelligent input prompt response
			{
				MsgFontElement.SetAttribute( FIELD_ID,   m_sData.aszMessage[0] );
				MsgFontElement.SetAttribute( FIELD_SIZE, m_sData.aszMessage[1] < 6 ? 12 : m_sData.aszMessage[1] );
				MsgFontElement.SetAttribute( FIELD_MODE, m_sData.aszMessage[2] );
				MsgFontElement.SetAttribute( FIELD_COLOR,(int)RGB(m_sData.aszMessage[3], m_sData.aszMessage[4], m_sData.aszMessage[5] ));
				MsgElement.InsertEndChild(MsgFontElement);	// Add font at first

				BOOL		 bSubType = FALSE;
				const char* pszMsg	 = &m_sData.aszMessage[10];
                const char* pRobotStart = strstr(pszMsg, "<robotResponse>");
                const char* pRobotEnd = strstr(pszMsg, "</robotResponse>");
                eIMStringA szRobotContent;
                if(pRobotStart && pRobotEnd) //增加结尾的判断,防止空指针导致线程崩溃
                {
                    SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
					//szValueA.assign( pRobotStart, pRobotEnd - pRobotStart  + strlen("</robotResponse>") );
                    szValueA.assign(pszMsg, m_sData.dwMsgLen - 10);
					TiXmlElement TextElement( FIELD_MSGTEXT );
					TiXmlText    Text(szValueA.c_str());
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
                }
				else if ( IsTypeOfMsg(pszMsg, "apptype") )
				{	// 轻应用
					m_sData.cMsgType = eIM_JSON;	// 类型转换成 WebApp JSON
					SetFlag(CMD_FLAG_MSG_JSON, CMD_FLAG_MSG_JSON);
					TiXmlElement TextElement( FIELD_MSGWEBAPP );
					TiXmlText    Text(pszMsg);
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}
				else if ( IsTypeOfMsg(pszMsg, "type", "location") )
				{	// 地图
					m_sData.cMsgType = eIM_JSON;	// 类型转换成 地图 JSON
					SetFlag(CMD_FLAG_MSG_JSON, CMD_FLAG_MSG_JSON);
					TiXmlElement TextElement( FIELD_MSGMAP );
					TiXmlText    Text(pszMsg);
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}
				else if ( IsTypeOfMsg(pszMsg, "type", "redPacket") )
				{	// 红包
					m_sData.cMsgType = eIM_JSON;	// 类型转换成 红包 JSON
					SetFlag(CMD_FLAG_MSG_JSON, CMD_FLAG_MSG_JSON);
					TiXmlElement TextElement( FIELD_MSGREDPACKET );
					TiXmlText    Text(pszMsg);
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}
				else if ( IsTypeOfMsg(pszMsg, "type", "redPacketAction") )
				{	// 红包动作
					m_sData.cMsgType = eIM_JSON;	// 类型转换成 红包 JSON
					SetFlag(CMD_FLAG_MSG_JSON, CMD_FLAG_MSG_JSON);
					TiXmlElement TextElement( FIELD_MSGREDPACKETACT );
					TiXmlText    Text(pszMsg);
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}
				else if ( IsTypeOfMsg(pszMsg, "type", "replyTo") )
				{	// 定向回复
					m_sData.cMsgType = eIM_JSON;	// 类型转换成 定向回复 JSON
					SetFlag(CMD_FLAG_MSG_JSON, CMD_FLAG_MSG_JSON);
					TiXmlElement TextElement( FIELD_MSGREPLYTO );
					TiXmlText    Text(pszMsg);
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}				
				else if ( IsTypeOfMsg(pszMsg, "type", "secret") )
				{	// 秘聊
					m_sData.cMsgType = eIM_JSON;	// 类型转换成 秘聊 JSON
					SetFlag(CMD_FLAG_MSG_JSON, CMD_FLAG_MSG_JSON);
					TiXmlElement TextElement( FIELD_MSGSECRET );
					TiXmlText    Text(pszMsg);
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}
				else
                {
				    const char* pszStart = strchr(pszMsg, '[');
				    const char* pszEnd   = pszStart ? strchr(pszStart, ']') : NULL;
				    const char* pszChar  = NULL;
				    for ( ; pszStart && pszEnd; )
				    {
					    if ( pszEnd < pszStart + 3)
					    {	// Invalid [/xx]
						    pszStart = strchr(pszEnd + 1, '[');
						    pszEnd   = pszStart ? strchr(pszStart + 1, ']') : NULL;
						    continue;
					    }
                   
					    szValueA.assign( pszStart + 1, pszEnd - pszStart - 1 );
					    TiXmlElement SubEl( FIELD_MSGEMOT );
					    if ( pszStart[1] == _T('/') )		// MsgEmot
					    {	// eg.: [/wx][/han]
						    //szValueA.erase( 0, 1 );	// Remove '/'
						    ::eIMUTF8ToTChar(szValueA.c_str(), szValue);
						    if ( pIEmot->IsExistEmot(szValue.c_str()) )
						    {
							    bSubType = TRUE;
							    SetFlag(CMD_FLAG_MSG_EMOT, CMD_FLAG_MSG_EMOT);
							    SubEl.SetAttribute( FIELD_NAME, szValueA.c_str() );
						    }
						    else
						    {
							    pszStart = strchr(pszStart + 1, '[');
							    pszEnd   = pszStart ? strchr(pszStart + 1, ']') : NULL;
							    continue;
						    }
					    }
					    else if ( pszStart[1] == _T('#') && IsFileKey(szValueA.substr(1).c_str()) )	// MsgImage
					    {	// eg.: [#m6R73a.jpg]
						    szValueA.erase( 0, 1);
						    bSubType = TRUE;

						    SetFlag(CMD_FLAG_MSG_IMAGE, CMD_FLAG_MSG_IMAGE);

						    szFileNameA = szValueA.substr(0, szValueA.rfind('.'));
						    SubEl.SetValue( FIELD_MSGIMAGE );
						    SubEl.SetAttribute( FIELD_KEY, szFileNameA.c_str() );
			
						    GET_FILE_PATH_(szValueA.c_str(), PATH_TYPE_APPDOC_IMAGE, szValueA.c_str());
						    SubEl.SetAttribute( FIELD_FID, GET_QSID_ASTR_(fid, szBuf));
						    SubEl.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
						    CheckAndSetFileAttribute(&SubEl, szValueA.c_str(), SubEl.Attribute(FIELD_KEY));
					    }
					    else
					    {	// Maybe need modify here...
						    pszStart = strchr(pszStart + 1, '[');
						    pszEnd   = pszStart ? strchr(pszStart + 1, ']') : NULL;
						    continue;
					    }

					    if ( bSubType )
					    { 
						    if ( pszMsg != pszStart )
						    {	// Add pre-head TEXT
							    SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
							    szValueA.assign( pszMsg, pszStart - pszMsg );
							    TiXmlElement TextElement( FIELD_MSGTEXT );
							    TiXmlText    Text(szValueA.c_str());
							    Text.SetCDATA(true);

							    TextElement.InsertEndChild(Text);
							    MsgElement.InsertEndChild( TextElement );
						    }

						    MsgElement.InsertEndChild( SubEl );	// Add MsgEmot or MsgImage
					    }
					    else
					    {	// Add as TEXT
						    SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
						    szValueA.assign( pszMsg, pszEnd - pszMsg );
						    TiXmlElement TextElement( FIELD_MSGTEXT );

						    TiXmlText    Text(szValueA.c_str());
						    Text.SetCDATA(true);

						    TextElement.InsertEndChild(Text);
						    MsgElement.InsertEndChild( TextElement );
					    }

					    pszMsg = pszEnd + 1;
					    if (bSubType)
					    {
						    bSubType = FALSE;
						    pszStart = strchr(pszMsg, '[');
						    pszEnd   = strchr(pszMsg, ']');	
					    }
				    } // for ( ; pszStart && pszEnd; pszStart = 
                    if ( pszMsg && *pszMsg )
				    {
					    SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
					    TiXmlElement TextElement( FIELD_MSGTEXT );
					    TiXmlText    Text(pszMsg);
					    Text.SetCDATA(true);

					    TextElement.InsertEndChild(Text);
					    MsgElement.InsertEndChild( TextElement );
				    }
                }				
			}
			break;
		case eIM_MSGTYPE_IMG: 		// ERCR_IMAGE = 1,			//图片
			{	
				SetFlag(CMD_FLAG_MSG_IMAGE, CMD_FLAG_MSG_IMAGE);
				TiXmlElement MsgImage( FIELD_MSGIMAGE );
				FILE_META* psFileMeta = (FILE_META*)m_sData.aszMessage;
				GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
				MsgImage.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
				psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
				CheckAndSetFileAttribute(&MsgImage, szValueA.c_str(), MsgImage.Attribute(FIELD_KEY));
				MsgImage.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
				MsgImage.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
				MsgImage.SetAttribute(FIELD_KEY, psFileMeta->aszURL);
				MsgImage.SetAttribute(FIELD_NAME, szValueA.c_str());

				objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);
				MsgElement.InsertEndChild(MsgImage);	// Add MsgImage
			}
			break;
		case eIM_MSGTYPE_VOICE:		// ERCR_VPOCE = 2,			//语音
		{
			SetFlag(CMD_FLAG_MSG_VOICE, CMD_FLAG_MSG_VOICE);
			objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

			TiXmlElement MsgVoice( FIELD_MSGVOICE );
			FILE_META* psFileMeta = (FILE_META*)m_sData.aszMessage;

			psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
			GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
			MsgVoice.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
			GET_FILE_PATH_NAME(szValueA);
			CheckAndSetFileAttribute(&MsgVoice, szValueA.c_str(), psFileMeta->aszURL);

			MsgVoice.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
			MsgVoice.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
			MsgVoice.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
			MsgVoice.SetAttribute( FIELD_NAME, szFileNameA.c_str() );

			MsgElement.InsertEndChild(MsgVoice);	// Add MsgVoice
		}
			break;
		case eIM_MSGTYPE_VIDEO: 	// ERCR_VIDEO = 3,			//视频
            {
			    SetFlag(CMD_FLAG_MSG_VIDEO, CMD_FLAG_MSG_VIDEO);
                objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

			    TiXmlElement MsgVideo( FIELD_MSGVIDEO );
			    FILE_META* psFileMeta = (FILE_META*)m_sData.aszMessage;

			    psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
			    GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
			    MsgVideo.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
			    GET_FILE_PATH_NAME(szValueA);
			    CheckAndSetFileAttribute(&MsgVideo, szValueA.c_str(), psFileMeta->aszURL);

			    MsgVideo.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
			    MsgVideo.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
			    MsgVideo.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
			    MsgVideo.SetAttribute( FIELD_NAME, szFileNameA.c_str() );

			    MsgElement.InsertEndChild(MsgVideo);	// Add MsgVideo
            }
			break;
		case eIM_MSGTYPE_FILE: 		// ERCR_ANNEX = 4,			//其它（附件）
			{	
				SetFlag(CMD_FLAG_MSG_ANNEX, CMD_FLAG_MSG_ANNEX);
				objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

				TiXmlElement MsgAnnex( FIELD_MSGANNEX );
				FILE_META* psFileMeta = (FILE_META*)m_sData.aszMessage;

				psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
				GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
				MsgAnnex.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
				GET_FILE_PATH_NAME(szValueA);
				CheckAndSetFileAttribute(&MsgAnnex, szValueA.c_str(), psFileMeta->aszURL);

				MsgAnnex.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
				MsgAnnex.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
				MsgAnnex.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
				MsgAnnex.SetAttribute( FIELD_NAME, szFileNameA.c_str() );
				MsgAnnex.SetAttribute(FILE_ORINAME,psFileMeta->aszFileName);

				MsgElement.InsertEndChild(MsgAnnex);	// Add MsgAnnex
			}
			break;
		case eIM_MSGTYPE_P2P: 		// ERCR_PTOP  = 5,			//P2P 文件...
			{	// MsgP2P, Format: Key + Token + "|" + Size + "|" + Name
				TiXmlDocument doc;
				doc.Parse(m_sData.aszMessage);
				if ( !doc.Error() && doc.RootElement() )
				{
					if ( strcmp(doc.RootElement()->Value(), FIELD_MSGP2P) == 0 )
					{
						SetFlag(CMD_FLAG_MSG_P2P, CMD_FLAG_MSG_P2P);
						MsgElement.InsertEndChild(*doc.RootElement());
						break;
					}
					else if ( strcmp(doc.RootElement()->Value(), FIELD_MSGRDP) == 0 )
					{
						SetFlag(CMD_FLAG_MSG_RDP, CMD_FLAG_MSG_RDP);
						MsgElement.InsertEndChild(*doc.RootElement());
						break;
					}
				}

				ASSERT_(FALSE);
				m_i32Result  = EIMERR_BAD_CMD;
			}
			break;
		case eIM_MSGTYPE_FILERECVED: 	// ERCR_ANNEXACK = 6,		//附件应答（用于离附件）
			{	// eg.: 对方已成功接收了您发送的离线文件 "depot_tools.zip" (8.35 M)。
				SetFlag(CMD_FLAG_MSG_ANNEX_ACK, CMD_FLAG_MSG_ANNEX_ACK);

				TiXmlElement MsgAnnexAck( FIELD_MSGANNEXACK );
				MsgAnnexAck.SetAttribute( FIELD_ACK, m_sData.aszMessage);
				MsgElement.InsertEndChild(MsgAnnexAck);	// Add MsgAnnexAck
			}
			break;
		case eIM_MSGTYPE_LONGTEXT:		// ERCR_LONGMSG = 7,		//长消息
			{
				SetFlag(CMD_FLAG_MSG_LONG, CMD_FLAG_MSG_LONG);		
				objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

				SetFlag(CMD_FLAG_MSG_ANNEX, CMD_FLAG_MSG_ANNEX);
				TiXmlElement MsgLong( FIELD_MSGLONG );
				FILE_META* psFileMeta = (FILE_META*)&m_sData.aszMessage[10];

				psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
				
				//if (psFileMeta->dwFileSize <= 0)
				//	break;

				GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_MSGLONG, psFileMeta->aszURL);
				MsgLong.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
				GET_FILE_PATH_NAME(szValueA);
				CheckAndSetFileAttribute(&MsgLong, szValueA.c_str(), psFileMeta->aszURL);

				MsgLong.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
				MsgLong.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
				MsgLong.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
				MsgLong.SetAttribute( FIELD_NAME, szFileNameA.c_str() );
			//	MsgLong.SetAttribute( FIELD_SUMMARY, &m_sData.aszMessage[sizeof(FILE_META)]);
				
				eIMString szKey;
				::eIMUTF8ToTChar(psFileMeta->aszURL, szKey);
				if ( WaitMsgLong(fid, szValue.c_str(), szKey.c_str()) || 
					 WaitMsgLong(fid, szValue.c_str(), szKey.c_str()) )	// Try again when failed
				{
					FILE* pflRead = _tfopen(szValue.c_str(), _T("rb, ccs=UTF-8"));	ASSERT_(pflRead);
					TRACE_(_T("Open MsgLong file[%s] %s"), szValue.c_str(),  pflRead ? _T("Succeeded") : _T("Failed"));
					if ( pflRead )
					{
						fseek(pflRead, 0, SEEK_END);
						const int i32Size = ftell(pflRead);
						fseek(pflRead, 0, SEEK_SET);
						char* pszBuf = new char[i32Size + 1];
					 
						TRACE_(_T("Read MsgLong file[%s], size: %d %s"), szValue.c_str(), i32Size, pszBuf ? _T("Succeeded") : _T("Failed"));
						if ( pszBuf && i32Size )
						{
							fread(pszBuf, sizeof(char), i32Size, pflRead);

							TiXmlText Text(IS_BOM_UTF8(pszBuf) ? &pszBuf[3] : pszBuf);
							Text.SetCDATA(true);
							MsgLong.InsertEndChild(Text);
							SAFE_DELETE_PTR_(pszBuf);
						}

						fclose(pflRead);
					}
				}

				if ( !MsgLong.GetText() )
				{
					TRACE_(_T("Download MsgLong file failed, only display the summary"));

					TiXmlText Text(&m_sData.aszMessage[sizeof(FILE_META) + 10]);
					Text.SetCDATA(true);
					MsgLong.InsertEndChild(Text);
				}
				
				MsgElement.InsertEndChild(MsgLong);	// Add MsgLong
			}
			break;
		case eIM_MSGTYPE_PCAUTO: 	// ERCR_AUTOMSG = 8,		//自动回复消息	}
			{	// eg.: [自动回复] 现在忙
				eIMStringA szAutoTipA;
				::eIMTChar2UTF8(hpi.GetErrorLangInfo(EIMERR_MSGAUTO_TIP, _T("[自动回复] ")), szAutoTipA);
				szAutoTipA += m_sData.aszMessage;
				SetFlag(CMD_FLAG_MSG_AUTO, CMD_FLAG_MSG_AUTO);
				TiXmlElement MsgAuto( FIELD_MSGAUTO );
				TiXmlText    Text(szAutoTipA);
				Text.SetCDATA(true);
				MsgAuto.InsertEndChild(Text);

				objEgn.AddFontElement(eSESSION_TYPE_MSGAUTO, FALSE, &MsgElement);
				MsgElement.InsertEndChild(MsgAuto);	// Add MsgAuto
			}
			break;

		case eIM_MSGTYPE_SMS_RSP:
			{
				MsgFontElement.SetAttribute( FIELD_ID,   0 );
				MsgFontElement.SetAttribute( FIELD_SIZE, 11 );
				MsgFontElement.SetAttribute( FIELD_MODE, 0 );
				MsgFontElement.SetAttribute( FIELD_COLOR, 0x00000000);
				MsgElement.InsertEndChild(MsgFontElement);	// Add font at first

				eIMStringA szContextA;
				eIMTChar2UTF8(m_sReply.szContent, szContextA);

				if (!szContextA.empty())
				{
					//SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
					TiXmlElement TextElement( FIELD_SMS_TEXT );
					TiXmlText    Text(szContextA.c_str());
					Text.SetCDATA(true);

					TextElement.InsertEndChild(Text);
					MsgElement.InsertEndChild( TextElement );
				}
			}
			
			break;
		default:
			break;
		}


//Del 		//============ Begin === 2014/8/26 21:07:27 deleted by LongWQ ======
//Del 		//Reason: Not used it
//Del 		if (GetFlag(CMD_FLAG_TRANSFER))
//Del 			SetFlag(CMD_FLAG_MSG_WAIT_FILE, CMD_FLAG_MSG_WAIT_FILE);
//Del 		else
//Del 			SetFlag(0, CMD_FLAG_MSG_WAIT_FILE);
//Del 		//============ End ===== 2014/8/26 21:07:27 deleted by LongWQ ======

		return TRUE;
	}

	BOOL C_CmdMsgNotice::FromXml(const TCHAR* pszXml)
	{
		GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		C_eIMEngine& eng = Eng;

		int				i32Value = 0;
		long long		i64Value = 0ll;
		const char*		pszValue = NULL;
		eIMStringA		szValueA;
		TiXmlElement*	pEl = ParseXmlCmdHeader(m_xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		ZERO_STRUCT_(m_sMsg);

		if( pEl->Attribute(FIELD_BUSINESS_ID, &i32Value)) 
			m_dwBusinessId = i32Value;
		else
			m_dwBusinessId = 0;

		m_sMsg.qeid = eng.GetPurview(GET_LOGIN_QEID);
		char szBuf[CONVERT_TO_STRING_SIZE] = {0};
		if (pEl->Attribute(FIELD_MSG_ID) == NULL)		// Check and set QMID
		{
			m_sMsg.qmid = eng.NewID();	// By CA
			m_sData.dwMsgID = m_sMsg.qmid;
			pEl->SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sMsg.qmid, szBuf) );
		}
		else
		{
			m_sData.dwMsgID = GroupId2Qsid(pEl->Attribute(FIELD_MSG_ID));
			m_sMsg.qmid	 = m_sData.dwMsgID;
		}

		m_sData.nMsgTotal    = 1;
		m_sData.nMsgSeq      = 1;
		m_sData.nOffMsgTotal = 0;
		m_sData.nOffMsgSeq	 = 0;
		m_sData.cOffline	 = 0;
		m_sData.dwMsgID		 = GroupId2Qsid(pEl->Attribute(FIELD_MSG_ID));
		m_sData.dwSrcMsgID	 = GroupId2Qsid(pEl->Attribute(FILE_1To10T_MSG_SRC_ID));
		pEl->Attribute(FIELD_SENDER_ID,       &i32Value); m_sData.dwSenderID = i32Value;
		pEl->Attribute(FIELD_RECVER_ID,       &i32Value); m_sData.dwRecverID = i32Value;
		pEl->Attribute(FIELD_IS_GROUP,        &i32Value); m_sData.cIsGroup   = i32Value;
		pEl->Attribute(FIELD_MSG_TYPE,        &i32Value); m_sData.cMsgType   = i32Value;
		pEl->Attribute(FIELD_READ,	          &i32Value); m_sData.cRead      = i32Value;
		pEl->Attribute(FIELD_MSG_LEN,         &i32Value); m_sData.dwMsgLen   = i32Value;
		pEl->Attribute(FIELD_SEND_TIME,       &i32Value); m_sData.dwSendTime = i32Value;

		pEl->Attribute(FILED_IS_1To10T,	      &i32Value); 
		if(!!i32Value)
			m_sData.cAllReply  = eSESSION_TYPE_1TO_TEN_THOUSAND;
		

		if(eIM_MSGTYPE_TEXT == m_sData.cMsgType)
		{
			//// <MsgUI /> ==========================================================
			pEl = pEl->FirstChildElement(FIELD_MSGUI);	
			if ( pEl == NULL || _stricmp(pEl->Value(), FIELD_MSGUI) )
			{
				TRACE_(_T("Error FromXml 1 to 10 T format[%s]"), pszXml);
				m_i32Result = EIMERR_BAD_CMD;
				return FALSE;
			}

			// <MsgUI><Message /></MsgUI> =========================================
			TiXmlHandle h(pEl);
			TiXmlElement*	pMessageEl = h.FirstChildElement(FIELD_MESSAGE).ToElement();	
			if (pMessageEl == NULL)
			{
				TRACE_(_T("Error FromXml 1 to 10 T format[%s][1]"), pszXml);
				m_i32Result = EIMERR_BAD_CMD;
				return FALSE;
			}

			int i32Index = 0;
			for(pMessageEl = pMessageEl->FirstChildElement(); pMessageEl; pMessageEl = pMessageEl->NextSiblingElement())
			{	// Enumerate each child element
				pszValue = pMessageEl->Value();
				ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, NULL);
				const char* pszAttr = NULL;
				if(_stricmp(pszValue, FIELD_MSGFONT) == 0)
				{	// MsgFont
					DWORD	   dwColor   = strtol(pMessageEl->Attribute(FIELD_COLOR), NULL, 0);
					S_FONTINFO sFontInfo = { 0 };

					pszAttr = pMessageEl->Attribute(FIELD_ID);
					sFontInfo.u8Id   = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_ID_DEFAULT;

					pszAttr = pMessageEl->Attribute(FIELD_MODE);
					sFontInfo.u8Mode = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_MODE_DEFAULT;

					pszAttr = pMessageEl->Attribute(FIELD_SIZE);
					sFontInfo.u8Size = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_SIZE_DEFAULT;

					sFontInfo.au8Color[0] = (BYTE)GetRValue(dwColor);	// R
					sFontInfo.au8Color[1] = (BYTE)GetGValue(dwColor);	// G
					sFontInfo.au8Color[2] = (BYTE)GetBValue(dwColor);	// B

					memcpy(&m_sData.aszMessage[i32Index], &sFontInfo, sizeof(S_FONTINFO));
					i32Index += sizeof(S_FONTINFO);
				}
				else if(_stricmp(pszValue, FIELD_MSGTEXT ) == 0)
				{	// MsgText
					pszAttr    = pMessageEl->GetText();
					int i32Ret = strlen(pszAttr);

					ASSERT_(i32Ret);
					memcpy(&m_sData.aszMessage[i32Index], pszAttr, i32Ret);
					i32Index += i32Ret;
				}
			}
		}
		else
		{
			FILE_META sFileMeta = { 0 };

			sFileMeta.dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
			sFileMeta.dwFileSize = ntohl(sFileMeta.dwFileSize);

			strncpy(sFileMeta.aszURL, pEl->Attribute(FIELD_KEY), COUNT_OF_ARRAY_(sFileMeta.aszURL));
			strncpy(sFileMeta.aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(sFileMeta.aszFileName));
			memcpy(m_sData.aszMessage,    (void*)&sFileMeta, sizeof(FILE_META));
		}

		return TRUE;
	}

    void C_CmdMsgNotice::DealMsgRecalled2Db(I_SQLite3* pIDb)
    {
        C_eIMMsgMgr		mmgr;
		mmgr.Init(pIDb);
        //修改撤回消息的消息类型
        mmgr.UpdateType(m_sData.dwSrcMsgID, eIM_MSGTYPE_MSG_RECALL);
    }

//Del 	//============ Begin === 2014/8/2 15:35:09 deleted by LongWQ ======
//Del 	//Reason: 
//Del 	//C_CmdMsgNoticeAck///////////////////////////////////////////////////////////////////
//Del 	REGISTER_CMDCLASS(C_CmdMsgNoticeAck, eCMD_MSG_NOTICE_ACK);
//Del 	int C_CmdMsgNoticeAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
//Del 	{
//Del 		GET_CMD_REF_INTERFACE_();
//Del 		m_i32Result = pICa->SendMsgNoticeAck(m_sData);
//Del 		if ( FAILED(m_i32Result) )
//Del 			EIMLOGGER_ERROR_(pILog, _T("MsgId: %llu, result: 0x%08X, desc:%s"), m_sData , m_i32Result, GetErrorDesc());
//Del 		else
//Del 			EIMLOGGER_DEBUG_(pILog, _T("MsgId: %llu"), m_sData);
//Del 
//Del 		return m_i32Result;
//Del 	}
//Del 	//============ End ===== 2014/8/2 15:35:09 deleted by LongWQ ======

	//C_CmdGetUserState//////////////////////////////////////////////////////////////////
	//REGISTER_CMDCLASS(C_CmdGetUserState, eCMD_GET_USER_STATE);
	int C_CmdGetUserState::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetUserStateList();
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetUserStateInfo////////////////////////////////////////////////////////////////get state new way
	//REGISTER_CMDCLASS(C_CmdGetUserStateInfo, eCMD_GET_USER_STATE);
	int C_CmdGetUserStateInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		//m_i32Result = pICa->GetUserStateList();
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetUserStateAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetUserStateAck, eCMD_GET_USER_STATE_ACK);
	int C_CmdGetUserStateAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdGetUserStateAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		if (FAILED(m_i32Result))
			return m_i32Result;

		S_EmpInfo	   sEmp = { 0 };
		C_eIMContacts& obj = C_eIMContacts::GetObject();
		for ( int i32Index = 0; i32Index < m_sData.wCurrNum; i32Index++ )
		{
			USERSTATE& sState = m_sData.aUserState[i32Index];

			sEmp.Id			  = sState.dwUserID;
			sEmp.bitStatus	  = (sState.cState & IME_EMP_STATUS_MASK);
			sEmp.bitLoginType = (sState.cLoginType & IME_EMP_TERM_MASK);

			if (!obj.SetEmps(sEmp, eIMECSET_MODS, TRUE))
				EIMLOGGER_ERROR_(pILog, _T("Update user[%d]'s status failed, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
		}


		return m_i32Result;
	}

	int	C_CmdGetUserStateAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		if ( m_sData.wCurrPage == 0 )	// Reach at the end pate of GetUserState
			SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);

		return m_i32Result;
	}


	//C_CmdModifyEmp//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyEmp, eCMD_MODIFY_EMPLOYEE);
	int C_CmdModifyEmp::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
//		int i32BitIndex;
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		CONVERT_TO_ICA_RET_(pvCa);

#define SEND_INT_VALUE_(FLAG, FIELD) \
	if (SUCCEEDED(m_i32Result) && \
		(i32BitIndex = GetBitIndex(m_eModifyType, FLAG)) >= 0 ) \
		m_i32Result = pICa->ModiInfo(i32BitIndex, strlen(szBuf), GET_U32_ASTR_(FIELD, szBuf));

#define SEND_STR_VALUE_(FLAG, FIELD) \
	if ( SUCCEEDED(m_i32Result) && \
		(i32BitIndex = GetBitIndex(m_eModifyType, FLAG	  )) >= 0 )	\
		m_i32Result = pICa->ModiInfo(i32BitIndex, 0, FIELD);

		m_i32Result = EIMERR_NO_ERROR;
		/*SEND_INT_VALUE_(eMTYPE_SEX,		 m_sData.cSex);				// [bit0]Sex
		SEND_INT_VALUE_(eMTYPE_BIRTHDAY, m_sData.dwBirth);			// [bit2]Birthday
		SEND_STR_VALUE_(eMTYPE_TEL,		 m_sData.aszTel);			// [bit4]Telephone
		SEND_STR_VALUE_(eMTYPE_PHONE,	 m_sData.aszPhone);			// [bit5]Phone
		SEND_STR_VALUE_(eMTYPE_PSW,		 m_sData.aszPassword);		// [bit6]Password
		SEND_STR_VALUE_(eMTYPE_LOGO,	 m_sData.aszLogo);			// [bit7]Logo
		SEND_STR_VALUE_(eMTYPE_SIGN,	 m_sData.aszSign);			// [bit8]Sign
		SEND_INT_VALUE_(eMTYPE_PURVIEW,  m_sData.dwBirth);			// [bit9]Purview
		SEND_STR_VALUE_(eMTYPE_HOMETEL,  m_sData.aszHomeTel);		// [bit10]Home telephone
		SEND_STR_VALUE_(eMTYPE_EMRGTEL,  m_sData.aszEmergencyPhone);// [bit11]Emergency Phone
		SEND_INT_VALUE_(eMTYPE_SYN_TYPE, m_sData.cMsgSynType);		// [bit12]Message synchronization type, see: IME_EMP_SYNC_* 
		SEND_STR_VALUE_(eMTYPE_DUTY,	 m_sData.aszLocal);			// [bit13]Duty
		SEND_STR_VALUE_(eMTYPE_EMAIL,	 m_sData.aszEmail);
		*/
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdModifyEmpAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyEmpAck, eCMD_MODIFY_EMPLOYEE_ACK);
	int C_CmdModifyEmpAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	// ...
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdModifyEmpAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdSendBroadcast//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdSendBroadcast, eCMD_SEND_BROADCAST);
	int C_CmdSendBroadcast::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->SendBroadCast(
			(char*)m_sData.aRecver, 
			m_sData.wRecverNum, 
			m_sData.aszTitile, 
			m_sData.aszMessage,
			m_sData.dwMsgLen, 
			m_sData.dwMsgID,
			m_sData.dwTime,
			m_sData.cMsgType,
			m_sData.cAllReply,
			m_sData.dwSrcMsgID);

		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdSendBroadcast::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		return EIMERR_NO_ERROR;
	}

	int	C_CmdSendBroadcast::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		int i32Result = m_i32Result;

		if(EIMERR_NO_ERROR == i32Result)
		{
			C_eIMBroMgr bmgr;
			bmgr.Init(pIDb);

			S_BroInfo sBroInfo = { 0 };

			sBroInfo.qMid			= m_sData.dwMsgID;
			sBroInfo.i32timestamp	= m_sData.dwTime;
			sBroInfo.psztitle		= m_sData.aszTitile;
			sBroInfo.i32type		= 0;
			sBroInfo.i32offmsg_flag = 0;
			sBroInfo.i32newmsg_flag = 0;
			sBroInfo.i32flag		= 0;

			eIMStringA szRecvInfo;
			eIMTChar2UTF8(m_szRecver.c_str(), szRecvInfo);
			sBroInfo.pszrecver = szRecvInfo.c_str();

			i32Result = bmgr.Set(&sBroInfo, m_szMsgUI.c_str());
		}

		if ( FAILED(i32Result) )
		{
			EIMLOGGER_ERROR_(pILog, _T("Broadcast QMID: %llu,  Time:%d, Msg:%s"), m_sData.dwMsgID, m_sData.dwTime, m_szMsgUI.c_str());
			return i32Result;
		}

		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdSendBroadcast::FromXml( const TCHAR* pszXml )
	{
		GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		C_eIMEngine&   eng = Eng;
		C_eIMContacts& objContacts  = C_eIMContacts::GetObject();
		int				i32Value = 0;
		
		TiXmlElement*	pEl = ParseXmlCmdHeader(m_xmlDoc, pszXml, m_i32CmdId, m_i32Result);
		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();

		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		if ( pEl->Attribute(FIELD_MSG_ID) == NULL )		// Check and set QMID
		{
			m_sData.dwMsgID = eng.NewID();
			pEl->SetAttribute(FIELD_MSG_ID, szBuf);
		}
		else
		{
			m_sData.dwMsgID = GroupId2Qsid(pEl->Attribute( FIELD_MSG_ID));
		}


		pEl->Attribute( FIELD_MSG_LEN,  &i32Value ); 
		m_sData.dwMsgLen = i32Value;

		const char *tszTitle = (char *)pEl->Attribute(FILED_BRO_TITLE);
		strcpy(m_sData.aszTitile, tszTitle);
		/*m_sData.cMsgType  = eIM_MSGTYPE_TEXT;*/
		m_sData.cAllReply  = eBRO_BROCAST;

		const char* pszWndID = pEl->Attribute(FILED_BRO_WND_ID);

		TiXmlElement*	pEl_MsgUI = NULL;
		for(pEl = pEl->FirstChildElement(); pEl && m_sData.wRecverNum < 100; pEl = pEl->NextSiblingElement())
		{
			const char *tszValue = pEl->Value();
			if(0 == strcmp(tszValue, FILED_BRO_RECV))
			{
				BROADCAST_RECVER sBroRecv = { 0 };

				int i32Type = -1;
				pEl->Attribute(FILED_BRO_ISDEPT, &i32Type);

				eIMString szRecv;
				const char* tszRecvId = pEl->Attribute(FIELD_RECVER_ID);
				eIMUTF8ToTChar(tszRecvId, szRecv);
				
				sBroRecv.dwRecverID = Str2Int(szRecv.c_str(), 0);
				sBroRecv.cIsDept    = (UINT8)i32Type;


				if(0 == i32Type)
				{
					PS_EmpInfo_ psEmpInfo = objContacts.GetEmp(sBroRecv.dwRecverID);
					if(!psEmpInfo) continue;

					m_szRecver += psEmpInfo->pszCode;
					m_szRecver += _T("(");
					m_szRecver += objContacts.GetEmpName(psEmpInfo);
					m_szRecver += _T(");");
				}
				else if(1 == i32Type)
				{
					PS_DeptInfo_ psDeptInfo = objContacts.GetDeptItem(sBroRecv.dwRecverID, eGDIM_SELF);
					m_szRecver += objContacts.GetDeptName(psDeptInfo);
					m_szRecver += _T("(");
					m_szRecver += objContacts.GetDeptName(psDeptInfo);
					m_szRecver += _T(");");
				}
				else
				{
					continue;
				}

				m_sData.aRecver[m_sData.wRecverNum++] = sBroRecv;
			}
			else if(0 == strcmp(tszValue, FIELD_MSGUI))
			{
				pEl_MsgUI = pEl;
			}
		}

		// <MsgUI /> ==========================================================
		if(!pEl_MsgUI)
		{
			TRACE_(_T("Error BroMsg format[%s]"), pszXml);
			m_i32Result = EIMERR_BAD_CMD;
			return FALSE;
		}

		int i32Time = (int)eng.GetTimeNow();
		if (pEl_MsgUI->Attribute(FIELD_SEND_TIME) == NULL)			// Check and set SendTime
			pEl_MsgUI->SetAttribute(FIELD_SEND_TIME, i32Time);

		m_sData.dwTime = i32Time;

		PS_EmpInfo_ psInfo = eng.GetLoginEmp();
		if (pEl_MsgUI->Attribute(FIELD_SENDER_ID) == NULL)			// Check and set QEID
			pEl_MsgUI->SetAttribute(FIELD_SENDER_ID, psInfo->Id);

		if (pEl_MsgUI->Attribute( FIELD_SENDER_NAME ) == NULL)	// Check and Set SenderName
		{
			eIMStringA szNameA;
			eIMTChar2UTF8(objContacts.GetEmpName(psInfo), szNameA);
			pEl_MsgUI->SetAttribute(FIELD_SENDER_NAME, szNameA.c_str());
		}

		if (pEl_MsgUI->Attribute(FIELD_USER_CODE) == NULL)		// Check and Set SenderName
		{
			eIMStringA szCodeA;
			eIMTChar2UTF8(psInfo->pszCode, szCodeA);
			pEl_MsgUI->SetAttribute(FIELD_USER_CODE, szCodeA.c_str());
		}

		if (pEl_MsgUI->Attribute( FIELD_IS_SEND ) == NULL)		// Check and Set IsSend
			pEl_MsgUI->SetAttribute(FIELD_IS_SEND, 1);

		const char* pBroType = pEl_MsgUI->Attribute(FIELD_TYPE);
		if (pBroType == NULL)			// Check and Set SessionType
		{
			pEl_MsgUI->SetAttribute(FIELD_TYPE, BRO_NORMAL_TYPE);
			m_sData.cMsgType  = eBRO_BROCAST;
		}
		else
		{
			int i32Type = atoi(pBroType);
			if(BRO_SUPER_TYPE == i32Type)
				m_sData.cMsgType  = BRO_SUPER/*eBRO_SUPERBRO*/;
			else
				m_sData.cMsgType  = eBRO_BROCAST;
		}

		// <MsgUI><FILED_BRO_MSG /></MsgUI> =========================================
		TiXmlHandle h(pEl_MsgUI);
		TiXmlElement*	pMessageEl = h.FirstChildElement(FILED_BRO_MSG).ToElement();	
		if (pMessageEl == NULL)
		{
			TRACE_(_T("Error BroMsg format[%s]"), pszXml);
			m_i32Result = EIMERR_BAD_CMD;
			return FALSE;
		}

		m_sData.dwMsgLen = MSG_MAXBROADLEN;
		if (!Xml2Msg(pMessageEl, m_sData.aszMessage, m_sData.dwMsgLen, pIProt))
		{
			m_i32Result = EIMERR_BAD_CMD;
			return FALSE;
		}

		// <MsgUI><MsgFont /></MsgUI> =========================================
		if (h.FirstChildElement(FIELD_MSGUI).ToElement() == NULL)
		{	// Need add it
			eng.AddFontElement((E_SessionType)7, TRUE, pEl_MsgUI);
		}

		// Need convert to string because may be add some attribute
		ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
		ConvertToXml(*pEl_MsgUI, m_szMsgUI);

		return TRUE;
	}

	char* C_CmdSendBroadcast::Xml2Msg(TiXmlElement* pEl, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol)
	{
#define CHECK_BUF_SIZE_( NEED, TOTAL, RET ) \
	do{ \
	if ( (NEED) > (TOTAL) ) \
		{ \
		ASSERT_(FALSE); \
		TRACE_( _T("Message out of buffer[%d > %d]!"), (NEED), (TOTAL) ); \
		return (RET); \
	} \
	}while( 0 )

		int				i32Ret   = 0;
		int				i32Index = 0;
		const char*		pszValue = NULL;
		const char*		pszAttr  = NULL;
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };

		for ( pEl = pEl->FirstChildElement(); pEl; pEl = pEl->NextSiblingElement() )
		{	// Enumerate each child element
			pszValue = pEl->Value();
			ASSERT_( pszValue );
			CHECK_NULL_RET_(pszValue, NULL);

			if ( _stricmp(pszValue, FIELD_MSGFONT ) == 0 )
			{	// MsgFont
				DWORD	   dwColor   = strtol( pEl->Attribute(FIELD_COLOR), NULL, 0 );
				S_FONTINFO sFontInfo = { 0 };

				pszAttr = pEl->Attribute(FIELD_ID);
				sFontInfo.u8Id   = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_ID_DEFAULT;

				pszAttr = pEl->Attribute(FIELD_MODE);
				sFontInfo.u8Mode = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_MODE_DEFAULT;

				pszAttr = pEl->Attribute(FIELD_SIZE);
				sFontInfo.u8Size = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_SIZE_DEFAULT;

				sFontInfo.au8Color[0] = (BYTE)GetRValue(dwColor);	// R
				sFontInfo.au8Color[1] = (BYTE)GetGValue(dwColor);	// G
				sFontInfo.au8Color[2] = (BYTE)GetBValue(dwColor);	// B

				CHECK_BUF_SIZE_( i32Index + (int)sizeof(S_FONTINFO), u16Size, NULL );
				memcpy( &pszMsg[i32Index], &sFontInfo, sizeof(S_FONTINFO) );
				i32Index += sizeof(S_FONTINFO);
			}
			else if (_stricmp(pszValue, FILED_BRO_MSG_TEXT) == 0)
			{
				pszAttr = pEl->GetText();
				i32Ret  = strlen(pszAttr);

				ASSERT_(i32Ret);
				CHECK_BUF_SIZE_(i32Index + i32Ret, u16Size, NULL);
				memcpy( &pszMsg[i32Index], pszAttr, i32Ret);
				i32Index += i32Ret;
			}
			else
			{	// Error type of message...
				ASSERT_(FALSE);
			}
		}

		u16Size = (WORD)i32Index + 1;
		return pszMsg;
	}

	//- eCMD_SEND_BROADCAST_ACK
	//C_CmdSendBroadcast//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdSendBroadcastAck, eCMD_SEND_BROADCAST_ACK);
	int	C_CmdSendBroadcastAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);

		return m_i32Result;
	}

	int	C_CmdSendBroadcastAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdSendBroadcastAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		/*if (FAILED(m_i32Result))*/

		return m_i32Result;
	}

	//C_CmdBroadcastNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdBroadcastNotice, eCMD_BROADCAST_NOTICE);
	int C_CmdBroadcastNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	// in db??... 
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdBroadcastNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMContacts& objContacts  = C_eIMContacts::GetObject();
		S_BroInfo     sBroInfo = { 0 };

		sBroInfo.qMid			  = m_sData.dwMsgID;
		sBroInfo.i32timestamp	  = m_sData.dwSendTime;
		sBroInfo.psztitle		  = m_sData.aszTitile;
		sBroInfo.i32offmsg_flag = 0;
		sBroInfo.i32newmsg_flag = 0;
		sBroInfo.i32flag	      = 0;

		C_eIMBroMgr bmgr;
		bmgr.Init(pIDb);

		if(eBRO_BROCAST==m_sData.cAllReply)
		{
			eIMStringA szBroRecversA;
			PS_EmpInfo_ psInfo = objContacts.GetEmp(m_sData.dwRecverID);
			if(psInfo)
			{
				eIMString szRecv =  psInfo->pszCode;
				szRecv += _T("(");
				szRecv += objContacts.GetEmpName(psInfo);
				szRecv += _T(");");

				eIMTChar2UTF8(szRecv.c_str(), szBroRecversA);
			}
			sBroInfo.pszrecver = szBroRecversA.c_str();
			sBroInfo.i32type   = m_sData.cMsgType/*eBRO_BROCAST*/;

			ToXml(pIDb);
			bmgr.Set(&sBroInfo, m_szMsgUI.c_str());
		}
		else if(eBRO_ALERT == m_sData.cAllReply)
		{
			sBroInfo.i32type   = eBRO_ALERT;
			sBroInfo.u32SendID = m_sData.dwSenderID;
			ToAlertXml();
			bmgr.SetAlert(&sBroInfo, m_szMsgUI.c_str());
		}
		else if(eBRO_1To10T == m_sData.cAllReply)
		{
			const TCHAR* pszXml = To1To10TXml(pILog);
			if(pszXml)
			{
				Eng.Command(pszXml);
				EIMLOGGER_DEBUG_(pILog, _T("%s"), pszXml);
			}
			else
			{
				EIMLOGGER_ERROR_(pILog, _T("BrocastNotice:1 to 10T toxml failed!"));
			}
		}
		else if(eBRO_WEBAPP == m_sData.cAllReply)
		{
			// Not need dealwith anything here
		}
		else
		{
			EIMLOGGER_ERROR_(pILog, _T("Brocast:Receive type is invalid!"));
		}
		
		return m_i32Result;
	}

	int	C_CmdBroadcastNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		S_BroNotice sNotice = { 0 };

		sNotice.i32Time = m_sData.dwSendTime;
		sNotice.qMid = m_sData.dwMsgID;
		sNotice.qRecvEid = m_sData.dwRecverID;
		sNotice.qSendEid = m_sData.dwSenderID;

		if(eBRO_BROCAST == m_sData.cAllReply)
		{
			sNotice.u8Type   = m_sData.cMsgType;
		}
		else if(eBRO_1To10T == m_sData.cAllReply)
		{
			return m_i32Result;
		}
		else if(eBRO_WEBAPP == m_sData.cAllReply)
		{
			ASSERT_(m_sData.cMsgType == 0);
			//S_WebApps sWebApps = { eWEBAPP_NOTICE, 1, NULL };
			//sWebApps.psNotice  = (PS_WebAppNotice)m_sData.aszMessage;
			//SAFE_SEND_EVENT_(pIEMgr, EVENT_WEB_APP, (void*)&sWebApps);	// To Notify UI

			return m_i32Result;
		}
		else
			sNotice.u8Type   = m_sData.cAllReply;

		eIMString szMsg;
		eIMUTF8ToTChar(&m_sData.aszMessage[sizeof(S_FONTINFO)], szMsg);
		_tcscpy(sNotice.tszMsg, szMsg.c_str());

		eIMString szTitle;
		eIMUTF8ToTChar(m_sData.aszTitile, szTitle);
		_tcscpy(sNotice.tszTitle, szTitle.c_str());

		if(eBRO_BROCAST == m_sData.cAllReply)
			SAFE_SEND_EVENT_(pIEMgr, EVENT_BRO_NOTICE, &sNotice);
		else if(eBRO_ALERT == m_sData.cAllReply)
			SAFE_SEND_EVENT_(pIEMgr, EVENT_ALERT_NOTICE, &sNotice);
		else
			EIMLOGGER_ERROR_(pILog, _T("Brocast:Receive type is invalid!"));

		return m_i32Result;
	}

	const TCHAR* C_CmdBroadcastNotice::ToXml(I_SQLite3* pIDb)
	{
		GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		C_eIMEngine& eng = Eng;

		char		 szBuf[CONVERT_TO_STRING_SIZE];
		eIMString	 szValue;
		eIMStringA	 szValueA;
		TiXmlElement MsgEl(PROTOCOL_ELEMENT_CMD);
		TiXmlElement MsgUiEl(FIELD_MSGUI);

		// Set attribute
		MsgEl.SetAttribute(FIELD_ID,	 eCMD_BROADCAST_NOTICE);
		MsgEl.SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sData.dwMsgID, szBuf));

		PS_EmpInfo_ psSendInfo = pIConts->GetEmp(m_sData.dwSenderID);
		PS_EmpInfo_ psRecvInfo = pIConts->GetEmp(m_sData.dwRecverID);

		if(psSendInfo)
		{
			::eIMTChar2UTF8(pIConts->GetEmpName(m_sData.dwSenderID), szValueA);
			eIMStringA szCodeA;
			eIMTChar2UTF8(psSendInfo->pszCode, szCodeA);
			MsgUiEl.SetAttribute( FIELD_USER_CODE,	 szCodeA.c_str());
			MsgUiEl.SetAttribute( FIELD_SENDER_NAME, szValueA.c_str());
			MsgUiEl.SetAttribute( FIELD_SENDER_ID,	 (int)m_sData.dwSenderID );
		}
		else
		{
			MsgUiEl.SetAttribute( FIELD_USER_CODE,	 "");
			MsgUiEl.SetAttribute( FIELD_SENDER_NAME, "");
			MsgUiEl.SetAttribute( FIELD_SENDER_ID,	 0);
		}

		MsgUiEl.SetAttribute(FIELD_SEND_TIME,	(int)m_sData.dwSendTime );
		MsgUiEl.SetAttribute(FIELD_IS_SEND,	    0 );

		if(/*eBRO_SUPERBRO*/BRO_SUPER == m_sData.cMsgType)
			MsgUiEl.SetAttribute(FIELD_TYPE,		BRO_NORMAL_TYPE);
		else
			MsgUiEl.SetAttribute(FIELD_TYPE,		BRO_SUPER_TYPE);

		TiXmlElement e_(FILED_BRO_MSG);
		if (!Msg2Element(e_, eng) )
			return FALSE;

		if(/*eBRO_SUPERBRO*/BRO_SUPER == m_sData.cMsgType)
			eng.AddFontElement((E_SessionType)BRO_NORMAL_TYPE, FALSE, &MsgUiEl);
		else
			eng.AddFontElement((E_SessionType)BRO_SUPER_TYPE, FALSE, &MsgUiEl);
		MsgUiEl.InsertEndChild(e_);
		MsgEl.InsertEndChild(MsgUiEl);

		m_xmlDoc.Clear();
		m_xmlDoc.InsertEndChild(MsgEl);

		ConvertToXml( MsgEl, m_szXml );
		ConvertToXml( MsgUiEl, m_szMsgUI);

		return m_szXml.c_str();
	}

	const TCHAR* C_CmdBroadcastNotice::ToAlertXml()
	{
		GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		C_eIMEngine& eng = Eng;

		char		 szBuf[CONVERT_TO_STRING_SIZE];
		eIMString	 szValue;
		eIMStringA	 szValueA;
		TiXmlElement MsgEl(PROTOCOL_ELEMENT_CMD);
		TiXmlElement MsgUiEl(FIELD_MSGUI);

		// Set attribute
		MsgEl.SetAttribute(FIELD_ID,	 eCMD_BROADCAST_NOTICE);
		MsgEl.SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sData.dwMsgID, szBuf));

		MsgUiEl.SetAttribute(FIELD_SEND_TIME,	(int)m_sData.dwSendTime );
		MsgUiEl.SetAttribute(FIELD_IS_SEND,	    0 );
		MsgUiEl.SetAttribute(FIELD_TYPE,		ALERT_TYPE);

		TiXmlElement e_(FILED_ALERT_MSG);
		if (!Msg2Element(e_, eng) )
			return FALSE;

		eng.AddFontElement((E_SessionType)ALERT_TYPE, FALSE, &MsgUiEl);
		MsgUiEl.InsertEndChild(e_);
		MsgEl.InsertEndChild(MsgUiEl);

		m_xmlDoc.Clear();
		m_xmlDoc.InsertEndChild(MsgEl);

		ConvertToXml( MsgEl, m_szXml );
		ConvertToXml( MsgUiEl, m_szMsgUI);

		return m_szXml.c_str();
	}

	const TCHAR* C_CmdBroadcastNotice::To1To10TXml(I_EIMLogger* pILog)
	{
		C_eIMEngine& eng = Eng;

		char		 szBuf[CONVERT_TO_STRING_SIZE];
		eIMString	 szValue;
		eIMStringA	 szValueA;
		TiXmlElement MsgEl(PROTOCOL_ELEMENT_CMD);
		TiXmlElement MsgUiEl(FIELD_MSGUI);

		// Set attribute
		MsgEl.SetAttribute(FIELD_ID,	 eCMD_MSG_NOTICE);
		MsgEl.SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sData.dwMsgID, szBuf));
		MsgEl.SetAttribute(FIELD_RECVER_ID,		   m_sData.dwRecverID);
 		MsgEl.SetAttribute(FIELD_SENDER_ID,		   m_sData.dwSenderID);
		MsgEl.SetAttribute(FIELD_SEND_TIME,	       (int)m_sData.dwSendTime );
		MsgEl.SetAttribute(FIELD_IS_SEND,	       0);
		MsgEl.SetAttribute(FILED_IS_1To10T,	       1); //一呼万应
		MsgEl.SetAttribute(FIELD_MSG_TYPE,         m_sData.cMsgType);
		MsgEl.SetAttribute(FILE_1To10T_MSG_SRC_ID, GET_QSID_ASTR_(m_sData.dwSrcMsgID, szBuf));
		MsgEl.SetAttribute(FIELD_MSG_LEN,          m_sData.dwMsgLen);
		MsgEl.SetAttribute(FIELD_IS_GROUP,         0);

		TiXmlElement e_(FIELD_MESSAGE);
		if (!Msg2Element(e_, eng))
			return FALSE;

		if(m_sData.cMsgType != 0)
		{
			if(NULL==m_fFileMeta.aszFileName || 0==strlen(m_fFileMeta.aszFileName))
			{
				char tszName[64] = {0};
				strncpy(tszName, m_fFileMeta.aszURL, (COUNT_OF_ARRAY_(tszName)-5));  //减5预留.jpg+空格
				if(1 == m_sData.cMsgType)
					strcat(tszName, ".jpg");
				else if(2 == m_sData.cMsgType)
					strcat(tszName, ".amr");

				MsgEl.SetAttribute(FIELD_NAME, tszName);

				eIMString szKey;
				eIMUTF8ToTChar(m_fFileMeta.aszURL, szKey);
				EIMLOGGER_ERROR_(pILog, _T("One to 10 thound image name is null qmid=%I64u sendid=%u key=%s"), 
					m_sData.dwMsgID, m_sData.dwSenderID, szKey.c_str());
			}
			else
			{
				MsgEl.SetAttribute(FIELD_NAME, m_fFileMeta.aszFileName);
			}

			MsgEl.SetAttribute(FIELD_KEY,  m_fFileMeta.aszURL);

			//文件大小，此处不做网络字节序转换，以便MSGNOTICE中重新拷贝到aszMessage中
			TCHAR szNumBuf[64] = {0};
			Int64ToStr((INT64)m_fFileMeta.dwFileSize, szNumBuf, FALSE);
			eIMStringA szNum; eIMTChar2UTF8(szNumBuf, szNum);
			MsgEl.SetAttribute(FIELD_SIZE, szNum.c_str());
		}

		eng.AddFontElement((E_SessionType)BRO_NORMAL_TYPE, FALSE, &MsgUiEl);
		MsgUiEl.InsertEndChild(e_);
		MsgEl.InsertEndChild(MsgUiEl);

		m_xmlDoc.Clear();
		m_xmlDoc.InsertEndChild(MsgEl);

		ConvertToXml(MsgEl, m_szXml);
		ConvertToXml(MsgUiEl, m_szMsgUI);

		return m_szXml.c_str();
	}

	const BOOL C_CmdBroadcastNotice::Msg2Element(TiXmlElement& MsgElement, C_eIMEngine& objEgn)
	{
		GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);

		memset(&m_fFileMeta, 0, sizeof(FILE_META));
		char			szBuf[CONVERT_TO_STRING_SIZE] = {0};
		TiXmlElement    MsgFontElement( FIELD_MSGFONT );
		switch (m_sData.cAllReply)	// Brocast type
		{
		case eBRO_BROCAST:
			{
				MsgFontElement.SetAttribute(FIELD_ID,   0);
				MsgFontElement.SetAttribute(FIELD_SIZE, 11);
				MsgFontElement.SetAttribute(FIELD_MODE, 0);
				MsgFontElement.SetAttribute(FIELD_COLOR,0x00000000);
				MsgElement.InsertEndChild(MsgFontElement);	// Add font at first

				eIMStringA szContextA = &m_sData.aszMessage[sizeof(S_FONTINFO)];
				if (szContextA.empty())
					szContextA = " ";

				TiXmlElement TextElement(FILED_BRO_MSG_TEXT);
				TiXmlText    Text(szContextA.c_str());
				Text.SetCDATA(true);

				TextElement.InsertEndChild(Text);
				MsgElement.InsertEndChild(TextElement);
			}
			break;
		case eBRO_ALERT:
			{
				MsgFontElement.SetAttribute(FIELD_ID,   0);
				MsgFontElement.SetAttribute(FIELD_SIZE, 11);
				MsgFontElement.SetAttribute(FIELD_MODE, 0);
				MsgFontElement.SetAttribute(FIELD_COLOR,0x00000000);
				MsgElement.InsertEndChild(MsgFontElement);	// Add font at first

				eIMStringA szContextA = &m_sData.aszMessage[sizeof(S_FONTINFO)];
				if (szContextA.empty())
					szContextA = " ";

				TiXmlElement TextElement(FILED_ALERT_MSG_TEXT);
				TiXmlText    Text(szContextA.c_str());
				Text.SetCDATA(true);

				TextElement.InsertEndChild(Text);
				MsgElement.InsertEndChild(TextElement);
			}
			break;
		case eBRO_1To10T:
			{
				MsgFontElement.SetAttribute(FIELD_ID,   0);
				MsgFontElement.SetAttribute(FIELD_SIZE, 12);
				MsgFontElement.SetAttribute(FIELD_MODE, 0);
				MsgFontElement.SetAttribute(FIELD_COLOR,0x00000000);
				MsgElement.InsertEndChild(MsgFontElement);	// Add font at first

				eIMStringA szContextA;
				switch(m_sData.cMsgType)
				{
				case 0:
					szContextA = &m_sData.aszMessage[sizeof(S_FONTINFO)];
					break;
				case 1:
				case 2:
					{
						memcpy((void*)&m_fFileMeta, (const char *)m_sData.aszMessage, sizeof(FILE_META));
					}
					
					break;
				default:
					szContextA = m_sData.aszMessage;
				}

				if (szContextA.empty())
					szContextA = " ";

				TiXmlElement TextElement(FIELD_MSGTEXT);
				TiXmlText    Text(szContextA.c_str());
				Text.SetCDATA(true);

				TextElement.InsertEndChild(Text);
				MsgElement.InsertEndChild(TextElement);
			}
			break;
		default:
			break;
		}

		return TRUE;
	}

	//- eCMD_BROADCAST_NOTICE_ACK,
	//C_CmdMsgRead//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdMsgRead, eCMD_MSG_READ);
	int C_CmdMsgRead::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->SendReadSMS(m_sData.dwSenderID, m_sData.dwMsgID, m_sData.cMsgType, m_sData.dwTime);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdMsgRead::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort )
	{
        DWORD dwUserOffLine = Eng.GetPurview(GET_USER_OFFLINE_FLAG);
        C_eIMMsgReadErrorMgr cMsgReadError;
        cMsgReadError.Init(pIDb);
        if(0 < dwUserOffLine)
        {
            //用户已经离线
            cMsgReadError.Set(m_sData.dwMsgID, m_sData.dwSenderID);
        }
        else
        {
            cMsgReadError.Del(m_sData.dwMsgID);
        }
		return m_i32Result;
	}

	int	C_CmdMsgRead::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort )
	{
		return m_i32Result;
	}

	BOOL C_CmdMsgRead::FromXml( const TCHAR* pszXml )
	{
		GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
		C_eIMContacts& objContacts  = C_eIMContacts::GetObject();
		int				i32Value = 0;

		TiXmlElement*	pEl = ParseXmlCmdHeader(m_xmlDoc, pszXml, m_i32CmdId, m_i32Result);
		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();

		m_sData.dwMsgID = GroupId2Qsid(pEl->Attribute(FIELD_MSG_ID));
		ASSERT_(m_sData.dwMsgID);

		if( pEl->Attribute( FILED_MSG_READ_TYPE,  &i32Value ) == NULL )
		{
			ASSERT_(FALSE);
			return FALSE;
		}
		m_sData.cMsgType = (UINT8)i32Value;

		if( pEl->Attribute( FIELD_RECVER_ID,  &i32Value ) == NULL )
		{
			ASSERT_(FALSE);
			return FALSE;
		}
		m_sData.dwSenderID = i32Value;

		int i32Time = (int)Eng.GetTimeNow();
		if( pEl->Attribute( FIELD_SEND_TIME,  &i32Value ) == NULL )
		{
			pEl->SetAttribute(FIELD_SEND_TIME,i32Time);
			m_sData.dwTime = i32Time;
		}
		else
		{
			m_sData.dwTime = i32Value;
		}

		return TRUE;
	}

    //- eCMD_MSG_READ_ACK,			
	//C_CmdBroadcastNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdMsgReadNotice, eCMD_MSG_READ_NOTICE);
	int C_CmdMsgReadNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	// in db??... 
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int C_CmdMsgReadNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMMsgReadMgr MsgReadMgr;
		MsgReadMgr.Init(pIDb);
		//ToXml(pIDb);
        C_eIMMsgMgr		mmgr;
        mmgr.Init(pIDb);
        ZERO_STRUCT_(m_Msg);
        eIMStringA sDataA;
        BOOL bGet = mmgr.Get(m_sData.dwMsgID, m_Msg, sDataA);
        if (!bGet || Eng.isNeedUpdateGroup(m_sData.dwMsgID))
        {
            S_ReadMsgNotice sData;
            sData = m_sData;
            Eng.SaveReadMsgNotice(sData);
            EIMLOGGER_DEBUG_(pILog, _T("need update group msgid:%llu sederid:%u msgtype:%u\n"), m_sData.dwMsgID, m_sData.dwSenderID,m_sData.cMsgType);
        }
        else
        {
            if(MSG_RECEIPT == m_sData.cMsgType || MSG_1TO100 == m_sData.cMsgType)
            {
                EIMLOGGER_DEBUG_(pILog, _T("write readmsgdb msgid:%llu sederid:%u msgtype:%u\n"), m_sData.dwMsgID, m_sData.dwSenderID,m_sData.cMsgType);
                m_i32Result = MsgReadMgr.UpdateFlagAndReadTime(m_sData.dwMsgID,m_sData.dwSenderID, 
                    MSG_READ_FLAG_READ, m_sData.dwTime);
            }
        }
		return m_i32Result;
	}

	int	C_CmdMsgReadNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
        EIMLOGGER_DEBUG_(pILog, _T("enter MsgReadNotice::DoCmd msgtype:%u\n"), m_sData.cMsgType);
		if ( SUCCEEDED( m_i32Result ) )	// Except the Constraint or error
		{
			if(MSG_RECEIPT == m_sData.cMsgType)
			{
				S_1To100Read sMsgRead = { 0 };
				sMsgRead.bIsSend    = true;  //用于更新“发送一呼百应”按钮文本更新
				sMsgRead.qeidSender = m_sData.dwSenderID;
				sMsgRead.qmid       = m_sData.dwMsgID;
				sMsgRead.qsid       = m_Msg.qsid;
				SAFE_SEND_EVENT_(pIEMgr, EVENT_RECEIPT_NOTICE, (void*)&sMsgRead);

				//SAFE_SEND_EVENT_(pIEMgr, EVENT_RECV_MSG, (void*)this);
			}
			else
			{
				S_1To100Read sMsgRead = { 0 };
				sMsgRead.bIsSend    = true;  //用于更新“发送一呼百应”按钮文本更新
				sMsgRead.qeidSender = m_sData.dwSenderID;
				sMsgRead.qmid       = m_sData.dwMsgID;
				sMsgRead.qsid       = m_Msg.qsid;
				SAFE_SEND_EVENT_(pIEMgr, EVENT_1TO100_READ_NOTICE, (void*)&sMsgRead);
			}
		}

		return m_i32Result;
	}

	const TCHAR* C_CmdMsgReadNotice::ToXml(I_SQLite3* pIDb)
	{
		C_eIMEngine& eng = Eng;
		C_eIMMsgReadMgr MsgReadMgr;
		C_eIMMsgMgr		mmgr;
		mmgr.Init(pIDb);
		MsgReadMgr.Init(pIDb);

		// 获取数据库中的消息XML
		ZERO_STRUCT_(m_Msg);
		eIMStringA sDataA;
		BOOL bGet = mmgr.Get(m_sData.dwMsgID, m_Msg, sDataA);
		CHECK_NULL_RET_(bGet,NULL);

		//此处返回为一呼百应通知，为获取m_Msg信息
		/*if(MSG_1TO100 == m_sData.cMsgType)*/
		return _T("");

		//ZERO_STRUCT_(m_sCreateMsg);
		//m_sCreateMsg.qsid = m_Msg.qsid;
		//m_sCreateMsg.eType = eSESSION_TYPE_SINGLE;
		//m_sCreateMsg.qmid = Eng.NewID();
		//m_sCreateMsg.dwTime = Eng.GetTimeNow();
		//m_sCreateMsg.qeid = m_Msg.qeid;
		//m_sCreateMsg.dwFlag = DB_FLAG_MSG_NEW;
		//
		//// 获取字体
		//eIMString  szMsgFont;
		//szMsgFont = objEgn.GetAttributeStr(MSGUI_FONT_ERROR, _T(MSGUI_FONT_ERROR_DEFAULT));
		//eIMStringA szMsgFontA;
		//::eIMTChar2UTF8(szMsgFont.c_str(), szMsgFontA);
		//TiXmlDocument xmlfont;
		//xmlfont.Parse(szMsgFontA.c_str());

		//// 消息XML解析
		//TiXmlDocument xml;
		//xml.Parse(sDataA.c_str());
		//TiXmlElement* xmlMsgUI = xml.RootElement();
		//CHECK_NULL_RET_(xmlMsgUI, NULL);

		//// 标题清除旧字体
		//TiXmlElement* pFontEL = xmlMsgUI->FirstChildElement(FIELD_MSGFONT);
		//if(pFontEL)
		//	xmlMsgUI->RemoveChild(pFontEL);
		//// 加上新字体
		//TiXmlElement* pFistChildEl = xmlMsgUI->FirstChildElement();
		//if ( pFistChildEl )
		//	xmlMsgUI->InsertBeforeChild(pFistChildEl, *xmlfont.RootElement());
		//else
		//	xmlMsgUI->InsertEndChild(*xmlfont.RootElement());

		//int i32ReadNum = 0;
		//int i32AllNum = 0;

		//i32ReadNum = MsgReadMgr.GetHasMsgReadCount( m_sData.dwMsgID );
		//i32AllNum = MsgReadMgr.GetMsgReadCount( m_sData.dwMsgID );

		//TCHAR szFront[71] = {0};
		//PS_EmpInfo_ psEmp = C_eIMContacts::GetObject().GetEmp(m_sData.dwSenderID);
		//if(psEmp)
		//	_sntprintf(szFront, 70 , ::eIMGetErrorLangInfo(EIMERR_RECEIPT1, _T("Receipt info %s(%s) readed.(%d/%d):")), psEmp->pszCode, C_eIMContacts::GetObject().GetEmpName(psEmp),i32ReadNum,i32AllNum);
		//else
		//	_sntprintf(szFront, 70 , ::eIMGetErrorLangInfo(EIMERR_RECEIPT2, _T("Receipt info (%u) readed.(%d/%d):")), m_sData.dwSenderID,i32ReadNum,i32AllNum);

		//TiXmlElement* pMsgUIFistChild = xmlMsgUI->FirstChildElement();
		//eIMStringA szMsgFrontA;
		//::eIMTChar2UTF8(szFront, szMsgFrontA);
		//TiXmlDocument xmlFront;
		//xmlFront.Parse(szMsgFrontA.c_str());
		////xmlMsgUI->InsertBeforeChild(pMsgUIFistChild,*xmlFront.RootElement());

		//TiXmlElement MsgEl( PROTOCOL_ELEMENT_CMD);
		//char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		//MsgEl.SetAttribute( FIELD_ID,		 eCMD_MSG_NOTICE);
		//MsgEl.SetAttribute( FIELD_RECVER_ID, (int)m_sData.dwRecverID );
		//MsgEl.SetAttribute( FIELD_MSG_ID,    GET_QSID_ASTR_(m_sCreateMsg.qmid, szBuf) );
		//MsgEl.SetAttribute( FIELD_IS_GROUP,  0	);
		//MsgEl.SetAttribute( FIELD_GROUP_ID, GET_QSID_ASTR_(m_Msg.qsid, szBuf) );
		//MsgEl.InsertEndChild(*xmlMsgUI);

		//ConvertToXml( MsgEl , m_szXml );
		//ConvertToXml( *xmlMsgUI, m_szXmlUI );

		//return m_szXml.c_str();
	}

	//- eCMD_GET_USERS_SIMPLE_LIST,			
    //- eCMD_GET_USERS_SIMPLE_LIST_ACK,		
	//C_CmdMsgNoticeConfirm//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdMsgNoticeConfirm, eCMD_MSG_NOTICE_CONFIRM);
	int C_CmdMsgNoticeConfirm::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	// //P2P 发送32字节码应答确认通知
		CMD_TRACE_STR_(m_szXml.substr(0, 1024), m_i32Result);
		return m_i32Result;
	}

	int	C_CmdMsgNoticeConfirm::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
	//	SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		//SAFE_SEND_EVENT_(pIEMgr, EVENT_P2P_NOTICE_CONFIRM, (void*)m_szXml.c_str());
		return m_i32Result;
	}

	const TCHAR* C_CmdMsgNoticeConfirm::ToXml()
	{
		C_eIMSessionMgr smgr;
		TiXmlElement  CmdEl("Cmd");
		TiXmlDocument xmlDoc;
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };

		xmlDoc.Parse(m_sData.aszMessage);
		if ( xmlDoc.Error() )
		{
			m_i32Result = EIMERR_BAD_CMD;
			return NULL;
		}

		S_SessionInfo sInfo = { 0 };
		smgr.Get(m_sData.dwSenderID, m_sData.dwRecverID, &sInfo, 0);

		CmdEl.SetAttribute(FIELD_ID,		eCMD_MSG_NOTICE_CONFIRM);
		CmdEl.SetAttribute(FIELD_RECVER_ID, m_sData.dwRecverID);
		CmdEl.SetAttribute(FIELD_SENDER_ID, m_sData.dwSenderID);
		CmdEl.SetAttribute(FIELD_MSG_ID,	GET_QSID_ASTR_(m_sData.dwMsgID, szBuf) );
		CmdEl.SetAttribute(FIELD_GROUP_ID,  GET_QSID_ASTR_(sInfo.qsid, szBuf));

		CmdEl.InsertEndChild(*xmlDoc.RootElement());
		ConvertToXml(CmdEl, m_szXml);
		return m_szXml.c_str();
	}

	//C_CmdVirGroupUpdateReq//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdVirGroupUpdateReq, eCMD_REGULAR_GROUP_UPDATE_REQ);
	int C_CmdVirGroupUpdateReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetVirGroupInfo(m_sData.dwRegularTime);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdVirGroupUpdateRsp//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdVirGroupUpdateRsp, eCMD_REGULAR_GROUP_UPDATE_RSP);
	int C_CmdVirGroupUpdateRsp::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CMD_TRACE_(m_i32Result);
		Eng.SetAttributeInt(IME_ATTRIB_OFF_GROUP_COUNT, m_sData.wGroupNum);
		EIMLOGGER_DEBUG_( pILog, _T("group count=%d"), m_sData.wGroupNum );
		return m_i32Result;
	}
	
	//C_CmdCheckTimeReq////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCheckTimeReq, eCMD_CHECK_TIME_REQ);
	int C_CmdCheckTimeReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->CheckTime(m_sData);
		if (FAILED(m_i32Result)) 
			EIMLOGGER_ERROR_( pILog, _T("Serial:%u, Failed, result: 0x%08X, desc:%s"), m_sData, m_i32Result, GetErrorDesc()); 
		else 
			EIMLOGGER_DEBUG_( pILog, _T("Serial:%u, Succeeded"), m_sData);


		return m_i32Result;
	}

	//C_CmdCheckTimeAck////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCheckTimeRsp, eCMD_CHECK_TIME_RSP);
	int C_CmdCheckTimeRsp::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		EIMLOGGER_DEBUG_( pILog, _T("Serial: %u, TimeNow: %u"), m_sData.dwSerial, m_sData.timeNow );
		return m_i32Result;
	}

	int	C_CmdCheckTimeRsp::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdGetOfflineMsgReq//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetOfflineMsgReq, eCMD_GET_OFFLINE_REQ);
	int C_CmdGetOfflineMsgReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetOffline(TERMINAL_PC);
		CMD_TRACE_(m_i32Result);
	//	s_u32OffMsgCount = 0;
		return m_i32Result;
	}

	//C_CmdGetOfflineMsgRsp//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetOfflineMsgRsp, eCMD_GET_OFFLINE_RSP);
	int C_CmdGetOfflineMsgRsp::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		EIMLOGGER_DEBUG_( pILog, _T("OfflineMsgCount:%d"), m_sData.dwOfflineMsgCount);
		return m_i32Result;
	}

	int	C_CmdGetOfflineMsgRsp::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
	//	SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		if(m_sData.dwOfflineMsgCount == 0)
			SAFE_SEND_EVENT_(pIEMgr, EVENT_RECV_OFFLINE_MSG , NULL);

		return m_i32Result;
	}

	//- eCMD_REFUSE_GROUP_MSG_REQ,	
    //- eCMD_REFUSE_GROUP_MSG_RSP,	
	//C_CmdQuitGroup////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdQuitGroup, eCMD_QUIT_GROUP);
	int C_CmdQuitGroup::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
		m_i32Result = pICa->QuitGroup(GET_QSID_ASTR_(m_sData, szBuf));
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int	C_CmdQuitGroup::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		CHECK_FAILED_SEND_EVENT_RET_(EVENT_QUIT_GROUP, (void*)this);
		return EIMERR_NO_ERROR;
	}

	BOOL C_CmdQuitGroup::FromXml( const TCHAR* pszXml )
	{
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();

		m_sData = GroupId2Qsid(pEl->Attribute(FIELD_GROUP_ID));
		ASSERT_(m_sData);

		m_szXml = pszXml;
		return TRUE;
	}


	//C_CmdQuitGroupAck////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdQuitGroupAck, eCMD_QUIT_GROUP_ACK);
	int C_CmdQuitGroupAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int C_CmdQuitGroupAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result))
		{
			C_eIMSessionMgr smgr;

			smgr.Init(pIDb);
			smgr.UpdateFlag(GroupId2Qsid(m_sData.aszGroupID), eSEFLAG_DELETE, TRUE);
		}

		return EIMERR_NO_ERROR;
	}

	int	C_CmdQuitGroupAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_QUIT_GROUP_ACK, (void*)&qsid);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdQuitGroupAck::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		m_i32Result = pICa->GetErrorCode((RESULT)m_sData.nReturn);
		if ( FAILED(m_i32Result) )
		{
			TiXmlElement El(PROTOCOL_ELEMENT_CMD);

			El.SetAttribute(FIELD_ID,		  m_i32CmdId);
			SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);

			ConvertToXml(El, m_szXml);
			return m_szXml.c_str();
		}

		return NULL;
	}
	
	//C_CmdQuitGroupNotice////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdQuitGroupNotice, eCMD_QUIT_GROUPNOTICE);
	int C_CmdQuitGroupNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}

	int C_CmdQuitGroupNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMSessionMgr smgr;

		smgr.Init(pIDb);
		QSID qsid   = GroupId2Qsid(m_sData.aszGroupID);
		m_i32Result = smgr.Set(qsid, eSET_TYPE_SUTIME, m_sData.dwTime);
		m_i32Result = smgr.Set(qsid, (PQEID)&m_sData.dwUserID, 1, FALSE, FALSE);

		return EIMERR_NO_ERROR;
	}

	int	C_CmdQuitGroupNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, EVENT_QUIT_GROUP_NOTICE, (void*)this);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdQuitGroupNotice::ToXml()
	{
		m_i32Result = EIMERR_NO_ERROR;
		TiXmlElement El(PROTOCOL_ELEMENT_CMD);

		El.SetAttribute(FIELD_ID,		  m_i32CmdId);
		SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
		El.SetAttribute(FIELD_USER_ID,	  m_sData.dwUserID);
		El.SetAttribute(FIELD_TIMESTAMP,  m_sData.dwTime);

		ConvertToXml(El, m_szXml);
		return m_szXml.c_str();
	}


	//C_CmdModifySelfInfo////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifySelfInfo, eCMD_MODIFY_SELF_INFO);
	int C_CmdModifySelfInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->ModiSelfNotice(&m_sData);
		if (FAILED(m_i32Result))
			EIMLOGGER_ERROR_( pILog, _T("Modify type[%d] failed, result: 0x%08X, desc:%s"), m_sData.cModiType, m_i32Result, GetErrorDesc());
		else
			EIMLOGGER_INFO_( pILog, _T("Modify type[%d] succeeded"), m_sData.cModiType);

		return m_i32Result;
	}

	//C_CmdModifySelfInfoAck////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifySelfInfoAck, eCMD_MODIFY_SELF_INFO_ACK);
	int C_CmdModifySelfInfoAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode((RESULT)m_sData.nReturn);
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdModifySelfInfoAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdModifySelfInfoNotice////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifySelfInfoNotice, eCMD_MODIFY_SELF_INFO_NOTICE);
	int C_CmdModifySelfInfoNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdModifySelfInfoNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMContactsMgr cmgr;
		cmgr.Init(pIDb);
		m_i32Result = cmgr.Set(&m_sData);
		return m_i32Result;
	}

	int	C_CmdModifySelfInfoNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}
	
	//C_CmdCreateSchedule////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCreateSchedule, eCMD_CREATE_SCHEDULE);
	int C_CmdCreateSchedule::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->CreateSchedule(&m_sData);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdCreateSchedule::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		//...
		return m_i32Result;
	}

	//C_CmdCreateScheduleAck////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCreateScheduleAck, eCMD_CREATE_SCHEDULE_ACK);
	int C_CmdCreateScheduleAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdCreateScheduleAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdCreateScheduleNotice////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCreateScheduleNotice, eCMD_CREATE_SCHEDULE_NOTICE);
	int C_CmdCreateScheduleNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
		
	int	C_CmdCreateScheduleNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		//...
		return m_i32Result;
	}

	int	C_CmdCreateScheduleNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

    //- eCMD_CREATE_SCHEDULE_NOTICE_ACK,	// ( 105 ) Alias="创建日程提醒通知应答" 
	//C_CmdGetUserListType//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetDataListType, eCMD_GET_DATA_LIST_TYPE);
	int C_CmdGetDataListType::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_sData.nTermType = TERMINAL_PC;
		m_sData.nNetType  = Eng.GetAttributeInt( IME_ATTRIB_ACCESS_MODE, CMNET ) == CMNET ? CMNET : CMWAP;
		m_i32Result = pICa->GetDataListType(&m_sData);
		if ( FAILED(m_i32Result) )
		{
			EIMLOGGER_ERROR_(pILog, _T("UpdataTypeDept:%u, UpdataTypeDeptUser:%u, UpdataTypeUser:%u, LastUpdateTimeDept:%u, LastUpdateTimeDeptuser:%u, LastUpdateTimeUser:%u"),
				m_sData.cUpdataTypeDept,      m_sData.cUpdataTypeDeptUser,	    m_sData.cUpdataTypeUser,
				m_sData.dwLastUpdateTimeDept, m_sData.dwLastUpdateTimeDeptUser, m_sData.dwLastUpdateTimeUser);
		}
		else
		{
			EIMLOGGER_DEBUG_(pILog, _T("UpdataTypeDept:%u, UpdataTypeDeptUser:%u, UpdataTypeUser:%u, LastUpdateTimeDept:%u, LastUpdateTimeDeptuser:%u, LastUpdateTimeUser:%u"),
				m_sData.cUpdataTypeDept,      m_sData.cUpdataTypeDeptUser,	    m_sData.cUpdataTypeUser,
				m_sData.dwLastUpdateTimeDept, m_sData.dwLastUpdateTimeDeptUser, m_sData.dwLastUpdateTimeUser);
		}

		SetBusinessTypeId(m_i32CmdId);
		C_GetDataListTypeWaitOverItem* pBaseWaitItem = new C_GetDataListTypeWaitOverItem();
		pBaseWaitItem->SetData(this);
		pBaseWaitItem->SetStartTime(GetTickCount());
		C_UIWaitOverMgr::GetObject().AddWaitOverItem(pBaseWaitItem);

		return m_i32Result;
	}

	//C_CmdGetUserListTypeAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetDataListTypeAck, eCMD_GET_DATA_LIST_TYPE_ACK);
	int C_CmdGetDataListTypeAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		if ( FAILED(m_i32Result) )
		{
			EIMLOGGER_ERROR_(pILog, _T("UpdataTypeDept:%u, UpdataTypeDeptUser:%u, UpdataTypeUser:%u, LastUpdateTimeDept:%u, LastUpdateTimeDeptuser:%u, LastUpdateTimeUser:%u, cDownLoadTypeDept:%u, cDownLoadTypeDeptUser:%u, cDownLoadTypeUser:%u, DownLoadPathDept:%S, DownLoadPathDeptUser:%S, DownLoadPathUser:%S"),
				m_sData.cUpdataTypeDept,      m_sData.cUpdataTypeDeptUser,	    m_sData.cUpdataTypeUser,
				m_sData.dwLastUpdateTimeDept, m_sData.dwLastUpdateTimeDeptuser, m_sData.dwLastUpdateTimeUser,
				m_sData.cDownLoadTypeDept,	  m_sData.cDownLoadTypeDeptUser,	m_sData.cDownLoadTypeUser,
				m_sData.strDownLoadPathDept,  m_sData.strDownLoadPathDeptUser,  m_sData.strDownLoadPathUser);
		}
		else
		{
			C_UIWaitOverMgr::GetObject().DelWaitOverItem(eWaitType_GetDataListType, eCMD_GET_DATA_LIST_TYPE);	// Del when succeed;  the failed where troggle timeout

			EIMLOGGER_DEBUG_(pILog, _T("UpdataTypeDept:%u, UpdataTypeDeptUser:%u, UpdataTypeUser:%u, LastUpdateTimeDept:%u, LastUpdateTimeDeptuser:%u, LastUpdateTimeUser:%u,  cDownLoadTypeDept:%u, cDownLoadTypeDeptUser:%u, cDownLoadTypeUser:%u, DownLoadPathDept:%S, DownLoadPathDeptUser:%S, DownLoadPathUser:%S"),
				m_sData.cUpdataTypeDept,      m_sData.cUpdataTypeDeptUser,	    m_sData.cUpdataTypeUser,
				m_sData.dwLastUpdateTimeDept, m_sData.dwLastUpdateTimeDeptuser, m_sData.dwLastUpdateTimeUser,
				m_sData.cDownLoadTypeDept,	  m_sData.cDownLoadTypeDeptUser,	m_sData.cDownLoadTypeUser,
				m_sData.strDownLoadPathDept,  m_sData.strDownLoadPathDeptUser,  m_sData.strDownLoadPathUser);
		}

		return m_i32Result;
	}

	int	C_CmdGetDataListTypeAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		if ( FAILED(m_i32Result) )
			return m_i32Result;

		Eng.DownloadContactFiles(&m_sData, pIDb);
		return m_i32Result;
	}

	int	C_CmdGetDataListTypeAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdGetUserListTypeAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdCoLastTimeNotice, eCMD_COMP_LAST_TIME_NOTICE);
	int C_CmdCoLastTimeNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		EIMLOGGER_DEBUG_(pILog, _T("cid:%u, TTDept:%u, TTDeptEmps:%u, TTEmps:%u, TTVgts:%u"),
			m_sData.dwCompID, m_sData.dwDeptUpdateTime, m_sData.dwDeptUserUpdateTime, m_sData.dwUserUpdateTime, m_sData.vgts);

		return m_i32Result;
	}

	int	C_CmdCoLastTimeNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdDelSchedule////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdDelSchedule, eCMD_DELETE_SCHDULE);
	int C_CmdDelSchedule::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->DeleteSchedule(&m_sData);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdDelSchedule::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		//...
		return m_i32Result;
	}

	//C_CmdDelScheduleAck////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdDelScheduleAck, eCMD_DELETE_SCHDULE_ACK);
	int C_CmdDelScheduleAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdDelScheduleAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdDelScheduleNotice////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdDelScheduleNotice, eCMD_DELETE_SCHDULE_NOTICE);
	int C_CmdDelScheduleNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
		
	int	C_CmdDelScheduleNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		//...
		return m_i32Result;
	}

	int	C_CmdDelScheduleNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_( pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}
	
	//- eCMD_DELETE_SCHDULE_NOTICE_ACK,		// ( 112 ) 删除日程提醒通知应答 
    //- eCMD_GROUP_PUSH_FLAG,				// ( 113 ) ios群组消息推送 
    //- eCMD_GROUP_PUSH_FLAG_ACK,			// ( 114 ) ios群组消息推送修改应答 
    //- eCMD_IOS_BACKGROUND_REQ,			// ( 115 ) IOS转入后台请求 
    //- eCMD_IOS_BACKGROUND_ACK,			// ( 116 ) IOS转入后台应答
    //- eCMD_GET_USER_RANK_REQ,				// ( 117 ) 获取级别(员工所属)请求 
    //- eCMD_GET_USER_RANK_ACK,				// ( 118 ) 获取级别(员工所属)应答 
    //- eCMD_GET_USER_PROFE_REQ,			// ( 119 ) 获取业务(员工所属)请求 
    //- eCMD_GET_USER_PROFE_ACK,			// ( 120 ) 获取业务(员工所属)应答 
    //- eCMD_GET_USER_AREA_REQ,				// ( 121 ) 获取地域(员工所属)请求 
    //- eCMD_GET_USER_AREA_ACK,				// ( 122 ) 获取地域(员工所属)应答 
	//C_CmdGetSpecialList//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetSpecialList, eCMD_GET_SPECIAL_LIST);
	int C_CmdGetSpecialList::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetSpecialList(&m_sData);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//C_CmdGetSpecialListAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdGetSpecialListAck, eCMD_GET_SPECIAL_LIST_ACK);
	int C_CmdGetSpecialListAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.result);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdGetSpecialListAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdGetSpecialListAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);

		if (FAILED(m_i32Result))
			SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_SPECIAL_LIST_END, (void*)m_i32Result);
		return m_i32Result;
	}

	//C_CmdModifySpecialListNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifySpecialListNotice, eCMD_MODI_SPECIAL_LIST_NOTICE);
	int C_CmdModifySpecialListNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
	//	CONVERT_TO_ICA_RET_(pvCa);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	//eCMD_MODI_SPECIAL_LIST_NOTICE_ACK//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifySpecialListNoticeAck, eCMD_MODI_SPECIAL_LIST_NOTICE_ACK);
	int C_CmdModifySpecialListNoticeAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdModifySpecialListNoticeAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdModifySpecialListNoticeAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_SPECIAL_LIST_END, (void*)m_i32Result);
		return m_i32Result;
	}
	// =============================固定群组===============================
	// 创建群组通知新协议
	REGISTER_CMDCLASS(C_CmdCreateRegularGroupNoticeV2, eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE);
	int	C_CmdCreateRegularGroupNoticeV2::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		m_bHasGetAll = FALSE;
		return m_i32Result;
	}
	int	C_CmdCreateRegularGroupNoticeV2::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		m_bHasGetAll = FALSE;
		eIMString		szValue;
		S_SessionInfo	sSInfo = { 0 };
		C_eIMSessionMgr smgr;

		smgr.Init(pIDb);
		sSInfo.dwCreateTime = m_sData.dwTime;
		sSInfo.dwSUpdateTime= m_sData.dwTime;
		sSInfo.eFlag		= (E_SEFlag)(eSEFLAG_VGROUP);	
		sSInfo.eType		= eSESSION_TYPE_RGROUP;
		sSInfo.qsid			= GroupId2Qsid(m_sData.aszGroupID);
		sSInfo.CreaterId	= m_sData.dwCreaterID;
		sSInfo.dwUpdateTime = m_sData.dwTime;
		SET_UTF8_2TVALUE_(m_sData.aszGroupName, sSInfo.szTitle);

		smgr.Set(&sSInfo);
		C_eIMEngine& eng = Eng;
		C_eIMSaveData* pSaveData = eng.GetEIMSaveDataPtr();
		CHECK_NULL_RET_(pSaveData, 0);
		int iPageCount = pSaveData->GetGroupIDUpdatingPage(sSInfo.qsid);
		if(iPageCount == 0)
		{
			m_i32Result = smgr.Set(sSInfo.qsid, m_sData.aUserList, m_sData.wCurrentNum, TRUE, TRUE);
		}
		else
		{
			m_i32Result = smgr.Set(sSInfo.qsid, m_sData.aUserList, m_sData.wCurrentNum, TRUE, FALSE);
		}
		pSaveData->AddGroupIDUpdatingPage(sSInfo.qsid);
		EIMLOGGER_DATA_(pILog, (BYTE*)m_sData.aUserList, sizeof(regulargroup_member) * m_sData.wCurrentNum, _T("Regular group:  Creater:%d, Qsid:%llu, Title:%s, CreateTime:%d, Num:%d Page:%d PageAll:%d"),
			sSInfo.CreaterId, sSInfo.qsid, sSInfo.szTitle, sSInfo.dwCreateTime, 
			m_sData.wCurrentNum, m_sData.wMemberPage, m_sData.wMemberTotalPage);
		if(iPageCount == m_sData.wMemberTotalPage - 1)
		{
			EIMLOGGER_DEBUG_(pILog, _T("Regular group Data OK! Qsid:%llu, Page:%d PageAll:%d  Num:%d "),
				sSInfo.qsid, m_sData.wMemberPage, m_sData.wMemberTotalPage, m_sData.wCurrentNum);
			pSaveData->DelGroupIDUpdating(sSInfo.qsid);
			m_bHasGetAll = TRUE;
		}

		return m_i32Result;
	}
	int	C_CmdCreateRegularGroupNoticeV2::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		if(m_bHasGetAll)
		{
			QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
			SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
			SAFE_SEND_EVENT_(pIEMgr,EVENT_GET_VGROUP_INFO_ACK,&qsid);
		}
		return m_i32Result;
	}

	// 创建群组通知
	/*REGISTER_CMDCLASS(C_CmdCreateRegularGroupNotice, eCMD_CREATEREGULARGROUPNOTICE);
	int	C_CmdCreateRegularGroupNotice::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
	int	C_CmdCreateRegularGroupNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		S_SessionInfo	sSInfo = { 0 };
		C_eIMSessionMgr smgr;

		smgr.Init(pIDb);
		sSInfo.dwCreateTime = m_sData.dwTime;
		sSInfo.eFlag		= (E_SEFlag)(eSEFLAG_VGROUP);	
		sSInfo.eType		= eSESSION_TYPE_RGROUP;
		sSInfo.qsid			= GroupId2Qsid(m_sData.aszGroupID);
		sSInfo.CreaterId	= m_sData.dwCreaterID;
		sSInfo.dwUpdateTime = m_sData.dwTime;
		SET_UTF8_2TVALUE_(m_sData.aszGroupName, sSInfo.szTitle);

		smgr.Set(&sSInfo);
		m_i32Result = mgr.Set(sSInfo.qsid, m_sData.aUserList, m_sData.wUserNum, TRUE, TRUE);
		EIMLOGGER_DEBUG_(pILog, _T("Creater:%d, Qsid:%llu, Title:%s, CreateTime:%d, Num:%d"),
			sSInfo.CreaterId, sSInfo.qsid, sSInfo.szTitle, sSInfo.dwCreateTime, m_sData.wUserNum);

		return m_i32Result;
	}
	int	C_CmdCreateRegularGroupNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}*/

	// 删除群组通知
	REGISTER_CMDCLASS(C_CmdDeleteRegularGroupNotice, eCMD_DELETEREGULARGROUPNOTICE);
	int	C_CmdDeleteRegularGroupNotice::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
	int	C_CmdDeleteRegularGroupNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMSessionMgr smgr;
		smgr.Init(pIDb);
		QSID qsid			= GroupId2Qsid(m_sData.aszGroupID);
		E_SetType rType = (E_SetType)eSET_TYPE_FLAG;
		E_SEFlag eFlag = (E_SEFlag)(eSEFLAG_DELETE);
		smgr.Set(qsid, rType, eFlag);
		EIMLOGGER_DEBUG_(pILog, _T("Delete user ID:%d, Qsid:%llu, DeleteTime:%d"), m_sData.dwDeleteID, qsid, m_sData.dwTime);
		C_eIMSaveData* pSaveData = Eng.GetEIMSaveDataPtr();
		CHECK_NULL_RET_(pSaveData, 0);
		pSaveData->DelGroupIDUpdating(qsid);
		return m_i32Result;
	}
	int	C_CmdDeleteRegularGroupNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		SAFE_SEND_EVENT_(pIEMgr, eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE, &m_sData);//表示已经同步此固定群组
		return m_i32Result;
	}

	// 修改群组成员
	REGISTER_CMDCLASS(C_CmdRegularGroupMemberChangeNotice, eCMD_GULARGROUPMEMBERCHANGENOTICE);
	int	C_CmdRegularGroupMemberChangeNotice::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s, result:0x%08x, desc:%s"), m_szXml.c_str(), m_i32Result, GetErrorDesc());

		return m_i32Result;
	}
	int	C_CmdRegularGroupMemberChangeNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMSessionMgr smgr;

		smgr.Init(pIDb);
		m_i32Result = smgr.Set(GroupId2Qsid(m_sData.aszGroupID), eSET_TYPE_SUTIME, m_sData.dwTime);
		m_i32Result = smgr.Set(GroupId2Qsid(m_sData.aszGroupID), (PQEID)m_sData.aUserList, m_sData.wUserNum, m_sData.cOperType == 0, FALSE);

		return m_i32Result;
	}
	int	C_CmdRegularGroupMemberChangeNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		SAFE_SEND_EVENT_(pIEMgr,EVENT_MODIFY_VGROUP_MEMBER_NOTICE,(void*)this);
		return m_i32Result;
	}
	const TCHAR* C_CmdRegularGroupMemberChangeNotice::ToXml()
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
		m_i32Result = EIMERR_NO_ERROR;
		TiXmlElement El(PROTOCOL_ELEMENT_CMD);
		TiXmlElement ItemEl(FIELD_ITEM);

		El.SetAttribute(FIELD_ID,		  m_i32CmdId);
		SET_GROUP_ID_ATTR_(m_sData.aszGroupID, El);
		El.SetAttribute(FIELD_TIMESTAMP,  m_sData.dwTime );
		ItemEl.SetAttribute(FIELD_TYPE,	  m_sData.cOperType);
		El.SetAttribute(FIELD_MODIFY_ID,  m_sData.dwModifyID );

		for (int i32Index = 0; i32Index < m_sData.wUserNum; i32Index++)
		{
			ItemEl.SetAttribute(FIELD_USER_ID, m_sData.aUserList[i32Index].dwUserID);
			El.InsertEndChild(ItemEl);
		}

		ConvertToXml(El, m_szXml);
		return m_szXml.c_str();

	}

	// 修改群组名
	REGISTER_CMDCLASS(C_CmdRegularGroupNameChangeNotice, eCMD_GULARGROUPNAMECHANGENOTICE);
	int	C_CmdRegularGroupNameChangeNotice::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
	int	C_CmdRegularGroupNameChangeNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		eIMString		szID;
		C_eIMSessionMgr smgr;

		::eIMUTF8ToTChar((char*)m_sData.aszGroupName, szValue);
		::eIMUTF8ToTChar((char*)m_sData.aszGroupID, szID);
		smgr.Init(pIDb);
		smgr.SetTitle(GroupId2Qsid(m_sData.aszGroupID), szValue.c_str());

		EIMLOGGER_DEBUG_(pILog, _T("ModifyID:%d, GroupID:%llu, Title:%s, ModifyTime:%d"),
			m_sData.dwModifyID, szID.c_str(), szValue.c_str(), m_sData.dwTime);

		return m_i32Result;
	}
	int	C_CmdRegularGroupNameChangeNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		QSID qsid = GroupId2Qsid(m_sData.aszGroupID);
		SAFE_SEND_EVENT_(pIEMgr,EVENT_MODIFY_VGROUP_NAME_NOTICE,&qsid);
		return m_i32Result;
	}

	// 修改群组公告
	REGISTER_CMDCLASS(C_CmdRegularGroupProclamationChangeNotice, eCMD_GULARGROUPPROCLAMATIONCHANGENOTICE);
	int	C_CmdRegularGroupProclamationChangeNotice::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
	int	C_CmdRegularGroupProclamationChangeNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		eIMString		szValue;
		eIMString		szID;
		C_eIMSessionMgr smgr;

		::eIMUTF8ToTChar((char*)m_sData.aszGroupProclamation, szValue);
		::eIMUTF8ToTChar((char*)m_sData.aszGroupID, szID);
		smgr.Init(pIDb);
		smgr.SetRemark(GroupId2Qsid(m_sData.aszGroupID), szValue.c_str());

		EIMLOGGER_DEBUG_(pILog, _T("ModifyID:%d, GroupID:%llu, Proclamation:%s, ModifyTime:%d"),
			m_sData.dwModifyID, szID.c_str(), szValue.c_str(), m_sData.dwTime);
		return m_i32Result;
	}
	int	C_CmdRegularGroupProclamationChangeNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	// ==============================虚拟组信息===========================
	REGISTER_CMDCLASS(C_CmdGetVirtualGroupReq,CMD_VIRTUAL_GROUP_REQ);
	int C_CmdGetVirtualGroupReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
	{
		CONVERT_TO_ICA_RET_(pvCa);

		m_i32Result = pICa->GetVirtualGroupInfoReq(m_sData.dwTimestamp,m_sData.cTerminalType);

		return m_i32Result;
	}
	int C_CmdGetVirtualGroupReq::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
	{
		return m_i32Result;
	}

	REGISTER_CMDCLASS(C_CmdGetVirtualGroupAck,CMD_VIRTUAL_GROUP_ACK);
	int C_CmdGetVirtualGroupAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		if (FAILED(m_i32Result))
			EIMLOGGER_ERROR_( pILog, _T("failed: load virtual group message failed!"), GetErrorDesc());
		else
			EIMLOGGER_DEBUG_( pILog, _T("success: load virtual group message succeed"));

		return m_i32Result;
	}
	int C_CmdGetVirtualGroupAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
	{
		return m_i32Result;
	}

	REGISTER_CMDCLASS(C_CmdGetVirtualGroupNotice,CMD_VIRTUAL_GROUP_NOTICE);
	int C_CmdGetVirtualGroupNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
	{
		C_eIMSessionMgr smgr;
		smgr.Init(pIDb);
		m_i32Result = smgr.Set(&m_sData);
		return m_i32Result;
	}
	int C_CmdGetVirtualGroupNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
	{
		return m_i32Result;
	}
	// =============================机器人信息=============================
	REGISTER_CMDCLASS(C_CmdRobotSyncReq, eCMD_ROBOTSYNCREQ);
	int	C_CmdRobotSyncReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort/* = NULL*/)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->RobotDataSync(&m_sData);
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
	int	C_CmdRobotSyncReq::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort/* = NULL*/)
	{
		return m_i32Result;
	}
	int	C_CmdRobotSyncReq::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort/* = NULL*/)
	{
		return m_i32Result;
	}

	REGISTER_CMDCLASS(C_CmdRobotSyncRsp, eCMD_ROBOTSYNCRSP);
	int	C_CmdRobotSyncRsp::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort/* = NULL*/)
	{
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}
	int	C_CmdRobotSyncRsp::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort/* = NULL*/)
	{
		C_eIMRoamDataMgr rmgr;
		rmgr.Init(pIDb);

		int iGetPageCount = Eng.GetAttributeInt(IME_ATTRIB_GET_ROBOT_PAGE_COUNT);
		iGetPageCount++;
		Eng.SetAttributeInt(IME_ATTRIB_GET_ROBOT_PAGE_COUNT, iGetPageCount);
		if(iGetPageCount == 1)
		{
			rmgr.ClearRobotInfo();
		}
		Robot_User_Info rInfo;
		for(int i=0; i<m_sData.wRobotNum; i++)
		{
			memset(&rInfo, 0, sizeof(Robot_User_Info));
			memcpy(&rInfo, &m_sData.sRobotList[i], sizeof(Robot_User_Info));
			rmgr.AddRobotInfo(rInfo);
		}
		EIMLOGGER_DEBUG_(pILog, _T("CurrentPage:%d TotalPage:%d   RobotNum:%d"),
			m_sData.wCurrentPage, m_sData.wTotalPage, m_sData.wRobotNum);
		
		return m_i32Result;
	}
	int	C_CmdRobotSyncRsp::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort/* = NULL*/)
	{
		int iGetPageCount = Eng.GetAttributeInt(IME_ATTRIB_GET_ROBOT_PAGE_COUNT);
		if(iGetPageCount == m_sData.wTotalPage)
		{
			EIMLOGGER_DEBUG_(pILog, _T("received robot info finished"));
			SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		}
		return m_i32Result;
	}

	// =============================头像请求与应答=========================
	REGISTER_CMDCLASS(C_CmdGetHeadIconAddListReq, eCMD_GET_HEAD_ICON_ADD_LIST_REQ);
	int	C_CmdGetHeadIconAddListReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		EIMLOGGER_DEBUG_( pILog, _T("Send get others head img time:[%u]"), m_sData);
		m_i32Result = pICa->GetUserHeadIconList(m_sData);
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	REGISTER_CMDCLASS(C_CmdGetHeadIconAddListAck, eCMD_GET_HEAD_ICON_ADD_LIST_RSP);
	int	C_CmdGetHeadIconAddListAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		if (FAILED(m_i32Result))
			EIMLOGGER_ERROR_( pILog, _T("failed: currpage:%d currnum:%d, result: 0x%08X, desc:%s"), m_sData.wCurrPage,m_sData.wCurrNum, m_i32Result, GetErrorDesc());
		else
			EIMLOGGER_DEBUG_( pILog, _T("success: currpage:%d currnum:%d"), m_sData.wCurrPage,m_sData.wCurrNum);
		
		return m_i32Result;
	}

	int	C_CmdGetHeadIconAddListAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMContactsMgr cmgr;
		cmgr.Init(pIDb);
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		UINT32 uPos = 0;
		TUserHeadIconList rList;
		// 再刷新所有数据  可以优化一次更新多条  优化函数待验证
		//while(SUCCEEDED(pICa->ParseUserHeadIconList(m_sData.strPacketBuff, &uPos, &rList)))
		//{
		//	mgr.SetEmpLogotimeInfo(rList.dwUserID, HEAD_IMAGE_NEEDDOWNLOAD);
		//}
		
		int iCount = 0;
		int i32Ret;
#define MAX_INSERT_USER_LEN    100		// 考虑数据库能接收参数的长度
		QEID alleid[MAX_INSERT_USER_LEN] = {0};
		while(SUCCEEDED(i32Ret = pICa->ParseUserHeadIconList(m_sData.strPacketBuff, &uPos, &rList)))
		{
			PARSE_FINISHED_BREAK_(i32Ret);
			alleid[iCount] = rList.dwUserID;
			iCount++;
			if(iCount == MAX_INSERT_USER_LEN)
			{
				cmgr.SetEmpsAllLogotime(alleid, iCount, HEAD_IMAGE_NEEDDOWNLOAD);
				iCount = 0;
				memset(alleid, 0, sizeof(alleid));
			}
		}
		if(iCount > 0)
		{
			cmgr.SetEmpsAllLogotime(alleid, iCount, HEAD_IMAGE_NEEDDOWNLOAD);
		}

		return m_i32Result;
	}

	int	C_CmdGetHeadIconAddListAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		//SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		if(m_sData.wCurrPage == 0)
		{
			SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_EMPSAVATAR_LIST_END, &m_sData);
		}
		return m_i32Result;
	}

	//=====================================数据漫游=========================================

	//C_CmdRoamDataSyncREQ////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdRoamDataSyncREQ, eCMD_ROAMINGDATASYN);
	int C_CmdRoamDataSyncREQ::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->RoamingDataSync(&m_sData);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdRoamDataSyncREQ::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdRoamDataSyncREQ::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdRoamDataSyncAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdRoamDataSyncAck, eCMD_ROAMINGDATASYNACK);
	int C_CmdRoamDataSyncAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdRoamDataSyncAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMRoamDataMgr rmgr;
		rmgr.Init(pIDb);
		m_i32Result = rmgr.Set(&m_sData);
		return m_i32Result;
	}

	int	C_CmdRoamDataSyncAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdRoamDataModify////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdRoamDataModify, eCMD_CUSTOMIZE_ACTION);
	int C_CmdRoamDataModify::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		
		if(m_rRoamDataModi.cRequestType == ROAM_TYPE_COM_CONTACT
			|| m_rRoamDataModi.cRequestType == ROAM_TYPE_COM_DEPT
			|| m_rRoamDataModi.cRequestType == ROAM_TYPE_CONCERN_CONTACT
			|| m_rRoamDataModi.cRequestType == ROAM_TYPE_DEFAULT_COMCONTACT)
		{
			m_i32Result = pICa->RoamingDataModi(&m_rRoamDataModi);
		}

		EIMLOGGER_LOG_(pILog, SUCCEEDED(m_i32Result) ? eLLDEBUG : eLLERROR, _T("%s, result:0x%08x"), m_szXml.c_str(), m_i32Result);
		return m_i32Result;
	}

	int	C_CmdRoamDataModify::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMRoamDataMgr rmgr;
		rmgr.Init(pIDb);
		m_i32Result = rmgr.Set(&m_sData);
		return m_i32Result;
	}

	int	C_CmdRoamDataModify::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);

		//关注人员做单独处理
		if(m_sData.dwCustomizeId == ROAM_TYPE_CONCERN_CONTACT)
		{
			for(DWORD dwIndex = 0; dwIndex < m_sData.dwIdNum; dwIndex++)
			{
				SAFE_SEND_EVENT_(pIEMgr,EVENT_UPDATE_CONCERN_EMP,(void*)m_sData.adwId[dwIndex]);
			}
		}

		return m_i32Result;
	}

	BOOL C_CmdRoamDataModify::FromXml( const TCHAR* pszXml )
	{
		int				i32Value;
		const char*		pszValue = NULL;
		eIMString		szValue;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;
		memset(&m_sData, 0, sizeof(m_sData));
		pEl->QueryValueAttribute(CUSTOMIZE_ID, &m_sData.dwCustomizeId);
		pEl->QueryValueAttribute(CUSTOM_GROUP_ID, &m_sData.iGroupId);
		pEl->QueryValueAttribute(CUSTOM_OPERA_TYPE,(int*)&m_sData.iOperateType);

		pszValue = pEl->Attribute(CUSTOM_GROUP_NAME);
		SET_UTF8_2TVALUE_(pszValue, m_sData.aszName);
		//eIMTChar2UTF8(rData.aszName, m_sDefineGroupName);

		for( pEl = pEl->FirstChildElement(); pEl && m_sData.dwIdNum < EIM_CUSTOM_GTOUP_MEMBER_MAX; pEl = pEl->NextSiblingElement() )
		{
			if ( pEl->Attribute(CUSTOM_MEMNER_ID, &i32Value ) )
				m_sData.adwId[m_sData.dwIdNum++] = i32Value;
		}
	//	PS_EmpInfo_ psEmpInfo = objEgn.GetLoginEmp();
		m_rRoamDataModi.dwUserid = eng.GetPurview(GET_LOGIN_QEID);
		m_rRoamDataModi.dwCompid = eng.GetPurview(GET_LOGIN_COID);
		m_rRoamDataModi.cTerminalType = TERMINAL_PC;
		m_rRoamDataModi.cModifyType = LOBYTE(m_sData.iOperateType);
		m_rRoamDataModi.cRequestType = LOBYTE(m_sData.dwCustomizeId) ;
		m_rRoamDataModi.wNum = LOWORD(m_sData.dwIdNum);
		memcpy(m_rRoamDataModi.dwUsersList, m_sData.adwId, sizeof(m_sData.adwId));

		m_szXml = pszXml;
		return TRUE;
	}


	//C_CmdRoamDataModifyAck//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdRoamDataModifyAck, eCMD_ROAMINGDATAMODIACK);
	int C_CmdRoamDataModifyAck::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.cResult);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdRoamDataModifyAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMEngine& eng = Eng;
		C_eIMRoamDataMgr rmgr;
		rmgr.Init(pIDb);
		m_i32Result = rmgr.Set(&m_sData);

		if(m_sData.cModifyType == ROAM_OPER_TYPE_ADD 
			&& (m_sData.cResponseType == ROAM_TYPE_COM_CONTACT			// 常用联系人
			|| m_sData.cResponseType == ROAM_TYPE_CONCERN_CONTACT))		// 关注联系人直接有返回状态
		{
			GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
			TUserStatusList rList;
			pICa->user_status_Parse(&m_sData.tUserStatus, &rList);
			S_EmpInfo		 sEmp = { 0 };
			C_eIMContacts&	 obj = C_eIMContacts::GetObject();
			for(DWORD i=0; i< rList.dwUserStatusNum; i++)
			{
				sEmp.Id			  = rList.szUserStatus[i].dwUserID;
				sEmp.bitStatus	  =(rList.szUserStatus[i].cStatus & IME_EMP_STATUS_MASK);
				sEmp.bitLoginType =(rList.szUserStatus[i].cLoginType & IME_EMP_TERM_MASK);

				EIMLOGGER_DEBUG_(pILog, _T("Update user[%d]'s status, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
				if (!obj.SetEmps(sEmp, eIMECSET_MODS, TRUE))
					EIMLOGGER_ERROR_(pILog, _T("Update user[%d]'s status failed, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
			}
		}

		return m_i32Result;
	}

	int	C_CmdRoamDataModifyAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		//SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_SPECIAL_LIST_END, (void*)m_i32Result);
		return m_i32Result;
	}

	//C_CmdRoamDataModifyNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdRoamDataModifyNotice, eCMD_ROAMINGDATAMODINOTICE);
	int C_CmdRoamDataModifyNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{	
		CMD_TRACE_(m_i32Result);
		return m_i32Result;
	}

	int	C_CmdRoamDataModifyNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMEngine& eng = Eng;
		C_eIMRoamDataMgr rmgr;
		rmgr.Init(pIDb);
		m_i32Result = rmgr.Set(&m_sData);

		if(m_sData.cModifyType == ROAM_OPER_TYPE_ADD 
			&& (m_sData.cResponseType == ROAM_TYPE_COM_CONTACT			// 常用联系人
			|| m_sData.cResponseType == ROAM_TYPE_CONCERN_CONTACT))		// 关注联系人直接有返回状态
		{
			GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
			TUserStatusList rList;
			pICa->user_status_Parse(&m_sData.tUserStatus, &rList);
			S_EmpInfo		 sEmp = { 0 };
			C_eIMContacts&	 obj = C_eIMContacts::GetObject();
			for(DWORD i=0; i< rList.dwUserStatusNum; i++)
			{
				sEmp.Id			  = rList.szUserStatus[i].dwUserID;
				sEmp.bitStatus	  =(rList.szUserStatus[i].cStatus & IME_EMP_STATUS_MASK);
				sEmp.bitLoginType =(rList.szUserStatus[i].cLoginType & IME_EMP_TERM_MASK);

				EIMLOGGER_DEBUG_(pILog, _T("Update user[%d]'s status, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
				if (!obj.SetEmps(sEmp, eIMECSET_MODS, TRUE))
					EIMLOGGER_ERROR_(pILog, _T("Update user[%d]'s status failed, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
			}
		}
		
		return m_i32Result;
	}

	int	C_CmdRoamDataModifyNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);

		//关注人员做单独处理
		if(m_sData.cResponseType == ROAM_TYPE_CONCERN_CONTACT)
		{
			GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
			TUserStatusList rList;
			pICa->user_status_Parse(&m_sData.tUserStatus, &rList);
			S_EmpInfo		 sEmp = { 0 };
			C_eIMContacts&	 obj = C_eIMContacts::GetObject();
			for(DWORD i=0; i< rList.dwUserStatusNum; i++)
			{
				SAFE_SEND_EVENT_(pIEMgr,EVENT_UPDATE_CONCERN_EMP,(void*)rList.szUserStatus[i].dwUserID);
			}
		}

		return m_i32Result;
	}

	//=====================================数据漫游 end======================================

	//=====================================状态订阅 start====================================
	//C_CmdRoamDataModifyNotice//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdTGetStatusReq, eCMD_GET_STATUS_REQ);
	int	C_CmdTGetStatusReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		C_eIMEngine& eng = Eng;

		m_sData.uUserId  = eng.GetPurview(GET_LOGIN_QEID);
		m_sData.dwCompID = eng.GetPurview(GET_LOGIN_COID);
		m_sData.cTerminalType = TERMINAL_PC;
		m_i32Result = pICa->GetUserStateReq(&m_sData);

		if ( SUCCEEDED(m_i32Result) )
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_ERROR_(pILog, _T("%s"), m_szXml.c_str());

		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdTGetStatusReq::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdTGetStatusReq::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	BOOL C_CmdTGetStatusReq::FromXml( const TCHAR* pszXml )
	{
		int				i32Value;
		const char*		pszValue = NULL;
		eIMString		szValue;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;
		pEl->Attribute(SUBSCRIBE_GETSTATETYPE, (int*)&m_sData.cUserType) ;
		for( pEl = pEl->FirstChildElement(); pEl && m_sData.nUserNum < eng.GetPurview(GET_STATUS_MAX_NUM); pEl = pEl->NextSiblingElement() )
		{
			if ( pEl->Attribute(CUSTOM_MEMNER_ID, &i32Value ) )
				m_sData.aUserId[m_sData.nUserNum++] = i32Value;
		}
	
		m_sData.uUserId  = eng.GetPurview(GET_LOGIN_QEID);
		m_sData.dwCompID = eng.GetPurview(GET_LOGIN_COID);
		m_sData.cTerminalType = TERMINAL_PC;
		m_szXml = pszXml;

		return TRUE;
	}

	//C_CmdTGetStatusRsp//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdTGetStatusRsp, eCMD_NOTICE_STATE);
	int	C_CmdTGetStatusRsp::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		//CMD_TRACE_(m_i32Result);
		EIMLOGGER_DATA_(pILog, (BYTE*)m_sData.szUserStatus, m_sData.dwUserStatusNum * sizeof(m_sData.szUserStatus[0]), 
			_T("Count:%d, first of 10 status: %d:%d; %d:%d; %d:%d; %d:%d; %d:%d; %d:%d; %d:%d; %d:%d; %d:%d; %d:%d;"),
			m_sData.dwUserStatusNum, 
			m_sData.szUserStatus[0].dwUserID, m_sData.szUserStatus[0].cStatus, 
			m_sData.szUserStatus[1].dwUserID, m_sData.szUserStatus[1].cStatus, 
			m_sData.szUserStatus[2].dwUserID, m_sData.szUserStatus[2].cStatus, 
			m_sData.szUserStatus[3].dwUserID, m_sData.szUserStatus[3].cStatus, 
			m_sData.szUserStatus[4].dwUserID, m_sData.szUserStatus[4].cStatus, 
			m_sData.szUserStatus[5].dwUserID, m_sData.szUserStatus[5].cStatus, 
			m_sData.szUserStatus[6].dwUserID, m_sData.szUserStatus[6].cStatus, 
			m_sData.szUserStatus[7].dwUserID, m_sData.szUserStatus[7].cStatus, 
			m_sData.szUserStatus[8].dwUserID, m_sData.szUserStatus[8].cStatus, 
			m_sData.szUserStatus[9].dwUserID, m_sData.szUserStatus[9].cStatus);

		return m_i32Result;
	}

	int	C_CmdTGetStatusRsp::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

		S_EmpInfo		 sEmp = { 0 };
		C_eIMContacts&	 obj = C_eIMContacts::GetObject();
		C_eIMStateSubscribeMgr& objSubscribeMgr = C_eIMStateSubscribeMgr::GetObject();;
		for(DWORD i=0; i< m_sData.dwUserStatusNum; i++)
		{
			if(*pbAbort)
			{
				break;
			}
			
			sEmp.Id			  = m_sData.szUserStatus[i].dwUserID;
			sEmp.bitStatus	  =(m_sData.szUserStatus[i].cStatus & IME_EMP_STATUS_MASK);
			sEmp.bitLoginType =(m_sData.szUserStatus[i].cLoginType & IME_EMP_TERM_MASK);

			PS_EmpInfo_ psEmpInfo = obj.GetEmp(sEmp.Id);
			if( psEmpInfo && psEmpInfo->bitConcern && 
				(sEmp.bitStatus == IME_EMP_STATUS_ONLINE || sEmp.bitStatus == IME_EMP_STATUS_LEAVE) && 
				psEmpInfo->bitStatus != IME_EMP_STATUS_ONLINE &&
				psEmpInfo->bitStatus != IME_EMP_STATUS_LEAVE )
			{
				m_vectConcernEmp.push_back(sEmp.Id);
			}

            if(objSubscribeMgr.IsRobot(m_sData.szUserStatus[i].dwUserID))
			{
				continue;
			}

			//EIMLOGGER_DEBUG_(pILog, _T("Update user[%d]'s status, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
			if (!obj.SetEmps(sEmp, eIMECSET_MODS, TRUE))
				EIMLOGGER_ERROR_(pILog, _T("Update user[%d]'s status failed, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
		}

		return m_i32Result;
	}

	int	C_CmdTGetStatusRsp::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);

		vector<QEID>::iterator it = m_vectConcernEmp.begin();
		while(it != m_vectConcernEmp.end())
		{
			if(*pbAbort)
			{break;}

			SAFE_SEND_EVENT_(pIEMgr,EVETN_CONCERN_PROMPT,(void*)*it);
			it++;
		}

		return m_i32Result;
	}

	//C_CmdNoticeStateAll//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdNoticeStateAll, eCMD_NOTICESTATE_ALL);//因底层拆分成状态通知这里不需要处理
	int	C_CmdNoticeStateAll::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		//m_i32Result = pICa->GetErrorCode(m_sData.cResult);
		CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdNoticeStateAll::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdNoticeStateAll::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//C_CmdSubscribeReq//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdSubscribeReq, eCMD_SUBSCRIBERREQ);
	int	C_CmdSubscribeReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		
		m_i32Result = pICa->SendSubscribeReq(&m_sData);

		//CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdSubscribeReq::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdSubscribeReq::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	BOOL C_CmdSubscribeReq::FromXml( const TCHAR* pszXml )
	{
		int				i32Value;
		const char*		pszValue = NULL;
		eIMString		szValue;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;
		pEl->Attribute(SUBSCRIBE_OPERTYPE,(int*)&m_sData.cRequestType) ;
		for( pEl = pEl->FirstChildElement(); pEl && m_sData.wNum < eng.GetPurview(TEMP_SUBSCRIBE_MAX_NUM); pEl = pEl->NextSiblingElement() )
		{
			if ( pEl->Attribute(CUSTOM_MEMNER_ID, &i32Value ) )
				m_sData.dwIdList[m_sData.wNum++] = i32Value;
		}


		m_sData.mPackageHead.dwCompId =eng.GetPurview(GET_LOGIN_COID);
		m_sData.mPackageHead.cBusinessType = 101;
		m_sData.mPackageHead.dwSrcId = eng.GetPurview(GET_LOGIN_QEID);
		m_sData.mPackageHead.dwDstId = eng.GetPurview(GET_LOGIN_QEID);

		return TRUE;
	}

	//C_CmdSubscribeRsp//////////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdSubscribeRsp, CMD_PROTOCOL_V2);
	int	C_CmdSubscribeRsp::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->GetErrorCode(m_sData.cResult);
		//CMD_TRACE_(m_i32Result);

		return m_i32Result;
	}

	int	C_CmdSubscribeRsp::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		GET_ICA_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		TUserStatusList rList;
		pICa->user_status_Parse(&m_sData.mUserStatus, &rList);
		S_EmpInfo		 sEmp = { 0 };
		C_eIMContacts&	 obj = C_eIMContacts::GetObject();
		for(DWORD i=0; i< rList.dwUserStatusNum; i++)
		{
			sEmp.Id			  = rList.szUserStatus[i].dwUserID;
			sEmp.bitStatus	  =(rList.szUserStatus[i].cStatus & IME_EMP_STATUS_MASK);
			sEmp.bitLoginType =(rList.szUserStatus[i].cLoginType & IME_EMP_TERM_MASK);

			//EIMLOGGER_DEBUG_(pILog, _T("Update user[%d]'s status, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
			if (!obj.SetEmps(sEmp, eIMECSET_MODS, TRUE))
				EIMLOGGER_ERROR_(pILog, _T("Update user[%d]'s status failed, state:%d, login type:%d"), sEmp.Id, sEmp.bitStatus, sEmp.bitLoginType);
		}

		return m_i32Result;
	}

	int	C_CmdSubscribeRsp::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	//=====================================状态订阅 end====================================

	/*Frame template **************************************************************
	//classname////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(clsname, cmdid);
	int clsname::DoCmd()
	{
		GET_CMD_REF_INTERFACE_();
		m_i32Result = 0;
		// Add code here...

		return TRUE;
	}
	******************************************************************************/


//- eCMD_SEND_SMS,	
REGISTER_CMDCLASS(C_CmdSendSms, eCMD_SEND_SMS);
int C_CmdSendSms::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
{
	CONVERT_TO_ICA_RET_(pvCa);
	m_i32Result = pICa->SendSMSEx( &m_sData );
	CMD_TRACE_STR_(m_szXml.substr(0, 1024), m_i32Result);

	m_retResult.i32Result = m_i32Result;
	return m_i32Result;
}

int	C_CmdSendSms::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
//	const char*		pszValue;
	S_Sms sSms = {0};
	C_eIMSMSMgr smsgr;
	smsgr.Init(pIDb);
	int i32Result = 0;

	if(EIMERR_NO_ERROR == m_i32Result)
	{
		S_Sms sInfo = {0};
		sInfo.qmid			 = m_sMsg.qmid;
		sInfo.dwFlag		 = m_sMsg.dwFlag;
		sInfo.dwTime		 = m_sMsg.dwTime;
		sInfo.iType			 = m_sMsg.eType;
		sInfo.i32newmsg_flag = 0;
		sInfo.i32offmsg_flag = 0;

		if(0 == m_retResult.qEid)
		{
			sInfo.eidorPhone		 = m_retResult.u64Phone;
			sInfo.i32eidOrPhoneType  = 1;
		}
		else
		{
			sInfo.eidorPhone		 = m_retResult.qEid;
			sInfo.i32eidOrPhoneType  = 0;
		}
		i32Result = smsgr.Set(&sInfo, m_szMsgUI.c_str());
	}

	if ( FAILED(i32Result) )
	{
		//SAFE_SEND_EVENT_(pIEMgr, EVENT_SMS_SUBMIT_RLT, (void*)&m_retResult);
		EIMLOGGER_ERROR_(pILog, _T("QMID: %llu,  Time:%d, Msg:%s"), m_sMsg.qmid, m_sMsg.dwTime, m_szMsgUI.c_str());
		return m_i32Result;
	}

	return EIMERR_NO_ERROR;
}

int	C_CmdSendSms::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
{
	SAFE_SEND_EVENT_(pIEMgr, EVENT_SMS_SUBMIT_RLT, (void*)&m_retResult);
	return EIMERR_NO_ERROR;
}

BOOL C_CmdSendSms::FromXml( const TCHAR* pszXml )
{
	GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
	C_eIMEngine& eng = Eng;

	int				i32Value = 0;
	long long		i64Value = 0ll;
	const char*		pszValue = NULL;
	eIMStringA		szValueA;
	memset(&m_retResult, 0, sizeof(S_SmsSendRlt));
	TiXmlElement*	pEl = ParseXmlCmdHeader(m_xmlDoc, pszXml, m_i32CmdId, m_i32Result);

	CHECK_NULL_RET_(pEl, FALSE);
	ResetData();
	ZERO_STRUCT_(m_sMsg);

	m_sMsg.qeid = eng.GetPurview(GET_LOGIN_QEID);
	char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
	if ( pEl->Attribute( FIELD_MSG_ID) == NULL )		// Check and set QMID
	{
		//m_sMsg.qmid		= ::eIMGetMsgId(m_sMsg.qeid);
		m_sMsg.qmid = eng.NewID();	// By CA
		m_sData.dwMsgID = m_sMsg.qmid;
		pEl->SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sMsg.qmid, szBuf) );
	}
	else
	{
		m_sData.dwMsgID = GroupId2Qsid(pEl->Attribute( FIELD_MSG_ID));
		m_sMsg.qmid	 = m_sData.dwMsgID;
	}

	 
	//pEl->Attribute( FIELD_IS_GROUP, &i32Value ); m_sData.cIsGroup   = i32Value; 
	//pEl->Attribute( FIELD_MSG_TOTAL,&i32Value ); m_sData.nMsgTotal  = i32Value; 
	//pEl->Attribute( FIELD_MSG_SEQ,  &i32Value ); m_sData.nMsgSeq    = i32Value; 
	pEl->Attribute( FIELD_RECVER_ID,&i32Value ); m_sData.dwRecverID = i32Value;
	pEl->Attribute( FIELD_READ,	    &i32Value ); m_sData.cRead      = i32Value;
	pEl->Attribute( FIELD_MSG_LEN,  &i32Value ); m_sData.dwMsgLen   = i32Value; 
	if (pEl->Attribute( FIELD_TYPE,	&i32Value ))
		m_sData.cType = i32Value; 
	else
		m_sData.cType = 0xFF;	// Flag of no this attribute

	const char* pszWndID = pEl->Attribute(FIELD_SMS_WND_ID);
	m_retResult.dwWndID = (DWORD)char2Int64(pszWndID);

	m_retResult.qEid = m_sData.dwRecverID;

	UINT64 u64Phone = 0;
	const char* pcszPhone = pEl->Attribute(SMS_RECV_PHONE); 
	u64Phone = char2Int64(pcszPhone);
	if(0 == u64Phone) return FALSE;
	m_retResult.u64Phone = u64Phone;

	DWORD dwNo = 0;
	const char* pcszNo = pEl->Attribute(FILED_BATCH_NO);
	dwNo = (DWORD)char2Int64(pcszNo);

	DWORD dwTotal = 0;
	const char* pcszTotal = pEl->Attribute(FILED_SMS_TOTAL);
	dwTotal = (DWORD)char2Int64(pcszTotal);

	const char* pcszRecvName = pEl->Attribute( FIELD_RECVER_NAME );
	const char* pcszRecvCode = pEl->Attribute( FIELD_RECV_UCODE );

	// <MsgUI /> ==========================================================
	pEl = pEl->FirstChildElement();	
	if ( pEl == NULL || _stricmp(pEl->Value(), FIELD_MSGUI) )
	{
		TRACE_(_T("Error SendMsg format[%s]"), pszXml);
		m_i32Result = EIMERR_BAD_CMD;
		return FALSE;
	}

	if ( pEl->Attribute( FIELD_SENDER_ID ) == NULL )		// Check and set QEID
		pEl->SetAttribute(FIELD_SENDER_ID, (int)m_sMsg.qeid);

	if ( pEl->Attribute( FIELD_SEND_TIME) == NULL )		// Check and set SendTime
	{
		m_sMsg.dwTime = eng.GetTimeNow();
		pEl->SetAttribute(FIELD_SEND_TIME, (int)m_sMsg.dwTime);
	}

	if ( pEl->Attribute( FIELD_SENDER_NAME ) == NULL )	// Check and Set SenderName
	{
		GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		::eIMTChar2UTF8(pIConts->GetEmpName(m_sMsg.qeid), szValueA);
		pEl->SetAttribute(FIELD_SENDER_NAME, szValueA.c_str());
	}

	if ( pEl->Attribute( FIELD_USER_CODE ) == NULL )	// Check and Set SenderName
	{
		GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		PS_EmpInfo_ psEmpInfo = pIConts->GetEmp(m_sMsg.qeid);
		ASSERT_(psEmpInfo);
		if ( psEmpInfo )
		{
			::eIMTChar2UTF8(psEmpInfo->pszCode, szValueA);
			pEl->SetAttribute(FIELD_USER_CODE, szValueA.c_str());
		}
	}

	
	pEl->SetAttribute(FIELD_RECVER_ID, m_sData.dwRecverID);
	if(NULL == pcszRecvName)
		pEl->SetAttribute(FIELD_RECVER_NAME, pcszPhone);
	else
		pEl->SetAttribute(FIELD_RECVER_NAME, pcszRecvName);

	if(NULL == pcszRecvCode)
		pEl->SetAttribute(FIELD_RECV_UCODE, pcszPhone);
	else
		pEl->SetAttribute(FIELD_RECV_UCODE, pcszRecvCode);

	if(NULL == pcszPhone)
		pEl->SetAttribute(SMS_RECV_PHONE, "");
	else
		pEl->SetAttribute(SMS_RECV_PHONE, pcszPhone);



	if ( pEl->Attribute( FIELD_IS_SEND ) == NULL )		// Check and Set IsSend
		pEl->SetAttribute(FIELD_IS_SEND, 1);

	m_sMsg.dwFlag= 0;	// Maybe set m_sData.cMsgType...
	m_sMsg.eType = eSESSION_TYPE_SMS;
	if ( pEl->Attribute( FIELD_TYPE ) == NULL )			// Check and Set SessionType
	{
		pEl->SetAttribute(FIELD_TYPE, m_sMsg.eType);
	}

	pEl->Attribute( FIELD_SEND_TIME, (int*)&m_sData.nSendTime);
	pEl->Attribute( FIELD_SENDER_ID, (int*)&m_sData.dwUserID);

	// <MsgUI><Message /></MsgUI> =========================================
	TiXmlHandle h(pEl);
	TiXmlElement*	pMessageEl = h.FirstChildElement(SMS_MSG).ToElement();	
	if ( pMessageEl == NULL )
	{
		TRACE_(_T("Error SendMsg format[%s]"), pszXml);
		m_i32Result = EIMERR_BAD_CMD;
		return FALSE;
	}

	m_sData.dwMsgLen = MSG_MAXLEN;
	if ( !Xml2Msg(pMessageEl, m_sData.aszMessage, m_sData.dwMsgLen, pIProt, m_sMsg.qeid, u64Phone, dwNo, dwTotal) )
	{
		m_i32Result = EIMERR_BAD_CMD;
		return FALSE;
	}

	// <MsgUI><MsgFont /></MsgUI> =========================================
	if ( h.FirstChildElement(FIELD_MSGUI).ToElement() == NULL )
	{	// Need add it
		eng.AddFontElement(m_sMsg.eType, TRUE, pEl);
	}

	// Need convert to string because may be add some attribute
	ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
	ConvertToXml(*pEl, m_szMsgUI);

	m_i32Result = EIMERR_NO_ERROR;
	return TRUE;
}

char* C_CmdSendSms::Xml2Msg(  TiXmlElement* pEl, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol, QEID qeid, UINT64 u64Phone, DWORD dwNo, DWORD dwTotal )
{
#define CHECK_BUF_SIZE_( NEED, TOTAL, RET ) \
	do{ \
	if ( (NEED) > (TOTAL) ) \
	{ \
	ASSERT_(FALSE); \
	TRACE_( _T("Message out of buffer[%d > %d]!"), (NEED), (TOTAL) ); \
	return (RET); \
} \
	}while( 0 )

	int				i32Ret   = 0;
	int				i32Index = 0;
	const char*		pszValue = NULL;
	const char*		pszAttr  = NULL;
	char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };


	for ( pEl = pEl->FirstChildElement(); pEl; pEl = pEl->NextSiblingElement() )
	{	// Enumerate each child element
		pszValue = pEl->Value();
		ASSERT_( pszValue );
		CHECK_NULL_RET_(pszValue, NULL);

		if ( _stricmp(pszValue, FIELD_MSGFONT ) == 0 )
		{	// MsgFont
			DWORD	   dwColor   = strtol( pEl->Attribute(FIELD_COLOR), NULL, 0 );
			S_FONTINFO sFontInfo = { 0 };

			pszAttr = pEl->Attribute(FIELD_ID);
			sFontInfo.u8Id   = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_ID_DEFAULT;

			pszAttr = pEl->Attribute(FIELD_MODE);
			sFontInfo.u8Mode = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_MODE_DEFAULT;

			pszAttr = pEl->Attribute(FIELD_SIZE);
			sFontInfo.u8Size = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_SIZE_DEFAULT;

			sFontInfo.au8Color[0] = (BYTE)GetRValue(dwColor);	// R
			sFontInfo.au8Color[1] = (BYTE)GetGValue(dwColor);	// G
			sFontInfo.au8Color[2] = (BYTE)GetBValue(dwColor);	// B

			CHECK_BUF_SIZE_( i32Index + (int)sizeof(S_FONTINFO), u16Size, NULL );
			memcpy( &pszMsg[i32Index], &sFontInfo, sizeof(S_FONTINFO) );
			i32Index += sizeof(S_FONTINFO);
		}
		else if ( _stricmp(pszValue, FIELD_SMS_TEXT ) == 0 )
		{	// MsgText
			//SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
			pszAttr = pEl->GetText();
			i32Ret  = strlen(pszAttr);

			ASSERT_( i32Ret );
			CHECK_BUF_SIZE_( i32Index + i32Ret, u16Size, NULL );
			S_SMSINFO sInfo = { 0 };

			sInfo.u64Phone = htonl64(u64Phone);
			sInfo.dwTotal = htonl(dwTotal);
			sInfo.dwNo = htonl(dwNo);
			memcpy(sInfo.szSmsContent, pszAttr, i32Ret);
			i32Ret = sizeof(sInfo);
			memcpy( &pszMsg[i32Index], &sInfo, i32Ret );
			i32Index += i32Ret;
		}
		else
		{	// Error type of message...
			ASSERT_(FALSE);
		}
	}

	u16Size = (WORD)i32Index + 1;
	return pszMsg;
}

//C_CmdReadMsgSyncReq//////////////////////////////////////////////////////////////////
REGISTER_CMDCLASS(C_CmdReadMsgSyncReq, eCMD_READ_MSG_SYNC_REQ);
int	C_CmdReadMsgSyncReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
{
	CONVERT_TO_ICA_RET_(pvCa);
	m_i32Result = pICa->MsgReadSyncReq(&m_sData);
	CMD_TRACE_(m_i32Result);

	return m_i32Result;
}

//C_CmdReadMsgSyncReq//////////////////////////////////////////////////////////////////
REGISTER_CMDCLASS(C_CmdReadMsgSyncNotice, eCMD_READ_MSG_SYNC_NOTICE);
int	C_CmdReadMsgSyncNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
{
	return EIMERR_NO_ERROR;
}

int	C_CmdReadMsgSyncNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
	TiXmlElement	ReadEl(PROTOCOL_ELEMENT_CMD);
	C_eIMMsgMgr		msgr;
	C_eIMSessionMgr smgr;
	char			szSid[40] = { 0 };

	msgr.Init(pIDb);
	smgr.Init(pIDb);

	ReadEl.SetAttribute(FIELD_USER_ID, m_sData.dwUserID);
	ReadEl.SetAttribute(FIELD_TERM_TYPE, m_sData.cTerminalType);
	ReadEl.SetAttribute(FIELD_COUNT, m_sData.wNum);

	for ( UINT16 u16Index = 0; u16Index < m_sData.wNum; u16Index++ )
	{
		session_data& sItem = m_sData.aSessionData[u16Index];
		TiXmlElement ItemEl(FIELD_ITEM);
		QSID sid = (sItem.cType == 1) ? Eng.NewID(sItem.dwUserID, m_sData.dwUserID) : GroupId2Qsid((char*)sItem.aszGroupID);
		
		GET_QSID_ASTR_(sid, szSid);
		ItemEl.SetAttribute(FIELD_TIMESTAMP, sItem.dwTimestamp);
		ItemEl.SetAttribute(FIELD_TYPE, sItem.cType);
		ItemEl.SetAttribute(FIELD_GROUP_ID, szSid);

		ReadEl.InsertEndChild(ItemEl);

		// 先简单处理，收到了就完全清除当前会话的“新消息”标识
	//	smgr.SendMsgRead(sid);
		smgr.UpdateFlag(sid, (DWORD)eSEFLAG_OFFLINE|eSEFLAG_NEW, FALSE);	// 清除
		msgr.UpdateReadMsgSyncNotice(sid, sItem.dwTimestamp);				// 清除
	}

	ConvertToXml( ReadEl, m_szXml );
	EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());
	return EIMERR_NO_ERROR;
}

int	C_CmdReadMsgSyncNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
SAFE_SEND_EVENT_(pIEMgr, eCMD_READ_MSG_SYNC_NOTICE, (void*)m_szXml.c_str());
	return EIMERR_NO_ERROR;
}

//C_CmdContactsUpdateNotice//////////////////////////////////////////////////////////////////
REGISTER_CMDCLASS(C_CmdContactsUpdateNotice, eCMD_CONTACTS_UPDATE_NOTICE);
int	C_CmdContactsUpdateNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
{
	CONVERT_TO_ICA_RET_(pvCa);
  	EIMLOGGER_DEBUG_(pILog, _T("qeid:%u, timestamp:%u, term:%u, updatetype:%u, count:%u"), m_sData.dwUserID, m_sData.dwTimeStampe, m_sData.cTerminalType, m_sData.cUpdateType, m_sData.dwCount);
	m_i32Result = pICa->SendContactsUpdateAck(m_sData.dwTimeStampe);
	return m_i32Result;
}

int	C_CmdContactsUpdateNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
	C_eIMContactsMgr cmgr;
	S_UpdateContsInfo sInfo = { m_sData.dwTimeStampe };
	cmgr.Init(pIDb);

	if (cmgr.GetReloadContactsInfo(&sInfo))
	{
		EIMLOGGER_ERROR_(pILog, _T("Repeat notice of qeid:%u, timestamp:%u, term:%u, updatetype:%u, count:%u"), m_sData.dwUserID, m_sData.dwTimeStampe, m_sData.cTerminalType, m_sData.cUpdateType, m_sData.dwCount);
		m_i32Result = EIMERR_REPEAT_NOTICE;
	}
	else
		cmgr.SetReloadContactsInfo(m_sData.dwTimeStampe, m_sData.cUpdateType, m_sData.dwCount, IME_UPDATE_CONTS_FLAG_NOT_START);

	return m_i32Result;
}

int	C_CmdContactsUpdateNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
	if ( m_sData.cUpdateType != IME_UPDATE_CONTS_TYPE_NOW || m_i32Result == EIMERR_REPEAT_NOTICE)
		return EIMERR_NO_ERROR;

	if ( m_sData.dwCount == 0 )
	{
		SAFE_SEND_EVENT_(pIEMgr, EVENT_RELOAD_CONTACTS, (void*)IME_UPDATE_CONTS_TYPE_NOW);
	}
	else
	{	// Countdown to start contact update
		SetBusinessTypeId(m_i32CmdId);
		C_ContactUpdateWaitOverItem* pWaitItem = new C_ContactUpdateWaitOverItem(m_sData.dwCount * 1000);	// *1000 to ms
		pWaitItem->SetData(this);
		pWaitItem->SetStartTime(GetTickCount());
		C_UIWaitOverMgr::GetObject().AddWaitOverItem(pWaitItem);
	}

	return EIMERR_NO_ERROR;
}

REGISTER_CMDCLASS(C_CmdSendCollectModifyMsg, CMD_FAVORITE_MODIFY_REQ);
int	C_CmdSendCollectModifyMsg::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
{
    if ( GetFlag(CMD_FLAG_MSG_WAIT_FILE) )
    {
        SetFlag(CMD_FLAG_THREAD_UI, CMD_FLAG_THREAD_UI);	
        SetFlag(~CMD_FLAG_THREAD_DB, CMD_FLAG_THREAD_DB);
        return EIMERR_NO_ERROR;
    }

    CONVERT_TO_ICA_RET_(pvCa);
    m_i32Result = pICa->SendFavoriteMsg( &m_sData );
    if(SUCCEEDED(m_i32Result))
    {
        C_MsgWaitOverItem* pBaseWaitItem = new C_MsgWaitOverItem();
        if(3 == m_sData.cOperType)
        {
            SetBusinessTypeId(m_sData.stFavoriteBatch.ddwMsgID[0]);
        }
        else
        {
            SetBusinessTypeId(m_sData.stFavoriteInfo.ddwMsgID);
        }
        pBaseWaitItem->SetData(this);
        pBaseWaitItem->SetStartTime(GetTickCount());
        C_UIWaitOverMgr::GetObject().AddWaitOverItem(pBaseWaitItem);
    }
    return EIMERR_NO_ERROR;
}
int	C_CmdSendCollectModifyMsg::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
	C_eIMMsgCollectMgr MsgCollectMgr;
    MsgCollectMgr.Init(pIDb);
    switch(m_sData.cOperType)
    {
    case 1:
        {
            S_CollectMsg sMsg;
            sMsg.m_collectID = m_sData.stFavoriteInfo.ddwMsgID;
            sMsg.m_dwTime = m_sData.stFavoriteInfo.dwCollTime;
            sMsg.m_eType = (E_CollectType)ChangeMsgType2CollectType(m_szMsgUI);
            sMsg.m_isGroup = m_sData.stFavoriteInfo.cIsGroup;
            sMsg.m_sendid = m_sData.stFavoriteInfo.dwSender;
            eIMString strTitle;
            if(sMsg.m_isGroup)
            {
                S_SessionInfo rInfo;
                C_eIMSessionMgr smgr;
                smgr.Init(pIDb);
                QSID qsid = GroupId2Qsid((char*)m_sData.stFavoriteInfo.strGroupID);
                smgr.Get(qsid, &rInfo);
                strTitle = rInfo.szTitle;
                sMsg.m_Sid = qsid;
            }
            else
            {
                C_eIMContacts&	 obj = C_eIMContacts::GetObject();
                strTitle  = obj.GetEmpName(sMsg.m_sendid);
                sMsg.m_Sid = 0;
            }
            MsgCollectMgr.Set(sMsg, m_szMsgUI.c_str(), strTitle.c_str());
        }
        break;
    case 2:
        break;
    case 3:
        {
            for (int i=0; i< m_sData.stFavoriteBatch.wNum; ++i)
            {
                MsgCollectMgr.Delete(m_sData.stFavoriteBatch.ddwMsgID[i]);
            }
        }
        break;
    default:break;
    }
    
    return EIMERR_NO_ERROR;
}
int	C_CmdSendCollectModifyMsg::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
    if ( GetFlag( CMD_FLAG_MSG_WAIT_FILE) )
    {
        const TCHAR*	ptszValue= NULL;
        const char*		pszValue = NULL;
        const char*		pszPos   = NULL;
        BOOL			bIsImage = FALSE;
        eIMString		szValue;
        eIMStringA		szValueA;
        S_FileInfo		sFileInfo = { 0 };
        PS_FileTrans	psFileTrans = static_cast<PS_FileTrans>(pvBIFile);
        TiXmlHandle h(m_xmlDoc.RootElement());
        TiXmlElement* pMsgEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
        ASSERT_(pMsgEl);

        sFileInfo.pICmd = this;
        sFileInfo.sFile.bitDirect = FILE_DIR_UPLOAD;
        sFileInfo.sFile.bitStatus = FILE_STATUS_READY;
        sFileInfo.sFile.bitType	  = FILE_TRANS_TYPE_OFFLINE;
        QEID eidLogin = Eng.GetPurview(GET_LOGIN_QEID);
        while (pMsgEl)
        {
            bIsImage = FALSE;
            pszValue = pMsgEl->Value();
            ASSERT_(pszValue);
            if ( (bIsImage = (_stricmp(pszValue, FIELD_MSGIMAGE) == 0))  ||
                _stricmp(pszValue, FIELD_MSGANNEX) == 0 ||
                _stricmp(pszValue, FIELD_MSGLONG ) == 0  ||
                _stricmp(pszValue, FIELD_MSGVOICE ) == 0 ||
                _stricmp(pszValue, FIELD_MSGVIDEO ) == 0 )
            {
                sFileInfo.i32Ratio			= -1;
                sFileInfo.sFile.u64Fid		= _strtoui64(pMsgEl->Attribute(FIELD_FID), NULL, 0);
                sFileInfo.sFile.i64FileSize = _strtoi64(pMsgEl->Attribute(FIELD_SIZE), NULL, 0);
                sFileInfo.sFile.bitIsImage  = bIsImage;
                sFileInfo.sFile.u32LoginEmpId = eidLogin;

                pszValue = pMsgEl->Attribute(FIELD_FILE_PATH);	ASSERT_(pszValue);
                SET_UTF8_2TVALUE_(pszValue, sFileInfo.sFile.szFile); 
                m_i32Result = AddFileTask(*psFileTrans, sFileInfo, TRUE);
            }
            else if(_stricmp(pszValue, FIELD_MSGP2P ) == 0 )
            {	//...
                ASSERT_(FALSE);
            }

            pMsgEl = pMsgEl->NextSiblingElement();
        }

        if ( FAILED(m_i32Result) )
            return m_i32Result;

        ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
    }

    return EIMERR_NO_ERROR;
}

BOOL  C_CmdSendCollectModifyMsg::FromXml( const TCHAR* pszXml )
{
    GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
    C_eIMEngine& eng = Eng;

    int				i32Value = 0;
    long long		i64Value = 0ll;
    const char*		pszValue = NULL;
    eIMStringA		szValueA;
    TiXmlElement*	pEl = ParseXmlCmdHeader(m_xmlDoc, pszXml, m_i32CmdId, m_i32Result);
    CHECK_NULL_RET_(pEl, FALSE);
    ResetData();
    m_sData.dwUserID = Eng.GetLoginEmp()->Id;
    m_sData.stFavoriteInfo.dwUserID = m_sData.dwUserID;
    if ( pEl->Attribute( FIELD_MSG_ID) == NULL )		// Check and set QMID
    {        
        m_sData.stFavoriteInfo.ddwMsgID = eng.NewID();
        char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
        pEl->SetAttribute(FIELD_MSG_ID, GET_QSID_ASTR_(m_sData.stFavoriteInfo.ddwMsgID, szBuf) );
    }
    else
    {
        m_sData.stFavoriteInfo.ddwMsgID = GroupId2Qsid(pEl->Attribute( FIELD_MSG_ID));
    }

    pEl->Attribute(FIELD_COLLECT_UPDATE_TYPE,       &i32Value); 
    m_sData.cOperType = i32Value;
    pEl->Attribute( FIELD_IS_GROUP, &i32Value ); 
    m_sData.stFavoriteInfo.cIsGroup   = i32Value; 

    if ( m_sData.stFavoriteInfo.cIsGroup )
    {
        pszValue = pEl->Attribute(FIELD_GROUP_ID);	ASSERT_(pszValue);
        SET_UTF8_VALUE_(pszValue, m_sData.stFavoriteInfo.strGroupID);
    }

    if ( pEl->Attribute( FIELD_COLLECT_TIME) == NULL )
    {
        m_sData.stFavoriteInfo.dwCollTime = eng.GetTimeNow();
        pEl->SetAttribute(FIELD_COLLECT_TIME, (int)m_sData.stFavoriteInfo.dwCollTime);
    }
    TiXmlElement*	pMsgUI = pEl->FirstChildElement(FIELD_MSGUI);
    CHECK_NULL_RET_(pEl, FALSE);
    pMsgUI->Attribute( FIELD_SENDER_ID,&i32Value ); 
    m_sData.stFavoriteInfo.dwSender = i32Value; 
    if ( pMsgUI->Attribute( FIELD_SEND_TIME) == NULL )
    {
        m_sData.stFavoriteInfo.dwSendTime = eng.GetTimeNow();
        pMsgUI->SetAttribute(FIELD_SEND_TIME, (int)m_sData.stFavoriteInfo.dwSendTime);
    }
    else
    {
        pMsgUI->Attribute(FIELD_SEND_TIME,       &i32Value); 
        m_sData.stFavoriteInfo.dwSendTime = i32Value;
    }

    m_sData.stFavoriteInfo.dwMsgSize = MSG_MAXLEN;
    TiXmlHandle h(pMsgUI);
    TiXmlElement*	pMessageEl = h.FirstChildElement(FIELD_MESSAGE).ToElement();	
    if ( pMessageEl == NULL )
    {
        m_i32Result = EIMERR_BAD_CMD;
        return FALSE;
    }
    if ( !Xml2Msg(pMessageEl, m_sData.stFavoriteInfo.strMsgContent, m_sData.stFavoriteInfo.dwMsgSize) )
    {
        m_i32Result = EIMERR_BAD_CMD;
        return FALSE;
    }

    if ( m_dwFlag & CMD_FLAG_MSG_VOICE )
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_VOICE;
    else if ( m_dwFlag & CMD_FLAG_MSG_VIDEO )
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_VIDEO;	
    else if ( m_dwFlag & CMD_FLAG_MSG_ANNEX)
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_FILE;
    else if ( m_dwFlag & CMD_FLAG_MSG_P2P )
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_P2P;	
    else if ( m_dwFlag & CMD_FLAG_MSG_ANNEX )
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_FILERECVED;	
    else if ( m_dwFlag & CMD_FLAG_MSG_LONG )
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_LONGTEXT;	
    else if ( m_dwFlag & CMD_FLAG_MSG_AUTO )
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_PCAUTO;
    else
        m_sData.stFavoriteInfo.wMsgType = eIM_MSGTYPE_TEXT;	
    
    if (GetFlag(CMD_FLAG_TRANSFER))
        SetFlag(CMD_FLAG_MSG_WAIT_FILE, CMD_FLAG_MSG_WAIT_FILE);
    else
        SetFlag(0, CMD_FLAG_MSG_WAIT_FILE);

    ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
    ConvertToXml(*pMsgUI, m_szMsgUI);
    return TRUE;
}

char* C_CmdSendCollectModifyMsg::Xml2Msg( TiXmlElement* pEl, char* pszMsg, WORD& u16Size)
{
#define CHECK_BUF_SIZE_( NEED, TOTAL, RET ) \
    do{ \
    if ( (NEED) > (TOTAL) ) \
    { \
    ASSERT_(FALSE); \
    TRACE_( _T("Message out of buffer[%d > %d]!"), (NEED), (TOTAL) ); \
    return (RET); \
} \
    }while( 0 )

    int				i32Ret   = 0;
    int				i32Index = 0;
    const char*		pszValue = NULL;
    const char*		pszAttr  = NULL;
    char szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
    for ( pEl = pEl->FirstChildElement(); pEl; pEl = pEl->NextSiblingElement() )
    {
        pszValue = pEl->Value();
        ASSERT_( pszValue );
        CHECK_NULL_RET_(pszValue, NULL);

        if ( _stricmp(pszValue, FIELD_MSGFONT ) == 0 )
        {
            DWORD	   dwColor   = strtol( pEl->Attribute(FIELD_COLOR), NULL, 0 );
            S_FONTINFO sFontInfo = { 0 };

            pszAttr = pEl->Attribute(FIELD_ID);
            sFontInfo.u8Id   = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_ID_DEFAULT;

            pszAttr = pEl->Attribute(FIELD_MODE);
            sFontInfo.u8Mode = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_MODE_DEFAULT;

            pszAttr = pEl->Attribute(FIELD_SIZE);
            sFontInfo.u8Size = pszAttr ? (BYTE)strtol(pszAttr, NULL, 0) : FONT_SIZE_DEFAULT;

            sFontInfo.au8Color[0] = (BYTE)GetRValue(dwColor);	// R
            sFontInfo.au8Color[1] = (BYTE)GetGValue(dwColor);	// G
            sFontInfo.au8Color[2] = (BYTE)GetBValue(dwColor);	// B

            CHECK_BUF_SIZE_( i32Index + (int)sizeof(S_FONTINFO), u16Size, NULL );
            memcpy( &pszMsg[i32Index], &sFontInfo, sizeof(S_FONTINFO) );
            i32Index += sizeof(S_FONTINFO);
        }
        else if ( _stricmp(pszValue, FIELD_MSGTEXT ) == 0 )
        {
            SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
            pszAttr = pEl->GetText();
            i32Ret  = strlen(pszAttr);
            CHECK_BUF_SIZE_( i32Index + i32Ret, u16Size, NULL );
            memcpy( &pszMsg[i32Index], pszAttr, i32Ret );
            i32Index += i32Ret;
        }
        else if ( _stricmp(pszValue, FIELD_MSGEMOT ) == 0 )
        {
            SetFlag(CMD_FLAG_MSG_EMOT, CMD_FLAG_MSG_EMOT);
            pszValue = pEl->Attribute(FIELD_NAME);
            if ( pszValue )
            {
                i32Ret = _snprintf( &pszMsg[i32Index], u16Size - i32Index - 1, "[%s]", pszValue );
                if ( i32Ret < 0 )
                {
                    return NULL;
                }
                i32Index += i32Ret;
            }
        }
        else if ( _stricmp(pszValue, FIELD_MSGIMAGE) == 0 )
        {
            SetFlag(CMD_FLAG_MSG_IMAGE, CMD_FLAG_MSG_IMAGE);
            CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
            pszValue =  pEl->Attribute(FIELD_KEY);
            if( pszValue )
            {
                const char* pszFileExt =  strrchr(pEl->Attribute(FIELD_NAME), '.');
                i32Ret = _snprintf( &pszMsg[i32Index], u16Size - i32Index - 1, "[#%s%s]", pszValue, pszFileExt);
                if ( i32Ret < 0 )
                {
                    return NULL;
                }
                i32Index += i32Ret;
            }
        }
        else if ( _stricmp(pszValue, FIELD_MSGANNEX ) == 0 )	// 8751084#mMvyAn#depot_tools.zip
        {
            SetFlag(CMD_FLAG_MSG_ANNEX, CMD_FLAG_MSG_ANNEX);
            CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
            FILE_META* psFileMeta  = (FILE_META*)pszMsg;
            SetFileMetaData(pEl, *psFileMeta, i32Index);
//             pszValue = pEl->Attribute(FIELD_KEY);
//             if ( pszValue )
//             {
//                 eIMString szData;
//                 i32Index			   = sizeof( FILE_META );
//                 FILE_META* psFileMeta  = (FILE_META*)pszMsg;
//                 psFileMeta->dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
//                 strncpy(psFileMeta->aszURL, pszValue, COUNT_OF_ARRAY_(psFileMeta->aszURL));
//                 strncpy(psFileMeta->aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(psFileMeta->aszFileName));
//                 psFileMeta->dwFileSize = htonl(psFileMeta->dwFileSize);
//             }
        }
        else if ( _stricmp(pszValue, FIELD_MSGANNEXACK ) == 0 )
        {
            SetFlag(CMD_FLAG_MSG_ANNEX_ACK, CMD_FLAG_MSG_ANNEX_ACK);
            i32Ret = sprintf( &pszMsg[i32Index], "%s", pEl->Attribute(FIELD_ACK) );
            ASSERT_(i32Ret > 0);
            i32Index += i32Ret;
        }
        else if ( _stricmp(pszValue, FIELD_MSGLONG ) == 0 )
        {
            SetFlag(CMD_FLAG_MSG_LONG, CMD_FLAG_MSG_LONG);
            CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
            FILE_META* psFileMeta  = (FILE_META*)&pszMsg[i32Index];
            SetFileMetaData(pEl, *psFileMeta, i32Index);
//             pszValue = pEl->Attribute(FIELD_KEY);
//             if ( pszValue )
//             {
//                 eIMString szData;
//                 FILE_META* psFileMeta  = (FILE_META*)&pszMsg[i32Index];
//                 psFileMeta->dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
//                 psFileMeta->dwFileSize = htonl(psFileMeta->dwFileSize);
//                 strncpy(psFileMeta->aszURL, pszValue, COUNT_OF_ARRAY_(psFileMeta->aszURL));
//                 strncpy(psFileMeta->aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(psFileMeta->aszFileName));
// 
//                 i32Index += sizeof( FILE_META );
//             }
        }
        else if ( _stricmp(pszValue, FIELD_MSGP2P ) == 0 )
        {
            eIMString szData;
            SetFlag(CMD_FLAG_MSG_P2P, CMD_FLAG_MSG_P2P);
            CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
            i32Ret = sprintf( &pszMsg[i32Index], "%s%s|%s|%s", pEl->Attribute( FIELD_SIZE ),pEl->Attribute( FIELD_KEY ),
                pEl->Attribute( FIELD_TOKEN ), pEl->Attribute( FIELD_NAME ));
            i32Index += i32Ret;
        }
        else if ( _stricmp(pszValue, FIELD_MSGVOICE ) == 0 )
        {	
            SetFlag(CMD_FLAG_MSG_VOICE, CMD_FLAG_MSG_VOICE);
            CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
            FILE_META* psFileMeta  = (FILE_META*)pszMsg;
            SetFileMetaData(pEl, *psFileMeta, i32Index);
//             pszValue = pEl->Attribute(FIELD_KEY);
//             if ( pszValue )
//             {
//                 eIMString szData;
//                 i32Index			   = sizeof( FILE_META );
//                 FILE_META* psFileMeta  = (FILE_META*)pszMsg;
//                 psFileMeta->dwFileSize = strtoul(pEl->Attribute(FIELD_SIZE), NULL, 0);
//                 psFileMeta->dwFileSize = htonl(psFileMeta->dwFileSize);
//                 strncpy(psFileMeta->aszURL, pszValue, COUNT_OF_ARRAY_(psFileMeta->aszURL));
//                 strncpy(psFileMeta->aszFileName, pEl->Attribute(FIELD_NAME), COUNT_OF_ARRAY_(psFileMeta->aszFileName));
//             }
        }
        else if ( _stricmp(pszValue, FIELD_MSGVIDEO ) == 0 )
        {	
            SetFlag(CMD_FLAG_MSG_VIDEO, CMD_FLAG_MSG_VIDEO);
            CheckAndSetFileAttribute(pEl, pEl->Attribute(FIELD_FILE_PATH), NULL);
            FILE_META* psFileMeta  = (FILE_META*)pszMsg;
            SetFileMetaData(pEl, *psFileMeta, i32Index);
        }
        else
        {
            ASSERT_(FALSE);
        }
    }
    u16Size = (WORD)i32Index + 1;
    return pszMsg;
}

int ChangeMsgType2CollectType(const eIMString& szMsgUI)
{
    if(szMsgUI.find(_T(FIELD_MSGANNEX)) !=  eIMString::npos
        || szMsgUI.find(_T(FIELD_MSGVIDEO)) !=  eIMString::npos)
    {
        return eCOLLECT_TYPE_FILE;
    }
    if( szMsgUI.find(_T(FIELD_MSGVOICE)) !=  eIMString::npos)
    {
        return eCOLLECT_TYPE_VIOCE;
    }

    if( szMsgUI.find(_T(FIELD_MSGIMAGE)) !=  eIMString::npos)
    {
        if(szMsgUI.find(_T(FIELD_MSGTEXT)) !=  eIMString::npos)
        {
            return eCOLLECT_TYPE_IMAGE_TEXT;
        }
        return eCOLLECT_TYPE_IMAGE;
    }

	if (szMsgUI.find(_T("<robotResponse>")) != eIMString::npos)
	{
		int iType = 0;
		ParseRobotMsgType(szMsgUI, iType);
		return iType;
	}

    return (int)eCOLLECT_TYPE_TEXT;
}

eIMString GetName(const TiXmlElement *pElement, LPCTSTR szDefault)
{
	const char* pValue = pElement->Value();
	eIMString sVal;
	if(!pValue || 0==strlen(pValue))
	{
		return sVal = szDefault;
	}
	else
	{
		eIMUTF8ToTChar(pValue, sVal);
	}

	return sVal;
}

void GetNodeText(const TiXmlElement* pElementchild, eIMString& strText)
{
	strText = _T("");
	const char* pValue = pElementchild->GetText();
	if(pValue)
	{
		::eIMMByte2TChar(pValue, strText );
	}
}

void ParseRobotMsgType(const eIMString& strRobotXml, int& iType)
{
	eIMString sValue = strRobotXml;
	eIMString strRobotBegin(_T("<robotResponse>"));
	eIMString strRobotEnd(_T("</robotResponse>"));
	int iFirst = sValue.find(strRobotBegin);
	int iLast = sValue.find(strRobotEnd);
	sValue = sValue.substr(iFirst, iLast + strRobotEnd.length());
// 	sValue.replace(0, _T("\r\n"), _T("&#xD;") );
// 	sValue.replace(0,_T("\n"), _T("&#xD;"));
// 	sValue.replace(0,_T("&#xD;&#xD;"), _T("&#xD;"));
	TiXmlDocument xmlDoc;
	eIMStringA	  szXmlMsgA;
	if ( 0 ==::eIMTChar2MByte(sValue.c_str(), szXmlMsgA) )
		return;
	xmlDoc.Parse(szXmlMsgA.c_str());
	if ( xmlDoc.RootElement() == NULL ||
		_stricmp(xmlDoc.RootElement()->Value(), "robotResponse") != 0 )
		return ; 
	TiXmlHandle h(xmlDoc.RootElement());

	TiXmlElement* pNode = h.FirstChild().ToElement();
	int iReatedNum = 1;
	BOOL bCommanKey = FALSE;
	while(pNode)
	{
		eIMString szNodeName = GetName(pNode, _T(""));
		if(0 == szNodeName.compare(_T("content")) 
			||  0 == szNodeName.compare(_T("relatedQuestions")))
		{
			iType = (int)eCOLLECT_TYPE_TEXT;
		}
		else if(0 == szNodeName.compare(_T("commands")))
		{
			TiXmlElement* pType = pNode->FirstChildElement("name");
			if(pType)
			{
				eIMString strValue;
				GetNodeText(pType, strValue);
				if(0 == strValue.compare(_T("imgtxtmsg"))
					|| 0 == strValue.compare(_T("wiki")))
				{
					iType = (int)eCOLLECT_TYPE_IMAGE_TEXT;
					return;
				}
				if( 0 == strValue.compare(_T("musicmsg")) )
				{
					iType = (int)eCOLLECT_TYPE_VIOCE;
					return;
				}
				if( 0 == strValue.compare(_T("videomsg")))
				{
					iType = (int)eCOLLECT_TYPE_FILE;
					return;
				}
				else if( 0 == strValue.compare(_T("imgmsg"))  )
				{
					iType = (int)eCOLLECT_TYPE_IMAGE;
					return;
				}
			}
		}
		pNode = pNode->NextSiblingElement();
	}
}

BOOL C_CmdSendCollectModifyMsg::SetAttribute(UINT64 U64AttrId, TCHAR* pszValue)
{
    const char* pszQfid = NULL;
    QFID		Qfid = 0;
    DWORD		dwCountWaiting = 0;
    TiXmlHandle h(m_xmlDoc.RootElement());
    TiXmlElement* pMsgEl = h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
    ASSERT_(pMsgEl);
    ASSERT_(pszValue);
    CHECK_NULL_RET_(pMsgEl, FALSE);
    CHECK_NULL_RET_(pMsgEl, FALSE);

    for( ;pMsgEl; pMsgEl = pMsgEl->NextSiblingElement() )
    {
        pszQfid = pMsgEl->Attribute(FIELD_FID);
        if (pszQfid == NULL)
            continue;

        Qfid = _strtoui64(pszQfid, NULL, 0);
        if(Qfid == U64AttrId)
        {
            eIMStringA szValueA;
            ::eIMTChar2UTF8(pszValue, szValueA);
            const char* pszValue = pMsgEl->Attribute(FIELD_FILE_PATH);
            if(pszValue)
            {
                eIMString szSaveFilePath;
                ::eIMUTF8ToTChar(pszValue, szSaveFilePath);
                //szSaveFilePath = _T("C:\\Users\\Administrator\\Documents\\eIM_Files\\232820\\Collect\\中文测试.log");
                eIMStringA szSaveFilePathA;
                ::eIMTChar2UTF8(szSaveFilePath.c_str(), szSaveFilePathA);
                pMsgEl->SetAttribute(FIELD_FILE_PATH, szSaveFilePathA.c_str());
                pszValue = pMsgEl->Attribute(FIELD_FILE_PATH);
            }
            
            pMsgEl->SetAttribute(FIELD_KEY, szValueA.c_str());
        }
        else if (pMsgEl->Attribute(FIELD_KEY) == NULL)
            dwCountWaiting++;
    }

    if ( dwCountWaiting == 0 )
    {
        SetFlag(0, CMD_FLAG_MSG_WAIT_FILE);
        SetFlag(CMD_FLAG_THREAD_SEND, CMD_FLAG_THREAD_SEND);
        SetFlag(CMD_FLAG_THREAD_DB, CMD_FLAG_THREAD_DB);
        SetFlag(0, CMD_FLAG_THREAD_UI);
        GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
        TiXmlHandle h(m_xmlDoc.RootElement());
        m_sData.stFavoriteInfo.dwMsgSize = MSG_MAXLEN;
        Xml2Msg(h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement(), m_sData.stFavoriteInfo.strMsgContent, m_sData.stFavoriteInfo.dwMsgSize);
        ConvertToXml(*h.FirstChildElement(FIELD_MSGUI).ToElement(), m_szMsgUI);
        ConvertToXml(*m_xmlDoc.RootElement(), m_szXml);
    }

    return TRUE;
}

REGISTER_CMDCLASS(C_CmdSendCollectModifyMsgAck, CMD_FAVORITE_MODIFY_ACK);
int	C_CmdSendCollectModifyMsgAck::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
{
    if (FAILED(m_i32Result))
    {
        C_UIWaitOverMgr::GetObject().DelSetFailedItem(eWaitType_Msg,m_sData.stFavoriteBatch.ddwMsgID[0], this);
        for (int i=0; i< m_sData.stFavoriteBatch.wNum; ++i)
        {
            EIMLOGGER_ERROR_( pILog, _T("MsgId: %llu %s"), m_sData.stFavoriteBatch.ddwMsgID[i], m_szXml.c_str());
        } 
    }
    else
    {
        for (int i=0; i< m_sData.stFavoriteBatch.wNum; ++i)
        {
            C_UIWaitOverMgr::GetObject().DelWaitOverItem(eWaitType_Msg,m_sData.stFavoriteBatch.ddwMsgID[i]);
            EIMLOGGER_DEBUG_( pILog, _T("MsgId: %llu"), m_sData.stFavoriteBatch.ddwMsgID[i]);
        }       
    }

    return EIMERR_NO_ERROR;
}
int	C_CmdSendCollectModifyMsgAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
    return EIMERR_NO_ERROR;
}
int	C_CmdSendCollectModifyMsgAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
    SAFE_SEND_EVENT_(pIEMgr, EVENT_UPDATE_COLLECT_WND, NULL);
    return EIMERR_NO_ERROR;
}


REGISTER_CMDCLASS(C_CmdCollectNotice, CMD_FAVORITE_NOTICE);
int	C_CmdCollectNotice::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
{
    return EIMERR_NO_ERROR;
}
int	C_CmdCollectNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
	C_eIMMsgCollectMgr MsgCollectMgr;
	MsgCollectMgr.Init(pIDb);
    switch(m_sData.cOperType)
    {
    case 1:
    case 2:
        {
            S_CollectMsg sMsg;
            ToXml(pIDb);
            sMsg.m_collectID = m_sData.stFavoriteInfo.ddwMsgID;
            sMsg.m_dwTime = m_sData.stFavoriteInfo.dwCollTime;
            sMsg.m_isGroup = m_sData.stFavoriteInfo.cIsGroup;
            sMsg.m_sendid = m_sData.stFavoriteInfo.dwSender;
            sMsg.m_eType = (E_CollectType)ChangeMsgType2CollectType(m_szMsgUI);
            eIMString strTitle;
            if(sMsg.m_isGroup)
            {
                S_SessionInfo rInfo;
                C_eIMSessionMgr smgr;
                smgr.Init(pIDb);
                QSID qsid = GroupId2Qsid((char*)m_sData.stFavoriteInfo.strGroupID);
                if(smgr.Get(qsid, &rInfo))
                    strTitle = rInfo.szTitle;
                sMsg.m_Sid = qsid;
            }
            else
            {
                C_eIMContacts&	 obj = C_eIMContacts::GetObject();
                strTitle  = obj.GetEmpName(sMsg.m_sendid);
                sMsg.m_Sid = 0;
            }
            MsgCollectMgr.Set(sMsg, m_szMsgUI.c_str(), strTitle.c_str());
        }
        break;
    case 3:
        {
            for (int i = 0; i< m_sData.stFavoriteBatch.wNum; ++i)
            {
                MsgCollectMgr.Delete(m_sData.stFavoriteBatch.ddwMsgID[i]);
            }
        }
        break;
    }
    
    return EIMERR_NO_ERROR;
}
int	C_CmdCollectNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
    if(m_sData.wCurNum == m_sData.wTotalNum)
    {
        SAFE_SEND_EVENT_(pIEMgr, EVENT_UPDATE_COLLECT_WND, NULL);
    }

    return EIMERR_NO_ERROR;
}

const TCHAR* C_CmdCollectNotice::ToXml(I_SQLite3* pIDb)
{
    if(m_sData.cOperType == 3)
    {
        return NULL;
    }
 	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_NULL);
	C_eIMEngine& eng = Eng;

	char		 szBuf[CONVERT_TO_STRING_SIZE];
	eIMString	 szValue;
	eIMStringA	 szValueA;
	TiXmlElement MsgEl( PROTOCOL_ELEMENT_CMD);
	TiXmlElement MsgUiEl(FIELD_MSGUI);

	MsgEl.SetAttribute( FIELD_ID,		 CMD_FAVORITE_NOTICE);
    
    MsgEl.SetAttribute( FIELD_SENDER_ID, (int)m_sData.stFavoriteInfo.dwSender);
    MsgEl.SetAttribute( FIELD_MSG_ID,    GET_QSID_ASTR_(m_sData.stFavoriteInfo.ddwMsgID, szBuf) );
    MsgEl.SetAttribute( FIELD_IS_GROUP,  m_sData.stFavoriteInfo.cIsGroup	);
    MsgEl.SetAttribute(FIELD_COLLECT_TIME, m_sData.stFavoriteInfo.dwCollTime);
	
    MsgUiEl.SetAttribute( FIELD_TYPE,      m_sData.stFavoriteInfo.wMsgType	);
    MsgUiEl.SetAttribute( FIELD_SENDER_ID, (int)m_sData.stFavoriteInfo.dwSender);
    MsgUiEl.SetAttribute( FIELD_SEND_TIME, (int)m_sData.stFavoriteInfo.dwSendTime);
    MsgUiEl.SetAttribute(FIELD_IS_SEND, 0);
	if ( m_sData.stFavoriteInfo.cIsGroup == 1 )
	{
		QSID qSid  = GroupId2Qsid((char*)m_sData.stFavoriteInfo.strGroupID);
        SET_GROUP_ID_ATTR_(m_sData.stFavoriteInfo.strGroupID, MsgEl);
		MsgUiEl.SetAttribute( FIELD_TYPE, eSESSION_TYPE_MULTI);
	}
	else if ( m_sData.stFavoriteInfo.cIsGroup == 2 )
	{
		QSID qSid  = GroupId2Qsid((char*)m_sData.stFavoriteInfo.strGroupID);
		SET_GROUP_ID_ATTR_(m_sData.stFavoriteInfo.strGroupID, MsgEl);
		MsgUiEl.SetAttribute( FIELD_TYPE, eSESSION_TYPE_RGROUP);
	}
	else
    {
		MsgEl.SetAttribute( FIELD_GROUP_ID, GET_QSID_ASTR_(m_sData.stFavoriteInfo.strGroupID, szBuf) );
		MsgUiEl.SetAttribute( FIELD_TYPE, eSESSION_TYPE_SINGLE);
		memcpy(m_sData.stFavoriteInfo.strGroupID, szBuf, sizeof(m_sData.stFavoriteInfo.strGroupID));
    }
	TiXmlElement e_(FIELD_MESSAGE);
	if (!Msg2Element(e_, eng) )
	{
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Msg2Element error QMID: %llu"), m_sData.stFavoriteInfo.ddwMsgID);
		return NULL;
	}

	//objEgn.AddFontElement(m_sData.stFavoriteInfo.wMsgType, FALSE, &MsgUiEl);
	MsgUiEl.InsertEndChild(e_);
	MsgEl.InsertEndChild(MsgUiEl);

	m_xmlDoc.Clear();
	m_xmlDoc.InsertEndChild(MsgEl);

	ConvertToXml( MsgEl, m_szXml );
	ConvertToXml( MsgUiEl, m_szMsgUI);

	return m_szXml.c_str();
}

const BOOL C_CmdCollectNotice::Msg2Element(TiXmlElement& MsgElement, C_eIMEngine& objEgn)
{
    GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);
    GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(RET_FALSE);

#define GET_FILE_PATH_(FILE, DIR, KEY) \
    fid = hpi.GetQfid(KEY); \
    ::eIMUTF8ToTChar(FILE, szValue); \
    ::eIMGetFullPath(PATH_TOKEN_LOGINID_DOC, DIR, szValue.c_str(), szValue,FALSE); \
    ::eIMTChar2UTF8(szValue.c_str(), szValueA);

#define GET_FILE_PATH_NAME(DIR) \
    do{ pos = DIR.rfind("/");\
    if (pos == eIMStringA::npos) {\
    pos = DIR.rfind("\\");\
    if (pos == eIMStringA::npos) {szFileNameA = DIR; break;} }\
    pos += 1;\
    szFileNameA = DIR.substr(pos, DIR.length() - pos);break;}while(1)

    char			szBuf[CONVERT_TO_STRING_SIZE] = { 0 };
    QFID			fid;
    eIMString		szValue;
    eIMStringA		szValueA;
    eIMStringA		szFileNameA;
    unsigned long pos = 0;
    TiXmlElement    MsgFontElement( FIELD_MSGFONT );
    switch ( m_sData.stFavoriteInfo.wMsgType )	// ERCR_TYPE
    {
    case eIM_MSGTYPE_TEXT: 
        {
            MsgFontElement.SetAttribute( FIELD_ID,   m_sData.stFavoriteInfo.strMsgContent[0] );
            MsgFontElement.SetAttribute( FIELD_SIZE, m_sData.stFavoriteInfo.strMsgContent[1] < 6 ? 12 : m_sData.stFavoriteInfo.strMsgContent[1] );
            MsgFontElement.SetAttribute( FIELD_MODE, m_sData.stFavoriteInfo.strMsgContent[2] );
            MsgFontElement.SetAttribute( FIELD_COLOR,(int)RGB(m_sData.stFavoriteInfo.strMsgContent[3], m_sData.stFavoriteInfo.strMsgContent[4], m_sData.stFavoriteInfo.strMsgContent[5] ));
            MsgElement.InsertEndChild(MsgFontElement);

            BOOL		 bSubType = FALSE;
            const char* pszMsg	 = &m_sData.stFavoriteInfo.strMsgContent[10];
            const char* pRobotStart = strstr(pszMsg, "<robotResponse>");
            const char* pRobotEnd = strstr(pszMsg, "</robotResponse>");
            eIMStringA szRobotContent;
            if(pRobotStart && pRobotEnd) 
            {
                SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
                //szValueA.assign( pRobotStart, pRobotEnd - pRobotStart  + strlen("</robotResponse>") );
                szValueA.assign(pszMsg, m_sData.stFavoriteInfo.dwMsgSize - 10);
                TiXmlElement TextElement( FIELD_MSGTEXT );
                TiXmlText    Text(szValueA.c_str());
                Text.SetCDATA(true);

                TextElement.InsertEndChild(Text);
                MsgElement.InsertEndChild( TextElement );
            }
            else
            {
                const char* pszStart = strchr(pszMsg, '[');
                const char* pszEnd   = pszStart ? strchr(pszStart, ']') : NULL;
                const char* pszChar  = NULL;
                for ( ; pszStart && pszEnd; )
                {
                    if ( pszEnd < pszStart + 3)
                    {
                        pszStart = strchr(pszEnd + 1, '[');
                        pszEnd   = pszStart ? strchr(pszStart + 1, ']') : NULL;
                        continue;
                    }

                    szValueA.assign( pszStart + 1, pszEnd - pszStart - 1 );
                    TiXmlElement SubEl( FIELD_MSGEMOT );
                    if ( pszStart[1] == _T('/') )		
                    {
                        ::eIMUTF8ToTChar(szValueA.c_str(), szValue);
                        if ( pIEmot->IsExistEmot(szValue.c_str()) )
                        {
                            bSubType = TRUE;
                            SetFlag(CMD_FLAG_MSG_EMOT, CMD_FLAG_MSG_EMOT);
                            SubEl.SetAttribute( FIELD_NAME, szValueA.c_str() );
                        }
                        else
                        {
                            pszStart = strchr(pszStart + 1, '[');
                            pszEnd   = pszStart ? strchr(pszStart + 1, ']') : NULL;
                            continue;
                        }
                    }
                    else if ( pszStart[1] == _T('#') && IsFileKey(szValueA.substr(1).c_str()) )
                    {
                        szValueA.erase( 0, 1);
                        bSubType = TRUE;
                        SetFlag(CMD_FLAG_MSG_IMAGE, CMD_FLAG_MSG_IMAGE);
                        szFileNameA = szValueA.substr(0, szValueA.rfind('.'));
                        SubEl.SetValue( FIELD_MSGIMAGE );
                        SubEl.SetAttribute( FIELD_KEY, szFileNameA.c_str() );
                        GET_FILE_PATH_(szValueA.c_str(), PATH_TYPE_APPDOC_IMAGE, szValueA.c_str());
                        SubEl.SetAttribute( FIELD_FID, GET_QSID_ASTR_(fid, szBuf));
                        SubEl.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
                        CheckAndSetFileAttribute(&SubEl, szValueA.c_str(), SubEl.Attribute(FIELD_KEY));
                    }
                    else
                    {
                        pszStart = strchr(pszStart + 1, '[');
                        pszEnd   = pszStart ? strchr(pszStart + 1, ']') : NULL;
                        continue;
                    }

                    if ( bSubType )
                    { 
                        if ( pszMsg != pszStart )
                        {
                            SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
                            szValueA.assign( pszMsg, pszStart - pszMsg );
                            TiXmlElement TextElement( FIELD_MSGTEXT );
                            TiXmlText    Text(szValueA.c_str());
                            Text.SetCDATA(true);

                            TextElement.InsertEndChild(Text);
                            MsgElement.InsertEndChild( TextElement );
                        }

                        MsgElement.InsertEndChild( SubEl );	
                    }
                    else
                    {
                        SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
                        szValueA.assign( pszMsg, pszEnd - pszMsg );
                        TiXmlElement TextElement( FIELD_MSGTEXT );
                        TiXmlText    Text(szValueA.c_str());
                        Text.SetCDATA(true);
                        TextElement.InsertEndChild(Text);
                        MsgElement.InsertEndChild( TextElement );
                    }

                    pszMsg = pszEnd + 1;
                    if (bSubType)
                    {
                        bSubType = FALSE;
                        pszStart = strchr(pszMsg, '[');
                        pszEnd   = strchr(pszMsg, ']');	
                    }
                } 
                if ( pszMsg && *pszMsg )
                {
                    SetFlag(CMD_FLAG_MSG_TEXT, CMD_FLAG_MSG_TEXT);
                    TiXmlElement TextElement( FIELD_MSGTEXT );
                    TiXmlText    Text(pszMsg);
                    Text.SetCDATA(true);
                    TextElement.InsertEndChild(Text);
                    MsgElement.InsertEndChild( TextElement );
                }
            }				
        }
        break;
    case eIM_MSGTYPE_IMG: 
        {	
            SetFlag(CMD_FLAG_MSG_IMAGE, CMD_FLAG_MSG_IMAGE);
            TiXmlElement MsgImage( FIELD_MSGIMAGE );
            FILE_META* psFileMeta = (FILE_META*)m_sData.stFavoriteInfo.strMsgContent;
            GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);

            MsgImage.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
            psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
            CheckAndSetFileAttribute(&MsgImage, szValueA.c_str(), MsgImage.Attribute(FIELD_KEY));
            MsgImage.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
            MsgImage.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
            MsgImage.SetAttribute(FIELD_KEY, psFileMeta->aszURL);
            MsgImage.SetAttribute(FIELD_NAME, szValueA.c_str());

            //objEgn.AddFontElement(m_sData.stFavoriteInfo.wMsgType, FALSE, &MsgElement);
            MsgElement.InsertEndChild(MsgImage);
        }
        break;
    case eIM_MSGTYPE_VOICE:	
        {
            SetFlag(CMD_FLAG_MSG_VOICE, CMD_FLAG_MSG_VOICE);
            //objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

            TiXmlElement MsgVoice( FIELD_MSGVOICE );
            FILE_META* psFileMeta = (FILE_META*)m_sData.stFavoriteInfo.strMsgContent;

            psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
            GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
            MsgVoice.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
            GET_FILE_PATH_NAME(szValueA);
            CheckAndSetFileAttribute(&MsgVoice, szValueA.c_str(), psFileMeta->aszURL);
            MsgVoice.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
            MsgVoice.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
            MsgVoice.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
            MsgVoice.SetAttribute( FIELD_NAME, szFileNameA.c_str() );
            MsgElement.InsertEndChild(MsgVoice);	
        }
        break;
    case eIM_MSGTYPE_VIDEO:
        {
            SetFlag(CMD_FLAG_MSG_VIDEO, CMD_FLAG_MSG_VIDEO);
            //objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

            TiXmlElement MsgVideo( FIELD_MSGVIDEO );
            FILE_META* psFileMeta = (FILE_META*)m_sData.stFavoriteInfo.strMsgContent;

            psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
            GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
            MsgVideo.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
            GET_FILE_PATH_NAME(szValueA);
            CheckAndSetFileAttribute(&MsgVideo, szValueA.c_str(), psFileMeta->aszURL);

            MsgVideo.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
            MsgVideo.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
            MsgVideo.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
            MsgVideo.SetAttribute( FIELD_NAME, szFileNameA.c_str() );

            MsgElement.InsertEndChild(MsgVideo);
        }
        break;
    case eIM_MSGTYPE_FILE: 
        {	
            SetFlag(CMD_FLAG_MSG_ANNEX, CMD_FLAG_MSG_ANNEX);
            //objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

            TiXmlElement MsgAnnex( FIELD_MSGANNEX );
            FILE_META* psFileMeta = (FILE_META*)m_sData.stFavoriteInfo.strMsgContent;

            psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
            GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_APPDOC_FILERECV, psFileMeta->aszURL);
            MsgAnnex.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
            GET_FILE_PATH_NAME(szValueA);
            CheckAndSetFileAttribute(&MsgAnnex, szValueA.c_str(), psFileMeta->aszURL);

            MsgAnnex.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
            MsgAnnex.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
            MsgAnnex.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
            MsgAnnex.SetAttribute( FIELD_NAME, szFileNameA.c_str() );

            MsgElement.InsertEndChild(MsgAnnex);
        }
        break;
    case eIM_MSGTYPE_P2P: 
        {
            SetFlag(CMD_FLAG_MSG_P2P, CMD_FLAG_MSG_P2P);
            m_i32Result  = EIMERR_BAD_CMD;  
            break;
        }
        break;
    case eIM_MSGTYPE_FILERECVED: 	
        {
            SetFlag(CMD_FLAG_MSG_ANNEX_ACK, CMD_FLAG_MSG_ANNEX_ACK);

            TiXmlElement MsgAnnexAck( FIELD_MSGANNEXACK );
            MsgAnnexAck.SetAttribute( FIELD_ACK, m_sData.stFavoriteInfo.strMsgContent);
            MsgElement.InsertEndChild(MsgAnnexAck);
        }
        break;
    case eIM_MSGTYPE_LONGTEXT:
        {
            SetFlag(CMD_FLAG_MSG_LONG, CMD_FLAG_MSG_LONG);		
            //objEgn.AddFontElement(m_sMsg.eType, FALSE, &MsgElement);

            SetFlag(CMD_FLAG_MSG_ANNEX, CMD_FLAG_MSG_ANNEX);
            TiXmlElement MsgLong( FIELD_MSGLONG );
            FILE_META* psFileMeta = (FILE_META*)&m_sData.stFavoriteInfo.strMsgContent[10];

            psFileMeta->dwFileSize = ntohl(psFileMeta->dwFileSize);
            GET_FILE_PATH_(psFileMeta->aszFileName, PATH_TYPE_MSGLONG, psFileMeta->aszURL);
            MsgLong.SetAttribute( FIELD_FILE_PATH, szValueA.c_str());
            GET_FILE_PATH_NAME(szValueA);
            CheckAndSetFileAttribute(&MsgLong, szValueA.c_str(), psFileMeta->aszURL);

            MsgLong.SetAttribute( FIELD_FID,  GET_QSID_ASTR_(fid, szBuf));
            MsgLong.SetAttribute( FIELD_SIZE, GET_QSID_ASTR_(psFileMeta->dwFileSize, szBuf) );
            MsgLong.SetAttribute( FIELD_KEY,  psFileMeta->aszURL );
            MsgLong.SetAttribute( FIELD_NAME, szFileNameA.c_str() );
            
            eIMString szKey;
            ::eIMUTF8ToTChar(psFileMeta->aszURL, szKey);
            if ( WaitMsgLong(fid, szValue.c_str(), szKey.c_str()) || 
                WaitMsgLong(fid, szValue.c_str(), szKey.c_str()) )	// Try again when failed
            {
                FILE* pflRead = _tfopen(szValue.c_str(), _T("rb, ccs=UTF-8"));	ASSERT_(pflRead);
                if ( pflRead )
                {
                    fseek(pflRead, 0, SEEK_END);
                    const int i32Size = ftell(pflRead);
                    fseek(pflRead, 0, SEEK_SET);
                    char* pszBuf = (char*)_alloca(i32Size);

                    TRACE_(_T("Read MsgLong file[%s], size: %d %s"), szValue.c_str(), i32Size, pszBuf ? _T("Succeeded") : _T("Failed"));
                    if ( pszBuf && i32Size )
                    {
                        fread(pszBuf, sizeof(char), i32Size, pflRead);

                        TiXmlText Text(IS_BOM_UTF8(pszBuf) ? &pszBuf[3] : pszBuf);
                        Text.SetCDATA(true);
                        MsgLong.InsertEndChild(Text);
                    }

                    fclose(pflRead);
                }
            }

            if ( !MsgLong.GetText() )
            {
                TiXmlText Text(&m_sData.stFavoriteInfo.strMsgContent[sizeof(FILE_META) + 10]);
                Text.SetCDATA(true);
                MsgLong.InsertEndChild(Text);
            }

            MsgElement.InsertEndChild(MsgLong);	// Add MsgLong
        }
        break;
    default:
        break;
    }

    return TRUE;
}

REGISTER_CMDCLASS(C_CmdColleceSyncReq, CMD_FAVORITE_SYNC_REQ);
int	C_CmdColleceSyncReq::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
{ 
    CONVERT_TO_ICA_RET_(pvCa);
    m_i32Result = pICa->SendFavoriteSync(m_sData );
    return EIMERR_NO_ERROR;
}
int	C_CmdColleceSyncReq::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
    return EIMERR_NO_ERROR;
}
int	C_CmdColleceSyncReq::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
    return EIMERR_NO_ERROR;
}

REGISTER_CMDCLASS(C_CmdColleceSyncAck, CMD_FAVORITE_SYNC_ACK);
int	C_CmdColleceSyncAck::DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort)
{
    return EIMERR_NO_ERROR;
}
int	C_CmdColleceSyncAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
    if(m_sData.wTotalNum > 0 && 0 == m_sData.dwResult)
    {
		C_eIMMsgCollectMgr MsgCollectMgr;
		MsgCollectMgr.Init(pIDb);
        C_eIMEngine& eng = Eng;
        MsgCollectMgr.SetCollectTime(eng.GetPurview(GET_LOGIN_QEID) , eng.GetTimeNow());
    }
    return EIMERR_NO_ERROR;
}
int	C_CmdColleceSyncAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
    
    return EIMERR_NO_ERROR;
}

REGISTER_CMDCLASS(C_CmdGetDeptShowConfigReq, eCMD_DEPTSHOWCONFIG_REQ);
int	C_CmdGetDeptShowConfigReq::DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort)
{
	CONVERT_TO_ICA_RET_(pvCa);
	m_i32Result = pICa->GetDeptShowConfigReq(&m_sData);
	if ( SUCCEEDED(m_i32Result) )
	{
		EIMLOGGER_DEBUG_( pILog, _T("UserID:%u, Timestamps:%u"), m_sData.dwUserID, m_sData.dwTimestamps );
	
		SetBusinessTypeId(eCMD_DEPTSHOWCONFIG_REQ);
		C_GetDeptShowConfigOverItem* pBaseWaitItem = new C_GetDeptShowConfigOverItem();
		pBaseWaitItem->SetData(this);
		pBaseWaitItem->SetStartTime(GetTickCount());
		C_UIWaitOverMgr::GetObject().AddWaitOverItem(pBaseWaitItem);
	}
	else
		EIMLOGGER_ERROR_( pILog, _T("UserID:%u, Timestamps:%u"), m_sData.dwUserID, m_sData.dwTimestamps );

	return EIMERR_NO_ERROR;
}

int C_CmdGetDeptShowConfigReq::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
	if ( FAILED(m_i32Result) )
	{	// Failed and continue to load contacts
		Eng.SetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO, -2);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_DEPT_EMPS_END, (void*)0);
	}

	return m_i32Result;
}

REGISTER_CMDCLASS(C_CmdGetDeptShowConfigAck, eCMD_DEPTSHOWCONFIG_ACK);
static VectDeptShowInfo s_vectDeptShowInfo;
int	C_CmdGetDeptShowConfigAck::DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort)
{
	 CONVERT_TO_ICA_RET_(pvCa);
	UINT32 u32StartPos = 0;
	SINGLEDEPTSHOWLEVEL sShowLevel = { 0 };
	
	C_UIWaitOverMgr::GetObject().DelWaitOverItem(eWaitType_Normal, eCMD_DEPTSHOWCONFIG_REQ);	// Del when succeed;  the failed where troggle timeout
	if ( pILog && pILog->GetLogLevel() == eLLDATA )
	{
		EIMLOGGER_DATA_(pILog, (BYTE*)m_sData.strPacketBuff, m_sData.wPacketLen, _T("PacketLen:%u, UserID:%u, UpdateFlag:%u, DefaultShowLevel:%u, CurrPage:%u, CurrNum:%u"),
			m_sData.wPacketLen, m_sData.dwUserID, m_sData.cUpdateFlag, m_sData.cDefaultShowLevel, m_sData.wCurrPage, m_sData.wCurrNum);
	}
	else
	{
		EIMLOGGER_DEBUG_(pILog, _T("PacketLen:%u, UserID:%u, UpdateFlag:%u, DefaultShowLevel:%u, CurrPage:%u, CurrNum:%u"),
			m_sData.wPacketLen, m_sData.dwUserID, m_sData.cUpdateFlag, m_sData.cDefaultShowLevel, m_sData.wCurrPage, m_sData.wCurrNum);
	}

	while(SUCCEEDED(m_i32Result = pICa->ParseDeptShowConfig(m_sData.strPacketBuff, &u32StartPos, &sShowLevel)) )
	{
		PARSE_FINISHED_BREAK_(m_i32Result);
		s_vectDeptShowInfo.push_back(sShowLevel);
	}

	return EIMERR_NO_ERROR;
}

int	C_CmdGetDeptShowConfigAck::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
{
	if ( (m_sData.wCurrPage == 0 && m_sData.cUpdateFlag == 1) )
	{	// Finished
		C_eIMContactsMgr cmgr;

		cmgr.Init(pIDb);
 		m_i32Result = cmgr.Set(s_vectDeptShowInfo, m_sData.cDefaultShowLevel);
		s_vectUserDept.clear();
	}
		
	return EIMERR_NO_ERROR;
}

int C_CmdGetDeptShowConfigAck::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
{
	if ( (m_sData.wCurrPage == 0 && m_sData.cUpdateFlag == 1) || m_sData.cUpdateFlag == 0 )
	{
		Eng.SetAttributeInt(IME_GET_DEPTUSER_VISABLE_INFO, -1);	// Finished
		SAFE_SEND_EVENT_(pIEMgr, eCMD_DEPTSHOWCONFIG_ACK, (void*)&m_sData);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_GET_DEPT_EMPS_END, (void*)0);
	}

	return EIMERR_NO_ERROR;
}
////////////////////////会议基本信息通知////////////////////////////////
//REGISTER_CMDCLASS(C_CmdMeetingBasicInfoNotic, eCMD_MEETING_INFO_NOTICE);
//int C_CmdMeetingBasicInfoNotic::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
//{
//    return m_i32Result;
//}
//
//int C_CmdMeetingBasicInfoNotic::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
//{
//	EIMLOGGER_INFO_(pILog,_T("BasicInfoNotic"));
//	C_MeetingMgr meetmgr;
//	m_i32Result = meetmgr.Set(&(m_sData.sConfBasicInfo));
//	return m_i32Result;
//}
//
//int C_CmdMeetingBasicInfoNotic::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort /* = NULL */)
//{
//	if (m_i32Result)
//	{
//		//通知UI刷新主界面
//		SAFE_SEND_EVENT_(pIEMgr, EVENT_MEETING_INFO_NOTICE, (void*)&m_sData.sConfBasicInfo);
//	}
//    return EIMERR_NO_ERROR;
//}
//
///////////////////////////会议备注提醒//////////////////////////////
//REGISTER_CMDCLASS(C_CmdMeetingRemarkNotice, eCMD_MEETING_REMARKS_NOTICE);
//int C_CmdMeetingRemarkNotice::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort /* = NULL */)
//{
//	if (m_i32Result)
//	{
//		//通知UI刷新主界面
//		SAFE_SEND_EVENT_(pIEMgr, EVENT_MEETING_REMARK_NOTICE, (void*)&m_sData);
//	}
//	return EIMERR_NO_ERROR;
//}
//
//int C_CmdMeetingRemarkNotice::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
//{
//	EIMLOGGER_INFO_(pILog,_T("MeetingRemarkNotice"));
//	C_MeetingMgr meetmgr;
//	m_i32Result = meetmgr.Set(&(m_sData));
//	return m_i32Result;
//}
//
//int C_CmdMeetingRemarkNotice::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
//{
//	return m_i32Result;
//}
//
//////////////////////会议成员信息提醒///////////////////////////////
//REGISTER_CMDCLASS(C_CmdMeetingMemberInfoNotic, eCMD_MEETING_MBRINFO_NOTICE);
//int C_CmdMeetingMemberInfoNotic::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
//{
//
//    return EIMERR_NO_ERROR;
//}
//
//int C_CmdMeetingMemberInfoNotic::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
//{
//	char tempTip[128];
//	eIMString stempTip;
//	snprintf(tempTip,COUNT_OF_ARRAY_(tempTip),"%s  cDataType:%d  wMbrNum:%d",m_sData.strConfId,m_sData.cDataType,m_sData.wMbrNum);
//	eIMUTF8ToTChar(tempTip,stempTip);
//	EIMLOGGER_INFO_(pILog,_T("MemberInfoNotic %s"),stempTip.c_str());
//
//	C_MeetingMgr meetmgr;
//	m_i32Result = meetmgr.Set(&(m_sData));
//	return m_i32Result;
//}
//
//int C_CmdMeetingMemberInfoNotic::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort /* = NULL */)
//{
//    if (m_i32Result)
//    {
//        //通知UI刷新主界面
//        SAFE_SEND_EVENT_(pIEMgr, EVENT_MEETING_MBRINFO_NOTICE, (void*)&m_sData);
//    }
//    return EIMERR_NO_ERROR;
//}
//////////////////////会议级别修改提醒///////////////////////////////
//REGISTER_CMDCLASS(C_CmdMeetingLevelInfoNotic, eCMD_GET_MEETING_LEVEL_INFO);
//int C_CmdMeetingLevelInfoNotic::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
//{
//
//	return EIMERR_NO_ERROR;
//}
//
//int C_CmdMeetingLevelInfoNotic::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
//{
//	EIMLOGGER_INFO_(pILog,_T("Change Level"));
//	C_MeetingMgr meetmgr;
//	m_i32Result = meetmgr.Set(&(m_sData));
//	return m_i32Result;
//}
//
//int C_CmdMeetingLevelInfoNotic::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort /* = NULL */)
//{
//	if (m_i32Result)
//	{
//		//通知UI刷新主界面
//		SAFE_SEND_EVENT_(pIEMgr, EVENT_MEETING_MBRINFO_NOTICE, (void*)&m_sData);
//	}
//	return EIMERR_NO_ERROR;
//}
/////////////////////////会议附件信息提醒//////////////////////////
//REGISTER_CMDCLASS(C_CmdMeetingFileInfoNotic, eCMD_MEETING_FILEINFO_NOTICE);
//int C_CmdMeetingFileInfoNotic::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
//{
//    return EIMERR_NO_ERROR;
//}
//
//int C_CmdMeetingFileInfoNotic::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
//{
//	EIMLOGGER_INFO_(pILog,_T("FileInfoNotice"));
//	C_MeetingMgr meetmgr;
//	m_i32Result = meetmgr.Set(&(m_sData));
//	return m_i32Result;
//}
//
//int C_CmdMeetingFileInfoNotic::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort /* = NULL */)
//{
//	if (m_i32Result)
//	{
//		//通知UI刷新主界面
//		SAFE_SEND_EVENT_(pIEMgr, EVENT_MEETING_FILEINFO_NOTICE, (void*)&m_sData);
//	}
//	return EIMERR_NO_ERROR;
//}
//
////////////////////////////会议帐号信息通知//////////////////////////
//REGISTER_CMDCLASS(C_CmdMeetingUserCodeInfoNotic, eCMD_MEETING_USERINFO_NOTICE);
//int C_CmdMeetingUserCodeInfoNotic::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort /* = NULL */)
//{
//    return EIMERR_NO_ERROR;
//}
//
//int C_CmdMeetingUserCodeInfoNotic::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort /* = NULL */)
//{
//	return EIMERR_NO_ERROR;
//}
//
//int C_CmdMeetingUserCodeInfoNotic::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort /* = NULL */)
//{
//    SAFE_SEND_EVENT_(pIEMgr, EVENT_MEETING_ACCOUNT_ACK, (void*)&m_sData);
//    return EIMERR_NO_ERROR;
//}

}	// namespace Cmd


