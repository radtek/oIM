
#ifndef __IMEETINGMGR_H__
#define __IMEETINGMGR_H__

#include "Public\Plugin.h"
#include "../../src/ClientAgent/Src/protocol.h"

//����JSON�����ֶ�
#define  MEETING_ACK_RESULT           ("result")
#define  MEETING_ACK_TYPE             ("type")
#define  MEETING_ACK_STATUS           ("status")
#define  MEETING_ACK_LAUNCHSTRING     ("launchString")
#define  MEETING_ACK_DATA           ("data")
#define  MEETING_ACK_FAILDTYPE        ("faildtype")

//���������Ϣ������ҳ��չʾ�ã�
typedef struct tagConfBasicInfo
{
	confBasicInfo basicInfo;  //�������·�������Ϣ
	char videoUrl[128];       //���¼����ƵԤ����ַ
	char videoDownUrl[128];   //���¼����Ƶ���ص�ַ
	char ShareDocUrl[128];    //����ĵ����ص�ַ
	char Remark[1540];        //���鱻ָ
}S_ConfBasicInfo;

//��ȡ�����𴮣����ʱʹ�ã�
typedef struct tagGetLaunchString
{
	char confid[CONFID_LENGTH];   //����ID
	char confUserName[60];        //����username
	char password[60];            //�������
	INT64 t;                      //ʱ���
	INT32 userid;                 //����ID
	const char* mdkey;            //mdkey
	tagGetLaunchString()
		:mdkey(NULL)
		,t(0)
		,userid(0)
	{
		memset(confid,0,CONFID_LENGTH);
		memset(confUserName,0,60);
		memset(password,0,60);
	}
}S_GetLaunchString,*PS_GetLaunchString;

//������
typedef struct tagJoinConference
{
	UINT	   nSessionID;   //����ID
	UINT	   nSessionType;  //��������
	UINT	   nLoadType;     //�����������
	eIMStringA szUserID;     //ȫʱID
	eIMStringA szCmdLine;    //�������
	eIMStringA szHeadImgUrl; //ͷ��URL
	void Reset()
	{
		nSessionID   = 0; 
		nSessionType = 0;
		nLoadType    = 0;  
		szUserID.clear();   
		szCmdLine.clear();
		szHeadImgUrl.clear();
	}
}S_JoinConference, *PS_JoinConference;

//����ͬ��
typedef struct tagMeetingSync
{
    UINT64        uMsgId;     //MSGid
    UINT32        userid;     //����ID
    UINT32        confUserId; //ȫʱID
    UINT32        uTime;      //ͬ��ʱ���
	tagMeetingSync()
		:uMsgId(0)
		,userid(0)
		,confUserId(0)
		,uTime(0)
	{
		
	}
}S_MeetingSync;

#define MAXLENGTH_INVITE    (400)
//����/�޸Ļ�������
typedef struct tagMeeting_create
{
	int operationType;            // �������� ��0���������� �� 1���޸Ļ��飩
	char confId[CONFID_LENGTH];   // ����ID �޸Ļ�������Ч
	int conferenceType;           //�������ͣ�0�����»��飬1���ƻ��飩
	UINT32 userid;                //�������û�������userid��
	UINT32 confUserId;            //�������û���ȫʱ��Ӧ��userid��
	TCHAR title[40];              //��������
	int length;                   //����ʱ��,����Ϊ��λ
	INT64 starttime;              //���鿪ʼʱ��
	INT64 inviteeList[INVITEE_MAX]; //��������ߣ�����userid
	int MemberNum  ;              //���ظ������� ,�����������
	TCHAR msgid[32];              //��Ϣ��ţ���ϢΨһ��ʾ
	int repetitionType;           //ѭ������(0:��ѭ����1: ѭ��)
	INT64 circulateStartTime;     //ѭ�����鿪ʼʱ�䣬ѭ������ʱ�������ʽΪlong����������
	INT64 circulateEndTime;       //ѭ���������ʱ�䣬ѭ������ʱ�������ʽΪlong����������
	int circulateType;            //ѭ�����ͣ�0:ÿ�죬1:ÿ�ܣ�2:ÿ�£�
	int circulateNode;            //ѭ���ڵ� ,��ѭ������Ϊÿ����ֵΪ0,��ѭ������Ϊÿ����ֵΪ1~7,��ѭ������Ϊÿ����ֵΪ1~31
	int conferenceMode;           //����ģʽ��0:Э��ģʽ��1:�᳡ģʽ��
	INT64 t;                      //ʱ���(��ʽΪlong��������)
	TCHAR mdkey[128];             //mdkey�����ڼ�Ȩmdkey = Encrip t.md5(userid + t Encript.getEncriptKey ())
	TCHAR address[64];            //����ص�
	TCHAR remark[512];            //��ע
	int  memberCount;             //������������
	int  messagenotice;           //�Ƿ��ж������� 0:������ 1:����
	int  conferenceLevel;         //���鼶�� 0����ͨ���飬1����Ҫ����

	tagMeeting_create()
	{
		memset(inviteeList,0,sizeof(INT64)*INVITEE_MAX);
	}
}S_Meeting_create,*PS_Meeting_create;

