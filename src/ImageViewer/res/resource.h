//stamp:074d6b7a6a8e2236
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
				SYS_SHADOW = _T("IMG:SYS_SHADOW");
				SYS_CLOSE = _T("IMG:SYS_CLOSE");
				SYS_BKGND = _T("IMG:SYS_BKGND");
				IMG_ERROR = _T("IMG:IMG_ERROR");
				IMG_RATIO = _T("IMG:IMG_RATIO");
				IMG_THUMB = _T("IMG:IMG_THUMB");
				BTN_PREV = _T("IMG:BTN_PREV");
				BTN_NEXT = _T("IMG:BTN_NEXT");
				BTN_LEFT = _T("IMG:BTN_LEFT");
				BTN_RIGHT = _T("IMG:BTN_RIGHT");
			}
			const TCHAR * SYS_SHADOW;
			const TCHAR * SYS_CLOSE;
			const TCHAR * SYS_BKGND;
			const TCHAR * IMG_ERROR;
			const TCHAR * IMG_RATIO;
			const TCHAR * IMG_THUMB;
			const TCHAR * BTN_PREV;
			const TCHAR * BTN_NEXT;
			const TCHAR * BTN_LEFT;
			const TCHAR * BTN_RIGHT;
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
		{L"btn_close",65539},
		{L"btn_next",65538},
		{L"btn_prev",65537},
		{L"imageSwitcher",65536},
		{L"img_thumb",65541},
		{L"txt_ratio",65540}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			btn_close = namedXmlID[0].strName;
			btn_next = namedXmlID[1].strName;
			btn_prev = namedXmlID[2].strName;
			imageSwitcher = namedXmlID[3].strName;
			img_thumb = namedXmlID[4].strName;
			txt_ratio = namedXmlID[5].strName;
		}
		 const wchar_t * btn_close;
		 const wchar_t * btn_next;
		 const wchar_t * btn_prev;
		 const wchar_t * imageSwitcher;
		 const wchar_t * img_thumb;
		 const wchar_t * txt_ratio;
		}name;

		class _id{
		public:
		const static int btn_close	=	65539;
		const static int btn_next	=	65538;
		const static int btn_prev	=	65537;
		const static int imageSwitcher	=	65536;
		const static int img_thumb	=	65541;
		const static int txt_ratio	=	65540;
		}id;

		class _string{
		public:
		const static int ratio	=	0;
		const static int sysClose	=	1;
		const static int tipInRecord	=	2;
		const static int tipLast	=	3;
		const static int tipPageN	=	4;
		const static int title	=	5;
		const static int ver	=	6;
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
