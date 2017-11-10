#pragma once
#include "public\plugin.h"

#define INAME_EIMSCREENCAPTURE		_T("SXIT.EIMScreenCapture")	// eIMScreenCapture interface name

class I_EIMScreenCapture :
	public I_EIMUnknown
{
public:
	/************************************************************************/
	/* ���� ����ģ������                                                     */
	/* pctszPath  �� ͼƬ����·��                                            */
	/* hParent �� ���Ի�����                                               */
	/************************************************************************/
	virtual DWORD ScreenCaptrue(const TCHAR *pctszPath,HWND hParent = NULL) = 0; 
};


