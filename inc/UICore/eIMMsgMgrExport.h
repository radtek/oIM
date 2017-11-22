#ifndef EIM_MSG_MGR_EXPORT_H
#define EIM_MSG_MGR_EXPORT_H
#pragma once

#include "Public/Typedefs.h"
#include <vector>
using namespace std;

enum DTYPE_EXPORT
{
	eIM_EXPORT_STRING    =  0,		//�ַ���
	eIM_EXPORT_MAPPATH   =  1,		//ͼƬ
	eIM_EXPORT_GIFPATH   =  2,		//ͼƬ
};

//�����ͱ�����Ϣ����
typedef struct tagSingleMsgInfo
{
	tagSingleMsgInfo()
	{
		eType = eIM_EXPORT_STRING;
	}

	DTYPE_EXPORT eType;			// ��������
	eIMString    szMsgContent;	// ��Ϣ����
}S_SingleMsgInfo, *PS_SingleMsgInfo;

typedef struct tagMsgMgrExportInfo
{
	tagMsgMgrExportInfo()
	{
		vecMsgInfo.clear();
		memset(&cfMsgHeader, 0, sizeof(CHARFORMAT2));
	}
	
	CHARFORMAT2				cfMsgHeader;	// ����
	eIMString               szUserHead;		// �û���Ϣ
	vector<S_SingleMsgInfo> vecMsgInfo;		// ��Ϣ����
}S_MsgMgrExp, *PS_MsgMgrExp;


#endif /* _EIMUIDEF_ */