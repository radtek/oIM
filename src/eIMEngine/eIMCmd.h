// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: EIMCmd is the engine function.
//   UI level use the XML string.
//   Each Command have two data:
//   1. Original data field, Engine internal used
//   2. XML string data value, UI level used
// 
//   The Database class is the friend class of the command
//
// Auth: yfgz
// Date:    2013/11/15 16:23:03 
// 
// History: 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_COMMAND_IMP_HEADER_2013_12_25_YFGZ__
#define __EIMENGINE_COMMAND_IMP_HEADER_2013_12_25_YFGZ__

#include "eIMEngine\eIMCmdBase.h"

#pragma pack( push, 1 )

class I_EIMProtocol;
class TiXmlElement;
namespace Cmd
{	
#define CONTS_CACHESIZE_MAX ( 10000 )						// Max cache size
#define CONTS_CAPACITY_MAX  ( CONTS_CACHESIZE_MAX + 100)	// Capacity: Max cache size + Max package size

	//C_CmdLogin///////////////////////////////////////////////////////////////////

	class C_CmdLogin: public T_eIMCmdBase< S_Login > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdLogin, eCMD_LOGIN, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa,	 BOOL* pbAbort = NULL);
	private:
		bool CheckUpdate(I_EIMLogger* pILog, I_ClientAgent* pICa, I_EIMEngine* pIEgn, I_EIMEventMgr* pIEMgr);
		inline const TCHAR* GetUpdateType(int i32Type);
		inline const TCHAR* GetMode(int i32Mode);
		inline const char*  GetLogFile(eIMStringA& szLogFileA);
		inline const char*  GetRsaKeyFile(eIMStringA& szRsaKeyFileA);
		inline BOOL LogInfo(I_EIMLogger* pILog);

	};

	class C_CmdLoginAck: public T_eIMCmdBase< LOGINACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdLoginAck, eCMD_LOGIN_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	private:
		eIMString GetMd5Param3();
	};

	class C_CmdLogout: public T_eIMCmdBase< LOGOUT > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdLogout, eCMD_LOGOUT, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdLogoutAck: public T_eIMCmdBase< LOGOUTACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdLogoutAck, eCMD_LOGOUT_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdNoticeState: public T_eIMCmdBase< USERSTATUSSETNOTICE > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdNoticeState, eCMD_NOTICE_STATE, CMD_FLAG_NO_XML_DB);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	//- eCMD_NOTICE_STATE_ACK Not used			// ( 26  ) Ա������״̬�仯֪ͨӦ��
	//- eCMD_ALIVE Not used						// ( 27  ) ����
	//- eCMD_ALIVE_ACK Not used					// ( 28  ) ����Ӧ��
	class C_CmdModifyInfo: public T_eIMCmdBase< MODIINFO > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifyInfo, eCMD_MODIFY_INFO, CMD_FLAG_NO_TOXML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdModifyInfoAck: public T_eIMCmdBase< MODIINFOACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifyInfoAck, eCMD_MODIFY_INFO_ACK, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdModifyInfoNotice: public T_eIMCmdBase< MODIINFONOTICE > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifyInfoNotice, eCMD_MODIFY_INFO_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	//- ECMD_MODIFY_INFO_NOTICE_ACK				// ( 32  ) �����޸�֪ͨӦ��
	class C_CmdGetCoInfo: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetCoInfo, eCMD_GET_CO_INFO, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetCoInfoAck: public T_eIMCmdBase< GETCOMPINFOACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetCoInfoAck, eCMD_GET_CO_INFO_ACK, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	//- eCMD_GET_CO_INFO_NOTICE					// ( 35  ) ��ҵ��Ϣ����֪ͨ
	//- eCMD_GET_CO_INFO_NOTICE_ACK				// ( 36  ) ��ҵ��Ϣ����֪ͨӦ��
	class C_CmdGetDeptList: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetDeptList, eCMD_GET_DEPT_LIST,  CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	//	CMD_DECLARE_CLASS_DB_DEFAULT_(C_CmdGetDeptListAck, GETDEPTLISTACK, eCMD_GET_DEPT_LIST_ACK);
	class C_CmdGetDeptListAck: public T_eIMCmdBase< GETDEPTLISTACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetDeptListAck, eCMD_GET_DEPT_LIST_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};
	//- eCMD_GET_DEPT_LIST_NOTICE				// ( 39  ) ��֯���ܸ���֪ͨ
	//- eCMD_GET_DEPT_LIST_NOTICE_ACK			// ( 40  ) ��֯���ܸ���֪ͨӦ��
	class C_CmdGetUserList: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetUserList, eCMD_GET_USER_LIST, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetUserListAck: public T_eIMCmdBase< GETUSERLISTACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetUserListAck, eCMD_GET_USER_LIST_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdGetDeptEmps: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetDeptEmps, eCMD_GET_DEPT_USER, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

