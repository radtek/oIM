#include "stdafx.h"
#include "C_MeetingMgr.h"

const char* const kSelectUpdateTime2       = "update_time";
const char* const kSelectUpdateTime1      = "updatetime";
const char* const ktable_basicinfo        = "t_meeting_basicinfo";
const char* const ktable_file             = "t_meeting_file";
const char* const ktable_member           = "t_meeting_member";

const char* const kcolumnvideourl         = "videourl";
const char* const kcolumnvideodownurl     = "videodownurl";
const char* const kcolumnsharedocurl      = "sharedocurl";

IMPLEMENT_PLUGIN_REF_(C_MeetingMgr, INAME_EIMENGINE_MEETINGMGR, m_dwRef);
C_MeetingMgr::C_MeetingMgr(void)
	:m_pIDb ( NULL )
    , m_dwRef(1)
	, m_HaveClear(FALSE)
{
}

C_MeetingMgr::~C_MeetingMgr(void)
{
    Uninit();
}

BOOL C_MeetingMgr::Init(I_SQLite3* pIDb)
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

BOOL C_MeetingMgr::Uninit()
{
	SAFE_RELEASE_INTERFACE_(m_pIDb);
	return TRUE;
}

DWORD  C_MeetingMgr::EnumMeeting(PFN_GetMeetingItem pfnGetMeetingItem, void *pvUserData)
{
	CHECK_INIT_RET_(0);

	I_SQLite3Table* pITable = m_pIDb->GetTable(kSelectAllBasicInfo, false);
	CHECK_NULL_RET_(pITable,0);
	AUTO_RELEASE_(pITable);

	S_ConfBasicInfo sBasicInfo = { 0 };
	while( pITable->Step() == SQLITE_ROW )
	{
		_GetRow(pITable, &sBasicInfo);
		if (!pfnGetMeetingItem(sBasicInfo, pvUserData) )
			return 1;
	}

	return 0;
}

