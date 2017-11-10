#include "stdafx.h"
#include "C_eIMMsgMgr.h"

IMPLEMENT_PLUGIN_REF_(C_eIMMsgMgr, INAME_EIMENGINE_MSGMGR, m_dwRef);
C_eIMMsgMgr::C_eIMMsgMgr(void)
	: m_dwRef( 1 )
	, m_pIDb ( NULL )
{
}

C_eIMMsgMgr::~C_eIMMsgMgr(void)
{
	Uninit();
}

BOOL C_eIMMsgMgr::Init(I_SQLite3* pIDb)
{
	if ( pIDb )
	{
		SAFE_RELEASE_INTERFACE_(m_pIDb);// Release old
		pIDb->AddRef();		
		m_pIDb = pIDb;			// Set the new db
	}

	if ( m_pIDb )
		return TRUE;

	eIMStringA szDbFileA;
	if ( FAILED(GetEngineInterface(INAME_SQLITE_DB, (void**)&m_pIDb)) ||
		GetDbFile(eDBTYPE_CONTS, szDbFileA) == NULL ||
		SQLITE_OK != m_pIDb->Open(szDbFileA.c_str()) ||
		SQLITE_OK != m_pIDb->Key(DB_KEY, DB_KEY_LEN) ||
		GetDbFile(eDBTYPE_MSG, szDbFileA) == NULL ||
		SQLITE_OK != m_pIDb->Attach(szDbFileA.c_str(), MSG_DB_AS_NAME, DB_KEY))
	{
		SAFE_RELEASE_INTERFACE_(m_pIDb);
		GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
		EIMLOGGER_ERROR_(pILog, _T("Initial mamager failed!")); 
		return FALSE;
	}

	m_pIDb->SetBusyTimeout(DB_BUSY_TIMEOUT);
	return TRUE;
}

BOOL C_eIMMsgMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

QSID C_eIMMsgMgr::Get(QMID qmid)
{
	CHECK_INIT_RET_(0);
	
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsQmid, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return (QSID)pITable->GetColInt64(0);
}

BOOL C_eIMMsgMgr::Get(QMID qmid, S_Msg& sMsg, eIMStringA& szXmlA)
{
	szXmlA.clear();
	CHECK_INIT_RET_(FALSE);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsQmid2, qmid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, FALSE);
	AUTO_RELEASE_(pITable);

	if( pITable->Step()!= SQLITE_ROW )
		return FALSE;
	
	//SELECT sid, eid, msg, timestamp, type, flag,
	sMsg.qsid   = (QSID)pITable->GetColInt64(0);
	sMsg.qmid   = qmid;
	sMsg.qeid   = pITable->GetColInt(1);
	sMsg.dwTime = pITable->GetColInt(3);
	sMsg.eType  = (E_SessionType)pITable->GetColInt(4);
	sMsg.dwFlag = pITable->GetColInt(5);
	
	szXmlA = eIMCheckColTextNull(pITable->GetColText(2));
	return TRUE;
}

int C_eIMMsgMgr::Set(PS_Msg psMsg, const TCHAR* pszXml, BOOL bReplace)
{
	CHECK_NULL_RET_(psMsg,  EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(pszXml, EIMERR_INVALID_PARAM);
	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, bReplace ? kReplaceRecords : kInsertRecords, pIStmt, i32Ret);

	eIMStringA szXmlA;
	::eIMTChar2UTF8(pszXml, szXmlA);
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@msgid_",	(INT64)psMsg->qmid);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@sid",		(INT64)psMsg->qsid);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@eid",		(int)psMsg->qeid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@msg",		szXmlA.c_str());		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@timestamp", (int)psMsg->dwTime);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@type",		(int)psMsg->eType);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag",		(int)psMsg->dwFlag);	ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	if (i32Ret != SQLITE_OK && psMsg->qeid != Eng.GetPurview(GET_LOGIN_QEID))
	{
		TiXmlDocument doc;
		bool bSuccessLoad = false;
		eIMString FilePath = _T("");
		doc.Parse(szXmlA.c_str());
		bSuccessLoad = !doc.Error(); 

		if (bSuccessLoad)
		{
			EIMLOGGER_DEBUG_(pILog, _T("load msg ok"));
			TiXmlElement* pElementroot = doc.RootElement();
			const char* pName = pElementroot->Value();
			if(pName && strcmp(pName, FIELD_MSGUI) == 0)
			{
				EIMLOGGER_DEBUG_(pILog, _T("find msgui success"));
				TiXmlHandle	h(pElementroot);
				TiXmlElement* pFileElement = h.FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGANNEX).ToElement();
				if(pFileElement)
				{
					EIMLOGGER_DEBUG_(pILog, _T("is file"));
					FilePath = GetAttributeValue(pFileElement, _T(FIELD_FILE_PATH), _T("") ).c_str();

					if( ::DeleteFile(FilePath.c_str()) )
					{
						EIMLOGGER_DEBUG_(pILog, _T("Delete File  %s  :Success"),FilePath.c_str());
					}
				}
			}
		}
	}

	return EIMERR_NO_ERROR;
}

BOOL C_eIMMsgMgr::UpdateFlag(QMID qmid, DWORD dwFlag, BOOL bSet)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(bSet ? kUpdateRecordsSetFlag : kUpdateRecordsClearFlag, dwFlag, qmid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}

BOOL C_eIMMsgMgr::UpdateFlagByQSID(QSID qsid, DWORD dwFlag, BOOL bSet)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(bSet ? kUpdateRecordsSetFalgByQSID : kUpdateRecordsClearFlagByQSID, dwFlag, qsid);
	int i32Ret = m_pIDb->SQL(pszSql,true);

	return (SQLITE_OK == i32Ret);
}

