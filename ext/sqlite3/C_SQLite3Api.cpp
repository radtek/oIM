#include "sqllite\I_SQLite.h"
#include "Public\Utils.h"
#include "string\strcpcvt.h"

extern "C" __declspec(dllexport) int __stdcall CreateInterface(const TCHAR* pctszIID, void** ppvIObject);

#define ENCRYPT_PSW(out, len) \
	{ \
		int iIndex = 0; \
		int iDelta = (out[0] & 0x07) ? (out[0] & 0x07) : 7;\
		int iSize = len;\
		for(iIndex = 0; iIndex < len; iIndex++) \
		{ \
			if(out[iIndex] + iDelta > '~') \
				out[iIndex] = ( out[iIndex] + iDelta ) % '~' + ' '; \
			else \
				out[iIndex] = ( out[iIndex] + iDelta ); \
		} \
		for(iIndex = 0; iIndex < len - 1; iIndex+=2) \
		{ \
			char ch = out[iIndex]; \
			out[iIndex] = out[iIndex + 1]; \
			out[iIndex + 1] = ch; \
		} \
		for(iIndex = 0; iIndex < len / 2; iIndex++) \
		{ \
			char ch = out[iIndex]; \
			out[iIndex] = out[len - iIndex - 1]; \
			out[len - iIndex - 1] = ch; \
		} \
	} \

///////////////////////////////////////////////////////////////////////////////
class C_SQLite3: public I_SQLite3
{
private:   
	ULONG	 m_ulRef;
	sqlite3* m_pDb;

public:
	C_SQLite3()
		: m_pDb( NULL )
		, m_ulRef( 1 )
	{
	}

	virtual HRESULT QueryInterface(	const TCHAR* ptszIID, void** ppvIObject)
	{
		if ( _tcsnicmp(ptszIID, INAME_SQLITE_DB, _tcslen(INAME_SQLITE_DB)) == 0 )
		{	// Query myself
			AddRef();
			*ppvIObject = this;
			return ERR_NO_ERROR;
		}
		else	// Query a new sub interface
			return CreateInterface(ptszIID, ppvIObject);
	}

	virtual ULONG AddRef(void)
	{
		return ++m_ulRef;
	}

	virtual ULONG Release(void)
	{
		if ( m_ulRef == 0 )
			return 0;

		if ( --m_ulRef == 0)
		{
			Close();
			delete this;
			return 0;
		}

		return m_ulRef;
	}

public:
	//=============================================================================
	//Function:     Initialize
	//Description:	Initialize sqlite3 library
	//				see: http://www.sqlite.org/c3ref/initialize.html
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int Initialize(void)
	{
		return ::sqlite3_initialize();
	}

	//=============================================================================
	//Function:     Shutdown
	//Description:	Shutdown sqlite3 library
	//				see: http://www.sqlite.org/c3ref/initialize.html
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int Shutdown(void)
	{
		return ::sqlite3_shutdown();
	}

	//=============================================================================
	//Function:     Config
	//Description:	Set sqlite3 config
	//				see: http://www.sqlite.org/c3ref/config.html
	//
	//Parameter:
	//	op    - Option: SQLITE_CONFIG_SINGLETHREAD(1)
	//					SQLITE_CONFIG_MULTITHREAD(2)
	//					SQLITE_CONFIG_SERIALIZED(3)
	//					SQLITE_CONFIG_PCACHE(14)
	//					SQLITE_CONFIG_GETPCACHE(15)
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int Config(int op)
	{
		return ::sqlite3_config(op);
	}

	//=============================================================================
	//Function:     Config
	//Description:	Set sqlite3 config
	//				see: http://www.sqlite.org/c3ref/config.html
	//
	//Parameter:
	//	op										i32Value
	//	SQLITE_CONFIG_MEMSTATUS(9)				boolean
	//	SQLITE_CONFIG_URI(17)					int
	//	SQLITE_CONFIG_COVERING_INDEX_SCAN(20)	int
	//	SQLITE_CONFIG_WIN32_HEAPSIZE(23)		int(bytes)
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int Config(int op, int i32Value)
	{
		return ::sqlite3_config(op, i32Value);
	}

	// Database file
	//=============================================================================
	//Function:     Open
	//Description:	Open database file
	//				see: http://www.sqlite.org/c3ref/open.html
	//
	//Parameter:
	//	pszFile    - Database file as UTF8
	//
	//Return:
	//		SQLITE_*              
	//=============================================================================
	virtual int	Open(const char* pszFile, int i32Flag = SQLITE_OPEN_FLAG_DEFAULT)
	{
		if ( m_pDb )
			return SQLITE_MISUSE;
 
		int i32Ret = ::sqlite3_open_v2(pszFile, &m_pDb, i32Flag, NULL);
		if ( i32Ret != SQLITE_OK )
		{
			STRACE(_T("Open database [%s] failed"), pszFile);
			return i32Ret;
		}

		STRACE(_T("Open database [%s] succeeded"), pszFile);
		return SQLITE_OK;
	}