int C_MeetingMgr::Set(const confBasicInfo* basicInfo)
{
	CHECK_NULL_RET_(basicInfo, FALSE);
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	int i32Ret = 0;
	eIMString		szValue;
	eIMUTF8ToTChar(basicInfo->strConfId,szValue);

	EIMLOGGER_INFO_(pILog,_T("BasicInfoNotice confid:%s UpdateType:%d"),szValue.c_str(),basicInfo->cUpdateType);

	if (basicInfo->cUpdateType == 3)
	{
		const char* pszSql = m_pIDb->VMPrintf(kDeleteMeetingBasicInfo,basicInfo->strConfId);
		if (SUCCEEDED(m_pIDb->SQL(pszSql, true)))
			EIMLOGGER_INFO_(pILog,_T("delete meetingbasicInfo success %s"),szValue.c_str());
		else
			EIMLOGGER_ERROR_(pILog,_T("delete meetingbasicInfo faild %s"),szValue.c_str());
		
		pszSql = m_pIDb->VMPrintf(kDeleteMeetingMbr,basicInfo->strConfId);
		if (SUCCEEDED(m_pIDb->SQL(pszSql, true)))
			EIMLOGGER_INFO_(pILog,_T("delete meetingMbrInfo success %s"),szValue.c_str());
		else
			EIMLOGGER_ERROR_(pILog,_T("delete meetingMbrInfo faild %s"),szValue.c_str());

		pszSql = m_pIDb->VMPrintf(kDeleteMeetingFileInfo,basicInfo->strConfId);
		if (SUCCEEDED(m_pIDb->SQL(pszSql, true)))
			EIMLOGGER_INFO_(pILog,_T("delete meetingFileInfo success %s"),szValue.c_str());
		else
			EIMLOGGER_ERROR_(pILog,_T("delete meetingFileInfo faild %s"),szValue.c_str());

		pszSql = m_pIDb->VMPrintf(kDeleteFileOnPathTable,basicInfo->strConfId);
		if (SUCCEEDED(m_pIDb->SQL(pszSql, true)))
			EIMLOGGER_INFO_(pILog,_T("delete FilePathInfo success %s"),szValue.c_str());
		else
			EIMLOGGER_ERROR_(pILog,_T("delete FilePathInfo faild %s"),szValue.c_str());

			return 1;
	}

	I_SQLite3Stmt*	pIStmt = NULL; 
	
	BOOL InSert = TRUE;
	const char* pszSql = m_pIDb->VMPrintf(kSelectMeetingExist,basicInfo->strConfId);
	if(MeetingExists(basicInfo->strConfId,pszSql))
	{
		InSert = FALSE;
	}

	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, InSert?kInsertMeetingBasicInfo:kUpdateMeetingBasicInfo, pIStmt, i32Ret);
	
	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	int UpdateType = (INT32)basicInfo->cUpdateType;
	if (UpdateType > 3 || UpdateType < 0)
	{
		EIMLOGGER_ERROR_(pILog,_T("error conference  confid:%s UpdateType:%d "),szValue.c_str(),UpdateType);
		UpdateType = 1;
	}

	INT32 UpdateTime = (INT32)basicInfo->dwUpdateTime;
	INT32 TimeNow = hpi.GetTimeNow();
	if (UpdateTime > TimeNow || UpdateTime < 0)
	{
		EIMLOGGER_ERROR_(pILog,_T("error conference  confid:%s UpdateTime:%d  TimeNow:%d"),szValue.c_str(),UpdateTime,TimeNow);
		UpdateTime = 0;
	}

	i32Ret = pIStmt->Bind("@confid_",	basicInfo->strConfId);			        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@hostcode",	basicInfo->strHostCode);		        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@mbrcode", basicInfo->strMbrCode);			        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@conftitle", basicInfo->strConfTitle);		        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@starttime",	  (INT32)basicInfo->dwStartTime);	        ASSERT_(i32Ret == SQLITE_OK);      
	i32Ret = pIStmt->Bind("@endtime", 	  (INT32)basicInfo->dwEndTime);	        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@conflength",  (INT32)basicInfo->dwConfLength);        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@confstate",   (INT32)basicInfo->cConfStatus);	        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@creatoracct", basicInfo->strCreatorAcct);	        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@creatorid",   (INT32)basicInfo->dwcreatorID);	        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@conftype",    (INT32)basicInfo->cConfType);	        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@isrepeat",    (INT32)basicInfo->cIsRepeat);	        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@repeatkey" ,  basicInfo->strRepeatKey) ;            ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@confmode" ,   (INT32)basicInfo->cConfMode) ;          ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@reallength" , (INT32)basicInfo->dwRealLength) ;       ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@mbrmaxnum" ,  (INT32)basicInfo->dwMbrMaxNum) ;       ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@mbrurl" ,     basicInfo->strMbrUrl) ;               ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@hosturl" ,    basicInfo->strHostUrl) ;              ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@location" ,   basicInfo->strLocation) ;             ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@updatetype" , UpdateType) ;                        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@updatetime" , UpdateTime) ;                        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@conflevel" ,  3) ;                                  ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@smsnotice" ,   (INT32)basicInfo->cSMSNotice) ;        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@dnumbernum" ,   (INT32)basicInfo->dwPartNum) ;        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@dfilenum" ,   (INT32)basicInfo->dwFileNum) ;        ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@realstarttime", (INT32)basicInfo->dwRealStartTime);   ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@realendtime",  (INT32)basicInfo->dwRealEndTime);     ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	char logtext[2048] = { 0 };
	snprintf(logtext, COUNT_OF_ARRAY_(logtext)-1,"confid_:%s, hostcode:%s, mbrcode:%s, conftitle:%s, starttime:%d, endtime:%d, conflength:%d, confstate:%d, creatoracct:%s, creatorid:%d,conftype:%d, isrepeat:%d,repeatkey:%s, confmode:%d,reallength:%d, mbrurl:%s, hosturl:%s, location:%s,updatetype:%d,updateType1:%d, updatetime:%d, updatetime1:%d, conflevel:%d smsnotice:%d dnumbernum:%d dfilenum:%d dwrelstarttime:%d dwrelendtime:%d",
		basicInfo->strConfId,basicInfo->strHostCode,basicInfo->strMbrCode,basicInfo->strConfTitle,(INT32)basicInfo->dwStartTime,(INT32)basicInfo->dwEndTime,
		(INT32)basicInfo->dwConfLength,(INT32)basicInfo->cConfStatus,basicInfo->strCreatorAcct,(INT32)basicInfo->dwcreatorID,(INT32)basicInfo->cConfType,(INT32)basicInfo->cIsRepeat,
		basicInfo->strRepeatKey,(INT32)basicInfo->cConfMode,(INT32)basicInfo->dwRealLength,basicInfo->strMbrUrl,basicInfo->strHostUrl,basicInfo->strLocation,(INT32)basicInfo->cUpdateType,UpdateType,
		(INT32)basicInfo->dwUpdateTime,UpdateTime,3,(INT32)basicInfo->cSMSNotice,(INT32)basicInfo->dwPartNum,(INT32)basicInfo->dwFileNum,(INT32)basicInfo->dwRealStartTime,(INT32)basicInfo->dwRealEndTime);

	eIMString temp;
	eIMUTF8ToTChar(logtext,temp);
	EIMLOGGER_INFO_(pILog,_T("%s"),temp.c_str());
	logtext[0] = '\0';

	return TRUE;
}

int C_MeetingMgr::Set(const confRemarksNotice* remarkInfo)
{
	CHECK_NULL_RET_(remarkInfo, FALSE);
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	eIMString		szValue;
	eIMString       szRemark;
	BOOL IsExists = FALSE;
	eIMUTF8ToTChar(remarkInfo->strConfId,szValue);
	eIMUTF8ToTChar(remarkInfo->strConfRemark,szRemark);

	const char* pszSql = m_pIDb->VMPrintf(kSelectMeetingExist,remarkInfo->strConfId);
	if (MeetingExists(remarkInfo->strConfId,pszSql))
		IsExists = TRUE;
		
	EIMLOGGER_INFO_(pILog,_T("RemarksNotice confid:%s ReceiveId:%d Remark;%s MeetingExists:%d"),szValue.c_str(),remarkInfo->dwUserId,szRemark.c_str(),IsExists);

	pszSql = m_pIDb->VMPrintf(IsExists?kUpdateBasicRemark:kInsertBasicRemark,remarkInfo->strConfRemark,remarkInfo->strConfId);
	int i32Ret = m_pIDb->SQL(pszSql,true);
	
	if (SUCCEEDED(i32Ret))
	{
		EIMLOGGER_INFO_(pILog,_T("Update RemarksNotice Success confid:%s "),szValue.c_str());
		return 1;
	}
	else
	{
		EIMLOGGER_INFO_(pILog,_T("Update RemarksNotice Faild confid:%s "),szValue.c_str());
		return 0;
	}
}