BOOL C_eIMMsgMgr::UpdateType(QMID qmid, DWORD dwType)
{
    CHECK_INIT_RET_(FALSE);

    S_Msg sMsgInfo;
    eIMStringA szXml;
    int i32Ret = SQLITE_ERROR;
    if(Get(qmid, sMsgInfo, szXml) )
    {
        TiXmlDocument doc;
	    bool bSuccessLoad = false;
	    doc.Parse(szXml.c_str());
	    bSuccessLoad = !doc.Error();  
	    if(bSuccessLoad)
        {
            TiXmlElement* pMsgUINode = doc.RootElement();
            if(pMsgUINode)
			{
				if ( eIM_AT_MSG == dwType )
				{
					pMsgUINode->SetAttribute( FIELD_AT, 1 );	// 已经点@消息（@提示栏）
				}
				else if ( dwType == eIM_FILE_DOWNLOADED )
				{
					int iStatus = 0;
					TiXmlHandle h(pMsgUINode);
					if ( TiXmlElement* pEl = h.FirstChildElement(FIELD_MESSAGE).FirstChildElement(FIELD_MSGANNEX).ToElement() )
						pEl->Attribute(FIELD_STATUS, &iStatus);

					return (iStatus == FILE_STATUS_FINISHED);	// 是否已经下载过
				}
				else
				{
					QEID qSenderID = Eng.GetPurview(GET_LOGIN_QEID);
					if(sMsgInfo.qeid != qSenderID )
					{
						TiXmlElement* pMessageNode = pMsgUINode->FirstChildElement( FIELD_MESSAGE );
						if(pMessageNode)
						{
							DelMsgFile(pMessageNode);
						}
					}
					pMsgUINode->SetAttribute(FIELD_TYPE, dwType);
					TiXmlElement* pMessage  = pMsgUINode->FirstChildElement(FIELD_MESSAGE);
					if(pMessage)
					{
						pMessage->Clear();
					}
				}
				
				eIMStringA oStr;
				oStr << *pMsgUINode ;
				const char* pszSql = m_pIDb->VMPrintf(kUpdateRecordsType, dwType, oStr.c_str(), qmid);
				i32Ret = m_pIDb->SQL(pszSql,true);
			}
        }
    }

	return (SQLITE_OK == i32Ret);
}

BOOL C_eIMMsgMgr::UpdateMsg2ReCallType(SENDRECALLMSGACK sRecallInfo )
{
    BOOL bResult = UpdateType(sRecallInfo.dwMsgID, eIM_MSGTYPE_MSG_RECALL);
    if(bResult)
    {
        hpi.SendEvent(EVENT_SEND_RECALL_MSG_ACK, &sRecallInfo);
    }
    return bResult;
}

//=============================================================================
//Function:     GetCount
//Description:	Get the message count of qsid by date range, for calcate the pages
//
//Parameter:
//  qsid	   - Session ID	
//	dwStart    - Start date, 0 to min value
//	dwEnd      - End date, 0xFFFFFFFF to max value
//
//Return:
//		Return the count of the message
//=============================================================================
DWORD C_eIMMsgMgr::GetCount(QSID qsid, DWORD dwStart, DWORD dwEnd)
{
	CHECK_INIT_RET_(0);
	
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsCount, qsid, dwStart, dwEnd);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() != SQLITE_ROW )
		return 0;

	return (DWORD)pITable->GetColInt(0);
}

//=============================================================================
//Function:     GetCount
//Description:	Get the message count
//
//Parameter:
//	psCond      - Parameters
	
//Return:
//		Return the count             
//=============================================================================
DWORD C_eIMMsgMgr::GetCount(PS_MsgEnum psCond)
{
	CHECK_NULL_RET_(psCond, 0);
	CHECK_INIT_RET_(0);

	DWORD		dwCount = 0;
	char		szQsidCond[40] = { 0 };
	const char* pszSql    = NULL;
	const char* pszDate   = NULL;

	GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	C_eIMEngine& eng = Eng;
	if ( psCond->dwLimit == 0 )
		psCond->dwLimit = 0xFFFFFFFF;
	
	if ( psCond->rQSID )
		snprintf(szQsidCond, COUNT_OF_ARRAY_(szQsidCond), "sid=%lld AND ", psCond->rQSID);

	// Date
	if ( psCond->eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("%stimestamp=%u", szQsidCond, psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
		pszDate = m_pIDb->VMPrintf("%stimestamp>=%u", szQsidCond, psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
		pszDate = m_pIDb->VMPrintf("%stimestamp>=%u AND timestamp<=%u", szQsidCond, psCond->dwStart, psCond->dwEnd);

	if ((psCond->eOperate & eMSG_OP_NEW) && (psCond->eOperate & eMSG_OP_OFFLINE) )
		pszSql = m_pIDb->VMPrintf(kCountSelectRecordsFlag, DB_FLAG_MSG_NEW | DB_FLAG_MSG_OFFLINE, psCond->dwLimit, psCond->dwOffset);
	else if ( psCond->eOperate & eMSG_OP_NEW )
		pszSql = m_pIDb->VMPrintf(kCountSelectRecordsFlag, DB_FLAG_MSG_NEW, psCond->dwLimit, psCond->dwOffset);
	else if ( psCond->eOperate & eMSG_OP_OFFLINE )
		pszSql = m_pIDb->VMPrintf(kCountSelectRecordsFlag, DB_FLAG_MSG_OFFLINE, psCond->dwLimit, psCond->dwOffset);
	else
	{
		eIMStringA	szTextA;
		eIMStringA	szContsA;
		const char* pszSearch = NULL;

		// Content and contacts
		if ( (psCond->eOperate & eMSG_OP_TEXT) && (psCond->eOperate & eMSG_OP_CONTS) )
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			::eIMTChar2UTF8(psCond->szContacts, szContsA);
			pszSearch = m_pIDb->VMPrintf(kSelectRecordsNameText, szTextA.c_str(), szContsA.c_str(), szContsA.c_str(), psCond->rQSID);
		}
		else if (psCond->eOperate & eMSG_OP_TEXT)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearch = m_pIDb->VMPrintf("msg LIKE '%%%q%%'", szTextA.c_str()); 
		}
		else if (psCond->eOperate & eMSG_OP_CONTS)
		{
			::eIMTChar2UTF8(psCond->szContacts, szContsA);
			pszSearch = m_pIDb->VMPrintf(kSelectRecordsName, szContsA.c_str(), szContsA.c_str(), psCond->rQSID); // join...
		}

		if (pszDate && pszSearch)
			pszSql = m_pIDb->VMPrintf(kCountSelectRecords2, pszDate, pszSearch, psCond->dwLimit, psCond->dwOffset);
		else if (pszDate)
			pszSql = m_pIDb->VMPrintf(kCountSelectRecords, pszDate, psCond->dwLimit, psCond->dwOffset);
		else if (pszSearch)
			pszSql = m_pIDb->VMPrintf(kCountSelectRecords, pszSearch, psCond->dwLimit, psCond->dwOffset);
		else
			ASSERT_(FALSE);

		m_pIDb->Free((void*)pszSearch);
	}

	m_pIDb->Free((void*)pszDate);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	if( pITable->Step() == SQLITE_ROW )
	{
		dwCount		= pITable->GetColInt(0);
	}

	return dwCount;
}

//=============================================================================
//Function:     Enum
//Description:	Enumerate the session's date list by date range, for QQ like DateCtrl of msg
//
//Parameter:
//  qsid	   - Session ID	
//	dwStart    - Start date, 0 to min value
//	dwEnd      - End date, 0xFFFFFFFF to max value
//	pfnDateCb  - Date callback function 
//	pvUserData - User parameter passed to callback function
//
//Return:
//		Return the count             
//=============================================================================
DWORD C_eIMMsgMgr::Enum(QSID qsid, DWORD dwStart, DWORD dwEnd, PFN_DateCb pfnDateCb, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pfnDateCb, 0);

	DWORD			dwCount = 0;
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsDate, qsid, dwStart, dwEnd);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		if ( !pfnDateCb(pITable->GetColInt(0), pvUserData) )
			break;
	}
		
	return dwCount;
}