//��������󷵻���Ϣ��ҳ��չʾ�ã�
typedef struct tagMeetingCreate_Ack
{
	char conferenceId[CONFID_LENGTH]; //����ID
	char hostCode[CONFCODE_LENGTH];   //������Code
	char mbrCode[CONFCODE_LENGTH];    //�λ���Code
	char hostUrl[256];                //������Url
	char mbrUrl[256];                 //�λ���Url

	char protocolHostStartUrl[256];   //̩�����������Url
	
	tagMeetingCreate_Ack()
	{
		memset(conferenceId,0,32);
		memset(hostCode,0,32);
		memset(hostUrl,0,256);
		memset(mbrCode,0,32);
		memset(mbrUrl,0,256);
		memset(protocolHostStartUrl,0,256);
	}
}S_MeetingCreate_Ack;

//���������õ��Ļ�����Ϣ��ҳ��չʾ�ã�
typedef struct tagMeetingBasicInfo_ack
{
	char confid[CONFID_LENGTH];      //����ID
	char hostCode[CONFCODE_LENGTH];  //������Code
	char mbrCode[CONFCODE_LENGTH];   //�λ���Code
	char hostUrl[256];               //������Url
	char mbrUrl[256];                //�λ���Url
	S_Meeting_create meeting_create;
	tagMeetingBasicInfo_ack()
	{
		memset(confid,0,CONFID_LENGTH);
		memset(hostCode,0,32);
		memset(hostUrl,0,256);
		memset(mbrCode,0,32);
		memset(mbrUrl,0,256);
	}
}S_MeetingBasicInfo_ack;

//�޸Ļ�����Ա
typedef struct tagMeetingMemberChange
{
	char conferenceId[CONFID_LENGTH];  // ����ID
	int userid;             // ����ID
	int confUserId;         // ȫʱID
	INT64 msgid ;           // ��ϢID
	int memberaddNum;       // ������Ա���� (����)
	int memberdelNum;       // ɾ����Ա���� �����
	INT64 mbrAddList[CHANGEMEMBERS_MAX];  // ���ӵ���Ա�б�
	INT64 mbrDelList[CHANGEMEMBERS_MAX];  // ɾ������Ա�б�
	INT64 t;                // ʱ���
	char mdkey[64];         // md5
	
	tagMeetingMemberChange()
		:userid(0)
		,confUserId(0)
		,msgid(0)
		,memberaddNum(0)
		,memberdelNum(0)
		,t(0)
	{
		memset(mdkey,0,64);
		memset(mbrAddList,0,sizeof(INT64)*CHANGEMEMBERS_MAX);
		memset(mbrDelList,0,sizeof(INT64)*CHANGEMEMBERS_MAX);
	}
}S_MeetingMemberChange;

//ȡ������
typedef struct tagMeetingCancel
{ 
	char conferenceId[CONFID_LENGTH];   //ȫʱID
	TCHAR  circulateKey[32];            //ѭ�������ʶ
    int userid;                         //����ID
    int confUserId;                     //ȫʱID
	INT64  msgid;                       //��ϢID
    int  delType;                       //ɾ�����ͣ�0:ɾ��ѡ�л��飬1: ɾ��ѭ�����飩�����ѭ���������
    int  conferenceType;;               //�������ͣ�0�����»��飬1���ƻ��飩
    UINT8 strConfLang;                  //���� confUserInfo�ṹ������
	INT64 t;                            //ʱ���

	tagMeetingCancel()
		:userid(0)
		,confUserId(0)
		,msgid(0)
		,delType(0)
		,conferenceType(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
		memset(circulateKey,0,sizeof(TCHAR)*32);
	}
}S_MeetingCancel;