int C_MeetingMgr::Set(const confMbrInfoNotice* mbrInfo)
{
	CHECK_NULL_RET_(mbrInfo, FALSE);
	CHECK_INIT_RET_(FALSE);

    if (mbrInfo->wMbrNum <=0 )
    {
        return TRUE;
    }

	int i32Ret = 0;
	eIMStringA		szValueA;
	I_SQLite3Stmt*	pIStmt = NULL; 
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	EIMLOGGER_INFO_(pILog,_T("MbrInfoNotice dwUserId:%d cTerminal:%d cOperType:%d cDataType:%d wMbrNum:%d"),mbrInfo->dwUserId,mbrInfo->cTerminal,mbrInfo->cOperType,mbrInfo->cDataType,mbrInfo->wMbrNum);

	eIMString tempConfid;
	eIMUTF8ToTChar(mbrInfo->strConfId,tempConfid);

	if(mbrInfo->cDataType == 2 || mbrInfo->cDataType == 3)
	{
		const char* pszSql;

		for (int i = 0 ; i < mbrInfo->wMbrNum; i++)
		{
			pszSql = m_pIDb->VMPrintf(kDeleteMeetingMbrInfo,mbrInfo->strConfId,mbrInfo->sMbrList[i].dwMbrId);
			i32Ret = m_pIDb->SQL(pszSql, true);

			if (SUCCEEDED(i32Ret))
				EIMLOGGER_INFO_(pILog,_T("Delete mbr Success %s,%u"),tempConfid.c_str(),mbrInfo->sMbrList[i].dwMbrId);
			else
				EIMLOGGER_ERROR_(pILog,_T("Delete mbr faild %s,%u"),tempConfid.c_str(),mbrInfo->sMbrList[i].dwMbrId);
		}

		if (mbrInfo->cDataType == 3)
			return TRUE;
	}
		
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertMeetingMbrInfo, pIStmt, i32Ret);

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	for (int j = 0; j < mbrInfo->wMbrNum ; j++)
	{
		i32Ret = pIStmt->Bind("@confid",	mbrInfo->strConfId);		ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@mbrid",	    (INT32)mbrInfo->sMbrList[j].dwMbrId);		                        ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@dwoperaid", (INT32)mbrInfo->sMbrList[j].dwOperaId);			                ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@isaccept",  (INT32)mbrInfo->sMbrList[j].cIsAccept);			                ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@updatetime", (long)mbrInfo->sMbrList[j].dwUpdateTime);	    ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@updatetype", (INT32)mbrInfo->cOperType);	              ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@conflevel", (int)mbrInfo->sMbrList[j].cConfLevel);		    ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@cisread",  (INT32)mbrInfo->sMbrList[j].cIsRead);		    ASSERT_(i32Ret == SQLITE_OK);


		char logtext[2048] = { 0 };
		eIMString szTempLog;
		snprintf(logtext,COUNT_OF_ARRAY_(logtext),"MbrInfo:confid:%s mbrid:%d dwoperaid:%d isaccept:%d updatetime:%lld updatetype:%d conflevel:%d cisread:%d",
			mbrInfo->strConfId,(INT32)mbrInfo->sMbrList[j].dwMbrId,(INT32)mbrInfo->sMbrList[j].dwOperaId,(INT32)mbrInfo->sMbrList[j].cIsAccept,(long)mbrInfo->sMbrList[j].dwUpdateTime,(INT32)mbrInfo->cOperType,(int)mbrInfo->sMbrList[j].cConfLevel,(INT32)mbrInfo->sMbrList[j].cIsRead);
		eIMUTF8ToTChar(logtext,szTempLog);
		EIMLOGGER_INFO_(pILog,_T("Insert Member %s"),szTempLog.c_str());
		logtext[0] = '\0';

		i32Ret = pIStmt->Step();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);
	}

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return TRUE;
}