//=============================================================================
//Function:     Enum
//Description:	Enumerate the session by parameters
//
//Parameter:
//	psCond      - Parameters
//	pfnMsgCb    - Msg callback function
//	pvUserData	- User parameter passed to callback function

//Return:
//		Return the count             
//=============================================================================
DWORD C_eIMMsgMgr::Enum(PS_MsgEnum psCond, PFN_MsgCb pfnMsgCb, void* pvUserData, BOOL	  bRecent/* = FALSE*/)
{
	CHECK_NULL_RET_(psCond, 0);
	CHECK_NULL_RET_(pfnMsgCb, 0);
	CHECK_INIT_RET_(0);

	DWORD		dwCount = 0;
	char		szQsidCond[40] = { 0 };
	const char* pszSql    = NULL;
	const char* pszDate   = NULL;
	char		szOrderbyInfo[10] = { 0 };

	GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	C_eIMEngine& eng = Eng;
	if ( psCond->eOperate & eMSG_OP_LAST_MSG )
	{
		pszSql = m_pIDb->VMPrintf(kSelectRecordsLastMsgSid, psCond->rQSID);
	}
	else
	{
		if ( psCond->dwLimit == 0 )
			psCond->dwLimit = 0xFFFFFFFF;

		if ( psCond->rQSID )
			snprintf(szQsidCond, COUNT_OF_ARRAY_(szQsidCond), "sid=%lld AND ", psCond->rQSID);

		// Date
		if ( psCond->eOperate & eMSG_OP_DATE )
			pszDate = m_pIDb->VMPrintf("%stimestamp=%u", szQsidCond, psCond->dwStart);
		else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
			pszDate = m_pIDb->VMPrintf("%stimestamp>=%u", szQsidCond, psCond->dwStart);
		else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
			pszDate = m_pIDb->VMPrintf("%stimestamp>=%u AND timestamp<=%u", szQsidCond, psCond->dwStart, psCond->dwEnd);

		if(bRecent)
		{
			snprintf(szOrderbyInfo, COUNT_OF_ARRAY_(szOrderbyInfo), "DESC");
		}
		else
		{
			snprintf(szOrderbyInfo, COUNT_OF_ARRAY_(szOrderbyInfo), "ASC");
		}

		if ((psCond->eOperate & eMSG_OP_NEW) && (psCond->eOperate & eMSG_OP_OFFLINE) )
			pszSql = m_pIDb->VMPrintf(kSelectRecordsFlag,pszDate, DB_FLAG_MSG_NEW | DB_FLAG_MSG_OFFLINE, szOrderbyInfo, psCond->dwLimit, psCond->dwOffset);
		else if ( psCond->eOperate & eMSG_OP_NEW )
			pszSql = m_pIDb->VMPrintf(kSelectRecordsFlag, pszDate, DB_FLAG_MSG_NEW, szOrderbyInfo, psCond->dwLimit, psCond->dwOffset);
		else if ( psCond->eOperate & eMSG_OP_OFFLINE )
			pszSql = m_pIDb->VMPrintf(kSelectRecordsFlag, pszDate,DB_FLAG_MSG_OFFLINE, szOrderbyInfo, psCond->dwLimit, psCond->dwOffset);
		else
		{
			eIMStringA	szTextA;
			eIMStringA	szContsA;
			const char* pszSearch = NULL;

			// Content and contacts
			if ( (psCond->eOperate & eMSG_OP_TEXT) && (psCond->eOperate & eMSG_OP_CONTS) )
			{
				::eIMTChar2UTF8(psCond->szContent, szTextA);
				::eIMTChar2UTF8(psCond->szContacts, szContsA);
				pszSearch = m_pIDb->VMPrintf(kSelectRecordsNameText, szTextA.c_str(), szContsA.c_str(), szContsA.c_str(), psCond->rQSID);
			}
			else if (psCond->eOperate & eMSG_OP_TEXT)
			{
				::eIMTChar2UTF8(psCond->szContent, szTextA);
				pszSearch = m_pIDb->VMPrintf("msg LIKE '%%%q%%'", szTextA.c_str()); 
			}
			else if (psCond->eOperate & eMSG_OP_CONTS)
			{
				::eIMTChar2UTF8(psCond->szContacts, szContsA);
				pszSearch = m_pIDb->VMPrintf(kSelectRecordsName, szContsA.c_str(), szContsA.c_str(), psCond->rQSID); // join...
			}
			else if (psCond->eOperate & eMsg_OP_IMAGE)
			{
				pszSearch = m_pIDb->VMPrintf("msg LIKE '%%<MsgImage%%'", szTextA.c_str()); 
			}
			else if (psCond->eOperate & eMSG_OP_FILE)
			{
				pszSearch = m_pIDb->VMPrintf("(msg LIKE '%%<MsgAnnex%%' or msg LIKE '%%<MsgVideo%%')", szTextA.c_str()); 
				//MsgAnnexs and MsgVideoes are all Group on File    by fxy    
			}

			if (pszDate && pszSearch)
				pszSql = m_pIDb->VMPrintf(kSelectRecords2, pszDate, pszSearch, szOrderbyInfo, psCond->dwLimit, psCond->dwOffset);
			else if (pszDate)
				pszSql = m_pIDb->VMPrintf(kSelectRecords, pszDate, szOrderbyInfo, psCond->dwLimit, psCond->dwOffset);
			else if (pszSearch)
				pszSql = m_pIDb->VMPrintf(kSelectRecords, pszSearch, szOrderbyInfo, psCond->dwLimit, psCond->dwOffset);
			else
				ASSERT_(FALSE);

			m_pIDb->Free((void*)pszSearch);
		}

		m_pIDb->Free((void*)pszDate);
	}

	//TCHAR			szMsgUi[1024];
	//BOOL			bSend;
	DWORD			dwTimestamp;
	//DWORD			dwSType;
	QEID			qeidSender;
	QEID			qeidLoginId = eng.GetPurview(GET_LOGIN_QEID);
	eIMString		szXml;
	eIMString		szMsgFont;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		//dwSType		= pITable->GetColInt(4);
		qeidSender  = pITable->GetColInt(1);
		//bSend		= (qeidLoginId == qeidSender);
		dwTimestamp = pITable->GetColInt(3);
		//_sntprintf(szMsgUi, COUNT_OF_ARRAY_(szMsgUi), kMsgUiBegin, 
		//	qeidSender,							// SenderId
		//	pIConts->GetEmpName(qeidSender),	// Display name
		//	dwTimestamp,						// Timestamp
		//	dwSType,							// Session type
		//	bSend);								// Is send

        
		//::eIMUTF8ToTChar((const char*)pITable->GetColText(2), szXml);
        QMID msgId = pITable->GetColInt64(6);
	    TiXmlDocument xml;
	    xml.Parse(eIMCheckColTextNull(pITable->GetColText(2)));
	    TiXmlElement* pMsgUiNode = xml.RootElement();
	    if(pMsgUiNode && 0 < msgId)
        {
            TCHAR szValue[64] = {0};
            Int64ToStr(msgId, szValue, FALSE);
            eIMStringA strAid;
            ::eIMTChar2MByte(szValue, strAid);
            pMsgUiNode->SetAttribute(FIELD_MSG_ID,  strAid.c_str() );
            eIMStringA oStr;
	        oStr << *pMsgUiNode ;
	        ::eIMUTF8ToTChar(oStr.c_str(), szXml);
        }
        else
        {
            ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szXml);
        }
		if ( !pfnMsgCb(pITable->GetColInt64(0), pITable->GetColInt64(6), qeidSender, dwTimestamp, szXml.c_str(), pvUserData) )
			break;
	}

	return dwCount;
}

