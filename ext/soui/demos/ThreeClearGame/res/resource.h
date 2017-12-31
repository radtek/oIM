//stamp:074cc2ca9769066c
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
				png_btn_star = _T("IMG:png_btn_star");
				png_btn_heart = _T("IMG:png_btn_heart");
				png_btn_sword = _T("IMG:png_btn_sword");
				png_btn_shield = _T("IMG:png_btn_shield");
				png_btn_delete = _T("IMG:png_btn_delete");
			}
			const TCHAR * png_btn_star;
			const TCHAR * png_btn_heart;
			const TCHAR * png_btn_sword;
			const TCHAR * png_btn_shield;
			const TCHAR * png_btn_delete;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"btn_close",65536},
		{L"btn_grid",65543},
		{L"btn_min",65538},
		{L"btn_restart",65539},
		{L"btn_restore",65537},
		{L"edit_log",65541},
		{L"text_score",65540},
		{L"tv_net",65542}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			btn_close = namedXmlID[0].strName;
			btn_grid = namedXmlID[1].strName;
			btn_min = namedXmlID[2].strName;
			btn_restart = namedXmlID[3].strName;
			btn_restore = namedXmlID[4].strName;
			edit_log = namedXmlID[5].strName;
			text_score = namedXmlID[6].strName;
			tv_net = namedXmlID[7].strName;
		}
		 const wchar_t * btn_close;
		 const wchar_t * btn_grid;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restart;
		 const wchar_t * btn_restore;
		 const wchar_t * edit_log;
		 const wchar_t * text_score;
		 const wchar_t * tv_net;
		}name;

		class _id{
		public:
		const static int btn_close	=	65536;
		const static int btn_grid	=	65543;
		const static int btn_min	=	65538;
		const static int btn_restart	=	65539;
		const static int btn_restore	=	65537;
		const static int edit_log	=	65541;
		const static int text_score	=	65540;
		const static int tv_net	=	65542;
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
