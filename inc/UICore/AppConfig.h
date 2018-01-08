// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: eIMUI config file const defined 
// 
// Auth: yfgz
// Date: 
// 
// History: 
//    2014/6/26 yfgz Finished first version
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __APP_CONFIG_HEADER_2017_12_28_YFGZ__
#define __APP_CONFIG_HEADER_2017_12_28_YFGZ__

#include <windows.h>
#include <tchar.h>

#define KB  ( 1024 )	// 1KB
#define MB	( KB * KB )	// 1MB

// Config files define
#define PLUGIN_CONFIG_FILE		_T("Plugins.config")		// Plugin config file
#define PROTOCOL_CONFIG_FILE	_T("Protocol.config")		// Protocol config file
#define APP_CONFIG_FILE			_T("App.config")			// Application config file
#define USER_CONFIG_FILE		_T("User.config")			// Application for user config
#define FONT_CONFIG_FILE        _T("Font.config")           // Font config file 
#define UPDATE_CONFIG_FILE		_T("Update.config")			// Updater config file
#define CONTACTS_CONFIG_FILE	_T("Group.config")			// Contact config file

//App.Config///////////////////////////////////////////////////////////////////
// EL: Element
#define APP_ROOT_EL                         _T("oIM")
#define APP_ROOT_ATTR_VERSION               _T("Version")

// App
#define APP_EL                              _T("App")
#define APP_ATTR_LANGUAGE                   _T("Language")
#define APP_ATTR_COMPANY					_T("Company")	// Company english name
#define APP_ATTR_VERSION                    _T("Version")
#define APP_ATTR_MULTI_INST					_T("MultiInst")	// 支持多开进程，同时，登录窗口中就可以选择登录账号
#define APP_ATTR_DOWNLOADURL                _T("DownloadUrl")
#define APP_ATTR_FEEDBACKURL                _T("FeedbackUrl")
#define APP_ATTR_CLEAR_HISTORY_LOG_DAYS		_T("ClearHistoryLogDays")
#define APP_ATTR_CLEAR_DOWNLOAD_CONTS_DAYS	_T("ClearDownloadContsDays")
#define APP_ATTR_CLEAR_BKDB_DAYS			_T("ClearBkDBDays")
# define CLEAR_FILE_DAYS_DEFAULT			(31)
#define APP_ATTR_DISK_SPACE_PROMPT			_T("DiskSpacePrompt")
# define APP_ATTR_DISK_SPACE_PROMPT_DEFAULT	( 1024 )		// 1GB

#define APP_ATTR_UI_LOG_LEVEL				_T("UILogLevel")
#define APP_ATTR_UI_LOG_FLAG				_T("UILogFlag")

// Skin
#define SKIN_EL								_T("Skin")
#define SKIN_ATTR_TYPE						_T("Type")
# define SKIN_TYPE_FILE						( 0 )	// 从文件中加载资源
# define SKIN_TYPE_RES						( 1 )	// 从EXE资源中加载资源
# define SKIN_TYPE_ZIP						( 2 )	// 从ZIP中加载资源
#define SKIN_ATTR_PATH						_T("Path")

// Optional Funcion
#define FUNC_EL								_T("Function") 

// Engine
#define ENGINE_EL                           _T("Engine")
#define ENGINE_ATTR_LOGLEVEL                _T("LogLevel")


// Hotkeys /////////////////////////////////////////////////////////
#define SETTING_HOTKEY_EL					_T("Hotkey")
#define SETTING_HOTKEY_ATTR_REG_PROMPT		_T("RegHotKeyPrompt")

#define SETTING_HOTKEY_EXTRACT_MSG_EL		_T("ExtractMsg")
#define SETTING_HOTKEY_SCREENSHOT_EL		_T("ScreenShot")
#define SETTING_HOTKEY_ACTIVE_SEARCH_EL		_T("ActiveSearch")

#define SETTING_HOTKEY_ATTR_TYPE			_T("Type")
#define SETTING_HOTKEY_ATTR_DEF_MODIFI		_T("Modifiers")
#define SETTING_HOTKEY_ATTR_DEF_VKCODE		_T("VKCode")
#define SETTING_HOTKEY_ATTR_CUSTOM_MODIFI	_T("CustomModifiers")
#define SETTING_HOTKEY_ATTR_CUSTOM_VKCODE	_T("CustomVKCode")
#define SETTING_HOTKEY_ATTR_HOTKEY			_T("Hotkey")	// Combined by Ctrl,Alt,Shift and [0-9A-Z]; SendMsg only[Ctrl+Enter,Enter]

#define SETTING_HOTKEY_SENDMSG_EL			_T("SendMsg")
# define HOTKEY_CONTROL_ENTER				( 1 )	// Control+Enter
# define HOTKEY_ENTER						( 2 )	// Enter
#define SETTING_HOTKEY_ATTR_STATE			_T("State")		// -1: is conflict; 0: is disable; 1: default is ok
# define HOTKEY_CONFLICT					( -1 )	// conflict
# define HOTKEY_DISABLE						( 0 )	// disable
# define HOTKEY_OK							( 1 )	// ok

//Font.config/////////////////////////////////////////////////////////////////////////
#define FONT_DEFAULT_FAMILY                 _T("宋体")   // Default font

