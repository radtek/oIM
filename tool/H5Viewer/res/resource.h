//stamp:074d3eebd58f91cc
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
				png_chrome_tab_close = _T("IMG:png_chrome_tab_close");
				png_chrome_tab_new = _T("IMG:png_chrome_tab_new");
				png_webbtn_back = _T("IMG:png_webbtn_back");
				png_webbtn_fore = _T("IMG:png_webbtn_fore");
				png_webbtn_refresh = _T("IMG:png_webbtn_refresh");
				jpg_bkgnd_1 = _T("IMG:jpg_bkgnd_1");
			}
			const TCHAR * png_chrome_tab_close;
			const TCHAR * png_chrome_tab_new;
			const TCHAR * png_webbtn_back;
			const TCHAR * png_webbtn_fore;
			const TCHAR * png_webbtn_refresh;
			const TCHAR * jpg_bkgnd_1;
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
				lang_jp = _T("translator:lang_jp");
			}
			const TCHAR * lang_cn;
			const TCHAR * lang_jp;
		}translator;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"btn_close",65538},
		{L"btn_max",65539},
		{L"btn_min",65541},
		{L"btn_restore",65540},
		{L"btn_webkit_back",65544},
		{L"btn_webkit_forward",65545},
		{L"btn_webkit_go",65548},
		{L"btn_webkit_refresh",65546},
		{L"button_run_c_plus_plus",65537},
		{L"button_run_js",65536},
		{L"chromeTab",65542},
		{L"edit_url",65547},
		{L"webkit_toolbar",65543},
		{L"wke_test",65549}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			btn_close = namedXmlID[0].strName;
			btn_max = namedXmlID[1].strName;
			btn_min = namedXmlID[2].strName;
			btn_restore = namedXmlID[3].strName;
			btn_webkit_back = namedXmlID[4].strName;
			btn_webkit_forward = namedXmlID[5].strName;
			btn_webkit_go = namedXmlID[6].strName;
			btn_webkit_refresh = namedXmlID[7].strName;
			button_run_c_plus_plus = namedXmlID[8].strName;
			button_run_js = namedXmlID[9].strName;
			chromeTab = namedXmlID[10].strName;
			edit_url = namedXmlID[11].strName;
			webkit_toolbar = namedXmlID[12].strName;
			wke_test = namedXmlID[13].strName;
		}
		 const wchar_t * btn_close;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_webkit_back;
		 const wchar_t * btn_webkit_forward;
		 const wchar_t * btn_webkit_go;
		 const wchar_t * btn_webkit_refresh;
		 const wchar_t * button_run_c_plus_plus;
		 const wchar_t * button_run_js;
		 const wchar_t * chromeTab;
		 const wchar_t * edit_url;
		 const wchar_t * webkit_toolbar;
		 const wchar_t * wke_test;
		}name;

		class _id{
		public:
		const static int btn_close	=	65538;
		const static int btn_max	=	65539;
		const static int btn_min	=	65541;
		const static int btn_restore	=	65540;
		const static int btn_webkit_back	=	65544;
		const static int btn_webkit_forward	=	65545;
		const static int btn_webkit_go	=	65548;
		const static int btn_webkit_refresh	=	65546;
		const static int button_run_c_plus_plus	=	65537;
		const static int button_run_js	=	65536;
		const static int chromeTab	=	65542;
		const static int edit_url	=	65547;
		const static int webkit_toolbar	=	65543;
		const static int wke_test	=	65549;
		}id;

		class _string{
		public:
		const static int title	=	0;
		const static int ver	=	1;
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
