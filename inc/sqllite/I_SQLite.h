// This is a part of the SXIT Technology source files.
// Copyright (C) SXIT Corporation all rights reserved.
//
// Description: SQLITE3 线程安全参考：　http://www.cnblogs.com/wuhenke/archive/2011/11/20/2256618.html
//		即一个简单的原则为：以SQLITE_OPEN_NOMUTEX（已经为I_SQLiteDatabase的打开默认方式）
//	打开数据库连接，且必须一个线程一个连接，禁止在多个线程中共享连接。
// 
// 
// 
// Auth: LongWQ
// Date: 2014/6/26 10:31:39
// 
// History: 
//    2014/6/26 LongWQ 
//    
// Example:
//  I_SQLite3*		pIDb = NULL;
//	I_SQLite3Table*	pITbl= NULL;
//	I_SQLite3Stmt*	pIStmt= NULL;
//	int i32Ret = pIEngine->QueryInterface(INAME_SQLITE_DB, &pIDb);
//	if (FAILED(i32Ret))
//		return i32Ret;
//
//	if (SQLITE_OK == pIDb->Open("d:\\test.db"))
//  {
//		// Query data==============================================================
//		pITbl = pIDb->GetTable("select * from users;");
//		if (pITbl) 
//		{
//			// Do anyting here..
//			while(pITbl->Step() == SQLITE_ROW )
//			{	
//				pITbl->GetCol*(..)
//				//...
//			}
//			pITbl->Finalize();	// Release sqlite sqlite3_stmt*, it also auto call it in Release()
//			SAFE_RELEASE_INTERFACE_(pITbl);
//		}
//		
//		// Update data=========================================================
//		pIDb->Begin();
//		pIStmt = pIDb->Compile("INSERT INTO t_test(id, name) VALUES(@id, @name);");
//		if (pIStmt)
//		{
//			for(...)
//			{
//				pIStmt->Bind(...);	
//				//...
//				pIStmt->Step();
//			}
//			pIStmt->Commit();
//			pIStmt->Finalize();	// Release sqlite sqlite3_stmt*, it also auto call it in Release()
//			SAFE_RELEASE_INTERFACE_(pIStmt);
//		}
//		pIDb->Close();
//		SAFE_RELEASE_INTERFACE_(pIDb);
//	}
/////////////////////////////////////////////////////////////////////////////

#ifndef __IEIM_SQLITE_INTERFACE_HEADER_2017_12_18_YFGZ__
#define __IEIM_SQLITE_INTERFACE_HEADER_2017_12_18_YFGZ__

#include <Windows.h>
#include "unknown\obj-ref-i.h"
#include "sqllite\SQLite3.h"

#define INAME_SQLITE_DB		_T("SXIT.SQLite3.Database")		// I_SQLite3 interface name
#define INAME_SQLITE_TABLE	_T("SXIT.SQLite3.Table")		// I_SQLite3Table interface name
#define INAME_SQLITE_STMT	_T("SXIT.SQLite3.Statement")	// I_SQLite3Stmt interface name

#define SQLITE_OPEN_FLAG_DEFAULT	(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX)

class I_SQLite3;
class I_SQLite3Table;
class I_SQLite3Stmt;
typedef enum tagTransType
{
	eTRANS_NONE = 0,
	eTRANS_DEFERRED,
	eTRANS_IMMEDIATE,
	eTRANS_EXCLUSIVE,
}E_TransType, *PE_TransType;

class __declspec(novtable) I_SQLite3: public IObjRef
{
public:
	//=============================================================================
	//Function:     Initialize
	//Description:	Initialize sqlite3 library
	//				see: http://www.sqlite.org/c3ref/initialize.html
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int Initialize(void) = 0;

	//=============================================================================
	//Function:     Shutdown
	//Description:	Shutdown sqlite3 library
	//				see: http://www.sqlite.org/c3ref/initialize.html
	//
	//Return:
	//		SQLITE_*          
	//=============================================================================
	virtual int Shutdown(void) = 0;

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
	virtual int Config(int op) = 0;

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
	virtual int Config(int op, int i32Value) = 0;

	// Database file
	//=============================================================================
	//Function:     Open
	//Description:	Open database file
	//
	//Parameter:
	//	pszFile    - Database file as UTF8
	//
	//Return:
	//		SQLITE_*              
	//=============================================================================
	virtual int	Open(const char* pszFile, int i32Flag = SQLITE_OPEN_FLAG_DEFAULT) = 0;	

