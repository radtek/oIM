// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: 
//	This is helper cmds, NOT communicate with ClientAgent.
// 
// 
// Auth: LongWQ
// Date: 2014/6/19 10:36:00
// 
// History: 
//    2014/6/19 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_HELPER_COMMAND_IMP_HEADER_2014_06_19_YFGZ__
#define __EIMENGINE_HELPER_COMMAND_IMP_HEADER_2014_06_19_YFGZ__
#pragma once

#include "eIMEngine\eIMCmdBase.h"
#include "json/cJSON.h"

namespace HelperCmd
{
	//C_CmdLoadContacts////////////////////////////////////////////////////////
	class C_CmdLoadContacts: public T_eIMCmdBase< DWORD >
	{
	public:
		C_CmdLoadContacts( const C_CmdLoadContacts& obj ): T_eIMCmdBase( obj ){};
		C_CmdLoadContacts( const DWORD* pdwUserId ): T_eIMCmdBase(pdwUserId, eCMD_LOAD_CONTACTS, CMD_FLAG_THREAD_DB | CMD_FLAG_THREAD_UI){};

	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);

	private:
			BOOL _CheckReloadContacts(int i32Result, I_EIMLogger* pILog);

	};

	class C_CmdModifyMyselfInfo: public T_eIMCmdBase< S_ModifyInfo >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyMyselfInfo, eCMD_MODIFY_MYSELF_INFO, CMD_FLAG_THREAD_ALL);

	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );

	private:
		int			 m_i32Count;
		S_ModifyInfo m_sModifyInfo[15];
	};

	class C_CmdFileAction: public T_eIMCmdBase< S_FileTransInfo >
	{
		CMD_DECLARE_DEFAULT_(C_CmdFileAction, eCMD_FILE_ACTION, CMD_FLAG_THREAD_DB|CMD_FLAG_NO_XML);
		virtual int DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb, BOOL* pbAbort = NULL);
	private:
	};

	class C_CmdCustomizeAction: public T_eIMCmdBase< S_CustomizeInfo >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCustomizeAction, eCMD_CUSTOMIZE_ACTION, CMD_FLAG_THREAD_ALL);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	class C_CmdUpdateTimestampInfo: public T_eIMCmdBase< DWORD >
	{
		CMD_DECLARE_DEFAULT_(C_CmdUpdateTimestampInfo, eCMD_UPDATE_TIMESTAMP, CMD_FLAG_THREAD_DB);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdSubscribeAction: public T_eIMCmdBase< S_SubscribeAction >
	{
		CMD_DECLARE_DEFAULT_(C_CmdSubscribeAction, eCMD_SUBSCRIBE_ACTION, CMD_FLAG_THREAD_ALL);
	private:
		VectSessionMember m_vectEmps;

	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};
	
	class C_CmdUpdateEmpHeadFlag: public T_eIMCmdBase< S_EmpHeadFlag >
	{
		CMD_DECLARE_DEFAULT_(C_CmdUpdateEmpHeadFlag, eCMD_UPDATE_EMP_HEAD_FLAG, CMD_FLAG_THREAD_DB | CMD_FLAG_NO_TOXML);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdSaveOverTimeSendMsg: public T_eIMCmdBase< QMID >
	{
		CMD_DECLARE_DEFAULT_(C_CmdSaveOverTimeSendMsg, eCMD_ADD_OVERTIME_SENDMSG, CMD_FLAG_THREAD_DB | CMD_FLAG_NO_XML);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdFlushoutDatabase: public T_eIMCmdBase< DWORD >
	{
		CMD_DECLARE_DEFAULT_(C_CmdFlushoutDatabase, eCMD_FLUSHOUT_DATABASE, CMD_FLAG_THREAD_DB | CMD_FLAG_NO_XML);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdBackupDatabase: public T_eIMCmdBase< DWORD >
	{
		CMD_DECLARE_DEFAULT_(C_CmdBackupDatabase, eCMD_BACKUP_DATABASE, CMD_FLAG_THREAD_DB | CMD_FLAG_NO_XML);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	private:
		typedef enum DbBkupType
		{
			eBKOP_TIMESTAMP = 0,	// Operate by timestamp field
			eBKOP_REPLACE_ALL,		// Replace all
			eBKOP_DEL_REPLACE_ALL,	// At first delete, then replace
		}E_DbBkupType, *PE_DbBkupType;
		typedef struct tagDbBkupTable
		{
			const char* pszTable;
			E_DbBkupType  eBkupType;
		}S_DbBkupTable, *PS_DbBkupTable;
		typedef const tagDbBkupTable* PS_DbBkupTable_;

		static const S_DbBkupTable s_asContsDbTable[];
		static const S_DbBkupTable s_asMsgDbTable[];

		int		_BackupDatabase(I_EIMLogger* pILog, I_SQLite3* pIDb, E_DBFILE_TYPE eType, PS_DbBkupTable_ psTable, int i32Count);
		int		_BackupTable(I_EIMLogger* pILog, I_SQLite3* pIDb, const char* const pszSrcTable, const char* const pszDstTable, E_DbBkupType eDbBkupType);
		INT64	_GetLastRecordTimestamp(I_SQLite3* pIDb, const char* const pszTable);
		BOOL	_IsValidDababase(I_EIMLogger* pILog, const char* pszDbFile, PS_DbBkupTable_ psTable, int i32Count);

	};

	class C_CmdSendMsgNoticeConfirm: public T_eIMCmdBase< MSGNOTICECONFIRM > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdSendMsgNoticeConfirm, eCMD_SEND_MSG_NOTICE_CONFIRM, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	typedef struct tagIFErrorInfo
	{
		TCHAR szErrorInfo[1024];
		TCHAR szCmd[2048];
	}S_IFErrorInfo, *PS_IFErrorInfo;
	class C_CmdSendImageFileFailed: public T_eIMCmdBase< S_IFErrorInfo >
	{
		CMD_DECLARE_DEFAULT_(C_CmdSendImageFileFailed, eCMD_SEND_IMAGE_FILE_ERROR, CMD_FLAG_THREAD_UI | CMD_FLAG_THREAD_DB);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	private:
		S_Msg	  m_sMsg;
		eIMString m_szMsgUI;
	};

	class C_CmdHttpDownloadMapImgae : public T_eIMCmdBase< tagHttpDownloadMapImage >
	{
		CMD_DECLARE_DEFAULT_(C_CmdHttpDownloadMapImgae, eCMD_HTTP_DOWNLOAD_MAPIMAGE, CMD_FLAG_THREAD_UI);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
	};
}	// namespace HelperCmd



#endif //__EIMENGINE_HELPER_COMMAND_IMP_HEADER_2014_06_19_YFGZ__