//�޸Ļ��鼶��
typedef struct tagMeetingLevel
{
	char conferenceId[CONFID_LENGTH]; //ȫʱID
	int userid;                 //����ID
	int conferenceLevel;        //���鼶��
	INT64  msgid;               //��ϢID
	INT64 t;                    //ʱ���

	tagMeetingLevel()
		:userid(0)
		,conferenceLevel(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,32);
	}
}S_MeetingLevel;

typedef struct tagMeetingModify
{
    INT64  msgid;                //��ϢID
    UINT32 userid;               //����ID
    UINT32 confUserId;           //ȫʱID
    UINT32    starttime;         //��ʼʱ��
    INT32     inviteeNum;        //��������
    UINT32    inviteeList[100];  //�����б�
    INT32  conferenceType;       //��������
    INT32  memberCount;          //��������
    INT32    length;             //ʱ��
    INT32    messagenotice;      //�Ƿ��ж�������
    INT32    conferenceLevel;    //���鼶��
    char   title[41];            //����
    char    address[1024];       //��ַ
    char    remark[1540];        //��ע
    char conferenceId[33];       //����ID

	tagMeetingModify()
		:msgid(0)
		,userid(0)
		,confUserId(0)
		,starttime(0)
		,inviteeNum(0)
		,conferenceType(0)
		,memberCount(0)
		,length(0)
		,messagenotice(0)
		,conferenceLevel(0)
	{
		memset(inviteeList,0,sizeof(UINT32)*100);
		memset(title,0,41);
		memset(address,0,1024);
		memset(remark,0,1540);
		memset(conferenceId,0,33);

	}
}S_MeetingModify;

//����������ض�Ӧ������
typedef enum tagMeetingAckType
{
	eMEETING_CREATE = 201,     //��������
	eMEETING_UPDATE,           //�޸Ļ�����Ϣ
	eMEETING_UPDATEMEMBERS,    //���/ɾ�������Ա
	eMEETING_UPDATEATTACHMENTS,//���/ɾ�����鸽��
	eMEETING_CANCEL,           //ȡ��ԤԼ����
	eMEETING_SYN = 207,        //ͬ�������б�
	eMEETING_GETLAUNCH = 210,  //��ȡ�������
	eMEETING_ACCEPT = 211,     //�����Ա���ܻ���

}ENUM_MEETINGACK;

//���鸽����ʼ������
typedef enum tagFiletype
{
	eFileUpload_Creator = 100,  //���鴴�����ϴ�
	eFileInit_Creator,          //���鴴���߳�ʼ��
	eFileInit_Entrants,         //��������߳�ʼ��
	eFileThread_Upload,         // �����ϴ���
	eFileThread_Download,       // ����������
	eFileThread_Close,          // δ�ϴ�Ҳδ����
}enumFiletype;

//�����������
typedef enum tagcurlMeetingType
{
	eMeeting_default = 130,
	eMeeting_getLaunchstring,       //��ȡ�������
	eMeeting_JoinMeeting,           //���
	eMeeting_MeetingSync,           //
	eMeeting_MeetingCreate,         //��������
	eMeeting_MeetingChange,         //������Ϣ�޸�
	eMeeting_MeetingCancel,         //ȡ������
	eMeeting_MeetingMemberChange,   //������Ա�䶯
	eMeeting_MeetingLevelChange,    //���鼶��䶯
	eMeeting_ReGetMeetingInfo,      //���»�ȡ������Ϣ
	eMeeting_ReceiveMeeting,        //���ܻ�������
	eMeeting_MeetingRead,           //�����Ѷ�
	eMeeting_FileChange,            //�޸��ļ�
	eMeeting_GetMeetingVideoUrl,    //��ȡ����¼�Ƶ���Ƶ
	eMeeting_GetMeetingRoomlist,    //̩�̻�ȡ���û�����
}enumcurlMeetingType;

typedef enum tagMemberNodeType
{
	eMemberNode_Creator = 300,      //�����ߴ���ʾ
	eMemberNode_Entrants,           //�λ��ߴ���ʾ
	eMemberNode_Noremove,
};