	//=============================================================================
	//Function:     GetFileName
	//Description:	Returns a pointer to a filename associated with database pszDbName of connected.
	//				See: http://www.sqlite.org/c3ref/db_filename.html
	//
	//Parameter:
	//	pszDbName    -  Database Name
	//
	//Return:
	//		Returns a pointer to a filename associated with database pszDbName of connected.    
	//=============================================================================
	virtual const char* GetFile(const char* pszAsDbName = NULL)
	{
		if ( m_pDb )
			return ::sqlite3_db_filename(m_pDb, pszAsDbName);

		return NULL;
	}

	//=============================================================================
	//Function:     Attach
	//Description:	Attach a database to current database, MUST be opened the current database.
	//				see: http://www.sqlite.org/c3ref/limit.html
	//
	//Parameter:
	//	pszFile    - Database file to attach to current database handle
	//	pszAsDb    - Attach as database name
	//				 see: http://www.sqlite.org/lang_attach.html
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int	Attach(const char* pszFile, const char* pszAsDb, const char* pszKey)
	{
		if ( pszFile == NULL  || pszAsDb == NULL )
			return SQLITE_MISUSE;

		char* pszSql = NULL;
		if ( pszKey && pszKey[0] )
		{
			char szTmp[17] = { 0 };	// size as: DB_KEY_LENGTH_BYTE + 1
			int nKey = strlen(pszKey);
			if ( nKey > 16 )
				nKey = 16;

			memcpy(szTmp, pszKey, nKey);
			ENCRYPT_PSW(szTmp, nKey);

			pszSql = VMPrintf("ATTACH DATABASE \"%s\" AS \"%s\" KEY \"%s\"", pszFile, pszAsDb, szTmp);
		}
		else
			pszSql = VMPrintf("ATTACH DATABASE \"%s\" AS \"%s\"", pszFile, pszAsDb);

		int i32Ret = SQL(pszSql);
		STRACE(_T("%s, %s"), pszSql, i32Ret == SQLITE_OK ? _T("succeeded") : _T("failed"));
		Free(pszSql);
		
		return i32Ret;
	}

	//=============================================================================
	//Function:     Detach
	//Description:	Detach the database
	//				see: http://www.sqlite.org/lang_detach.html
	//
	//Parameter:
	//	pszAsDb    - Database name
	//
	//Return:
	//		SQLITE_*            
	//=============================================================================
	virtual int	Detach(const char* pszAsDb)
	{
		if ( pszAsDb == NULL )
			return SQLITE_MISUSE;

		char* pszSql = VMPrintf("DETACH DATABASE \"%s\"", pszAsDb);
		int i32Ret = SQL(pszSql);
		STRACE(_T("%s, %s"), pszSql, i32Ret == SQLITE_OK ? _T("succeeded") : _T("failed"));
		Free(pszSql);

		return i32Ret;
	}

	//=============================================================================
	//Function:     Close
	//Description:	Close database
	//				see: http://www.sqlite.org/c3ref/close.html
	//
	//
	//Return:
	//     SQLITE_*         
	//=============================================================================
	virtual int	Close()
	{
		if ( m_pDb == NULL )
			return SQLITE_OK;

		STRACE(_T("Close database [%s]"), GetFile());
		int i32Ret = ::sqlite3_close_v2( m_pDb );
		if(SQLITE_OK == i32Ret )
			m_pDb = NULL;

		return i32Ret;
	}

	//=============================================================================
	//Function:     GetHandle
	//Description:	Get the current database handle
	//
	//
	//Return:
	//       Current database handle when opened, else is NULL       
	//=============================================================================
	virtual sqlite3* GetDbHandle()
	{
		return m_pDb;
	}

	// Encrypt
	//=============================================================================
	//Function:     Key
	//Description:	Set the SQLite database key for encrypt, the database MUST be opened
	//
	//Parameter:
	//	pKey    - Key
	//	nKey    - Key length, range as (0, 16]
	//
	//Return:
	//		SQLITE_*
	//=============================================================================
	virtual int Key(const void* pKey, int nKey)
	{
		char szTmp[17] = { 0 };	// size as: DB_KEY_LENGTH_BYTE + 1
		if ( pKey == NULL || nKey <= 0 || nKey > 16 || m_pDb == NULL )
			return SQLITE_MISUSE;
		
		memcpy(szTmp, pKey, nKey);
		ENCRYPT_PSW(szTmp, nKey);

		return ::sqlite3_key(m_pDb, szTmp, nKey);
	}

