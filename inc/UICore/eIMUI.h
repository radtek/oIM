// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: eIMUI is a public module
// 
// Auth: yfgz
// Date: 
// 
// History: 
//    2013/7/4 yfgz Finished first version
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __EIMUI_HEADER_2014_04_15_YFGZ__
#define __EIMUI_HEADER_2014_04_15_YFGZ__

#include "eIMConfigConst.h"
#include "C_HelpAPI.h"
// UI日志接口名（虚拟名字，以区分不同类型的UI日志）
#define INAME_EIMUI_LOGGER				_T("SXIT.EIMUI.Logger")		// eIMUILogger interface name
#define INAME_EIMUI_MAIN				_T("SXIT.EIMUI.Main")		// Main interface
#define INAME_EIMUI_LOGIN				_T("SXIT.EIMUI.Login")		// Login interface
#define INAME_EIMUI_CONFIGER			_T("SXIT.EIMUI.Configer")	// App Configer

// EIM 中通用的全局皮肤名字
const TCHAR* const kSkinFmt				= _T("file='%s' corner='600,300,5,5'");

//App
const TCHAR* const kAppName				=_T("app_name");		// Application name
const TCHAR* const kAppAccount			=_T("app_account");		// Application account
const TCHAR* const kAppSession			=_T("app_session");		// Application session
const TCHAR* const kAppMsgboxTitle		=_T("app_msgbox_title");// Application message box title
const TCHAR* const kText				=_T("text");			// Text
const TCHAR* const kTextFmt				=_T("text_fmt");
const TCHAR* const kTitle				=_T("lbl_title");		// Title
const TCHAR* const kTooltip				=_T("tooltip");		// Tooltip

// State
const TCHAR* const kStateUser	        = _T("state_user");		// User state
const TCHAR* const kStateMale		    = _T("male");			// Male
const TCHAR* const kStateFemale		    = _T("female");			// Female
const TCHAR* const kStateOffline		= _T("offline");		// Offline
const TCHAR* const kStateOnlinePC		= _T("pc_online");		// PC online
const TCHAR* const kStateLeavePC		= _T("pc_leave");		// PC leave
const TCHAR* const kStateOnlineMobile	= _T("mobile_online");	// Mobile online
const TCHAR* const kStateLeaveMobile    = _T("mobile_leave");	// Mobile leave
const TCHAR* const kStateGroup		    = _T("group");			// Group
const TCHAR* const kStateGroupNoRecv	= _T("group_norecv");	// Group disable receive prompt
const TCHAR* const kStateGroupFix		= _T("group_fix");		// Fix group
const TCHAR* const kStateGroupFixMsg	= _T("group_fixmsg");	// Fix group message

// Common control name
// Duilib中已经定义了 "closebtn","minbtn","maxbtn","restorebtn", 直接使用
const TCHAR* const kTitlebarControlName		    = _T("titlebar");	// Titlebar
const TCHAR* const kLogoLabelControlName		= _T("logo");		// Logo
const TCHAR* const kTitleLabelControlName		= _T("title");		// Title
const TCHAR* const kOkButtonControlName			= _T("btn_ok");		// OK button
const TCHAR* const kCancelButtonControlName		= _T("btn_cancel");	// Cancel button
const TCHAR* const kBackgroundControlName		= _T("bg");			// Background
const TCHAR* const kBorderControlName		    = _T("border");		// Border
const TCHAR* const kWatermarkFmt			    = _T("watermark");	// Border's watermark

const TCHAR* const kSkinFolder					= _T("Skin\\");							// Skin folder
const TCHAR* const kSkinFolderX					= _T("SkinX");							// SkinX folder
const TCHAR* const kHeadiRobotWoman             = _T("Common/irobot.jpg");				// Head logo of robot woman
const TCHAR* const kHeadLogoMan					= _T("Common/male.png");				// Head logo of man
const TCHAR* const kHeadLogoWoman				= _T("Common/female.png");				// Head logo of woman

