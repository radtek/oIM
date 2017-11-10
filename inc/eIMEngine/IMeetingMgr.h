
#ifndef __IMEETINGMGR_H__
#define __IMEETINGMGR_H__

#include "Public\Plugin.h"
#include "../../src/ClientAgent/Src/protocol.h"

//会议JSON返回字段
#define  MEETING_ACK_RESULT           ("result")
#define  MEETING_ACK_TYPE             ("type")
#define  MEETING_ACK_STATUS           ("status")
#define  MEETING_ACK_LAUNCHSTRING     ("launchString")
#define  MEETING_ACK_DATA           ("data")
#define  MEETING_ACK_FAILDTYPE        ("faildtype")

//会议基本信息（用于页面展示用）
typedef struct tagConfBasicInfo
{
	confBasicInfo basicInfo;  //服务器下发基本信息
	char videoUrl[128];       //会后录制视频预览地址
	char videoDownUrl[128];   //会后录制视频下载地址
	char ShareDocUrl[128];    //会后文档下载地址
	char Remark[1540];        //会议被指
}S_ConfBasicInfo;

//获取入会调起串（入会时使用）
typedef struct tagGetLaunchString
{
	char confid[CONFID_LENGTH];   //会议ID
	char confUserName[60];        //会议username
	char password[60];            //入会密码
	INT64 t;                      //时间戳
	INT32 userid;                 //万信ID
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

//入会参数
typedef struct tagJoinConference
{
	UINT	   nSessionID;   //会议ID
	UINT	   nSessionType;  //会议类型
	UINT	   nLoadType;     //会议加载类型
	eIMStringA szUserID;     //全时ID
	eIMStringA szCmdLine;    //会议调起串
	eIMStringA szHeadImgUrl; //头像URL
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

//会议同步
typedef struct tagMeetingSync
{
    UINT64        uMsgId;     //MSGid
    UINT32        userid;     //万信ID
    UINT32        confUserId; //全时ID
    UINT32        uTime;      //同步时间戳
	tagMeetingSync()
		:uMsgId(0)
		,userid(0)
		,confUserId(0)
		,uTime(0)
	{
		
	}
}S_MeetingSync;

#define MAXLENGTH_INVITE    (400)
//创建/修改会议请求
typedef struct tagMeeting_create
{
	int operationType;            // 操作类型 （0：创建会议 ， 1：修改会议）
	char confId[CONFID_LENGTH];   // 会议ID 修改会议是有效
	int conferenceType;           //会议类型（0：线下会议，1：云会议）
	UINT32 userid;                //操作者用户（万信userid）
	UINT32 confUserId;            //操作者用户（全时对应的userid）
	TCHAR title[40];              //会议主题
	int length;                   //会议时长,分钟为单位
	INT64 starttime;              //会议开始时间
	INT64 inviteeList[INVITEE_MAX]; //会议参与者，万信userid
	int MemberNum  ;              //本地辅助参数 ,会议参与人数
	TCHAR msgid[32];              //消息编号，消息唯一标示
	int repetitionType;           //循环会议(0:非循环，1: 循环)
	INT64 circulateStartTime;     //循环会议开始时间，循环会议时必填，（格式为long，毫秒数）
	INT64 circulateEndTime;       //循环会议结束时间，循环会议时必填，（格式为long，毫秒数）
	int circulateType;            //循环类型（0:每天，1:每周，2:每月）
	int circulateNode;            //循环节点 ,当循环类型为每天是值为0,当循环类型为每周是值为1~7,当循环类型为每月是值为1~31
	int conferenceMode;           //会议模式（0:协作模式，1:会场模式）
	INT64 t;                      //时间戳(格式为long，毫秒数)
	TCHAR mdkey[128];             //mdkey，用于鉴权mdkey = Encrip t.md5(userid + t Encript.getEncriptKey ())
	TCHAR address[64];            //会议地点
	TCHAR remark[512];            //备注
	int  memberCount;             //会议人数上限
	int  messagenotice;           //是否有短信提醒 0:不提醒 1:提醒
	int  conferenceLevel;         //会议级别 0：普通会议，1：重要会议

	tagMeeting_create()
	{
		memset(inviteeList,0,sizeof(INT64)*INVITEE_MAX);
	}
}S_Meeting_create,*PS_Meeting_create;

//创建会议后返回信息（页面展示用）
typedef struct tagMeetingCreate_Ack
{
	char conferenceId[CONFID_LENGTH]; //会议ID
	char hostCode[CONFCODE_LENGTH];   //主持人Code
	char mbrCode[CONFCODE_LENGTH];    //参会者Code
	char hostUrl[256];                //主持人Url
	char mbrUrl[256];                 //参会者Url

	char protocolHostStartUrl[256];   //泰禾主持人入会Url
	
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

//创建会议后得到的基本信息（页面展示用）
typedef struct tagMeetingBasicInfo_ack
{
	char confid[CONFID_LENGTH];      //会议ID
	char hostCode[CONFCODE_LENGTH];  //主持人Code
	char mbrCode[CONFCODE_LENGTH];   //参会者Code
	char hostUrl[256];               //主持人Url
	char mbrUrl[256];                //参会者Url
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

//修改会议人员
typedef struct tagMeetingMemberChange
{
	char conferenceId[CONFID_LENGTH];  // 会议ID
	int userid;             // 万信ID
	int confUserId;         // 全时ID
	INT64 msgid ;           // 消息ID
	int memberaddNum;       // 增加人员数量 (必填)
	int memberdelNum;       // 删除人员数量 （必填）
	INT64 mbrAddList[CHANGEMEMBERS_MAX];  // 增加的人员列表
	INT64 mbrDelList[CHANGEMEMBERS_MAX];  // 删除的人员列表
	INT64 t;                // 时间戳
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

//取消会议
typedef struct tagMeetingCancel
{ 
	char conferenceId[CONFID_LENGTH];   //全时ID
	TCHAR  circulateKey[32];            //循环会议标识
    int userid;                         //万信ID
    int confUserId;                     //全时ID
	INT64  msgid;                       //消息ID
    int  delType;                       //删除类型（0:删除选中会议，1: 删除循环会议）如果是循环会议必填
    int  conferenceType;;               //会议类型（0：线下会议，1：云会议）
    UINT8 strConfLang;                  //语言 confUserInfo结构体中有
	INT64 t;                            //时间戳

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

//修改会议级别
typedef struct tagMeetingLevel
{
	char conferenceId[CONFID_LENGTH]; //全时ID
	int userid;                 //万信ID
	int conferenceLevel;        //会议级别
	INT64  msgid;               //消息ID
	INT64 t;                    //时间戳

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
    INT64  msgid;                //消息ID
    UINT32 userid;               //万信ID
    UINT32 confUserId;           //全时ID
    UINT32    starttime;         //开始时间
    INT32     inviteeNum;        //邀请人数
    UINT32    inviteeList[100];  //邀请列表
    INT32  conferenceType;       //会议类型
    INT32  memberCount;          //会议人数
    INT32    length;             //时长
    INT32    messagenotice;      //是否有短信提醒
    INT32    conferenceLevel;    //会议级别
    char   title[41];            //标题
    char    address[1024];       //地址
    char    remark[1540];        //备注
    char conferenceId[33];       //会议ID

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

//会议操作返回对应的类型
typedef enum tagMeetingAckType
{
	eMEETING_CREATE = 201,     //创建会议
	eMEETING_UPDATE,           //修改会议信息
	eMEETING_UPDATEMEMBERS,    //添加/删除会议成员
	eMEETING_UPDATEATTACHMENTS,//添加/删除会议附件
	eMEETING_CANCEL,           //取消预约会议
	eMEETING_SYN = 207,        //同步会议列表
	eMEETING_GETLAUNCH = 210,  //获取会议吊起串
	eMEETING_ACCEPT = 211,     //会议成员接受会议

}ENUM_MEETINGACK;

//会议附件初始化类型
typedef enum tagFiletype
{
	eFileUpload_Creator = 100,  //会议创建者上传
	eFileInit_Creator,          //会议创建者初始化
	eFileInit_Entrants,         //会议参与者初始化
	eFileThread_Upload,         // 正在上传中
	eFileThread_Download,       // 正在下载中
	eFileThread_Close,          // 未上传也未下载
}enumFiletype;

//会议操作类型
typedef enum tagcurlMeetingType
{
	eMeeting_default = 130,
	eMeeting_getLaunchstring,       //获取会议调起传
	eMeeting_JoinMeeting,           //入会
	eMeeting_MeetingSync,           //
	eMeeting_MeetingCreate,         //创建会议
	eMeeting_MeetingChange,         //会议信息修改
	eMeeting_MeetingCancel,         //取消会议
	eMeeting_MeetingMemberChange,   //会议人员变动
	eMeeting_MeetingLevelChange,    //会议级别变动
	eMeeting_ReGetMeetingInfo,      //重新获取会议信息
	eMeeting_ReceiveMeeting,        //接受会议邀请
	eMeeting_MeetingRead,           //会议已读
	eMeeting_FileChange,            //修改文件
	eMeeting_GetMeetingVideoUrl,    //获取会议录制的视频
	eMeeting_GetMeetingRoomlist,    //泰禾获取可用会议室
}enumcurlMeetingType;

typedef enum tagMemberNodeType
{
	eMemberNode_Creator = 300,      //创建者处显示
	eMemberNode_Entrants,           //参会者处显示
	eMemberNode_Noremove,
};

//会议状态
typedef enum tagMeetingState
{
	eMeeting_NotStart = 400,        //会议未开始
	eMeeting_Starting,              //会议进行中
	eMeeting_End,                   //会议结束
}enumMeetingState;

//会议附件上传下载回调
typedef struct tagUploadMeetingFile
{/////////
	char confid[CONFID_LENGTH];  //会议ID
	INT32 fileSize;         //文件大小
	int fileIndex;          //文件在页面上的Index
	int ThreadType;         //类型（0：上传 1：下载）
	int i32stop;            //0 不中断，1：中断
	int i32Progress;        //上传进度
	TCHAR  szToken[CONFFILETOKEN_LENGTH]; // 文件Token
	tagUploadMeetingFile()
	{
		memset(confid,0,CONFID_LENGTH);
		memset(szToken,0,32);
	}
}S_UploadMeetingFile;

typedef struct tagMeetingFileInfo
{
	//仅作下载用
	char confid[CONFID_LENGTH];  //会议ID
	int  fileNum;                //文件数量
	TCHAR szLocalPath[256];      //本地路径
	TCHAR szFileName[256];       //文件名
	TCHAR szCurlUrl[1024];       //文件URL
	TCHAR szCurlToken[1024];     //文件TOKEN
	long timeKey;                //key标识
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
	char confid[CONFID_LENGTH];         //会议ID
	TCHAR  token[CONFFILETOKEN_LENGTH]; //文件token
	int FileIndex;                      //文件在文件列表的index
	tagRemoveFile()
		:FileIndex(0)
	{
		memset(confid,0,CONFID_LENGTH);
		memset(token,0,sizeof(TCHAR)*32);
	}
}S_RemoveFile;

typedef struct tagFaildTip
{
	char confid[CONFID_LENGTH];  //会议ID
	char tip[100];               //失败提示
	tagFaildTip()
	{
		memset(confid,0,CONFID_LENGTH);
		memset(tip,0,100);
	}
}S_FaildTip;

typedef struct tagGetLaunchFaild
{
	int type;              //类型
	char tip[100];         //失败提示
	tagGetLaunchFaild()
		:type(1)
	{
		memset(tip,0,100);
	}
}S_GetLaunchFaild;

typedef struct tagMeetingVideoUrl
{
	char confid[CONFID_LENGTH]; //会议ID
	char video[128];         //视频录制链接
	char videoDownUrl[128];  //视频录制下载链接
	char DocUrl[128];        //会后文档预览链接
	char DocDownUrl[128];    //会后文档下载链接
	tagMeetingVideoUrl()
	{
		memset(video,0,128);
		memset(videoDownUrl,0,128);
		memset(DocUrl,0,128);
		memset(DocDownUrl,0,128);
	}
}S_MeetingVideoUrl;

//发送文件成功后给服务器的通知
typedef struct tagUpdateAttachments
{
	char conferenceId[CONFID_LENGTH];  //会议ID
	int userid;            //万信ID
	int confUserId;        //全时ID
	QSID msgid ;           //消息ID
	int modify;            //操作类型
	TCHAR  name[128];      //文件名
	TCHAR  token[CONFFILETOKEN_LENGTH];  //文件token
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

//附件信息（页面显示用）
typedef struct tagFileInf2Detail
{
	char fileToken[CONFFILETOKEN_LENGTH];  //文件TOKEN
	char fileName[128];                    //文件名
	char filePath[400];                    //文件路径
	UINT32 fileSize;                       //文件大小
	UINT32 dwUpdateTime;                   //更新时间
	tagFileInf2Detail()
		:fileSize(0)
		,dwUpdateTime(0)
	{
		memset(fileToken,0,32);
		memset(filePath,0,128);
		memset(filePath,0,400);
	}
}S_FileInf2Detail;

//单独获取会议信息
typedef struct tagReGetMeetingInfo
{
	int usid ;                        //万信ID
	char conferenceId[CONFID_LENGTH]; //会议ID
	int terminal;                     //操作客户端（1:android,2:ios,3:PC）
	INT64 msgid;                      //消息ID
	INT64 t;                          //时间戳
	tagReGetMeetingInfo()
		:usid(0)
		,terminal(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
	}
}S_ReGetMeetingInfo;

//接受会议
typedef struct tagReceiveMeeting
{
	char conferenceId[CONFID_LENGTH];  //会议ID
	long  userid;              //万信ID
	long  creatorAcct;         //会议创建者万信ID
	INT64 msgid;               //消息ID
	INT64 t;                   //时间戳
	tagReceiveMeeting()
		:userid(0)
		,creatorAcct(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
	}
}S_ReceiveMeeting;

//会议已读
typedef struct tagMeetingRead
{
	char conferenceId[CONFID_LENGTH];  //会议ID
	long  userid;              //万信ID
	INT64 msgid;               //消息ID
	INT64 t;                   //时间戳
	tagMeetingRead()
		:userid(0)
		,msgid(0)
		,t(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
	}
}S_MeetingRead;

//获取会议录制视频
typedef struct tagMeetingGetVideo
{
	char conferenceId[CONFID_LENGTH];     //会议ID
	long  userid;                         //万信ID
	INT64 t;                              //时间戳
}S_MeetingGetVideo;

typedef struct tagFileIsExistTip
{
	char conferenceId[CONFID_LENGTH];     //会议ID
	char token[CONFFILETOKEN_LENGTH];     //文件TOKEN 
	int fileIndex;             //文件在列表中的标识
	tagFileIsExistTip()
		:fileIndex(0)
	{
		memset(conferenceId,0,CONFID_LENGTH);
		memset(token,0,CONFFILETOKEN_LENGTH);
	}
}S_FileIsExistTip;

typedef struct tagMeetingDetail
{
	int Detailtype;   //0：创建者 1：参会者
	void* Detailhwnd; //窗口指针
}S_MeetingDetail;


//泰禾获取会议室列表请求
typedef struct tagMeeting_GetRoomlist
{
	UINT32 userid;                //操作者用户（万信userid）
	TCHAR usercode[32];			  //操作者用户名（万信usercode）
	int length;                   //会议时长,分钟为单位
	INT64 starttime;              //会议开始时间
	INT64 t;                      //时间戳(格式为long，毫秒数)
	TCHAR mdkey[128];             //mdkey，用于鉴权mdkey = Encrip t.md5(userid + t Encript.getEncriptKey ())

}S_Meeting_GetRoomlist,*PS_Meeting_GetRoomlist;

//泰禾获取会议室列表请求后返回信息（页面展示用）
typedef struct tagMeetingGetRoomlist_Ack
{
	UINT32 meetingRoomType;		//会议室类型（比如：6、10、25）
	UINT32 usableNum;			//可用个数

	bool bChecked;				//是否为选中的会议室，默认未选中，第一次加载时，默认选中第一个会议室
	
}S_MeetingGetRoomlist_Ack;

typedef enum tagMeetingFaildType
{
	eMeeting_GetLaunchStringFaild = 2,  //获取入会调起串失败
	eMeeting_UploadFaild,               //更新全时客户端失败
	eMeeting_JoinFaild,                 //入会失败
};

//全时SDK检测到的全时组件状态
typedef enum tagMeetingSdkState
{
	eMeeting_Uploading = 1,         //正在升级
	eMeeting_Download,              //正在下载
	eMeeting_Install,               //正在安装
	eMeeting_Joining,               //正在入会
};

typedef enum tagMeetingUrlColumn
{
	eVideoUrl = 1,                 //videourl
	eVideoDownUrl,                 //videodownurl
	eDocUrl,                       //
	eDocDownUrl,                   //sharedocurl
};
//获取单个会议信息的回调
//参数：S_ConfBasicInfo  - 会议基本信息
//返回:
//      1  - 成功
//		0  - 失败
typedef int (__stdcall* PFN_GetMeetingItem)(S_ConfBasicInfo sInfo, void* pvUserData);

//获取会议信息的回调
//参数：S_ConfBasicInfo  - 会议基本信息
//返回:
//      1  - 成功
//		0  - 失败
typedef int (__stdcall* PFN_MeetingBasicInfoCb_All)(S_ConfBasicInfo* pBasicInfo, void* pvUserData);
typedef int (__stdcall* PFN_MeetingBasicInfoCb)(confBasicInfo* pBasicInfo, void* pvUserData);

//获取单个会议人员的回调
//参数：confBasicMbrInfo  - 会议人员信息
//返回:
//      1  - 成功
//		0  - 失败
typedef int (__stdcall* PFN_MeetingBasicMbrInfoCb)(confBasicMbrInfo* pBasicMbrInfo, void* pvUserData);

//获取单个会议附件的回调
//参数：S_FileInf2Detail  - 会议人员信息
//返回:
//      1  - 成功
//		0  - 失败
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
	//获得会议基本信息（基本不用）
	virtual int Get(char* confId,PFN_MeetingBasicInfoCb pfnCb,void* pvUserData) = 0;
	
	//获得会议信息(重新封装的结构体)
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