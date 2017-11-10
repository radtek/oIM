#include "stdafx.h"
#include "C_HelperCmd.h"
#include "C_XmlProtocol.h"
//#include "eIMEngine\C_CmdReflection.h"
#include "C_eIMContacts.h"

namespace HelperCmd
{
	//C_CmdLoadContacts////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdLoadContacts, eCMD_LOAD_CONTACTS);
	int	C_CmdLoadContacts::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		m_i32Result = EIMERR_NO_ERROR;
		C_eIMContactsMgr cmgr;
		cmgr.Init(pIDb);
		if (hpi.UIGetAttributeInt(PATH_FUNC,FUNC_CLEAR_INVAILD_DEPT_EMP,0) == 0)
		{
			cmgr.Clear(CLEAR_FLAG_INVALID_DEPT_EMPS);
			hpi.UISetAttributeInt(PATH_FUNC,FUNC_CLEAR_INVAILD_DEPT_EMP,1);
		}
		m_i32Result = cmgr.LoadEmps(pbAbort);
		if (FAILED(m_i32Result))
		{
			_CheckReloadContacts(m_i32Result, pILog);
			EIMLOGGER_ERROR_(pILog, _T("LoadEmps failed"));
			return m_i32Result;
		}
		EIMLOGGER_DEBUG_(pILog, _T("LoadEmps OK"));
		m_i32Result = cmgr.LoadContacts(pbAbort);
		if (FAILED(m_i32Result))
		{
			_CheckReloadContacts(m_i32Result, pILog);
			EIMLOGGER_ERROR_(pILog, _T("LoadContacts failed"));
			return m_i32Result;
		}

