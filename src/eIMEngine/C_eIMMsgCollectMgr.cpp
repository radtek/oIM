#include "stdafx.h"
#include "C_eIMMsgCollectMgr.h"


#define  TM_YEAR_ADD   (1900)
#define  TM_MONTH_ADD (1)

eIMString GetNodeName(const TiXmlElement *pElement, LPCTSTR szDefault)
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

IMPLEMENT_PLUGIN_REF_(C_eIMMsgCollectMgr, INAME_EIMENGINE_MSGCOLLECTMGR, m_dwRef);
C_eIMMsgCollectMgr::C_eIMMsgCollectMgr(void)
	:m_pIDb ( NULL )
    ,m_dwRef(1)
{
}

C_eIMMsgCollectMgr::~C_eIMMsgCollectMgr(void)
{
    Uninit();
}

BOOL C_eIMMsgCollectMgr::Init(I_SQLite3* pIDb)
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

BOOL C_eIMMsgCollectMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

DWORD  C_eIMMsgCollectMgr::EnumMsgCollect(E_CollectType eType, PFN_GetCollectItem pfnGetCollectItem, void *pvUserData) 
{
    CHECK_INIT_RET_(0);
    const char*		pszSql  = NULL;
    if (eCOLLECT_TYPE_ALL == eType)
    {
       pszSql = m_pIDb->VMPrintf(kSelectCollects);
    }
    else
    {
        pszSql = m_pIDb->VMPrintf(kSelectCollectsType, eType);
    }
    
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);
    DWORD dwCount = 0;
    while( pITable->Step() == SQLITE_ROW )
    {
        ++dwCount;
        eIMString szXml;
        ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szXml);

        eIMString szTitle;
        ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szTitle);

        S_CollectMsg sInfo;
        sInfo.m_collectID = pITable->GetColInt64(0); 
        sInfo.m_dwTime =pITable->GetColInt(4); 
        sInfo.m_isGroup = pITable->GetColInt(3);
        sInfo.m_eType = (E_CollectType )pITable->GetColInt(5); 
        sInfo.m_sendid = pITable->GetColInt(6); 
        sInfo.m_Sid = pITable->GetColInt64(8);
        if ( !pfnGetCollectItem(sInfo,  szXml.c_str(), szTitle.c_str(),  pvUserData) )
            break;
    }
    return dwCount;
}

DWORD  C_eIMMsgCollectMgr::EnumMsgCollect(LPCTSTR pszSearch, PFN_GetCollectItem pfnGetCollectItem, void *pvUserData)
{
    CHECK_INIT_RET_(0);
    I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectCollects, false);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);
    DWORD dwCount = 0;
    while( pITable->Step() == SQLITE_ROW )
    {
        ++dwCount;
        eIMString szXml;
        ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(1)), szXml);
        BOOL bHasInXml = hasSearchData(pszSearch, szXml);
        
        eIMString szTitle;
        ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szTitle);
        QEID sendid = pITable->GetColInt(6); 

        int titlePos = szTitle.find(pszSearch);
        if (-1 == titlePos)
        {
            C_eIMContacts&	 obj = C_eIMContacts::GetObject();
            eIMString strContantName  = obj.GetEmpName(sendid);
            titlePos = strContantName.find(pszSearch);
        }
       
        BOOL isSearchTime = isSearchData((DWORD)pITable->GetColInt64(4), pszSearch);
        if (bHasInXml || -1 < titlePos || isSearchTime)
        {
            eIMString szTitle;
            ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(2)), szTitle);

            S_CollectMsg sInfo;
            sInfo.m_collectID = pITable->GetColInt64(0); 
            sInfo.m_dwTime =pITable->GetColInt(4); 
            sInfo.m_isGroup = pITable->GetColInt(3);
            sInfo.m_eType = (E_CollectType )pITable->GetColInt(5); 
            sInfo.m_sendid = sendid;
            sInfo.m_Sid = pITable->GetColInt64(8);
            if ( !pfnGetCollectItem(sInfo,  szXml.c_str(), szTitle.c_str(),  pvUserData) )
                break;
        }    
    }
    return dwCount;
}

bool compare(const eIMString& x, const eIMString& y)
{
    eIMString::const_iterator p = x.begin();
    while (p != x.end())
    {
        eIMString::const_iterator qTmp = y.begin();
        if (toupper(*p) == toupper(*qTmp))
        {
            eIMString::const_iterator pTmp = p+1;
            ++qTmp;
            while (pTmp != x.end() && qTmp != y.end() && toupper(*pTmp) == toupper(*qTmp))
            {
                ++pTmp;
                ++qTmp;
                if (qTmp == y.end())
                {
                    return true;
                }
            }
        }
        ++p;
    }
    return false;
}