//	CMD_DECLARE_CLASS_DB_DEFAULT_(C_CmdGetDeptEmpsAck, GETUSERDEPTACK, eCMD_GET_DEPT_USER_ACK);
	class C_CmdGetDeptEmpsAck: public T_eIMCmdBase< GETUSERDEPTACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetDeptEmpsAck, eCMD_GET_DEPT_USER_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdGetEmpInfo: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetEmpInfo, eCMD_GET_EMPLOYEE_INFO, CMD_FLAG_NO_TOXML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	class C_CmdGetEmpInfoAck: public T_eIMCmdBase< GETEMPLOYEEACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetEmpInfoAck, eCMD_GET_EMPLOYEE_INFO_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);

	private:
		S_EmpInfo m_sEmpInfo;
	};

	class C_CmdCreateGroup: public T_eIMCmdBase< CREATEGROUP >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateGroup, eCMD_CREATE_GROUP, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	class C_CmdCreateGroupAck: public T_eIMCmdBase< CREATEGROUPACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateGroupAck, eCMD_CREATE_GROUP_ACK, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml(); 
	};

	class C_CmdCreateGroupNotice: public T_eIMCmdBase< CREATEGROUPNOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateGroupNotice, eCMD_CREATE_GROUP_NOTICE, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	private:
		S_SessionInfo	m_sSInfo;
	};
	
	//- eCMD_CREATE_GROUP_NOTICE_ACK			// ( 50  ) ����Ⱥ��֪ͨӦ��
	class C_CmdModifyGroup: public T_eIMCmdBase< MODIGROUP > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifyGroup, eCMD_MODIFY_GROUP, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	class C_CmdModifyGroupAck: public T_eIMCmdBase< MODIGROUPACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyGroupAck, eCMD_MODIFY_GROUP_ACK, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	};

	class C_CmdModifyGroupNotice: public T_eIMCmdBase< MODIGROUPNOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyGroupNotice, eCMD_MODIFY_GROUP_NOTICE, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	};

	//- eCMD_MODIFY_GROUP_NOTICE_ACK			// ( 54  ) �޸�Ⱥ������֪ͨӦ��
	class C_CmdGetGroupInfo: public T_eIMCmdBase< GETGROUPINFO > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetGroupInfo, eCMD_GET_GROUP_INFO, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetGroupInfoAck: public T_eIMCmdBase< GETGROUPINFOACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetGroupInfoAck, eCMD_GET_GROUP_INFO_ACK, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();

        std::list<S_ReadMsgNotice> m_msgReadNoticeLst;

	};

	class C_CmdModifyGroupMember: public T_eIMCmdBase< MODIMEMBER >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyGroupMember, eCMD_MODIFY_GROUP_MEMBER, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );

	private:
		MODIMEMBER m_sDelMember;	// MODIMEMBER �޷�����ͬʱ����Ӻ�ɾ�������Σ�����ר�Ŷ���һ��������ɾ���ģ�Ĭ�ϵ�m_sData���洢��ӵġ�
	};

	class C_CmdModifyGroupMemberAck: public T_eIMCmdBase< MODIMEMBERACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyGroupMemberAck, eCMD_MODIFY_GROUP_MEMBER_ACK, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	};

	class C_CmdModifyGroupMemberNotice: public T_eIMCmdBase< MODIMEMBERNOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyGroupMemberNotice, eCMD_MODI_MEMBER_NOTICE, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	};

	//- eCMD_MODI_MEMBER_NOTICE_ACK,			// ( 59 ) Ⱥ���Ա�仯֪ͨ
	//C_CmdSendMsg/////////////////////////////////////////////////////////////////////////////
	typedef struct tagFontInfo
	{
		BYTE	u8Id;			// Font id(index) 
		BYTE	u8Size;			// Font size
		BYTE	u8Mode;			// Font mode, bit0:Bold, bit1: Italic, bit2: Underline
		BYTE    au8Color[3];	// Font color
		DWORD   dwRev;
	}S_FONTINFO, *PS_FONTINFO;

	/*typedef struct tagSmsInfo
	{
		UINT64 u64Phone;
		char   szSmsContent[1500];
	}S_SMSINFO, *PS_SMSINFO;*/
	class C_CmdSendMsg: public T_eIMCmdBase< SENDMSG >
	{
		CMD_DECLARE_DEFAULT_(C_CmdSendMsg, eCMD_SEND_MSG, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		char*		  Xml2Msg(  TiXmlElement* pE, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol, QEID qeid );
		/*char*		  Xml2MsgForSms(TiXmlElement* pEl, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol, QEID qeid, UINT64 u64Phone);*/
		virtual BOOL  FromXml( const TCHAR* pszXml );
		virtual BOOL  SetAttribute(UINT64 u64AttrId, TCHAR* pszValue);
        QSID GetSessionId();
	private:
		TiXmlDocument	m_xmlDoc;
		S_Msg			m_sMsg;		// For Db
		eIMString		m_szMsgUI;	// <MsgUi />
		eIMStringA		m_szRobotBeginA;
		VectSessionMember m_MsgEmps; //Msg session emps
	};
		
	class C_CmdSendMsgAck: public T_eIMCmdBase< _SendMSGAck > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdSendMsgAck, eCMD_SEND_MSG_ACK, CMD_FLAG_THREAD_DB);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml(); 
        SENDRECALLMSGACK m_sSendData;
	};

    class C_CmdSendCollectModifyMsg:public T_eIMCmdBase< FAVORITE_MODIFY_REQ > 
    {
        CMD_DECLARE_DEFAULT_(C_CmdSendCollectModifyMsg, CMD_FAVORITE_MODIFY_REQ, CMD_FLAG_THREAD_ALL);
        virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
        virtual BOOL  FromXml( const TCHAR* pszXml );
        char* Xml2Msg( TiXmlElement* pEl, char* pszMsg, WORD& u16Size );
        BOOL SetAttribute(UINT64 U64AttrId, TCHAR* pszValue);
    private:
        TiXmlDocument m_xmlDoc;
        eIMString		m_szMsgUI;
    };

    class C_CmdSendCollectModifyMsgAck:public T_eIMCmdBase< FAVORITE_MODIFY_ACK > 
    {
        CMD_DECLARE_DEFAULT_(C_CmdSendCollectModifyMsgAck, CMD_FAVORITE_MODIFY_ACK, CMD_FLAG_THREAD_ALL);
        virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
    };

    class C_CmdCollectNotice:public T_eIMCmdBase< FAVORITE_NOTICE > 
    {
        CMD_DECLARE_DEFAULT_(C_CmdCollectNotice, CMD_FAVORITE_NOTICE, CMD_FLAG_THREAD_ALL | CMD_FLAG_NO_TOXML);
        virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
        const TCHAR* ToXml(){return NULL;}
        const TCHAR* ToXml(I_SQLite3* pIDb); 
        const BOOL Msg2Element(TiXmlElement& MsgElement, C_eIMEngine& objEgn);
        TiXmlDocument m_xmlDoc;
        eIMString m_szMsgUI;
    };

    class C_CmdColleceSyncReq:public T_eIMCmdBase< UINT32 > 
    {
        CMD_DECLARE_DEFAULT_(C_CmdColleceSyncReq, CMD_FAVORITE_SYNC_REQ, CMD_FLAG_THREAD_ALL);
        virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
    };

    class C_CmdColleceSyncAck:public T_eIMCmdBase< FAVORITE_SYNC_ACK > 
    {
        CMD_DECLARE_DEFAULT_(C_CmdColleceSyncAck, CMD_FAVORITE_SYNC_ACK, CMD_FLAG_THREAD_ALL);
        virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
        virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
    };

	//���Ż�ִ���ͻ���
	typedef struct WD_SMS_to_client
	{
		UINT64 dwPhoneNum;     //�ֻ�����
		UINT8 cResultType;          //��ִ���� 0���ɹ�  1��ʧ��  ���ɹ���ʱ���ִ����Ϊ��
		//UINT8 aszDescribe[256];      //����ʧ�ܵĽ������   �ȴ������������ͻ��˲����Ӵ��ֶγ���
		UINT8 aszDescribe[64];
	}SMStoclient;

	typedef struct WD_SMS_batch_to_client
	{
		UINT32 dwBatch;                 //���κ�
		UINT32 dwTotal;				    //��������
		UINT32 dwFailNum;				//����Ӧ��ʧ����
		//SMStoclient  aszResult[24];     //����ʧ�ܵ�Ӧ������
	}SMSbatchtoclient;

	typedef struct _SMS_down_to_client
	{
		UINT32 dwSender;						 //������
		UINT64  dwSenderMobile;					 //�������ֻ�����
		UINT8 cFlag;							//����Ϣ�Ƿ���Ҫ��ִ 0:����Ҫ  1:��Ҫ
		UINT32 dwTime;							//���Żظ�ʱ��
		UINT8 aszMSgContent[1500];				//��Ϣ����
	}SMS_client;

	typedef struct tagSessionEmps
	{
		tagSessionEmps()
		{
			self_qeid = 0;
		}

		QEID	          self_qeid;
		VectSessionMember vecMember;
	}S_SessionEmps, *PS_SessionEmps;

	inline int __stdcall GetEmpByQSID(QEID qeid, void* pvUserData)
	{
		if(pvUserData && qeid!=0)
		{
			PS_SessionEmps pEmp = (PS_SessionEmps)pvUserData;
			if(pEmp->self_qeid != qeid)
			{
				pEmp->vecMember.push_back(qeid);
			}
		}

		return 1;
	}

	class C_CmdMsgNotice: public T_eIMCmdBase<MSGNOTICE>
	{
		CMD_DECLARE_DEFAULT_(C_CmdMsgNotice, eCMD_MSG_NOTICE, CMD_FLAG_THREAD_ALL | CMD_FLAG_NO_TOXML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		const TCHAR* ToXml(){ return NULL; }
		const TCHAR* ToXml(I_SQLite3* pIDb); 
		const TCHAR* ToXmlForSms(I_SQLite3* pIDb);
		const BOOL Msg2Element(TiXmlElement& MsgElement, C_eIMEngine& objEgn);
		virtual BOOL  FromXml(const TCHAR* pszXml);
		BOOL  IsTypeOfMsg(const char* pszMsg, const char* pszType, const char* pszValue = NULL);
        void DealMsgRecalled2Db(I_SQLite3* pIDb);
	private:
		TiXmlDocument	m_xmlDoc;
		S_Msg			m_sMsg;	// For Db
		eIMString		m_szMsgUI;
		BOOL			m_bGetGroupInfo;
		BOOL			m_bConstraintDb;	// Write database constraint
		S_SmsReply		m_sReply;
	};


//Del 	//============ Begin === 2014/8/2 15:35:22 deleted by LongWQ ======
//Del 	//Reason: 
//Del 	class C_CmdMsgNoticeAck: public T_eIMCmdBase< QMID > 
//Del 	{ 
//Del 		CMD_DECLARE_DEFAULT_(C_CmdMsgNoticeAck, eCMD_MSG_NOTICE_ACK, CMD_FLAG_NON);
//Del 		virtual int DoCmd(BOOL* pbAbort = NULL); 
//Del 	};
//Del 	//============ End ===== 2014/8/2 15:35:22 deleted by LongWQ ======

	class C_CmdGetUserState: public T_eIMCmdBase< DWORD > 
	{ 
		//CMD_DECLARE_DEFAULT_(C_CmdGetUserState, eCMD_GET_USER_STATE, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetUserStateInfo: public T_eIMCmdBase< GETUSERSTATELIST > 
	{ 
		//CMD_DECLARE_DEFAULT_(C_CmdGetUserStateInfo, eCMD_GET_USER_STATE, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetUserStateAck: public T_eIMCmdBase< GETUSERSTATELISTACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetUserStateAck, eCMD_GET_USER_STATE_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

 	//C_CmdModifyEmp//////////////////////////////////////////////////////////////////
	typedef enum tagModifyType
	{
		eMTYPE_SEX		= 0x00000001,			// [bit0]Sex
		eMTYPE_NATIVE	= 0x00000002,			//-[bit1]Native
		eMTYPE_BIRTHDAY	= 0x00000004,			// [bit2]Birthday
		eMTYPE_ADDRESS	= 0x00000008,			//-[bit3]Address
		eMTYPE_TEL		= 0x00000010,			// [bit4]Telephone
		eMTYPE_PHONE	= 0x00000020,			// [bit5]Phone
		eMTYPE_PSW		= 0x00000040, 			// [bit6]Password
		eMTYPE_LOGO		= 0x00000080,			// [bit7]Logo
		eMTYPE_SIGN		= 0x00000100,			// [bit8]Sign
		eMTYPE_PURVIEW	= 0x00000200,			// [bit9]Purview
		eMTYPE_HOMETEL	= 0x00000400,			// [bit10]Home telephone
		eMTYPE_EMRGTEL	= 0x00000800,			// [bit11]Emergency Phone
		eMTYPE_SYN_TYPE	= 0x00001000,			// [bit12]Message synchronization type, see: IME_EMP_SYNC_* 
		eMTYPE_DUTY		= 0x00002000,			// [bit13]Duty
		eMTYPE_EMAIL	= 0x00004000,			// [bit14]Email
	}E_ModifyType, *PE_ModifyType;
	class C_CmdModifyEmp: public T_eIMCmdBase< EMPLOYEE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifyEmp, eCMD_MODIFY_EMPLOYEE, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);

	private:
		E_ModifyType m_eModifyType;	// From XML
	};

	class C_CmdModifyEmpAck: public T_eIMCmdBase< MODIINFOACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifyEmpAck, eCMD_MODIFY_EMPLOYEE_ACK, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdSendBroadcast: public T_eIMCmdBase< SENDBROADCAST > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdSendBroadcast, eCMD_SEND_BROADCAST, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL  FromXml( const TCHAR* pszXml );
		char* Xml2Msg(TiXmlElement* pEl, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol);
	
	private:
		TiXmlDocument	m_xmlDoc;
		eIMString		m_szMsgUI;
		eIMString       m_szRecver;
	};

	//- eCMD_SEND_BROADCAST_ACK				// ( 70  ) ���͹㲥Ӧ��
	class C_CmdSendBroadcastAck: public T_eIMCmdBase< SENDBROADCASTACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdSendBroadcastAck, eCMD_SEND_BROADCAST_ACK, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	//- eCMD_SEND_BROADCAST_ACK				// ( 71  ) �㲥֪ͨ
	class C_CmdBroadcastNotice: public T_eIMCmdBase< BROADCASTNOTICE > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdBroadcastNotice, eCMD_BROADCAST_NOTICE, CMD_FLAG_THREAD_ALL | CMD_FLAG_NO_TOXML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);

	private:
		const TCHAR* ToXml(I_SQLite3* pIDb); 
		const TCHAR* ToXml(){ return NULL; }
		const TCHAR* ToAlertXml();
		const TCHAR* To1To10TXml(I_EIMLogger* pILog);
		const BOOL Msg2Element(TiXmlElement& MsgElement, C_eIMEngine& objEgn);
		TiXmlDocument m_xmlDoc;
		eIMString     m_szMsgUI;
		FILE_META	  m_fFileMeta;
	};

	//- eCMD_BROADCAST_NOTICE_ACK,			// ( 72  ) �㲥֪ͨӦ��
	class C_CmdMsgRead: public T_eIMCmdBase< MSGREAD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdMsgRead, eCMD_MSG_READ, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL  FromXml( const TCHAR* pszXml );

	private:
		TiXmlDocument	m_xmlDoc;
	};

	//- eCMD_MSG_READ_ACK,					// ( 74  ) ��Ϣ�Ѷ�Ӧ��
	class C_CmdMsgReadNotice: public T_eIMCmdBase< MSGREADNOTICE > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdMsgReadNotice, eCMD_MSG_READ_NOTICE, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);

	private:
		const TCHAR* ToXml(I_SQLite3* pIDb); 
		const TCHAR* ToXml(){ return NULL; }
		TiXmlDocument m_xmlDoc;
		S_Msg m_Msg;
		S_Msg m_sCreateMsg;
		eIMString	m_szXmlUI;
	};

	//- eCMD_MSG_READ_NOTICE_ACK,			// ( 76  ) ��Ϣ�Ѷ�֪ͨӦ��
	//- eCMD_GET_USERS_SIMPLE_LIST,			// ( 77  ) ��ȡԱ����Ҫ��Ϣ�б�
    //- eCMD_GET_USERS_SIMPLE_LIST_ACK,		// ( 78  ) ��ȡԱ����Ҫ��Ϣ�б�Ӧ��
	class C_CmdMsgNoticeConfirm: public T_eIMCmdBase< MSGNOTICECONFIRM > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdMsgNoticeConfirm, eCMD_MSG_NOTICE_CONFIRM, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	};

	//- eCMD_REGULAR_GROUP_UPDATE_REQ,		// ( 80  ) ��Ϣ֪ͨ�ѽ���ȷ��Ӧ��
	class C_CmdVirGroupUpdateReq: public T_eIMCmdBase< REGULAR_GROUP_UPDATE_REQ > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdVirGroupUpdateReq, eCMD_REGULAR_GROUP_UPDATE_REQ, CMD_FLAG_NON);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdVirGroupUpdateRsp: public T_eIMCmdBase< REGULAR_GROUP_UPDATE_RSP >
	{
		CMD_DECLARE_DEFAULT_(C_CmdVirGroupUpdateRsp, eCMD_REGULAR_GROUP_UPDATE_RSP, CMD_FLAG_NO_TOXML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdCheckTimeReq: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdCheckTimeReq, eCMD_CHECK_TIME_REQ, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdCheckTimeRsp: public T_eIMCmdBase< CHECK_TIME_RESP > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdCheckTimeRsp, eCMD_CHECK_TIME_RSP, CMD_FLAG_NO_TOXML | CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdGetOfflineMsgReq: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetOfflineMsgReq, eCMD_GET_OFFLINE_REQ, CMD_FLAG_NO_FROMXML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetOfflineMsgRsp: public T_eIMCmdBase< GET_OFFLINE_RESP > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetOfflineMsgRsp, eCMD_GET_OFFLINE_RSP, CMD_FLAG_NO_TOXML | CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	//- eCMD_REFUSE_GROUP_MSG_REQ,			// ( 87  ) ֹͣȺ������/��Ϣ
    //- eCMD_REFUSE_GROUP_MSG_RSP,			// ( 88  ) ֹͣȺ������/��ϢӦ��
	class C_CmdQuitGroup: public T_eIMCmdBase< QSID > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdQuitGroup, eCMD_QUIT_GROUP, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	class C_CmdQuitGroupAck: public T_eIMCmdBase< QUITGROUPACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdQuitGroupAck, eCMD_QUIT_GROUP_ACK, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml(); 
	};

	class C_CmdQuitGroupNotice: public T_eIMCmdBase< QUITGROUPNOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdQuitGroupNotice, eCMD_QUIT_GROUPNOTICE, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml(); 
	};

    //- eCMD_QUIT_GROUPNOTICE_ACK,			// ( 92  ) ������Ⱥ֪ͨӦ��
	class C_CmdModifySelfInfo: public T_eIMCmdBase< RESETSELFINFO > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifySelfInfo, eCMD_MODIFY_SELF_INFO, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdModifySelfInfoAck: public T_eIMCmdBase< RESETSELFINFOACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifySelfInfoAck, eCMD_MODIFY_SELF_INFO_ACK, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdModifySelfInfoNotice: public T_eIMCmdBase< RESETSELFINFONOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifySelfInfoNotice, eCMD_MODIFY_SELF_INFO_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	//- eCMD_MODIFY_SELF_INFO_NOTICE_ACK,	// ( 96  ) �û���Ϣ���֪ͨӦ��
    //- eCMD_ECWX_SYNC_REQ,					// ( 97  ) ���ſͻ���ͬ�������˺�����
    //- eCMD_ECWX_SYNC_RSP,					// ( 98  ) ���ſͻ���ͬ�������˺���Ӧ
    //- eCMD_ECWX_SMSG_REQ,					// ( 99  ) ���ſͻ��������˺�������Ϣ����
    //- eCMD_ECWX_SMSG_RSP,					// ( 100 ) ���ſͻ��������˺�������Ϣ��Ӧ
    //- eCMD_ECWX_PACC_NOT,					// ( 101 ) ����ƽ̨������Ϣ�����ſͻ���֪ͨ
	class C_CmdCreateSchedule: public T_eIMCmdBase< CREATESCHEDULE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateSchedule, eCMD_CREATE_SCHEDULE, CMD_FLAG_THREAD_DB);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdCreateScheduleAck: public T_eIMCmdBase< CREATESCHEDULEACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdCreateScheduleAck, eCMD_CREATE_SCHEDULE_ACK, CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdCreateScheduleNotice: public T_eIMCmdBase< CREATESCHEDULENOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateScheduleNotice, eCMD_CREATE_SCHEDULE_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	//- eCMD_CREATE_SCHEDULE_NOTICE_ACK,	// ( 105 ) Alias="�����ճ�����֪ͨӦ��" 
	class C_CmdGetDataListType: public T_eIMCmdBase< GETDATALISTTYPEPARAMETET > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetDataListType, eCMD_GET_DATA_LIST_TYPE, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetDataListTypeAck: public T_eIMCmdBase< GETDATALISTTYPEACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetDataListTypeAck, eCMD_GET_DATA_LIST_TYPE_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdCoLastTimeNotice: public T_eIMCmdBase< COMPLASTTIMENOTICE > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdCoLastTimeNotice, eCMD_COMP_LAST_TIME_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_UI);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdDelSchedule: public T_eIMCmdBase< DELETESCHEDULE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdDelSchedule, eCMD_DELETE_SCHDULE, CMD_FLAG_THREAD_DB);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdDelScheduleAck: public T_eIMCmdBase< DELETESCHEDULEACK > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdDelScheduleAck, eCMD_DELETE_SCHDULE_ACK, CMD_FLAG_NON);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdDelScheduleNotice: public T_eIMCmdBase< DELETESCHEDULE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdDelScheduleNotice, eCMD_DELETE_SCHDULE_NOTICE, CMD_FLAG_THREAD_DB);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};
	//- eCMD_DELETE_SCHDULE_NOTICE_ACK,		// ( 112 ) ɾ���ճ�����֪ͨӦ��
    //- eCMD_GROUP_PUSH_FLAG,				// ( 113 ) iosȺ����Ϣ���� 
    //- eCMD_GROUP_PUSH_FLAG_ACK,			// ( 114 ) iosȺ����Ϣ�����޸�Ӧ�� 
    //- eCMD_IOS_BACKGROUND_REQ,			// ( 115 ) IOSת���̨���� 
    //- eCMD_IOS_BACKGROUND_ACK,			// ( 116 ) IOSת���̨Ӧ��
    //- eCMD_GET_USER_RANK_REQ,				// ( 117 ) ��ȡ����(Ա������)���� 
    //- eCMD_GET_USER_RANK_ACK,				// ( 118 ) ��ȡ����(Ա������)Ӧ�� 
    //- eCMD_GET_USER_PROFE_REQ,			// ( 119 ) ��ȡҵ��(Ա������)���� 
    //- eCMD_GET_USER_PROFE_ACK,			// ( 120 ) ��ȡҵ��(Ա������)Ӧ�� 
    //- eCMD_GET_USER_AREA_REQ,				// ( 121 ) ��ȡ����(Ա������)���� 
    //- eCMD_GET_USER_AREA_ACK,				// ( 122 ) ��ȡ����(Ա������)Ӧ�� 
	//C_CmdGetSpecialList//////////////////////////////////////////////////////////////////
	class C_CmdGetSpecialList: public T_eIMCmdBase< GETSPECIALLIST > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetSpecialList, eCMD_GET_SPECIAL_LIST, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetSpecialListAck: public T_eIMCmdBase< GETSPECIALLISTACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetSpecialListAck, eCMD_GET_SPECIAL_LIST_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdModifySpecialListNotice: public T_eIMCmdBase< MODISPECIALLISTNOTICE > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdModifySpecialListNotice, eCMD_MODI_SPECIAL_LIST_NOTICE, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdModifySpecialListNoticeAck: public T_eIMCmdBase< MODISPECIALLISTNOTICEACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdModifySpecialListNoticeAck, eCMD_MODI_SPECIAL_LIST_NOTICE_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};


	// =============================�̶�Ⱥ��===============================
	/*eCMD_CREATEREGULARGROUPNOTICE = 135,		//135 �̶��鴴��֪ͨ
	eCMD_CREATEREGULARGROUPNOTICEACK,			//136 �̶��鴴��֪ͨӦ��
	eCMD_DELETEREGULARGROUPNOTICE,				//137 �̶���ɾ��֪ͨ
	eCMD_DELETEREGULARGROUPNOTICEACK,			//138 �̶���ɾ��֪ͨӦ��
	eCMD_GULARGROUPMEMBERCHANGENOTICE,			//149 �̶����Ա�仯֪ͨ
	eCMD_GULARGROUPMEMBERCHANGENOTICEACK,		//140 �̶����Ա�仯֪ͨӦ��
	eCMD_GULARGROUPNAMECHANGENOTICE,			//141 �̶������Ʊ仯֪ͨ
	eCMD_GULARGROUPNAMECHANGENOTICEACK,			//142 �̶������Ʊ仯֪ͨӦ��
	eCMD_GULARGROUPPROCLAMATIONCHANGENOTICE,	//143 �̶��鹫��仯֪ͨ
	eCMD_GULARGROUPPROCLAMATIONCHANGENOTICEACK,	//144 �̶��鹫��仯֪ͨӦ��*/
	// eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE   ����Ⱥ��֪ͨ�޸ĳɴ�����
	class C_CmdCreateRegularGroupNoticeV2: public T_eIMCmdBase< CREATEREGULARGROUPPROTOCOL2NOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateRegularGroupNoticeV2, eCMD_GULARGROUP_PROTOCOL2_CREATENOTICE, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	private:
		BOOL m_bHasGetAll;
	};

	/*class C_CmdCreateRegularGroupNotice: public T_eIMCmdBase< CREATEREGULARGROUPNOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdCreateRegularGroupNotice, eCMD_CREATEREGULARGROUPNOTICE, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};*/

	class C_CmdDeleteRegularGroupNotice: public T_eIMCmdBase< DELETEREGULARGROUPNOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdDeleteRegularGroupNotice, eCMD_DELETEREGULARGROUPNOTICE, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdRegularGroupMemberChangeNotice: public T_eIMCmdBase< GULARGROUPMEMBERCHANGENOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRegularGroupMemberChangeNotice, eCMD_GULARGROUPMEMBERCHANGENOTICE, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		virtual const TCHAR* ToXml();
	};

	class C_CmdRegularGroupNameChangeNotice: public T_eIMCmdBase< GULARGROUPNAMECHANGENOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRegularGroupNameChangeNotice, eCMD_GULARGROUPNAMECHANGENOTICE, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdRegularGroupProclamationChangeNotice: public T_eIMCmdBase< GULARGROUPPROCLAMATIONCHANGENOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRegularGroupProclamationChangeNotice, eCMD_GULARGROUPPROCLAMATIONCHANGENOTICE, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};
	// =============================��������Ϣ=============================
	class C_CmdRobotSyncReq: public T_eIMCmdBase< ROBOTSYNCREQ >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRobotSyncReq, eCMD_ROBOTSYNCREQ, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdRobotSyncRsp: public T_eIMCmdBase< ROBOTSYNCRSP >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRobotSyncRsp, eCMD_ROBOTSYNCRSP, CMD_FLAG_NO_XML| CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	////==============================���������Ϣ֪ͨ===============================
	//class C_CmdMeetingBasicInfoNotic:public T_eIMCmdBase<confInfoNotice>
	//{
	//	CMD_DECLARE_DEFAULT_(C_CmdMeetingBasicInfoNotic, eCMD_MEETING_INFO_NOTICE, CMD_FLAG_NO_XML |CMD_FLAG_THREAD_ALL);
	//public:
	//	virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	//};

	////==============================���鱸ע֪ͨ================================
	//class C_CmdMeetingRemarkNotice:public T_eIMCmdBase<confRemarksNotice>
	//{
	//	CMD_DECLARE_DEFAULT_(C_CmdMeetingRemarkNotice, eCMD_MEETING_REMARKS_NOTICE, CMD_FLAG_NO_XML |CMD_FLAG_THREAD_ALL);
	//public:
	//	virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	//};
	////==============================�����Ա��Ϣ֪ͨ============================
	//class C_CmdMeetingMemberInfoNotic:public T_eIMCmdBase<confMbrInfoNotice>
	//{
	//	CMD_DECLARE_DEFAULT_(C_CmdMeetingMemberInfoNotic, eCMD_MEETING_MBRINFO_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_DB | CMD_FLAG_THREAD_UI);
	//public:
	//	virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	//};
	////==============================���鼶���޸�֪ͨ============================
	//class C_CmdMeetingLevelInfoNotic:public T_eIMCmdBase<confMbrInfoNotice>
	//{
	//	CMD_DECLARE_DEFAULT_(C_CmdMeetingLevelInfoNotic, eCMD_MEETING_MBRINFO_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_DB | CMD_FLAG_THREAD_UI);
	//public:
	//	virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	//};
	////===============================���鸽����Ϣ֪ͨ=========================
	//class C_CmdMeetingFileInfoNotic:public T_eIMCmdBase<confFileInfoNotice>
	//{
	//	CMD_DECLARE_DEFAULT_(C_CmdMeetingFileInfoNotic, eCMD_MEETING_FILEINFO_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
	//public:
	//	virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	//};
	////==============================�����ʺ���Ϣ֪ͨ==========================
	//class C_CmdMeetingUserCodeInfoNotic:public T_eIMCmdBase<confUserInfoNotice>
	//{
	//	CMD_DECLARE_DEFAULT_(C_CmdMeetingUserCodeInfoNotic, eCMD_MEETING_USERINFO_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
	//public:
	//	virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	//};
	//==============================����������Ϣ==============================
	//class C_CmdMeetingNotic:public T_eIMCmdBase<>
	// =============================��������Ϣ==============================
	class C_CmdGetVirtualGroupReq: public T_eIMCmdBase< VIRTUAL_GROUP_INFO_REQ >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetVirtualGroupReq, CMD_VIRTUAL_GROUP_REQ, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_DB);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};

	class C_CmdGetVirtualGroupAck: public T_eIMCmdBase< VIRTUAL_GROUP_INFO_ACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetVirtualGroupAck, CMD_VIRTUAL_GROUP_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_DB);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};	
	class C_CmdGetVirtualGroupNotice: public T_eIMCmdBase< VIRTUAL_GROUP_INFO_NOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetVirtualGroupNotice, CMD_VIRTUAL_GROUP_NOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_DB);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	};
	// =============================ͷ��������Ӧ��=========================
	class C_CmdGetHeadIconAddListReq: public T_eIMCmdBase< DWORD > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdGetHeadIconAddListReq, eCMD_GET_HEAD_ICON_ADD_LIST_REQ, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	};

	class C_CmdGetHeadIconAddListAck: public T_eIMCmdBase< TGetUserHeadIconListAck >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetHeadIconAddListAck, eCMD_GET_HEAD_ICON_ADD_LIST_RSP, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL );
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};
	//  ============================��������===============================

	class C_CmdRoamDataSyncREQ: public T_eIMCmdBase< ROAMDATASYNC > 
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdRoamDataSyncREQ, eCMD_ROAMINGDATASYN, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdRoamDataSyncAck: public T_eIMCmdBase< ROAMDATASYNCACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRoamDataSyncAck, eCMD_ROAMINGDATASYNACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdRoamDataModify: public T_eIMCmdBase< S_CustomizeInfo >// S_CustomizeInfo
	{ 
		CMD_DECLARE_DEFAULT_(C_CmdRoamDataModify, eCMD_CUSTOMIZE_ACTION, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	private:
		ROAMDATAMODI	m_rRoamDataModi;
	};

	class C_CmdRoamDataModifyAck: public T_eIMCmdBase< ROAMDATAMODIACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRoamDataModifyAck, eCMD_ROAMINGDATAMODIACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdRoamDataModifyNotice: public T_eIMCmdBase< ROAMDATAMODINOTICE >
	{
		CMD_DECLARE_DEFAULT_(C_CmdRoamDataModifyNotice, eCMD_ROAMINGDATAMODINOTICE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};
	//=================================���ݶ���======================================

	class C_CmdTGetStatusReq: public T_eIMCmdBase < TGetStatusReq >			// ��ȡ״̬
	{
		CMD_DECLARE_DEFAULT_(C_CmdTGetStatusReq, eCMD_GET_STATUS_REQ, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	};

	class C_CmdTGetStatusRsp:public T_eIMCmdBase< TGetStatusRsp >			// ��ȡ״̬Ӧ��
	{
		CMD_DECLARE_DEFAULT_(C_CmdTGetStatusRsp, eCMD_NOTICE_STATE, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	private:
		vector<QEID> m_vectConcernEmp;
	};

	class C_CmdNoticeStateAll:public T_eIMCmdBase< TALLUserStatus >		// ȫ��״̬Ӧ��
	{
		CMD_DECLARE_DEFAULT_(C_CmdNoticeStateAll, eCMD_NOTICESTATE_ALL, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdSubscribeReq: public T_eIMCmdBase < SUBSCRIBER_REQ >			// ״̬��������
	{
		CMD_DECLARE_DEFAULT_(C_CmdSubscribeReq, eCMD_SUBSCRIBERREQ, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
		virtual BOOL FromXml( const TCHAR* pszXml );
	private:
	};

	class C_CmdSubscribeRsp:public T_eIMCmdBase< SUBSCRIBER_ACK >			// ״̬����Ӧ��
	{
		CMD_DECLARE_DEFAULT_(C_CmdSubscribeRsp, CMD_PROTOCOL_V2, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};


	typedef struct tagSmsInfo
	{
		DWORD  dwNo;			//���ţ����ݲ��������ͬһ��Ϊһ����
		DWORD dwTotal;
		UINT64 u64Phone;
		char   szSmsContent[1500];
	}S_SMSINFO, *PS_SMSINFO;
	class C_CmdSendSms: public T_eIMCmdBase< SENDMSG >
	{
		CMD_DECLARE_DEFAULT_(C_CmdSendSms, eCMD_SEND_SMS, CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
		char*		  Xml2Msg(TiXmlElement* pEl, char* pszMsg, WORD& u16Size, I_EIMProtocol* pIProtocol, QEID qeid, UINT64 u64Phone, DWORD dwNo, DWORD dwTotal);
		virtual BOOL  FromXml( const TCHAR* pszXml );
	    UINT64 char2Int64(const char* pszvalue, int i32Def=0)
		{
			if(!pszvalue || 0==strlen(pszvalue)) return i32Def;
			eIMString szPhoneW;
			eIMUTF8ToTChar(pszvalue, szPhoneW);
			return Str2Int64(szPhoneW.c_str(), i32Def);
		}

	private:
		TiXmlDocument	m_xmlDoc;
		S_Msg			m_sMsg;		// For Db
		eIMString		m_szMsgUI;	// <MsgUi />
		S_SmsSendRlt    m_retResult;
	};
	

	class C_CmdReadMsgSyncReq: public T_eIMCmdBase < MSG_READ_SYNC >			// �Ѷ�����
	{
		CMD_DECLARE_DEFAULT_(C_CmdReadMsgSyncReq, eCMD_READ_MSG_SYNC_REQ, CMD_FLAG_NO_XML);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
	//	virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr, void* pvBIFile, BOOL* pbAbort = NULL);
	//	virtual BOOL FromXml( const TCHAR* pszXml );
	private:
	};

	class C_CmdReadMsgSyncNotice:public T_eIMCmdBase< MSG_READ_SYNC >			// �Ѷ�֪ͨ
	{
		CMD_DECLARE_DEFAULT_(C_CmdReadMsgSyncNotice, eCMD_READ_MSG_SYNC_NOTICE, CMD_FLAG_NO_XML|CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdContactsUpdateNotice:public T_eIMCmdBase< CONTACTSUPDATENOTICE >			// ˢ��ͨѶ¼֪ͨ
	{
		CMD_DECLARE_DEFAULT_(C_CmdContactsUpdateNotice, eCMD_CONTACTS_UPDATE_NOTICE, CMD_FLAG_NO_XML|CMD_FLAG_THREAD_ALL);
		virtual int	DoCmd(I_EIMLogger* pILog, void* pICa, BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdGetDeptShowConfigReq : public T_eIMCmdBase< GETDEPTSHOWCONFIGREQ >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetDeptShowConfigReq, eCMD_DEPTSHOWCONFIG_REQ, CMD_FLAG_NO_XML|CMD_FLAG_THREAD_UI);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};

	class C_CmdGetDeptShowConfigAck: public T_eIMCmdBase< GETDEPTSHOWCONFIGACK >
	{
		CMD_DECLARE_DEFAULT_(C_CmdGetDeptShowConfigAck, eCMD_DEPTSHOWCONFIG_ACK, CMD_FLAG_NO_XML | CMD_FLAG_THREAD_ALL);
	public:
		virtual int	DoCmd(I_EIMLogger* pILog, void* pvCa,	 BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_SQLite3* pIDb,	BOOL* pbAbort = NULL);
		virtual int	DoCmd(I_EIMLogger* pILog, I_EIMEventMgr* pIEMgr,  void* pvBIFile, BOOL* pbAbort = NULL);
	};
};	// namespace
#pragma pack( pop )


#endif // __EIMENGINE_COMMAND_IMP_HEADER_2013_12_25_YFGZ__