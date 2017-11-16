// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define  _CRT_SECURE_NO_WARNINGS
#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include <vector>
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;

typedef std::vector<SStringT> VectImage;
typedef VectImage::iterator   VectImageIt;
typedef struct tagParams
{
	UINT64   u64Fid;
	SStringT szDbFile;
	VectImage vectImage;
	SStringT szImgFilter;
	SStringT szImgPath;
}S_Params, *PS_Params;
extern S_Params g_sParams;