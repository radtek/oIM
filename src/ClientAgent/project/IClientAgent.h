#ifndef __ICLIENT_AGENT_2013_09_06_BY_YFGZ__
#define __ICLIENT_AGENT_2013_09_06_BY_YFGZ__

#include "..\src\client.h"
#include "Public\Plugin.h"
//#include "Public\Errors.h"
#include "vld\vld.h"

#define CONTACT_UPDATE_TYPE_DEPT		( 1 )	// Update dept
#define CONTACT_UPDATE_TYPE_DEPTUSER	( 2 )	// Update deptuser
#define CONTACT_UPDATE_TYPE_USER		( 4 )	// Update user
typedef struct tagContsData
{
	int i32Type;	// Type: CONTACT_UPDATE_TYPE_*
	union
	{
		DEPTINFO	sDeptInfo;
		USERINFO	sUserInfo;
		USERDEPT	sDeptUser;
	};
}S_ContsData, *PS_ContsData;
typedef const tagContsData* PS_ContsData_;


// PS_ContsData_ is NULL when parse ended
typedef bool (__stdcall *PFN_ParseContactsCallback)(PS_ContsData_, void*);

#define CA_STATUS_INITED	0x00000001	// Inited
#define CA_STATUS_CONNECT	0x00000002	// Connect flag
#define CA_STATUS_LOGIN		0x00000004	// Login flag
#define CA_STATUS_KICK		0x00000008	// Kick flag
#define CA_STATUS_FORBIDDEN	0x00000010	// Forbidden flag
#define CA_STATUS_WORKING	( CA_STATUS_INITED | CA_STATUS_CONNECT | CA_STATUS_LOGIN )
#define CA_STATUS_ALL		(CA_STATUS_WORKING | CA_STATUS_KICK | CA_STATUS_FORBIDDEN)

#define INAME_CLIENT_AGENT		_T("SXIT.CLIENTAGENT")	// Client Agent interface name

class __declspec(novtable) I_ClientAgent: public I_EIMUnknown
{
public:
	virtual int		GetStatus(int i32Flag) = 0;
	virtual int		GetErrorCode(RESULT eResult) = 0;

	virtual void  	UnInit(unsigned char nManual) = 0;
	virtual int		Init(QY_ACCESS_MODE cMode, char* pszRsaKeyFile, const char* pszLogFile = NULL) = 0;
	virtual int 	Connect(char *ip, int port, char* pAccount, char* pVersion) = 0;
	virtual int		Disconnect() = 0;
	virtual LOGINACCESSACK* GetLoginAccessAck() = 0;

	virtual int 	Login(char* pszUserName, char* pszPassword, TERMINAL_TYPE cType, char* pszVersion, char* pszMac) = 0;
	virtual int 	Logout(int nStatus, unsigned char cManual) = 0;

	virtual int 	GetVersionInfo(LOGINACCESSACK* psLoginAck) = 0;

	virtual int 	CheckTime(int nSerial) = 0;
	
	virtual int 	CreateGroup(char *pszGroupID, char *pszGroupName, int nGroupNameLen, char *pszUsers, int num, int nGroupTime) = 0;
	virtual int 	GetGroupInfo(char *pszGroupID) = 0;
	virtual int 	ModiGroup(char *pszGroupID, char* pszNew, int nType, int nGroupTime) = 0;
	virtual int 	SendtoGroup(char *pszGroupID, int nType, char *pszMsg, int len, UINT64 nMsgID, int nSendTime, int nGroupType, unsigned char nMsgTotal, unsigned char nMsgSeq,UINT8 cAllReply = 0) = 0;
	virtual int 	QuitGroup(char *pszGroupID) = 0;
	virtual int 	RefuseGroupMsg(char *pszGroupID, unsigned char cRefuseType) = 0;

	virtual int 	GetVirGroupInfo(UINT32 uTimestamp) = 0;

	virtual int 	GetEmployee(int nUserID, int nType) = 0;
	virtual int 	ModiEmpyInfo(EMPLOYEE *pEmpyInfo) = 0;

	virtual int 	GetCompInfo() = 0;
	
	virtual int		GetMessage(MESSAGE *pMessage) = 0;
	virtual int 	GetDeptInfo(int nLastUpdateTime, TERMINAL_TYPE cType) = 0;
	virtual int 	GetUserList(int nLastUpdateTime, TERMINAL_TYPE cType) = 0;
	virtual int 	GetUserDept(int nLastUpdateTime, TERMINAL_TYPE cType) = 0;
	virtual int 	GetUserStateList() = 0;
	virtual int 	GetOffline(UINT8 nTermType) = 0;