int C_MeetingMgr::Set(const confFileInfoNotice* fileInfo)
{
	CHECK_NULL_RET_(fileInfo, FALSE);
	CHECK_INIT_RET_(FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	if (fileInfo->wFileNum <= 0 )
	{
		return TRUE;
	}

	int i32Ret = 0;

	if (fileInfo->cDataType == 2 || fileInfo->cDataType == 3)
	{
		const char* pszSql;
		eIMString filename,confid;
		
		for (int j = 0; j < fileInfo->wFileNum ; j++)
		{
			pszSql = m_pIDb->VMPrintf(kDeleteMeetingFile,fileInfo->strConfId,fileInfo->sFileList[j].fileToken);
			i32Ret = m_pIDb->SQL(pszSql, true);

			eIMUTF8ToTChar(fileInfo->strConfId,confid);
			eIMUTF8ToTChar(fileInfo->sFileList[j].fileName,filename);

			DeletePathTableFile(fileInfo->strConfId,fileInfo->sFileList[j].fileToken);

			if (i32Ret)
				EIMLOGGER_INFO_(pILog,_T("Delete FileInfo success %s %s %d"),confid.c_str(),filename.c_str(),fileInfo->cDataType);
			else
				EIMLOGGER_ERROR_(pILog,_T("Delete FileInfo faild %s %s %d"),confid.c_str(),filename.c_str(),fileInfo->cDataType);
		}
		if (fileInfo->cDataType == 3)
			return 1;
	}

	eIMStringA		szValueA;
	I_SQLite3Stmt*	pIStmt = NULL; 
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kInsertMeetingFileInfo, pIStmt, i32Ret);
	EIMLOGGER_INFO_(pILog,_T("Insert FileInfo to DB"));

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	confFileInfo sconfFileItemInfo;
	for (int i = 0; i < fileInfo->wFileNum ; i++)
	{
		sconfFileItemInfo = fileInfo->sFileList[i];

		i32Ret = pIStmt->Bind("@confid",	fileInfo->strConfId);		ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@userid",	(INT32)fileInfo->dwUserId);		                        ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@cterminal", fileInfo->cTerminal);			                ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@copertype",  fileInfo->cOperType);			                ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@updatetime", (INT32)sconfFileItemInfo.dwUpdateTime);	    ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@filesize", (INT32)sconfFileItemInfo.fileSize);		    ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@filename", sconfFileItemInfo.fileName);		    ASSERT_(i32Ret == SQLITE_OK);
		i32Ret = pIStmt->Bind("@url",      sconfFileItemInfo.fileToken);		    ASSERT_(i32Ret == SQLITE_OK);

		i32Ret = pIStmt->Step();
		ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

		char logtext[2048] = { 0 };
		eIMString szTempLog;
		snprintf(logtext,COUNT_OF_ARRAY_(logtext),"fileInfo:confid:%s userid:%d cterminal:%d copertype:%d updatetime:%d filesize:%d filename:%s url:%s",
			fileInfo->strConfId,(INT32)fileInfo->dwUserId,(INT32)fileInfo->cTerminal,(INT32)fileInfo->cOperType,(INT32)sconfFileItemInfo.dwUpdateTime,
			(INT32)sconfFileItemInfo.fileSize,sconfFileItemInfo.fileName,sconfFileItemInfo.fileToken);
		eIMUTF8ToTChar(logtext,szTempLog);
		EIMLOGGER_INFO_(pILog,_T("Insert into File %s"),szTempLog.c_str());

		logtext[0] = '\0';
	}

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return TRUE;
}

int C_MeetingMgr::SetMeetingUrl(const char* confid,const char* url,tagMeetingUrlColumn urlType)
{
	CHECK_INIT_RET_(0);
	const  char* pszSql = NULL;
	switch(urlType)
	{
	case eVideoUrl:
		pszSql = m_pIDb->VMPrintf(kSetMeetingUrl,kcolumnvideourl,url,confid);
		break;
	case eVideoDownUrl:
		pszSql = m_pIDb->VMPrintf(kSetMeetingUrl,kcolumnvideodownurl,url,confid);
		break;
	case eDocDownUrl:
		pszSql = m_pIDb->VMPrintf(kSetMeetingUrl,kcolumnsharedocurl,url,confid);
		break;
	}

	if (pszSql)
		return m_pIDb->SQL(pszSql,true);

	return 0;
}


int C_MeetingMgr::Get(char* confId,PFN_MeetingBasicInfoCb pfnCb,void* pvUserData)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectBasicInfo,confId);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,0);
	AUTO_RELEASE_(pITable);

	confBasicInfo sBasicInfo = { 0 };
	int i32Ret = 0;
	while( pITable->Step() == SQLITE_ROW )
	{
		i32Ret = _GetRow(pITable, &sBasicInfo);
		if (!pfnCb(&sBasicInfo, pvUserData) )
			return 1;
	}

	return i32Ret;
}

int C_MeetingMgr::Get(char* confId,PFN_MeetingBasicInfoCb_All pfnCb,void* pvUserData)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectBasicInfo,confId);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,0);
	AUTO_RELEASE_(pITable);

	S_ConfBasicInfo sBasicInfo = { 0 };
	int i32Ret = 0;
	while( pITable->Step() == SQLITE_ROW )
	{
		i32Ret = _GetRow(pITable, &sBasicInfo);
		if (!pfnCb(&sBasicInfo, pvUserData) )
			return 1;
	}

	return i32Ret;
}

int C_MeetingMgr::Get(char* confId,PFN_MeetingBasicMbrInfoCb pfnCb,void* pvUserData)
{
	CHECK_INIT_RET_(0);
	const char* pszSql = m_pIDb->VMPrintf(kSelectBasicMbrInfo,confId);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,0);
	AUTO_RELEASE_(pITable);

	confBasicMbrInfo sBasicMbrInfo = { 0 };
	while( pITable->Step() == SQLITE_ROW )
	{
		_GetRow(pITable, &sBasicMbrInfo);
		if (!pfnCb(&sBasicMbrInfo, pvUserData) )
			return 1;
	}

	return 1;
}