DWORD C_eIMMsgMgr::SearchMsgCount(PS_MsgEnum psCond)
{
	CHECK_NULL_RET_(psCond, 0);
	CHECK_INIT_RET_(0);

	const char* pszDate   = NULL;
	const char* pszContact   = NULL;
	const char* pszContent   = NULL;

	// 时间过滤
	if ( psCond->eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp=%u ", psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			pszDate = m_pIDb->VMPrintf("");
		}
		else
		{
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u ", psCond->dwStart);
		}
	}
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
	{
		pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u ", psCond->dwStart, psCond->dwEnd);
	}

	// 联系人过滤
	eIMStringA	szTextA;
	pszContact = m_pIDb->VMPrintf(""); 

	// 内容过滤
	if (psCond->eOperate & eMSG_OP_TEXT)
	{
		eIMTChar2UTF8(psCond->szContent,szTextA);
		if(psCond->eOperate & eMsg_OP_IMAGE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddContentTextLike,szTextA.c_str(), "<MsgImage");
		}
		else if (psCond->eOperate & eMSG_OP_FILE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddFileContentTextLike,szTextA.c_str());
		}
		else
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddContentLike, szTextA.c_str());
		}
	}
	else
	{
		if(psCond->eOperate & eMsg_OP_IMAGE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddContentLike, "<MsgImage");
		}
		else if (psCond->eOperate & eMSG_OP_FILE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddFileContentLike);
		}
		else
		{
			pszContent = m_pIDb->VMPrintf(""); 
		}
	}
	
	const char* pszSql = m_pIDb->VMPrintf(kNewSelectMsgCount, psCond->rQSID, pszContact, pszDate, pszContent); 

	m_pIDb->Free((void*)pszDate);
	m_pIDb->Free((void*)pszContact);
	m_pIDb->Free((void*)pszContent);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	DWORD		dwCount = 0;
	if( pITable->Step() == SQLITE_ROW )
	{
		dwCount	= pITable->GetColInt(0);
	}

	return dwCount;
}

