// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#endif
#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include <vector>
#include <Shellapi.h>
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;

typedef std::vector<SStringT> VectImage;
typedef VectImage::iterator   VectImageIt;
