// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: Utils.h header file, used for eIM projects to help simplify
//   the work, it included all sub helper function or class.
//  
//  Plugin.h		- Plugin helper, defined the Plugin interface and helper class to use the plugin
//  Debugs.h		- Debug helper Macro or function
//  Errors.h		- Error code defined
//  StrUtil.h		- String helper function
//  PathUtils.h		- Path helper function
//  ThreadUtil.h	- Thread helper function
//  Configer.h		- INI configuration helper class
//  XmlConfiger.h	- XML configuration helper dlass
// 
// Auth: LongWQ
// Date: 16:06 17/2/24
// 
// History: 
//    16:06 17/2/24
//    
//    
/////////////////////////////////////////////////////////////////////////////


#ifndef __EIM_CRYPTO_API_2017_2_24_BY_YFGZ__
#define __EIM_CRYPTO_API_2017_2_24_BY_YFGZ__

#include "Utils.h"
#include "base64\base64.h"
#include "openssl\ssl.h"
#include "openssl\aes.h"

#if 0
# include "vld\vld.h"
#endif

BOOL AesCbcEncrypt(const char* pszIn, const char* pszKey, eIMString& szOut, const char* pszIv="16-Bytes--String");

#include "Crypto.inl"

#endif	// __EIM_CRYPTO_API_2017_2_24_BY_YFGZ__

