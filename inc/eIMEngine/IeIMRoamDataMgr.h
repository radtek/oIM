// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: I_EIMRoamDataMgr is the engine function to 
//	 operate the roam data
// 
// Auth: 
// Date:     2014/9/19 10:11:05 
// 
// History: 
//    
// Example:


/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMENGINE_ROAMDATA_MANAGER_HEADER_2014_09_19_YFGZ__
#define __EIMENGINE_ROAMDATA_MANAGER_HEADER_2014_09_19_YFGZ__
#include "Public\Plugin.h"

#define ROAMINGDATA_CONTACT_MAXCOUNT	400		//������ϵ�˸���
#define ROAMINGDATA_MAX_NAME_LEN		100		//�Զ���������󳤶�

#define  ROAM_TYPE_COM_CONTACT		1	//	������ϵ��
#define  ROAM_TYPE_COM_DEPT			2	//	���ò���
#define  ROAM_TYPE_CONCERN_CONTACT	3	//	��ע��ϵ��
#define  ROAD_TYPE_DEFINE_GROUP		4	//  �Զ�����
#define  ROAD_TYPE_DEFINE_GROUP_MEM	5	//  �Զ������Ա�仯
#define  ROAM_TYPE_DEFAULT_COMCONTACT    6	//Ĭ�ϳ�����ϵ��
#define  ROAM_TYPE_ROBOT			7		// ������
#define  ROAM_TYPE_VIRGROUP			8		// ������
#define  ROAM_TYPE_COMMON_GROUP		9		// ����Ⱥ��only CSAIR now��

#define  ROAM_OPER_TYPE_ADD			1	// ����
#define  ROAM_OPER_TYPE_DELETE		2	// ɾ��

typedef struct tagRomaDataInfo
{
	UINT64 id;									// ��Ա���߲���ID
	int   iis_default;							// �Ƿ���Ĭ�ϣ�������ϵ�ˣ�
	TCHAR szName[ROAMINGDATA_MAX_NAME_LEN];		// �Զ�����ʱ������
	int	  iUdateFlag;							// 0 δͬ��  1ͬ���� 2��ͬ��
}ROAM_DATA_INFO, *PROAM_DATA_INFO;

typedef struct _robot_user_info
{
#define MAX_ROBOTGREETINGS_NUM	300		//�������ʺ�������ֽ���
	UINT32	dwUserID;	//������ID
	UINT32	dwAttribute;	//����������(�����ֶ�)
	UINT8	cUserType;	//����������
	UINT8	aszGreetings [MAX_ROBOTGREETINGS_NUM];	//�������ʺ���
}Robot_User_Info, *PRobot_User_Info;

typedef struct tagGroupMember
{
	int   i32GID;
	DWORD dwEid;
	int   flag;
}S_GroupMember, PS_GroupMember;

typedef int (__stdcall* PFN_RoamDataCB)(PROAM_DATA_INFO psInfo, void* pvUserData);
typedef int (__stdcall* PFN_DefineGroupMemberCb)(QEID qeid, void* pvUserData);

class __declspec(novtable) I_EIMRoamDataMgr: public I_EIMUnknown
{
public:
	///////////////////////////////////////////////////////////////////////////
	//=============================================================================
	//Function:     RoamDataSyncReq 
	//Description:  request syn roam data
	//
	//Parameter:
	//
	//Return:
	//		void
	//=============================================================================
	virtual void RoamDataSyncReq() = 0;

	//=============================================================================
	//Function:     Enum
	//Description:	Enumerate roam data by eFlag
	//
	//Parameter:
	//	iType         - ROAM_TYPE_
	//	pfnCb         - Callback function to receive the result, return 0 to break;
	//	pvUserData    - User defined data, pass to pfnCb
	//
	//Return:
	//		The count of enumerated sessions 
	//=============================================================================
	virtual DWORD Enum(int iType,PFN_RoamDataCB pfnCb,void* pvUserData) = 0;

	//=============================================================================
	//Function:     EnumMember
	//Description:	Enumerate define group's members
	//
	//Parameter:
	//	qgid          - defien group ID
	//	pfnCb         - Callback function to receive the result, return 0 to break;
	//	pvUserData    - User defined data, pass to pfnCb
	//
	//Return:
	//		The count of enumerated members 
	//=============================================================================
	virtual DWORD EnumDefineGroupMember(QGID qgid,PFN_DefineGroupMemberCb pfnCb,
		void*	pvUserData
		) = 0;

	//=============================================================================
	//Function:     GetRobotInfo
	//Description:	Get robot information
	//
	//Parameter:
	//	userid        - employee id
	//	rInfo         - Return employee information
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL GetRobotInfo(QEID userid, Robot_User_Info& rInfo) = 0;

	//=============================================================================
	//Function:     AddTopContact
	//Description:	Add common contact
	//
	//Parameter:
	//	eid         - Employee id
	//	is_default  - Default flag
	//  iFlag		- Flag
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int AddTopContact(QEID eid, int is_default, int iFlag) = 0;

	//=============================================================================
	//Function:     AddTopDept
	//Description:	Add common department
	//
	//Parameter:
	//	did         - Department id
	//  iFlag		- Flag
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int AddTopDept(QDID did, int iFlag) = 0;

	//=============================================================================
	//Function:     AddDefineGroup
	//Description:	Add user define group
	//
	//Parameter:
	//	id          - group id
	//  pszAlias	- Alias
	//  iFlag		- Flag
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int AddDefineGroup(int id, const char* pszAlias, int iFlag) = 0;
	//virtual int AddGroupMember(int iGroupID, QEID qeid, int iFlag) = 0;

	//=============================================================================
	//Function:     AddGroupMember
	//Description:	Add user define group member
	//
	//Parameter:
	//	pvData      - group members
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int AddGroupMember(void* pvData) = 0;

	//=============================================================================
	//Function:     AddDefineCommonGroup
	//Description:	Add common group
	//
	//Parameter:
	//	id			- Session id
	//  pszAlias	- Alias
	//  iFlag		- Flag
	//
	//Return:
	//	HRESULT
	//=============================================================================
	virtual int AddDefineCommonGroup(QSID id, const TCHAR* pszAlias=NULL, int iFlag=0) = 0;

	//=============================================================================
	//Function:     DelDefineCommonGroup
	//Description:	Add common group
	//
	//Parameter:
	//	id			- Session id
	//  bNoSelfs	- TRUE is not include myself, else include
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL DelDefineCommonGroup(QSID id, BOOL bNoSelfs=TRUE) = 0;

	
	//=============================================================================
	//Function:     IsTypeof
	//Description:	Check session type
	//
	//Parameter:
	//	sid		- Session id
	//  i32Type	- ROAM_TYPE_*
	//
	//Return:
	//	TRUE	- Succeeded
	//  FALSE	- Failed
	//=============================================================================
	virtual BOOL IsTypeof(QSID sid, int i32Type) = 0; 
};

#endif // __EIMENGINE_ROAMDATA_MANAGER_HEADER_2014_09_19_YFGZ__