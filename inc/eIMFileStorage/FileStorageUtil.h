#ifndef __FILE_STORAGE_UTIL
#define __FILE_STORAGE_UTIL

#include "Public\Errors.h"

#define LOCAL_PATH_LEN					512		//�����ļ�·��
#define SERVER_URL_LEN					512		//������URL
#define USER_ID_LEN						64		//�û�ID����
#define FILE_NAME_LEN					256		//�ļ�������
#define FILE_UPLOAD_KEY					128		//�ϴ��ļ�����KEYֵ
#define HTTP_ERR_MSG_LEN				1024    //��ʽ��WININET������Ϣ�ĳ���
#define SHORT_KEY_LEN					8		//�ļ�������֤������ShortKey�ĳ��ȣ�ֻ�����˵�1��8���ַ����ɵĽ����

//ʹ�õ�¼Ӧ��Э�����ֶΣ�UINT16 wPCMaxSendFileSize; // PC�ļ�����ʹ�С����λΪM��������2�ֽ�
// #define FILE_TRANS_MODE_NON_RESUME      50
// #define FILE_TRANS_MODE_RESUME		 51

#pragma pack( push, 8 )

//�ļ��������ȼ�������˳������䶯
typedef enum tagEIMTaskPriority  
{
	eInterFile_High = 1,						//������ȼ�
	eInterFile_AboveNormal,						//�е����ȼ�
	eInterFile_Normal,							//Ĭ��
}E_EIMTaskPriority, *PE_EIMTaskPriority;

//�ļ����䷽��
typedef enum tagEIMFileTransDirec
{
	eInterFile_Upload = 1,					//�ϴ�
	eInterFile_Download,					//����
}E_EIMFileTransDirec, *PE_EIMFileTransDirec;

typedef enum tagEIMFileType
{
	eInterFile_File,						//��ͨ�ļ�
	eInterFile_Image,						//ͼƬ
	eInterFile_HeadImg,						//ͷ��
}E_EIMFileType, *PE_EIMFileType;

//URL�Ӵ��Ƿ����
typedef enum tagEIMEncrpt
{
	eInterFile_NoEncrpt,
	eInterFile_Encrpt,
}E_EIMEncrpt;

//���䷽ʽ
typedef enum tagEIMTransWay
{
	eInterFile_NonResuming=0,    //��ͨ��ʽ
	eInterFile_Resuming=1,  //�ϵ�����
}E_EIMTransWay;


//�����ļ���Ϣ����Ҫ�����¾�����Ҫ��Щ��Ϣ
typedef struct tagEIMFileInfo 
{
	UINT64 Qfid;								//Ψһ��ʶ�ļ����ϲ����ɷ��ظ��ײ�
	TCHAR tszFileName[FILE_NAME_LEN];			//�ļ��� �ϴ�����ʱʹ��
	TCHAR tszDownServerUrl[SERVER_URL_LEN];     //���ط�������ַ,��ַΪȫ·��  �ϵ㷽ʽʱ�洢�ϵ��ļ�URL����ֻ�������ļ���URLԭ���洢������Ҫkey=%s
	TCHAR tszUpServerUrl[SERVER_URL_LEN];		//�ϴ���������ַ  �ϵ㷽ʽʱ�洢�ϵ��ļ�URL
	TCHAR tszLocalPath[LOCAL_PATH_LEN];			//����·��,��ַ��Ϊ·�������������ļ�������β������"\"�����磺C:\aa 
	E_EIMTaskPriority	ePriority;				//���ȼ�
	E_EIMFileTransDirec eDirec;					//���䷽��
	E_EIMFileType eFileType;					//��ͨ�ļ���ͷ��
	E_EIMEncrpt   eEncrpt;						//�Ƿ���ļ���ͼƬ�����ļ����м��ܴ���

	TCHAR tszFileKey[FILE_UPLOAD_KEY];			//���ϵ�����ʹ�ã�����˷��ص�KEY������ʱʹ�ã���KEY�Ѳ�����6λ��40λ֮��
	TCHAR tszUpTokenUrl[SERVER_URL_LEN];		//���ϵ�����ʹ�ã��ϴ�ʱ�������ļ���TOKEN��URL
	E_EIMTransWay eTransWay;					//�Ƿ���ļ���ͼƬ���жϵ�������ʽ����
}S_EIMFileInfo, *PS_EIMFileInfo;

//���䷽ʽͬ�����첽
typedef enum tagEIMTransType  
{
	eInterFile_Sync = 1,            //ͬ��
	eInterFile_Async,               //�첽
}E_EIMTransType, *PE_EIMTransType;

//����Ĵ���״̬
typedef enum tagEIMTransStat
{
	eInterFile_TaskWait = 1,		//����ȴ�
	eInterFile_TaskBegin,			//����ʼ
	eInterFile_TaskTransing,		//��������
	eInterFile_TaskEnd,				//������ɣ��ϴ�ʱKEYֵ�Ӵ�״̬����
	eInterFile_TaskDeling,			//����ɾ������
	eInterFile_TaskDel,				//����ɾ��
	eInterFile_NonTask,				//û���ҵ�ָ���ļ�
	eInterFile_Failed,				//����ʧ��
	eInterFile_Verify_Failed,		//�ļ�����ʧ��
}E_EIMTransStat, *PE_EIMTransStat;

