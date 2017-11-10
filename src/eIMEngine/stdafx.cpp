// stdafx.cpp : source file that includes just the standard includes
// eIMUI.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
//=============================================================================
//Function:     GetBitIndex
//Description:	Get a bit index by mask
//
//Parameter:
//	i32Data    - To find the bit index
//	i32Mask    - Mask 
//
//Return:
//		-1		Not exist 1 bit, else is the bit index
//=============================================================================

#define INAME_EIMUI_CONFIGER			_T("SXIT.EIMUI.Configer")			// App Configer

int GetBitIndex(int i32Data, int i32Mask)
{
	int i32Index = 0;
	int i32Value = i32Data & i32Mask;

	while(!(i32Value & (1 << i32Index)))
		i32Index++;

	if (i32Index >=32)
		return -1;

	return i32Index;
}

const char* GetDbFile(E_DBFILE_TYPE eDbFileType, eIMStringA& szDbFileA)
{
	eIMString szDbFile;

	switch(eDbFileType)
	{
	case eDBTYPE_USERS:	// Users.db
		szDbFile = PATH_TOKEN_APPDATA_FILE_(USERS_DB_FILE);
		break;
	case eDBTYPE_CONTS:	// Contacts.db
		szDbFile = PATH_TOKEN_LOGINID_FILE_(CONTACTS_DB_FILE);
		break;
	case eDBTYPE_MSG:
		szDbFile = PATH_TOKEN_LOGINID_FILE_(MSG_DB_FILE);
		break;
	case eDBTYPE_BK_USER:
		szDbFile =  PATH_TOKEN_APPDATA_FILE_(PATH_TYPE_CONTACTS_TMP _T("\\") USERS_DB_FILE);
		break;
	case eDBTYPE_BK_CONTS:
		szDbFile =  PATH_TOKEN_LOGINID_FILE_(PATH_TYPE_CONTACTS_TMP _T("\\") CONTACTS_DB_FILE);
		break;
	case eDBTYPE_BK_MSG:
		szDbFile =  PATH_TOKEN_LOGINID_FILE_(PATH_TYPE_CONTACTS_TMP _T("\\") MSG_DB_FILE);
		break;
	default:
		ASSERT_(FALSE);
		break;
	}


	if (::eIMReplaceToken(szDbFile) && szDbFile.size() > 0)
	{
		::eIMTChar2UTF8(szDbFile.c_str(), szDbFileA);
		return szDbFileA.c_str();
	}

	return NULL;
}

// aGroupID 不是字符串，无法直接调用转换函数，否则要先COPY到临时BUF
QSID GroupId2Qsid(const char* pau8GroupId)
{
	CHECK_NULL_RET_(pau8GroupId, 0);
	QSID qsid = pau8GroupId[0] - '0';

	for( int i32Index = 1; i32Index < GROUPID_MAXLEN && pau8GroupId[i32Index]; i32Index++)
	{
		qsid *= 10;
		qsid += pau8GroupId[i32Index] - '0';
	}

	return qsid;
}

TiXmlElement* ParseXmlCmdHeader(TiXmlDocument& xmlDoc, const TCHAR* pszXml, int i32CmdId, int& i32Result)
{
	int			i32Value;
	eIMStringA	szValueA;

	::eIMTChar2UTF8(pszXml, szValueA);
	xmlDoc.Parse( szValueA.c_str() );
	if ( xmlDoc.RootElement() == NULL ||
		xmlDoc.RootElement()->Attribute(FIELD_ID, &i32Value ) == NULL || i32Value != i32CmdId)
	{
		TRACE_( _T("Parse [%s] failed!"), pszXml );
		i32Result = EIMERR_BAD_CMD;
		return NULL;
	}

	return xmlDoc.RootElement();
}

