#ifndef EIM_UPD_DATA_H
#define EIM_UPD_DATA_H

#include <tchar.h>
#include "Public/Plugin.h"
#include "Public/StrUtil.h"

#define UPD_FILE_NAME_LEN		256
#define UPD_DOWN_URL_LEN		1024
#define UPD_LOCAL_PATH_LEN		1024
#define VERSION_INFO_LEN	    64

#define UPDATETIPDOWNLOAD      _T("download")
#define PCUPDATENOTIEPATH      _T("pcupdatenotie.txt")


#pragma pack( push, 8 )

typedef enum tagEIMUdpLanguage
{
	eUPDATER_EN = 1, //英文（默认）
	eUPDATER_CN,		//中文
}E_EIMUdpLanguage, *PE_EIMUdpLanguage;

typedef enum tagEIMUpdaterType
{
	eUPDATER_SETUP = 1,		    //程序安装
	eUPDATER_DB,				//DB
	eUPDATER_STRUCT,			//当前结构
	eUPDATER_SILENT,			//静默升级
	eUPDATER_EXPORTIM,			//导入旧版本中数据
}E_EIMUpdaterType, *PE_EIMUpdaterType;

//安装更新包
typedef struct tagEIMUdpSetup
{
	//TCHAR szUpdatePath[UPD_LOCAL_PATH_LEN];		//更新程序存放目录,DUilib.dll eIMInterFile.dll，也要保存在此目录中
	//TCHAR szUpdateDupPath[UPD_LOCAL_PATH_LEN];	//更新程序副本目录
	//TCHAR szOldVersion[VERSION_INFO_LEN];			   //旧版本号
	//TCHAR szNewVersion[VERSION_INFO_LEN];			   //新版本号
	//TCHAR szDownFileName[UPD_FILE_NAME_LEN];		//下载文件的文件名(即存放在本地目录中的文件名)
	
	//TCHAR szDownLocalPath[UPD_LOCAL_PATH_LEN];	//下载文件本地存储路径，目录
	TCHAR szDownloadUrl[UPD_DOWN_URL_LEN];			//下载路径 全路径
	TCHAR szMainProgramName[UPD_FILE_NAME_LEN];		//主程序在进程中的名字，用于强行关闭进程
	TCHAR szVerDesc[UPD_DOWN_URL_LEN];				//版本描述URL
}S_EIMUdpSetup, *PS_EIMUdpSetup;

//目录结构变更
typedef struct tagEIMUdpDirStruct
{
	int i32Ver;										//当前版本号
	TCHAR szXmlPath[UPD_LOCAL_PATH_LEN];			//XML文件全路径
}S_EIMUdpDirStruct, *PS_EIMUdpDirStruct;

//DB结构变更
typedef struct tagEIMUdpDbStruct
{
	char  szPsw[16];								// Password
	BYTE  u8PswLen;									// Password length
	DWORD dwFlag;
	int i32DbVer;									//DB版本号
	TCHAR szXmlPath[UPD_LOCAL_PATH_LEN];			//XML文件全路径
}S_EIMUdpDbStruct, *PS_EIMUdpDbStruct;

typedef enum tagEIMSilentType
{
	eEIM_DOWNLOAD = 1,								// Download
	eEIM_INSTALL,									// Install
}E_EIMSilentType;

typedef struct tagEIMUpdSilent
{
	E_EIMSilentType eType;
	TCHAR tszDownFileName[UPD_FILE_NAME_LEN];		//下载文件的文件名(即存放在本地目录中的文件名)
	TCHAR tszDownloadUrl[UPD_DOWN_URL_LEN];			//下载路径 全路径
	TCHAR tszDownLocalPath[UPD_LOCAL_PATH_LEN];		//下载文件本地存储路径，目录

	TCHAR tszInstallFilePath[UPD_LOCAL_PATH_LEN];	//安装文件全路径 eEIM_INSTALL时起作用
}S_EIMUpdSilent, *PS_EIMUpdSilent;

typedef struct tagEIMUpdaterInfo
{
	E_EIMUdpLanguage eLanguage;						//语言类型
	E_EIMUpdaterType eUpdaterType;					//更新类型
	//void			 *pvUpdateStruct;				//具体的更新结构，例如updater_Setup类型为S_EIMUdpSetUp结构
	union
	{
		S_EIMUdpSetup      seIMUdpSetup;
		S_EIMUdpDirStruct  seIMUdpStruct;
		S_EIMUdpDbStruct   seIMUdpDb;
		S_EIMUpdSilent     seIMSilent;

	}/*UN_EIMUdpInfo*/;
}S_EIMUpdaterInfo, *PS_EIMUpdaterInfo;



#pragma pack(pop)

#endif