		EIMLOGGER_DEBUG_(pILog, _T("LoadContacts OK "));
		return m_i32Result;
	}

	int	C_CmdLoadContacts::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		if ( SUCCEEDED(m_i32Result)/* && m_i32Result != EIMERR_CONSTACT_LOADED */)		//Load Contacts finished
		{
			EIMLOGGER_DEBUG_(pILog, _T("SendEvent(EVENT_LOAD_CONTS_END) before"));
			SAFE_SEND_EVENT_(pIEMgr, EVENT_LOAD_CONTS_END, NULL);
			EIMLOGGER_DEBUG_(pILog, _T("SendEvent(EVENT_LOAD_CONTS_END) after"));
		}

		return TRUE;
	}

	BOOL C_CmdLoadContacts::_CheckReloadContacts(int i32Result, I_EIMLogger* pILog)
	{
		if ( i32Result != EIMERR_NO_INIT )
			return FALSE;
		
		int i32Times = Eng.GetAttributeInt(IME_RELOAD_CONTACTS);
		if ( i32Times > -3 )
		{
			EIMLOGGER_INFO_(pILog, _T("i32Times: %d"), i32Times);
			Eng.SetAttributeInt(IME_RELOAD_CONTACTS, i32Times - 1);
			hpi.SendEvent(EVENT_RELOAD_CONTACTS, (void*)IME_UPDATE_CONTS_TYPE_SERVER);
			return TRUE;
		}

		EIMLOGGER_ERROR_(pILog, _T("i32Times: %d < -3, abort"), i32Times);
		return FALSE;
	}


	//C_CmdModifyMyselfInfo////////////////////////////////////////////////////////////
	REGISTER_CMDCLASS(C_CmdModifyMyselfInfo, eCMD_MODIFY_MYSELF_INFO);
	int C_CmdModifyMyselfInfo::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		I_ClientAgent* pICa = static_cast<I_ClientAgent*>(pvCa);

		for ( int i32Index = 0; i32Index < m_i32Count; i32Index++ )
		{
			m_sModifyInfo[i32Index].cLen = strlen((char*)m_sModifyInfo[i32Index].aszModiInfo);
			m_sModifyInfo[i32Index].dwUserID = Eng.GetPurview(GET_LOGIN_QEID);
			m_i32Result = pICa->ModiInfo(m_sModifyInfo[i32Index].cModiType, m_sModifyInfo[i32Index].cLen, (char*)m_sModifyInfo[i32Index].aszModiInfo);
			if (FAILED(m_i32Result))
			{
				m_sModifyInfo[i32Index].cLen = 0;
				break;
			}
		}

		return m_i32Result;
	}

	int	C_CmdModifyMyselfInfo::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMContactsMgr cmgr;

		cmgr.Init(pIDb);
		for ( int i32Index = 0; i32Index < m_i32Count && m_sModifyInfo[i32Index].cLen; i32Index++ )
		{
			cmgr.Set((RESETSELFINFONOTICE*)&m_sModifyInfo[i32Index]);
		}

		return m_i32Result;
	}

	int	C_CmdModifyMyselfInfo::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		if (FAILED(m_i32Result))
			EIMLOGGER_ERROR_(pILog, _T("%s"), m_szXml.c_str());
		else
			EIMLOGGER_DEBUG_(pILog, _T("%s"), m_szXml.c_str());

		return m_i32Result;
	}

	BOOL C_CmdModifyMyselfInfo::FromXml( const TCHAR* pszXml )
	{
		int				i32Value = 0;
		const char*		pszValue = NULL;
		eIMString		szValue;
		C_eIMEngine&	eng	= Eng;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl		= ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);
		
		m_i32Count = 0;
		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		ZERO_STRUCT_(m_sModifyInfo);
		
		PS_EmpInfo psEmpInfo = (PS_EmpInfo)eng.GetLoginEmp();
		CHECK_NULL_RET_(psEmpInfo, FALSE);
		pszValue = pEl->Attribute(FIELD_SEX, &i32Value);
		if ( pszValue )
		{	// Sex
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 0;
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
			psEmpInfo->bitSex = i32Value;	ASSERT_(i32Value == 0 || i32Value == 1);
		}

		// 1. 籍贯...

		pszValue = pEl->Attribute(FIELD_BIRTHDAY, &i32Value);
		if ( pszValue )
		{	// Birthday
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 2;
			psEmpInfo->dwBirthday = i32Value;
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_ADDRESS);
		if ( pszValue )
		{	// Address
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 3;
			psEmpInfo->pszAddress =eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_OFFICE_TEL);
		if ( pszValue )
		{	// OfficeTel
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 4;
			psEmpInfo->pszTel = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_PHONE);
		if ( pszValue )
		{	// Phone
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 5;
			psEmpInfo->pszPhone = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_PASSWORD);
		if ( pszValue )
		{	// Password
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 6;
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_LOGO);
		if ( pszValue )
		{	// Logo
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 7;
//			psEmpInfo->pszLogo = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_SIGN);
		if ( pszValue )
		{	// Sign
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 8;
		//	SET_UTF8_2TVALUE_(pszValue, psEmpInfo->szSign);
			psEmpInfo->pszSign = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		// 9 权限...

		pszValue = pEl->Attribute(FIELD_HOME_TEL);
		if ( pszValue )
		{	// HomeTel
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 10;
		//	SET_UTF8_2TVALUE_(pszValue, psEmpInfo->szHomeTel);
			psEmpInfo->pszHomeTel = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_EMRG_TEL);
		if ( pszValue )
		{	// Emergency Phone
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 11;
		//	SET_UTF8_2TVALUE_(pszValue, psEmpInfo->szEmrgTel);
			psEmpInfo->pszEmrgTel = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		// 12, not exist

		pszValue = pEl->Attribute(FIELD_DUTY);
		if ( pszValue )
		{	// Duty
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 13;
			psEmpInfo->pszDuty = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
		//	SET_UTF8_2TVALUE_(pszValue, psEmpInfo->szDuty);
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIELD_EMAIL);
		if ( pszValue )
		{	// Email
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 14;
		//	SET_UTF8_2TVALUE_(pszValue, psEmpInfo->szEmail);
			psEmpInfo->pszEmail = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}

		pszValue = pEl->Attribute(FIEID_LOGO_EX,&i32Value);
		if(pszValue)
		{
			S_ModifyInfo& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = 15;
			psEmpInfo->dwLogoTime = IME_EMP_LOGO_NO_DOWNLOAD;
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}
		/*pszValue = pEl->Attribute(FIELD_FAX,&i32Value);
		if(pszValue)
		{
			MODIINFO& mi = m_sModifyInfo[m_i32Count++];
			mi.cModiType = **;
			psEmpInfo->pszFax = eIMAddString_((::eIMUTF8ToTChar(pszValue, szValue), szValue.c_str()));
			SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		}*/
		//pszValue = pEl->Attribute(FIELD_USER_NAME);
		//if ( pszValue )
		//{	// Name...
		//	MODIINFO& mi = m_sModifyInfo[m_i32Count++];
		//	mi.cModiType = **; 
		//	SET_UTF8_2TVALUE_(pszValue, psEmpInfo->szName);
		//	SET_UTF8_VALUE_(pszValue, mi.aszModiInfo);
		//}

		m_szXml = pszXml;

		return TRUE;
	}

	REGISTER_CMDCLASS(C_CmdFileAction, eCMD_FILE_ACTION);
	int	C_CmdFileAction::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		if ( !(m_sData.bitIsModify || m_sData.bitSaveStatus) )
			return EIMERR_NO_ERROR;

		const char*	pszValue;
		eIMStringA	 szValueA;
		eIMStringA	 szXmlA;
		S_Msg		 sMsg;
		S_OffFile	 sOffFile = { 0 };
		C_eIMFileMgr fmgr;
		C_eIMMsgMgr  mmgr;

		fmgr.Init(pIDb);
		mmgr.Init(pIDb);

		if (!fmgr.Get(m_sData.u64Fid, &sOffFile))
		{
			ASSERT_(FALSE);
			return EIMERR_INVALID_FID;
		}

		::eIMTChar2UTF8(m_sData.szFile, szValueA);
		strncpy(sOffFile.szLocalPath, szValueA.c_str(), COUNT_OF_ARRAY_(sOffFile.szLocalPath));
		fmgr.Set(&sOffFile);	// Update file information

		if (mmgr.Get(sOffFile.Qmid, sMsg, szXmlA) )
		{	// Update msg record
			TiXmlDocument xmlDoc;
			BOOL bUpdatedPath = FALSE;

			xmlDoc.Parse( szXmlA.c_str() );
			TiXmlHandle h(xmlDoc.RootElement());
			TiXmlElement* pEl = h.FirstChildElement(FIELD_MESSAGE).FirstChildElement().ToElement();
			for( ;pEl; pEl = pEl->NextSiblingElement() )
			{
				pszValue = pEl->Attribute(FIELD_FID);
				if (pszValue == NULL )
					continue;

				QFID qfid = _strtoui64(pszValue, NULL, 0);
				if ( qfid == m_sData.u64Fid )
				{
					if ( m_sData.bitSaveStatus )
					{
						pEl->SetAttribute(FIELD_STATUS, m_sData.bitStatus);
					}
					else if ( m_sData.u64FileId )
					{	// Update the cloud disk's file_id
						char szFileId[64] = { 0 };
						sprintf(szFileId, "%llu", m_sData.u64FileId );
						pEl->SetAttribute(FIELD_FILE_ID, szFileId);
					}
					else
					{
						const char* pszFile = pEl->Attribute(FIELD_FILE_PATH);
						if ( pszFile && _stricmp(pszFile, szValueA.c_str()) != 0 )
							bUpdatedPath = TRUE;

						pEl->SetAttribute(FIELD_FILE_PATH, szValueA.c_str());
					}

					ConvertToXml(*xmlDoc.RootElement(), m_szXml);
					mmgr.Set(&sMsg, m_szXml.c_str(), TRUE);

					if ( bUpdatedPath )
					{	// 发送消息记录更新通知
						S_UpdatedMsgRecord sUpdatedMsg = { sMsg.qsid, sMsg.qmid };
						hpi.SendEvent(EVENT_UPDATED_MSG_RECORD, (void*)&sUpdatedMsg);
					}

					return EIMERR_NO_ERROR;
				}
			}
		}

		return EIMERR_INVALID_FID;
	}

	//REGISTER_CMDCLASS(C_CmdCustomizeAction, eCMD_CUSTOMIZE_ACTION);
	int	C_CmdCustomizeAction::DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort)
	{
		return 0;
	}

	int	C_CmdCustomizeAction::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		//C_eIMEngine& eng = Eng;
		//C_eIMSessionMgr mgr;
		//mgr.Init(pIDb);
		//if ( eSEFLAG_CUSTOM_TOP_CONTACTS == m_sData.dwCustomizeId || eSEFLAG_CUSTOM_USER_GROUP == m_sData.dwCustomizeId)
		//{
		//	if(m_sData.byOperateType == eAdd )
		//	{
		//		S_SessionInfo sSInfo = {0};
		//		sSInfo.dwCreateTime = 0;
		//		sSInfo.eFlag		= eSEFLAG_CUSTOM_USER_GROUP;	// When CreateGroupAck to clear the flag of eSEFLAG_DELETE
		//		sSInfo.eType		= eSESSION_TYPE_SINGLE;
		//		sSInfo.qsid			= m_sData.u64GroupId;
		//		sSInfo.CreaterId	= eng.GetLoginEmp()->Id;
		//		sSInfo.dwUpdateTime = eng.GetTimeNow();
		//		_tcsncpy(sSInfo.szTitle,m_sData.aszName,MAX_SESSION_TITLE -1);

		//		mgr.Set(&sSInfo);
		//		m_i32Result = mgr.Set(sSInfo.qsid, (PQEID)m_sData.adwId, m_sData.dwIdNum, TRUE);
		//	}
		//	else if (m_sData.byOperateType == eDel)
		//	{
		//		mgr.Remove(m_sData.u64GroupId);
		//	}

		//}

		return m_i32Result;
	}

	int	C_CmdCustomizeAction::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		return m_i32Result;
	}

	BOOL C_CmdCustomizeAction::FromXml( const TCHAR* pszXml )
	{
		/*int				i32Value;
		const char*		pszValue = NULL;
		eIMString		szValue;
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;

		pEl->QueryValueAttribute(CUSTOMIZE_ID, &m_sData.dwCustomizeId) ;
		pEl->QueryValueAttribute(CUSTOM_GROUP_ID, &m_sData.u64GroupId);
		pEl->QueryValueAttribute(CUSTOM_OPERA_TYPE,(int*)&m_sData.byOperateType);

		pszValue = pEl->Attribute(CUSTOM_GROUP_NAME);
		SET_UTF8_2TVALUE_(pszValue, m_sData.aszName);

		for( pEl = pEl->FirstChildElement(); pEl && m_sData.dwIdNum < EIM_CUSTOM_GTOUP_MEMBER_MAX; pEl = pEl->NextSiblingElement() )
		{
		if ( pEl->Attribute(CUSTOM_MEMNER_ID, &i32Value ) )
		m_sData.adwId[m_sData.dwIdNum++] = i32Value;
		}

		m_szXml = pszXml;*/
		return TRUE;
	}


	REGISTER_CMDCLASS(C_CmdUpdateTimestampInfo, eCMD_UPDATE_TIMESTAMP);
	int	C_CmdUpdateTimestampInfo::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		Eng.UpdateTimestamp(pIDb);
		return 0;
	}

	REGISTER_CMDCLASS(C_CmdSubscribeAction, eCMD_SUBSCRIBE_ACTION);
	int	C_CmdSubscribeAction::DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort)
	{
		return 0;
	}

	int	C_CmdSubscribeAction::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		return m_i32Result;
	}

	int	C_CmdSubscribeAction::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMStateSubscribeMgr& objSubscribeMgr = C_eIMStateSubscribeMgr::GetObject();
		if(m_sData.eType == eStatusGet)					//状态拉取
		{
			if(m_sData.eOper == eOperAdd)
				objSubscribeMgr.UpdateExpandItemData(m_sData.u64Id, m_sData.iItemType, OPERATE_ADD);
			else if(m_sData.eOper == eOperDel)
				objSubscribeMgr.UpdateExpandItemData(m_sData.u64Id, m_sData.iItemType, OPERATE_DELETE);
			else
			{
				ASSERT_(FALSE);
			}
		}
		else if(m_sData.eType == eStatusSubscribe)		// 状态订阅
		{
			if(m_sData.eOper == eOperAdd)
				objSubscribeMgr.UpdateEmpsSubcribe(m_vectEmps, OPERATE_ADD);
			else if(m_sData.eOper == eOperDel)
				objSubscribeMgr.UpdateEmpsSubcribe(m_vectEmps, OPERATE_DELETE);
			else
			{
				ASSERT_(FALSE);
			}
		}

		return 0;
	}
	BOOL C_CmdSubscribeAction::FromXml( const TCHAR* pszXml )
	{
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();
		C_eIMEngine& eng = Eng;

		pEl->QueryValueAttribute("Opera",(int*)&m_sData.eOper);
		pEl->QueryValueAttribute("ItemID", &m_sData.u64Id);
		pEl->QueryValueAttribute("ItemType", &m_sData.iItemType);
		pEl->QueryValueAttribute("Type",(int*)&m_sData.eType);
		pEl->QueryValueAttribute("EmpsNum",&m_sData.u32QeidNum);
		
		if (m_sData.u32QeidNum > 0)
		{
			DWORD dwEmpId = 0;
			for( pEl = pEl->FirstChildElement(); pEl; pEl = pEl->NextSiblingElement() )
			{
				pEl->QueryValueAttribute("SubscribeEmpId",&dwEmpId);
				m_vectEmps.push_back(dwEmpId);
			}

			if(m_vectEmps.size() != m_sData.u32QeidNum) { ASSERT_(FALSE); }
		}

		m_szXml = pszXml;

		return TRUE;
	}

	REGISTER_CMDCLASS(C_CmdUpdateEmpHeadFlag, eCMD_UPDATE_EMP_HEAD_FLAG);
	int	C_CmdUpdateEmpHeadFlag::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		C_eIMContactsMgr cmgr;
		cmgr.Init(pIDb);
		cmgr.SetEmpLogotimeInfo(m_sData.qeid,m_sData.dwDownloadFlag == IME_EMP_LOGO_DOWNLOAD ? 0 : 1 );
		return 0;
	}

	REGISTER_CMDCLASS(C_CmdSaveOverTimeSendMsg, eCMD_ADD_OVERTIME_SENDMSG);
	int	C_CmdSaveOverTimeSendMsg::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		C_eIMEngine& eng = Eng;
		C_eIMMsgMgr		mmgr;
		mmgr.Init(pIDb);

		// 获取数据库中的消息XML
		S_Msg rMsg;
		eIMStringA sDataA;
		BOOL bGet = mmgr.Get(m_sData, rMsg, sDataA);
		if(bGet == FALSE)
		{
			return 0;
		}

		// 获取错误的字体
		eIMString  szMsgFont;
		szMsgFont = eng.GetAttributeStr(MSGUI_FONT_ERROR, _T(MSGUI_FONT_ERROR_DEFAULT));
		eIMStringA szMsgFontA;
		::eIMTChar2UTF8(szMsgFont.c_str(), szMsgFontA);
		TiXmlDocument xmlfont;
		xmlfont.Parse(szMsgFontA.c_str());

		// 消息XML解析
		TiXmlDocument xml;
		xml.Parse(sDataA.c_str());
		TiXmlElement* xmlCmd = xml.RootElement();
		CHECK_NULL_RET_(xmlCmd, 0);

		// 标题清除旧字体
		TiXmlElement* pFontEL = xmlCmd->FirstChildElement(FIELD_MSGFONT);
		if(pFontEL)
			xmlCmd->RemoveChild(pFontEL);
		// 加上新字体
		TiXmlElement* pFistChildEl = xmlCmd->FirstChildElement();
		if ( pFistChildEl )
			xmlCmd->InsertBeforeChild(pFistChildEl, *xmlfont.RootElement());
		else
			xmlCmd->InsertEndChild(*xmlfont.RootElement());

		// 内容清除旧字体
		TiXmlElement* pMsgEl = xmlCmd->FirstChildElement(FIELD_MESSAGE);
		CHECK_NULL_RET_(pMsgEl, 0);
		TiXmlElement* pFontELText = pMsgEl->FirstChildElement(FIELD_MSGFONT);
		if(pFontELText)
			pMsgEl->RemoveChild(pFontELText);
		// 加上新字体
		pFistChildEl = pMsgEl->FirstChildElement();
		if ( pFistChildEl )
			pMsgEl->InsertBeforeChild(pFistChildEl, *xmlfont.RootElement());
		else
			pMsgEl->InsertEndChild(*xmlfont.RootElement());

		// 内容添加  发送失败文字
		TiXmlElement* pMsgText = xmlCmd->FirstChildElement(FIELD_MESSAGE)->FirstChildElement(FIELD_MSGTEXT);
		CHECK_NULL_RET_(pMsgText, 0);
		const char* pText = pMsgText->GetText();
		
		#define ADD_NODE_START(N)				(_T("<") N _T(">"))
		#define ADD_NODE_END(N)					(_T("</") N _T(">"))
		eIMString sText;
		::eIMUTF8ToTChar(pText, sText);
		eIMString sNewText = sText +  _T("\n");
		eIMString sError = hpi.GetErrorLangInfo(EIMERR_MSG_OVERTIME, _T("消息应答超时，本条消息可能未发送成功！"));
		sNewText += sError;
		eIMString sTemp = ADD_NODE_START(_T(FIELD_MSGTEXT));
			sTemp += _T("<![CDATA[");
			sTemp += sNewText;
			sTemp += _T("]]>");
			sTemp += ADD_NODE_END(_T(FIELD_MSGTEXT));
		eIMStringA szMsgTextA;
		eIMTChar2UTF8(sTemp.c_str(), szMsgTextA);
		TiXmlDocument xmlNewText;
		xmlNewText.Parse(szMsgTextA.c_str());

		xmlCmd->FirstChildElement(FIELD_MESSAGE)->ReplaceChild(pMsgText, *xmlNewText.RootElement());

		eIMString szTxt;
		ConvertToXml(*xmlCmd, szTxt);
		// 更新数据库
		mmgr.Set(&rMsg, szTxt.c_str(), TRUE);