//����״̬
typedef enum tagMeetingState
{
	eMeeting_NotStart = 400,        //����δ��ʼ
	eMeeting_Starting,              //���������
	eMeeting_End,                   //�������
}enumMeetingState;

//���鸽���ϴ����ػص�
typedef struct tagUploadMeetingFile
{/////////
	char confid[CONFID_LENGTH];  //����ID
	INT32 fileSize;         //�ļ���С
	int fileIndex;          //�ļ���ҳ���ϵ�Index
	int ThreadType;         //���ͣ�0���ϴ� 1�����أ�
	int i32stop;            //0 ���жϣ�1���ж�
	int i32Progress;        //�ϴ�����
	TCHAR  szToken[CONFFILETOKEN_LENGTH]; // �ļ�Token
	tagUploadMeetingFile()
	{
		memset(confid,0,CONFID_LENGTH);
		memset(szToken,0,32);
	}
}S_UploadMeetingFile;

typedef struct tagMeetingFileInfo
{
	//����������
	char confid[CONFID_LENGTH];  //����ID
	int  fileNum;                //�ļ�����
	TCHAR szLocalPath[256];      //����·��
	TCHAR szFileName[256];       //�ļ���
	TCHAR szCurlUrl[1024];       //�ļ�URL
	TCHAR szCurlToken[1024];     //�ļ�TOKEN
	long timeKey;                //key��ʶ
	S_UploadMeetingFile* sUploadMeetingFile;
	tagMeetingFileInfo()
		:fileNum(0)
		,timeKey(0)
		,sUploadMeetingFile(NULL)
	{
		memset(confid,0,CONFID_LENGTH);
		memset(szLocalPath, 0 , sizeof(TCHAR)*256);
		memset(szFileName, 0 ,sizeof(TCHAR)*256);
		memset(szCurlUrl, 0 ,sizeof(TCHAR)*1024);
		memset(szCurlToken, 0 ,sizeof(TCHAR)*1024);
	};
}S_MeetingFileInfo;

typedef struct tagRemoveFile
{
	char confid[CONFID_LENGTH];         //����ID
	TCHAR  token[CONFFILETOKEN_LENGTH]; //�ļ�token
	int FileIndex;                      //�ļ����ļ��б��index
	tagRemoveFile()
		:FileIndex(0)
	{
		memset(confid,0,CONFID_LENGTH);
		memset(token,0,sizeof(TCHAR)*32);
	}
}S_RemoveFile;

typedef struct tagFaildTip
{
	char confid[CONFID_LENGTH];  //����ID
	char tip[100];               //ʧ����ʾ
	tagFaildTip()
	{
		memset(confid,0,CONFID_LENGTH);
		memset(tip,0,100);
	}
}S_FaildTip;

typedef struct tagGetLaunchFaild
{
	int type;              //����
	char tip[100];         //ʧ����ʾ
	tagGetLaunchFaild()
		:type(1)
	{
		memset(tip,0,100);
	}
}S_GetLaunchFaild;

typedef struct tagMeetingVideoUrl
{
	char confid[CONFID_LENGTH]; //����ID
	char video[128];         //��Ƶ¼������
	char videoDownUrl[128];  //��Ƶ¼����������
	char DocUrl[128];        //����ĵ�Ԥ������
	char DocDownUrl[128];    //����ĵ���������
	tagMeetingVideoUrl()
	{
		memset(video,0,128);
		memset(videoDownUrl,0,128);
		memset(DocUrl,0,128);
		memset(DocDownUrl,0,128);
	}
}S_MeetingVideoUrl;

//�����ļ��ɹ������������֪ͨ
typedef struct tagUpdateAttachments
{
	char conferenceId[CONFID_LENGTH];  //����ID
	int userid;            //����ID
	int confUserId;        //ȫʱID
	QSID msgid ;           //��ϢID
	int modify;            //��������
	TCHAR  name[128];      //�ļ���
	TCHAR  token[CONFFILETOKEN_LENGTH];  //�ļ�token
	int size;
	tagUpdateAttachments()
		:userid(0)
		,confUserId(0)
		,msgid(0)
		,modify(0)
		,size(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
		memset(name,0,sizeof(TCHAR)*128);
		memset(token,0,sizeof(TCHAR)*32);
	}
}S_UpdateAttachments;