int C_MeetingMgr::Get(char* confId,PFN_MeetingFileInfoCb pfnCb,void* pvUserData)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kSelectMeetingFiles,confId);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,0);
	AUTO_RELEASE_(pITable);

	S_FileInf2Detail sFileInfo;
	while( pITable->Step() == SQLITE_ROW )
	{
		_GetRow(pITable, &sFileInfo);
		if ( !pfnCb(&sFileInfo, pvUserData) )
			return 1;
	}

	return 1;
}

BOOL C_MeetingMgr::_GetRow(I_SQLite3Table* pITable, confBasicMbrInfo* psBasicMbrInfo) 
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pITable, FALSE);
	CHECK_NULL_RET_(psBasicMbrInfo, FALSE);

	psBasicMbrInfo->dwMbrId		 = pITable->GetColInt(1);
	psBasicMbrInfo->dwOperaId	 = pITable->GetColInt(2);
	psBasicMbrInfo->cIsAccept	 = pITable->GetColInt(3);
	psBasicMbrInfo->dwUpdateTime = pITable->GetColInt(4);
	psBasicMbrInfo->cConfLevel	 = pITable->GetColInt(6);

	return TRUE;
}

BOOL C_MeetingMgr::_GetRow(I_SQLite3Table* pITable, confBasicInfo* psBasicInfo) 
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pITable, FALSE);
	CHECK_NULL_RET_(psBasicInfo, FALSE);

	const unsigned char* pszValueA    = NULL;
	eIMString		szValue;

	memset(psBasicInfo, 0, sizeof(confBasicInfo)); 
	strncpy(psBasicInfo->strConfId,   eIMCheckColTextNull(pITable->GetColText(0)),COUNT_OF_ARRAY_(psBasicInfo->strConfId)-1);
	strncpy(psBasicInfo->strHostCode, eIMCheckColTextNull(pITable->GetColText(1)),COUNT_OF_ARRAY_(psBasicInfo->strHostCode)-1);
	strncpy(psBasicInfo->strMbrCode,  eIMCheckColTextNull(pITable->GetColText(2)),COUNT_OF_ARRAY_(psBasicInfo->strMbrCode)-1);
	strncpy(psBasicInfo->strConfTitle,eIMCheckColTextNull(pITable->GetColText(3)),COUNT_OF_ARRAY_(psBasicInfo->strConfTitle)-1);
	psBasicInfo->dwStartTime  = pITable->GetColInt(4);
	psBasicInfo->dwEndTime    = pITable->GetColInt(5);
	psBasicInfo->dwConfLength = pITable->GetColInt(6);
	psBasicInfo->cConfStatus  = pITable->GetColInt(7);
	strncpy(psBasicInfo->strCreatorAcct,eIMCheckColTextNull(pITable->GetColText(8)),COUNT_OF_ARRAY_(psBasicInfo->strCreatorAcct)-1);
	psBasicInfo->dwcreatorID  = pITable->GetColInt(9);
	psBasicInfo->cConfType    = pITable->GetColInt(10);
	psBasicInfo->cIsRepeat    = pITable->GetColInt(11);
	//psBasicInfo->cRepeatType  = pITable->GetColInt(12);
	strncpy(psBasicInfo->strRepeatKey, eIMCheckColTextNull(pITable->GetColText(13)),COUNT_OF_ARRAY_(psBasicInfo->strRepeatKey)-1);
	psBasicInfo->cConfMode    = pITable->GetColInt(14);
	//psBasicInfo->dwConfMsgId  = pITable->GetColInt64(15);
	psBasicInfo->dwRealLength = pITable->GetColInt(16);
	psBasicInfo->dwMbrMaxNum  = pITable->GetColInt(17);
	strncpy(psBasicInfo->strMbrUrl,    eIMCheckColTextNull(pITable->GetColText(18)),COUNT_OF_ARRAY_(psBasicInfo->strMbrUrl)-1);
	strncpy(psBasicInfo->strHostUrl,   eIMCheckColTextNull(pITable->GetColText(19)),COUNT_OF_ARRAY_(psBasicInfo->strHostUrl)-1);
	strncpy(psBasicInfo->strLocation,  eIMCheckColTextNull(pITable->GetColText(20)),COUNT_OF_ARRAY_(psBasicInfo->strLocation)-1);
	//strncpy(psBasicInfo->strConfRemark,eIMCheckColTextNull(pITable->GetColText(21)),COUNT_OF_ARRAY_(psBasicInfo->strConfRemark)-1);
	psBasicInfo->cUpdateType  = pITable->GetColInt(22);
	psBasicInfo->dwUpdateTime = pITable->GetColInt(23);
	//psBasicInfo->cConfLevel = pITable->GetColInt(24);
	psBasicInfo->cSMSNotice   = pITable->GetColInt(27);
	psBasicInfo->dwPartNum    = pITable->GetColInt(28);
	psBasicInfo->dwFileNum    = pITable->GetColInt(29);
	psBasicInfo->dwRealStartTime = pITable->GetColInt(32);
	psBasicInfo->dwRealEndTime = pITable->GetColInt(33);

	return TRUE;
}

