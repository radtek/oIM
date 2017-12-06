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
//���͵��Ļ�ִ��ʾ�ı�,�ȴ��ظ�ʱ�ı�
#define ERR_RECEIPT_SIGN_WAIT   	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 10) 
//���͵��Ļ�ִ���ʱ����ʾ�ı�
#define ERR_RECEIPT_SIGN_END   		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 11) 
//����Ⱥ�Ļ�ִ���ʱ����ʾ�ı�
#define ERR_RECEIPT_GROUP_END   	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 12) 

#define ERR_RECALL_MY_SHOW			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 13)
#define ERR_RECALL_SENDER_SHOW		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 14)

#define ERR_GROUPNOTEXIST			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 15)

#define ERR_VIRGROUPTITLETIP		MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 16)
#define ERR_PROMPT					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 17)
#define ERR_DELALLMSGS				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 18)
#define ERR_DROPOUTGROUP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 19)
#define ERR_DISCARD_MSG				MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 20)	// ������Ϣ������˵ġ��򿪺������

// Ⱥ�����Ϣ(ԭ����Ƥ�������õģ������޷�����Ƥ����Ҳ���ܴ���û�д򿪣�)
#define ERR_YOU						MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 21)   // "��"
#define ERR_INVITE					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 22)   // "������"
#define ERR_ADD_TO_GROUP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 23)   // "����Ⱥ"
#define ERR_FORCE					MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 24)   // "��"
#define ERR_FORCEEXIT_FROM_GROUP	MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 25)   // "�Ƴ���Ⱥ"
#define ERR_EXIT_FROM_GROUP			MAKE_SF_EXCEPTION(SEVERITY_INFO, FACILITY_ENGINE, 26)   // "�˳���Ⱥ"


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
#define ERR_HTTP_100				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 100)	// �ͻ���Ӧ�������������������ʱ��Ӧ������֪ͨ�ͻ������Ĳ��������Ѿ������������գ�����δ���ܾ����ͻ���Ӧ���������������ʣ�ಿ�֣�������������Ѿ���ɣ����������Ӧ��������������������ɺ���ͻ��˷���һ��������Ӧ��
#define ERR_HTTP_101				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 101)	// �������Ѿ�����˿ͻ��˵����󣬲���ͨ��Upgrade ��Ϣͷ֪ͨ�ͻ��˲��ò�ͬ��Э���������������ڷ����������Ӧ���Ŀ��к󣬷����������л�����Upgrade ��Ϣͷ�ж������ЩЭ�顣 ����ֻ�����л��µ�Э����кô���ʱ���Ӧ�ò�ȡ���ƴ�ʩ�����磬�л����µ�HTTP �汾�Ⱦɰ汾�������ƣ������л���һ��ʵʱ��ͬ����Э���Դ������ô������Ե���Դ��
#define ERR_HTTP_102				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 102)	// ��WebDAV��RFC 2518����չ��״̬�룬������������ִ�С�
#define ERR_HTTP_200				MAKE_SF_EXCEPTION(SEVERITY_INFO,  FACILITY_HTTPS, 200)	//o�����ѳɹ���������ϣ������Ӧͷ�������彫�����Ӧ���ء�
#define ERR_HTTP_201				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 201)	// �����Ѿ���ʵ�֣�������һ���µ���Դ�Ѿ������������Ҫ������������ URI �Ѿ���Location ͷ��Ϣ���ء�������Ҫ����Դ�޷���ʱ�����Ļ���Ӧ������ '202 Accepted'��
#define ERR_HTTP_202				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 202)	// �������ѽ������󣬵���δ�������������ܱ��ܾ�һ�������ո�������ܻ�Ҳ���ܲ��ᱻִ�С����첽�����ĳ����£�û�бȷ������״̬�������������ˡ� ��������202״̬�����Ӧ��Ŀ������������������������̵���������ĳ��ÿ��ִֻ��һ�εĻ���������Ĳ��������������ÿͻ���һֱ�����������������ֱ�����������ȫ����ɡ��ڽ�������������202״̬�����ӦӦ���ڷ��ص�ʵ���а���һЩָʾ����ǰ״̬����Ϣ���Լ�ָ����״̬��������״̬Ԥ���ָ�룬�Ա��û��ܹ����Ʋ����Ƿ��Ѿ���ɡ�
#define ERR_HTTP_203				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 203)	// �������ѳɹ����������󣬵����ص�ʵ��ͷ��Ԫ��Ϣ������ԭʼ����������Ч��ȷ�����ϣ��������Ա��ػ��ߵ������Ŀ�������ǰ����Ϣ������ԭʼ�汾���Ӽ����߳��������磬������Դ��Ԫ���ݿ��ܵ���ԭʼ������֪��Ԫ��Ϣ�ĳ�����ʹ�ô�״̬�벻�Ǳ���ģ�����ֻ������Ӧ��ʹ�ô�״̬���᷵��200 OK������²��Ǻ��ʵġ�
#define ERR_HTTP_204				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 204)	// �������ɹ����������󣬵�����Ҫ�����κ�ʵ�����ݣ�����ϣ�����ظ����˵�Ԫ��Ϣ����Ӧ����ͨ��ʵ��ͷ������ʽ�������µĻ���º��Ԫ��Ϣ�����������Щͷ����Ϣ����Ӧ����������ı������Ӧ�� ��������ͻ�����������Ļ�����ô�û������Ӧ���������˸������ҳ�棬���������κ��ĵ���ͼ�ϵı仯����ʹ���չ淶�µĻ���º��Ԫ��ϢӦ����Ӧ�õ��û���������ͼ�е��ĵ��� ��������204��Ӧ����ֹ�����κ���Ϣ�壬�����ʼ������Ϣͷ��ĵ�һ�����н�β��
#define ERR_HTTP_205				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 205)	// �������ɹ�������������û�з����κ����ݡ�������204��Ӧ��ͬ�����ش�״̬�����ӦҪ�������������ĵ���ͼ������Ӧ��Ҫ�Ǳ����ڽ����û�������������ñ����Ա��û��ܹ����ɵؿ�ʼ��һ�����롣 ������204��Ӧһ��������ӦҲ����ֹ�����κ���Ϣ�壬������Ϣͷ��ĵ�һ�����н�����
#define ERR_HTTP_206				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 206)	// �������Ѿ��ɹ������˲��� GET ���������� FlashGet ����Ѹ������� HTTP ���ع��߶���ʹ�ô�����Ӧʵ�ֶϵ��������߽�һ�����ĵ��ֽ�Ϊ������ض�ͬʱ���ء� ���������������� Range ͷ��Ϣ��ָʾ�ͻ���ϣ���õ������ݷ�Χ�����ҿ��ܰ��� If-Range ����Ϊ���������� ������Ӧ����������µ�ͷ���� ����Content-Range ����ָʾ������Ӧ�з��ص����ݵķ�Χ������� Content-Type Ϊ multipart/byteranges �Ķ�����أ���ÿһ multipart ���ж�Ӧ���� Content-Range ������ָʾ���ε����ݷ�Χ��������Ӧ�а��� Content-Length����ô������ֵ����ƥ�������ص����ݷ�Χ����ʵ�ֽ����� ����Date ����ETag ��/�� Content-Location������ͬ��������Ӧ�÷���200��Ӧ�� ����Expires, Cache-Control����/�� Vary��������ֵ������֮ǰ��ͬ������������Ӧ��Ӧ��ֵ��ͬ�Ļ��� �������籾��Ӧ����ʹ���� If-Range ǿ������֤����ô������Ӧ��Ӧ�ð�������ʵ��ͷ�����籾��Ӧ������ʹ���� If-Range ��������֤����ô������Ӧ��ֹ��������ʵ��ͷ��������˻����ʵ�����ݺ͸����˵�ʵ��ͷ��Ϣ֮��Ĳ�һ�¡����򣬱���Ӧ��Ӧ���������б�Ӧ�÷���200��Ӧ��Ӧ�����ص�����ʵ��ͷ���� �������� ETag �� Last-Modified ͷ�����ܾ�ȷƥ��Ļ�����ͻ��˻���Ӧ��ֹ��206��Ӧ���ص�������֮ǰ�κλ���������������һ�� �����κβ�֧�� Range �Լ� Content-Range ͷ�Ļ��涼��ֹ����206��Ӧ���ص����ݡ�
#define ERR_HTTP_207				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 207)	// ��WebDAV(RFC 2518)��չ��״̬�룬����֮�����Ϣ�彫��һ��XML��Ϣ�����ҿ�������֮ǰ�����������Ĳ�ͬ������һϵ�ж�������Ӧ���롣
#define ERR_HTTP_300				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 300)	// ���������Դ��һϵ�пɹ�ѡ��Ļ�����Ϣ��ÿ�������Լ��ض��ĵ�ַ�������������������Ϣ���û���������ܹ�����ѡ��һ����ѡ�ĵ�ַ�����ض��� ������������һ�� HEAD ���󣬷������ӦӦ������һ����Դ���Լ���ַ���б��ʵ�壬�Ա��û������������ѡ������ʵ��ض����ַ�����ʵ��ĸ�ʽ�� Content-Type ����ĸ�ʽ����������������ܸ�����Ӧ�ĸ�ʽ�Լ�����������������Զ���������ʵ�ѡ�񡣵�Ȼ��RFC 2616�淶��û�й涨�������Զ�ѡ�����ν��С� ������������������Ѿ�������ѡ�Ļ���ѡ����ô�� Location ��Ӧ��ָ����������� URI����������ܻὫ��� Location ֵ��Ϊ�Զ��ض���ĵ�ַ�����⣬���Ƕ���ָ�������������ӦҲ�ǿɻ���ġ�
#define ERR_HTTP_301				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 301)	// ���������Դ�������ƶ�����λ�ã����ҽ����κζԴ���Դ�����ö�Ӧ��ʹ�ñ���Ӧ���ص����ɸ� URI ֮һ��������ܣ�ӵ�����ӱ༭���ܵĿͻ���Ӧ���Զ�������ĵ�ַ�޸�Ϊ�ӷ��������������ĵ�ַ�����Ƕ���ָ�������������ӦҲ�ǿɻ���ġ� �����µ������Ե� URI Ӧ������Ӧ�� Location ���з��ء���������һ�� HEAD ���󣬷�����Ӧ��ʵ����Ӧ������ָ���µ� URI �ĳ����Ӽ����˵���� ��������ⲻ��һ�� GET ���� HEAD ��������������ֹ�Զ������ض��򣬳��ǵõ��û���ȷ�ϣ���Ϊ���������������˷����仯�� ����ע�⣺����ĳЩʹ�� HTTP/1.0 Э���������������Ƿ��͵� POST ����õ���һ��301��Ӧ�Ļ������������ض������󽫻��� GET ��ʽ��
#define ERR_HTTP_302				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 302)	// �������Դ������ʱ�Ӳ�ͬ�� URI ��Ӧ���������������ض�������ʱ�ģ��ͻ���Ӧ��������ԭ�е�ַ�����Ժ������ֻ����Cache-Control��Expires�н�����ָ��������£������Ӧ���ǿɻ���ġ� �����µ���ʱ�Ե� URI Ӧ������Ӧ�� Location ���з��ء���������һ�� HEAD ���󣬷�����Ӧ��ʵ����Ӧ������ָ���µ� URI �ĳ����Ӽ����˵���� ��������ⲻ��һ�� GET ���� HEAD ������ô�������ֹ�Զ������ض��򣬳��ǵõ��û���ȷ�ϣ���Ϊ���������������˷����仯�� ����ע�⣺��ȻRFC 1945��RFC 2068�淶������ͻ������ض���ʱ�ı�����ķ��������Ǻܶ��ִ���������302��Ӧ����Ϊ303��Ӧ������ʹ�� GET ��ʽ������ Location �й涨�� URI��������ԭ������ķ�����״̬��303��307������˽�����������ȷ�������ڴ��ͻ��˽��к��ַ�Ӧ��
#define ERR_HTTP_303				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 303)	// ��Ӧ��ǰ�������Ӧ��������һ�� URI �ϱ��ҵ������ҿͻ���Ӧ������ GET �ķ�ʽ�����Ǹ���Դ����������Ĵ�����Ҫ��Ϊ�������ɽű������POST��������ض���һ���µ���Դ������µ� URI ����ԭʼ��Դ��������á�ͬʱ��303��Ӧ��ֹ�����档��Ȼ���ڶ��������ض��򣩿��ܱ����档 �����µ� URI Ӧ������Ӧ�� Location ���з��ء���������һ�� HEAD ���󣬷�����Ӧ��ʵ����Ӧ������ָ���µ� URI �ĳ����Ӽ����˵���� ����ע�⣺��� HTTP/1.1 ����ǰ�� �����������ȷ���303״̬�������Ҫ��������Щ�����֮��Ļ�����302״̬��Ӧ�ÿ���ʤ�Σ���Ϊ����������������302��Ӧʱ�ķ�ʽǡǡ���������淶Ҫ��ͻ��˴���303��ӦʱӦ�����ġ�
#define ERR_HTTP_304				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 304)	// ����ͻ��˷�����һ���������� GET �����Ҹ������ѱ��������ĵ������ݣ����ϴη����������߸����������������û�иı䣬�������Ӧ���������״̬�롣304��Ӧ��ֹ������Ϣ�壬���ʼ������Ϣͷ��ĵ�һ�����н�β�� ��������Ӧ����������µ�ͷ��Ϣ�� ����Date���������������û��ʱ�ӡ�����û��ʱ�ӵķ�����Ҳ������Щ������ô����������Լ��ͻ��˿������н� Date �ֶ���ӵ����յ�����Ӧͷ��ȥ������RFC 2068�й涨��һ������������ƽ������������� ����ETag ��/�� Content-Location������ͬ��������Ӧ����200��Ӧ�� ����Expires, Cache-Control����/��Vary��������ֵ������֮ǰ��ͬ������������Ӧ��Ӧ��ֵ��ͬ�Ļ��� �������籾��Ӧ����ʹ����ǿ������֤����ô������Ӧ��Ӧ�ð�������ʵ��ͷ���������磬ĳ���������� GET ����ʹ������������֤����������Ӧ��ֹ��������ʵ��ͷ��������˻����˵�ʵ�����ݺ͸����˵�ʵ��ͷ��Ϣ֮��Ĳ�һ�¡� ��������ĳ��304��Ӧָ���˵�ǰĳ��ʵ��û�л��棬��ô����ϵͳ������������Ӧ�������ظ����Ͳ������������������� ����������յ�һ��Ҫ�����ĳ��������Ŀ��304��Ӧ����ô����ϵͳ�������������Ŀ�Է�ӳ��������Ӧ�б����µ��ֶε�ֵ��
#define ERR_HTTP_305				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 305)	// ���������Դ����ͨ��ָ���Ĵ�����ܱ����ʡ�Location ���н�����ָ���Ĵ������ڵ� URI ��Ϣ����������Ҫ�ظ�����һ������������ͨ�����������ܷ�����Ӧ��Դ��ֻ��ԭʼ���������ܽ���305��Ӧ�� ����ע�⣺RFC 2068��û����ȷ305��Ӧ��Ϊ���ض���һ�����������󣬶���ֻ�ܱ�ԭʼ������������������Щ���ƿ��ܵ������صİ�ȫ�����
#define ERR_HTTP_306				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 306)	// �����°�Ĺ淶�У�306״̬���Ѿ����ٱ�ʹ�á�
#define ERR_HTTP_307				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 307)	// �������Դ������ʱ�Ӳ�ͬ��URI ��Ӧ���������������ض�������ʱ�ģ��ͻ���Ӧ��������ԭ�е�ַ�����Ժ������ֻ����Cache-Control��Expires�н�����ָ��������£������Ӧ���ǿɻ���ġ� �����µ���ʱ�Ե�URI Ӧ������Ӧ�� Location ���з��ء���������һ��HEAD ���󣬷�����Ӧ��ʵ����Ӧ������ָ���µ�URI �ĳ����Ӽ����˵������Ϊ�������������ʶ��307��Ӧ�������Ҫ���������Ҫ��Ϣ�Ա��û��ܹ���Ⲣ���µ� URI ������������ ��������ⲻ��һ��GET ���� HEAD ������ô�������ֹ�Զ������ض��򣬳��ǵõ��û���ȷ�ϣ���Ϊ���������������˷����仯��
#define ERR_HTTP_400				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 400)	// 1���������󣬵�ǰ�����޷�����������⡣���ǽ����޸ģ�����ͻ��˲�Ӧ���ظ��ύ������� ����2�������������
#define ERR_HTTP_401				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 401)	// ��ǰ������Ҫ�û���֤������Ӧ�������һ�������ڱ�������Դ�� WWW-Authenticate ��Ϣͷ����ѯ���û���Ϣ���ͻ��˿����ظ��ύһ������ǡ���� Authorization ͷ��Ϣ�����������ǰ�����Ѿ������� Authorization ֤�飬��ô401��Ӧ�����ŷ�������֤�Ѿ��ܾ�����Щ֤�顣���401��Ӧ��������ǰһ����Ӧ��ͬ�������֤ѯ�ʣ���������Ѿ����ٳ�����һ����֤����ô�����Ӧ�����û�չʾ��Ӧ�а�����ʵ����Ϣ����Ϊ���ʵ����Ϣ�п��ܰ�������������Ϣ���μ�RFC 2617��
#define ERR_HTTP_402				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 402)	// ��״̬����Ϊ�˽������ܵ������Ԥ���ġ�
#define ERR_HTTP_403				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 403)	// �������Ѿ�������󣬵��Ǿܾ�ִ��������401��Ӧ��ͬ���ǣ������֤�������ṩ�κΰ����������������Ҳ��Ӧ�ñ��ظ��ύ������ⲻ��һ�� HEAD ���󣬶��ҷ�����ϣ���ܹ������Ϊ�������ܱ�ִ�У���ô��Ӧ����ʵ���������ܾ���ԭ�򡣵�Ȼ������Ҳ���Է���һ��404��Ӧ����������ϣ���ÿͻ��˻���κ���Ϣ��
#define ERR_HTTP_404				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 404)	// ����ʧ�ܣ�������ϣ���õ�����Դδ���ڷ������Ϸ��֡�û����Ϣ�ܹ������û����״����������ʱ�Ļ������õġ����������֪������Ļ���Ӧ��ʹ��410״̬������֪����Դ��ΪĳЩ�ڲ������û������⣬�Ѿ����õĲ����ã�����û���κο�����ת�ĵ�ַ��404���״̬�뱻�㷺Ӧ���ڵ������������ʾ����Ϊ�����󱻾ܾ�����û�������ʺϵ���Ӧ���õ�����¡�
#define ERR_HTTP_405				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 405)	// ��������ָ�������󷽷����ܱ�����������Ӧ����Դ������Ӧ���뷵��һ��Allow ͷ��Ϣ���Ա�ʾ����ǰ��Դ�ܹ����ܵ����󷽷����б� �������� PUT��DELETE ������Է������ϵ���Դ����д������������󲿷ֵ���ҳ����������֧�ֻ�����Ĭ�������²������������󷽷������ڴ���������᷵��405����
#define ERR_HTTP_406				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 406)	// �������Դ�����������޷���������ͷ�е�����������޷�������Ӧʵ�塣 ������������һ�� HEAD ���󣬷������Ӧ��Ӧ������һ�������������û��������������ѡ������ʵ�ʵ�������Լ���ַ�б��ʵ�塣ʵ��ĸ�ʽ�� Content-Type ͷ�ж����ý�����;�������������Ը��ݸ�ʽ���������������������ѡ�񡣵��ǣ��淶�в�û�ж����κ����������Զ�ѡ��ı�׼��
#define ERR_HTTP_407				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 407)	// ����401��Ӧ���ƣ�ֻ�����ͻ��˱����ڴ���������Ͻ��������֤��������������뷵��һ�� Proxy-Authenticate ���Խ������ѯ�ʡ��ͻ��˿��Է���һ�� Proxy-Authorization ��Ϣͷ������֤���μ�RFC 2617��
#define ERR_HTTP_408				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 408)	// ����ʱ���ͻ���û���ڷ�����Ԥ���ȴ���ʱ�������һ������ķ��͡��ͻ��˿�����ʱ�ٴ��ύ��һ�������������κθ��ġ�
#define ERR_HTTP_409				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 409)	// ���ںͱ��������Դ�ĵ�ǰ״̬֮����ڳ�ͻ�������޷���ɡ��������ֻ������������������²��ܱ�ʹ�ã��û�����Ϊ�ܹ������ͻ�����һ������ύ�µ����󡣸���ӦӦ�������㹻����Ϣ�Ա��û����ֳ�ͻ��Դͷ�� ������ͻͨ�������ڶ� PUT ����Ĵ����С����磬�ڲ��ð汾���Ļ����£�ĳ�� PUT �ύ�Ķ��ض���Դ���޸������������İ汾��Ϣ��֮ǰ��ĳ�������������������ͻ����ô��ʱ��������Ӧ�÷���һ��409���󣬸�֪�û������޷���ɡ���ʱ����Ӧʵ���кܿ��ܻ����������ͻ�汾֮��Ĳ���Ƚϣ��Ա��û������ύ�鲢�Ժ���°汾��
#define ERR_HTTP_410				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 410)	// ���������Դ�ڷ��������Ѿ����ٿ��ã�����û���κ���֪��ת����ַ��������״��Ӧ������Ϊ�������Եġ�������ܣ�ӵ�����ӱ༭���ܵĿͻ���Ӧ���ڻ���û���ɺ�ɾ������ָ�������ַ�����á������������֪�������޷�ȷ�����״���Ƿ������õģ���ô��Ӧ��ʹ��404״̬�롣���Ƕ���˵�������������Ӧ�ǿɻ���ġ� ����410��Ӧ��Ŀ����Ҫ�ǰ�����վ����Աά����վ��֪ͨ�û�����Դ�Ѿ����ٿ��ã����ҷ�����ӵ����ϣ������ָ�������Դ��Զ������Ҳ��ɾ���������¼�����ʱ����ֵ�����к��ձ顣ͬ����410��ӦҲ������֪ͨ�ͻ����ڵ�ǰ������վ���ϣ�ԭ������ĳ�����˵���Դ�Ѿ����ٿ��á���Ȼ���Ƿ���Ҫ���������ò����õ���Դ���Ϊ'410 Gone'���Լ��Ƿ���Ҫ���ִ˱�Ƕ೤ʱ�䣬��ȫȡ���ڷ�����ӵ���ߡ�
#define ERR_HTTP_411				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 411)	// �������ܾ���û�ж��� Content-Length ͷ������½�������������˱���������Ϣ�峤�ȵ���Ч Content-Length ͷ֮�󣬿ͻ��˿����ٴ��ύ������
#define ERR_HTTP_412				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 412)	// ����������֤�������ͷ�ֶ��и����Ⱦ�����ʱ��û���������е�һ�����������״̬������ͻ����ڻ�ȡ��Դʱ�������Ԫ��Ϣ������ͷ�ֶ����ݣ��������Ⱦ��������Դ˱�������󷽷���Ӧ�õ���ϣ���������������Դ�ϡ�
#define ERR_HTTP_413				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 413)	// �������ܾ�����ǰ������Ϊ�������ύ��ʵ�����ݴ�С�����˷�����Ը������ܹ�����ķ�Χ����������£����������Թر���������ͻ��˼������ʹ����� ����������״������ʱ�ģ�������Ӧ������һ�� Retry-After ����Ӧͷ���Ը�֪�ͻ��˿����ڶ���ʱ���Ժ����³��ԡ�
#define ERR_HTTP_414				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 414)	// �����URI ���ȳ����˷������ܹ����͵ĳ��ȣ���˷������ܾ��Ը������ṩ������Ƚ��ټ���ͨ������������� ������Ӧʹ��POST�����ı��ύ�����GET���������²�ѯ�ַ�����Query String�������� �����ض���URI ���ڶ���������ÿ���ض���Ѿɵ� URI ��Ϊ�µ� URI ��һ���֣����������ɴ��ض���� URI ������ �����ͻ������ڳ�������ĳЩ�������д��ڵİ�ȫ©�����������������������ʹ�ù̶����ȵĻ����ȡ���������� URI���� GET ��Ĳ�������ĳ����ֵ�󣬿��ܻ�������������������������뱻ִ��[1]��û�д���©���ķ�������Ӧ������414״̬�롣
#define ERR_HTTP_415				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 415)	// ���ڵ�ǰ����ķ��������������Դ���������ύ��ʵ�岢���Ƿ���������֧�ֵĸ�ʽ��������󱻾ܾ���
#define ERR_HTTP_416				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 416)	// ��������а����� Range ����ͷ������ Range ��ָ�����κ����ݷ�Χ���뵱ǰ��Դ�Ŀ��÷�Χ���غϣ�ͬʱ��������û�ж��� If-Range ����ͷ����ô��������Ӧ������416״̬�롣 �������� Range ʹ�õ����ֽڷ�Χ����ô�����������ָ����ָ�����������ݷ�Χ�����ֽ�λ�ö������˵�ǰ��Դ�ĳ��ȡ�������ҲӦ���ڷ���416״̬���ͬʱ������һ�� Content-Range ʵ��ͷ������ָ����ǰ��Դ�ĳ��ȡ������ӦҲ����ֹʹ�� multipart/byteranges ��Ϊ�� Content-Type��
#define ERR_HTTP_417				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 417)	// ������ͷ Expect ��ָ����Ԥ�������޷������������㣬���������������һ��������������������Ե�֤��֤���ڵ�ǰ·�ɵ���һ���ڵ��ϣ�Expect �������޷������㡣
#define ERR_HTTP_421				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 421)	// �ӵ�ǰ�ͻ������ڵ�IP��ַ���������������������˷�������ɵ����Χ��ͨ���������IP��ַָ���Ǵӷ������Ͽ����Ŀͻ��˵�ַ�������û������ػ��ߴ����������ַ��������������£��������ļ�������漰����ֹһ���ն��û���
#define ERR_HTTP_422				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 422)	// �ӵ�ǰ�ͻ������ڵ�IP��ַ���������������������˷�������ɵ����Χ��ͨ���������IP��ַָ���Ǵӷ������Ͽ����Ŀͻ��˵�ַ�������û������ػ��ߴ����������ַ��������������£��������ļ�������漰����ֹһ���ն��û���
#define ERR_HTTP_422				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 422)	// �����ʽ��ȷ���������ں�����������޷���Ӧ����RFC 4918 WebDAV��423 Locked ������ǰ��Դ����������RFC 4918 WebDAV��
#define ERR_HTTP_424				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 424)	// ����֮ǰ��ĳ���������Ĵ��󣬵��µ�ǰ����ʧ�ܣ����� PROPPATCH����RFC 4918 WebDAV��
#define ERR_HTTP_425				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 425)	// ��WebDav Advanced Collections �ݰ��ж��壬����δ�����ڡ�WebDAV ˳��Э�顷��RFC 3658���С�
#define ERR_HTTP_426				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 426)	// �ͻ���Ӧ���л���TLS/1.0����RFC 2817��
#define ERR_HTTP_449				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 449)	// ��΢����չ����������Ӧ����ִ�����ʵ��Ĳ�����������ԡ�
#define ERR_HTTP_500				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 500)	// ������������һ��δ��Ԥ�ϵ�״�������������޷���ɶ�����Ĵ���һ����˵��������ⶼ���ڷ������ĳ��������ʱ���֡�
#define ERR_HTTP_501				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 501)	// ��������֧�ֵ�ǰ��������Ҫ��ĳ�����ܡ����������޷�ʶ������ķ����������޷�֧������κ���Դ������
#define ERR_HTTP_502				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 502)	// ��Ϊ���ػ��ߴ������ķ���������ִ������ʱ�������η��������յ���Ч����Ӧ��
#define ERR_HTTP_503				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 503)	// ������ʱ�ķ�����ά�����߹��أ���������ǰ�޷������������״������ʱ�ģ����ҽ���һ��ʱ���Ժ�ָ�������ܹ�Ԥ���ӳ�ʱ�䣬��ô��Ӧ�п��԰���һ�� Retry-After ͷ���Ա�������ӳ�ʱ�䡣���û�и������ Retry-After ��Ϣ����ô�ͻ���Ӧ���Դ���500��Ӧ�ķ�ʽ�������� ����ע�⣺503״̬��Ĵ��ڲ�����ζ�ŷ������ڹ��ص�ʱ�����ʹ������ĳЩ������ֻ������ϣ���ܾ��ͻ��˵����ӡ�
#define ERR_HTTP_504				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 504)	// ��Ϊ���ػ��ߴ������ķ���������ִ������ʱ��δ�ܼ�ʱ�����η�������URI��ʶ���ķ�����������HTTP��FTP��LDAP�����߸���������������DNS���յ���Ӧ�� ����ע�⣺ĳЩ�����������DNS��ѯ��ʱʱ�᷵��400����500����
#define ERR_HTTP_505				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 505)	// ��������֧�֣����߾ܾ�֧����������ʹ�õ� HTTP �汾���ⰵʾ�ŷ��������ܻ�Ըʹ����ͻ�����ͬ�İ汾����Ӧ��Ӧ������һ��������Ϊ�ΰ汾����֧���Լ�������֧����ЩЭ���ʵ�塣
#define ERR_HTTP_506				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 506)	// �ɡ�͸������Э��Э�顷��RFC 2295����չ����������������ڲ����ô��󣺱������Э�̱�Ԫ��Դ������Ϊ��͸������Э����ʹ���Լ��������һ��Э�̴����в���һ�����ʵ��ص㡣
#define ERR_HTTP_507				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 507)	// �������޷��洢�����������������ݡ����״������Ϊ����ʱ�ġ�WebDAV (RFC 4918)
#define ERR_HTTP_509				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 509)	// �������ﵽ�������ơ��ⲻ��һ���ٷ���״̬�룬�����Ա��㷺ʹ�á�
#define ERR_HTTP_510				MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, 510)	// ��ȡ��Դ����Ҫ�Ĳ��Բ�û��û���㡣��RFC 2774��

#define ERR_HTTP_ERROR_(E)			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_HTTPS, (E))   

#define ERR_WEBAPPS_DISABLE			MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_WEBAPPS, 200)   
#define ERR_WEBAPPS_ERROR_(E)		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_WEBAPPS, (E))   

// Cloud disk
#define ERR_CLOUD_UPLOAD_SUCCEEDED	MAKE_SF_EXCEPTION(SEVERITY_INFO,  FACILITY_CLOUD_DISK, 1)	// ���浽���̳ɹ�
#define ERR_CLOUD_UPLOADING			MAKE_SF_EXCEPTION(SEVERITY_INFO,  FACILITY_CLOUD_DISK, 2)	// ����ת������...

#define ERR_CLOUD_UPLOAD_FAILED		MAKE_SF_EXCEPTION(SEVERITY_ERROR, FACILITY_CLOUD_DISK, 1)	// ���浽����ʧ��


//
#define ERROR_BUFFER_SIZE	( 2048 )	// Format error information buffer size, See FormatError API

#endif	// __OIM_ERRORS_2013_11_29_BY_YFGZ__