//������Ϣ��ҳ����ʾ�ã�
typedef struct tagFileInf2Detail
{
	char fileToken[CONFFILETOKEN_LENGTH];  //�ļ�TOKEN
	char fileName[128];                    //�ļ���
	char filePath[400];                    //�ļ�·��
	UINT32 fileSize;                       //�ļ���С
	UINT32 dwUpdateTime;                   //����ʱ��
	tagFileInf2Detail()
		:fileSize(0)
		,dwUpdateTime(0)
	{
		memset(fileToken,0,32);
		memset(filePath,0,128);
		memset(filePath,0,400);
	}
}S_FileInf2Detail;

//������ȡ������Ϣ
typedef struct tagReGetMeetingInfo
{
	int usid ;                        //����ID
	char conferenceId[CONFID_LENGTH]; //����ID
	int terminal;                     //�����ͻ��ˣ�1:android,2:ios,3:PC��
	INT64 msgid;                      //��ϢID
	INT64 t;                          //ʱ���
	tagReGetMeetingInfo()
		:usid(0)
		,terminal(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
	}
}S_ReGetMeetingInfo;

//���ܻ���
typedef struct tagReceiveMeeting
{
	char conferenceId[CONFID_LENGTH];  //����ID
	long  userid;              //����ID
	long  creatorAcct;         //���鴴��������ID
	INT64 msgid;               //��ϢID
	INT64 t;                   //ʱ���
	tagReceiveMeeting()
		:userid(0)
		,creatorAcct(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
	}
}S_ReceiveMeeting;

//�����Ѷ�
typedef struct tagMeetingRead
{
	char conferenceId[CONFID_LENGTH];  //����ID
	long  userid;              //����ID
	INT64 msgid;               //��ϢID
	INT64 t;                   //ʱ���
	tagMeetingRead()
		:userid(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
	}
}S_MeetingRead;

//��ȡ����¼����Ƶ
typedef struct tagMeetingGetVideo
{
	char conferenceId[CONFID_LENGTH];     //����ID
	long  userid;                         //����ID
	INT64 t;                              //ʱ���
}S_MeetingGetVideo;

typedef struct tagFileIsExistTip
{
	char conferenceId[CONFID_LENGTH];     //����ID
	char token[CONFFILETOKEN_LENGTH];     //�ļ�TOKEN 
	int fileIndex;             //�ļ����б��еı�ʶ
	tagFileIsExistTip()
		:fileIndex(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
		memset(token,0,CONFFILETOKEN_LENGTH);
	}
}S_FileIsExistTip;

typedef struct tagMeetingDetail
{
	int Detailtype;   //0�������� 1���λ���
	void* Detailhwnd; //����ָ��
}S_MeetingDetail;


//̩�̻�ȡ�������б�����
typedef struct tagMeeting_GetRoomlist
{
	UINT32 userid;                //�������û�������userid��
	TCHAR usercode[32];			  //�������û���������usercode��
	int length;                   //����ʱ��,����Ϊ��λ
	INT64 starttime;              //���鿪ʼʱ��
	INT64 t;                      //ʱ���(��ʽΪlong��������)
	TCHAR mdkey[128];             //mdkey�����ڼ�Ȩmdkey = Encrip t.md5(userid + t Encript.getEncriptKey ())

}S_Meeting_GetRoomlist,*PS_Meeting_GetRoomlist;

//̩�̻�ȡ�������б�����󷵻���Ϣ��ҳ��չʾ�ã�
typedef struct tagMeetingGetRoomlist_Ack
{
	UINT32 meetingRoomType;		//���������ͣ����磺6��10��25��
	UINT32 usableNum;			//���ø���

	bool bChecked;				//�Ƿ�Ϊѡ�еĻ����ң�Ĭ��δѡ�У���һ�μ���ʱ��Ĭ��ѡ�е�һ��������
	
}S_MeetingGetRoomlist_Ack;

typedef enum tagMeetingFaildType
{
	eMeeting_GetLaunchStringFaild = 2,  //��ȡ������ʧ��
	eMeeting_UploadFaild,               //����ȫʱ�ͻ���ʧ��
	eMeeting_JoinFaild,                 //���ʧ��
};

