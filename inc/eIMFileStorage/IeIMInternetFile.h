
#ifndef __I_EIMINTERNET_FILE_2013_11_8
#define __I_EIMINTERNET_FILE_2013_11_8

#include "public/plugin.h"
#include "eIMFileStorage/FileStorageUtil.h"
#include <vector>
using namespace std;

extern "C" int __stdcall eIMStorageInterface(const TCHAR* pctszIID, void** ppvIObject);

typedef int (__stdcall* PFN_GetVoice2Text)(LPCTSTR pszText,  void* pvUserData);

#define EIMTASK_RETRANS_NUM_DEFAULT		0		//Ĭ���ش�����,Ĭ�ϲ��ش�
#define EIMTASK_NUM_DEFAULT				2		//Ĭ�ϲ�����

#define INAME_EIMSINGLE_INTER_FILE		_T("SXIT.EIMInternetFile.SINGLE")	// EIMInternetFile interface name Single File
#define INAME_EIMBATCH_INTER_FILE		_T("SXIT.EIMInternetFile.BATCH")	// EIMInternetFile interface name Batch File

typedef vector<S_EIMFileInfo> VectFileInfo;

//���ļ��ӿ���
class __declspec(novtable) I_eIMSingleInternetFile : public I_EIMUnknown
{
public:
	//�������ܣ���ʼ��,��������ã�����ʱ�ᰴĬ��ֵ��ʼ����ע��ֻ�ڵ�һ�ε���ʱ��Ч
	//i16ReTransNum���ش����� 
	//����ֵ:�Ƿ�ɹ�
	virtual bool Init(unsigned short i16ReTransNum = EIMTASK_RETRANS_NUM_DEFAULT) = 0;

	//�������ܣ��ļ����䣬֧���ϴ����أ�ע���ഴ����ʵ����ÿ�δ˺���ִ����ɺ�����ٴε��ô˺��� 
	//sFileInfo�������ļ���Ϣ 
	//����ֵΪ������
	virtual int TaskTrans(S_EIMFileInfo &sFileInfo) = 0;

	//�������ܣ�ʵʱ֪ͨ��ѯ���ص��������������ļ���Ϣ�жϻص�ʱ��ֵ
	//sFileInfo:pfn_stat�ص�����ָ��
	//����ֵ��������
	virtual int StatNotify(PFN_StatNotify pfn_stat, void *pvUserData) = 0;

	//�������ܣ�������ѯ�ӿ�
	//sFileInfo:Ҫ��ѯ�ļ����ļ���Ϣ��Ψһ��ʶһ���ļ�  sStat�������ļ���״̬��Ϣ
	//����ֵ��������
	virtual int GetFileStat(const S_EIMQueryInfo &sInfo, S_EIMStatNotify &sStat) = 0;

	//�������ܣ�ȡ��/ɾ���ļ����䣬����ļ����ڴ��䣬ͨ���ص���������ֹͣ״̬
	//sFileInfo:Ҫ��ѯ�ļ����ļ���Ϣ��Ψһ��ʶһ���ļ�
	//����ֵ��������
	virtual int DelTransTask(const S_EIMQueryInfo &sInfo) = 0;

	//�������ܣ��������ת��������
	//pFileInfo:Ҫ��ѯ�ļ����ļ���Ϣ��Ψһ��ʶһ���ļ�
	// pfnGetText: �ص�����
	// pUserData: �û�����
	//����ֵ��������
   virtual int AddTaskForV2T(S_EIMFileInfo* pFileInfo, PFN_GetVoice2Text pfnGetText, void *pUserData) = 0;
};



//�������ӿ���
class __declspec(novtable) I_eIMBatchInternetFile : public I_EIMUnknown
{
public:
	//�������ܣ���ʼ��,��������ã�����ʱ�ᰴĬ��ֵ��ʼ����ֻ�ڵ�һ�ε���ʱ��Ч
	//i16ReTransNum���ش�����  i16TaskNum�������� 
	//����ֵ:�Ƿ�ɹ�
	virtual bool Init(unsigned short i16ReTransNum = EIMTASK_RETRANS_NUM_DEFAULT, 
		unsigned short i16TaskNum = EIMTASK_NUM_DEFAULT) = 0;

	//�������ܣ�1���ļ���֧���첽���䣬2��֧���ϴ����� 3����ֻ���뵥�ļ�Ҳ�����봫����а����ȼ�˳���� 4���������ʱֻ���ٴε��ô˺���
	//vectFileInfo�������ļ����� vectRetInfo������ļ�ID���ļ���Ϣ
	//����ֵΪ������
	virtual int AddTransTask(VectFileInfo &vectInfo) = 0;

	//�������ܣ�ʵʱ֪ͨ��ѯ���ص�������
	//sFileInfo:pfn_stat�ص�����ָ��
	//����ֵ��������
	virtual int StatNotify(PFN_StatNotify pfn_stat, void *pvUserData) = 0;

	//�������ܣ�������ѯ�ӿ�
	//sFileInfo:Ҫ��ѯ�ļ����ļ���Ϣ��Ψһ��ʶһ���ļ�  sStat�������ļ���״̬��Ϣ
	//����ֵ��������
	virtual int GetFileStat(const S_EIMQueryInfo &sInfo, S_EIMStatNotify &sStat) = 0;

	//�������ܣ�ȡ��/ɾ���ļ�����
	//sFileInfo:Ҫ��ѯ�ļ����ļ���Ϣ��Ψһ��ʶһ���ļ�
	//����ֵ��������
	virtual int DelTransTask(const S_EIMQueryInfo &sInfo) = 0;

	//�������ܣ�ȡ��/ɾ����������
	//����ֵ��������
	virtual int DelAllTransTask() = 0;

	//�������ܣ�������������е��ļ����ȼ���ֻ��δ��ʼ������Ч
	//sInfo��������Ϣ  ePriority�����ȼ�
	//����ֵ��������
	virtual int SetTaskPriority(const S_EIMQueryInfo& sInfo, E_EIMTaskPriority ePriority) = 0;
};

#endif