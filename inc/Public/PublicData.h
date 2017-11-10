//提供公用数据，解决一些结构、枚举等不方便访问的情况    
/////////////////////////////////////////////////////////////////////////////

#ifndef PUBLIC_DATA_H
#define PUBLIC_DATA_H

#include "Utils.h"

typedef struct tagVoiceToText
{
	tagVoiceToText()
	{
		i64Fid   = 0;
	}
	
	UINT64     i64Fid;	// File Id
	eIMString szText;  //转换后文本
}S_VoiceToText, *PS_VoiceToText;

#endif

