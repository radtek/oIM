//stamp:3310452e07b04a99
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
				xml_init = _T("UIDEF:xml_init");
			}
			const TCHAR * xml_init;
		}UIDEF;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
				style = _T("values:style");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
			const TCHAR * style;
		}values;
		class _SMENUEX{
			public:
			_SMENUEX(){
				new_channel_menu = _T("SMENUEX:new_channel_menu");
				hot_channel_menu = _T("SMENUEX:hot_channel_menu");
			}
			const TCHAR * new_channel_menu;
			const TCHAR * hot_channel_menu;
		}SMENUEX;
		class _LAYOUT{
			public:
			_LAYOUT(){
				dlg_main = _T("LAYOUT:dlg_main");
				dlg_login = _T("LAYOUT:dlg_login");
				dlg_channel = _T("LAYOUT:dlg_channel");
				page_login = _T("LAYOUT:page_login");
				page_settings = _T("LAYOUT:page_settings");
				page_new_channel = _T("LAYOUT:page_new_channel");
				page_hot_channel = _T("LAYOUT:page_hot_channel");
				page_friendlist = _T("LAYOUT:page_friendlist");
				SelectionDropDownWnd = _T("LAYOUT:SelectionDropDownWnd");
				FileOle = _T("LAYOUT:FileOle");
				SplitLineOle = _T("LAYOUT:SplitLineOle");
				ImageOle = _T("LAYOUT:ImageOle");
				FetchMoreOle = _T("LAYOUT:FetchMoreOle");
				RemainderOle = _T("LAYOUT:RemainderOle");
				SingleNewsOle = _T("LAYOUT:SingleNewsOle");
				MultiNewsOle = _T("LAYOUT:MultiNewsOle");
				group_chatframe = _T("LAYOUT:group-chatframe");
				chatframe_sidebar_group = _T("LAYOUT:chatframe-sidebar-group");
				chatframe_bottombar = _T("LAYOUT:chatframe-bottombar");
				chatframe_sessionlist = _T("LAYOUT:chatframe-sessionlist");
				chatframe_caption = _T("LAYOUT:chatframe-caption");
				dlg_choose_avatar = _T("LAYOUT:dlg_choose_avatar");
			}
			const TCHAR * dlg_main;
			const TCHAR * dlg_login;
			const TCHAR * dlg_channel;
			const TCHAR * page_login;
			const TCHAR * page_settings;
			const TCHAR * page_new_channel;
			const TCHAR * page_hot_channel;
			const TCHAR * page_friendlist;
			const TCHAR * SelectionDropDownWnd;
			const TCHAR * FileOle;
			const TCHAR * SplitLineOle;
			const TCHAR * ImageOle;
			const TCHAR * FetchMoreOle;
			const TCHAR * RemainderOle;
			const TCHAR * SingleNewsOle;
			const TCHAR * MultiNewsOle;
			const TCHAR * group_chatframe;
			const TCHAR * chatframe_sidebar_group;
			const TCHAR * chatframe_bottombar;
			const TCHAR * chatframe_sessionlist;
			const TCHAR * chatframe_caption;
			const TCHAR * dlg_choose_avatar;
		}LAYOUT;
		class _IMGX{
			public:
			_IMGX(){
				IMG_LOGIN_BK = _T("IMGX:IMG_LOGIN_BK");
				IMG_LOGIN_BORDER = _T("IMGX:IMG_LOGIN_BORDER");
				BTN_LOGIN_SETTING = _T("IMGX:BTN_LOGIN_SETTING");
				BTN_USERLISTCLOSE = _T("IMGX:BTN_USERLISTCLOSE");
				BTN_REGISTER = _T("IMGX:BTN_REGISTER");
				BTN_GETPSWBACK = _T("IMGX:BTN_GETPSWBACK");
				BTN_KEYBOARD = _T("IMGX:BTN_KEYBOARD");
				BTN_LOGIN = _T("IMGX:BTN_LOGIN");
				BTN_LOGIN2 = _T("IMGX:BTN_LOGIN2");
				BTN_LOGIN_2DCODE = _T("IMGX:BTN_LOGIN_2DCODE");
				IMG_HEAD_MASK = _T("IMGX:IMG_HEAD_MASK");
				IMG_HEAD = _T("IMGX:IMG_HEAD");
				IMG_AVATARS = _T("IMGX:IMG_AVATARS");
				user_header = _T("IMGX:user_header");
				user_header1 = _T("IMGX:user_header1");
				friends_search = _T("IMGX:friends_search");
				default_scrollbar = _T("IMGX:default.scrollbar");
			}
			const TCHAR * IMG_LOGIN_BK;
			const TCHAR * IMG_LOGIN_BORDER;
			const TCHAR * BTN_LOGIN_SETTING;
			const TCHAR * BTN_USERLISTCLOSE;
			const TCHAR * BTN_REGISTER;
			const TCHAR * BTN_GETPSWBACK;
			const TCHAR * BTN_KEYBOARD;
			const TCHAR * BTN_LOGIN;
			const TCHAR * BTN_LOGIN2;
			const TCHAR * BTN_LOGIN_2DCODE;
			const TCHAR * IMG_HEAD_MASK;
			const TCHAR * IMG_HEAD;
			const TCHAR * IMG_AVATARS;
			const TCHAR * user_header;
			const TCHAR * user_header1;
			const TCHAR * friends_search;
			const TCHAR * default_scrollbar;
		}IMGX;
		class _jpg{
			public:
			_jpg(){
				IMG_HEAD_SMALL = _T("jpg:IMG_HEAD_SMALL");
			}
			const TCHAR * IMG_HEAD_SMALL;
		}jpg;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _PNG{
			public:
			_PNG(){
				IMG_MAIN_BK = _T("PNG:IMG_MAIN_BK");
				ID_MAIN_TAB_GROUP = _T("PNG:ID_MAIN_TAB_GROUP");
				ID_MAIN_TAB_HOVER = _T("PNG:ID_MAIN_TAB_HOVER");
				ID_MAIN_TAB_NORMAL = _T("PNG:ID_MAIN_TAB_NORMAL");
				ID_PNG_TAB_BTN_NORMAL = _T("PNG:ID_PNG_TAB_BTN_NORMAL");
				ID_PNG_TAB_BTN_HOVER = _T("PNG:ID_PNG_TAB_BTN_HOVER");
				ID_SYSBTN_CLOSE = _T("PNG:ID_SYSBTN_CLOSE");
				ID_SYSBTN_MAX = _T("PNG:ID_SYSBTN_MAX");
				ID_SYSBTN_MIN = _T("PNG:ID_SYSBTN_MIN");
				ID_SYSBTN_RESTORE = _T("PNG:ID_SYSBTN_RESTORE");
				ID_TAB_FADE_EFF = _T("PNG:ID_TAB_FADE_EFF");
				ID_SHADOW = _T("PNG:ID_SHADOW");
				ID_TAB_SCROLL = _T("PNG:ID_TAB_SCROLL");
				ID_TABBTN_CLOSE = _T("PNG:ID_TABBTN_CLOSE");
				ID_SPLITBAR_VER = _T("PNG:ID_SPLITBAR_VER");
				ID_TABBTN = _T("PNG:ID_TABBTN");
				ID_ICON1 = _T("PNG:ID_ICON1");
				ID_ICON2 = _T("PNG:ID_ICON2");
				ID_ICON3 = _T("PNG:ID_ICON3");
				ID_ICON4 = _T("PNG:ID_ICON4");
				ID_ICON5 = _T("PNG:ID_ICON5");
				ID_ICON_MASK = _T("PNG:ID_ICON_MASK");
				ID_GROUP_TOPTOOLBAR = _T("PNG:ID_GROUP_TOPTOOLBAR");
				ID_GROUP_TABBTN = _T("PNG:ID_GROUP_TABBTN");
				ID_AD = _T("PNG:ID_AD");
				ID_EMPTY = _T("PNG:ID_EMPTY");
				ID_NO_ANNOUCE = _T("PNG:ID_NO_ANNOUCE");
				ID_GROUP_APP1 = _T("PNG:ID_GROUP_APP1");
				ID_GROUP_APP2 = _T("PNG:ID_GROUP_APP2");
				ID_GROUP_APP3 = _T("PNG:ID_GROUP_APP3");
				ID_GROUP_APP4 = _T("PNG:ID_GROUP_APP4");
				ID_GROUP_APP5 = _T("PNG:ID_GROUP_APP5");
				ID_GROUP_APP6 = _T("PNG:ID_GROUP_APP6");
				ID_GROUP_APP_TABICON = _T("PNG:ID_GROUP_APP_TABICON");
				ID_GROUP_APP_TABBTN = _T("PNG:ID_GROUP_APP_TABBTN");
				ID_GROUP_EXPAND_BTN = _T("PNG:ID_GROUP_EXPAND_BTN");
				ID_GROUP_SEARCH_CHK = _T("PNG:ID_GROUP_SEARCH_CHK");
				ID_SENDBTN_LEFT = _T("PNG:ID_SENDBTN_LEFT");
				ID_SENDBTN_RIGHT = _T("PNG:ID_SENDBTN_RIGHT");
				ID_SENDBTN_SPLITLINE = _T("PNG:ID_SENDBTN_SPLITLINE");
				ID_SENDBTN_ARROW = _T("PNG:ID_SENDBTN_ARROW");
				ID_CHATBTN_CLOSE = _T("PNG:ID_CHATBTN_CLOSE");
				ID_CHATBTN_FONT = _T("PNG:ID_CHATBTN_FONT");
				ID_CHAT_TOOLBAT_BK = _T("PNG:ID_CHAT_TOOLBAT_BK");
				ID_CHATBTN_CUT = _T("PNG:ID_CHATBTN_CUT");
				ID_CHAT_TOOLBTN_ARROW = _T("PNG:ID_CHAT_TOOLBTN_ARROW");
				ID_CHAT_QUICKBAR_UPLOADFILE = _T("PNG:ID_CHAT_QUICKBAR_UPLOADFILE");
				ID_CHAT_QUICKBAR_SENDPIC = _T("PNG:ID_CHAT_QUICKBAR_SENDPIC");
				ID_RICH_FILE_BK = _T("PNG:ID_RICH_FILE_BK");
				ID_FILE_ICON_TEXT = _T("PNG:ID_FILE_ICON_TEXT");
				ID_FETCH_MORE = _T("PNG:ID_FETCH_MORE");
				ID_LOADING = _T("PNG:ID_LOADING");
				ID_NEW_BG = _T("PNG:ID_NEW_BG");
				ID_NEWS_BG_ALPHA = _T("PNG:ID_NEWS_BG_ALPHA");
				ID_MENU_BKG = _T("PNG:ID_MENU_BKG");
				ID_SMALL_ICON = _T("PNG:ID_SMALL_ICON");
				ID_BKGND_MASK = _T("PNG:ID_BKGND_MASK");
				ID_BACKGROUND = _T("PNG:ID_BACKGROUND");
				ID_RICH_WARNING = _T("PNG:ID_RICH_WARNING");
				ID_RICH_LEFT_BUBBLE = _T("PNG:ID_RICH_LEFT_BUBBLE");
				ID_RICH_RIGHT_BUBBLE = _T("PNG:ID_RICH_RIGHT_BUBBLE");
				ID_RICH_SYSMSG_BK = _T("PNG:ID_RICH_SYSMSG_BK");
			}
			const TCHAR * IMG_MAIN_BK;
			const TCHAR * ID_MAIN_TAB_GROUP;
			const TCHAR * ID_MAIN_TAB_HOVER;
			const TCHAR * ID_MAIN_TAB_NORMAL;
			const TCHAR * ID_PNG_TAB_BTN_NORMAL;
			const TCHAR * ID_PNG_TAB_BTN_HOVER;
			const TCHAR * ID_SYSBTN_CLOSE;
			const TCHAR * ID_SYSBTN_MAX;
			const TCHAR * ID_SYSBTN_MIN;
			const TCHAR * ID_SYSBTN_RESTORE;
			const TCHAR * ID_TAB_FADE_EFF;
			const TCHAR * ID_SHADOW;
			const TCHAR * ID_TAB_SCROLL;
			const TCHAR * ID_TABBTN_CLOSE;
			const TCHAR * ID_SPLITBAR_VER;
			const TCHAR * ID_TABBTN;
			const TCHAR * ID_ICON1;
			const TCHAR * ID_ICON2;
			const TCHAR * ID_ICON3;
			const TCHAR * ID_ICON4;
			const TCHAR * ID_ICON5;
			const TCHAR * ID_ICON_MASK;
			const TCHAR * ID_GROUP_TOPTOOLBAR;
			const TCHAR * ID_GROUP_TABBTN;
			const TCHAR * ID_AD;
			const TCHAR * ID_EMPTY;
			const TCHAR * ID_NO_ANNOUCE;
			const TCHAR * ID_GROUP_APP1;
			const TCHAR * ID_GROUP_APP2;
			const TCHAR * ID_GROUP_APP3;
			const TCHAR * ID_GROUP_APP4;
			const TCHAR * ID_GROUP_APP5;
			const TCHAR * ID_GROUP_APP6;
			const TCHAR * ID_GROUP_APP_TABICON;
			const TCHAR * ID_GROUP_APP_TABBTN;
			const TCHAR * ID_GROUP_EXPAND_BTN;
			const TCHAR * ID_GROUP_SEARCH_CHK;
			const TCHAR * ID_SENDBTN_LEFT;
			const TCHAR * ID_SENDBTN_RIGHT;
			const TCHAR * ID_SENDBTN_SPLITLINE;
			const TCHAR * ID_SENDBTN_ARROW;
			const TCHAR * ID_CHATBTN_CLOSE;
			const TCHAR * ID_CHATBTN_FONT;
			const TCHAR * ID_CHAT_TOOLBAT_BK;
			const TCHAR * ID_CHATBTN_CUT;
			const TCHAR * ID_CHAT_TOOLBTN_ARROW;
			const TCHAR * ID_CHAT_QUICKBAR_UPLOADFILE;
			const TCHAR * ID_CHAT_QUICKBAR_SENDPIC;
			const TCHAR * ID_RICH_FILE_BK;
			const TCHAR * ID_FILE_ICON_TEXT;
			const TCHAR * ID_FETCH_MORE;
			const TCHAR * ID_LOADING;
			const TCHAR * ID_NEW_BG;
			const TCHAR * ID_NEWS_BG_ALPHA;
			const TCHAR * ID_MENU_BKG;
			const TCHAR * ID_SMALL_ICON;
			const TCHAR * ID_BKGND_MASK;
			const TCHAR * ID_BACKGROUND;
			const TCHAR * ID_RICH_WARNING;
			const TCHAR * ID_RICH_LEFT_BUBBLE;
			const TCHAR * ID_RICH_RIGHT_BUBBLE;
			const TCHAR * ID_RICH_SYSMSG_BK;
		}PNG;
		class _SMENU{
			public:
			_SMENU(){
				menu_send = _T("SMENU:menu_send");
			}
			const TCHAR * menu_send;
		}SMENU;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"BtnDebugJavaScript",65558},
		{L"BtnLoadURL",65557},
		{L"CefBrowser",65563},
		{L"EditURL",65556},
		{L"HeadPic_Large",65603},
		{L"HeadPic_Middle",65604},
		{L"HeadPic_Small",65605},
		{L"WindowDeubg",65560},
		{L"_name_start",65535},
		{L"bkwindow",65589},
		{L"btnChoose",65606},
		{L"btnSave",65607},
		{L"btn_1",65549},
		{L"btn_2",65550},
		{L"btn_3",65551},
		{L"btn_4",65552},
		{L"btn_close",65537},
		{L"btn_copy",65562},
		{L"btn_fetch_log",65559},
		{L"btn_friends",65547},
		{L"btn_hot_channel",65545},
		{L"btn_login",65568},
		{L"btn_max",65554},
		{L"btn_min",65538},
		{L"btn_new_channel",65546},
		{L"btn_restore",65555},
		{L"btn_send",65598},
		{L"btn_setting",65564},
		{L"btn_setting_cancel",65577},
		{L"btn_setting_ok",65578},
		{L"btn_showsendmenu",65599},
		{L"btn_test_net_set",136},
		{L"btn_user_header",65539},
		{L"btn_user_online",65540},
		{L"cb_server_type",134},
		{L"cbx_account",65566},
		{L"chromeTab",65600},
		{L"col_nick",65580},
		{L"edit_search",65541},
		{L"edt_channel_info",65561},
		{L"edt_pwd",65567},
		{L"edt_search_input",65542},
		{L"friends_list",65587},
		{L"group_member_list",65595},
		{L"hot_channel_list",65586},
		{L"img_avatar",65565},
		{L"img_icon",65583},
		{L"lv_dropdown",65543},
		{L"member_avatar",65596},
		{L"net_cfg_addr_edt",137},
		{L"net_cfg_field_edt",65574},
		{L"net_cfg_port_edt",65573},
		{L"net_cfg_pswd_edt",65572},
		{L"net_cfg_type_cmb",65570},
		{L"net_cfg_user_edt",65571},
		{L"new_channel_list",65579},
		{L"nickname",65597},
		{L"picchoose",65602},
		{L"re-input",65593},
		{L"re-message",65591},
		{L"search_group",65594},
		{L"selectionlist",65588},
		{L"send-pic",65592},
		{L"serv_cfg_addr_edt",65575},
		{L"serv_cfg_port_edt",65576},
		{L"set_form",65569},
		{L"split-vert",65590},
		{L"tab_chat1",65601},
		{L"tab_main",65548},
		{L"tgl_tv_expand",65581},
		{L"turn3d",65553},
		{L"txt_desc",65585},
		{L"txt_group",65582},
		{L"txt_key",65544},
		{L"txt_name",65584},
		{L"txt_title",65536}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			BtnDebugJavaScript = namedXmlID[0].strName;
			BtnLoadURL = namedXmlID[1].strName;
			CefBrowser = namedXmlID[2].strName;
			EditURL = namedXmlID[3].strName;
			HeadPic_Large = namedXmlID[4].strName;
			HeadPic_Middle = namedXmlID[5].strName;
			HeadPic_Small = namedXmlID[6].strName;
			WindowDeubg = namedXmlID[7].strName;
			_name_start = namedXmlID[8].strName;
			bkwindow = namedXmlID[9].strName;
			btnChoose = namedXmlID[10].strName;
			btnSave = namedXmlID[11].strName;
			btn_1 = namedXmlID[12].strName;
			btn_2 = namedXmlID[13].strName;
			btn_3 = namedXmlID[14].strName;
			btn_4 = namedXmlID[15].strName;
			btn_close = namedXmlID[16].strName;
			btn_copy = namedXmlID[17].strName;
			btn_fetch_log = namedXmlID[18].strName;
			btn_friends = namedXmlID[19].strName;
			btn_hot_channel = namedXmlID[20].strName;
			btn_login = namedXmlID[21].strName;
			btn_max = namedXmlID[22].strName;
			btn_min = namedXmlID[23].strName;
			btn_new_channel = namedXmlID[24].strName;
			btn_restore = namedXmlID[25].strName;
			btn_send = namedXmlID[26].strName;
			btn_setting = namedXmlID[27].strName;
			btn_setting_cancel = namedXmlID[28].strName;
			btn_setting_ok = namedXmlID[29].strName;
			btn_showsendmenu = namedXmlID[30].strName;
			btn_test_net_set = namedXmlID[31].strName;
			btn_user_header = namedXmlID[32].strName;
			btn_user_online = namedXmlID[33].strName;
			cb_server_type = namedXmlID[34].strName;
			cbx_account = namedXmlID[35].strName;
			chromeTab = namedXmlID[36].strName;
			col_nick = namedXmlID[37].strName;
			edit_search = namedXmlID[38].strName;
			edt_channel_info = namedXmlID[39].strName;
			edt_pwd = namedXmlID[40].strName;
			edt_search_input = namedXmlID[41].strName;
			friends_list = namedXmlID[42].strName;
			group_member_list = namedXmlID[43].strName;
			hot_channel_list = namedXmlID[44].strName;
			img_avatar = namedXmlID[45].strName;
			img_icon = namedXmlID[46].strName;
			lv_dropdown = namedXmlID[47].strName;
			member_avatar = namedXmlID[48].strName;
			net_cfg_addr_edt = namedXmlID[49].strName;
			net_cfg_field_edt = namedXmlID[50].strName;
			net_cfg_port_edt = namedXmlID[51].strName;
			net_cfg_pswd_edt = namedXmlID[52].strName;
			net_cfg_type_cmb = namedXmlID[53].strName;
			net_cfg_user_edt = namedXmlID[54].strName;
			new_channel_list = namedXmlID[55].strName;
			nickname = namedXmlID[56].strName;
			picchoose = namedXmlID[57].strName;
			re_input = namedXmlID[58].strName;
			re_message = namedXmlID[59].strName;
			search_group = namedXmlID[60].strName;
			selectionlist = namedXmlID[61].strName;
			send_pic = namedXmlID[62].strName;
			serv_cfg_addr_edt = namedXmlID[63].strName;
			serv_cfg_port_edt = namedXmlID[64].strName;
			set_form = namedXmlID[65].strName;
			split_vert = namedXmlID[66].strName;
			tab_chat1 = namedXmlID[67].strName;
			tab_main = namedXmlID[68].strName;
			tgl_tv_expand = namedXmlID[69].strName;
			turn3d = namedXmlID[70].strName;
			txt_desc = namedXmlID[71].strName;
			txt_group = namedXmlID[72].strName;
			txt_key = namedXmlID[73].strName;
			txt_name = namedXmlID[74].strName;
			txt_title = namedXmlID[75].strName;
		}
		 const wchar_t * BtnDebugJavaScript;
		 const wchar_t * BtnLoadURL;
		 const wchar_t * CefBrowser;
		 const wchar_t * EditURL;
		 const wchar_t * HeadPic_Large;
		 const wchar_t * HeadPic_Middle;
		 const wchar_t * HeadPic_Small;
		 const wchar_t * WindowDeubg;
		 const wchar_t * _name_start;
		 const wchar_t * bkwindow;
		 const wchar_t * btnChoose;
		 const wchar_t * btnSave;
		 const wchar_t * btn_1;
		 const wchar_t * btn_2;
		 const wchar_t * btn_3;
		 const wchar_t * btn_4;
		 const wchar_t * btn_close;
		 const wchar_t * btn_copy;
		 const wchar_t * btn_fetch_log;
		 const wchar_t * btn_friends;
		 const wchar_t * btn_hot_channel;
		 const wchar_t * btn_login;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_new_channel;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_send;
		 const wchar_t * btn_setting;
		 const wchar_t * btn_setting_cancel;
		 const wchar_t * btn_setting_ok;
		 const wchar_t * btn_showsendmenu;
		 const wchar_t * btn_test_net_set;
		 const wchar_t * btn_user_header;
		 const wchar_t * btn_user_online;
		 const wchar_t * cb_server_type;
		 const wchar_t * cbx_account;
		 const wchar_t * chromeTab;
		 const wchar_t * col_nick;
		 const wchar_t * edit_search;
		 const wchar_t * edt_channel_info;
		 const wchar_t * edt_pwd;
		 const wchar_t * edt_search_input;
		 const wchar_t * friends_list;
		 const wchar_t * group_member_list;
		 const wchar_t * hot_channel_list;
		 const wchar_t * img_avatar;
		 const wchar_t * img_icon;
		 const wchar_t * lv_dropdown;
		 const wchar_t * member_avatar;
		 const wchar_t * net_cfg_addr_edt;
		 const wchar_t * net_cfg_field_edt;
		 const wchar_t * net_cfg_port_edt;
		 const wchar_t * net_cfg_pswd_edt;
		 const wchar_t * net_cfg_type_cmb;
		 const wchar_t * net_cfg_user_edt;
		 const wchar_t * new_channel_list;
		 const wchar_t * nickname;
		 const wchar_t * picchoose;
		 const wchar_t * re_input;
		 const wchar_t * re_message;
		 const wchar_t * search_group;
		 const wchar_t * selectionlist;
		 const wchar_t * send_pic;
		 const wchar_t * serv_cfg_addr_edt;
		 const wchar_t * serv_cfg_port_edt;
		 const wchar_t * set_form;
		 const wchar_t * split_vert;
		 const wchar_t * tab_chat1;
		 const wchar_t * tab_main;
		 const wchar_t * tgl_tv_expand;
		 const wchar_t * turn3d;
		 const wchar_t * txt_desc;
		 const wchar_t * txt_group;
		 const wchar_t * txt_key;
		 const wchar_t * txt_name;
		 const wchar_t * txt_title;
		}name;

		class _id{
		public:
		const static int BtnDebugJavaScript	=	65558;
		const static int BtnLoadURL	=	65557;
		const static int CefBrowser	=	65563;
		const static int EditURL	=	65556;
		const static int HeadPic_Large	=	65603;
		const static int HeadPic_Middle	=	65604;
		const static int HeadPic_Small	=	65605;
		const static int WindowDeubg	=	65560;
		const static int _name_start	=	65535;
		const static int bkwindow	=	65589;
		const static int btnChoose	=	65606;
		const static int btnSave	=	65607;
		const static int btn_1	=	65549;
		const static int btn_2	=	65550;
		const static int btn_3	=	65551;
		const static int btn_4	=	65552;
		const static int btn_close	=	65537;
		const static int btn_copy	=	65562;
		const static int btn_fetch_log	=	65559;
		const static int btn_friends	=	65547;
		const static int btn_hot_channel	=	65545;
		const static int btn_login	=	65568;
		const static int btn_max	=	65554;
		const static int btn_min	=	65538;
		const static int btn_new_channel	=	65546;
		const static int btn_restore	=	65555;
		const static int btn_send	=	65598;
		const static int btn_setting	=	65564;
		const static int btn_setting_cancel	=	65577;
		const static int btn_setting_ok	=	65578;
		const static int btn_showsendmenu	=	65599;
		const static int btn_test_net_set	=	136;
		const static int btn_user_header	=	65539;
		const static int btn_user_online	=	65540;
		const static int cb_server_type	=	134;
		const static int cbx_account	=	65566;
		const static int chromeTab	=	65600;
		const static int col_nick	=	65580;
		const static int edit_search	=	65541;
		const static int edt_channel_info	=	65561;
		const static int edt_pwd	=	65567;
		const static int edt_search_input	=	65542;
		const static int friends_list	=	65587;
		const static int group_member_list	=	65595;
		const static int hot_channel_list	=	65586;
		const static int img_avatar	=	65565;
		const static int img_icon	=	65583;
		const static int lv_dropdown	=	65543;
		const static int member_avatar	=	65596;
		const static int net_cfg_addr_edt	=	137;
		const static int net_cfg_field_edt	=	65574;
		const static int net_cfg_port_edt	=	65573;
		const static int net_cfg_pswd_edt	=	65572;
		const static int net_cfg_type_cmb	=	65570;
		const static int net_cfg_user_edt	=	65571;
		const static int new_channel_list	=	65579;
		const static int nickname	=	65597;
		const static int picchoose	=	65602;
		const static int re_input	=	65593;
		const static int re_message	=	65591;
		const static int search_group	=	65594;
		const static int selectionlist	=	65588;
		const static int send_pic	=	65592;
		const static int serv_cfg_addr_edt	=	65575;
		const static int serv_cfg_port_edt	=	65576;
		const static int set_form	=	65569;
		const static int split_vert	=	65590;
		const static int tab_chat1	=	65601;
		const static int tab_main	=	65548;
		const static int tgl_tv_expand	=	65581;
		const static int turn3d	=	65553;
		const static int txt_desc	=	65585;
		const static int txt_group	=	65582;
		const static int txt_key	=	65544;
		const static int txt_name	=	65584;
		const static int txt_title	=	65536;
		}id;

		class _string{
		public:
		const static int close	=	0;
		const static int cue_search	=	1;
		const static int file_manager	=	2;
		const static int friends	=	3;
		const static int hot_channels	=	4;
		const static int main_menu	=	5;
		const static int msg_manager	=	6;
		const static int new_channels	=	7;
		const static int search_friend	=	8;
		const static int title	=	9;
		const static int ver	=	10;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int friendlist_item_header	=	1;
		const static int gray	=	2;
		const static int green	=	3;
		const static int red	=	4;
		const static int white	=	5;
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