int AddFileTask(S_FileTrans& sFileTrans, S_FileInfo& sFileInfo, BOOL bAdd2Map)
{
	const TCHAR*	pszNamePos = NULL;
	const TCHAR*	pszValue   = NULL;
	const TCHAR*	pszToken  = NULL;
	VectFileInfo	vectFileInfo;
	S_EIMFileInfo	sFile  = { 0 };
	
	CHECK_NULL_RET_(sFileTrans.pIBIFile, EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(sFileTrans.pIEMgr, EIMERR_INVALID_PARAM);
	CHECK_NULL_RET_(sFileTrans.pILog, EIMERR_INVALID_PARAM);

	pszNamePos = _tcsrchr(sFileInfo.sFile.szFile, _T('\\'));
	if (pszNamePos == NULL)
		pszNamePos = _tcsrchr(sFileInfo.sFile.szFile, _T('/'));

	CHECK_NULL_RET_(pszNamePos, EIMERR_INVALID_PARAM);
	_tcsncpy(sFile.tszFileName, pszNamePos + 1, COUNT_OF_ARRAY_(sFile.tszFileName) -1);
	_tcsncpy(sFile.tszLocalPath, sFileInfo.sFile.szFile, COUNT_OF_ARRAY_(sFile.tszLocalPath) -1);
	sFile.tszLocalPath[pszNamePos - sFileInfo.sFile.szFile] = _T('\0');

	sFile.eDirec  = sFileInfo.sFile.bitDirect == FILE_DIR_UPLOAD ? eInterFile_Upload : eInterFile_Download;
	sFile.Qfid    = sFileInfo.sFile.u64Fid;
	sFile.eEncrpt = eInterFile_Encrpt;
	
	if ( sFileInfo.sFile.bitIsImage == FILE_SERVER_TYPE_IMAGE )
	{
		sFile.eFileType = eInterFile_Image;
		sFile.ePriority = eInterFile_AboveNormal;

		if(sFileTrans.pIEgn->GetPurview(FILE_TRANS_MODE_RESUME))
		{
			sFile.eTransWay = eInterFile_Resuming;
			_tcsncpy(sFile.tszFileKey, sFileInfo.sFile.szKey, COUNT_OF_ARRAY_(sFile.tszFileKey));
			if ( sFile.eDirec == eInterFile_Upload )
			{
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD_BT);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_tcsncpy(sFile.tszUpServerUrl, pszValue, COUNT_OF_ARRAY_(sFile.tszUpServerUrl) -1);

				pszToken = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD_TOKEN); ASSERT_(pszToken);
				CHECK_NULL_RET_(pszToken, FALSE);
				_tcsncpy(sFile.tszUpTokenUrl, pszToken, COUNT_OF_ARRAY_(sFile.tszUpTokenUrl) -1);
			}
			else
			{
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_IMAGE_DOWNLOAD_BT);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszDownServerUrl), _T("%s"), pszValue);
			}
		}
		else
		{
			sFile.eTransWay = eInterFile_NonResuming;
			if ( sFile.eDirec == eInterFile_Upload )
			{
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_IMAGE_UPLOAD);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_tcsncpy(sFile.tszUpServerUrl, pszValue, COUNT_OF_ARRAY_(sFile.tszUpServerUrl) -1);
			}
			else
			{
				//	const TCHAR* pszExtPos = _tcsrchr(sFileInfo.sFile.szFile, _T('.'));
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_IMAGE_DOWNLOAD);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszDownServerUrl), _T("%stype=%d&key=%s"), pszValue, sFileInfo.i32Type, sFileInfo.sFile.szKey);
			}
		}
	}
	else if(sFileInfo.sFile.bitIsImage == FILE_SERVER_TYPE_FILE)
	{
		sFile.eFileType = eInterFile_File;
		sFile.ePriority = eInterFile_Normal;

		if(sFileTrans.pIEgn->GetPurview(FILE_TRANS_MODE_RESUME))
		{
			sFile.eTransWay = eInterFile_Resuming;
			_tcsncpy(sFile.tszFileKey, sFileInfo.sFile.szKey, COUNT_OF_ARRAY_(sFile.tszFileKey));
			if ( sFile.eDirec == eInterFile_Upload )
			{
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_FILE_UPLOAD_BT);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_tcsncpy(sFile.tszUpServerUrl, pszValue, COUNT_OF_ARRAY_(sFile.tszUpServerUrl) -1);

				pszToken = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_FILE_UPLOAD_TOKEN); ASSERT_(pszToken);
				CHECK_NULL_RET_(pszToken, FALSE);
				_tcsncpy(sFile.tszUpTokenUrl, pszToken, COUNT_OF_ARRAY_(sFile.tszUpTokenUrl) -1);
			}
			else
			{
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD_BT);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszDownServerUrl), _T("%s"), pszValue);
			}
		}
		else
		{
			sFile.eTransWay = eInterFile_NonResuming;
			if ( sFile.eDirec == eInterFile_Upload )
			{
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_FILE_UPLOAD);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_tcsncpy(sFile.tszUpServerUrl, pszValue, COUNT_OF_ARRAY_(sFile.tszUpServerUrl) -1);
			}
			else
			{
				//	const TCHAR* pszExtPos = _tcsrchr(sFileInfo.sFile.szFile, _T('.'));
				pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_FILE_DOWNLOAD);	ASSERT_(pszValue);
				CHECK_NULL_RET_(pszValue, FALSE);
				_sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszDownServerUrl), _T("%skey=%s"), pszValue, sFileInfo.sFile.szKey);
			}
		}
	}
	else if(sFileInfo.sFile.bitIsImage == FILE_SERVER_TYPE_HEAD || sFileInfo.sFile.bitIsImage == FILE_SERVER_TYPE_HEADS)
	{
		sFile.eFileType = eInterFile_HeadImg;
		sFile.ePriority = eInterFile_AboveNormal;
		sFile.eEncrpt = eInterFile_NoEncrpt;
		sFile.eTransWay = eInterFile_NonResuming;
		if ( sFile.eDirec == eInterFile_Upload )
		{
			ASSERT_(FALSE); return EIMERR_FAIL;//先不做上传
		}
		else
		{
			pszValue = sFileTrans.pIEgn->GetAttributeStr(IME_ATTRIB_HEAD_DOWNLOAD);	ASSERT_(pszValue);
			CHECK_NULL_RET_(pszValue, FALSE);

			char achRCData[40] = {0};
			char achOut[10] = {0};
			eIMString strCR;
			sprintf_s(achRCData,40,"%u%u",sFileInfo.sFile.u32LoginEmpId,sFileInfo.sFile.u32Eid);
			hpi.CalCRC8((unsigned char*)achRCData,strlen(achRCData),achOut);
			eIMMByte2TChar(achOut,strCR);
			_sntprintf(sFile.tszDownServerUrl,  COUNT_OF_ARRAY_(sFile.tszDownServerUrl), 
				_T("%ssrcid=%u&type=%d&rc=%s&dstid=%u"), 
				pszValue,
				sFileInfo.sFile.u32LoginEmpId,
				sFileInfo.sFile.bitIsImage == FILE_SERVER_TYPE_HEADS ? 1 : 0,
				strCR.c_str(),
				sFileInfo.sFile.u32Eid );
		}
	}

	if ( bAdd2Map && sFileInfo.pICmd )
	{
		sFileInfo.pICmd->AddRef();
		C_EIMAutoLock al(sFileTrans.csMapFiles);
		sFileTrans.mapFiles.insert(PairFiles(sFileInfo.sFile.u64Fid , sFileInfo));
	}

	vectFileInfo.push_back(sFile);
	int i32Ret = sFileTrans.pIBIFile->AddTransTask(vectFileInfo);
	if (FAILED(i32Ret) && i32Ret != EIM_ERR_TASK_EXIST)
	{
		C_EIMAutoLock al(sFileTrans.csMapFiles);
		sFileTrans.mapFiles.erase(sFileInfo.sFile.u64Fid);
		EIMLOGGER_ERROR_(sFileTrans.pILog, _T("Add file transfer task failed, result:%d"), i32Ret);
		return i32Ret;
	}

	return EIMERR_NO_ERROR;
}

BOOL IsFileKey(const char* pszKey)
{
	CHECK_NULL_RET_(pszKey, FALSE);
	const char* pszExt = PathFindExtensionA(pszKey);
	CHECK_NULL_RET_(pszExt, FALSE);

	return (
		strnicmp(pszExt, ".jpg", 4) == 0 ||
		strnicmp(pszExt, ".jpeg", 5) == 0 ||
		strnicmp(pszExt, ".png", 4) == 0 ||
		strnicmp(pszExt, ".gif", 4) == 0 ||
		strnicmp(pszExt, ".bmp", 4) == 0 );
}

