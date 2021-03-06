// Description: Error code define and helper function
// 
// 
// 
// Auth: LongWQ
// Date: 2013/12/21 19:11:14
// 
// History: 
//    2013/12/21 LongWQ 
//    
//    
/////////////////////////////////////////////////////////////////////////////

#ifndef __OIM_ERRORS_2013_11_29_BY_YFGZ__
#define __OIM_ERRORS_2013_11_29_BY_YFGZ__
#include <WinError.h>

#ifndef MAKE_SF_EXCEPTION
///////////////////////////////////////////////////////////////////////////////
//  HRESULTs are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|R|C|N|r|    Facility         |               Code            |
//  +-+-+-+-+-+---------------------+-------------------------------+
//  where
//
//      Sev      - is the severity code
//                 00 - Success
//                 01 - Informational
//                 10 - Warning
//                 11 - Error
//
//      C        - is the Customer code flag
//      R        - is a reserved bit
//      Facility - is the facility code
//      Code     - is the facility's status code

// Useful macro for creating our own software exception codes
#define MAKE_SF_EXCEPTION(sev, fac, code) \
   ((HRESULT) ( \
   /* Severity code    */  ( ((unsigned long)(sev))           << 30) |     \
   /* MS(0) or Cust(1) */  ( 1								  << 29) |     \
   /* Reserved(0)      */  ( 0                                << 28) |     \
   /* Facility code    */  ( (((unsigned long)(fac))&0x0fff)  << 16) |     \
   /* Exception code   */  ( (((unsigned long)(code))&0xFFFF) << 0)) )
#endif //MAKE_SF_EXCEPTION

#define ISSYSTEMEXCEPTION(hr)  ((((hr) >> 29) & 0x01) == 0 )
#define EXCEPTION_SEVERITY(hr) (((hr) >> 30)  & 0x3)
#define EXCEPTION_FACILITY(hr) (((hr) >> 16)  & 0x1fff)
#define EXCEPTION_CODE(hr)     ((hr)          & 0xFFFF)

///////////////////////////////////////////////////////////////////////////////
// Define the severity codes
//
#ifdef SEVERITY_SUCCESS
#undef SEVERITY_SUCCESS
#endif

#ifdef SEVERITY_ERROR
#undef SEVERITY_ERROR
#endif

#define	SEVERITY_SUCCESS    0	// Succeess
#define	SEVERITY_INFO		1	// Information
#define	SEVERITY_WARNING	2	// Warning
#define	SEVERITY_ERROR		3	// Error

///////////////////////////////////////////////////////////////////////////////
// Define the facility codes
#pragma region DefineFacility
#define FACILITY_OIM			1	// for Common error
#define FACILITY_ENGINE			2	// for Engine
//#define FACILITY_KERNEL			3	// for ClientAgent 
#define FACILITY_P2P			4	// for P2P
#define FACILITY_DB				5	// for SQLite3 Database
#define FACILITY_ZIP			6	// for Zip
#define FACILITY_IFILE			7	// for Internet file
#define FACILITY_UPDATER		8	// for Updater
#define FACILITY_CURL			9	// for LibCURL
#define FACILITY_HTTPS			10	// for HTTP status code
#define FACILITY_WEBAPPS		11	// for WebApps
#define FACILITY_MEETING        12  // for meeting
#define FACILITY_CLOUD_DISK     13  // for Cloud disk

#define FACILITY_USERS_BASE     128 // Users error facility base
#pragma endregion Define facility


///////////////////////////////////////////////////////////////////////////////
// Common error
#pragma region DefineCommonError
#ifndef S_OK
#  define S_OK					(0L)
#endif
#ifndef E_FAIL
#  define E_FAIL				(0x80004005L)
#endif
#ifndef E_NOTIMPL
#  define E_NOTIMPL				(0x80004001L)
#endif
#ifndef E_NOINTERFACE
#  define E_NOINTERFACE			(0x80004002L)
#endif
#ifndef E_POINTER
#  define E_POINTER				(0x80004003L)
#endif
#ifndef E_HANDLE
#  define E_HANDLE				(0x80070006L)
#endif
#ifndef E_INVALIDARG
#  define E_INVALIDARG			(0x80070057L)
#endif
#ifndef E_OUTOFMEMORY
#  define E_OUTOFMEMORY			(0x8007000EL)
#endif
#ifndef E_ACCESSDENIED
#  define E_ACCESSDENIED		(0x80070005L)
#endif

// No Error
#define ERR_NO_ERROR				S_OK
// Unspecified error
#define ERR_FAIL					E_FAIL
// Not implement						
#define ERR_NOT_IMPL				E_NOTIMPL
// No Interface						
#define ERR_NO_INTERFACE			E_NOINTERFACE
// Invalid Pointer
#define ERR_INVALID_POINTER			E_POINTER
// Invalid Parameter 
#define ERR_INVALID_PARAM			E_INVALIDARG
// Invalid handle
#define ERR_INVALID_HANDLE			E_HANDLE
// Out of memory
#define ERR_OUT_OF_MEMORY			E_OUTOFMEMORY
// Access denied error
#define ERR_ACCESS_DENIED			E_ACCESSDENIED
#pragma endregion

///////////////////////////////////////////////////////////////////////////////
#pragma region AppError
// oIM error
// [Auto Response]
#define ERR_MSGAUTO_TIP				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 1)
// Updater notice exit
#define ERR_EXIT_FOR_UPDATE			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 2)
// CTX
//#define ERR_APP_TITLE				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 3)
// Session
//#define ERR_SESSION				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 4)
//Start Receive
#define ERR_FILE_START_RECEIVE		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 5)
//Cancel Receive
#define ERR_FILE_CANCEL_RECEIVE		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 6)
//Receive Failed
#define ERR_FILE_RECEIVE_FAILED		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 7)
//Receive Canceled
#define ERR_FILE_RECEIVE_CANCELED	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 8)
//Waiting...
#define ERR_FILE_WAITING			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 9)
//Start Send
#define ERR_FILE_START_SEND			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 10)
//Cancel Send
#define ERR_FILE_CANCEL_SEND		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 11)
//Send Failed
#define ERR_FILE_SEND_FAILED		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 12)
//Send Canceled
#define ERR_FILE_SEND_CANCELED		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 13)
//Re Send
#define ERR_FILE_RE_SEND			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 14)
//Re Receive
#define ERR_FILE_RE_RECEIVE			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 15)
//File is occupied
#define ERR_FILE_OCCUPIED			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 16)