//ȫʱSDK��⵽��ȫʱ���״̬
typedef enum tagMeetingSdkState
{
	eMeeting_Uploading = 1,         //��������
	eMeeting_Download,              //��������
	eMeeting_Install,               //���ڰ�װ
	eMeeting_Joining,               //�������
};

typedef enum tagMeetingUrlColumn
{
	eVideoUrl = 1,                 //videourl
	eVideoDownUrl,                 //videodownurl
	eDocUrl,                       //
	eDocDownUrl,                   //sharedocurl
};
//��ȡ����������Ϣ�Ļص�
//������S_ConfBasicInfo  - ���������Ϣ
//����:
//      1  - �ɹ�
//		0  - ʧ��
typedef int (__stdcall* PFN_GetMeetingItem)(S_ConfBasicInfo sInfo, void* pvUserData);

//��ȡ������Ϣ�Ļص�
//������S_ConfBasicInfo  - ���������Ϣ
//����:
//      1  - �ɹ�
//		0  - ʧ��
typedef int (__stdcall* PFN_MeetingBasicInfoCb_All)(S_ConfBasicInfo* pBasicInfo, void* pvUserData);
typedef int (__stdcall* PFN_MeetingBasicInfoCb)(confBasicInfo* pBasicInfo, void* pvUserData);

//��ȡ����������Ա�Ļص�
//������confBasicMbrInfo  - ������Ա��Ϣ
//����:
//      1  - �ɹ�
//		0  - ʧ��
typedef int (__stdcall* PFN_MeetingBasicMbrInfoCb)(confBasicMbrInfo* pBasicMbrInfo, void* pvUserData);

//��ȡ�������鸽���Ļص�
//������S_FileInf2Detail  - ������Ա��Ϣ
//����:
//      1  - �ɹ�
//		0  - ʧ��
typedef int (__stdcall* PFN_MeetingFileInfoCb)(S_FileInf2Detail* pFileInfo,void* pvUserData);

class  __declspec(novtable) I_MeetingMgr: public I_EIMUnknown
{
public:
	//=============================================================================
	//Function:     Set
	//Description:	Store meeting basic information
	//
	//Parameter:
	//	confBasicInfo  - The basic information of the distributed server
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Set(const confBasicInfo* mbrInfo) = 0;

	//=============================================================================
	//Function:     Set
	//Description:	Store meeting member information
	//
	//Parameter:
	//	confMbrInfoNotice  - The members information of the distributed server
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Set(const confMbrInfoNotice* mbrInfo) = 0;
	
	//=============================================================================
	//Function:     Set
	//Description:	Store meeting files information
	//
	//Parameter:
	//	confFileInfoNotice  - The fimes information of the distributed server
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Set(const confFileInfoNotice* fileInfo) = 0;
	
	//=============================================================================
	//Function:     Set
	//Description:	Store meeting remark
	//
	//Parameter:
	//	confRemarksNotice  - The remark of information from distributed server
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Set(const confRemarksNotice* remarkInfo) = 0;
	
	//=============================================================================
	//Function:     EnumMeeting
	//Description:	get single meeting information
	//
	//Parameter:
	//	PFN_GetMeetingItem  - 
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
    virtual DWORD  EnumMeeting(PFN_GetMeetingItem pfnGetMeetingItem, void *pvUserData) = 0;
	//��û��������Ϣ���������ã�
	virtual int Get(char* confId,PFN_MeetingBasicInfoCb pfnCb,void* pvUserData) = 0;
	
	//��û�����Ϣ(���·�װ�Ľṹ��)
	//=============================================================================
	//Function:     Get
	//Description:	get single meeting information
	//
	//Parameter:
	//	PFN_MeetingBasicInfoCb_All  - 
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Get(char* confId,PFN_MeetingBasicInfoCb_All pfnCb,void* pvUserData) = 0;
	
	//=============================================================================
	//Function:     Get
	//Description:	get single meeting members information
	//
	//Parameter:
	//	PFN_MeetingBasicMbrInfoCb  - 
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Get(char* confId,PFN_MeetingBasicMbrInfoCb pfnCb,void* pvUserData) = 0;
	
	//=============================================================================
	//Function:     Get
	//Description:	get single meeting files information
	//
	//Parameter:
	//	PFN_MeetingFileInfoCb  - 
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int Get(char* confid,PFN_MeetingFileInfoCb pfnCb,void* pvUserData) = 0;
	
