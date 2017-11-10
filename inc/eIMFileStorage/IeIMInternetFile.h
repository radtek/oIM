
#ifndef __I_EIMINTERNET_FILE_2013_11_8
#define __I_EIMINTERNET_FILE_2013_11_8

#include "public/plugin.h"
#include "eIMFileStorage/FileStorageUtil.h"
#include <vector>
using namespace std;

extern "C" int __stdcall eIMStorageInterface(const TCHAR* pctszIID, void** ppvIObject);

typedef int (__stdcall* PFN_GetVoice2Text)(LPCTSTR pszText,  void* pvUserData);

#define EIMTASK_RETRANS_NUM_DEFAULT		0		//默认重传次数,默认不重传
#define EIMTASK_NUM_DEFAULT				2		//默认并发数

#define INAME_EIMSINGLE_INTER_FILE		_T("SXIT.EIMInternetFile.SINGLE")	// EIMInternetFile interface name Single File
#define INAME_EIMBATCH_INTER_FILE		_T("SXIT.EIMInternetFile.BATCH")	// EIMInternetFile interface name Batch File

typedef vector<S_EIMFileInfo> VectFileInfo;

//单文件接口类
class __declspec(novtable) I_eIMSingleInternetFile : public I_EIMUnknown
{
public:
	//函数功能：初始化,如果不设置，构造时会按默认值出始化，注意只在第一次调用时有效
	//i16ReTransNum：重传次数 
	//返回值:是否成功
	virtual bool Init(unsigned short i16ReTransNum = EIMTASK_RETRANS_NUM_DEFAULT) = 0;

	//函数功能：文件传输，支持上传下载，注意类创建类实例，每次此函数执行完成后才能再次调用此函数 
	//sFileInfo：传输文件信息 
	//返回值为错误码
	virtual int TaskTrans(S_EIMFileInfo &sFileInfo) = 0;

	//函数功能：实时通知查询（回调函数），根据文件信息判断回调时的值
	//sFileInfo:pfn_stat回调函数指针
	//返回值：错误码
	virtual int StatNotify(PFN_StatNotify pfn_stat, void *pvUserData) = 0;

	//函数功能：主动查询接口
	//sFileInfo:要查询文件的文件信息，唯一标识一个文件  sStat：返回文件的状态信息
	//返回值：错误码
	virtual int GetFileStat(const S_EIMQueryInfo &sInfo, S_EIMStatNotify &sStat) = 0;

	//函数功能：取消/删除文件传输，如果文件正在传输，通过回调函数返回停止状态
	//sFileInfo:要查询文件的文件信息，唯一标识一个文件
	//返回值：错误码
	virtual int DelTransTask(const S_EIMQueryInfo &sInfo) = 0;

	//函数功能：添加语音转文字任务
	//pFileInfo:要查询文件的文件信息，唯一标识一个文件
	// pfnGetText: 回调函数
	// pUserData: 用户数据
	//返回值：错误码
   virtual int AddTaskForV2T(S_EIMFileInfo* pFileInfo, PFN_GetVoice2Text pfnGetText, void *pUserData) = 0;
};



//批量件接口类
class __declspec(novtable) I_eIMBatchInternetFile : public I_EIMUnknown
{
public:
	//函数功能：初始化,如果不设置，构造时会按默认值出始化，只在第一次调用时有效
	//i16ReTransNum：重传次数  i16TaskNum：并发数 
	//返回值:是否成功
	virtual bool Init(unsigned short i16ReTransNum = EIMTASK_RETRANS_NUM_DEFAULT, 
		unsigned short i16TaskNum = EIMTASK_NUM_DEFAULT) = 0;

	//函数功能：1、文件仅支持异步传输，2、支持上传下载 3、如只加入单文件也将放入传输队列按优先级顺序传输 4、添加任务时只需再次调用此函数
	//vectFileInfo：传输文件集合 vectRetInfo：添加文件ID的文件信息
	//返回值为错误码
	virtual int AddTransTask(VectFileInfo &vectInfo) = 0;

	//函数功能：实时通知查询（回调函数）
	//sFileInfo:pfn_stat回调函数指针
	//返回值：错误码
	virtual int StatNotify(PFN_StatNotify pfn_stat, void *pvUserData) = 0;

	//函数功能：主动查询接口
	//sFileInfo:要查询文件的文件信息，唯一标识一个文件  sStat：返回文件的状态信息
	//返回值：错误码
	virtual int GetFileStat(const S_EIMQueryInfo &sInfo, S_EIMStatNotify &sStat) = 0;

	//函数功能：取消/删除文件传输
	//sFileInfo:要查询文件的文件信息，唯一标识一个文件
	//返回值：错误码
	virtual int DelTransTask(const S_EIMQueryInfo &sInfo) = 0;

	//函数功能：取消/删除所有任务
	//返回值：错误码
	virtual int DelAllTransTask() = 0;

	//函数功能：设置任务队列中的文件优先级，只对未开始任务生效
	//sInfo：任务信息  ePriority：优先级
	//返回值：错误码
	virtual int SetTaskPriority(const S_EIMQueryInfo& sInfo, E_EIMTaskPriority ePriority) = 0;
};

#endif