BOOL C_eIMMsgCollectMgr::hasSearchData(const LPCTSTR& pszSearch, const eIMString& szXml)
{
    TiXmlDocument doc;
    eIMStringA szXmlA;
    ::eIMTChar2UTF8(szXml.c_str(), szXmlA);
    if(!doc.Parse(szXmlA.c_str()))
    {
        TiXmlElement* pElementroot = doc.RootElement();
        if(pElementroot)
        {
            for(TiXmlElement* pNode=pElementroot->FirstChildElement(); pNode; pNode=pNode->NextSiblingElement())
            {
                if(pNode)
                {
                    eIMString szRootName = GetNodeName(pNode, _T(""));
                    if(0 == szRootName.compare(_T(FIELD_MESSAGE)) || 0 == szRootName.compare(_T(SMS_MSG)) || 0 == szRootName.compare(_T(FILED_BRO_MSG)))
                    {
                        for(TiXmlElement* node=pNode->FirstChildElement(); node; node=node->NextSiblingElement())
                        {
                            eIMString szChildName = GetNodeName(node, _T(""));
                            if(0 == szChildName.compare(_T(FIELD_MSGTEXT))
                                || 0 == szChildName.compare(_T(FIELD_SMS_TEXT))
                                || 0 == szChildName.compare(_T(FILED_ALERT_MSG_TEXT))
                                || 0 == szChildName.compare(_T(FILED_BRO_MSG_TEXT)) 
                                || 0 == szChildName.compare(_T(FIELD_MSGIMAGE)) )
                            {
                                const char* pText = node->GetText();
                                eIMString strText;
                                ::eIMUTF8ToTChar(pText, strText);
                                bool bEqu = compare(strText, pszSearch);
                                if (bEqu)
                                {
                                    return TRUE;
                                }
                            }
                            else if(0 == szChildName.compare(_T(FIELD_MSGANNEX)) 
                                || 0 == szChildName.compare(_T(FIELD_MSGVOICE))
                                || 0 == szChildName.compare(_T(FIELD_MSGVIDEO)))
                            {
                                const char* pszValue = node->Attribute(FIELD_FILE_PATH);
                                eIMString strText;
                                ::eIMUTF8ToTChar(pszValue, strText);
                                bool bEqu = compare(strText, pszSearch);
                                if (bEqu)
                                {
                                    return TRUE;
                                }
                            }
                            else if( 0 == szChildName.compare(_T(FIELD_MSGLONG)))
                            {
                                const char* pszValue = node->Attribute(FIELD_FILE_PATH);
                                eIMString szPathValue;
                                if(pszValue)
                                    ::eIMUTF8ToTChar(pszValue, szPathValue);

                                const char* pTitleText = node->GetText();
                                eIMString sText;
                                ::eIMUTF8ToTChar(pTitleText, sText);
                                FILE* f = _tfopen(szPathValue.c_str(), _T("r+b"));
                                if(f)
                                {
									char szBuf[1024+1] = { 0 };
                                    while (!feof(f)) 
                                    { 
                                        fgets(szBuf,1024,f);  //∂¡»°“ª––
                                        eIMString strText;
                                        ::eIMUTF8ToTChar(szBuf, strText);
                                        int iPos = strText.find(pszSearch);
                                        if (-1 < iPos)
                                        {
                                            fclose(f);
                                            return TRUE;
                                        }
                                    } 
                                    fclose(f);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return FALSE;
}

BOOL C_eIMMsgCollectMgr::isSearchData(DWORD dwCollectTime, LPCTSTR pszSearch)
{
    time_t collectTime = (time_t)dwCollectTime;
     tm *pTime = localtime(&collectTime);
     pTime->tm_year += TM_YEAR_ADD;
     pTime->tm_mon += TM_MONTH_ADD;
     eIMString strSearch = pszSearch;
     TCHAR strYear[8] = {0};
     IntToStr(pTime->tm_year, strYear, FALSE);
     TCHAR strMonth[8] = {0};
     IntToStr(pTime->tm_mon, strMonth, FALSE);
     TCHAR strDay[8] = {0};
     IntToStr(pTime->tm_mday, strDay, FALSE);

     eIMString cmpStr = strYear;
     int iYearPos = strSearch.find(strYear);
     int iMonthPos =strSearch.find(strMonth);
     int iDayPos = strSearch.find(strDay);
     if (-1 < iYearPos && -1 < iMonthPos && -1 < iDayPos)
     {
         return TRUE;
     }
     
     return FALSE;
}

DWORD C_eIMMsgCollectMgr::GetCount()
{
    CHECK_INIT_RET_(0);
    I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectCollectItemCount, false);
    CHECK_NULL_RET_(pITable, 0);
    AUTO_RELEASE_(pITable);
    DWORD dwCount = 0;
    pITable->Step();
    dwCount = (DWORD)pITable->GetColInt64(0);
    return dwCount;
}

int C_eIMMsgCollectMgr::Set(const S_CollectMsg& sMsg, const TCHAR* pszXml, const TCHAR* pszTitle)
{
    CHECK_NULL_RET_(pszXml, EIMERR_INVALID_PARAM);
    CHECK_NULL_RET_(pszTitle, EIMERR_INVALID_PARAM);
    CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
    int i32Ret = 0;
    I_SQLite3Stmt*	pIStmt = NULL;
    GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
    
    GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertCollects, pIStmt, i32Ret);

    eIMStringA szXmlA;
    ::eIMTChar2UTF8(pszXml, szXmlA);

    eIMStringA szTitleA;
    ::eIMTChar2UTF8(pszTitle, szTitleA);

    i32Ret = m_pIDb->Begin();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
   
    i32Ret = pIStmt->Bind("@collectid_",	(INT64)sMsg.m_collectID);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@msg",		szXmlA.c_str());		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@title",	szTitleA.c_str());	ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@isgroup",	sMsg.m_isGroup);	ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@timestamp", (int)sMsg.m_dwTime);	ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@type",		(int)sMsg.m_eType);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@sendid",		(INT64)sMsg.m_sendid);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@needremind",		0);		ASSERT_(i32Ret == SQLITE_OK);
    i32Ret = pIStmt->Bind("@sid",		(INT64)sMsg.m_Sid);		ASSERT_(i32Ret == SQLITE_OK);
    
    i32Ret = pIStmt->Step();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    i32Ret = m_pIDb->Commit();
    ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

    return EIMERR_NO_ERROR;
}

BOOL C_eIMMsgCollectMgr::Delete(QMID collectID)
{
    CHECK_INIT_RET_(FALSE);
    const char* const pszSql = m_pIDb->VMPrintf(kDeleteCollectItem, collectID);
    int i32Ret = m_pIDb->SQL(pszSql, true);

    return (i32Ret == SQLITE_OK);
}

 BOOL C_eIMMsgCollectMgr::GetContents(QMID collectID, PFN_GetCollectContents pfnGetContents, void* pvUserData)
 {
     CHECK_INIT_RET_(FALSE);
    const char* const pszSql = m_pIDb->VMPrintf(kSelectContents, collectID);
    I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
    if(pITable)
    {
        AUTO_RELEASE_(pITable);
        if( pITable->Step() == SQLITE_ROW )
        {
            eIMString szContent;
            ::eIMUTF8ToTChar(eIMCheckColTextNull(pITable->GetColText(0)), szContent);
            pfnGetContents( szContent.c_str(), pvUserData) ;
            return TRUE;
        }
    }
    return FALSE;
 }
 
 BOOL C_eIMMsgCollectMgr::GetCollectTime(QEID UserId,  DWORD& collectTime)
 {
     CHECK_INIT_RET_(FALSE);
     const char* const pszSql = m_pIDb->VMPrintf(kSelectTime, UserId);
     I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
     if(pITable)
     {
         AUTO_RELEASE_(pITable);
         if( pITable->Step() == SQLITE_ROW )
         {
             collectTime = pITable->GetColInt(0);
             return TRUE;
         }
     }
     return FALSE;
 }

 BOOL C_eIMMsgCollectMgr::SetCollectTime(QEID UserId,  DWORD collectTime)
 {
     CHECK_INIT_RET_(EIMERR_PLUGIN_INIT_FAIL);
     int i32Ret = 0;
     I_SQLite3Stmt*	pIStmt = NULL;
     GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
     
     GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertCollectsTime, pIStmt, i32Ret);

     i32Ret = m_pIDb->Begin();
     ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

     i32Ret = pIStmt->Bind("@eid",	(INT64)UserId);		ASSERT_(i32Ret == SQLITE_OK);
     i32Ret = pIStmt->Bind("@collecttime",		(int)collectTime);		ASSERT_(i32Ret == SQLITE_OK);
     i32Ret = pIStmt->Step();
     ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

     i32Ret = m_pIDb->Commit();
     ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
     return TRUE;
 }