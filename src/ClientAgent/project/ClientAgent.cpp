
#include "IClientAgent.h"
#include <string.h>
#include <TCHAR.h>
#include <iphlpapi.h>  
#include <atlconv.h>

#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")  
#pragma comment(lib,"Iphlpapi.lib")  

// Parse contacts's files
#define BSIZE_(F)				((i32FieldEndPos - i32FieldStartPos) < (sizeof(F) - 1) ? (i32FieldEndPos - i32FieldStartPos) : (sizeof(F) - 1))  
#define IS_LINE_END_(c)			((c) == '\n'|| (c) == '\r'|| (c) == '\0')
#define IS_NEW_FIELD_(c)		((c) == '|' || IS_LINE_END_(c))
#define CHECK_EMPTY_CONTINUE_() \
	if( i32FieldEndPos == i32FieldStartPos ) \
	{ \
		i32FieldIndex++; \
		i32FieldStartPos = i32FieldEndPos + 1; \
		continue; \
	}

int CLIENT_ParseDeptInfoLine(const char* pszLine, S_ContsData& sCData)
{	// ����ID[0]|������ID[1]|������������[2]|����Ӣ������[3]|��������[4]|��������[5]|���ŵ绰[6]|������ʱ��[7]|������˾[8]
	DEPTINFO& sDeptInfo = sCData.sDeptInfo;
	memset(&sDeptInfo, 0, sizeof(sDeptInfo));
	if ( pszLine == NULL || *pszLine == '\0' )
		return 0;	// Invalid data or Empty line

	int i32FieldStartPos = 0;		
	int i32FieldEndPos   = 0;
	int i32FieldIndex    = 0;

	do
	{
		if(IS_NEW_FIELD_(pszLine[i32FieldEndPos]))
		{	
			CHECK_EMPTY_CONTINUE_();	// Check empty field
			switch(i32FieldIndex)
			{
			case 0:		// ����ID
				sDeptInfo.dwDeptID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 1:		// ������ID
				sDeptInfo.dwPID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 2:		// ������������
				memcpy(sDeptInfo.szCnDeptName, &pszLine[i32FieldStartPos], BSIZE_(sDeptInfo.szCnDeptName));
				break;
			case 3:		// ����Ӣ������
				memcpy(sDeptInfo.szEnDeptName, &pszLine[i32FieldStartPos], BSIZE_(sDeptInfo.szEnDeptName));
				break;
			case 4:		// ��������, 1: ���� 2: �޸� 3: ɾ��
				sDeptInfo.wUpdate_type = (INT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 5:		// ��������
				sDeptInfo.wSort = (UINT16)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 6:		// ���ŵ绰
				memcpy(sDeptInfo.aszDeptTel, &pszLine[i32FieldStartPos], BSIZE_(sDeptInfo.aszDeptTel));
				break;
			case 7:		// ������ʱ��
				sDeptInfo.dwUpdateTime = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 8:		// ������˾
				sDeptInfo.dwCompID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			default:
				break;
			}	// switch(i32FieldIndex)

			i32FieldIndex++;
			i32FieldStartPos = i32FieldEndPos + 1;
		}	// if( IS_NEW_FIELD_(pszLine[i32FieldEndPos]) )

		if(IS_LINE_END_(pszLine[i32FieldEndPos]))		// Line end
			break;
	}while( pszLine[++i32FieldEndPos] );

	return i32FieldIndex;
}

int CLIENT_ParseUserInfoLine(const char* pszLine, S_ContsData& sCData)
{	// �û�ID[0]|�û�����[1]|�û�������[2]|�û�Ӣ����[3]|�û�����[4]|�Ա�[5]|��������[6]|ְ��[7]|�칫�绰[8]|�ֻ�����[9]|����[10]|��ϵ��ַ[11]|�ʱ�[12]|��ְʱ��[13]|����ǩ��[14]|��������[15]|����ʱ��[16]
	USERINFO& sUserInfo = sCData.sUserInfo;
	memset(&sUserInfo, 0, sizeof(sUserInfo));
	if ( pszLine == NULL || *pszLine == '\0' )
		return 0;	// Invalid data or Empty line

	int i32FieldStartPos = 0;		
	int i32FieldEndPos   = 0;
	int i32FieldIndex    = 0;

	do
	{
		if(IS_NEW_FIELD_(pszLine[i32FieldEndPos]))
		{	
			CHECK_EMPTY_CONTINUE_();
			switch(i32FieldIndex)
			{
			case 0:		// �û�ID
				sUserInfo.dwUserID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 1:		// ����
				memcpy(sUserInfo.aszUserCode, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszUserCode));
				break;
			case 2:		// ��������
				memcpy(sUserInfo.aszCnUserName, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszCnUserName));
				break;
			case 3:		// Ӣ������
				memcpy(sUserInfo.aszEnUserName, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszEnUserName));
				break;
			case 4:		// ����
				memcpy(sUserInfo.aszEmail, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszEmail));
				break;
			case 5:		// �Ա�
				sUserInfo.cSex = (INT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 6:		// ��������
				sUserInfo.dwBirth = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 7:		// ְ��
				memcpy(sUserInfo.aszPost, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszPost));
				break;
			case 8:		// �칫�绰
				memcpy(sUserInfo.aszTel, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszTel));
				break;
			case 9:		// �ֻ�����
				memcpy(sUserInfo.aszPhone, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszPhone));
				break;
			case 10:	// ����
				memcpy(sUserInfo.aszFax, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszFax));
				break;
			case 11:	// ��ϵ��ַ
				memcpy(sUserInfo.aszAdrr, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszAdrr));
				break;
			case 12:	// �ʱ�
				memcpy(sUserInfo.aszPostcode, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszPostcode));
				break;
			case 13:	// ��ְʱ��
				sUserInfo.dwHiredate = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 14:	// ����ǩ��
				memcpy(sUserInfo.aszSign, &pszLine[i32FieldStartPos], BSIZE_(sUserInfo.aszSign));
				break;
			case 15:	// ��������, 1: ���� 2: �޸� 3: ɾ��
				sUserInfo.wUpdate_type = (UINT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 16:	// ����ʱ��
				sUserInfo.dwUpdateTime = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			default:
				break;
			}

			i32FieldIndex++;
			i32FieldStartPos = i32FieldEndPos + 1;
		}	// if( IS_NEW_FIELD_(pszLine[i32FieldEndPos]) )

		if(IS_LINE_END_(pszLine[i32FieldEndPos]))		// Line end
			break;
	}while( pszLine[++i32FieldEndPos] );

	return i32FieldIndex;
}

