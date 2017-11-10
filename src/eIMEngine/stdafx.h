// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define Eng C_eIMEngine::GetObject()

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <objbase.h>
#include <atlimage.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <windows.h>
#include "Public\Utils.h"
#include "..\ClientAgent\project\IClientAgent.h"			// ClientAgent 的引用无法将 *.h 抽出来放到 inc 中，这里只好直接引用 src
#include "eIMUICore\eIMDbConst.h"
#include "StringMgr.h"
#include "eIMEngine\IeIMEngine.h"
#include "eIMLogger\IeIMLogger.h"
#include "eIMUpdater\IeIMUpdater.h"
#include "eIMFileStorage\IeIMInternetFile.h"
#include "Public\XmlConfiger.h"
#include "eIMUICore\eIMConfigConst.h"
//#include "eIMZip\IeIMZip.h"
#include "C_eIMSessionMgr.h"
#include "C_eIMContacts.h"
#include "C_eIMMsgMgr.h"
#include "C_eIMMsgCollectMgr.h"
#include "curl\curl.h"
#include "C_eIMEngine.h"
#include "C_eIMContactsMgr.h"
#include "C_eIMLoginUserMgr.h"
#include "C_eIMFileMgr.h"
#include "C_eIMStateSubscribeMgr.h"
#include "C_eIMRoamDataMgr.h"
#include "XmlEmoticon.h"
#include "C_eIMSMSMgr.h"
#include "C_eIMBroMgr.h"
#include "C_eIMUIWaitOverMgr.h"
#include "C_eIMMsgReadMgr.h"
#include "eIMEngine\IeIMImportMsgMgr.h"
#include "C_eIMImportMsgMgr.h"
#include "C_eIMUrlFileMgr.h"
#include "C_eIMMsgReadErrorMgr.h"
#include "C_eIMMsgCollectMgr.h"
#include "C_MeetingMgr.h"
#include "eIMEngine/IMeetingMgr.h"
#include "md5\md5_storage.h"
#include "vld\vld.h"
#include "eIMUICore\C_HelpAPI.h"

#include "resource.h"

HRESULT GetEngineInterface(LPCTSTR lpctszName, void** ppvIObj);

#define RET_HR		0
#define RET_NULL	1
#define RET_FALSE	2
template<typename T>	T ReturnT(T ret){ return ret;}
#define QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME, P, R) \
	do \
	{ \
		int hr = GetEngineInterface(INAME, (void**)&P); \
		if ( FAILED(hr) ) \
		{ \
			ASSERT_(FALSE); \
			TRACE_(_T("Query interface [%s] failed, result: 0x%08X"), INAME, hr); \
			if ((R) == RET_NULL) \
				ReturnT( NULL ); \
			else if ((R) == RET_FALSE)	\
				ReturnT( FALSE ); \
			else \
				ReturnT( hr ); \
		} \
	}while(0); \
	AUTO_RELEASE_(P)

#define GET_IEGN_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMEngine*	pIEgn  = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMENGINE, pIEgn, R); 

#define GET_ICA_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_ClientAgent*	pICa  = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_CLIENT_AGENT, pICa, R); 

#define GET_ILOG_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMLogger*	pILog = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMLOGGER, pILog, R); 

#define GET_IEMGR_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMEventMgr*  pIEMgr = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMEVENTMGR, pIEMgr, R);

#define GET_IPY_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMPinyin*  pIPy = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMUI_PINYIN, pIPy, R);

#define GET_IEMOT_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_eIMXmlEmoticon* pIEmot = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMENGINE_EMOTICON, pIEmot, R);

#define GET_IPROT_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMProtocol* pIProt = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMENGINE_PROTOCOL, pIProt, R);

#define GET_ICONTS_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMContacts* pIConts = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMENGINE_CONTACTS, pIConts, R);

#define GET_IBIFILE_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_eIMBatchInternetFile* pIBIFile = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMBATCH_INTER_FILE, pIBIFile, R);

#define GET_ISIFILE_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_eIMSingleInternetFile* pISIFile = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMSINGLE_INTER_FILE, pISIFile, R);

#define GET_IP2P_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_eIMP2P* pIP2P = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMP2P, pIP2P, R);

#define GET_ISMGR_INTERFACE_AUTO_RELEASE_RET_(R) \
	I_EIMSessionMgr* pISMgr = NULL; \
	QUERY_ENGINE_INTERFACE_AUTO_RELEASE_RET_(INAME_EIMENGINE_SESSIONMGR, pISMgr, R);


#define ROLLBACK_FAILED_RET_(PDB, RET) \
	if (RET != SQLITE_OK) \
	{ \
		if ( RET != SQLITE_CONSTRAINT ) {ASSERT_(FALSE);} \
		(RET) = EIMERR_DB_ERROR_(RET); \
		EIMLOGGER_ERROR_(pILog, _T("Rallback, result: 0x%08X"), RET); \
		(PDB)->Rollback(); \
		return RET; \
	}