//Receipt1
#define ERR_RECEIPT1				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 17)
//Receipt2
//#define ERR_RECEIPT2				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 17)

#define ERR_FILE_OPEN				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 18)

#define ERR_1_TO_HUNFDRED			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 19)
#define ERR_SESSION_1_TO_10T		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 20)

#define ERR_CLOSE_WINDOW			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 21)

// Plugin not initialed
#define ERR_NO_INIT_PLUGIN			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 1)
// File not exist
#define ERR_FILE_NOT_EXIST		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 2)
// Open file failed
#define ERR_OPEN_FILE_FAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 3)
// File not conform to requirement
#define ERR_FILE_NOT_CONFORM		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 4)
// Language not exist
#define ERR_LANG_NOT_EXIST			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 5)
// Create window failed
#define ERR_CREATE_WND_FAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 6)
// Unknown error
#define ERR_UNKNOWN					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 7)
// Plugin initial failed
#define ERR_PLUGIN_INIT_FAIL		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 8)
// File is empty
#define ERR_EMPTY_FILE				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 9)
// File transfering,stop it?
#define ERR_STOP_FILETRAN			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 10)
// The count of members over the limit of %d
#define ERR_MEM_OVERLIMIT			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 11)
// TThe file is not exist.It maybe has been deleted or removed.
#define ERR_FILE_REMOVED_NOTICE		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 12)
// SMS members exceeds the limit of %u , please re select the members
#define ERR_SMS_MEM_OVEERLIMIT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 13)
// The name of sending file contains special characters:
#define ERR_SENDFILE_NAME_INVALID	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_OIM, 14)
//Exited
#define ERR_HAVE_EXIT_SESSION		MAKE_SF_EXCEPTION(SEVERITY_ERROR,FACILITY_OIM,15)
//image
#define ERR_MSG_IMAGE				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 21)
//emoji
#define ERR_MSG_EMOJI				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 22)
//voice
#define ERR_MSG_VOICE				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 23)
//video
#define ERR_MSG_VIDEO				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 24)
//p2p 
#define ERR_MSG_P2P					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 25)
//file
#define ERR_MSG_FILE				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 26)
//aoto reply
#define ERR_MSG_REPLY				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 27)

#define ERR_MSG_ALTER				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 28)
#define ERR_MSG_DING				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 29)
#define ERR_MSG_RECPITE				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 30)
#define ERR_MSG_MASTER				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 31)
#define ERR_MSG_DING_TIP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 32)
#define ERR_IMAGE_MR				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 33)
#define ERR_IMAGE_LAST				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 34)
#define ERR_SESSION_MOREMSG			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 35)
#define ERR_GROUP_LIMITED1			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 36)
#define ERR_GROUP_LIMITED2			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 37)
#define ERR_MSGMGRWND_NOSESSIONTIP	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_OIM, 38)

#pragma endregion App Error

///////////////////////////////////////////////////////////////////////////////
#pragma region EngineError
// Engine error
// Contacts has loaded
#define ERR_CONSTACT_LOADED			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 1)
// A partial message records have been from the session window removed, please see more in the message log.\n
#define ERR_PART_MSG_REMOVED 		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 2)   
// [Receipt has been sent]
#define ERR_RECEIPT_SENT 			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 3)   
// Click send read
#define ERR_CLICK_SENDREAD 			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 4)  
// Creating a group...
#define ERR_CREATIG_GROUP 			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 5)  
// Successful refresh the group
#define ERR_REFRESH_GROUP_OK 		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 6)  
// The group was created successfully
#define ERR_CREATE_GROUP_OK 	    MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 7)  
// Creating a group, waiting
#define ERR_CREATE_GROUP_WAITING	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 8)
//PIN Message
#define ERR_RECEIPT_SEND_PIN   		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 9)
//发送单聊回执显示文本,等待回复时文本
#define ERR_RECEIPT_SIGN_WAIT   	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 10) 
//发送单聊回执完成时，显示文本
#define ERR_RECEIPT_SIGN_END   		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 11) 
//发送群聊回执完成时，显示文本
#define ERR_RECEIPT_GROUP_END   	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 12) 

#define ERR_RECALL_MY_SHOW			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 13)
#define ERR_RECALL_SENDER_SHOW		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 14)

#define ERR_GROUPNOTEXIST			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 15)

#define ERR_VIRGROUPTITLETIP		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 16)
#define ERR_PROMPT					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 17)
#define ERR_DELALLMSGS				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 18)
#define ERR_DROPOUTGROUP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 19)
#define ERR_DISCARD_MSG				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 20)	// 丢弃消息（如别人的【打开红包】）

// 群变更信息(原来在皮肤中配置的，引擎无法访问皮肤（也可能窗口没有打开）)
#define ERR_YOU						MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 21)   // "您"
#define ERR_INVITE					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 22)   // "邀请了"
#define ERR_ADD_TO_GROUP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 23)   // "加入群"
#define ERR_FORCE					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 24)   // "将"
#define ERR_FORCEEXIT_FROM_GROUP	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 25)   // "移出了群"
#define ERR_EXIT_FROM_GROUP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 26)   // "退出了群"


// No initial engine
#define ERR_NO_INIT					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 1)
// Abort cmd
#define ERR_ABORT_CMD				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 2)
// Unknown command type or format
#define ERR_BAD_CMD					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 3)
// Invalid file id
#define ERR_INVALID_FID				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 4)
// You are not logged in, you cannot send a message 
#define ERR_NOT_LOGIN_MSG			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 5)
// You are offline, cannot send a message
#define ERR_OFFLINE_MSG				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 6)
// invalid user
#define ERR_INVALID_USER			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 7)
// Get message ACK overtime.The message maybe sent failure.
#define ERR_MSG_OVERTIME  			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 8)     
// TThe picture send failure.
#define ERR_PIC_SEND_FAIL			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 9)
// The group to create is not successful, failed to send message
#define ERR_GROUPFAILED_MSGERROR	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 10)
// Create a group failed
#define ERR_CREATE_GROUPFAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 11)
// Create a group timeout, please close the window and retry to create
#define ERR_CREATE_GROUPTIMEOUT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 12)

#define ERR_COLLECT_FILE_PROMPT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 13)
#define ERR_COLLECT_VOICE_PROMPT	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 14)
#define ERR_COLLECT_IMAGE_PROMPT	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 15)