	//=============================================================================
	//Function:     GetFileName
	//Description:	Returns a pointer to a filename associated with database pszDbName of connected.
	//				See: http://www.sqlite.org/c3ref/db_filename.html
	//
	//Parameter:
	//	pszDbName    -  Database Name, "main" or attach as database name
	//
	//Return:
	//		Returns a pointer to a filename associated with database pszDbName of connected.    
	//=============================================================================
	virtual const char* GetFile(const char* pszAsDbName = NULL) = 0;

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
	virtual int	Attach(const char* pszFile, const char* pszAsDb, const char* pszKey=NULL) = 0;

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
	virtual int	Detach(const char* pszAsDb) = 0;

	//=============================================================================
	//Function:     Close
	//Description:	Close database
	//				see: http://www.sqlite.org/c3ref/close.html
	//
	//
	//Return:
	//     SQLITE_*         
	//=============================================================================
	virtual int	Close() = 0;

	//=============================================================================
	//Function:     GetHandle
	//Description:	Get the current database handle
	//
	//
	//Return:
	//       Current database handle when opened, else is NULL       
	//=============================================================================
	virtual sqlite3* GetDbHandle() = 0;

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
	virtual int	Key(const void* pKey, int nKey) = 0;
	
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
	virtual int Rekey(const void* pKey, int nKey) = 0;
	
	//
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
	virtual int	SQL(const char* pszSql, bool bFree = false, sqlite3_callback pfnCallback = NULL, void* pvData = NULL) = 0;

	//=============================================================================
	//Function:     GetTable
	//Description: Get a table by SQL
	//
	//Parameter:
	//	pszSql    - SQL statement, UTF-8 encoded 
	//  bFree	  - true: auto free memory of pszSql
	//Return:
	//		The table interface when success, else is NULL          
	//=============================================================================
	virtual I_SQLite3Table* GetTable(const char* pszSql, bool bFree = false, int* pi32Ret = NULL) = 0;

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
	virtual TCHAR*	GetErrorMsg(int i32ErrorCode) = 0;

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
	virtual BOOL IsExistTable(const TCHAR* pszTableName) = 0;

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
	virtual BOOL IsAutoCommit() = 0;

	//=============================================================================
	//Function:     GetVersion
	//Description:	Get SQLite3 version number
	//		eg.:  "3.7.17" as 3007017, see: http://www.sqlite.org/c3ref/c_source_id.html
	//
	//Return:
	//		The version number          
	//=============================================================================
	virtual int	GetVersion() = 0;					// Get sqlite version number

	//=============================================================================
	//Function:     GetLastRowId
	//Description:	Get last RowId
	//				see: http://www.sqlite.org/c3ref/last_insert_rowid.html
	//
	//
	//Return:
	//      The last RowId        
	//=============================================================================
	virtual INT64 GetLastRowId() = 0;

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
	virtual int SetBusyTimeout(int i32Ms) = 0;
	
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
	virtual int	SetBusyHandler(int(*pfnBusy)(void*,int), void* pvArg) = 0;

	//=============================================================================
	//Function:     Interrupt
	//Description:	Set interrupt flag
	//				see: http://www.sqlite.org/c3ref/interrupt.html 
	//
	//Return:
	//              None
	//=============================================================================
	virtual void Interrupt() = 0;

	//=============================================================================
	//Function:     Changes
	//Description:	returns the number of database rows that were changed or inserted 
	//				or deleted by the most recently completed SQL statement.
	//				see: http://www.sqlite.org/c3ref/changes.html
	//
	//Return:
	//		The rows number changed last modify             
	//=============================================================================
	virtual int	Changes() = 0;						// Return the number of changes in the most recent call to sqlite3_exec().

	//=============================================================================
	//Function:     TotalChanges
	//Description:	returns the number of row changes caused by INSERT, UPDATE or 
	//				DELETE statements since the database connection was opened.
	//				see: http://www.sqlite.org/c3ref/total_changes.html
	//
	//Return:
	//		The  total number of changed since opened          
	//=============================================================================
	virtual int	TotalChanges() = 0;					// Return the number of changes since the database handle was opened.

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
	virtual char* VMPrintf(const char* pszFmt, ...) = 0;

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
	virtual void Free(void* pvData) = 0;

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
	virtual int	Begin(E_TransType eTransType = eTRANS_IMMEDIATE) = 0;						// Begin transaction