DWORD C_eIMMsgMgr::SearchMsg(PS_MsgEnum psCond, PFN_MsgCb pfnMsgCb, void* pvUserData)
{
	CHECK_NULL_RET_(psCond, 0);
	CHECK_NULL_RET_(pfnMsgCb, 0);
	CHECK_INIT_RET_(0);

	const char* pszDate   = NULL;
	const char* pszContact   = NULL;
	const char* pszContent   = NULL;
	const char* pszLimite    = NULL;

	// 时间过滤
	if ( psCond->eOperate & eMSG_OP_DATE )
		pszDate = m_pIDb->VMPrintf("AND timestamp=%u ", psCond->dwStart);
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			pszDate = m_pIDb->VMPrintf("");
		}
		else
		{
			pszDate = m_pIDb->VMPrintf("AND timestamp>=%u ", psCond->dwStart);
		}
	}
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
	{
		pszDate = m_pIDb->VMPrintf("AND timestamp>=%u AND timestamp<=%u ", psCond->dwStart, psCond->dwEnd);
	}

	// 联系人过滤
	eIMStringA	szTextA;
	pszContact = m_pIDb->VMPrintf(""); 

	// 内容过滤
	if (psCond->eOperate & eMSG_OP_TEXT)
	{
		eIMTChar2UTF8(psCond->szContent,szTextA);
		if(psCond->eOperate & eMsg_OP_IMAGE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddContentTextLike,szTextA.c_str(), "<MsgImage");
		}
		else if (psCond->eOperate & eMSG_OP_FILE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddFileContentTextLike,szTextA.c_str());
		}
		else
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddContentLike, szTextA.c_str());
		}
	}
	else
	{
		if(psCond->eOperate & eMsg_OP_IMAGE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddContentLike, "<MsgImage");
		}
		else if (psCond->eOperate & eMSG_OP_FILE)
		{
			pszContent = m_pIDb->VMPrintf(kNewMsgAddFileContentLike);
		}
		else
		{
			pszContent = m_pIDb->VMPrintf(""); 
		}
	}

	//条数限制
	if (psCond->dwLimit > 0)
		pszLimite = m_pIDb->VMPrintf(kNewSelectMsgLimite,psCond->dwLimit, psCond->dwOffset);
	
	const char* pszSql = m_pIDb->VMPrintf(kNewSelectMsgDetail, 
		psCond->rQSID, pszContact, pszDate, pszContent,pszLimite ); 

	m_pIDb->Free((void*)pszDate);
	m_pIDb->Free((void*)pszContact);
	m_pIDb->Free((void*)pszContent);
	m_pIDb->Free((void*)pszLimite);

	C_eIMEngine&    eng = Eng;
	DWORD			dwTimestamp;
	QEID			qeidSender;
	eIMString		szXml;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);
	
	DWORD		dwCount = 0;
	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		qeidSender  = pITable->GetColInt(1);
		dwTimestamp = pITable->GetColInt(3);
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szXml);
        QMID qMsgId = pITable->GetColInt64(6);
        SetMidToXml(qMsgId, szXml);

		if ( !pfnMsgCb(pITable->GetColInt64(0), pITable->GetColInt64(4), qeidSender, dwTimestamp, szXml.c_str(), pvUserData) )
			break;
	}

	return dwCount;
}

void C_eIMMsgMgr::SetMidToXml(QMID qMsgId, eIMString&		szXml)
{
    stringstream sstr;
    sstr << qMsgId;
    eIMStringA strMsgId = sstr.str();;
    eIMStringA strMsgA;
    TiXmlDocument xml;
    ::eIMTChar2UTF8(szXml.c_str(), strMsgA);
    xml.Parse(strMsgA.c_str());
    TiXmlElement* xmlCmd = xml.RootElement();
    if(xmlCmd)
    {
        xmlCmd->ToElement()->SetAttribute(FIELD_MSG_ID,  strMsgId.c_str());
        eIMStringA oStr;
        oStr << *xmlCmd ;
        ::eIMUTF8ToTChar(oStr.c_str(), szXml);
    }
}

BOOL C_eIMMsgMgr::Get(QMID qmid, PFN_MsgCb pfnMsgCb,   void*	 pvUserData)
{
	CHECK_NULL_RET_(pfnMsgCb, 0);
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsQmid2, qmid);

	BOOL			bGet = FALSE;
	DWORD			dwTimestamp;
	QEID			qeidSender;
	eIMString		szXml;
	eIMString		szMsgFont;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
	{
		bGet = TRUE;
		qeidSender  = pITable->GetColInt(1);
		dwTimestamp = pITable->GetColInt(3);

		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szXml);
        QMID qMsgId = pITable->GetColInt64(6);
        SetMidToXml(qMsgId, szXml);
		pfnMsgCb(pITable->GetColInt64(0),qmid, qeidSender, dwTimestamp, szXml.c_str(), pvUserData);
	}

	return bGet;
}

BOOL C_eIMMsgMgr::Get(QMID qmid, PFN_MsgInfoCb pfnMsgCb,   void*	 pvUserData)
{
    CHECK_NULL_RET_(pfnMsgCb, 0);
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsQmid2, qmid);

	BOOL			bGet = FALSE;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
	{
		bGet = TRUE;
		QEID qeidSender  = pITable->GetColInt(1);
		DWORD dwTimestamp = pITable->GetColInt(3);
        int type = pITable->GetColInt(4);
		pfnMsgCb(qeidSender, type, dwTimestamp, pvUserData);
	}

	return bGet;
}