#define ERR_WEBAPP_CONFIG_ERROR		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 16)
#define ERR_WEBAPP_ARET_ERROR		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 17)
#define ERR_COLLECT_FACE_KEY		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 18)

#define ERR_ROBOT_MSG_IMGTEXT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 19)
#define ERR_ROBOT_MSG_IMG			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 20)
#define ERR_ROBOT_MSG_VIDEO			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 21)
#define ERR_ROBOT_MSG_VOICE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 22)
#define ERR_COLLECT_SENDTIME		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 23)
#define ERR_RICHE_COLLECTTIME		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 24)
#define ERR_COLLECT_VIDEO_PROMPT	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_ENGINE, 25)

#pragma endregion Engine Error

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// P2P

///////////////////////////////////////////////////////////////////////////////
#pragma region DbError
// Database error convert helper macro
// Complie sql statement failed
#define ERR_COMPLIE_FAILED       MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, 1)   
// Get table failed
#define ERR_GET_TABLE_FAILED     MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, 2)   

#define SQLITE3_ERROR_BASE          (0x100) // Base value of convert the sqlite3 error to standard error code
#define ERR_SQLITE_ERROR			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 1  ) //SQL error or missing database
#define ERR_SQLITE_INTERNAL			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 2  ) //Internal logic error in SQLite
#define ERR_SQLITE_PERM				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 3  ) //Access permission denied
#define ERR_SQLITE_ABORT			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 4  ) //Callback routine requested an abort
#define ERR_SQLITE_BUSY				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 5  ) //The database file is locked
#define ERR_SQLITE_LOCKED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 6  ) //A table in the database is locked
#define ERR_SQLITE_NOMEM			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 7  ) //A malloc() failed
#define ERR_SQLITE_READONLY			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 8  ) //Attempt to write a readonly database
#define ERR_SQLITE_INTERRUPT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 9  ) //Operation terminated by sqlite3_interrup
#define ERR_SQLITE_IOERR			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 10 ) //Some kind of disk I/O error occurred
#define ERR_SQLITE_CORRUPT			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 11 ) //The database disk image is malformed
#define ERR_SQLITE_NOTFOUND			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 12 ) //Unknown opcode in sqlite3_file_control()
#define ERR_SQLITE_FULL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 13 ) //Insertion failed because database is full
#define ERR_SQLITE_CANTOPEN			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 14 ) //Unable to open the database file
#define ERR_SQLITE_PROTOCOL			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 15 ) //Database lock protocol error
#define ERR_SQLITE_EMPTY			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 16 ) //Database is empty
#define ERR_SQLITE_SCHEMA			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 17 ) //The database schema changed
#define ERR_SQLITE_TOOBIG			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 18 ) //String or BLOB exceeds size limit
#define ERR_SQLITE_CONSTRAINT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 19 ) //Abort due to constraint violation
#define ERR_SQLITE_MISMATCH			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 20 ) //Data type mismatch
#define ERR_SQLITE_MISUSE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 21 ) //Library used incorrectly
#define ERR_SQLITE_NOLFS			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 22 ) //Uses OS features not supported on host
#define ERR_SQLITE_AUTH				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 23 ) //Authorization denied
#define ERR_SQLITE_FORMAT			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 24 ) //Auxiliary database format error
#define ERR_SQLITE_RANGE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 25 ) //2nd parameter to sqlite3_bind out of range 
#define ERR_SQLITE_NOTADB			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 26 ) //File opened that is not a database file
#define ERR_SQLITE_NOTICE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 27 ) //Notifications from sqlite3_log()
#define ERR_SQLITE_WARNING			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 28 ) //Warnings from sqlite3_log()
#define ERR_SQLITE_ROW				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 100) //sqlite3_step() has another row ready
#define ERR_SQLITE_DONE				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + 101) //sqlite3_step() has finished executing


// SQLite3 error convert helper macro
#define ERR_DB_ERROR_(E)			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_DB, SQLITE3_ERROR_BASE + (E))   

#pragma endregion Db Error

///////////////////////////////////////////////////////////////////////////////
// Zip

///////////////////////////////////////////////////////////////////////////////
// Internet file