#if 0
		I_EIMCmd*		pCmd = NULL;
		eIMString szCmd;
		eIMStringA oStr;
		oStr << *xmlCmd;
		::eIMUTF8ToTChar(oStr.c_str(), szCmd);
		pCmd = CmdCreateInstance<I_EIMCmd*>( eCMD_SEND_MSG, szCmd.c_str());
		GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		SAFE_SEND_EVENT_(pIEMgr, EVENT_SEND_MSG, (void*)pCmd);
#endif
		return 0;
	}

	REGISTER_CMDCLASS(C_CmdFlushoutDatabase, eCMD_FLUSHOUT_DATABASE);
	int	C_CmdFlushoutDatabase::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		CHECK_NULL_RET_(pIDb, EIMERR_INVALID_POINTER);

		C_eIMSessionMgr smgr;
		smgr.Init(pIDb);
		smgr.Set(0, eSET_TYPE_CHECKPOINT, 0);

		return EIMERR_NO_ERROR;
	}

	REGISTER_CMDCLASS(C_CmdBackupDatabase, eCMD_BACKUP_DATABASE);
	const C_CmdBackupDatabase::S_DbBkupTable C_CmdBackupDatabase::s_asContsDbTable[] = 
	{ 
		{"t_define_group"		,eBKOP_REPLACE_ALL},
		{"t_define_group_emps"	,eBKOP_REPLACE_ALL},
		{"t_define_top_dept"	,eBKOP_REPLACE_ALL},
		{"t_define_top_emp"		,eBKOP_REPLACE_ALL},
		{"t_dept"				,eBKOP_DEL_REPLACE_ALL},
		{"t_dept_emp" 			,eBKOP_DEL_REPLACE_ALL},
		{"t_emp" 				,eBKOP_DEL_REPLACE_ALL},
	/*	{"t_expanditem_info"	,eBKOP_REPLACE_ALL},*/
		{"t_robot_info"			,eBKOP_REPLACE_ALL},
		{"t_session" 			,eBKOP_REPLACE_ALL},
		{"t_session_emp"		,eBKOP_REPLACE_ALL},
	/*	{"t_session_resign"		,eBKOP_REPLACE_ALL},*/ 
	/*	{"t_subscribe_info"		,eBKOP_REPLACE_ALL},*/
		{"t_timestamp"			,eBKOP_REPLACE_ALL},
		{"t_update_contacts"	,eBKOP_REPLACE_ALL},
		{"t_user_purview"		,eBKOP_REPLACE_ALL},
	/*	{"t_user_purview"		,eBKOP_REPLACE_ALL},*/
	};

	const C_CmdBackupDatabase::S_DbBkupTable C_CmdBackupDatabase::s_asMsgDbTable[] = 
	{	
		{"t_alert_records"	,eBKOP_TIMESTAMP},
		{"t_bro_records"	,eBKOP_TIMESTAMP},
		{"t_msg_read"		,eBKOP_TIMESTAMP},
		{"t_p2p_files"		,eBKOP_TIMESTAMP},
		{"t_schedule"		,eBKOP_TIMESTAMP},
		{"t_session_files"	,eBKOP_TIMESTAMP},
		{"t_session_records",eBKOP_TIMESTAMP},
		{"t_sms_records"	,eBKOP_TIMESTAMP},
	};

	int	C_CmdBackupDatabase::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort)
	{
		CHECK_NULL_RET_(pILog, EIMERR_INVALID_POINTER);
		
 		if ( pIDb == NULL )
		{	// Initial
			eIMString  szContsDbFile;
			eIMString  szContsDbBkFile;
			eIMString  szMsgDbBkFile;
			eIMString  szMsgDbFile;
			eIMStringA szDbFileA;

			::eIMGetFullPath(PATH_TOKEN_LOGINID, PATH_TYPE_CONTACTS_TMP, CONTACTS_DB_FILE, szContsDbBkFile, FALSE);
			::eIMGetFullPath(PATH_TOKEN_LOGINID, PATH_TYPE_CONTACTS_TMP, MSG_DB_FILE, szMsgDbBkFile, FALSE);

			::eIMGetFullPath(PATH_TOKEN_LOGINID, NULL, CONTACTS_DB_FILE, szContsDbFile, FALSE);
			::eIMGetFullPath(PATH_TOKEN_LOGINID, NULL, MSG_DB_FILE, szMsgDbFile, FALSE);

			::GetDbFile(eDBTYPE_CONTS, szDbFileA);
			if ( _IsValidDababase(pILog, szDbFileA.c_str(), s_asContsDbTable, COUNT_OF_ARRAY_(s_asContsDbTable)) )
				::CopyFile(szContsDbFile.c_str(), szContsDbBkFile.c_str(), TRUE);

			::GetDbFile(eDBTYPE_MSG, szDbFileA);
			if ( _IsValidDababase(pILog, szDbFileA.c_str(), s_asMsgDbTable, COUNT_OF_ARRAY_(s_asMsgDbTable)) )
				::CopyFile(szMsgDbFile.c_str(), szMsgDbBkFile.c_str(), TRUE);
			
			Eng.InitDb(FALSE, 0, TRUE);
			return EIMERR_NO_ERROR;
		}

		_BackupDatabase(pILog, pIDb, eDBTYPE_BK_MSG,   s_asMsgDbTable,   COUNT_OF_ARRAY_(s_asMsgDbTable));		// Backup msg.db
		_BackupDatabase(pILog, pIDb, eDBTYPE_BK_CONTS, s_asContsDbTable, COUNT_OF_ARRAY_(s_asContsDbTable));	// Backup contacts.db

		return m_i32Result;
	}

	int C_CmdBackupDatabase::_BackupDatabase(I_EIMLogger* pILog, I_SQLite3* pIDb, E_DBFILE_TYPE eType, PS_DbBkupTable_ psTable, int i32Count)
	{
		CHECK_NULL_RET_(pIDb, EIMERR_INVALID_POINTER);

		eIMStringA szBkFileA;
		::GetDbFile(eType, szBkFileA);
		if ( !::PathFileExistsA(szBkFileA.c_str()) )
		{
			EIMLOGGER_INFO_(pILog, _T("The backup database [%S] is not exist, ignore backup"), szBkFileA.c_str());
			return EIMERR_FILE_NOT_EXIST;
		}

		m_i32Result = pIDb->Attach(szBkFileA.c_str(), BKUP_DB_AS_NAME, DB_KEY);
		if ( SQLITE_OK != m_i32Result )
		{
			m_i32Result = EIMERR_DB_ERROR_(m_i32Result);
			EIMLOGGER_ERROR_(pILog, _T("Backup %S failed, ret:0x%08x"), szBkFileA.c_str(), m_i32Result);
			return m_i32Result;
		}

		char szSrcTable[128];
		char szDstTable[128];
		const char* pszSrcDbName = (eType == eDBTYPE_BK_MSG ? MSG_DB_AS_NAME : MAIN_DB_AS_NAME); 
		for ( int i32Index = 0; i32Index < i32Count; i32Index++)
		{
			_snprintf(szSrcTable, COUNT_OF_ARRAY_(szSrcTable), "%s.%s", pszSrcDbName,    psTable[i32Index].pszTable);
			_snprintf(szDstTable, COUNT_OF_ARRAY_(szDstTable), "%s.%s", BKUP_DB_AS_NAME, psTable[i32Index].pszTable);
			_BackupTable(pILog, pIDb, szSrcTable, szDstTable, psTable[i32Index].eBkupType);
		}

		pIDb->Detach(BKUP_DB_AS_NAME);

		return EIMERR_NO_ERROR;
	}

	int  C_CmdBackupDatabase::_BackupTable(I_EIMLogger* pILog, I_SQLite3* pIDb, const char* const pszSrcTable, const char* const pszDstTable, E_DbBkupType eDbBkupType)
	{
		CHECK_NULL_RET_(pIDb, EIMERR_INVALID_POINTER);
		CHECK_NULL_RET_(pszSrcTable, EIMERR_INVALID_POINTER);
		CHECK_NULL_RET_(pszDstTable, EIMERR_INVALID_POINTER);

		const char*	pszSql  = NULL;
		switch(eDbBkupType)
		{
		case eBKOP_TIMESTAMP:
			pszSql = pIDb->VMPrintf(kBkupRecordsByTT, pszDstTable, pszSrcTable, _GetLastRecordTimestamp(pIDb, pszDstTable));
			break;
		case eBKOP_DEL_REPLACE_ALL:
			// Check src table is valid
			pszSql = pIDb->VMPrintf(kIsValidTable, pszSrcTable);
			m_i32Result = pIDb->SQL(pszSql, true);
			if (SQLITE_OK != m_i32Result)	
			{	
				m_i32Result = EIMERR_DB_ERROR_(m_i32Result);
				return m_i32Result;
			}

			// Delete dst table at first(for t_dept, t_dept_emp, t_emps)
			pszSql = pIDb->VMPrintf(kDelRecords, pszDstTable);
			pIDb->SQL(pszSql, true);	
		case eBKOP_REPLACE_ALL:
			// Format the backup sql
			pszSql = pIDb->VMPrintf(kBkupRecordsAll, pszDstTable, pszSrcTable);
			break;
		default:
			ASSERT_(FALSE);
			break;
		}

		m_i32Result = pIDb->SQL(pszSql, true);
		return m_i32Result;
	}

	INT64 C_CmdBackupDatabase::_GetLastRecordTimestamp(I_SQLite3* pIDb, const char* const pszTable)
	{
		CHECK_NULL_RET_(pIDb, -1);
		CHECK_NULL_RET_(pszTable, -1);

		const char*		pszSql  = pIDb->VMPrintf(kGetLastRecordTT, pszTable);
		I_SQLite3Table* pITable = pIDb->GetTable(pszSql, true);
		CHECK_NULL_RET_(pITable, -1);
		AUTO_RELEASE_(pITable);

		if( pITable->Step() != SQLITE_ROW )
			return 0;

		return pITable->GetColInt64(0);
	}

	BOOL C_CmdBackupDatabase::_IsValidDababase(I_EIMLogger* pILog, const char* pszDbFile, PS_DbBkupTable_ psTable, int i32Count)
	{
		I_SQLite3* pIDb = NULL;

		QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_SQLITE_DB, pIDb, RET_FALSE);
		if (SQLITE_OK != pIDb->Open(pszDbFile))
			return FALSE;

		pIDb->Key(DB_KEY, DB_KEY_LEN);
		for ( int i32Index = 0; i32Index < i32Count; i32Index++)
		{
			const char* pszSQL = pIDb->VMPrintf(kIsValidTable, psTable[i32Index].pszTable);
			if (SQLITE_OK != pIDb->SQL(pszSQL, true))
				return FALSE;
		}

		return TRUE;
	}

	REGISTER_CMDCLASS(C_CmdSendMsgNoticeConfirm, eCMD_SEND_MSG_NOTICE_CONFIRM);
	int C_CmdSendMsgNoticeConfirm::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		CONVERT_TO_ICA_RET_(pvCa);
		m_i32Result = pICa->SendMSGNoticeConfirm(m_sData.dwRecverID, m_sData.aszMessage, m_sData.dwMsgLen, m_sData.dwMsgID);
		if (FAILED(m_i32Result)) 
			EIMLOGGER_ERROR_( pILog, _T("[%s] failed, result: 0x%08X, desc:%s"), m_szXml.c_str(), (m_i32Result), GetErrorDesc()); 
		else 
			EIMLOGGER_DEBUG_( pILog, _T("[%s] succeeded"), m_szXml.c_str()); 

		return m_i32Result;
	}

	int	C_CmdSendMsgNoticeConfirm::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort)
	{
		SAFE_SEND_EVENT_(pIEMgr, m_i32CmdId, &m_sData);
		return m_i32Result;
	}

	BOOL C_CmdSendMsgNoticeConfirm::FromXml( const TCHAR* pszXml )
	{
		TiXmlDocument	xmlDoc;
		TiXmlElement*	pEl = ParseXmlCmdHeader(xmlDoc, pszXml, m_i32CmdId, m_i32Result);

		CHECK_NULL_RET_(pEl, FALSE);
		ResetData();

		eIMStringA   oStr;
		char szMsgBuf[CONVERT_TO_STRING_SIZE] = { 0 };

		oStr << *pEl->FirstChildElement();
		strcpy(m_sData.aszMessage, oStr.c_str());

		pEl->QueryValueAttribute(FIELD_RECVER_ID, &m_sData.dwRecverID);
		m_sData.dwSenderID = Eng.GetPurview(GET_LOGIN_QEID);
		m_sData.dwMsgID    = Eng.NewID();	// By CA
		m_sData.dwMsgLen   = oStr.size() + 1;

		pEl->SetAttribute(FIELD_SENDER_ID, m_sData.dwSenderID);
		pEl->SetAttribute(FIELD_MSG_ID,  GET_QSID_ASTR_(m_sData.dwMsgID, szMsgBuf));
		pEl->SetAttribute(FIELD_MSG_LEN, m_sData.dwMsgLen);

		ConvertToXml(*pEl, m_szXml);
		return TRUE;
	}

	REGISTER_CMDCLASS(C_CmdSendImageFileFailed, eCMD_SEND_IMAGE_FILE_ERROR);
	int	C_CmdSendImageFileFailed::DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort)
	{
		C_eIMMsgMgr	mmgr;

		if ( mmgr.Init(pIDb) )
			mmgr.Set(&m_sMsg, m_szMsgUI.c_str(), FALSE);

		return EIMERR_NO_ERROR;
	}

	int	C_CmdSendImageFileFailed::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		EIMLOGGER_INFO_( pILog, _T("%s"), m_szXml.c_str()); 
		SAFE_SEND_EVENT_(pIEMgr, EVENT_SEND_MSG, (void*)this);
		return EIMERR_NO_ERROR;
	}

	const TCHAR* C_CmdSendImageFileFailed::ToXml()
	{
		eIMStringA		szValueA;
		TiXmlDocument	xmlDoc;
		C_eIMEngine&    eng = Eng;

		ZERO_STRUCT_(m_sMsg);
		::eIMTChar2UTF8(m_sData.szCmd, szValueA);
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
			::eIMTChar2UTF8(m_sData.szErrorInfo, szErrorA);
			TiXmlText	  ErrorText(szErrorA.c_str());
			TiXmlElement  ErrorEl(FIELD_MSGERROR);

			ErrorText.SetCDATA(true);
			ErrorEl.InsertEndChild(ErrorText);
			eng.AddFontElement(eSESSION_TYPE_ERROR, TRUE, h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement());
			h.FirstChildElement(FIELD_MSGUI).FirstChildElement(FIELD_MESSAGE).ToElement()->InsertEndChild(ErrorEl);

			eIMStringA szXml;
			szXml.clear();
			szXml << *xmlDoc.RootElement();
			::eIMUTF8ToTChar(szXml.c_str(), m_szXml);

			szXml.clear();
			szXml << *h.FirstChildElement(FIELD_MSGUI).ToElement();
			::eIMUTF8ToTChar(szXml.c_str(), m_szMsgUI);
			
			h.FirstChildElement(FIELD_MSGUI).Element()->QueryValueAttribute(FIELD_SEND_TIME, &m_sMsg.dwTime);
			m_sMsg.qmid = eng.NewID();
			m_sMsg.qeid = eng.GetPurview(GET_LOGIN_QEID);
			if ( const char* pszQSID = xmlDoc.RootElement()->Attribute( FIELD_GROUP_ID) )		// Check and set QMID
				m_sMsg.qsid = GroupId2Qsid(pszQSID);

			return m_szXml.c_str();
		}

		return NULL;
	}

	REGISTER_CMDCLASS(C_CmdHttpDownloadMapImgae, eCMD_HTTP_DOWNLOAD_MAPIMAGE);
	int	C_CmdHttpDownloadMapImgae::DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort)
	{
		return EIMERR_NO_ERROR;
	}

	int	C_CmdHttpDownloadMapImgae::DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort)
	{
		eIMStringA szResponeA;
		eIMStringA szUrlA;
		m_i32Result = C_eIMEngine::GetObject()._cUrlDownload(m_sData.szUrl,m_sData.szSavePath);
		if ( SUCCEEDED(m_i32Result) )
		{
			CImage image;

			HRESULT hr =  image.Load(m_sData.szSavePath);
			if(SUCCEEDED(hr)){
				image.Save(m_sData.szSavePath,Gdiplus::ImageFormatBMP);
				image.Detach();

				tagHttpDownloadMapImageData httpData;
				httpData.exID = m_sData.exID;
				_tcsncpy(httpData.exStr, m_sData.exStr, COUNT_OF_ARRAY_(m_sData.exStr));
				_tcsncpy(httpData.szLoadPath, m_sData.szSavePath, COUNT_OF_ARRAY_(m_sData.szSavePath));
				SAFE_SEND_EVENT_(pIEMgr, m_sData.receiveEvent, &httpData);
			}

		}
		else
		{
			m_i32Result = EIMERR_FAIL;
			//EIMLOGGER_ERROR_(pILog, _T("Failed, _cUrlDownload: %s, error:0x%08x"), szRequestUrl, m_i32Result);
		}
		return m_i32Result;
	}

}	// namespace HelperCmd