DWORD C_eIMMsgMgr::EnumBroMsg(PS_SessionEnum psCond, PFN_BroCb pfnBroMsgCb, void* pvUserData)
{
	CHECK_NULL_RET_(pfnBroMsgCb, 0);
	CHECK_INIT_RET_(0);
	DWORD dwCount = 0;

	CHECK_INIT_RET_(0);
	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearchConts = NULL;
	const char* pszSearchContent = NULL;
	BOOL bHasWhereSQL = FALSE;

	// 账号搜索 
	if ( psCond->eOperate & eMSG_OP_CONTS )
	{
		::eIMTChar2UTF8(psCond->szContacts, szContsA);
		pszSearchConts = m_pIDb->VMPrintf("where recver like '%%%q%%' ", szContsA.c_str());
		bHasWhereSQL = TRUE;
	}
	else
	{
		pszSearchConts = m_pIDb->VMPrintf("");
	}

	// 时间过滤
	const char* pszDateTime = NULL;
	if ( psCond->eOperate & eMSG_OP_DATE )
	{
		if (bHasWhereSQL)
		{
			pszDateTime = m_pIDb->VMPrintf("AND timestamp=%u ", psCond->dwStart);
		}
		else
		{
			bHasWhereSQL = TRUE;
			pszDateTime = m_pIDb->VMPrintf("WHERE timestamp=%u ", psCond->dwStart);
		}
	}
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			pszDateTime = m_pIDb->VMPrintf("");
		}
		else
		{
			if (bHasWhereSQL)
			{
				pszDateTime = m_pIDb->VMPrintf("AND timestamp>%u ", psCond->dwStart);
			}
			else
			{
				bHasWhereSQL = TRUE;
				pszDateTime = m_pIDb->VMPrintf("WHERE timestamp>%u ", psCond->dwStart);
			}
		}
	}
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
	{
		if (bHasWhereSQL)
		{
			pszDateTime = m_pIDb->VMPrintf("AND timestamp>%u AND timestamp<%u ", psCond->dwStart, psCond->dwEnd);
		}
		else
		{
			bHasWhereSQL = TRUE;
			pszDateTime = m_pIDb->VMPrintf("WHERE timestamp>%u AND timestamp<%u ", psCond->dwStart, psCond->dwEnd);
		}
	}

	// 内容搜索 
	if ( psCond->eOperate & eMSG_OP_TEXT )
	{
		if(bHasWhereSQL)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf("AND (msg like '%%%q%%' OR title like '%%%q%%')", szTextA.c_str(),szTextA.c_str());
		}
		else
		{
			bHasWhereSQL = TRUE;
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf("WHERE msg like '%%%q%%' OR title like '%%%q%%'", szTextA.c_str(),szTextA.c_str());
		}
	}
	else
	{
		pszSearchContent = m_pIDb->VMPrintf("");
	}

	const char* pszSql = NULL;

	pszSql = m_pIDb->VMPrintf(kSelectBroInfo, pszSearchConts, pszDateTime, pszSearchContent);

	if(pszSearchConts)
		m_pIDb->Free((void*)pszSearchConts);
	if(pszDateTime)
		m_pIDb->Free((void*)pszDateTime);
	if(pszSearchContent)
		m_pIDb->Free((void*)pszSearchContent);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_BroMsg rMsg;
	eIMString		szXml;
	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		memset(&rMsg, 0, sizeof(rMsg));
		rMsg.qmid  = pITable->GetColInt64(0);

		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szXml);
		_sntprintf(rMsg.szReceiver, COUNT_OF_ARRAY_(rMsg.szReceiver), szXml.c_str());
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szXml);
		_sntprintf(rMsg.szTitle, COUNT_OF_ARRAY_(rMsg.szTitle), szXml.c_str());
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(3)), szXml);
		_sntprintf(rMsg.szMsg, COUNT_OF_ARRAY_(rMsg.szMsg), szXml.c_str());
		rMsg.dwUpdateTime = pITable->GetColInt(4);
		if(!pfnBroMsgCb(&rMsg, pvUserData))
		{
			break;
		}
	}

	return dwCount;
}

DWORD C_eIMMsgMgr::EnumMsgNoticeMsg(PS_MsgEnum psCond, PFN_MsgNoticeCb pfnMsgNoticeCb, void* pvUserData)
{
	CHECK_NULL_RET_(pfnMsgNoticeCb, 0);
	CHECK_INIT_RET_(0);
	DWORD dwCount = 0;

	CHECK_INIT_RET_(0);
	eIMStringA	szTextA;
	eIMStringA	szContsA;
	const char* pszSearchConts = NULL;
	const char* pszSearchContent = NULL;
	BOOL bHasWhereSQL = FALSE;

	// 账号搜索 
	pszSearchConts = m_pIDb->VMPrintf("");

	// 时间过滤
	const char* pszDateTime = NULL;
	if ( psCond->eOperate & eMSG_OP_DATE )
	{
		if (bHasWhereSQL)
		{
			pszDateTime = m_pIDb->VMPrintf("AND timestamp=%u ", psCond->dwStart);
		}
		else
		{
			bHasWhereSQL = TRUE;
			pszDateTime = m_pIDb->VMPrintf("WHERE timestamp=%u ", psCond->dwStart);
		}
	}
	else if ( psCond->eOperate & eMSG_OP_GE_DATE ) 
	{
		if(psCond->dwStart == 0)
		{
			pszDateTime = m_pIDb->VMPrintf("");
		}
		else
		{
			if (bHasWhereSQL)
			{
				pszDateTime = m_pIDb->VMPrintf("AND timestamp>%u ", psCond->dwStart);
			}
			else
			{
				bHasWhereSQL = TRUE;
				pszDateTime = m_pIDb->VMPrintf("WHERE timestamp>%u ", psCond->dwStart);
			}
		}
	}
	else if ( psCond->eOperate & eMSG_OP_BETWEEN_DATE )
	{
		if (bHasWhereSQL)
		{
			pszDateTime = m_pIDb->VMPrintf("AND timestamp>%u AND timestamp<%u ", psCond->dwStart, psCond->dwEnd);
		}
		else
		{
			bHasWhereSQL = TRUE;
			pszDateTime = m_pIDb->VMPrintf("WHERE timestamp>%u AND timestamp<%u ", psCond->dwStart, psCond->dwEnd);
		}
	}

	// 内容搜索 
	if ( psCond->eOperate & eMSG_OP_TEXT )
	{
		if(bHasWhereSQL)
		{
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf("AND msg like '%%%q%%' ", szTextA.c_str());
		}
		else
		{
			bHasWhereSQL = TRUE;
			::eIMTChar2UTF8(psCond->szContent, szTextA);
			pszSearchContent = m_pIDb->VMPrintf("WHERE msg like '%%%q%%' ", szTextA.c_str());
		}
	}
	else
	{
		pszSearchContent = m_pIDb->VMPrintf("");
	}

	const char* pszSql = NULL;

	pszSql = m_pIDb->VMPrintf(kSelectMsgNoticeInfo, pszSearchConts, pszDateTime, pszSearchContent);

	if(pszSearchConts)
		m_pIDb->Free((void*)pszSearchConts);
	if(pszDateTime)
		m_pIDb->Free((void*)pszDateTime);
	if(pszSearchContent)
		m_pIDb->Free((void*)pszSearchContent);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_MsgNoticeMsg rMsg;
	eIMString		szXml;
	while( pITable->Step() == SQLITE_ROW )
	{
		dwCount++;
		memset(&rMsg, 0, sizeof(rMsg));
		rMsg.qmid  = pITable->GetColInt64(0);

		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szXml);
		_sntprintf(rMsg.szTitle, COUNT_OF_ARRAY_(rMsg.szTitle), szXml.c_str());
		::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(3)), szXml);
		_sntprintf(rMsg.szMsg, COUNT_OF_ARRAY_(rMsg.szMsg), szXml.c_str());
		rMsg.dwUpdateTime = pITable->GetColInt(4);
		if(!pfnMsgNoticeCb(&rMsg, pvUserData))
		{
			break;
		}
	}

	return dwCount;
}

