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
	eUPDATER_EN = 1, //Ӣ�ģ�Ĭ�ϣ�
	eUPDATER_CN,		//����
}E_EIMUdpLanguage, *PE_EIMUdpLanguage;

typedef enum tagEIMUpdaterType
{
	eUPDATER_SETUP = 1,		    //����װ
	eUPDATER_DB,				//DB
	eUPDATER_STRUCT,			//��ǰ�ṹ
	eUPDATER_SILENT,			//��Ĭ����
	eUPDATER_EXPORTIM,			//����ɰ汾������
}E_EIMUpdaterType, *PE_EIMUpdaterType;

//��װ���°�
typedef struct tagEIMUdpSetup
{
	//TCHAR szUpdatePath[UPD_LOCAL_PATH_LEN];		//���³�����Ŀ¼,DUilib.dll eIMInterFile.dll��ҲҪ�����ڴ�Ŀ¼��
	//TCHAR szUpdateDupPath[UPD_LOCAL_PATH_LEN];	//���³��򸱱�Ŀ¼
	//TCHAR szOldVersion[VERSION_INFO_LEN];			   //�ɰ汾��
	//TCHAR szNewVersion[VERSION_INFO_LEN];			   //�°汾��
	//TCHAR szDownFileName[UPD_FILE_NAME_LEN];		//�����ļ����ļ���(������ڱ���Ŀ¼�е��ļ���)
	
	//TCHAR szDownLocalPath[UPD_LOCAL_PATH_LEN];	//�����ļ����ش洢·����Ŀ¼
	TCHAR szDownloadUrl[UPD_DOWN_URL_LEN];			//����·�� ȫ·��
	TCHAR szMainProgramName[UPD_FILE_NAME_LEN];		//�������ڽ����е����֣�����ǿ�йرս���
	TCHAR szVerDesc[UPD_DOWN_URL_LEN];				//�汾����URL
}S_EIMUdpSetup, *PS_EIMUdpSetup;

//Ŀ¼�ṹ���
typedef struct tagEIMUdpDirStruct
{
	int i32Ver;										//��ǰ�汾��
	TCHAR szXmlPath[UPD_LOCAL_PATH_LEN];			//XML�ļ�ȫ·��
}S_EIMUdpDirStruct, *PS_EIMUdpDirStruct;

//DB�ṹ���
typedef struct tagEIMUdpDbStruct
{
	char  szPsw[16];								// Password
	BYTE  u8PswLen;									// Password length
	DWORD dwFlag;
	int i32DbVer;									//DB�汾��
	TCHAR szXmlPath[UPD_LOCAL_PATH_LEN];			//XML�ļ�ȫ·��
}S_EIMUdpDbStruct, *PS_EIMUdpDbStruct;

typedef enum tagEIMSilentType
{
	eEIM_DOWNLOAD = 1,								// Download
	eEIM_INSTALL,									// Install
}E_EIMSilentType;

typedef struct tagEIMUpdSilent
{
	E_EIMSilentType eType;
	TCHAR tszDownFileName[UPD_FILE_NAME_LEN];		//�����ļ����ļ���(������ڱ���Ŀ¼�е��ļ���)
	TCHAR tszDownloadUrl[UPD_DOWN_URL_LEN];			//����·�� ȫ·��
	TCHAR tszDownLocalPath[UPD_LOCAL_PATH_LEN];		//�����ļ����ش洢·����Ŀ¼

	TCHAR tszInstallFilePath[UPD_LOCAL_PATH_LEN];	//��װ�ļ�ȫ·�� eEIM_INSTALLʱ������
}S_EIMUpdSilent, *PS_EIMUpdSilent;

typedef struct tagEIMUpdaterInfo
{
	E_EIMUdpLanguage eLanguage;						//��������
	E_EIMUpdaterType eUpdaterType;					//��������
	//void			 *pvUpdateStruct;				//����ĸ��½ṹ������updater_Setup����ΪS_EIMUdpSetUp�ṹ
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