int CLIENT_ParseDeptUserLine(const char* pszLine, S_ContsData& sCData)
{	// ����ID[0]|�û�ID[1]|�û�����[2]|�û�������[3]|�û�Ӣ����[4]|�Ա�[5]|����[6]|��������[7]|������ʱ��[8]|���˼���[9]|����ҵ��[10]|���˵���[11]
	USERDEPT& sDeptUser = sCData.sDeptUser;
	memset(&sDeptUser, 0, sizeof(sDeptUser));
	if ( pszLine == NULL || *pszLine == '\0' )
		return 0;	// Invalid data or Empty line

	int i32FieldStartPos = 0;		
	int i32FieldEndPos   = 0;
	int i32FieldIndex    = 0;

	do
	{
		if(IS_NEW_FIELD_(pszLine[i32FieldEndPos]))
		{	
			CHECK_EMPTY_CONTINUE_();
			switch(i32FieldIndex)
			{
			case 0:		// ����ID
				sDeptUser.dwDeptID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 1:		// �û�ID
				sDeptUser.dwUserID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 2:		// �û�����
				memcpy(sDeptUser.aszUserCode, &pszLine[i32FieldStartPos], BSIZE_(sDeptUser.aszUserCode));
				break;
			case 3:		// �û�������
				memcpy(sDeptUser.aszCnUserName, &pszLine[i32FieldStartPos], BSIZE_(sDeptUser.aszCnUserName));
				break;
			case 4:		// �û�Ӣ����
				memcpy(sDeptUser.aszEnUserName, &pszLine[i32FieldStartPos], BSIZE_(sDeptUser.aszEnUserName));
				break;
			case 5:		// �Ա�
				sDeptUser.cSex = (INT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 6:		// ����
				sDeptUser.wSort = (UINT16)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 7:		// ��������, 1: ���� 2: �޸� 3: ɾ��
				sDeptUser.wUpdate_type = (UINT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 8:		// ������ʱ��
				sDeptUser.dwUpdateTime = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);;
				break;
			case 9:		// ���˼���
				sDeptUser.cRankID = (INT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 10:	// ����ҵ��
				sDeptUser.cProfessionalID = (INT8)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			case 11:	// ���˵���
				sDeptUser.dwAreaID = (UINT32)strtoul(&pszLine[i32FieldStartPos], NULL, 0);
				break;
			default:
				break;
			}

			i32FieldIndex++;
			i32FieldStartPos = i32FieldEndPos + 1;
		}	// if( IS_NEW_FIELD_(pszLine[i32FieldEndPos]) )

		if(IS_LINE_END_(pszLine[i32FieldEndPos]))		// Line end
			break;
	}while( pszLine[++i32FieldEndPos] );

	return i32FieldIndex;
}

//typedef bool (__stdcall *PFN_ParseContactsCallback)(PS_ContsData pCData, void* pUserData);
int CLIENT_ParseParseContactsFile(const char* pszFile, int i32Type, PFN_ParseContactsCallback pCallback, void* pUserData)
{
	if(pszFile == NULL || pszFile[0] == '\0' || pCallback == NULL || i32Type < 1 || i32Type > 6 )
		return EIMERR_INVALID_PARAMTER;	// Invalid parameter
	
	FILE* pfileRead  = fopen(pszFile, "rt");
	if(pfileRead)
	{	
		const BOOL	bDept	  = (i32Type == CONTACT_UPDATE_TYPE_DEPT);
		const BOOL	bDeptUser = (i32Type == CONTACT_UPDATE_TYPE_DEPTUSER);
		const int	i32DeptFieldCount  = 9;
		const int	i32DeptUFieldCount = 12;
		const int	i32UserFieldCount  = 17;
		typedef int (*PFN_ParseLine)(const char*, S_ContsData&);
		PFN_ParseLine pfnParseLine  = bDept ? CLIENT_ParseDeptInfoLine : (bDeptUser ? CLIENT_ParseDeptUserLine : &CLIENT_ParseUserInfoLine);
		const int   i32FieldCount   = bDept ? i32DeptFieldCount : (bDeptUser ? i32DeptUFieldCount : i32UserFieldCount);
		char		szLineBuf[2048] = { 0 };			// Line buffer
		S_ContsData	sCData			= {i32Type, 0 };	// User information structure

		TRACE_(_T("Begin parse file[%S], type:%u"), pszFile, i32Type);
		while( fgets(szLineBuf, sizeof(szLineBuf), pfileRead) )
		{
			if ( pfnParseLine(szLineBuf, sCData) < i32FieldCount )
				continue;

			if(!pCallback(&sCData, pUserData))
				break;				// User break;
		} // while( fgets(szLineBuf, sizeof(szLineBuf), pfileRead) )
		
		TRACE_(_T("Finish parse file[%S], type:%u"), pszFile, i32Type);
		pCallback(NULL, pUserData);	// Parse ENDED
		fclose(pfileRead);
		return EIMERR_SUCCESS;		// Succedeed
	}

	TRACE_(_T("Open file[%S] failed"), pszFile);
	return EIMERR_FAILED;			// Failed
}



/////////////////////////////////////////////////////////////////////////////////////////////////
class CClientAgent: public I_ClientAgent
{
private:
	PCONNCB		m_pConnCB;

public:
	CClientAgent()
		: m_pConnCB(NULL)
	{

	}
	
	~CClientAgent()
	{
		Release();
	}
		
	virtual HRESULT QueryInterface(const TCHAR* pctszIID, void** ppvIObject)
	{
		return E_NOTIMPL;
	}

	virtual ULONG	AddRef(void)
	{
		return 1;
	}

	virtual ULONG Release(void)
	{
	//	UnInit(1);
		return 0;
	}

	virtual int	GetErrorCode(RESULT eResult)
	{
		return CLIENT_GetErrorCode(eResult);
	}

	virtual int GetStatus(int i32Flag)
	{
		int i32Status = 0;
		
		if( m_pConnCB )
		{
			if( (i32Flag & CA_STATUS_INITED) )
				i32Status |= CA_STATUS_INITED;

			if( (i32Flag & CA_STATUS_CONNECT) && m_pConnCB->fConnect && m_pConnCB->nSocket != INVALID_SOCKET && m_pConnCB->nSocket )
				i32Status |= CA_STATUS_CONNECT;

			if( (i32Flag & CA_STATUS_LOGIN) && m_pConnCB->fLogin )
				i32Status |= CA_STATUS_LOGIN; 

			if( (i32Flag & CA_STATUS_KICK) && m_pConnCB->fKick )
				i32Status |= CA_STATUS_KICK;

			if( (i32Flag & CA_STATUS_FORBIDDEN) && m_pConnCB->fForbidden )
				i32Status |= CA_STATUS_FORBIDDEN;
		}

		return i32Status;
	}


	virtual void UnInit(unsigned char nManual)
	{
		CLIENT_UnInit(m_pConnCB, nManual);
		m_pConnCB = NULL;
	}

	virtual int Init(QY_ACCESS_MODE cMode, char* pszRsaKeyFile, const char* pszLogFile = NULL)
	{
		::CLIENT_SetRsaKeyPath(pszRsaKeyFile);
		m_pConnCB = CLIENT_Init(cMode, pszLogFile);
		if (m_pConnCB)
			return EIMERR_SUCCESS;

		return EIMERR_INIT_FAILED;
	}

	virtual int Connect(char *ip, int port, char* pAccount, char* pVersion)
	{
		TRACE_(_T("Before>> fConnect:%d, fLogin:%u, fKick:%u, fForbidden:%u, nSocket:%u"), m_pConnCB->fConnect, m_pConnCB->fLogin, m_pConnCB->fKick, m_pConnCB->fForbidden, m_pConnCB->nSocket);
		int i32Ret = CLIENT_Connect(m_pConnCB, ip, port, pAccount, 0, pVersion, TERMINAL_PC);
		TRACE_(_T("After<<  fConnect:%d, fLogin:%u, fKick:%u, fForbidden:%u, nSocket:%u, Result:0x%08x"), m_pConnCB->fConnect, m_pConnCB->fLogin, m_pConnCB->fKick, m_pConnCB->fForbidden, m_pConnCB->nSocket, i32Ret);
		return i32Ret;
	}

	virtual int	Disconnect()
	{
		return CLIENT_Disconnect(m_pConnCB);
	}

	virtual LOGINACCESSACK* GetLoginAccessAck()
	{
		if ( m_pConnCB )
			return &m_pConnCB->tAccessAck;

		return NULL;
	}
	virtual int Login(char* pszUserName, char* pszPassword, TERMINAL_TYPE cType, char* pszVersion, char* pszMac)
	{
		if ( pszUserName == NULL )
			return EIMERR_NO_USER;

		if ( pszPassword == NULL )
			return EIMERR_INVALID_PSW;

		char aszMac[20] = { 0 };
		if (pszMac == NULL || pszMac[0] == '\0')
		{
			memset(aszMac,0,sizeof(aszMac));
			GetMac(aszMac);
		}
		else
			memcpy(aszMac, pszMac, 20);

		return CLIENT_Login(m_pConnCB, pszUserName, pszPassword, cType, pszVersion, aszMac, NULL);
	}

	virtual int Logout(int nStatus, unsigned char cManual)
	{
		return CLIENT_Logout(m_pConnCB, nStatus, cManual);
	}

	virtual int GetVersionInfo(LOGINACCESSACK* psLoginAck)
	{
		if(psLoginAck == NULL)
			return EIMERR_INVALID_PARAMTER;

		if ( m_pConnCB == NULL)
			return EIMERR_NOT_CONN;

		if ( m_pConnCB->tAccessAck.ret == RESULT_SUCCESS)
		{
			memcpy(psLoginAck, &m_pConnCB->tAccessAck, sizeof(LOGINACCESSACK));
			return EIMERR_NO_ERROR;
		}

		return GetErrorCode((RESULT)m_pConnCB->tAccessAck.ret);
	}

	virtual int CheckTime(int nSerial)
	{
		return CLIENT_CheckTime(m_pConnCB, nSerial);
	}

	virtual int CreateGroup(char *pszGroupID, char *pszGroupName, int nGroupNameLen, char *pszUsers, int num, int nGroupTime)
	{
		return CLIENT_CreateGroup(m_pConnCB, pszGroupID, pszGroupName, nGroupNameLen, pszUsers, num, nGroupTime);
	}

	virtual int GetGroupInfo(char *pszGroupID)
	{
		return CLIENT_GetGroupInfo(m_pConnCB, pszGroupID);
	}

	virtual int ModiGroup(char *pszGroupID, char* pszNew, int nType, int nGroupTime)
	{
		return CLIENT_ModiGroup(m_pConnCB, pszGroupID, pszNew, nType, nGroupTime);
	}

	virtual int SendtoGroup(char *pszGroupID, int nType, char *pszMsg, int len, UINT64 nMsgID, int nSendTime, int nGroupType, unsigned char nMsgTotal, unsigned char nMsgSeq,UINT8 cAllReply)
	{
		return CLIENT_SendtoGroup(m_pConnCB, pszGroupID, nType, pszMsg, len, nMsgID, nSendTime, nGroupType, nMsgTotal, nMsgSeq,cAllReply, 0);
	}

	virtual int QuitGroup(char *pszGroupID)
	{
		return CLIENT_QuitGroup(m_pConnCB, pszGroupID);
	}

	virtual int RefuseGroupMsg(char *pszGroupID, unsigned char cRefuseType)
	{
		return CLIENT_RefuseGroupMsg(m_pConnCB, pszGroupID, cRefuseType);
	}

	virtual int GetVirGroupInfo(UINT32 uTimestamp)
	{
		return CLIENT_GetRegularGroupInfo(m_pConnCB, uTimestamp);
	}

	virtual int GetEmployee(int nUserID, int nType)
	{
		return CLIENT_GetEmployee(m_pConnCB, nUserID, nType);
	}

	virtual int ModiEmpyInfo(EMPLOYEE *pEmpyInfo)
	{
		return CLIENT_ModiEmpyInfo(m_pConnCB, pEmpyInfo);
	}

	virtual int GetCompInfo()
	{
		return CLIENT_GetCompInfo(m_pConnCB);
	}

	virtual int GetMessage(MESSAGE *pMessage)
	{
		return CLIENT_GetMessage(m_pConnCB, pMessage);
	}

	virtual int GetDeptInfo(int nLastUpdateTime, TERMINAL_TYPE cType)
	{
		return CLIENT_GetDeptInfo(m_pConnCB, nLastUpdateTime, cType);
	}

	virtual int GetUserList(int nLastUpdateTime, TERMINAL_TYPE cType)
	{
		return CLIENT_GetUserList(m_pConnCB, nLastUpdateTime, cType);
	}

	virtual int GetUserDept(int nLastUpdateTime, TERMINAL_TYPE cType)
	{
		return CLIENT_GetUserDept(m_pConnCB, nLastUpdateTime, cType);
	}

	virtual int GetUserStateList() 
	{
		return CLIENT_GetUserStateList(m_pConnCB);
	}

	virtual int GetOffline(UINT8 nTermType)
	{
		return CLIENT_GetOffline(m_pConnCB, nTermType);
	}

	virtual int ModiInfo(int nType, int nLen, char *szInfo)
	{
		return CLIENT_ModiInfo(m_pConnCB, nType, nLen, szInfo);
	}

	virtual int ModiSelfNotice(RESETSELFINFO *pNotice)
	{
		return CLIENT_ModiSelfNotice(m_pConnCB, pNotice);
	}

	virtual int ModiMember(char *pszGroupID, char *pszUsers, int num, int nType, int nGroupTime) 
	{
		return CLIENT_ModiMember(m_pConnCB, pszGroupID, pszUsers, num, nType, nGroupTime) ;
	}

	virtual int SendSMS(int nRecverID, int nType, char *pszMsg, int len, UINT64 nMsgID, int nSendTime, int nReadFlag, unsigned char nMsgTotal, unsigned char nMsgSeq,UINT8 cAllReply,UINT64 nSrcMsgID)
	{
		return CLIENT_SendSMS(m_pConnCB, nRecverID, nType, pszMsg, len, nMsgID, nSendTime, nReadFlag, nMsgTotal, nMsgSeq,cAllReply,nSrcMsgID);
	}

	virtual int SendSMSEx(SENDMSG *pSMS)
	{
		return CLIENT_SendSMSEx(m_pConnCB, pSMS);
	}

	virtual int SendMsgNoticeAck(UINT64 dwMsgID, UINT32 dwNetID)
	{
		return CLIENT_SendMsgNoticeAck(m_pConnCB, dwMsgID, dwNetID);
	}

	virtual int SendReadSMS(int nSenderID, UINT64 nMsgID, UINT8 uType,int nReadTime)
	{
		return CLIENT_SendReadSMS(m_pConnCB, nSenderID, nMsgID, uType,nReadTime);
	}

	virtual int SendMSGNoticeConfirm(int nRecvID, char *pszMsg, int len, UINT64 nMsgID)
	{
		return CLIENT_SendMSGNoticeConfirm(m_pConnCB, nRecvID, pszMsg, len, nMsgID);
	}

	virtual int SendBroadCast(char *pRecverIDs, int num, char *pszTitle, char *pszMsg, int len, UINT64 nMsgID, int nSendTime, int nMsgType, unsigned char nAllReply = 0, UINT64 nSrcMsgID = 0)
	{
		return CLIENT_SendBroadCast(m_pConnCB, pRecverIDs, num, pszTitle, pszMsg, len, nMsgID, nSendTime, nMsgType, nAllReply, nSrcMsgID);
	}

	virtual int GetDataListType(GETDATALISTTYPEPARAMETET* pGetDataTypePara)
	{
		return CLIENT_GETDATALISTTYPE(m_pConnCB, pGetDataTypePara);
	}

	virtual UINT64 PackMsgId(/*UINT32 uUserId, UINT8 nTermType, UINT32 uTime*/)
	{
		return CLIENT_PackMsgId(m_pConnCB->dwUserID, TERMINAL_PC, (unsigned int)time(NULL));
	}

	virtual UINT64 UnpackMsgId(UINT64 uMsgId)
	{
		return CLIENT_UnpackMsgId(uMsgId);
	}

	virtual int ParseContactsFile(const char* pszFile, int i32Type, PFN_ParseContactsCallback pCallback, void* pUserData)
	{
		return CLIENT_ParseParseContactsFile(pszFile, i32Type, pCallback, pUserData);
	}
	
	virtual int ParseDeptInfo(const char* pszDeptInfo, UINT32* pu32StartPos, DEPTINFO* psDeptInfo)
	{
		return CLIENT_ParseDeptInfo(pszDeptInfo, pu32StartPos, psDeptInfo);
	}

	virtual int ParseDeptUserInfo(const char* pszDeptUserInfo, UINT32* pu32StartPos, USERDEPT* psUserDept)
	{
		return CLIENT_ParseDeptUserInfo(pszDeptUserInfo, pu32StartPos, psUserDept);
	}

	virtual int ParseUserInfo(const char* pszUserInfo, UINT32* pu32StartPos, USERINFO* psUserInfo)
	{
		return CLIENT_ParseUserInfo(pszUserInfo, pu32StartPos, psUserInfo);
	}

	virtual int ParseUserStatusSetNotice(const char* pszUserStatusSetNotice, UINT32* pu32StartPos, USERSTATUSNOTICE* psUserStatusNotice)
	{
		return  CLIENT_ParseUserStatusSetNotice(pszUserStatusSetNotice, pu32StartPos, psUserStatusNotice);
	}

	virtual int ParseEmployee(const char* pszEmploee, UINT32* pu32StartPos, EMPLOYEE* psUserInfo)
	{
		return  CLIENT_ParseEmploee(pszEmploee, pu32StartPos, psUserInfo);
	}

	virtual int ParseUserHeadIconList(const char* pszUserList, UINT32* pu32StartPos, TUserHeadIconList* psUserList)
	{
		return  CLIENT_ParseUserHeadIconList(pszUserList, pu32StartPos, psUserList);
	}

	virtual int CreateSchedule(CREATESCHEDULE *pCreate)
	{
		return CLIENT_CreateSchedule(m_pConnCB, pCreate);
	}

	virtual int CreateScheduleNotieAck(char *pScheduleID)
	{
		return CLIENT_CreateScheduleNotieAck(m_pConnCB, pScheduleID);
	}

	virtual int	DeleteSchedule(DELETESCHEDULE *pDelete)
	{
		return CLIENT_DeleteSchedule(m_pConnCB,pDelete);
	}

	virtual int	GetSpecialList(GETSPECIALLIST *pGetSpecialList)
	{
		return CLIENT_GetSpecialList(m_pConnCB,pGetSpecialList);
	}
	
	virtual int ModiSpecialListNoticeAck(UINT64 nMsgID)
	{
		return CLIENT_ModiSpecialListNoticeAck(m_pConnCB,nMsgID);
	}

	virtual int	GetUserHeadIconList(int nLastUpdateTime)
	{
		return CLIENT_GetUserHeadIconList(m_pConnCB,nLastUpdateTime, TERMINAL_PC);
	}

	virtual int RoamingDataSync(ROAMDATASYNC *pReqData)
	{
		return CLIENT_RoamingDataSync(m_pConnCB,pReqData);
	}

	virtual int RoamingDataModi(ROAMDATAMODI *pModiData)
	{
		return CLIENT_RoamingDataModi(m_pConnCB,pModiData);
	}

	virtual int RobotDataSync(ROBOTSYNCREQ *pReqData)
	{
		return CLIENT_RobotInfoSync(m_pConnCB, pReqData);
	}

	virtual int	SendSubscribeReq(SUBSCRIBER_REQ *pData) 
	{
		pData->mPackageHead.cSrcType = TERMINAL_PC;
		return CLIENT_SendSubscribeReq(m_pConnCB,pData);
	}

	virtual int	GetUserStateReq(TGetStatusReq* pData)
	{
		return CLIENT_GetUserStatusReq(m_pConnCB,pData);
	}

	virtual int GetVirtualGroupInfoReq(UINT32 dwTimeStamp,UINT8 cTerminalType)
	{
		return CLIENT_VirtualGroupInfoReq(m_pConnCB,dwTimeStamp,cTerminalType);
	}

	virtual int user_status_Parse(user_status* pData, TUserStatusList* pStatusList)
	{
		return CLIENT_user_status_Parse(FALSE,pData, pStatusList);
	}
	virtual int	MsgReadSyncReq(MSG_READ_SYNC *pData)
	{
		return CLIENT_MsgReadSyncReq(m_pConnCB, pData);
	}

	virtual int SendContactsUpdateAck(UINT32 dwTimeStamp)
	{
		return CLIENT_SendContactsUpdateAck(m_pConnCB, dwTimeStamp, TERMINAL_PC);
	}

    virtual int      SendFavoriteMsg(FAVORITE_MODIFY_REQ *pFavorite)
    {
        pFavorite->cTerminal = TERMINAL_PC;
        return CLIENT_FavoriteModifyReq(m_pConnCB, pFavorite);
    }

    virtual int      SendFavoriteSync(UINT32 dwTimeStamp)
    {
        return CLIENT_FavoriteSync(m_pConnCB, dwTimeStamp, TERMINAL_PC);
    }

	virtual int GetDeptShowConfigReq(GETDEPTSHOWCONFIGREQ* psReq)
	{
		return CLIENT_GetDeptShowConfig(m_pConnCB, psReq->dwTimestamps, TERMINAL_PC);
	}

	virtual int	ParseDeptShowConfig(const char* pszBuf, UINT32* pu32StartPos, SINGLEDEPTSHOWLEVEL* psShowLevel)
	{
		return CLIENT_ParseDeptShowConfig(pszBuf, pu32StartPos, psShowLevel);
	}

    virtual int     GetMeetingAccountInfo()
    {
        return CLIENT_GetMeetingAccountInfo(m_pConnCB, TERMINAL_PC);
    }

protected:
	int GetMac(char * mac)     
	{  // ʹ��ARP ��ʽ����ȡMAC
		BYTE   au8Mac[8] = {0}; /* for 6-byte hardware addresses */  
		ULONG  ulAddrLen = 6;		 /* default to length of six bytes */  
		int    nLen = sizeof(sockaddr);
		struct sockaddr_in addr = { 0 };

		if ( getsockname(m_pConnCB->nSocket, (sockaddr*)&addr, &nLen) )
			return 0;

		if ( !SendARP(addr.sin_addr.s_addr, 0, au8Mac, &ulAddrLen) && ulAddrLen )
		{
			//	sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X", au8Mac[0], au8Mac[1], au8Mac[2], au8Mac[3], au8Mac[4], au8Mac[5]);
			memcpy(mac, au8Mac, ulAddrLen);
			return ulAddrLen;
		}
		else
		{	// ʹ��ARP�޷���ȡMAC����ʹ�ñ�ѡ����
			ulAddrLen = GetMac((BYTE*)&addr.sin_addr.s_addr, mac);			 
		}

		return ulAddrLen;   
	} 

	// Physical Address. . . . . . . . . : 00-0C-29-9D-7E-75
	// IPv4 Address. . . . . . . . . . . : 192.168.31.108(Preferred) 
	// �����ַ. . . . . . . . . . . . . : B8-CA-3A-97-D4-77
	// IPv4 ��ַ . . . . . . . . . . . . : 192.168.31.120(��ѡ) 
	int GetMac(BYTE aszIP[4], char aszMac[6])
	{	// ʹ�� ipconfig /all ����ȡMAC
		const TCHAR kMacEn[] = _T("Physical Address");
		const TCHAR kMacCn[] = _T("�����ַ");
		const TCHAR kIpEn[]  = _T("IPv4 Address");
		const TCHAR kIpCn[]  = _T("IPv4 ��ַ");
		int   nRet = 0;
		int   ai32IP[4] = { 0 };
		TCHAR szTempPath[MAX_PATH] = { 0 };
		TCHAR szTempFile[MAX_PATH] = { 0 };
		TCHAR szCmdLine[MAX_PATH * 2] = { 0 };

		USES_CONVERSION;
		nRet = GetTempPath(_countof(szTempPath), szTempPath);
		if ( nRet > _countof(szTempPath) || nRet == 0 )
			return 0;

		nRet = GetTempFileName(szTempPath, _T("mac"), 0, szTempFile);
		if ( nRet == 0 )
			return 0;

		_sntprintf(szCmdLine, _countof(szCmdLine), _T("cmd /C ipconfig /all > \"%s\""), szTempFile);
		TRACE_(_T("%s"), szCmdLine);
		STARTUPINFO si = {sizeof(STARTUPINFO), 0 };
		PROCESS_INFORMATION pi = { 0 };

		if ( !CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) )
			return 0;

		WaitForSingleObject( pi.hProcess, 5*1000 );	// �ȴ����
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		if ( FILE* pflRead = _tfopen(szTempFile, _T("rt")) )
		{	// ����ipconfig /all ���������ݣ��ҵ�ָ��IP��ַ��MAC
			char szLine[1024] = { 0 };
			while( fgets(szLine, _countof(szLine), pflRead) )
			{
				if ( szLine[0] == _T('\n') || szLine[1] == _T('\n') )
					continue;		// �Թ�����

				TCHAR* pszLine = A2T(szLine);	// ת������
				while ( *pszLine == _T(' ') || *pszLine == _T('\t') )
					pszLine++;		// �Թ��ո�

				if ( _tcsnicmp(pszLine, kMacCn, _countof(kMacCn) - 1) == 0 || _tcsnicmp(pszLine, kMacEn, _countof(kMacCn) - 1) == 0 )
				{
					while ( *pszLine != _T(':') )
						pszLine++;	// �Թ����з�":"

					_stscanf(++pszLine, _T("%02x-%02x-%02x-%02x-%02x-%02x"), &aszMac[0], &aszMac[1], &aszMac[2], &aszMac[3], &aszMac[4], &aszMac[5]);	// �õ� MAC
				}
				else if (_tcsnicmp(pszLine, kIpCn, _countof(kIpCn) - 1) == 0 || _tcsnicmp(pszLine, kIpEn, _countof(kIpEn) - 1) == 0 )
				{
					while ( *pszLine != _T(':') )
						pszLine++;	// �Թ����з�":"

					_stscanf(++pszLine, _T("%d.%d.%d.%d"), &ai32IP[0], &ai32IP[1], &ai32IP[2], &ai32IP[3]);	// IP
					if ( aszIP[0] == ai32IP[0] && aszIP[1] == ai32IP[1] && aszIP[2] == ai32IP[2] && aszIP[3] == ai32IP[3] )
					{	// �ҵ���ָ����IP��ַһ�����ˣ�MAC��Ϣ��IP��ַ��ǰ�棩���ͷ���
						nRet = 6;
						break;
					}
				}
			}

			fclose(pflRead);
			DeleteFile(szTempFile);
		}

		return nRet;
	}
};

extern "C" _declspec(dllexport) int __stdcall eIMCreateInterface(const TCHAR* pctszIID, void** ppvIObject)				
{
	if( pctszIID && _tcsnicmp(pctszIID, INAME_CLIENT_AGENT, _tcslen(INAME_CLIENT_AGENT)) == 0 )
	{
		static CClientAgent obj;

		if( ppvIObject )
		{
			*ppvIObject = &obj;
			return EIMERR_SUCCESS;
		}
	}

	return EIMERR_NOT_IMPL;
}