	//=============================================================================
	//Function:     Rekey
	//Description:	Changed the key, MUST be call Key at first 
	//
	//Parameter:
	//	pKey    - Key
	//	nKey    - Key length, range as[0,16]
	//
	//Return:
	//		SQLITE_*
	//=============================================================================	
	virtual int Rekey(const void* pKey, int nKey)
	{
		char szTmp[16] = { 0 };
		if ( nKey < 0 || nKey > 16 || m_pDb == NULL )
			return SQLITE_MISUSE;
		
		memcpy(szTmp, pKey, nKey);
		ENCRYPT_PSW(szTmp, nKey);

		return ::sqlite3_rekey(m_pDb, szTmp, nKey);
	}

	//=============================================================================
	//Function:     SQL
	//Description:	Execute SQL statement
	//				see http://www.sqlite.org/c3ref/exec.html
	//
	//Parameter:
	//	pszSql         - The UTF8 encoded SQL statement
	//	pfnCallback    - Callback function to dealwith the result
	//	pvData         - An array of pointers to strings obtained as if from sqlite3_column_text(), one for each column.
	//  bFree		   - true: auto free memory of pszSql
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int	SQL(const char* pszSql, bool bFree = false, sqlite3_callback pfnCallback = NULL, void* pvData = NULL)
	{
		int i32Ret = ::sqlite3_exec(m_pDb, pszSql, pfnCallback, pvData, NULL);

		if (i32Ret != SQLITE_OK)
		{
			STRACE(_T("Execute SQL[%s], result:%d"), pszSql, i32Ret);
		}

		if (bFree)
			Free((void*)pszSql);

		return i32Ret;
	}

	//=============================================================================
	//Function:     GetTable
	//Description: Get a table by SQL
	//
	//Parameter:
	//	pszSql    - SQL statement, UTF-8 encoded 
	//  bFree		   - true: auto free memory of pszSql
	//
	//Return:
	//		The table interface when success, else is NULL          
	//=============================================================================
	virtual I_SQLite3Table* GetTable(const char* pszSql, bool bFree = false, int* pi32Ret = NULL)
	{
		int			   i32Ret;
		const char*	   pszTail = NULL;
		sqlite3_stmt*  pVM = NULL;
		I_SQLite3Table* pTable = NULL;

		if ( pszSql == NULL || m_pDb == NULL ||
			 FAILED(CreateInterface(INAME_SQLITE_TABLE, (void**)&pTable)) )
		{
			if (bFree)	Free((void*)pszSql);
			if (pi32Ret) *pi32Ret = SQLITE_ERROR;
			return NULL;
		}

		if ( SQLITE_OK != (i32Ret = ::sqlite3_prepare_v2(m_pDb, pszSql, -1, &pVM, &pszTail)) )
		{
			STRACE(_T("Prepare SQL[%s] failed, result:%d"), pszSql, i32Ret);
			if (bFree)	Free((void*)pszSql);
			pTable->Release();
			if (pi32Ret) *pi32Ret = i32Ret;
			return NULL;
		}

		if (bFree)	Free((void*)pszSql);
		pTable->SetDbHandle(m_pDb);
		pTable->SetStmtHandle(pVM);
		if (pi32Ret) *pi32Ret = i32Ret;

		return pTable;
	}

	// Help 
	//=============================================================================
	//Function:     GetErrorMsg
	//Description:	Get the error description 
	//
	//Parameter:
	//	i32ErrorCode    - SQLite error code
	//
	//Return:
	//		Errodes description            
	//=============================================================================
	virtual TCHAR* GetErrorMsg(int i32ErrorCode )
	{
		switch (i32ErrorCode)
		{
		case SQLITE_OK          : return _T("Successful result");
		case SQLITE_ERROR       : return _T("SQL error or missing database");
		case SQLITE_INTERNAL    : return _T("Internal logic error in SQLite");
		case SQLITE_PERM        : return _T("Access permission denied");
		case SQLITE_ABORT       : return _T("Callback routine requested an abort");
		case SQLITE_BUSY        : return _T("The database file is locked");
		case SQLITE_LOCKED      : return _T("A table in the database is locked");
		case SQLITE_NOMEM       : return _T("A malloc() failed");
		case SQLITE_READONLY    : return _T("Attempt to write a readonly database");
		case SQLITE_INTERRUPT   : return _T("Operation terminated by sqlite3_interrupt()");
		case SQLITE_IOERR       : return _T("Some kind of disk I/O error occurred");
		case SQLITE_CORRUPT     : return _T("The database disk image is malformed");
		case SQLITE_NOTFOUND    : return _T("Unknown opcode in sqlite3_file_control()");
		case SQLITE_FULL        : return _T("Insertion failed because database is full");
		case SQLITE_CANTOPEN    : return _T("Unable to open the database file");
		case SQLITE_PROTOCOL    : return _T("Database lock protocol error");
		case SQLITE_EMPTY       : return _T("Database is empty");
		case SQLITE_SCHEMA      : return _T("The database schema changed");
		case SQLITE_TOOBIG      : return _T("String or BLOB exceeds size limit");
		case SQLITE_CONSTRAINT  : return _T("Abort due to constraint violation");
		case SQLITE_MISMATCH    : return _T("Data type mismatch");
		case SQLITE_MISUSE      : return _T("Library used incorrectly");
		case SQLITE_NOLFS       : return _T("Uses OS features not supported on host");
		case SQLITE_AUTH        : return _T("Authorization denied");
		case SQLITE_FORMAT      : return _T("Auxiliary database format error");
		case SQLITE_RANGE       : return _T("2nd parameter to sqlite3_bind out of range");
		case SQLITE_NOTADB      : return _T("File opened that is not a database file");
		case SQLITE_NOTICE      : return _T("Notifications from sqlite3_log()");
		case SQLITE_WARNING     : return _T("Warnings from sqlite3_log()");
		case SQLITE_ROW         : return _T("sqlite3_step() has another row ready");
		case SQLITE_DONE        : return _T("sqlite3_step() has finished executing");
		default					: return _T("Unknown sqlite3 error");			
		}										 
	}