#define CHECK_INIT_RET_(RET) \
	if ( !Init() ) \
		return RET;

#define GET_STMT_AUTO_RELEASE_RET_(PDB, SQL, PSTMT, R) \
	(PSTMT) = (PDB)->Compile(SQL, false, &(R)); \
	if ( (PSTMT) == NULL ) \
	{ \
		ASSERT_(FALSE); \
		eIMString strSQL;\
		int i32SqlStrLen = ::eIMMByte2TChar(SQL,strSQL);\
		EIMLOGGER_ERROR_( pILog, _T("Compile SQL failed! [SQL:%s][SQLStrLen:%u], error:%d"), \
			( i32SqlStrLen==0 || (SQL)==NULL ) ? _T("") : strSQL.c_str(),\
			i32SqlStrLen,\
			(R)); \
		(R) = EIMERR_COMPLIE_FAILED; \
		return (R); \
	} \
	AUTO_RELEASE_(PSTMT);

#define CONVERT_TO_ICA_RET_(P) \
	I_ClientAgent* pICa = static_cast<I_ClientAgent*>(P); \
	CHECK_NULL_RET_(pICa, EIMERR_INVALID_PARAM)

#define PARSE_FINISHED_BREAK_(R)	if( (R) == EIMERR_PARSE_FINISHED ) break

#define SET_UTF8_2TVALUE_(SRC, DST)	do{::eIMUTF8ToTChar((const char*)SRC, szValue); _tcsncpy(DST, szValue.c_str(), COUNT_OF_ARRAY_(DST) - 1); }while(0)
#define SET_UTF8_VALUE_(SRC, DST)	do{ if(SRC)	strncpy( (char*)(DST), (char*)(SRC), COUNT_OF_ARRAY_(DST) -1); else { (DST)[0]='\0'; ASSERT_(FALSE);} }while(0)

#define SET_GROUP_ID_ATTR_(SRC, E)	do{ char szBuf[GROUPID_MAXLEN + 1] = { 0 }; memcpy(szBuf, SRC, GROUPID_MAXLEN); E.SetAttribute(FIELD_GROUP_ID, szBuf); }while(0)
#define SET_GROUP_ID_(SRC, DST)		do{ if(SRC) memcpy(DST, SRC, COUNT_OF_ARRAY_(DST)); else ASSERT_(FALSE); }while(0)
#define GET_QSID_ASTR_(qsid, buf)	(_snprintf((char*)buf, COUNT_OF_ARRAY_(buf), "%llu", (QSID)qsid), (char*)buf)
#define GET_U32_ASTR_(U, buf)		(_snprintf((char*)buf, COUNT_OF_ARRAY_(buf), "%u", U), (char*)buf)

int GetBitIndex(int i32Data, int i32Mask);

#define DB_KEY		("SXIT@0518$YFGZ#")
#define DB_KEY_LEN	(15)	
typedef enum tagDbFileType
{
	eDBTYPE_USERS = 0,	// Users.db
	eDBTYPE_CONTS,		// Contacts.db
	eDBTYPE_MSG,		// Msg.db
	
	// Backup
	eDBTYPE_BK_USER,	// ~1\Users.db
	eDBTYPE_BK_CONTS,	// ~1\Contacts.db
	eDBTYPE_BK_MSG,		// ~1\Msg.db
}E_DBFILE_TYPE, *PE_DBFILE_TYPE;
const char* GetDbFile(E_DBFILE_TYPE eDbFileType, eIMStringA& szDbFileA);

// Stop with '\0' or < GROUPID_MAXLEN
QSID GroupId2Qsid(const char* pau8GroupId);

TiXmlElement* ParseXmlCmdHeader(TiXmlDocument& xmlDoc, const TCHAR* pszXml, int i32CmdId, int& i32Result);
int  AddFileTask(S_FileTrans& sFileTrans, S_FileInfo& sFileInfo, BOOL bAdd2Map);
BOOL IsFileKey(const char* pszKey);


#define LOGIN_SVR_COUNT		( 2 )
#define LOGIN_SVR_SIZE		(128)

typedef struct tagLogin
{
	QY_ACCESS_MODE cMode;							// Access mode
	BOOL			bReconn;						// Auto reconnect
	S_ConnParam		asSvr[LOGIN_SVR_COUNT];
	LOGIN			sLogin;
}S_Login, *PS_Login;

// 修改用户资料
typedef struct tagModiInfo
{
	UINT32	dwUserID;
	char	cModiType; // 0: 性别 1: 籍贯 2: 出生日期 3: 住址 4:办公电话号码 5: 手机号码 6: 密码 7:头像ID 8:个人签名 9:权限 10:宅电 11:紧急联系手机 14:修改邮箱  100:修改多项资料
	UINT8	cLen; // 修改内容长度
	INT8	aszModiInfo[UCHAR_MAX];	// 修改内容
} S_ModifyInfo, *PS_ModifyInfo;