///////////////////////////////////////////////////////////////////////////////
// libcurl 
#define ERR_CURLE_UNSUPPORTED_PROTOCOL		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 1 )	// 1 
#define ERR_CURLE_FAILED_INIT				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 2 )	// 2 
#define ERR_CURLE_URL_MALFORMAT				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 3 )	// 3 
#define ERR_CURLE_NOT_BUILT_IN				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 4 )	// 4 - [was obsoleted in August 2007 for 7.17.0, reused in April 2011 for 7.21.5] 
#define ERR_CURLE_COULDNT_RESOLVE_PROXY		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 5 )	// 5 
#define ERR_CURLE_COULDNT_RESOLVE_HOST		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 6 )	// 6 
#define ERR_CURLE_COULDNT_CONNECT			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 7 )	// 7 
#define ERR_CURLE_FTP_WEIRD_SERVER_REPLY	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 8 )	// 8 
#define ERR_CURLE_REMOTE_ACCESS_DENIED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 9 )	// 9 a service was denied by the server due to lack of access - when login fails this is not returned. 
#define ERR_CURLE_FTP_ACCEPT_FAILED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 10)	// 10 - [was obsoleted in April 2006 for 7.15.4, reused in Dec 2011 for 7.24.0]
#define ERR_CURLE_FTP_WEIRD_PASS_REPLY		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 11)	// 11 
#define ERR_CURLE_FTP_ACCEPT_TIMEOUT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 12)	// 12 - timeout occurred accepting server [was obsoleted in August 2007 for 7.17.0, reused in Dec 2011 for 7.24.0]
#define ERR_CURLE_FTP_WEIRD_PASV_REPLY		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 13)	// 13 
#define ERR_CURLE_FTP_WEIRD_227_FORMAT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 14)	// 14 
#define ERR_CURLE_FTP_CANT_GET_HOST			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 15)	// 15 
#define ERR_CURLE_HTTP2						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 16)	// 16 - A problem in the http2 framing layer. [was obsoleted in August 2007 for 7.17.0, reused in July 2014 for 7.38.0] 
#define ERR_CURLE_FTP_COULDNT_SET_TYPE		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 17)	// 17 
#define ERR_CURLE_PARTIAL_FILE				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 18)	// 18 
#define ERR_CURLE_FTP_COULDNT_RETR_FILE		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 19)	// 19 
#define ERR_CURLE_OBSOLETE20				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 20)	// 20 - NOT USED 
#define ERR_CURLE_QUOTE_ERROR				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 21)	// 21 - quote command failure 
#define ERR_CURLE_HTTP_RETURNED_ERROR		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 22)	// 22 
#define ERR_CURLE_WRITE_ERROR				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 23)	// 23 
#define ERR_CURLE_OBSOLETE24				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 24)	// 24 - NOT USED 
#define ERR_CURLE_UPLOAD_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 25)	// 25 - failed upload "command" 
#define ERR_CURLE_READ_ERROR				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 26)	// 26 - couldn't open/read from file 
#define ERR_CURLE_OUT_OF_MEMORY				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 27)	// 27 Note: CURLE_OUT_OF_MEMORY may sometimes indicate a conversion error instead of a memory allocation error if CURL_DOES_CONVERSIONS is defined 
#define ERR_CURLE_OPERATION_TIMEDOUT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 28)	// 28 - the timeout time was reached 
#define ERR_CURLE_OBSOLETE29				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 29)	// 29 - NOT USED 
#define ERR_CURLE_FTP_PORT_FAILED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 30)	// 30 - FTP PORT operation failed 
#define ERR_CURLE_FTP_COULDNT_USE_REST		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 31)	// 31 - the REST command failed 
#define ERR_CURLE_OBSOLETE32				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 32)	// 32 - NOT USED 
#define ERR_CURLE_RANGE_ERROR				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 33)	// 33 - RANGE "command" didn't work 
#define ERR_CURLE_HTTP_POST_ERROR			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 34)	// 34 
#define ERR_CURLE_SSL_CONNECT_ERROR			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 35)	// 35 - wrong when connecting with SSL 
#define ERR_CURLE_BAD_DOWNLOAD_RESUME		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 36)	// 36 - couldn't resume download 
#define ERR_CURLE_FILE_COULDNT_READ_FILE	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 37)	// 37 
#define ERR_CURLE_LDAP_CANNOT_BIND			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 38)	// 38 
#define ERR_CURLE_LDAP_SEARCH_FAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 39)	// 39 
#define ERR_CURLE_OBSOLETE40				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 40)	// 40 - NOT USED 
#define ERR_CURLE_FUNCTION_NOT_FOUND		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 41)	// 41 
#define ERR_CURLE_ABORTED_BY_CALLBACK		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 42)	// 42 
#define ERR_CURLE_BAD_FUNCTION_ARGUMENT		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 43)	// 43 
#define ERR_CURLE_OBSOLETE44				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 44)	// 44 - NOT USED 
#define ERR_CURLE_INTERFACE_FAILED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 45)	// 45 - CURLOPT_INTERFACE failed 
#define ERR_CURLE_OBSOLETE46				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 46)	// 46 - NOT USED 
#define ERR_CURLE_TOO_MANY_REDIRECTS 		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 47)	// 47 - catch endless re-direct loops 
#define ERR_CURLE_UNKNOWN_OPTION			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 48)	// 48 - User specified an unknown option 
#define ERR_CURLE_TELNET_OPTION_SYNTAX 		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 49)	// 49 - Malformed telnet option 
#define ERR_CURLE_OBSOLETE50				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 50)	// 50 - NOT USED 
#define ERR_CURLE_PEER_FAILED_VERIFICATION	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 51)	// 51 - peer's certificate or fingerprint wasn't verified fine 
#define ERR_CURLE_GOT_NOTHING				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 52)	// 52 - when this is a specific error 
#define ERR_CURLE_SSL_ENGINE_NOTFOUND		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 53)	// 53 - SSL crypto engine not found 
#define ERR_CURLE_SSL_ENGINE_SETFAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 54)	// 54 - can not set SSL crypto engine as default 
#define ERR_CURLE_SEND_ERROR				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 55)	// 55 - failed sending network data 
#define ERR_CURLE_RECV_ERROR				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 56)	// 56 - failure in receiving network data 
#define ERR_CURLE_OBSOLETE57				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 57)	// 57 - NOT IN USE 
#define ERR_CURLE_SSL_CERTPROBLEM			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 58)	// 58 - problem with the local certificate 
#define ERR_CURLE_SSL_CIPHER				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 59)	// 59 - couldn't use specified cipher 
#define ERR_CURLE_SSL_CACERT				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 60)	// 60 - problem with the CA cert (path?) 
#define ERR_CURLE_BAD_CONTENT_ENCODING		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 61)	// 61 - Unrecognized/bad encoding 
#define ERR_CURLE_LDAP_INVALID_URL			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 62)	// 62 - Invalid LDAP URL 
#define ERR_CURLE_FILESIZE_EXCEEDED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 63)	// 63 - Maximum file size exceeded 
#define ERR_CURLE_USE_SSL_FAILED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 64)	// 64 - Requested FTP SSL level failed 
#define ERR_CURLE_SEND_FAIL_REWIND			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 65)	// 65 - Sending the data requires a rewind that failed 
#define ERR_CURLE_SSL_ENGINE_INITFAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 66)	// 66 - failed to initialise ENGINE 
#define ERR_CURLE_LOGIN_DENIED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 67)	// 67 - user, password or similar was not accepted and we failed to login 
#define ERR_CURLE_TFTP_NOTFOUND				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 68)	// 68 - file not found on server 
#define ERR_CURLE_TFTP_PERM					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 69)	// 69 - permission problem on server 
#define ERR_CURLE_REMOTE_DISK_FULL			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 70)	// 70 - out of disk space on server 
#define ERR_CURLE_TFTP_ILLEGAL				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 71)	// 71 - Illegal TFTP operation 
#define ERR_CURLE_TFTP_UNKNOWNID			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 72)	// 72 - Unknown transfer ID 
#define ERR_CURLE_REMOTE_FILE_EXISTS		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 73)	// 73 - File already exists 
#define ERR_CURLE_TFTP_NOSUCHUSER			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 74)	// 74 - No such user 
#define ERR_CURLE_CONV_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 75)	// 75 - conversion failed 
#define ERR_CURLE_CONV_REQD					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 76)	// 76 - caller must register conversion callbacks using curl_easy_setopt options CURLOPT_CONV_FROM_NETWORK_FUNCTION, CURLOPT_CONV_TO_NETWORK_FUNCTION, and CURLOPT_CONV_FROM_UTF8_FUNCTION 
#define ERR_CURLE_SSL_CACERT_BADFILE		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 77)	// 77 - could not load CACERT file, missing or wrong format 
#define ERR_CURLE_REMOTE_FILE_NOT_FOUND		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 78)	// 78 - remote file not found 
#define ERR_CURLE_SSH						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 79)	// 79 - error from the SSH layer, somewhat generic so the error message will be of interest when this has happened 
#define ERR_CURLE_SSL_SHUTDOWN_FAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 80)	// 80 - Failed to shut down the SSL connection 
#define ERR_CURLE_AGAIN						MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 81)	// 81 - socket is not ready for send/recv, wait till it's ready and try again (Added in 7.18.2) 
#define ERR_CURLE_SSL_CRL_BADFILE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 82)	// 82 - could not load CRL file, missing or wrong format (Added in 7.19.0) 
#define ERR_CURLE_SSL_ISSUER_ERROR			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 83)	// 83 - Issuer check failed.  (Added in 7.19.0) 
#define ERR_CURLE_FTP_PRET_FAILED			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 84)	// 84 - a PRET command failed 
#define ERR_CURLE_RTSP_CSEQ_ERROR			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 85)	// 85 - mismatch of RTSP CSeq numbers 
#define ERR_CURLE_RTSP_SESSION_ERROR		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 86)	// 86 - mismatch of RTSP Session Ids 
#define ERR_CURLE_FTP_BAD_FILE_LIST			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 87)	// 87 - unable to parse FTP file list 
#define ERR_CURLE_CHUNK_FAILED				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 88)	// 88 - chunk callback reported error 
#define ERR_CURLE_NO_CONNECTION_AVAILABLE	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 89)	// 89 - No connection available, the session will be queued 
#define ERR_CURLE_SSL_PINNEDPUBKEYNOTMATCH	MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, 90)	// 90 - specified pinned public key did not 

