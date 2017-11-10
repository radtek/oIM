#ifndef __FILE_STORAGE_UTIL
#define __FILE_STORAGE_UTIL

#include "Public\Errors.h"

#define LOCAL_PATH_LEN					512		//本地文件路径
#define SERVER_URL_LEN					512		//服务器URL
#define USER_ID_LEN						64		//用户ID长度
#define FILE_NAME_LEN					256		//文件名长度
#define FILE_UPLOAD_KEY					128		//上传文件返回KEY值
#define HTTP_ERR_MSG_LEN				1024    //格式化WININET错误信息的长度
#define SHORT_KEY_LEN					8		//文件下载验证，生成ShortKey的长度（只计算了第1组8个字符生成的结果）

//使用登录应答协议中字段：UINT16 wPCMaxSendFileSize; // PC文件最大发送大小：单位为M，整数，2字节
// #define FILE_TRANS_MODE_NON_RESUME      50
// #define FILE_TRANS_MODE_RESUME		 51

#pragma pack( push, 8 )

//文件传输优先级，级别顺序不允许变动
typedef enum tagEIMTaskPriority  
{
	eInterFile_High = 1,						//最高优先级
	eInterFile_AboveNormal,						//中等优先级
	eInterFile_Normal,							//默认
}E_EIMTaskPriority, *PE_EIMTaskPriority;

//文件传输方向
typedef enum tagEIMFileTransDirec
{
	eInterFile_Upload = 1,					//上传
	eInterFile_Download,					//下载
}E_EIMFileTransDirec, *PE_EIMFileTransDirec;

typedef enum tagEIMFileType
{
	eInterFile_File,						//普通文件
	eInterFile_Image,						//图片
	eInterFile_HeadImg,						//头像
}E_EIMFileType, *PE_EIMFileType;

//URL子串是否加密
typedef enum tagEIMEncrpt
{
	eInterFile_NoEncrpt,
	eInterFile_Encrpt,
}E_EIMEncrpt;

//传输方式
typedef enum tagEIMTransWay
{
	eInterFile_NonResuming=0,    //普通方式
	eInterFile_Resuming=1,  //断点续传
}E_EIMTransWay;


//传输文件信息，需要讨论下具体需要哪些信息
typedef struct tagEIMFileInfo 
{
	UINT64 Qfid;								//唯一标识文件，上层生成返回给底层
	TCHAR tszFileName[FILE_NAME_LEN];			//文件名 上传下载时使用
	TCHAR tszDownServerUrl[SERVER_URL_LEN];     //下载服务器地址,地址为全路径  断点方式时存储断点文件URL并且只将配置文件中URL原样存储不在需要key=%s
	TCHAR tszUpServerUrl[SERVER_URL_LEN];		//上传服务器地址  断点方式时存储断点文件URL
	TCHAR tszLocalPath[LOCAL_PATH_LEN];			//本地路径,地址仅为路径名，不包括文件名，结尾不包含"\"，例如：C:\aa 
	E_EIMTaskPriority	ePriority;				//优先级
	E_EIMFileTransDirec eDirec;					//传输方向
	E_EIMFileType eFileType;					//普通文件和头像
	E_EIMEncrpt   eEncrpt;						//是否对文件、图片、长文件进行加密传输

	TCHAR tszFileKey[FILE_UPLOAD_KEY];			//仅断点续传使用：服务端返回的KEY，下载时使用，此KEY已不在是6位，40位之内
	TCHAR tszUpTokenUrl[SERVER_URL_LEN];		//仅断点续传使用：上传时，配置文件中TOKEN的URL
	E_EIMTransWay eTransWay;					//是否对文件、图片进行断点续传方式传输
}S_EIMFileInfo, *PS_EIMFileInfo;

//传输方式同步、异步
typedef enum tagEIMTransType  
{
	eInterFile_Sync = 1,            //同步
	eInterFile_Async,               //异步
}E_EIMTransType, *PE_EIMTransType;

//任务的传输状态
typedef enum tagEIMTransStat
{
	eInterFile_TaskWait = 1,		//任务等待
	eInterFile_TaskBegin,			//任务开始
	eInterFile_TaskTransing,		//任务传输中
	eInterFile_TaskEnd,				//任务完成，上传时KEY值从此状态返回
	eInterFile_TaskDeling,			//正在删除任务
	eInterFile_TaskDel,				//任务被删除
	eInterFile_NonTask,				//没有找到指定文件
	eInterFile_Failed,				//任务失败
	eInterFile_Verify_Failed,		//文件检验失败
}E_EIMTransStat, *PE_EIMTransStat;

