#pragma once
#include "public\plugin.h"

#define INAME_EIMSCREENCAPTURE		_T("SXIT.EIMScreenCapture")	// eIMScreenCapture interface name

class I_EIMScreenCapture :
	public I_EIMUnknown
{
public:
	/************************************************************************/
	/* 功能 截屏模块启动                                                     */
	/* pctszPath  ： 图片保存路径                                            */
	/* hParent ： 父对话框句柄                                               */
	/************************************************************************/
	virtual DWORD ScreenCaptrue(const TCHAR *pctszPath,HWND hParent = NULL) = 0; 
};