BOOL C_MeetingMgr::_GetRow(I_SQLite3Table* pITable, S_ConfBasicInfo* psBasicInfo) 
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pITable, FALSE);
	CHECK_NULL_RET_(psBasicInfo, FALSE);
	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);

	const unsigned char* pszValueA    = NULL;
	eIMString		szValue;

	memset(psBasicInfo, 0, sizeof(S_ConfBasicInfo)); 
	strncpy(psBasicInfo->basicInfo.strConfId,   eIMCheckColTextNull(pITable->GetColText(0)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strConfId)-1);
	strncpy(psBasicInfo->basicInfo.strHostCode, eIMCheckColTextNull(pITable->GetColText(1)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strHostCode)-1);
	strncpy(psBasicInfo->basicInfo.strMbrCode,  eIMCheckColTextNull(pITable->GetColText(2)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strMbrCode)-1);
	strncpy(psBasicInfo->basicInfo.strConfTitle,eIMCheckColTextNull(pITable->GetColText(3)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strConfTitle)-1);
	psBasicInfo->basicInfo.dwStartTime  = pITable->GetColInt(4);
	psBasicInfo->basicInfo.dwEndTime    = pITable->GetColInt(5);
	psBasicInfo->basicInfo.dwConfLength = pITable->GetColInt(6);
	psBasicInfo->basicInfo.cConfStatus  = pITable->GetColInt(7);
	strncpy(psBasicInfo->basicInfo.strCreatorAcct,eIMCheckColTextNull(pITable->GetColText(8)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strCreatorAcct)-1);
	psBasicInfo->basicInfo.dwcreatorID  = pITable->GetColInt(9);
	psBasicInfo->basicInfo.cConfType    = pITable->GetColInt(10);
	psBasicInfo->basicInfo.cIsRepeat    = pITable->GetColInt(11);
	//psBasicInfo->basicInfo.cRepeatType  = pITable->GetColInt(12);
	strncpy(psBasicInfo->basicInfo.strRepeatKey, eIMCheckColTextNull(pITable->GetColText(12)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strRepeatKey)-1);
	psBasicInfo->basicInfo.cConfMode    = pITable->GetColInt(13);
	//psBasicInfo->basicInfo.dwConfMsgId  = pITable->GetColInt64(15);
	psBasicInfo->basicInfo.dwRealLength = pITable->GetColInt(14);
	psBasicInfo->basicInfo.dwMbrMaxNum  = pITable->GetColInt(15);
	strncpy(psBasicInfo->basicInfo.strMbrUrl,    eIMCheckColTextNull(pITable->GetColText(16)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strMbrUrl)-1);
	strncpy(psBasicInfo->basicInfo.strHostUrl,   eIMCheckColTextNull(pITable->GetColText(17)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strHostUrl)-1);
	strncpy(psBasicInfo->basicInfo.strLocation,  eIMCheckColTextNull(pITable->GetColText(18)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strLocation)-1);
	strncpy(psBasicInfo->Remark,eIMCheckColTextNull(pITable->GetColText(19)),COUNT_OF_ARRAY_(psBasicInfo->Remark)-1);
	//strncpy(psBasicInfo->basicInfo.strConfRemark,eIMCheckColTextNull(pITable->GetColText(21)),COUNT_OF_ARRAY_(psBasicInfo->basicInfo.strConfRemark)-1);
	psBasicInfo->basicInfo.cUpdateType  = pITable->GetColInt(20);
	psBasicInfo->basicInfo.dwUpdateTime = pITable->GetColInt(21);
	psBasicInfo->basicInfo.cSMSNotice   = pITable->GetColInt(22);
	psBasicInfo->basicInfo.dwPartNum    = pITable->GetColInt(23);
	psBasicInfo->basicInfo.dwFileNum    = pITable->GetColInt(24);
	strncpy(psBasicInfo->videoUrl,eIMCheckColTextNull(pITable->GetColText(25)),COUNT_OF_ARRAY_(psBasicInfo->videoUrl)-1);
	psBasicInfo->basicInfo.dwRealStartTime = pITable->GetColInt(26);
	psBasicInfo->basicInfo.dwRealEndTime   = pITable->GetColInt(27); 
	strncpy(psBasicInfo->ShareDocUrl,eIMCheckColTextNull(pITable->GetColText(28)),COUNT_OF_ARRAY_(psBasicInfo->ShareDocUrl)-1);
	strncpy(psBasicInfo->videoDownUrl,eIMCheckColTextNull(pITable->GetColText(29)),COUNT_OF_ARRAY_(psBasicInfo->videoDownUrl)-1);

	char logtext[2048] = { 0 };
	snprintf(logtext, COUNT_OF_ARRAY_(logtext)-1,"Get Meeting Basic Info confid_:%s, hostcode:%s, mbrcode:%s, conftitle:%s, starttime:%d, endtime:%d, conflength:%d, confstate:%d, creatoracct:%s, creatorid:%d,conftype:%d, isrepeat:%d,repeatkey:%s, confmode:%d,reallength:%d, mbrurl:%s, hosturl:%s, location:%s,updatetype:%d, updatetime:%d conflevel:%d smsnotice:%d dnumbernum:%d dfilenum:%d videoUrl:%s dwrelstarttime:%d dwrelendtime:%d remark:%s",
		psBasicInfo->basicInfo.strConfId,psBasicInfo->basicInfo.strHostCode,psBasicInfo->basicInfo.strMbrCode,psBasicInfo->basicInfo.strConfTitle,(INT32)psBasicInfo->basicInfo.dwStartTime,(INT32)psBasicInfo->basicInfo.dwEndTime,
		(INT32)psBasicInfo->basicInfo.dwConfLength,(INT32)psBasicInfo->basicInfo.cConfStatus,psBasicInfo->basicInfo.strCreatorAcct,(INT32)psBasicInfo->basicInfo.dwcreatorID,(INT32)psBasicInfo->basicInfo.cConfType,(INT32)psBasicInfo->basicInfo.cIsRepeat,
		psBasicInfo->basicInfo.strRepeatKey,(INT32)psBasicInfo->basicInfo.cConfMode,(INT32)psBasicInfo->basicInfo.dwRealLength,psBasicInfo->basicInfo.strMbrUrl,psBasicInfo->basicInfo.strHostUrl,psBasicInfo->basicInfo.strLocation,(INT32)psBasicInfo->basicInfo.cUpdateType,
		(INT32)psBasicInfo->basicInfo.dwUpdateTime,3,(INT32)psBasicInfo->basicInfo.cSMSNotice,(INT32)psBasicInfo->basicInfo.dwPartNum,(INT32)psBasicInfo->basicInfo.dwFileNum,psBasicInfo->videoUrl,(INT32)psBasicInfo->basicInfo.dwRealStartTime,(INT32)psBasicInfo->basicInfo.dwRealEndTime,psBasicInfo->Remark);

	eIMString temp;
	eIMUTF8ToTChar(logtext,temp);
	EIMLOGGER_INFO_(pILog,_T("%s"),temp.c_str());

	return TRUE;
}