//=============================================================================
//Function:     Delete
//Description:	Delete the message records
//
//Parameter:
//	qsid		- Session Id to be deleted
//
//Return:
//		TRUE	- Succeeded
//		FALSE	- FALSE
//=============================================================================
BOOL C_eIMMsgMgr::Delete(QSID qsid)
{
	CHECK_INIT_RET_(FALSE);
	
	const char* const pszSql = m_pIDb->VMPrintf(kDeleteRecordsQsid, qsid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret == SQLITE_OK);
}

BOOL C_eIMMsgMgr::DeleteMsgByMsgID(QMID qmid)
{
	CHECK_INIT_RET_(FALSE);
	
	const char* const pszSql = m_pIDb->VMPrintf(kDeleteRecordsQmid, qmid, qmid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret == SQLITE_OK);
}

BOOL C_eIMMsgMgr::DeleteSMSByID( QMID qmid)
{
	CHECK_INIT_RET_(FALSE);
	
	const char* const pszSql = m_pIDb->VMPrintf(kDeleteSMSQmid, qmid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret == SQLITE_OK);
}

BOOL C_eIMMsgMgr::DeleteBoardcastByID(QMID qmid)
{
	CHECK_INIT_RET_(FALSE);
	
	const char* const pszSql = m_pIDb->VMPrintf(kDeleteBoardcastQmid, qmid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret == SQLITE_OK);
}

BOOL C_eIMMsgMgr::DeleteMsgNoticeByID(QMID qmid)
{
	CHECK_INIT_RET_(FALSE);
	
	const char* const pszSql = m_pIDb->VMPrintf(kDeleteMsgNoticeQmid, qmid);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (i32Ret == SQLITE_OK);
}

BOOL C_eIMMsgMgr::DeleteMsg(QSID qSid, DWORD dwDelTime)
{
    CHECK_INIT_RET_(FALSE);
    const char* const kSelectDeleteQSid = "SELECT msgid_ FROM t_session_records WHERE sid=%lld And timestamp<=%u";
    const char* const pszSql = m_pIDb->VMPrintf(kSelectDeleteQSid, qSid, dwDelTime);
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);
    while(SQLITE_ROW == pITable->Step())
    {
        DeleteMsgByMsgID(pITable->GetColInt64(0));
    }

    return TRUE;
}

BOOL C_eIMMsgMgr::IsMsgIDExist(QMID qmid)
{
	CHECK_INIT_RET_(0);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectMsgExistQmid, qmid);

	BOOL			bGet = FALSE;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
	{
		bGet = TRUE;
	}

	return bGet;
}

BOOL C_eIMMsgMgr::IsSMSIDExist(QMID qmid)
{
	CHECK_INIT_RET_(0);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectSMSExistQmid, qmid);

	BOOL			bGet = FALSE;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
	{
		bGet = TRUE;
	}

	return bGet;
}

BOOL C_eIMMsgMgr::IsBroIDExist(QMID qmid)
{
	CHECK_INIT_RET_(0);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectBroExistQmid, qmid);

	BOOL			bGet = FALSE;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
	{
		bGet = TRUE;
	}

	return bGet;
}

BOOL C_eIMMsgMgr::IsMsgNoticeIDExist(QMID qmid)
{
	CHECK_INIT_RET_(0);
	const char*		pszSql  = m_pIDb->VMPrintf(kSelectMsgNoticeExistQmid, qmid);

	BOOL			bGet = FALSE;
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
	{
		bGet = TRUE;
	}

	return bGet;
}

QMID C_eIMMsgMgr::GetLastId()
{
	CHECK_INIT_RET_(0);
	I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectRecordsLastId, false);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
		return pITable->GetColInt64(0);

	return 0;
}

DWORD C_eIMMsgMgr::GetSessionLastMsgTime( QSID qsid ) 
{
	CHECK_INIT_RET_(0);

	const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsSessionLastMsgTime, qsid);

	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	if( pITable->Step() == SQLITE_ROW )
		return pITable->GetColInt(0);

	return 0;
}

int  C_eIMMsgMgr::ReplaceSet(void* pszData)
{
	if(!pszData) return EIMERR_INVALID_PARAM;

	CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);

	int i32Ret = 0;
	I_SQLite3Stmt*	pIStmt = NULL;
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kReplaceRecords, pIStmt, i32Ret);

	vector<S_MsgInfo> *pVecRecrd = (vector<S_MsgInfo>*)pszData;
	for(vector<S_MsgInfo>::iterator it=pVecRecrd->begin(); it!=pVecRecrd->end(); it++)
	{
		i32Ret = pIStmt->Bind("@msgid_",	(INT64)it->sMsg.qmid);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@sid",		(INT64)it->sMsg.qsid);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@eid",		(int)it->sMsg.qeid);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@msg",		it->szXml);				ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@timestamp", (int)it->sMsg.dwTime);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@type",		(int)it->sMsg.eType);	ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@flag",		(int)it->sMsg.dwFlag);	ASSERT_(i32Ret == SQLITE_OK);
		int i32Ret = pIStmt->Step();
		if (SQLITE_OK != i32Ret)
		{
			eIMString szXml; eIMUTF8ToTChar(it->szXml, szXml);
			EIMLOGGER_ERROR_(pILog, _T("ReplaceSet Step Failed: code=%u, xml=%u"), GetLastError(), szXml.c_str());
		}
	}

	i32Ret = m_pIDb->Commit();
	return EIMERR_NO_ERROR;
}