const TCHAR* const kLateStyle	 = _T("LateStyle");	// Fix group message
const TCHAR* const kChineseStyle = _T("LangChinese");	// Fix group message
const TCHAR* const kEnglishStyle = _T("LangEnglish");	// Fix group message

const TCHAR* const kHideText = _T("*****");
const TCHAR* const kMenuMsg = _T("menu_msg");


// 屏蔽掉窗口的最大化
#define DISABLE_MAXIMIZE_RET_(P, R)	\
	if ( GET_SC_WPARAM(P) == SC_MAXIMIZE ) \
	{ \
		bHandled = TRUE; \
		return (R); \
	}

// 屏蔽掉主窗口的  最大化 和 最小化
#define DISABLE_MAXIMIZE_MINIMIZE_RET_(P, R)	\
	if ( GET_SC_WPARAM(P) == SC_MAXIMIZE || GET_SC_WPARAM(P) == SC_MINIMIZE) \
	{ \
		bHandled = TRUE; \
		return (R); \
	}

// 初始化皮肤语言切换功能
#define INIT_SKIN_(PM) \
{ \
	(PM).AddSkinChangedReceiver(this); \
	S_SkinChangeParam sParam; \
	sParam.szLanguage = hpi.UIGetLanguage(); \
	Receive(sParam); \
}

// 卸载皮肤语言切换功能
#define UNINIT_SKIN_(PM) \
	(PM).DelSkinChangedReceiver(this);

// 皮肤语言切换默认实现	
#define SKIN_RECEIVE_() \
	BOOL Receive(const S_SkinChangeParam& sParam) \
	{ \
		if (!sParam.szLanguage.IsEmpty()) \
		{ \
			m_PaintManager.SetCurStyles(sParam.szLanguage.GetData()); \
			return TRUE; \
		} \
		return FALSE; \
	}

static inline BOOL _GetSkinPath(int i32Ratio, CDuiString& szSkinPath)
{
	if ( i32Ratio > 1 )
		szSkinPath.Format(_T("%s%s%s%d\\"), CPaintManagerUI::GetInstancePath().GetData(), kSkinFolder, kSkinFolderX, i32Ratio);
	else
		szSkinPath.Format(_T("%s%s%s"), CPaintManagerUI::GetInstancePath().GetData(), kSkinFolder, kSkinFolder);

	return PathFileExists(szSkinPath.GetData());
}

// 注意：调用该函数前，需要先调用 CPaintManagerUI::SetInstance(m_hInstance);
inline CDuiString eIMGetSkinPath(int i32HPI = 0, int* pi32DpiRatio = NULL)
{
	static CDuiString szSkinPath;
	static int i32Ratio = 1;

	if ( szSkinPath.IsEmpty() )
	{
		int nDpiX = 96; // 禁用高DPI功能
		if ( i32HPI > 1 )
		{	// 当启用了高DPI， 就要查询当前的DPI，只使用X
			HWND hWnd = ::GetDesktopWindow();
			HDC hdc   = ::GetWindowDC(hWnd);
			int nDpiX = ::GetDeviceCaps(hdc, LOGPIXELSX); // 启用高DPI功能
			:: ReleaseDC(hWnd, hdc);
		}

		if ( i32HPI >= 3 && nDpiX >= 240 && _GetSkinPath(i32Ratio = 3, szSkinPath) ||	// 2.5 * 96DPI ==> 3X	
			 i32HPI >= 2 && nDpiX >= 144 && _GetSkinPath(i32Ratio = 2, szSkinPath) ||	// 1.5 * 96DPI ==> 2X
			 _GetSkinPath(i32Ratio = 1, szSkinPath) )									// As 96DPI    ==> 1X 
		{
			return szSkinPath;
		}
	}

	if ( pi32DpiRatio )
		*pi32DpiRatio = i32Ratio;

	return szSkinPath;
}

inline int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