BOOL C_MeetingMgr::_GetRow(I_SQLite3Table* pITable, S_FileInf2Detail* psFileInfo)
{
	CHECK_INIT_RET_(FALSE);
	CHECK_NULL_RET_(pITable, FALSE);
	CHECK_NULL_RET_(psFileInfo, FALSE);

	strncpy(psFileInfo->fileToken, eIMCheckColTextNull(pITable->GetColText(0)),COUNT_OF_ARRAY_(psFileInfo->fileToken)-1);
	strncpy(psFileInfo->fileName,  eIMCheckColTextNull(pITable->GetColText(1)),COUNT_OF_ARRAY_(psFileInfo->fileName)-1);
	strncpy(psFileInfo->filePath, "",COUNT_OF_ARRAY_(psFileInfo->filePath)-1);
	psFileInfo->fileSize = pITable->GetColInt(2);
	psFileInfo->dwUpdateTime = pITable->GetColInt(3);

	const char* pszSql = m_pIDb->VMPrintf(kSelectMeetingFilePath,eIMCheckColTextNull(pITable->GetColText(4)),eIMCheckColTextNull(pITable->GetColText(0)));
	I_SQLite3Table* pITable1 = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable1,0);
	AUTO_RELEASE_(pITable1);

	if(pITable1->Step() == SQLITE_ROW)
		strncpy(psFileInfo->filePath,eIMCheckColTextNull(pITable1->GetColText(0)),COUNT_OF_ARRAY_(psFileInfo->filePath)-1);

	return TRUE;
}

int C_MeetingMgr::GetMeetingTimeStamp()
{
	CHECK_INIT_RET_(0);
	INT32 i32Value = 0;
	int TimeStampe = 0;
	int TimeNow = hpi.GetTimeNow();
    i32Value = _GetOneResult(m_pIDb->VMPrintf(kSelectUpdateTime,kSelectUpdateTime1,ktable_basicinfo,kSelectUpdateTime1,TimeNow), true);
	if (i32Value > TimeStampe)
		TimeStampe = i32Value;
	return TimeStampe;
}

int C_MeetingMgr::GetTodayMeetingCount(QEID userid,int &count)
{
	CHECK_INIT_RET_(0);

	time_t now = time(NULL);
	tm *tnow = localtime(&now);
	tnow->tm_hour = 0;
	tnow->tm_min = 0;
	time_t start = mktime(tnow); 
	tnow->tm_hour = 23;
	tnow->tm_min = 59;
	time_t stend = mktime(tnow);

	return _GetOneResult(m_pIDb->VMPrintf(kSelectTadayMeetingCount,start,stend), true);
}

int C_MeetingMgr::UpdateMeetingLevel(const char* confId,QEID qeid,int Level)
{
	CHECK_INIT_RET_(0);
	const char* pszSql = m_pIDb->VMPrintf(kUpdateMeetingLevel,Level,confId,qeid);
	return m_pIDb->SQL(pszSql, true);
}

int C_MeetingMgr::GetMeetingLevel(char* confId,QEID qeid)
{
	CHECK_INIT_RET_(0);
	return _GetOneResult(m_pIDb->VMPrintf(kSelectMeetingLevel,confId,qeid), true);
}

int C_MeetingMgr::GetMeetingCreator(const char* confid)
{
	CHECK_INIT_RET_(0);
	return _GetOneResult(m_pIDb->VMPrintf(kSelectMeetingCreator,confid), true);
}