	//=============================================================================
	//Function:     IsExistTable
	//Description:	Check whether the table exist
	//
	//Parameter:
	//	pszTableName    - Table name
	//
	//Return:
	//		TRUE	Exist
	//		FALSE	Not exist
	//=============================================================================
	virtual BOOL IsExistTable(const TCHAR* pszTableName)
	{
		eIMStringA szNameA = SOUI::S_CT2A(pszTableName);
		char* pszSql = VMPrintf("select count(*) from sqlite_master where type='table' and name='%q'", szNameA.c_str());
		I_SQLite3Table* pTable = GetTable(pszSql);
		Free(pszSql);

		if ( pTable )
		{
			BOOL bExist = (pTable->Step() == SQLITE_ROW);
			pTable->Release();
			return bExist;
		}

		return FALSE;
	}

	//=============================================================================
	//Function:     IsAutoCommit
	//Description:	Check whether is auto commit mode, 
	//				see: http://www.sqlite.org/c3ref/get_autocommit.html
	//
	//
	//Return:
	//		TRUE	Is auto commit mode
	//		FALSE	Not auto commit mode
	//=============================================================================
	virtual BOOL IsAutoCommit()
	{
		if ( m_pDb == NULL )
			return SQLITE_MISUSE;

		return ::sqlite3_get_autocommit(m_pDb) ? TRUE : FALSE;
	}

	//=============================================================================
	//Function:     GetVersion
	//Description:	Get SQLite3 version number
	//		eg.:  "3.7.17" as 3007017, see: http://www.sqlite.org/c3ref/c_source_id.html
	//
	//Return:
	//		The version number          
	//=============================================================================
	virtual int	GetVersion()
	{
		return sqlite3_libversion_number();
	}

	//=============================================================================
	//Function:     GetLastRowId
	//Description:	Get last RowId
	//				see: http://www.sqlite.org/c3ref/last_insert_rowid.html
	//
	//
	//Return:
	//      The last RowId        
	//=============================================================================
	virtual INT64 GetLastRowId()
	{
		if ( m_pDb == NULL )
			return 0;

		return ::sqlite3_last_insert_rowid(m_pDb);
	}

	//=============================================================================
	//Function:     SetBusyTimeout
	//Description:	set busy timeout
	//				see: http://www.sqlite.org/c3ref/busy_timeout.html
	//
	//Parameter:
	//	i32Ms    - Timeout value as milliseconds 
	//
	//Return:
	//		SQLITE_*
	//=============================================================================
	virtual int SetBusyTimeout(int i32Ms)
	{
		if ( m_pDb )
			return ::sqlite3_busy_timeout(m_pDb, i32Ms);

		return SQLITE_MISUSE;
	}

	//=============================================================================
	//Function:     BusyHandler
	//Description:	Set busy handler 
	//				see: http://www.sqlite.org/c3ref/busy_handler.html
	//
	//Parameter:
	//	pfnBusy    - Callback funcation
	//	pvArg      - Arg
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int SetBusyHandler(int(*pfnBusy)(void*,int), void* pvArg)
	{
		if ( m_pDb )
			return ::sqlite3_busy_handler(m_pDb, pfnBusy, pvArg);

		return SQLITE_MISUSE;
	}

	//=============================================================================
	//Function:     Interrupt
	//Description:	Set interrupt flag
	//				see: http://www.sqlite.org/c3ref/interrupt.html 
	//
	//Return:
	//              None
	//=============================================================================
	virtual void Interrupt()
	{
		if ( m_pDb )
			::sqlite3_interrupt(m_pDb);
	}

	//=============================================================================
	//Function:     Changes
	//Description:	returns the number of database rows that were changed or inserted 
	//				or deleted by the most recently completed SQL statement.
	//				see: http://www.sqlite.org/c3ref/changes.html
	//
	//Return:
	//		The rows number changed last modify             
	//=============================================================================
	virtual int	Changes()
	{
		if ( m_pDb )
			return ::sqlite3_changes(m_pDb);

		return 0;
	}

