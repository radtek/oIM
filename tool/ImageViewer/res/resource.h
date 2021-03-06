//stamp:074db0dc61af8355
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
			}
			const TCHAR * XML_MAINWND;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMG{
			public:
			_IMG(){
				SYS_CLOSE = _T("IMG:SYS_CLOSE");
				SYS_BKGND = _T("IMG:SYS_BKGND");
				IMG_ERROR = _T("IMG:IMG_ERROR");
				IMG_RATIO = _T("IMG:IMG_RATIO");
				THUMB_BKGND = _T("IMG:THUMB_BKGND");
				THUMB_POS = _T("IMG:THUMB_POS");
				THUMB_TOGGLE = _T("IMG:THUMB_TOGGLE");
				BTN_PREV = _T("IMG:BTN_PREV");
				BTN_NEXT = _T("IMG:BTN_NEXT");
				BTN_LEFT = _T("IMG:BTN_LEFT");
				BTN_RIGHT = _T("IMG:BTN_RIGHT");
				TOOLBAR_BKGND = _T("IMG:TOOLBAR_BKGND");
				TOOLBAR_1TO1 = _T("IMG:TOOLBAR_1TO1");
				TOOLBAR_FULLSCREEN = _T("IMG:TOOLBAR_FULLSCREEN");
				TOOLBAR_ZOOMOUT = _T("IMG:TOOLBAR_ZOOMOUT");
				TOOLBAR_ZOOMIN = _T("IMG:TOOLBAR_ZOOMIN");
				TOOLBAR_ROTATE = _T("IMG:TOOLBAR_ROTATE");
				TOOLBAR_FOLDER = _T("IMG:TOOLBAR_FOLDER");
				TOOLBAR_SAVEAS = _T("IMG:TOOLBAR_SAVEAS");
				TOOLBAR_TOGGLE = _T("IMG:TOOLBAR_TOGGLE");
			}
			const TCHAR * SYS_CLOSE;
			const TCHAR * SYS_BKGND;
			const TCHAR * IMG_ERROR;
			const TCHAR * IMG_RATIO;
			const TCHAR * THUMB_BKGND;
			const TCHAR * THUMB_POS;
			const TCHAR * THUMB_TOGGLE;
			const TCHAR * BTN_PREV;
			const TCHAR * BTN_NEXT;
			const TCHAR * BTN_LEFT;
			const TCHAR * BTN_RIGHT;
			const TCHAR * TOOLBAR_BKGND;
			const TCHAR * TOOLBAR_1TO1;
			const TCHAR * TOOLBAR_FULLSCREEN;
			const TCHAR * TOOLBAR_ZOOMOUT;
			const TCHAR * TOOLBAR_ZOOMIN;
			const TCHAR * TOOLBAR_ROTATE;
			const TCHAR * TOOLBAR_FOLDER;
			const TCHAR * TOOLBAR_SAVEAS;
			const TCHAR * TOOLBAR_TOGGLE;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _translator{
			public:
			_translator(){
				lang_cn = _T("translator:lang_cn");
				lang_en = _T("translator:lang_en");
			}
			const TCHAR * lang_cn;
			const TCHAR * lang_en;
		}translator;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"btn_close",65537},
		{L"btn_next",65539},
		{L"btn_prev",65538},
		{L"btn_toolbar.1to1",65544},
		{L"btn_toolbar.folder",65550},
		{L"btn_toolbar.fullscreen",65545},
		{L"btn_toolbar.rotate",65548},
		{L"btn_toolbar.saveas",65549},
		{L"btn_toolbar.zoomin",65546},
		{L"btn_toolbar.zoomout",65547},
		{L"imageViewer",65536},
		{L"img_pos",65542},
		{L"img_thumb",65541},
		{L"thumb_toggle",65543},
		{L"thumb_view",65540},
		{L"toolbar_toggle",65551},
		{L"toolbar_view",100},
		{L"txt_ratio",65552}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			btn_close = namedXmlID[1].strName;
			btn_next = namedXmlID[2].strName;
			btn_prev = namedXmlID[3].strName;
			btn_toolbar_1to1 = namedXmlID[4].strName;
			btn_toolbar_folder = namedXmlID[5].strName;
			btn_toolbar_fullscreen = namedXmlID[6].strName;
			btn_toolbar_rotate = namedXmlID[7].strName;
			btn_toolbar_saveas = namedXmlID[8].strName;
			btn_toolbar_zoomin = namedXmlID[9].strName;
			btn_toolbar_zoomout = namedXmlID[10].strName;
			imageViewer = namedXmlID[11].strName;
			img_pos = namedXmlID[12].strName;
			img_thumb = namedXmlID[13].strName;
			thumb_toggle = namedXmlID[14].strName;
			thumb_view = namedXmlID[15].strName;
			toolbar_toggle = namedXmlID[16].strName;
			toolbar_view = namedXmlID[17].strName;
			txt_ratio = namedXmlID[18].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * btn_close;
		 const wchar_t * btn_next;
		 const wchar_t * btn_prev;
		 const wchar_t * btn_toolbar_1to1;
		 const wchar_t * btn_toolbar_folder;
		 const wchar_t * btn_toolbar_fullscreen;
		 const wchar_t * btn_toolbar_rotate;
		 const wchar_t * btn_toolbar_saveas;
		 const wchar_t * btn_toolbar_zoomin;
		 const wchar_t * btn_toolbar_zoomout;
		 const wchar_t * imageViewer;
		 const wchar_t * img_pos;
		 const wchar_t * img_thumb;
		 const wchar_t * thumb_toggle;
		 const wchar_t * thumb_view;
		 const wchar_t * toolbar_toggle;
		 const wchar_t * toolbar_view;
		 const wchar_t * txt_ratio;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int btn_close	=	65537;
		const static int btn_next	=	65539;
		const static int btn_prev	=	65538;
		const static int btn_toolbar_1to1	=	65544;
		const static int btn_toolbar_folder	=	65550;
		const static int btn_toolbar_fullscreen	=	65545;
		const static int btn_toolbar_rotate	=	65548;
		const static int btn_toolbar_saveas	=	65549;
		const static int btn_toolbar_zoomin	=	65546;
		const static int btn_toolbar_zoomout	=	65547;
		const static int imageViewer	=	65536;
		const static int img_pos	=	65542;
		const static int img_thumb	=	65541;
		const static int thumb_toggle	=	65543;
		const static int thumb_view	=	65540;
		const static int toolbar_toggle	=	65551;
		const static int toolbar_view	=	100;
		const static int txt_ratio	=	65552;
		}id;

		class _string{
		public:
		const static int btnThumbHide	=	0;
		const static int btnThumbShow	=	1;
		const static int btnToolbar1to1	=	2;
		const static int btnToolbarExitFullscreen	=	3;
		const static int btnToolbarFolder	=	4;
		const static int btnToolbarFullscreen	=	5;
		const static int btnToolbarHide	=	6;
		const static int btnToolbarIs1to1	=	7;
		const static int btnToolbarRotate	=	8;
		const static int btnToolbarSaveas	=	9;
		const static int btnToolbarShow	=	10;
		const static int btnToolbarZoomin	=	11;
		const static int btnToolbarZoomout	=	12;
		const static int lang_cn	=	13;
		const static int lang_en	=	14;
		const static int ratio	=	15;
		const static int sysClose	=	16;
		const static int tipInRecord	=	17;
		const static int tipLast	=	18;
		const static int tipPageN	=	19;
		const static int title	=	20;
		const static int usage_help	=	21;
		const static int usages	=	22;
		const static int ver	=	23;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int gray	=	1;
		const static int green	=	2;
		const static int red	=	3;
		const static int white	=	4;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