int C_MeetingMgr::UpdateFilePath(const char* confid,const TCHAR* fileToken,const TCHAR* filePath,long timeKey,int flag)
{
	CHECK_INIT_RET_(0);
	CHECK_NULL_RET_(fileToken,0);
	CHECK_NULL_RET_(filePath,0);

	ClearFilePathTable();

	GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(RET_HR);
	int i32Ret = 0;
	const char* pszSql = NULL;
	eIMStringA tempToken,tempPath;
	eIMTChar2UTF8(fileToken,tempToken);
	eIMTChar2UTF8(filePath,tempPath);

	I_SQLite3Stmt*	pIStmt = NULL; 
	GET_STMT_AUTO_RELEASE_RET_(m_pIDb, kUpdateMeetingFileInfo, pIStmt, i32Ret);
	EIMLOGGER_INFO_(pILog,_T("Insert FileInfo to DB"));

	i32Ret = m_pIDb->Begin();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	i32Ret = pIStmt->Bind("@confid",confid);		ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@token",	tempToken.c_str());	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@key",	(long)timeKey);	ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@path",  tempPath.c_str()); ASSERT_(i32Ret == SQLITE_OK);
	i32Ret = pIStmt->Bind("@flag", (int)flag); ASSERT_(i32Ret == SQLITE_OK);

	i32Ret = pIStmt->Step();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	char logtext[1024] = { 0 };
	eIMString szTempLog;
	snprintf(logtext,COUNT_OF_ARRAY_(logtext),"insert file path:confid:%s token:%s key:%ld path:%s ",confid,tempToken.c_str(),timeKey,tempPath.c_str());
	
	eIMUTF8ToTChar(logtext,szTempLog);
	EIMLOGGER_INFO_(pILog,_T("%s"),szTempLog.c_str());
	logtext[0] = '\0';

	i32Ret = m_pIDb->Commit();
	ROLLBACK_FAILED_RET_(m_pIDb, i32Ret);

	return TRUE;
}

int C_MeetingMgr::UpdateMeetingIsRead(char* confId,QEID qeid,int IsRead)
{
	CHECK_INIT_RET_(0);
	const char* pszSql = m_pIDb->VMPrintf(kUpdateMeetingIsRead,IsRead,confId,qeid);
	return m_pIDb->SQL(pszSql, true);
}

int C_MeetingMgr::GetMeetingIsRead(char* confId,QEID qeid)
{
	CHECK_INIT_RET_(0);
	return _GetOneResult(m_pIDb->VMPrintf(kSelectMeetingIsRead,confId,qeid), true);
}

int C_MeetingMgr::GetMeetingIsAccept(const char* confId,QEID qeid)
{
	CHECK_INIT_RET_(0);
	return _GetOneResult(m_pIDb->VMPrintf(kSelectMeetingIsAccept,confId,qeid), true);
}

BOOL C_MeetingMgr::CheckeFileIsExist(char* confid,char* token,long timeKey)
{
	CHECK_INIT_RET_(0);

	const char* pszSql = m_pIDb->VMPrintf(kCheckeFileExist1,confid,token);
	const char* pszSql1 = m_pIDb->VMPrintf(kCheckeFileExist2,confid,token,timeKey);
	I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true);
	CHECK_NULL_RET_(pITable,0);
	AUTO_RELEASE_(pITable);

	I_SQLite3Table* pITable1 = m_pIDb->GetTable(pszSql1, true);
	CHECK_NULL_RET_(pITable1,0);
	AUTO_RELEASE_(pITable1);

	int inFileTable = 0;
	while( pITable->Step() == SQLITE_ROW )
	{
		pszSql = m_pIDb->VMPrintf(kCheckeFileExist3,confid,token,timeKey);
		I_SQLite3Table* pITable3 = m_pIDb->GetTable(pszSql, true);
		CHECK_NULL_RET_(pITable3,0);
		AUTO_RELEASE_(pITable3);

		inFileTable = 2;
		if(pITable3->Step() == SQLITE_ROW)
		{
			inFileTable = 1;
			break;
		}
	}

	if (inFileTable == 2)
		return TRUE;

	if( pITable1->Step() == SQLITE_ROW )
	{
		return TRUE;
	}

	return FALSE;
}

int C_MeetingMgr::DeletePathTableFile(const char* confid,const char* token)
{
	CHECK_INIT_RET_(0);
	const char* pszSql = m_pIDb->VMPrintf(kDeletePathTableFile,confid,token);
	return m_pIDb->SQL(pszSql, true);
}

int C_MeetingMgr::ClearFilePathTable()
{
	if (m_HaveClear)
		return 0;

	CHECK_INIT_RET_(0);
	int ret = m_pIDb->SQL(kClearpathTable);
	m_HaveClear = TRUE;

	return ret;
}

INT32 C_MeetingMgr::_GetOneResult(const char* pszSql, bool bFree)
{
	CHECK_INIT_RET_(0);
	if ( I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, bFree))
	{
		AUTO_RELEASE_(pITable);
		if( pITable->Step() == SQLITE_ROW )
        {
			return pITable->GetColInt(0);
        }
	}

	return 0;
}

BOOL C_MeetingMgr::MeetingExists(const char* confid,const char* pszSql)
{
	CHECK_INIT_RET_(0);
	if ( I_SQLite3Table* pITable = m_pIDb->GetTable(pszSql, true))
	{
		AUTO_RELEASE_(pITable);
		if( pITable->Step() == SQLITE_ROW )
		{
			return TRUE;
		}
	}
	return FALSE;
}