//�����ļ���״̬��Ϣ(��Ҫ��һ����Ψһȷ���ļ����ֶ�)
typedef struct tagEIMStatNotify   
{
	DWORD  dwTimestamp;						// ʱ�����GetTickCount()��
	UINT64 Qfid;							//Ψһ��ʶ�ļ�
	LONG64 i64TotalSize;					//�ļ���С
	LONG64 i64CompleteSize;					//������ɴ�С
	E_EIMTransStat eTransStat;				//����Ĵ���״̬
	TCHAR tszUploadKey[FILE_UPLOAD_KEY];		//�ļ��ϴ������������KEYֵ
	TCHAR FileName[FILE_UPLOAD_KEY];          //�ļ���

}S_EIMStatNotify, *PS_EIMStatNotify; 

//Ҫ��ѯ�ļ��Ĳ�ѯ����,�����ѯ�ļ�״̬
typedef struct tagEIMQueryInfo				
{
	UINT64 Qfid;								//Ψһ��ʶ�ļ�
	E_EIMFileTransDirec eDirec;				//�����䷽��
}S_EIMQueryInfo, *PS_EIMQueryInfo;

#pragma pack( pop )

//�ص�������ʵʱ�����ļ�״̬
//sStat�������ļ�״̬    pvUserData�����û�������Ϣԭ������
typedef bool (__stdcall *PFN_StatNotify)(PS_EIMStatNotify psStat, void *pvUserData);


/////////////////////////////////////////////������//////////////////////////////////////////////

#define EIM_ERR_SUCCEED							EIMERR_NO_ERROR						//�ӿڲ����ɹ�
#define EIM_ERR_INTERFACE_POINTER				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  1)		//��ȡ�ӿ�ָ��ʧ��
#define EIM_ERR_NEW_OPER						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  2)		//ʹ��new����ʧ��
#define EIM_ERR_TRANS_DIRECTION					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  3)		//���䷽����󣬴��䷽��ֻ���ϴ�����
#define EIM_ERR_CALLBACK_POINT_NULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  4)		//�ص�����ָ��Ϊ��
#define EIM_ERR_FILE_INFO						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  5)		//�ļ��ṹS_EIMFileInfo��Ϣ����ȫ
#define EIM_ERR_FILE_INFO_VECT_NULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  6)		//�ļ��ṹvector<S_EIMFileInfo>��ϢΪ��
#define EIM_ERR_TASK_TRANSING					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  7)    	//�ļ����ڴ����У��������޸����ȼ�
#define EIM_ERR_NOT_FOUND_TASK					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  8)    	//������кͿ��ж���û���ҵ�����
#define EIM_ERR_THREADPOOL_NON_INIT				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE,  9)		//eIMInterFile.dll���߳�δ��ʼ��
#define EIM_ERR_RELEASE_SEMAPHORE				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 10)		//�ͷ��ź���ʧ��

#define EIM_ERR_HTTPCONN_NULL					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 201)
#define EIM_ERR_INPUTSTREAM_NULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 202)
#define EIM_ERR_STATUS_INTERNAL					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 203)    	//-2
#define EIM_ERR_STATUS_NETWORK					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 204)    	//-6
#define EIM_ERR_STATUS_INVALID_PARAM			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 205)    	//-5
#define EIM_ERR_STATUS_WRITING					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 206)    	//-4
#define EIM_ERR_STATUS_READING					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 207)    	//-3
#define EIM_ERR_STOP_TRANSMITION				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 208)    	//ֹͣ����
#define EIM_ERR_HTTP_UNAUTHORIZED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 209)		//401
#define EIM_ERR_HTTP_STATUS_OK					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 210)    	//200 // request completed
#define EIM_ERR_HTTP_RESUME_INCOMPLETE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 211)    	//308  //�ϵ�����
#define EIM_ERR_INVALID_RETURN_VALUE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 212)    	//��������Ч�ķ���HEADING
#define EIM_ERR_EXCEPTION						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 213)    	//�����쳣
#define EIM_ERR_PARAM_NULL						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 214)    	//����Ϊ��
#define EIM_ERR_PARSE_JASON						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 215)      //����JASON�ļ�ʧ��
#define EIM_ERR_TASK_EXIST						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 216)      //�����Ѿ�����
#define EIM_ERR_V2T_FAILED						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 217)      //��Ƶת����ʧ��
#define EIM_ERR_UPLOAD_FILE_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 218)      //�ϴ��ļ�ʧ��
#define EIM_ERR_UPLOAD_IMAGE_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 219)      //�ϴ�ͼƬʧ��
#define EIM_ERR_VERIFY_FAILED					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_IFILE, 220)      //�ļ�У��ʧ��

//#define EIM_ERR_INTERNET_FORCE_RETRY			EIM_ERR_SUCCEED+2009    //(INTERNET_ERROR_BASE + 32)
//#define EIM_ERR_HTTP_STATUS_PROXY_AUTH_REQ    EIM_ERR_SUCCEED+2013    //407 // proxy authentication required
//#define EIM_ERR_HTTP_STATUS_DENIED            EIM_ERR_SUCCEED+2014	//401 // access denied

#endif