#define ERR_CURL_ERROR_(E)					MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CURL, (E))   

///////////////////////////////////////////////////////////////////////////////
// HTTP
#define ERR_HTTP_100				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 100)	// 客户端应当继续发送请求。这个临时响应是用来通知客户端它的部分请求已经被服务器接收，且仍未被拒绝。客户端应当继续发送请求的剩余部分，或者如果请求已经完成，忽略这个响应。服务器必须在请求完成后向客户端发送一个最终响应。
#define ERR_HTTP_101				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 101)	// 服务器已经理解了客户端的请求，并将通过Upgrade 消息头通知客户端采用不同的协议来完成这个请求。在发送完这个响应最后的空行后，服务器将会切换到在Upgrade 消息头中定义的那些协议。 　　只有在切换新的协议更有好处的时候才应该采取类似措施。例如，切换到新的HTTP 版本比旧版本更有优势，或者切换到一个实时且同步的协议以传送利用此类特性的资源。
#define ERR_HTTP_102				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 102)	// 由WebDAV（RFC 2518）扩展的状态码，代表处理将被继续执行。
#define ERR_HTTP_200				MAKE_SF_EXCEPTION(SEVERITY_INFO,  FACILITY_HTTPS, 200)	//o请求已成功，请求所希望的响应头或数据体将随此响应返回。
#define ERR_HTTP_201				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 201)	// 请求已经被实现，而且有一个新的资源已经依据请求的需要而建立，且其 URI 已经随Location 头信息返回。假如需要的资源无法及时建立的话，应当返回 '202 Accepted'。
#define ERR_HTTP_202				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 202)	// 服务器已接受请求，但尚未处理。正如它可能被拒绝一样，最终该请求可能会也可能不会被执行。在异步操作的场合下，没有比发送这个状态码更方便的做法了。 　　返回202状态码的响应的目的是允许服务器接受其他过程的请求（例如某个每天只执行一次的基于批处理的操作），而不必让客户端一直保持与服务器的连接直到批处理操作全部完成。在接受请求处理并返回202状态码的响应应当在返回的实体中包含一些指示处理当前状态的信息，以及指向处理状态监视器或状态预测的指针，以便用户能够估计操作是否已经完成。
#define ERR_HTTP_203				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 203)	// 服务器已成功处理了请求，但返回的实体头部元信息不是在原始服务器上有效的确定集合，而是来自本地或者第三方的拷贝。当前的信息可能是原始版本的子集或者超集。例如，包含资源的元数据可能导致原始服务器知道元信息的超级。使用此状态码不是必须的，而且只有在响应不使用此状态码便会返回200 OK的情况下才是合适的。
#define ERR_HTTP_204				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 204)	// 服务器成功处理了请求，但不需要返回任何实体内容，并且希望返回更新了的元信息。响应可能通过实体头部的形式，返回新的或更新后的元信息。如果存在这些头部信息，则应当与所请求的变量相呼应。 　　如果客户端是浏览器的话，那么用户浏览器应保留发送了该请求的页面，而不产生任何文档视图上的变化，即使按照规范新的或更新后的元信息应当被应用到用户浏览器活动视图中的文档。 　　由于204响应被禁止包含任何消息体，因此它始终以消息头后的第一个空行结尾。
#define ERR_HTTP_205				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 205)	// 服务器成功处理了请求，且没有返回任何内容。但是与204响应不同，返回此状态码的响应要求请求者重置文档视图。该响应主要是被用于接受用户输入后，立即重置表单，以便用户能够轻松地开始另一次输入。 　　与204响应一样，该响应也被禁止包含任何消息体，且以消息头后的第一个空行结束。
#define ERR_HTTP_206				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 206)	// 服务器已经成功处理了部分 GET 请求。类似于 FlashGet 或者迅雷这类的 HTTP 下载工具都是使用此类响应实现断点续传或者将一个大文档分解为多个下载段同时下载。 　　该请求必须包含 Range 头信息来指示客户端希望得到的内容范围，并且可能包含 If-Range 来作为请求条件。 　　响应必须包含如下的头部域： 　　Content-Range 用以指示本次响应中返回的内容的范围；如果是 Content-Type 为 multipart/byteranges 的多段下载，则每一 multipart 段中都应包含 Content-Range 域用以指示本段的内容范围。假如响应中包含 Content-Length，那么它的数值必须匹配它返回的内容范围的真实字节数。 　　Date 　　ETag 和/或 Content-Location，假如同样的请求本应该返回200响应。 　　Expires, Cache-Control，和/或 Vary，假如其值可能与之前相同变量的其他响应对应的值不同的话。 　　假如本响应请求使用了 If-Range 强缓存验证，那么本次响应不应该包含其他实体头；假如本响应的请求使用了 If-Range 弱缓存验证，那么本次响应禁止包含其他实体头；这避免了缓存的实体内容和更新了的实体头信息之间的不一致。否则，本响应就应当包含所有本应该返回200响应中应当返回的所有实体头部域。 　　假如 ETag 或 Last-Modified 头部不能精确匹配的话，则客户端缓存应禁止将206响应返回的内容与之前任何缓存过的内容组合在一起。 　　任何不支持 Range 以及 Content-Range 头的缓存都禁止缓存206响应返回的内容。
#define ERR_HTTP_207				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 207)	// 由WebDAV(RFC 2518)扩展的状态码，代表之后的消息体将是一个XML消息，并且可能依照之前子请求数量的不同，包含一系列独立的响应代码。
#define ERR_HTTP_300				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 300)	// 被请求的资源有一系列可供选择的回馈信息，每个都有自己特定的地址和浏览器驱动的商议信息。用户或浏览器能够自行选择一个首选的地址进行重定向。 　　除非这是一个 HEAD 请求，否则该响应应当包括一个资源特性及地址的列表的实体，以便用户或浏览器从中选择最合适的重定向地址。这个实体的格式由 Content-Type 定义的格式所决定。浏览器可能根据响应的格式以及浏览器自身能力，自动作出最合适的选择。当然，RFC 2616规范并没有规定这样的自动选择该如何进行。 　　如果服务器本身已经有了首选的回馈选择，那么在 Location 中应当指明这个回馈的 URI；浏览器可能会将这个 Location 值作为自动重定向的地址。此外，除非额外指定，否则这个响应也是可缓存的。
#define ERR_HTTP_301				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 301)	// 被请求的资源已永久移动到新位置，并且将来任何对此资源的引用都应该使用本响应返回的若干个 URI 之一。如果可能，拥有链接编辑功能的客户端应当自动把请求的地址修改为从服务器反馈回来的地址。除非额外指定，否则这个响应也是可缓存的。 　　新的永久性的 URI 应当在响应的 Location 域中返回。除非这是一个 HEAD 请求，否则响应的实体中应当包含指向新的 URI 的超链接及简短说明。 　　如果这不是一个 GET 或者 HEAD 请求，因此浏览器禁止自动进行重定向，除非得到用户的确认，因为请求的条件可能因此发生变化。 　　注意：对于某些使用 HTTP/1.0 协议的浏览器，当它们发送的 POST 请求得到了一个301响应的话，接下来的重定向请求将会变成 GET 方式。
#define ERR_HTTP_302				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 302)	// 请求的资源现在临时从不同的 URI 响应请求。由于这样的重定向是临时的，客户端应当继续向原有地址发送以后的请求。只有在Cache-Control或Expires中进行了指定的情况下，这个响应才是可缓存的。 　　新的临时性的 URI 应当在响应的 Location 域中返回。除非这是一个 HEAD 请求，否则响应的实体中应当包含指向新的 URI 的超链接及简短说明。 　　如果这不是一个 GET 或者 HEAD 请求，那么浏览器禁止自动进行重定向，除非得到用户的确认，因为请求的条件可能因此发生变化。 　　注意：虽然RFC 1945和RFC 2068规范不允许客户端在重定向时改变请求的方法，但是很多现存的浏览器将302响应视作为303响应，并且使用 GET 方式访问在 Location 中规定的 URI，而无视原先请求的方法。状态码303和307被添加了进来，用以明确服务器期待客户端进行何种反应。
#define ERR_HTTP_303				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 303)	// 对应当前请求的响应可以在另一个 URI 上被找到，而且客户端应当采用 GET 的方式访问那个资源。这个方法的存在主要是为了允许由脚本激活的POST请求输出重定向到一个新的资源。这个新的 URI 不是原始资源的替代引用。同时，303响应禁止被缓存。当然，第二个请求（重定向）可能被缓存。 　　新的 URI 应当在响应的 Location 域中返回。除非这是一个 HEAD 请求，否则响应的实体中应当包含指向新的 URI 的超链接及简短说明。 　　注意：许多 HTTP/1.1 版以前的 浏览器不能正确理解303状态。如果需要考虑与这些浏览器之间的互动，302状态码应该可以胜任，因为大多数的浏览器处理302响应时的方式恰恰就是上述规范要求客户端处理303响应时应当做的。
#define ERR_HTTP_304				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 304)	// 如果客户端发送了一个带条件的 GET 请求且该请求已被允许，而文档的内容（自上次访问以来或者根据请求的条件）并没有改变，则服务器应当返回这个状态码。304响应禁止包含消息体，因此始终以消息头后的第一个空行结尾。 　　该响应必须包含以下的头信息： 　　Date，除非这个服务器没有时钟。假如没有时钟的服务器也遵守这些规则，那么代理服务器以及客户端可以自行将 Date 字段添加到接收到的响应头中去（正如RFC 2068中规定的一样），缓存机制将会正常工作。 　　ETag 和/或 Content-Location，假如同样的请求本应返回200响应。 　　Expires, Cache-Control，和/或Vary，假如其值可能与之前相同变量的其他响应对应的值不同的话。 　　假如本响应请求使用了强缓存验证，那么本次响应不应该包含其他实体头；否则（例如，某个带条件的 GET 请求使用了弱缓存验证），本次响应禁止包含其他实体头；这避免了缓存了的实体内容和更新了的实体头信息之间的不一致。 　　假如某个304响应指明了当前某个实体没有缓存，那么缓存系统必须忽视这个响应，并且重复发送不包含限制条件的请求。 　　假如接收到一个要求更新某个缓存条目的304响应，那么缓存系统必须更新整个条目以反映所有在响应中被更新的字段的值。
#define ERR_HTTP_305				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 305)	// 被请求的资源必须通过指定的代理才能被访问。Location 域中将给出指定的代理所在的 URI 信息，接收者需要重复发送一个单独的请求，通过这个代理才能访问相应资源。只有原始服务器才能建立305响应。 　　注意：RFC 2068中没有明确305响应是为了重定向一个单独的请求，而且只能被原始服务器建立。忽视这些限制可能导致严重的安全后果。
#define ERR_HTTP_306				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 306)	// 在最新版的规范中，306状态码已经不再被使用。
#define ERR_HTTP_307				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 307)	// 请求的资源现在临时从不同的URI 响应请求。由于这样的重定向是临时的，客户端应当继续向原有地址发送以后的请求。只有在Cache-Control或Expires中进行了指定的情况下，这个响应才是可缓存的。 　　新的临时性的URI 应当在响应的 Location 域中返回。除非这是一个HEAD 请求，否则响应的实体中应当包含指向新的URI 的超链接及简短说明。因为部分浏览器不能识别307响应，因此需要添加上述必要信息以便用户能够理解并向新的 URI 发出访问请求。 　　如果这不是一个GET 或者 HEAD 请求，那么浏览器禁止自动进行重定向，除非得到用户的确认，因为请求的条件可能因此发生变化。
#define ERR_HTTP_400				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 400)	// 1、语义有误，当前请求无法被服务器理解。除非进行修改，否则客户端不应该重复提交这个请求。 　　2、请求参数有误。
#define ERR_HTTP_401				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 401)	// 当前请求需要用户验证。该响应必须包含一个适用于被请求资源的 WWW-Authenticate 信息头用以询问用户信息。客户端可以重复提交一个包含恰当的 Authorization 头信息的请求。如果当前请求已经包含了 Authorization 证书，那么401响应代表着服务器验证已经拒绝了那些证书。如果401响应包含了与前一个响应相同的身份验证询问，且浏览器已经至少尝试了一次验证，那么浏览器应当向用户展示响应中包含的实体信息，因为这个实体信息中可能包含了相关诊断信息。参见RFC 2617。
#define ERR_HTTP_402				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 402)	// 该状态码是为了将来可能的需求而预留的。
#define ERR_HTTP_403				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 403)	// 服务器已经理解请求，但是拒绝执行它。与401响应不同的是，身份验证并不能提供任何帮助，而且这个请求也不应该被重复提交。如果这不是一个 HEAD 请求，而且服务器希望能够讲清楚为何请求不能被执行，那么就应该在实体内描述拒绝的原因。当然服务器也可以返回一个404响应，假如它不希望让客户端获得任何信息。
#define ERR_HTTP_404				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 404)	// 请求失败，请求所希望得到的资源未被在服务器上发现。没有信息能够告诉用户这个状况到底是暂时的还是永久的。假如服务器知道情况的话，应当使用410状态码来告知旧资源因为某些内部的配置机制问题，已经永久的不可用，而且没有任何可以跳转的地址。404这个状态码被广泛应用于当服务器不想揭示到底为何请求被拒绝或者没有其他适合的响应可用的情况下。
#define ERR_HTTP_405				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 405)	// 请求行中指定的请求方法不能被用于请求相应的资源。该响应必须返回一个Allow 头信息用以表示出当前资源能够接受的请求方法的列表。 　　鉴于 PUT，DELETE 方法会对服务器上的资源进行写操作，因而绝大部分的网页服务器都不支持或者在默认配置下不允许上述请求方法，对于此类请求均会返回405错误。
#define ERR_HTTP_406				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 406)	// 请求的资源的内容特性无法满足请求头中的条件，因而无法生成响应实体。 　　除非这是一个 HEAD 请求，否则该响应就应当返回一个包含可以让用户或者浏览器从中选择最合适的实体特性以及地址列表的实体。实体的格式由 Content-Type 头中定义的媒体类型决定。浏览器可以根据格式及自身能力自行作出最佳选择。但是，规范中并没有定义任何作出此类自动选择的标准。
#define ERR_HTTP_407				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 407)	// 　与401响应类似，只不过客户端必须在代理服务器上进行身份验证。代理服务器必须返回一个 Proxy-Authenticate 用以进行身份询问。客户端可以返回一个 Proxy-Authorization 信息头用以验证。参见RFC 2617。
#define ERR_HTTP_408				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 408)	// 请求超时。客户端没有在服务器预备等待的时间内完成一个请求的发送。客户端可以随时再次提交这一请求而无需进行任何更改。
#define ERR_HTTP_409				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 409)	// 由于和被请求的资源的当前状态之间存在冲突，请求无法完成。这个代码只允许用在这样的情况下才能被使用：用户被认为能够解决冲突，并且会重新提交新的请求。该响应应当包含足够的信息以便用户发现冲突的源头。 　　冲突通常发生于对 PUT 请求的处理中。例如，在采用版本检查的环境下，某次 PUT 提交的对特定资源的修改请求所附带的版本信息与之前的某个（第三方）请求向冲突，那么此时服务器就应该返回一个409错误，告知用户请求无法完成。此时，响应实体中很可能会包含两个冲突版本之间的差异比较，以便用户重新提交归并以后的新版本。
#define ERR_HTTP_410				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 410)	// 被请求的资源在服务器上已经不再可用，而且没有任何已知的转发地址。这样的状况应当被认为是永久性的。如果可能，拥有链接编辑功能的客户端应当在获得用户许可后删除所有指向这个地址的引用。如果服务器不知道或者无法确定这个状况是否是永久的，那么就应该使用404状态码。除非额外说明，否则这个响应是可缓存的。 　　410响应的目的主要是帮助网站管理员维护网站，通知用户该资源已经不再可用，并且服务器拥有者希望所有指向这个资源的远端连接也被删除。这类事件在限时、增值服务中很普遍。同样，410响应也被用于通知客户端在当前服务器站点上，原本属于某个个人的资源已经不再可用。当然，是否需要把所有永久不可用的资源标记为'410 Gone'，以及是否需要保持此标记多长时间，完全取决于服务器拥有者。
#define ERR_HTTP_411				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 411)	// 服务器拒绝在没有定义 Content-Length 头的情况下接受请求。在添加了表明请求消息体长度的有效 Content-Length 头之后，客户端可以再次提交该请求。
#define ERR_HTTP_412				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 412)	// 服务器在验证在请求的头字段中给出先决条件时，没能满足其中的一个或多个。这个状态码允许客户端在获取资源时在请求的元信息（请求头字段数据）中设置先决条件，以此避免该请求方法被应用到其希望的内容以外的资源上。
#define ERR_HTTP_413				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 413)	// 服务器拒绝处理当前请求，因为该请求提交的实体数据大小超过了服务器愿意或者能够处理的范围。此种情况下，服务器可以关闭连接以免客户端继续发送此请求。 　　如果这个状况是临时的，服务器应当返回一个 Retry-After 的响应头，以告知客户端可以在多少时间以后重新尝试。
#define ERR_HTTP_414				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 414)	// 请求的URI 长度超过了服务器能够解释的长度，因此服务器拒绝对该请求提供服务。这比较少见，通常的情况包括： 　　本应使用POST方法的表单提交变成了GET方法，导致查询字符串（Query String）过长。 　　重定向URI “黑洞”，例如每次重定向把旧的 URI 作为新的 URI 的一部分，导致在若干次重定向后 URI 超长。 　　客户端正在尝试利用某些服务器中存在的安全漏洞攻击服务器。这类服务器使用固定长度的缓冲读取或操作请求的 URI，当 GET 后的参数超过某个数值后，可能会产生缓冲区溢出，导致任意代码被执行[1]。没有此类漏洞的服务器，应当返回414状态码。
#define ERR_HTTP_415				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 415)	// 对于当前请求的方法和所请求的资源，请求中提交的实体并不是服务器中所支持的格式，因此请求被拒绝。
#define ERR_HTTP_416				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 416)	// 如果请求中包含了 Range 请求头，并且 Range 中指定的任何数据范围都与当前资源的可用范围不重合，同时请求中又没有定义 If-Range 请求头，那么服务器就应当返回416状态码。 　　假如 Range 使用的是字节范围，那么这种情况就是指请求指定的所有数据范围的首字节位置都超过了当前资源的长度。服务器也应当在返回416状态码的同时，包含一个 Content-Range 实体头，用以指明当前资源的长度。这个响应也被禁止使用 multipart/byteranges 作为其 Content-Type。
#define ERR_HTTP_417				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 417)	// 在请求头 Expect 中指定的预期内容无法被服务器满足，或者这个服务器是一个代理服务器，它有明显的证据证明在当前路由的下一个节点上，Expect 的内容无法被满足。
#define ERR_HTTP_421				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 421)	// 从当前客户端所在的IP地址到服务器的连接数超过了服务器许可的最大范围。通常，这里的IP地址指的是从服务器上看到的客户端地址（比如用户的网关或者代理服务器地址）。在这种情况下，连接数的计算可能涉及到不止一个终端用户。
#define ERR_HTTP_422				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 422)	// 从当前客户端所在的IP地址到服务器的连接数超过了服务器许可的最大范围。通常，这里的IP地址指的是从服务器上看到的客户端地址（比如用户的网关或者代理服务器地址）。在这种情况下，连接数的计算可能涉及到不止一个终端用户。
#define ERR_HTTP_422				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 422)	// 请求格式正确，但是由于含有语义错误，无法响应。（RFC 4918 WebDAV）423 Locked 　　当前资源被锁定。（RFC 4918 WebDAV）
#define ERR_HTTP_424				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 424)	// 由于之前的某个请求发生的错误，导致当前请求失败，例如 PROPPATCH。（RFC 4918 WebDAV）
#define ERR_HTTP_425				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 425)	// 在WebDav Advanced Collections 草案中定义，但是未出现在《WebDAV 顺序集协议》（RFC 3658）中。
#define ERR_HTTP_426				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 426)	// 客户端应当切换到TLS/1.0。（RFC 2817）
#define ERR_HTTP_449				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 449)	// 由微软扩展，代表请求应当在执行完适当的操作后进行重试。
#define ERR_HTTP_500				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 500)	// 服务器遇到了一个未曾预料的状况，导致了它无法完成对请求的处理。一般来说，这个问题都会在服务器的程序码出错时出现。
#define ERR_HTTP_501				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 501)	// 服务器不支持当前请求所需要的某个功能。当服务器无法识别请求的方法，并且无法支持其对任何资源的请求。
#define ERR_HTTP_502				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 502)	// 作为网关或者代理工作的服务器尝试执行请求时，从上游服务器接收到无效的响应。
#define ERR_HTTP_503				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 503)	// 由于临时的服务器维护或者过载，服务器当前无法处理请求。这个状况是临时的，并且将在一段时间以后恢复。如果能够预计延迟时间，那么响应中可以包含一个 Retry-After 头用以标明这个延迟时间。如果没有给出这个 Retry-After 信息，那么客户端应当以处理500响应的方式处理它。 　　注意：503状态码的存在并不意味着服务器在过载的时候必须使用它。某些服务器只不过是希望拒绝客户端的连接。
#define ERR_HTTP_504				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 504)	// 作为网关或者代理工作的服务器尝试执行请求时，未能及时从上游服务器（URI标识出的服务器，例如HTTP、FTP、LDAP）或者辅助服务器（例如DNS）收到响应。 　　注意：某些代理服务器在DNS查询超时时会返回400或者500错误
#define ERR_HTTP_505				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 505)	// 服务器不支持，或者拒绝支持在请求中使用的 HTTP 版本。这暗示着服务器不能或不愿使用与客户端相同的版本。响应中应当包含一个描述了为何版本不被支持以及服务器支持哪些协议的实体。
#define ERR_HTTP_506				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 506)	// 由《透明内容协商协议》（RFC 2295）扩展，代表服务器存在内部配置错误：被请求的协商变元资源被配置为在透明内容协商中使用自己，因此在一个协商处理中不是一个合适的重点。
#define ERR_HTTP_507				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 507)	// 服务器无法存储完成请求所必须的内容。这个状况被认为是临时的。WebDAV (RFC 4918)
#define ERR_HTTP_509				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 509)	// 服务器达到带宽限制。这不是一个官方的状态码，但是仍被广泛使用。
#define ERR_HTTP_510				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 510)	// 获取资源所需要的策略并没有没满足。（RFC 2774）

#define ERR_HTTP_ERROR_(E)			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, (E))   

#define ERR_WEBAPPS_DISABLE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_WEBAPPS, 200)   
#define ERR_WEBAPPS_ERROR_(E)		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_WEBAPPS, (E))   

// Cloud disk
#define ERR_CLOUD_UPLOAD_SUCCEEDED	MAKE_SF_EXCEPTION(SEVERITY_INFO,  FACILITY_CLOUD_DISK, 1)	// 保存到云盘成功
#define ERR_CLOUD_UPLOADING			MAKE_SF_EXCEPTION(SEVERITY_INFO,  FACILITY_CLOUD_DISK, 2)	// 正在转存云盘...

#define ERR_CLOUD_UPLOAD_FAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CLOUD_DISK, 1)	// 保存到云盘失败


//
#define ERROR_BUFFER_SIZE	( 2048 )	// Format error information buffer size, See FormatError API

#endif	// __OIM_ERRORS_2013_11_29_BY_YFGZ__

