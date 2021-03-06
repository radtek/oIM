// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define	 DLL_SOUI
#include "souistd.h.cpp"
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include <helper\slog.h>
#include "trayicon/SShellNotifyIcon.h"
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "..\Resource\resource.h"
using namespace SOUI;

#define LOG_OIM		"OIM"
#include "public\XmlConfig.h"
#include "C_UICore.h"

#define core C_UICore::GetObject()