	//=============================================================================
	//Function:     TotalChanges
	//Description:	returns the number of row changes caused by INSERT, UPDATE or 
	//				DELETE statements since the database connection was opened.
	//				see: http://www.sqlite.org/c3ref/total_changes.html
	//
	//Return:
	//		The  total number of changed since opened          
	//=============================================================================
	virtual int	TotalChanges()
	{
		if ( m_pDb )
			return sqlite3_total_changes(m_pDb);

		return 0;
	}

	//=============================================================================
	//Function:     VMPrintf
	//Description:	Format the SQL string, you MUST be call Free() to free the buffer.
	//				see: http://www.sqlite.org/c3ref/mprintf.html
	//
	//Parameter:
	//	pszFmt    - Format
	//	          - Args
	//
	//Return:
	//		Return the formated string, MUST be call Free() to free it after used.          
	//=============================================================================
	virtual char* VMPrintf(const char* pszFmt, ...)
	{
		va_list args;
		va_start(args, pszFmt);
		char* pszRet = ::sqlite3_vmprintf(pszFmt, args);
		va_end(args);

		return pszRet;
	}

	//=============================================================================
	//Function:     Free
	//Description:	Calling it with a pointer previously returned by VMPrintf, or 
	//				sqlite3_malloc() or sqlite3_realloc() releases that memory so 
	//				that it might be reused. 
	//				see: http://www.sqlite.org/c3ref/free.html
	//
	//Parameter:
	//	pvData    - Pointer to be free
	//
	//Return:
	//		None      
	//=============================================================================
	virtual void Free(void* pvData)
	{
		::sqlite3_free(pvData);
	}


	// Transaction
	//=============================================================================
	//Function:     Begin
	//Description:	Begin transaction
	//				see: http://www.sqlite.org/lang_transaction.html
	//
	//Parameter:
	//	eTransType    - Transaction type, default is eTRANS_IMMEDIATE
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int	Begin(E_TransType eTransType = eTRANS_IMMEDIATE)
	{
		switch( eTransType )
		{
		case eTRANS_NONE:
			return SQL("BEGIN TRANSACTION;");
		case eTRANS_DEFERRED:
			return SQL("BEGIN DEFERRED TRANSACTION;");
		case eTRANS_IMMEDIATE:
			return SQL("BEGIN IMMEDIATE TRANSACTION;");
		case eTRANS_EXCLUSIVE:
			return SQL("BEGIN EXCLUSIVE TRANSACTION;");
		default:
			SASSERT(FALSE);
			return SQL("BEGIN TRANSACTION;");
		}
	}

	//=============================================================================
	//Function:     Compile
	//Description:	Compile a statement
	//				see: http://www.sqlite.org/c3ref/prepare.html
	//
	//Parameter:
	//	pszSql    - SQL statement
	//  bFree     - true: auto free memory of pszSql
	//Return:
	//      The statement interface when succeeded, else is NULL        
	//=============================================================================
	virtual I_SQLite3Stmt* Compile(const char* pszSql, bool bFree = false, int* pi32Ret = NULL)
	{
		int			   i32Ret;
		const char*	   pszTail = NULL;
		sqlite3_stmt*  pVM = NULL;
		I_SQLite3Stmt* pStmt = NULL;
		
		if ( pszSql == NULL || m_pDb == NULL ||
			 FAILED(CreateInterface(INAME_SQLITE_STMT, (void**)&pStmt)) )
		{
			if (bFree)	Free((void*)pszSql);
			if (pi32Ret) *pi32Ret = SQLITE_ERROR;
			return NULL;
		}

		if ( SQLITE_OK != (i32Ret = ::sqlite3_prepare_v2(m_pDb, pszSql, -1, &pVM, &pszTail)) )
		{
			STRACE(_T("Compile SQL[%s] failed, result:%d"), pszSql, i32Ret);
			if (bFree)	Free((void*)pszSql);
			pStmt->Release();
			if (pi32Ret) *pi32Ret = i32Ret;

			return NULL;
		}
				
		if (bFree)	Free((void*)pszSql);

		pStmt->SetDbHandle(m_pDb);
		pStmt->SetStmtHandle(pVM);
		if (pi32Ret) *pi32Ret = i32Ret;

		return pStmt;
	}

	//=============================================================================
	//Function:     Commit
	//Description:	Commit transaction
	//				see: http://www.sqlite.org/lang_transaction.html
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int	Commit()
	{
		return SQL("COMMIT TRANSACTION;");
	}

	//=============================================================================
	//Function:     Rollback
	//Description:	Rollback transaction
	//				see: http://www.sqlite.org/lang_transaction.html
	//
	//Return:
	//              
	//=============================================================================
	virtual int	Rollback()
	{
		return SQL("ROLLBACK TRANSACTION;");
	}


};