	//=============================================================================
	//Function:     GetMeetingTimeStamp
	//Description:	get meeting updatetime
	//Return:
	//      int - updatetime
	//=============================================================================
	virtual int GetMeetingTimeStamp() = 0 ;
	
	//=============================================================================
	//Function:     GetTodayMeetingCount
	//Description:	Get all the meeting of today
	//
	//Parameter:
	//	userid   - id
	//  count   -  numbers of meeting
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int GetTodayMeetingCount(QEID userid,int &count) = 0;

	//=============================================================================
	//Function:     UpdateMeetingLevel
	//Description:	Update meeting level
	//Parameter:
	//	confId   - meeting id
	//  qeid   -  userid
	//  level   -  meeting'level
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int UpdateMeetingLevel(const char* confId,QEID qeid,int Level) = 0;
	
	//=============================================================================
	//Function:     GetMeetingLevel
	//Description:	judge the meeting is important
	//Parameter:
	//	confId   - meeting id
	//  qeid   -  userid
	//Return:
	//      1  - yes
	//		0  - no
	//=============================================================================
	virtual int GetMeetingLevel(char* confId,QEID qeid) = 0;

	//=============================================================================
	//Function:     GetMeetingCreator
	//Description:	get meeting creator
	//Parameter:
	//	confId   - meeting id
	//Return:
	//      int   - creator id
	//=============================================================================
	virtual int GetMeetingCreator(const char* confid) = 0;

	//=============================================================================
	//Function:     UpdateFilePath
	//Description:	update meeting file path
	//Parameter:
	//	confId   - meeting id
	//  fileToken  -  file token
	//  filepath  -  file path
	//  timeKey   - updatetime
	//  flag  -   flag
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int UpdateFilePath(const char* confid,const TCHAR* fileToken,const TCHAR* filePath,long timeKey,int flag=0) = 0;

	//=============================================================================
	//Function:     UpdateMeetingIsRead
	//Description:	update meeting is read
	//Parameter:
	//	confId   - meeting id
	//  qeid  -  userid
	//  IsRead  -  read
	//Return:
	//      1  - succeed
	//		0  - failed
	//=============================================================================
	virtual int UpdateMeetingIsRead(char* confId,QEID qeid,int IsRead) = 0;
	
	//=============================================================================
	//Function:     GetMeetingIsRead
	//Description:	get meeting is read
	//Parameter:
	//	confId   - meeting id
	//  qeid  -  userid
	//Return:
	//      1  - resd
	//		0  - not read
	//=============================================================================
	virtual int GetMeetingIsRead(char* confId,QEID qeid) = 0;

	//=============================================================================
	//Function:     GetMeetingIsAccept
	//Description:	get meeting is accept
	//Parameter:
	//	confId   - meeting id
	//  qeid  -  userid
	//Return:
	//      1  - accept
	//		0  - no
	//=============================================================================
	virtual int GetMeetingIsAccept(const char* confId,QEID qeid) = 0;

	//=============================================================================
	//Function:     CheckeFileIsExist
	//Description:	whether the file is belong the meeting 
	//Parameter:
	//	confId   - meeting id
	//  token  -  file token
	//  timeKey - time
	//Return:
	//      1  - yes
	//		0  - no
	//=============================================================================
	virtual BOOL CheckeFileIsExist(char* confid,char* token,long timeKey) = 0;

	//=============================================================================
	//Function:     DeletePathTableFile
	//Description:	delete data of t_meeting_filepath
	//Parameter:
	//	confId   - meeting id
	//  token  -  file token
	//Return:
	//      1  - success
	//		0  - faild
	//=============================================================================
	virtual int DeletePathTableFile(const char* confid,const char* token) = 0;

	//=============================================================================
	//Function:     SetMeetingUrl
	//Description:	delete data of t_meeting_filepath
	//Parameter:
	//	confId   - meeting id
	//  url  -  url
	//  tagMeetingUrlColumn  -
	//Return:
	//      1  - success
	//		0  - faild
	//=============================================================================
	virtual int SetMeetingUrl(const char* confid, const char* url,tagMeetingUrlColumn urlType) = 0;

};

#endif // __IEIMMSGCOLLECTMSGR_H__