	//=============================================================================
	//Function:     Compile
	//Description:	Compile a statement
	//				see: http://www.sqlite.org/c3ref/prepare.html
	//
	//Parameter:
	//	pszSql    - SQL statement
	//  bFree	  - true: auto free memory of pszSql
	//
	//Return:
	//      The statement interface when succeeded, else is NULL        
	//=============================================================================
	virtual I_SQLite3Stmt* Compile(const char* pszSql, bool bFree = false, int* pi32Ret = NULL) = 0;	// Compile statement

	//=============================================================================
	//Function:     Commit
	//Description:	Commit transaction
	//				see: http://www.sqlite.org/lang_transaction.html
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int	Commit() = 0;						// Commit transaction

	//=============================================================================
	//Function:     Rollback
	//Description:	Rollback transaction
	//				see: http://www.sqlite.org/lang_transaction.html
	//
	//Return:
	//              
	//=============================================================================
	virtual int	Rollback() = 0;						// Rollback transaction 

};

// Ths function of GetCol*'s index base with 0 of I_SQLite3Table
class __declspec(novtable) I_SQLite3Table: public IObjRef
{
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
	virtual BOOL SetDbHandle(sqlite3* pDb) = 0;

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
	virtual BOOL SetStmtHandle(sqlite3_stmt* pStmt) = 0;

	//=============================================================================
	//Function:     GetStmtHandle
	//Description:	Get statement handle
	//
	//
	//Return:
	//		The statement handle          
	//=============================================================================
	virtual sqlite3_stmt* GetStmtHandle() = 0;

	//=============================================================================
	//Function:     Finalize
	//Description:	Finalize the statement
	//				see: http://www.sqlite.org/c3ref/finalize.html
	//
	//Return:
	//		SQLITE_*         
	//=============================================================================
	virtual int Finalize() = 0;

	//=============================================================================
	//Function:     GetCols
	//Description:	Get table columns
	//				see: http://www.sqlite.org/c3ref/column_count.html
	//
	//Return:
	//		The columns          
	//=============================================================================
	virtual int	GetCols() = 0;

	//=============================================================================
	//Function:     Step
	//Description:	Do a step action
	//				see: http://www.sqlite.org/c3ref/step.html
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int	Step() = 0;

	// Column information
	//=============================================================================
	//Function:     GetColName
	//Description:	Get column's name as UTF8
	//				see: http://www.sqlite.org/c3ref/column_name.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The column name as UTF8        
	//=============================================================================
	virtual const char* GetColName  ( int i32Col ) = 0;
		
