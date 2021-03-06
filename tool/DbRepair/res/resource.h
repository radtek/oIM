//stamp:0af45a3b96dbb837
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
				XML_ABOUTWND = _T("LAYOUT:XML_ABOUTWND");
			}
			const TCHAR * XML_MAINWND;
			const TCHAR * XML_ABOUTWND;
		}LAYOUT;
		class _SMENU{
			public:
			_SMENU(){
				RICHEDIT_MENU = _T("SMENU:RICHEDIT_MENU");
			}
			const TCHAR * RICHEDIT_MENU;
		}SMENU;
		class _PNG{
			public:
			_PNG(){
				ID_PNG_SHADOW = _T("PNG:ID_PNG_SHADOW");
				ID_PNG_BG = _T("PNG:ID_PNG_BG");
			}
			const TCHAR * ID_PNG_SHADOW;
			const TCHAR * ID_PNG_BG;
		}PNG;
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
			}
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _lang{
			public:
			_lang(){
				lang_cn = _T("lang:lang_cn");
				lang_en = _T("lang:lang_en");
			}
			const TCHAR * lang_cn;
			const TCHAR * lang_en;
		}lang;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"btn_check_repair",65542},
		{L"btn_close",65536},
		{L"btn_db",65540},
		{L"btn_max",65537},
		{L"btn_min",65539},
		{L"btn_open_folder",65541},
		{L"btn_restore",65538},
		{L"btn_sql",65547},
		{L"cmb_db",65543},
		{L"cmb_sql",65546},
		{L"execSql",65545},
		{L"menuAbout",112},
		{L"menuChinese",110},
		{L"menuCleanup",107},
		{L"menuContactsZip",105},
		{L"menuEncrypted",104},
		{L"menuEnglish",111},
		{L"menuOpenDb",100},
		{L"menuOpenFolder",101},
		{L"menuOpenLog",108},
		{L"menuOpenLogFolder",109},
		{L"menuRepair",102},
		{L"menuRepairBest",103},
		{L"menuSql",106},
		{L"rdt_Log",65544},
		{L"rdt_help",65550},
		{L"txt_copyright",65549},
		{L"txt_version",65548}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			btn_check_repair = namedXmlID[0].strName;
			btn_close = namedXmlID[1].strName;
			btn_db = namedXmlID[2].strName;
			btn_max = namedXmlID[3].strName;
			btn_min = namedXmlID[4].strName;
			btn_open_folder = namedXmlID[5].strName;
			btn_restore = namedXmlID[6].strName;
			btn_sql = namedXmlID[7].strName;
			cmb_db = namedXmlID[8].strName;
			cmb_sql = namedXmlID[9].strName;
			execSql = namedXmlID[10].strName;
			menuAbout = namedXmlID[11].strName;
			menuChinese = namedXmlID[12].strName;
			menuCleanup = namedXmlID[13].strName;
			menuContactsZip = namedXmlID[14].strName;
			menuEncrypted = namedXmlID[15].strName;
			menuEnglish = namedXmlID[16].strName;
			menuOpenDb = namedXmlID[17].strName;
			menuOpenFolder = namedXmlID[18].strName;
			menuOpenLog = namedXmlID[19].strName;
			menuOpenLogFolder = namedXmlID[20].strName;
			menuRepair = namedXmlID[21].strName;
			menuRepairBest = namedXmlID[22].strName;
			menuSql = namedXmlID[23].strName;
			rdt_Log = namedXmlID[24].strName;
			rdt_help = namedXmlID[25].strName;
			txt_copyright = namedXmlID[26].strName;
			txt_version = namedXmlID[27].strName;
		}
		 const wchar_t * btn_check_repair;
		 const wchar_t * btn_close;
		 const wchar_t * btn_db;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_open_folder;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_sql;
		 const wchar_t * cmb_db;
		 const wchar_t * cmb_sql;
		 const wchar_t * execSql;
		 const wchar_t * menuAbout;
		 const wchar_t * menuChinese;
		 const wchar_t * menuCleanup;
		 const wchar_t * menuContactsZip;
		 const wchar_t * menuEncrypted;
		 const wchar_t * menuEnglish;
		 const wchar_t * menuOpenDb;
		 const wchar_t * menuOpenFolder;
		 const wchar_t * menuOpenLog;
		 const wchar_t * menuOpenLogFolder;
		 const wchar_t * menuRepair;
		 const wchar_t * menuRepairBest;
		 const wchar_t * menuSql;
		 const wchar_t * rdt_Log;
		 const wchar_t * rdt_help;
		 const wchar_t * txt_copyright;
		 const wchar_t * txt_version;
		}name;

		class _id{
		public:
		const static int btn_check_repair	=	65542;
		const static int btn_close	=	65536;
		const static int btn_db	=	65540;
		const static int btn_max	=	65537;
		const static int btn_min	=	65539;
		const static int btn_open_folder	=	65541;
		const static int btn_restore	=	65538;
		const static int btn_sql	=	65547;
		const static int cmb_db	=	65543;
		const static int cmb_sql	=	65546;
		const static int execSql	=	65545;
		const static int menuAbout	=	112;
		const static int menuChinese	=	110;
		const static int menuCleanup	=	107;
		const static int menuContactsZip	=	105;
		const static int menuEncrypted	=	104;
		const static int menuEnglish	=	111;
		const static int menuOpenDb	=	100;
		const static int menuOpenFolder	=	101;
		const static int menuOpenLog	=	108;
		const static int menuOpenLogFolder	=	109;
		const static int menuRepair	=	102;
		const static int menuRepairBest	=	103;
		const static int menuSql	=	106;
		const static int rdt_Log	=	65544;
		const static int rdt_help	=	65550;
		const static int txt_copyright	=	65549;
		const static int txt_version	=	65548;
		}id;

		class _string{
		public:
		const static int aboutCopyright	=	0;
		const static int aboutHelp	=	1;
		const static int aboutTitle	=	2;
		const static int aboutVersion	=	3;
		const static int addDbFile	=	4;
		const static int addDbFolder	=	5;
		const static int alreadyRun	=	6;
		const static int attachMsgDbFailed	=	7;
		const static int attachMsgDbOk	=	8;
		const static int backupFailed	=	9;
		const static int backupFailed2	=	10;
		const static int backupOk	=	11;
		const static int beginMakeZip	=	12;
		const static int checkFailed	=	13;
		const static int checkFailed2	=	14;
		const static int checkOk	=	15;
		const static int checkOk2	=	16;
		const static int checkUserDbFailed	=	17;
		const static int contactsZip	=	18;
		const static int dbEncrypted	=	19;
		const static int dbNoEncrypted	=	20;
		const static int dbNotExist	=	21;
		const static int dbOpened	=	22;
		const static int dbPathBad	=	23;
		const static int encryped	=	24;
		const static int execSql	=	25;
		const static int execSqlFailed	=	26;
		const static int execStop	=	27;
		const static int finishedTableRepair	=	28;
		const static int finishedTableRepair2	=	29;
		const static int lang_cn	=	30;
		const static int lang_en	=	31;
		const static int logFile	=	32;
		const static int mainTitle	=	33;
		const static int makeZip	=	34;
		const static int makeZipOk	=	35;
		const static int menuAbout	=	36;
		const static int menuChinise	=	37;
		const static int menuCleanup	=	38;
		const static int menuContactsZip	=	39;
		const static int menuEncrypted	=	40;
		const static int menuEnglish	=	41;
		const static int menuOpenDb	=	42;
		const static int menuOpenFolder	=	43;
		const static int menuOpenLog	=	44;
		const static int menuOpenLogFolder	=	45;
		const static int menuRepair	=	46;
		const static int menuRepairBest	=	47;
		const static int menuSql	=	48;
		const static int modeBest	=	49;
		const static int modeFast	=	50;
		const static int noExistDb	=	51;
		const static int notFindDb	=	52;
		const static int notFound	=	53;
		const static int notFound2	=	54;
		const static int notice	=	55;
		const static int openContactsDbFailed	=	56;
		const static int openDb	=	57;
		const static int openDbOk	=	58;
		const static int openFolder	=	59;
		const static int openLog	=	60;
		const static int openUserDbFailed	=	61;
		const static int prompt	=	62;
		const static int repair	=	63;
		const static int repairBegin	=	64;
		const static int repairEnd	=	65;
		const static int repairFailed	=	66;
		const static int repairFailed2	=	67;
		const static int repairFinished	=	68;
		const static int repairReady	=	69;
		const static int rowCount	=	70;
		const static int sql	=	71;
		const static int sqlDeptCount	=	72;
		const static int sqlEmpCount	=	73;
		const static int sqlIntegrityCheck	=	74;
		const static int sqlSessions	=	75;
		const static int sqlSqliteMaster	=	76;
		const static int sqlTables	=	77;
		const static int sqlUserVersion	=	78;
		const static int startFailed	=	79;
		const static int startRepair	=	80;
		const static int startSqlThreadFailed	=	81;
		const static int startTableRepair	=	82;
		const static int startThreadFiled	=	83;
		const static int syncBegin	=	84;
		const static int syncEnd	=	85;
		const static int syncEndFailed	=	86;
		const static int syncFailed	=	87;
		const static int sysClose	=	88;
		const static int sysMax	=	89;
		const static int sysMin	=	90;
		const static int sysRestore	=	91;
		const static int tableRepairFailed	=	92;
		const static int usage_help	=	93;
		const static int usages	=	94;
		const static int useBackupOk	=	95;
		const static int ver	=	96;
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
