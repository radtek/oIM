#ifndef EIM_MSG_MGR_EXPORT_H
#define EIM_MSG_MGR_EXPORT_H
#pragma once

#include "Public/Typedefs.h"
#include <vector>
using namespace std;

enum DTYPE_EXPORT
{
	eIM_EXPORT_STRING    =  0,		//字符串
	eIM_EXPORT_MAPPATH   =  1,		//图片
	eIM_EXPORT_GIFPATH   =  2,		//图片
};

//按类型保存消息内容
typedef struct tagSingleMsgInfo
{
	tagSingleMsgInfo()
	{
		eType = eIM_EXPORT_STRING;
	}

	DTYPE_EXPORT eType;			// 导出类型
	eIMString    szMsgContent;	// 消息内容
}S_SingleMsgInfo, *PS_SingleMsgInfo;

typedef struct tagMsgMgrExportInfo
{
	tagMsgMgrExportInfo()
	{
		vecMsgInfo.clear();
		memset(&cfMsgHeader, 0, sizeof(CHARFORMAT2));
	}
	
	CHARFORMAT2				cfMsgHeader;	// 字体
	eIMString               szUserHead;		// 用户信息
	vector<S_SingleMsgInfo> vecMsgInfo;		// 消息内容
}S_MsgMgrExp, *PS_MsgMgrExp;


#endif /* _EIMUIDEF_ */