//返回文件的状态信息(需要有一个能唯一确定文件的字段)
typedef struct tagEIMStatNotify   
{
	DWORD  dwTimestamp;						// 时间戳（GetTickCount()）
	UINT64 Qfid;							//唯一标识文件
	LONG64 i64TotalSize;					//文件大小
	LONG64 i64CompleteSize;					//传输完成大小
	E_EIMTransStat eTransStat;				//任务的传输状态
	TCHAR tszUploadKey[FILE_UPLOAD_KEY];		//文件上传后服务器生成KEY值
	TCHAR FileName[FILE_UPLOAD_KEY];          //文件名

}S_EIMStatNotify, *PS_EIMStatNotify; 

//要查询文件的查询条件,例如查询文件状态
typedef struct tagEIMQueryInfo				
{
	UINT64 Qfid;								//唯一标识文件
	E_EIMFileTransDirec eDirec;				//任务传输方向
}S_EIMQueryInfo, *PS_EIMQueryInfo;

#pragma pack( pop )

//回调函数，实时返回文件状态
//sStat：返回文件状态    pvUserData：将用户发送信息原样返回
typedef bool (__stdcall *PFN_StatNotify)(PS_EIMStatNotify psStat, void *pvUserData);


/////////////////////////////////////////////错误码//////////////////////////////////////////////

#define EIM_ERR_SUCCEED							EIMERR_NO_ERROR						//接口操作成功
#define EIM_ERR_INTERFACE_POINTER				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  1)		//获取接口指针失败
#define EIM_ERR_NEW_OPER						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  2)		//使用new操作失败
#define EIM_ERR_TRANS_DIRECTION					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  3)		//传输方向错误，传输方向只有上传下载
#define EIM_ERR_CALLBACK_POINT_NULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  4)		//回调函数指针为空
#define EIM_ERR_FILE_INFO						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  5)		//文件结构S_EIMFileInfo信息不完全
#define EIM_ERR_FILE_INFO_VECT_NULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  6)		//文件结构vector<S_EIMFileInfo>信息为空
#define EIM_ERR_TASK_TRANSING					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  7)    	//文件正在传输中，不允许修改优先级
#define EIM_ERR_NOT_FOUND_TASK					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  8)    	//传输队列和空闲队列没有找到任务
#define EIM_ERR_THREADPOOL_NON_INIT				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  9)		//eIMInterFile.dll的线程未初始化
#define EIM_ERR_RELEASE_SEMAPHORE				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 10)		//释放信号量失败

#define EIM_ERR_HTTPCONN_NULL					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 201)
#define EIM_ERR_INPUTSTREAM_NULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 202)
#define EIM_ERR_STATUS_INTERNAL					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 203)    	//-2
#define EIM_ERR_STATUS_NETWORK					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 204)    	//-6
#define EIM_ERR_STATUS_INVALID_PARAM			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 205)    	//-5
#define EIM_ERR_STATUS_WRITING					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 206)    	//-4
#define EIM_ERR_STATUS_READING					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 207)    	//-3
#define EIM_ERR_STOP_TRANSMITION				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 208)    	//停止传输
#define EIM_ERR_HTTP_UNAUTHORIZED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 209)		//401
#define EIM_ERR_HTTP_STATUS_OK					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 210)    	//200 // request completed
#define EIM_ERR_HTTP_RESUME_INCOMPLETE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 211)    	//308  //断点续传
#define EIM_ERR_INVALID_RETURN_VALUE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 212)    	//服务器无效的返回HEADING
#define EIM_ERR_EXCEPTION						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 213)    	//程序异常
#define EIM_ERR_PARAM_NULL						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 214)    	//参数为空
#define EIM_ERR_PARSE_JASON						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 215)      //解析JASON文件失败
#define EIM_ERR_TASK_EXIST						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 216)      //任务已经存在
#define EIM_ERR_V2T_FAILED						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 217)      //音频转文字失败
#define EIM_ERR_UPLOAD_FILE_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 218)      //上传文件失败
#define EIM_ERR_UPLOAD_IMAGE_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 219)      //上传图片失败
#define EIM_ERR_VERIFY_FAILED					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 220)      //文件校验失败

//#define EIM_ERR_INTERNET_FORCE_RETRY			EIM_ERR_SUCCEED+2009    //(INTERNET_ERROR_BASE + 32)
//#define EIM_ERR_HTTP_STATUS_PROXY_AUTH_REQ    EIM_ERR_SUCCEED+2013    //407 // proxy authentication required
//#define EIM_ERR_HTTP_STATUS_DENIED            EIM_ERR_SUCCEED+2014	//401 // access denied

#endif