#define FONT_ROOT_EL                        _T("eIMFont")
#define FONT_VERSION_ATTR                   _T("Version")

#define FONT_ITEM_EL                        _T("Item")
#define FONT_ITEM_ID_ATTR                   _T("Id")
#define FONT_ITEM_NAME_ATTR                 _T("Name")

#define PATH_FONT_ITEM_FMT	FONT_ROOT_EL _T("\\") FONT_ITEM_EL	_T("[%d]")	// eIMFont/Item[d]



// HELP MACRO for =============================================================
// 说明：当路径所在位置只有一项时，使用　"Path" 与 "Path[0]" 是一样的。
// _FMT: 是格式字符串，%d,
// _(N):   的N只能是常量数值，不能给变量。
// App.config
#define PATH_APP						APP_ROOT_EL  _T("\\") APP_EL				// oIM\App
#define PATH_SKIN						APP_ROOT_EL  _T("\\") SKIN_EL				// oIM\Skin
#define PATH_SPECIALID					APP_ROOT_EL  _T("\\") SPECIALID_EL			// oIM\SpecialId
#define PATH_FUNC						APP_ROOT_EL  _T("\\") FUNC_EL				// oIM\Function
#define PATH_FUNC_ITEM_(N)				APP_ROOT_EL  _T("\\") FUNC_EL _T("\\")##N	// oIM\Function
#define PATH_ENGINE						APP_ROOT_EL	 _T("\\") ENGINE_EL				// oIM\Engine
#define PATH_SETTING					APP_ROOT_EL  _T("\\") SETTING_EL			// oIM\Setting
#define PATH_FONT_ITEM_(N)              APP_ROOT_EL	 _T("\\") FONTS_EL _T("\\")##N	// oIM\App\Fonts\Font[d]
#define PATH_SETTING_MY					PATH_SETTING _T("\\") SETTING_MY_EL			// oIM\Setting\My
#define PATH_SETTING_BASIC				PATH_SETTING _T("\\") SETTING_BASIC_EL		// oIM\Setting\Basic
#define PATH_SETTING_TRANSFER			PATH_SETTING _T("\\") SETTING_TRANSFER_EL	// oIM\Setting\Transfer
#define PATH_SETTING_RANKIDLEVEL		PATH_SETTING _T("\\") RANKID_LEVEL 

#define PATH_SETTING_HOTKEY				PATH_SETTING _T("\\") SETTING_HOTKEY_EL		// oIM\Setting\Hotkey
#define PATH_SETTING_HOTKEY_ITEM_(N)	PATH_SETTING_HOTKEY _T("\\")##N				// oIM\Setting\Hotkey\SendMsg

#define PATH_SETTING_RESP				PATH_SETTING _T("\\") SETTING_RESPONSE_EL									// oIM\Setting\Response
#define PATH_SETTING_RESP_ITEM_FMT		PATH_SETTING_RESP _T("\\") SETTING_RESP_ITEM_EL	_T("[%d]")					// oIM\Setting\Response\Item[d]

#define PATH_SETTING_SERVER				PATH_SETTING _T("\\") SETTING_SERVER_EL										// oIM\Setting\Server
#define PATH_SETTING_SERVER_IM_(N)		PATH_SETTING_SERVER _T("\\") SETTING_IM_EL _T("[") _T(#N) _T("]")			// oIM\Setting\Server\IM[n]
#define PATH_SETTING_SERVER_P2P_(N)		PATH_SETTING_SERVER _T("\\") SETTING_P2P_EL _T("[") _T(#N) _T("]")			// oIM\Setting\Server\P2P[n]
#define PATH_SETTING_SERVER_RDP_(N)		PATH_SETTING_SERVER _T("\\") SETTING_RDP_EL _T("[") _T(#N) _T("]")			// oIM\Setting\Server\RDP[n]
#define PATH_SETTING_SERVER_FILE_(N)	PATH_SETTING_SERVER _T("\\") SETTING_FILE_EL _T("[") _T(#N) _T("]")			// oIM\Setting\Server\File[n]
#define PATH_SETTING_SERVER_IMAGE_(N)	PATH_SETTING_SERVER _T("\\") SETTING_IMAGE_EL _T("[") _T(#N) _T("]")		// oIM\Setting\Server\Image[n]
#define PATH_SETTING_SERVER_HEAD_(N)	PATH_SETTING_SERVER _T("\\") SETTING_HEAD_EL _T("[") _T(#N) _T("]")			// oIM\Setting\Server\Head[n]

#define PATH_SETTING_SERVER_FILE_BT(N)	 PATH_SETTING_SERVER _T("\\") SETTING_FILE_EL_BT _T("[") _T(#N) _T("]")		// oIM\Setting\Server\File_BT[n]
#define PATH_SETTING_SERVER_IMAGE_BT_(N) PATH_SETTING_SERVER _T("\\") SETTING_IMAGE_EL_BT _T("[") _T(#N) _T("]")	// oIM\Setting\Server\Image_BT[n]

// Font.config
#define PATH_FONT_ITEM_FMT				FONT_ROOT_EL _T("\\") FONT_ITEM_EL _T("[%d]")								// eIMFont\Item[d]

#endif // __APP_CONFIG_HEADER_2017_12_28_YFGZ__