//�ṩ�������ݣ����һЩ�ṹ��ö�ٵȲ�������ʵ����    
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
	eIMString szText;  //ת�����ı�
}S_VoiceToText, *PS_VoiceToText;

#endif