	//=============================================================================
	//Function:     GetColName16
	//Description:	Get a column's name as UTF16
	//				see: http://www.sqlite.org/c3ref/column_name.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The culumn name as UTF16            
	//=============================================================================
	virtual const void* GetColName16( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColType
	//Description:	Get column's type
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//     SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL.          
	//=============================================================================
	virtual int	GetColType  ( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColDeclType
	//Description:	Get column's declare type as UTF8
	//				see: http://www.sqlite.org/c3ref/column_decltype.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		Type name          
	//=============================================================================
	virtual const char *GetColDeclType  (int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColDeclType16
	//Description:	Get column's declare type as UTF16
	//				see: http://www.sqlite.org/c3ref/column_decltype.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		Type name           
	//=============================================================================
	virtual const void *GetColDeclType16(int i32Col ) = 0;

	// Column value
	//=============================================================================
	//Function:     GetColBlob
	//Description:	Get a column's data as blob
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The blob data         
	//=============================================================================
	virtual const void*	GetColBlob   ( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColBytes
	//Description:	Get column's data as byte
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The bytes data         
	//=============================================================================
	virtual int	GetColBytes  ( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColBytes16
	//Description:	Get column's data as word
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The word data       
	//=============================================================================
	virtual int	GetColBytes16( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColInt
	//Description:	Get column's data as integer
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The int data           
	//=============================================================================
	virtual int	GetColInt    ( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColInt64
	//Description:	Get column's data as int64
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The int64 data           
	//=============================================================================
	virtual INT64 GetColInt64  ( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColDbl
	//Description:	Get column's data as double
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The double data          
	//=============================================================================
	virtual double GetColDbl    ( int i32Col ) = 0;

	//=============================================================================
	//Function:     GetColText
	//Description:	Get column's data as UTF8
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The text data as UTF8      
	//=============================================================================
	virtual const unsigned char* GetColText( int i32Col ) = 0;
	
	//=============================================================================
	//Function:     GetColText16
	//Description:	Get column's data as UTF16
	//				see: http://www.sqlite.org/c3ref/column_blob.html
	//
	//Parameter:
	//	i32Col    - Column index, base with 0
	//
	//Return:
	//		The text data as UTF16          
	//=============================================================================
	virtual const void* GetColText16 ( int i32Col ) = 0;

};

// Ths function of Bind's index base with 1 of I_SQLite3Table
class __declspec(novtable) I_SQLite3Stmt: public IObjRef
{
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
	virtual BOOL SetDbHandle(sqlite3* pDb) = 0;

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
	virtual BOOL SetStmtHandle(sqlite3_stmt* pStmt) = 0;
		
	//=============================================================================
	//Function:     GetStmtHandle
	//Description:	Get statement handle
	//
	//
	//Return:
	//		The statement handle          
	//=============================================================================
	virtual sqlite3_stmt* GetStmtHandle() = 0;

	//=============================================================================
	//Function:     Finalize
	//Description:	Finalize the statement
	//				see: http://www.sqlite.org/c3ref/finalize.html
	//
	//Return:
	//		SQLITE_*         
	//=============================================================================
	virtual int Finalize() = 0;

	//=============================================================================
	//Function:     Step
	//Description:	Do a step action
	//				see: http://www.sqlite.org/c3ref/step.html
	//
	//Return:
	//		SQLITE_*           
	//=============================================================================
	virtual int	Step(int* pRowsChanged = NULL, BOOL bAutoReset = TRUE) = 0;

	//=============================================================================
	//Function:     Reset
	//Description:	Reset the statement
	//				see: http://www.sqlite.org/c3ref/reset.html
	//
	//Return:
	//		SQLITE_*            
	//=============================================================================
	virtual int Reset() = 0;

	//=============================================================================
	//Function:     GetBindParamName
	//Description:	Get bind parameter's name by index as UTF8
	//				see: http://www.sqlite.org/c3ref/bind_parameter_name.html
	//Parameter:
	//	i32Index    - Index of bind，begin with 1
	//
	//Return:
	//		The bind name as UTF8	            
	//=============================================================================
	virtual const char* GetBindParamName(int i32Index) = 0;

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
	virtual int	GetBindParamIndex( const char* pszName ) = 0;

	//=============================================================================
	//Function:     GetBindParamCount
	//Description:	Get bind parameter's count
	//				see: http://www.sqlite.org/c3ref/bind_parameter_count.html
	//
	//Return:
	//		The bind count          
	//=============================================================================
	virtual int	GetBindParamCount( ) = 0;

	// Bind by index of column

	// Bind by index of column
	//=============================================================================
	//Function:     Bind
	//Description:	Bind int value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	i32Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const int    i32Value) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind int64 value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	i64Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const INT64  i64Value) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind double value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	dValue      - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const double dValue  ) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind text(UTF8) value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	pszValue    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const char*  pszValue) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind blob value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	pvValue     - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index, const void*  pvValue, int nLen) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind NULL value by index
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( int i32Index) = 0;

	// Bind by name of column
	//=============================================================================
	//Function:     Bind
	//Description:	Bind int value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	i32Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const int    i32Value) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind int64 value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	i64Value    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const INT64  i64Value) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind double value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	dValue      - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const double dValue  ) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind text(UTF8) value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	pszValue    - Value to bind
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const char*  pszValue) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind blob value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//	pvValue     - Value to bind
	//  nLen		- Length of pvValue
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName, const void*  pvValue, int nLen) = 0;

	//=============================================================================
	//Function:     Bind
	//Description:	Bind NULL value by name
	//				see: http://www.sqlite.org/c3ref/bind_blob.html
	//
	//Parameter:
	//	i32Index    - Index to bind, base with 1
	//
	//Return:
	//		SQLITE_*        
	//=============================================================================
	virtual int Bind( const char* pszName) = 0;

};

#endif // __IEIM_SQLITE_INTERFACE_HEADER_2017_12_18_YFGZ__