DWORD C_eIMMsgMgr::EnumMsgId(QSID qsid, DWORD dwTime,  PFN_MsgIdCb pfnMsgIdCb, void* pvUserData)
{
    CHECK_INIT_RET_(0);
    CHECK_NULL_RET_(pfnMsgIdCb, 0);
     I_SQLite3Table* pITable = NULL;
     DWORD			dwCount = 0;
     if( 0 == dwTime)
     {
         const char* const kSelectRecordsAllMsgid = "SELECT msgid_ FROM t_session_records WHERE sid=%lld;";
         const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsAllMsgid, qsid);
         pITable = m_pIDb->GetTable(pszSql, true);
     }
     else
     {
         const char* const kSelectRecordsMsgid = "SELECT msgid_ FROM t_session_records WHERE sid=%lld AND timestamp<=%u;";
         const char*		pszSql  = m_pIDb->VMPrintf(kSelectRecordsDate, qsid, dwTime);
         pITable = m_pIDb->GetTable(pszSql, true);
     }
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);

    while( pITable->Step() == SQLITE_ROW )
    {
        dwCount++;
        if ( !pfnMsgIdCb(pITable->GetColInt64(0), pvUserData) )
            break;
    }

    return dwCount;
}

DWORD C_eIMMsgMgr::GetRecordsBySid(QSID qSid,  PFN_RecordsAll pfnRecords, void* pvUserData)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(pvUserData, 0);
	CHECK_NULL_RET_(pfnRecords, 0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectRecordsBySid, qSid);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql);
	CHECK_NULL_RET_(pITable, 0);
	AUTO_RELEASE_(pITable);

	S_MsgNoticeMsg rMsg;
	eIMString		szXml;
	DWORD dwCnt = 0;
	while(pITable->Step() == SQLITE_ROW)
	{
		dwCnt++;
		memset(&rMsg, 0, sizeof(rMsg));
		const unsigned char* pszMsg  = pITable->GetColText(0);
		if(pszMsg)
		{
			eIMString szMsg;
			//eIMUTF8ToTChar((const char*)pszMsg, szMsg);
			if(pfnRecords)
				pfnRecords((void*)pszMsg, pvUserData);
		}
	}

	return dwCnt;
}

eIMString C_eIMMsgMgr::GetName(const TiXmlElement *pElement, LPCTSTR szDefault)
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

eIMString C_eIMMsgMgr::GetAttributeValue(const TiXmlElement *pElement, LPCTSTR szAttriName, LPCTSTR szDefault)
{
    eIMStringA szNameA;
    ::eIMTChar2UTF8(szAttriName, szNameA);
    const char* pszValue = pElement->Attribute(szNameA.c_str());

    eIMString sVal;
    if(pszValue == NULL || pszValue[0] == '\0')
    {
        sVal = szDefault;
    }
    else
    {
        ::eIMUTF8ToTChar(pszValue, sVal);
    }

    return sVal;
}

void C_eIMMsgMgr::GetNodeText(const TiXmlElement* pElementchild, eIMString& strText)
{
    strText = _T("");
    const char* pValue = pElementchild->GetText();
    if(pValue)
    {
        ::eIMMByte2TChar(pValue, strText );
    }
}

void C_eIMMsgMgr::DelMsgFile(TiXmlElement* pMessage)
{
    for(TiXmlElement* node=pMessage->FirstChildElement(); node; node=node->NextSiblingElement())
    {
        eIMString szChildName = GetName(node, _T(""));
        if( 0 == szChildName.compare(_T(FIELD_MSGTEXT)) )
        {
            DelTextMsgImg(node);
        }
        else if(0 == szChildName.compare(_T(FIELD_MSGANNEX))
            || 0 == szChildName.compare(_T(FIELD_MSGVIDEO)) 
            || 0 == szChildName.compare(_T(FIELD_MSGVOICE))
            || 0 == szChildName.compare(_T(FIELD_MSGIMAGE)) )
        {
            eIMString szFilePath = GetAttributeValue(node, _T(FIELD_FILE_PATH), _T(""));
            if(!szFilePath.empty())
            {
                eIMString szFileFid = GetAttributeValue(node, _T(FIELD_FID), _T(""));
                if( !szFileFid.empty())
                {
                    S_FileTransInfo sFileInfo;
                    sFileInfo.u64Fid		= Str2Int64(szFileFid.c_str(), 0);
                    _tcsncpy(sFileInfo.szFile, szFilePath.c_str(), COUNT_OF_ARRAY_(sFileInfo.szFile));
                    sFileInfo.bitAction = FILE_ACTION_CANCEL;
                    sFileInfo.bitType   = FILE_TRANS_TYPE_OFFLINE;
                    sFileInfo.bitDirect = FILE_DIR_DOWNLOAD;
                    sFileInfo.bitStatus = FILE_STATUS_CANCEL;
                   hpi.SendEvent(EVENT_FILE_TRANSFER_ACTION, &sFileInfo);
                }
                SetFileAttributes(szFilePath.c_str(), 0);  //去掉文件只读属性
                DeleteFile(szFilePath.c_str());
            }
        }
    }
}

void C_eIMMsgMgr::DelTextMsgImg(const TiXmlElement* pElementchild)
{
    eIMString szNodeData;
    GetNodeText(pElementchild, szNodeData);
    if(!szNodeData.empty())
    {

    }
}

BOOL C_eIMMsgMgr::UpdateReadMsgSyncNotice(QSID sid, DWORD dwTimestamp)
{
	CHECK_INIT_RET_(FALSE);

	const char* pszSql = m_pIDb->VMPrintf(kUpdateReadMsgSyncNotice, (DWORD)eSEFLAG_OFFLINE|eSEFLAG_NEW, sid, dwTimestamp ? Eng.GetTimeNow() : dwTimestamp);
	int i32Ret = m_pIDb->SQL(pszSql, true);

	return (SQLITE_OK == i32Ret);
}