	virtual int 	ModiInfo(int nType, int nLen, char *szInfo) = 0;
	virtual int 	ModiSelfNotice(RESETSELFINFO *pNotice) = 0;
	virtual int 	ModiMember(char *pszGroupID, char *pszUsers, int num, int nType, int nGroupTime) = 0;

	virtual int 	SendSMS(int nRecverID, int nType, char *pszMsg, int len, UINT64 nMsgID, int nSendTime, int nReadFlag, unsigned char nMsgTotal, unsigned char nMsgSeq,UINT8 cAllReply = 0, UINT64 nSrcMsgID = 0) = 0;
	virtual int 	SendSMSEx(SENDMSG *pSMS) = 0;
	virtual int 	SendMsgNoticeAck(UINT64 dwMsgID, UINT32 dwNetID) = 0;
	virtual int 	SendReadSMS(int nSenderID, UINT64 nMsgID,UINT8 uType,int nReadTime) = 0;
	virtual int 	SendMSGNoticeConfirm(int nRecvID, char *pszMsg, int len, UINT64 nMsgID) = 0;
	virtual int 	SendBroadCast(char *pRecverIDs, int num, char *pszTitle, char *pszMsg, int len, UINT64 nMsgID, int nSendTime, int nMsgType,unsigned char nAllReply = 0, UINT64 nSrcMsgID = 0) = 0;
	virtual int		GetDataListType(GETDATALISTTYPEPARAMETET* pGetDataTypePara) = 0;

	virtual UINT64 	PackMsgId(/*UINT32 uUserId, UINT8 nTermType, UINT32 uTime*/) = 0;
	virtual UINT64 	UnpackMsgId(UINT64 uMsgId) = 0;

	virtual int		ParseContactsFile(const char* pszFile, int i32Type, PFN_ParseContactsCallback pCallback, void* pUserData) = 0;
	virtual int		ParseDeptInfo(const char* pszDeptInfo, UINT32* pu32StartPos, DEPTINFO* psDeptInfo) = 0;
	virtual int		ParseDeptUserInfo(const char* pszDeptUserInfo, UINT32* pu32StartPos, USERDEPT* psUserDept) = 0;
	virtual int		ParseUserInfo(const char* pszUserInfo, UINT32* pu32StartPos, USERINFO* psUserInfo) = 0;
	virtual int		ParseUserStatusSetNotice(const char* pszUserStatusSetNotice, UINT32* pu32StartPos, USERSTATUSNOTICE* psUserStatusNotice) = 0;

	virtual int		ParseEmployee(const char* pszEmploee, UINT32* pu32StartPos, EMPLOYEE* psUserInfo) = 0;
	virtual int		ParseUserHeadIconList(const char* pszUserList, UINT32* pu32StartPos, TUserHeadIconList* psUserList) = 0;

	virtual int		CreateSchedule(CREATESCHEDULE *pCreate) = 0;
	virtual int		CreateScheduleNotieAck(char *pScheduleID) = 0;
	virtual int		DeleteSchedule(DELETESCHEDULE *pDelete) = 0;

	virtual int		GetSpecialList(GETSPECIALLIST *pGetSpecialList) = 0;
	virtual int		ModiSpecialListNoticeAck(UINT64 nMsgID) = 0;

	virtual int		GetUserHeadIconList(int nLastUpdateTime) = 0;

	virtual int		RoamingDataSync(ROAMDATASYNC *pReqData) = 0;
	virtual int		RoamingDataModi(ROAMDATAMODI *pModiData) = 0;
	virtual int     RobotDataSync(ROBOTSYNCREQ *pReqData) = 0;

	virtual int		SendSubscribeReq(SUBSCRIBER_REQ *pData) = 0;
	virtual int		GetUserStateReq(TGetStatusReq* pData) = 0;
	virtual int     user_status_Parse(user_status* pData, TUserStatusList* pStatusList) = 0;
	virtual int		MsgReadSyncReq(MSG_READ_SYNC *pData) = 0;

	virtual int     GetVirtualGroupInfoReq(UINT32 dwTimeStamp,UINT8 cTerminalType);
	virtual int		SendContactsUpdateAck(UINT32 dwTimeStamp) = 0;

    virtual int     SendFavoriteMsg(FAVORITE_MODIFY_REQ *pFavorite) = 0;
    virtual int     SendFavoriteSync(UINT32 dwTimeStamp) = 0;

	virtual int		GetDeptShowConfigReq(GETDEPTSHOWCONFIGREQ* psReq) = 0;
	virtual int		ParseDeptShowConfig(const char* psBuf, UINT32* pu32StartPos, SINGLEDEPTSHOWLEVEL* psShowLevel) = 0;
    virtual int     GetMeetingAccountInfo() = 0;
};



#endif //__ICLIENT_AGENT_2013_09_06_BY_YFGZ__