class C_SQLite3Table: public I_SQLite3Table
{
private:
	ULONG		  m_ulRef;
	sqlite3*	  m_pDb;
	sqlite3_stmt* m_pVM;

public:
	C_SQLite3Table()
		: m_ulRef( 1 )
		, m_pDb( NULL )
		, m_pVM( NULL )
	{
	}

	virtual HRESULT QueryInterface(	const TCHAR* ptszIID,	void** ppvIObject)
	{
		if ( _tcsnicmp(ptszIID, INAME_SQLITE_TABLE, _tcslen(INAME_SQLITE_TABLE)) == 0 )
		{	// Query myself
			AddRef();
			*ppvIObject = this;
			return ERR_NO_ERROR;
		}
		
		return ERR_NOT_IMPL;
	}

	virtual ULONG AddRef(void)
	{
		return ++m_ulRef;
	}

	virtual ULONG Release(void)
	{
		if ( m_ulRef == 0 )
			return 0;

		if ( --m_ulRef == 0)
		{
			Finalize();
			delete this;
			return 0;
		}

		return m_ulRef;
	}

public:
	//=============================================================================
	//Function:     SetDbHandle
	//Description:	Set database handle
	//
	//Parameter:
	//	pDb    - Database handle, NULL to clear it
	//
	//Return:
	//      TRUE	Succeeded
	//		FALSE	Failed
	//=============================================================================
	virtual BOOL SetDbHandle(sqlite3* pDb)
	{
		m_pDb = pDb;
		return TRUE;
	}

	//=============================================================================
	//Function:     SetStmtHandle
	//Description:	Set statement handle
	//
	//Parameter:
	//	pStmt    - Statement handle, NULL to clear it and remember to call Finalize at first
	//
	//Return:
	//     TRUE		Succeeded
	//		FALSE	Failed
	//=============================================================================
	virtual BOOL SetStmtHandle(sqlite3_stmt* pStmt)
	{
		m_pVM = pStmt;
		return TRUE;
	}

	//=============================================================================
	//Function:     GetStmtHandle
	//Description:	Get statement handle
	//
	//
	//Return:
	//		The statement handle          
	//=============================================================================
	virtual sqlite3_stmt* GetStmtHandle()
	{
		return m_pVM;
	}

	//=============================================================================
	//Function:     Finalize
	//Description:	Finalize the statement
	//				see: http://www.sqlite.org/c3ref/finalize.html
	//
	//Return:
	//		SQLITE_*         
	//=============================================================================
	virtual int Finalize()
	{
		if ( m_pVM )
		{
			sqlite3_stmt* pStmt = m_pVM;
			m_pVM = NULL;
			return ::sqlite3_finalize(pStmt);
		}

		return SQLITE_OK;
	}

	//=============================================================================
	//Function:     GetCols
	//Description:	Get table columns
	//				see: http://www.sqlite.org/c3ref/column_count.html
	//
	//Return:
	//		The columns          
	//=============================================================================
	virtual int GetCols()
	{
		return ::sqlite3_column_count(m_pVM);
	}

	//=============================================================================
	//Function:     Step
	//Description:	Do a step action
	//				see: http://www.sqlite.org/c3ref/step.html
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int Step()
	{
		int i32Ret = ::sqlite3_step(m_pVM);
		if ( i32Ret != SQLITE_ROW )
			::sqlite3_reset(m_pVM);

		return i32Ret;
	}

	//// Column information
	//=============================================================================
	//Function:     GetColName
	//Description:	Get column's name as UTF8
	//				see: http://www.sqlite.org/c3ref/column_name.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The column name as UTF8        
	//=============================================================================
	virtual const char* GetColName(int i32Col)
	{
		return ::sqlite3_column_name(m_pVM, i32Col);
	}
		