inline BOOL eIMMakeWatermark(LPCTSTR pszCompany, LPCTSTR pszAccount, LPCTSTR pszTemplate, LPCTSTR pszSaveas)
{
	Image	 img(pszTemplate);
	UINT	 u32Width  = img.GetWidth();
	UINT	 u32Height = img.GetHeight();
	float    fHeight   = u32Height / 3.0f;
	float    fWidth    = u32Width+fHeight;
//	float    fItem1X   = u32Height * 0.11111f;	// 1/9
	float    fItem2X   = u32Height * 0.33333f;	// 1/3
	float    fItem3X   = u32Height * 0.66666f;  // 2/3
	Graphics g(&img);
	StringFormat stringFmt;
	SYSTEMTIME   sysTime = { 0 };
	CString szDate;

	GetLocalTime(&sysTime);
	szDate.Format(_T("%d/%02d/%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);

	stringFmt.SetAlignment(StringAlignmentCenter);
	stringFmt.SetFormatFlags(StringFormatFlagsNoWrap);

	// Initialize arguments.
	Gdiplus::Font myFont(_T("仿宋"),16, FontStyleRegular, UnitPixel);
	RectF rtCompany1(0.0f, fItem2X - 20, fWidth, fHeight);
	RectF rtAccount( 0.0f, fItem2X, fWidth, fHeight);
	RectF rtCompany2(0.0f, fItem2X + 20, fWidth, fHeight);

	SolidBrush blackBrush(Color(255, 0, 0, 0));

	g.SetInterpolationMode(InterpolationModeHighQuality);
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.TranslateTransform(-fHeight, fItem3X);           
	g.RotateTransform(315.0);

	g.DrawString( pszCompany, -1, &myFont, rtCompany1, &stringFmt, &blackBrush);
	g.DrawString( pszAccount, -1, &myFont, rtAccount,  &stringFmt, &blackBrush);
	g.DrawString( szDate,     -1, &myFont, rtCompany2, &stringFmt, &blackBrush);
	
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);

	return (Ok == img.Save(pszSaveas,  &pngClsid));
}

inline BOOL eIMSetWatermark(CPaintManagerUI* pPM, BOOL bDel=FALSE)
{
	CHECK_NULL_RET_(pPM, FALSE);
	CDuiString  szWatermarkFmt = pPM->GetControlStyle(_T("Border"), kWatermarkFmt);
	if ( szWatermarkFmt.IsEmpty() )
		return FALSE;	// 没有配置水印就是退出

	LPCTSTR   pszAccount = hpi.GetAttributeStr(PATH_TOKEN_LOGINNAME);	CHECK_NULL_RET_(pszAccount, FALSE);
	eIMString szCompany  = pPM->GetControlStyle(kAppName, kText);
	eIMString szWatermark= PATH_TOKEN_LOGINID_FILE_(_T("Watermark.png"));
	eIMString szTemplate = eIMGetSkinPath(pPM->IsDpiAware()) + _T("Common\\watermark.png");
	
	eIMReplaceToken(szWatermark);
	eIMReplaceToken(szTemplate);

	if ( !CRenderEngine::IsValidImage(szTemplate.c_str()) )	
		return FALSE;	// 没有模板水印图片，也退出

	if ( bDel )
		DeleteFile(szWatermark.c_str());

	if ( !CRenderEngine::IsValidImage(szWatermark.c_str()) && // 不存在用户水印图片
		 !eIMMakeWatermark(szCompany.c_str(), pszAccount, szTemplate.c_str(), szWatermark.c_str()) )	// 生成用户水印图片
		 return FALSE;

	if ( CControlUI* pBorder = pPM->FindControl(kBorderControlName) )
	{
		CDuiString  szBkImage;

		szBkImage.Format(szWatermarkFmt, szWatermark.c_str());
		pBorder->SetBkImage(szBkImage);
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
#define SHOW_VIRT_WND_LOGIN		( 0 )	// Show Login virtual window
#define SHOW_VIRT_WND_MAIN		( 1 )	// Show Main virtual window


#endif // __EIMUI_HEADER_2014_04_15_YFGZ__