	//=============================================================================
	//Function:     GetColName16
	//Description:	Get a column's name as UTF16
	//				see: http://www.sqlite.org/c3ref/column_name.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The culumn name as UTF16            
	//=============================================================================
	virtual const void* GetColName16(int i32Col)
	{
		return ::sqlite3_column_name16(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColType
	//Description:	Get column's type
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//     SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL.          
	//=============================================================================
	virtual int	GetColType( int i32Col )
	{
		return ::sqlite3_column_type(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColDeclType
	//Description:	Get column's declare type as UTF8
	//				see: http://www.sqlite.org/c3ref/column_decltype.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		Type name          
	//=============================================================================
	virtual const char *GetColDeclType(int i32Col )
	{
		return ::sqlite3_column_decltype(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColDeclType16
	//Description:	Get column's declare type as UTF16
	//				see: http://www.sqlite.org/c3ref/column_decltype.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		Type name           
	//=============================================================================
	virtual const void *GetColDeclType16(int i32Col )
	{
		return ::sqlite3_column_decltype16(m_pVM, i32Col);
	}

	// Column value
	//=============================================================================
	//Function:     GetColBlob
	//Description:	Get a column's data as blob
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The blob data         
	//=============================================================================
	virtual const void*	GetColBlob( int i32Col )
	{
		return ::sqlite3_column_blob(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColBytes
	//Description:	Get column's data as byte
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The bytes data         
	//=============================================================================
	virtual int	GetColBytes( int i32Col )
	{
		return ::sqlite3_column_bytes(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColBytes16
	//Description:	Get column's data as word
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The word data       
	//=============================================================================
	virtual int GetColBytes16( int i32Col )
	{
		return ::sqlite3_column_bytes16(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColInt
	//Description:	Get column's data as integer
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The int data           
	//=============================================================================
	virtual int GetColInt( int i32Col )
	{
		return ::sqlite3_column_int(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColInt64
	//Description:	Get column's data as int64
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The int64 data           
	//=============================================================================
	virtual INT64 GetColInt64( int i32Col )
	{
		return ::sqlite3_column_int64(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColDbl
	//Description:	Get column's data as double
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The double data          
	//=============================================================================
	virtual double GetColDbl( int i32Col )
	{
		return ::sqlite3_column_double(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColText
	//Description:	Get column's data as UTF8
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The text data as UTF8      
	//=============================================================================
	virtual const unsigned char* GetColText( int i32Col )
	{
		return ::sqlite3_column_text(m_pVM, i32Col);
	}

	//=============================================================================
	//Function:     GetColText16
	//Description:	Get column's data as UTF16
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index
	//
	//Return:
	//		The text data as UTF16          
	//=============================================================================
	virtual const void* GetColText16( int i32Col ) 
	{
		return ::sqlite3_column_text16(m_pVM, i32Col);
	}

};

class C_SQLite3Stmt: public I_SQLite3Stmt
{
private:
	ULONG	 m_ulRef;
	sqlite3* m_pDb;
	sqlite3_stmt* m_pVM;


public:
	C_SQLite3Stmt()
		: m_ulRef( 1 )
		, m_pDb( NULL )
		, m_pVM( NULL )
	{
	}

	virtual HRESULT QueryInterface(	const TCHAR* ptszIID,	void** ppvIObject)
	{
		if ( _tcsnicmp(ptszIID, INAME_SQLITE_STMT, _tcslen(INAME_SQLITE_STMT)) == 0 )
		{	// Query myself
			AddRef();
			*ppvIObject = this;
			return ERR_NO_ERROR;
		}
		
		return ERR_NOT_IMPL;
	}

	virtual ULONG AddRef(void)
	{
		return ++m_ulRef;
	}

	virtual ULONG Release(void)
	{
		if ( m_ulRef == 0 )
			return 0;

		if ( --m_ulRef == 0)
		{
			Finalize();
			delete this;
			return 0;
		}

		return m_ulRef;
	}

public:
	//=============================================================================
	//Function:     SetDbHandle
	//Description:	Set database handle
	//
	//Parameter:
	//	pDb    - Database handle, NULL to clear it
	//
	//Return:
	//      TRUE	Succeeded
	//		FALSE	Failed
	//=============================================================================
	virtual BOOL SetDbHandle(sqlite3* pDb)
	{
		m_pDb = pDb;
		return TRUE;
	}

	//=============================================================================
	//Function:     SetStmtHandle
	//Description:	Set statement handle
	//
	//Parameter:
	//	pStmt    - Statement handle, NULL to clear it and remember to call Finalize at first
	//
	//Return:
	//     TRUE		Succeeded
	//		FALSE	Failed
	//=============================================================================
	virtual BOOL SetStmtHandle(sqlite3_stmt* pStmt)
	{
		m_pVM = pStmt;
		return TRUE;
	}
		
	//=============================================================================
	//Function:     GetStmtHandle
	//Description:	Get statement handle
	//
	//
	//Return:
	//		The statement handle          
	//=============================================================================
	virtual sqlite3_stmt* GetStmtHandle()
	{
		return m_pVM;
	}

	//=============================================================================
	//Function:     Finalize
	//Description:	Finalize the statement
	//				see: http://www.sqlite.org/c3ref/finalize.html
	//
	//Return:
	//		SQLITE_*         
	//=============================================================================
	virtual int Finalize()
	{
		if ( m_pVM )
		{
			sqlite3_stmt* pStmt = m_pVM;
			m_pVM = NULL;
			return ::sqlite3_finalize(pStmt);
		}

		return SQLITE_OK;
	}

	//=============================================================================
	//Function:     Step
	//Description:	Do a step action
	//				see: http://www.sqlite.org/c3ref/step.html
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int	Step(int* p32RowsChanged = NULL, BOOL bAutoReset = TRUE)
	{
		int nRet = ::sqlite3_step(m_pVM);
		if ( SQLITE_DONE == nRet )
		{
			if ( p32RowsChanged )
				*p32RowsChanged = ::sqlite3_changes(m_pDb);

			return ::sqlite3_reset(m_pVM);
		}
		else if (SQLITE_ROW == nRet )
		{
			return SQLITE_OK;
		}
		else
		{
			return ::sqlite3_reset(m_pVM);
		}
	}

	//=============================================================================
	//Function:     Reset
	//Description:	Reset the statement
	//				see: http://www.sqlite.org/c3ref/reset.html
	//
	//Return:
	//		SQLITE_*            
	//=============================================================================
	virtual int Reset() 
	{
		return ::sqlite3_reset(m_pVM);
	}

	//=============================================================================
	//Function:     GetBindParamName
	//Description:	Get bind parameter's name by index as UTF8
	//				see: http://www.sqlite.org/c3ref/bind_parameter_name.html
	//Parameter:
	//	i32Index    - Index of bind
	//
	//Return:
	//		The bind name as UTF8	            
	//=============================================================================
	virtual const char* GetBindParamName(int i32Index) 
	{
		return ::sqlite3_bind_parameter_name(m_pVM, i32Index);
	}

	//=============================================================================
	//Function:     GetBindParamIndex
	//Description:	Get bind parameter's index by name
	//				see: http://www.sqlite.org/c3ref/bind_parameter_index.html
	//Parameter:
	//	pszName    - Name of bind
	//
	//Return:
	//		The index           
	//=============================================================================
	virtual int GetBindParamIndex( const char* pszName ) 
	{
		return ::sqlite3_bind_parameter_index(m_pVM, pszName);
	}

	//=============================================================================
	//Function:     GetBindParamCount
	//Description:	Get bind parameter's count
	//				see: http://www.sqlite.org/c3ref/bind_parameter_count.html
	//
	//Return:
	//		The bind count          
	//=============================================================================
	virtual int GetBindParamCount( ) 
	{
		return ::sqlite3_bind_parameter_count(m_pVM);
	}

	// Bind by index of column
	//=============================================================================
	//Function:     Bind
	//Description:	Bind int value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	i32Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const int i32Value)
	{
		return ::sqlite3_bind_int(m_pVM, i32Index, i32Value);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind int64 value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	i64Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const INT64  i64Value) 
	{
		return ::sqlite3_bind_int64(m_pVM, i32Index, i64Value);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind double value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	dValue      - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const double dValue  ) 
	{
		return ::sqlite3_bind_double(m_pVM, i32Index, dValue);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind text(UTF8) value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	pszValue    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const char*  pszValue)
	{
		return ::sqlite3_bind_text(m_pVM, i32Index, pszValue, -1, SQLITE_TRANSIENT);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind blob value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	pvValue     - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const void*  pvValue, int nLen)
	{
		return ::sqlite3_bind_blob(m_pVM, i32Index, pvValue, nLen, SQLITE_TRANSIENT);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind NULL value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index)
	{
		return ::sqlite3_bind_null(m_pVM, i32Index);
	}

	// Bind by name of column
	//=============================================================================
	//Function:     Bind
	//Description:	Bind int value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	i32Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const int i32Value)
	{
		return Bind(GetBindParamIndex(pszName), i32Value);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind int64 value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	i64Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const INT64  i64Value)
	{
		return Bind(GetBindParamIndex(pszName), i64Value);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind double value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	dValue      - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const double dValue  )
	{
		return Bind(GetBindParamIndex(pszName), dValue);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind text(UTF8) value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	pszValue    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const char*  pszValue)
	{
		return Bind(GetBindParamIndex(pszName), pszValue);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind blob value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//	pvValue     - Value to bind
	//  nLen		- Length of pvValue
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const void*  pvValue, int nLen)
	{
		return Bind(GetBindParamIndex(pszName), pvValue, nLen);
	}

	//=============================================================================
	//Function:     Bind
	//Description:	Bind NULL value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName)
	{
		return Bind(GetBindParamIndex(pszName));
	}

};

///////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int __stdcall CreateInterface(const TCHAR* pctszIID, void** ppvIObject)
{
	CHECK_NULL_RET_(pctszIID, ERR_INVALID_POINTER);
	CHECK_NULL_RET_(ppvIObject, ERR_INVALID_POINTER);

	if ( _tcsnicmp(pctszIID, INAME_SQLITE_DB, _tcslen(INAME_SQLITE_DB)) == 0 )
	{
		*ppvIObject = new C_SQLite3;
	}
	else if ( _tcsnicmp(pctszIID, INAME_SQLITE_TABLE, _tcslen(INAME_SQLITE_TABLE)) == 0 )
	{
		*ppvIObject = new C_SQLite3Table;
	}
	else if ( _tcsnicmp(pctszIID, INAME_SQLITE_STMT, _tcslen(INAME_SQLITE_STMT)) == 0 )
	{
		*ppvIObject = new C_SQLite3Stmt;
	}
	else
		return ERR_NOT_IMPL;

	if (*ppvIObject)
		return ERR_NO_ERROR;

	STRACE(_T("Create interface[%s] failed"), pctszIID);
	return ERR_OUT_OF_MEMORY;
}
