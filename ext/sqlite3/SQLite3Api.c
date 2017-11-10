//#include "StdAfx.h"
#include <tchar.h>
#include <stdio.h>
#include "SQLite3Api.h"

static S_SQLite3Api gs;


BOOL Load(const TCHAR* pSqlite3Dll )
{
	memset(&gs, 0, sizeof(gs));
#define GET_API_RET_(V, T, N)	do{ (V) = (T)GetProcAddress(gs.m_hSQLite3, N); if ((V) == NULL) { _tprintf(_T("GetProcAddress(%S) failed!\n"), N); } }while(0)
	if ( pSqlite3Dll == NULL ) pSqlite3Dll = _T("sqlite3.dll");

	gs.m_hSQLite3 = LoadLibrary(pSqlite3Dll);
	if ( gs.m_hSQLite3 == NULL )
		return FALSE;

	GET_API_RET_(gs.m_pfnopen16					,pfn_open16					,"sqlite3_open16");
	GET_API_RET_(gs.m_pfnclose					,pfn_close					,"sqlite3_close");
	GET_API_RET_(gs.m_pfnexec					,pfn_exec					,"sqlite3_exec");
	GET_API_RET_(gs.m_pfncomplete				,pfn_complete				,"sqlite3_complete");
	GET_API_RET_(gs.m_pfncomplete16				,pfn_complete16				,"sqlite3_complete16");
	GET_API_RET_(gs.m_pfnfinalize				,pfn_finalize				,"sqlite3_finalize");
	GET_API_RET_(gs.m_pfnfree					,pfn_free					,"sqlite3_free");
	GET_API_RET_(gs.m_pfnfree_table				,pfn_free_table				,"sqlite3_free_table");
	GET_API_RET_(gs.m_pfnget_table				,pfn_get_table				,"sqlite3_get_table");
	GET_API_RET_(gs.m_pfnkey					,pfn_key					,"sqlite3_key");
	GET_API_RET_(gs.m_pfnrekey					,pfn_key					,"sqlite3_rekey");
	GET_API_RET_(gs.m_pfnreset					,pfn_reset					,"sqlite3_reset");
	GET_API_RET_(gs.m_pfnchanges				,pfn_changes				,"sqlite3_changes");
	GET_API_RET_(gs.m_pfnstep					,pfn_step					,"sqlite3_step");
	GET_API_RET_(gs.m_pfncolumn_count			,pfn_column_count			,"sqlite3_column_count");
	GET_API_RET_(gs.m_pfnbind_int				,pfn_bind_int				,"sqlite3_bind_int");
	GET_API_RET_(gs.m_pfnbind_text				,pfn_bind_text				,"sqlite3_bind_text");
	GET_API_RET_(gs.m_pfnbind_text16			,pfn_bind_text16			,"sqlite3_bind_text16");
	GET_API_RET_(gs.m_pfnbind_parameter_count	,pfn_bind_parameter_count	,"sqlite3_bind_parameter_count");
	GET_API_RET_(gs.m_pfnbind_parameter_index	,pfn_bind_parameter_index	,"sqlite3_bind_parameter_index");
	GET_API_RET_(gs.m_pfnbind_parameter_name	,pfn_bind_parameter_name	,"sqlite3_bind_parameter_name");
	GET_API_RET_(gs.m_pfncolumn_name			,pfn_column_name			,"sqlite3_column_name");
	GET_API_RET_(gs.m_pfncolumn_name16			,pfn_column_name16			,"sqlite3_column_name16");
	GET_API_RET_(gs.m_pfncolumn_text			,pfn_column_text			,"sqlite3_column_text");
	GET_API_RET_(gs.m_pfncolumn_text16			,pfn_column_text16			,"sqlite3_column_text16");
	GET_API_RET_(gs.m_pfncolumn_type			,pfn_column_type			,"sqlite3_column_type");
	GET_API_RET_(gs.m_pfnprepare				,pfn_prepare				,"sqlite3_prepare");
	GET_API_RET_(gs.m_pfnprepare16				,pfn_prepare16				,"sqlite3_prepare16");
	GET_API_RET_(gs.m_pfnerrcode				,pfn_errcode				,"sqlite3_errcode");
	GET_API_RET_(gs.m_pfnerrmsg					,pfn_errmsg					,"sqlite3_errmsg");
	GET_API_RET_(gs.m_pfnerrmsg16				,pfn_errmsg16				,"sqlite3_errmsg16");
	//////////////////////////////
	GET_API_RET_(gs.m_pfnbackup_init			,pfn_backup_init			,"sqlite3_backup_init");
	GET_API_RET_(gs.m_pfnbackup_finish			,pfn_backup_finish			,"sqlite3_backup_finish");
	GET_API_RET_(gs.m_pfnbackup_step			,pfn_backup_step			,"sqlite3_backup_step");
	GET_API_RET_(gs.m_pfnbusy_timeout			,pfn_busy_timeout			,"sqlite3_busy_timeout");
	GET_API_RET_(gs.m_pfnbind_null				,pfn_bind_null				,"sqlite3_bind_null");
	GET_API_RET_(gs.m_pfnbind_blob				,pfn_bind_blob				,"sqlite3_bind_blob");
	GET_API_RET_(gs.m_pfnbind_double			,pfn_bind_double			,"sqlite3_bind_double");
	GET_API_RET_(gs.m_pfnbind_int64				,pfn_bind_int64				,"sqlite3_bind_int64");
	GET_API_RET_(gs.m_pfnconfig					,pfn_config					,"sqlite3_config");
	GET_API_RET_(gs.m_pfncolumn_double			,pfn_column_double			,"sqlite3_column_double");
	GET_API_RET_(gs.m_pfncolumn_bytes			,pfn_column_bytes			,"sqlite3_column_bytes");
	GET_API_RET_(gs.m_pfncolumn_blob			,pfn_column_blob			,"sqlite3_column_blob");
	GET_API_RET_(gs.m_pfncolumn_int				,pfn_column_int				,"sqlite3_column_int");
	GET_API_RET_(gs.m_pfncolumn_int64			,pfn_column_int64			,"sqlite3_column_int64");
	GET_API_RET_(gs.m_pfncreate_function		,pfn_create_function		,"sqlite3_create_function");
	GET_API_RET_(gs.m_pfnextended_errcode		,pfn_extended_errcode		,"sqlite3_extended_errcode");
	GET_API_RET_(gs.m_pfndb_status				,pfn_db_status				,"sqlite3_db_status");
	GET_API_RET_(gs.m_pfninitialize				,pfn_initialize				,"sqlite3_initialize");
	GET_API_RET_(gs.m_pfnfile_control			,pfn_file_control			,"sqlite3_file_control");
	GET_API_RET_(gs.m_pfnget_autocommit			,pfn_get_autocommit			,"sqlite3_get_autocommit");
	GET_API_RET_(gs.m_pfnlibversion				,pfn_libversion				,"sqlite3_libversion");
	GET_API_RET_(gs.m_pfnopen					,pfn_open					,"sqlite3_open");
	GET_API_RET_(gs.m_pfnresult_text			,pfn_result_text			,"sqlite3_result_text");
	GET_API_RET_(gs.m_pfnresult_blob			,pfn_result_blob			,"sqlite3_result_blob");
	GET_API_RET_(gs.m_pfnresult_int64			,pfn_result_int64			,"sqlite3_result_int64");
	GET_API_RET_(gs.m_pfnstrglob				,pfn_strglob				,"sqlite3_strglob");
	GET_API_RET_(gs.m_pfnsleep					,pfn_sleep					,"sqlite3_sleep");
	GET_API_RET_(gs.m_pfnsnprintf				,pfn_snprintf				,"sqlite3_snprintf");
	GET_API_RET_(gs.m_pfnsourceid				,pfn_sourceid				,"sqlite3_sourceid");
	GET_API_RET_(gs.m_pfnsql					,pfn_sql					,"sqlite3_sql");
	GET_API_RET_(gs.m_pfnstmt_status			,pfn_stmt_status			,"sqlite3_stmt_status");
	GET_API_RET_(gs.m_pfnstatus					,pfn_status					,"sqlite3_status");
	GET_API_RET_(gs.m_pfnstrnicmp				,pfn_strnicmp				,"sqlite3_strnicmp");
	GET_API_RET_(gs.m_pfnstricmp				,pfn_stricmp				,"sqlite3_stricmp");
	GET_API_RET_(gs.m_pfnrealloc				,pfn_realloc				,"sqlite3_realloc");
	GET_API_RET_(gs.m_pfnmalloc					,pfn_malloc					,"sqlite3_malloc");
	GET_API_RET_(gs.m_pfnmprintf				,pfn_mprintf				,"sqlite3_mprintf");
	GET_API_RET_(gs.m_pfnvmprintf				,pfn_vmprintf				,"sqlite3_vmprintf");
	GET_API_RET_(gs.m_pfntest_control			,pfn_test_control			,"sqlite3_test_control");
	GET_API_RET_(gs.m_pfntrace					,pfn_trace					,"sqlite3_trace");
	GET_API_RET_(gs.m_pfnprepare_v2				,pfn_prepare_v2				,"sqlite3_prepare_v2");
	GET_API_RET_(gs.m_pfnvalue_text				,pfn_value_text				,"sqlite3_value_text");
	GET_API_RET_(gs.m_pfnvalue_bytes			,pfn_value_bytes			,"sqlite3_value_bytes");
	GET_API_RET_(gs.m_pfnvalue_blob				,pfn_value_blob				,"sqlite3_value_blob");
	GET_API_RET_(gs.m_pfnvfs_find				,pfn_vfs_find				,"sqlite3_vfs_find");
	GET_API_RET_(gs.m_pfnvfs_register			,pfn_vfs_register			,"sqlite3_vfs_register");
	GET_API_RET_(gs.m_pfnvfs_unregister			,pfn_vfs_unregister			,"sqlite3_vfs_unregister");

	return TRUE;
}

BOOL Unload()
{
	if ( gs.m_hSQLite3 )
		FreeLibrary(gs.m_hSQLite3);

	memset(&gs, 0, sizeof(gs));
	return TRUE;
}

int  sqlite3_open16(const void *file,sqlite3** db) {
	if ( gs.m_pfnopen16 ) return gs.m_pfnopen16(file, db);
	return SQLITE_ERROR;
}

int  sqlite3_close(sqlite3* db) {
	if ( gs.m_pfnclose ) return gs.m_pfnclose(db);
	return SQLITE_ERROR;
}

int  sqlite3_exec(sqlite3* db,const char* sql, sqlite3_callback cb, void* user, char** errmsg) {
	if (gs.m_pfnexec) return gs.m_pfnexec(db, sql, cb, user, errmsg);
	return SQLITE_ERROR;
}

int  sqlite3_complete(const char* sql) {
	if (gs.m_pfncomplete)  return gs.m_pfncomplete(sql);
	return SQLITE_ERROR;
}

int  sqlite3_complete16(const void* sql) {
	if (gs.m_pfncomplete16) return gs.m_pfncomplete16(sql);
	return SQLITE_ERROR;
}

int  sqlite3_finalize(sqlite3_stmt* pStmt) {
	if ( gs.m_pfnfinalize ) return gs.m_pfnfinalize(pStmt);
	return SQLITE_ERROR;
}

void sqlite3_free(void* p) {
	if (gs.m_pfnfree) gs.m_pfnfree(p);
}

void sqlite3_free_table(char**result) {
	if (gs.m_pfnfree_table) gs.m_pfnfree_table(result);
}

int  sqlite3_get_table(sqlite3* db,const char* zSql,char*** pazResult,int* pnRow,int* pnColumn,char** pzErrmsg) {
	if (gs.m_pfnget_table) return gs.m_pfnget_table(db, zSql, pazResult, pnRow, pnColumn,pzErrmsg);
	return SQLITE_ERROR;
}

int  sqlite3_key(sqlite3 *db, const void *pKey, int nKey) {
	if (gs.m_pfnkey) return gs.m_pfnkey(db, pKey, nKey);
	return SQLITE_ERROR;
}

int  sqlite3_rekey(sqlite3 *db, const void *pKey, int nKey) {
	if (gs.m_pfnrekey) return gs.m_pfnrekey(db, pKey, nKey);
	return SQLITE_ERROR;
}

int  sqlite3_reset(sqlite3_stmt*pStmt) {
	if (gs.m_pfnreset) return gs.m_pfnreset(pStmt);
	return SQLITE_ERROR;
}

int  sqlite3_changes(sqlite3* db) {
	if (gs.m_pfnchanges) return gs.m_pfnchanges(db);
	return SQLITE_ERROR;
}

int  sqlite3_step(sqlite3_stmt* pStmt) {
	if (gs.m_pfnstep) return gs.m_pfnstep(pStmt);
	return SQLITE_ERROR;
}

int  sqlite3_column_count(sqlite3_stmt* pStmt) {
	if ( gs.m_pfncolumn_count ) return gs.m_pfncolumn_count(pStmt);
	return SQLITE_ERROR;
}

int  sqlite3_bind_int(sqlite3_stmt* pStmt, int i, int v) {
	if (gs.m_pfnbind_int) return gs.m_pfnbind_int(pStmt, i, v);
	return SQLITE_ERROR;
}

int  sqlite3_bind_text(sqlite3_stmt* pStmt, int i,const char* zData, int n, void(*xDel)(void*)) {
	if ( gs.m_pfnbind_text ) return gs.m_pfnbind_text(pStmt, i, zData, n, xDel);
	return SQLITE_ERROR;
}

int  sqlite3_bind_text16(sqlite3_stmt* pStmt, int i, const void* zData, int n, void(*xDel)(void*)) {
	if (gs.m_pfnbind_text16) return gs.m_pfnbind_text16(pStmt, i, zData, n, xDel);
	return SQLITE_ERROR;

}

//int  bind_value(sqlite3_stmt* pStmt,int i,const sqlite3_value* v) {
//
//}
int  sqlite3_bind_parameter_count(sqlite3_stmt* pStmt) {
	if ( gs.m_pfnbind_parameter_count ) return gs.m_pfnbind_parameter_count(pStmt);
	return SQLITE_ERROR;
}

int  sqlite3_bind_parameter_index(sqlite3_stmt* pStmt,const char*zName) {
	if ( gs.m_pfnbind_parameter_index ) return gs.m_pfnbind_parameter_index(pStmt, zName);
	return SQLITE_ERROR;
}

const char* sqlite3_bind_parameter_name(sqlite3_stmt* pStmt,int i) {
	if ( gs.m_pfnbind_parameter_name ) return gs.m_pfnbind_parameter_name( pStmt, i);
	return NULL;
}

const char*	sqlite3_column_name(sqlite3_stmt* pStmt,int iCol) {
	if ( gs.m_pfncolumn_name ) return gs.m_pfncolumn_name(pStmt, iCol);
	return NULL;
}

const void*	sqlite3_column_name16(sqlite3_stmt* pStmt,int iCol) {
	if ( gs.m_pfncolumn_name16 ) return gs.m_pfncolumn_name16(pStmt, iCol);
	return NULL;
}

const unsigned char* sqlite3_column_text(sqlite3_stmt* pStmt,int iCol) {
	if ( gs.m_pfncolumn_text ) return gs.m_pfncolumn_text(pStmt, iCol);
	return NULL;
}

const void* sqlite3_column_text16(sqlite3_stmt* pStmt,int iCol) {
	if ( gs.m_pfncolumn_text16 ) return gs.m_pfncolumn_text16(pStmt, iCol);
	return NULL;
}

int	sqlite3_column_type(sqlite3_stmt* pStmt,int iCol) {
	if ( gs.m_pfncolumn_type ) return gs.m_pfncolumn_type( pStmt, iCol);
	return SQLITE_ERROR;
}

int sqlite3_prepare(sqlite3* db, const char* zSql, int nByte,sqlite3_stmt** pStmt,const char** pzTail) {
	if ( gs.m_pfnprepare ) return gs.m_pfnprepare(db, zSql, nByte, pStmt, pzTail);
	return SQLITE_ERROR;
}

int sqlite3_prepare16(sqlite3* db, const void* zSql, int nByte, sqlite3_stmt** pStmt,const void** pzTail) {
	if ( gs.m_pfnprepare16 ) return gs.m_pfnprepare16(db, zSql, nByte, pStmt, pzTail);
	return SQLITE_ERROR;
}

int sqlite3_errcode(sqlite3*db) {
	if ( gs.m_pfnerrcode ) return gs.m_pfnerrcode(db);
	return SQLITE_ERROR;
}

const char* sqlite3_errmsg(sqlite3* db) {
	if ( gs.m_pfnerrmsg ) return gs.m_pfnerrmsg(db);
	return NULL;
}

const void* sqlite3_errmsg16(sqlite3* db) {
	if ( gs.m_pfnerrmsg16 ) return gs.m_pfnerrmsg16(db);
	return NULL;
}

const char *sqlite3_libversion(void){ 
	if ( gs.m_pfnlibversion ) return gs.m_pfnlibversion();
	return NULL;
}

sqlite3_backup*	sqlite3_backup_init(sqlite3 *pDest, const char *zDestName, sqlite3 *pSource, const char *zSourceName) {
	if ( gs.m_pfnbackup_init ) return gs.m_pfnbackup_init(pDest, zDestName, pSource, zSourceName);
	return NULL;
}

int	sqlite3_backup_finish(sqlite3_backup *p) {
	if ( gs.m_pfnbackup_finish ) return gs.m_pfnbackup_finish(p);
	return SQLITE_ERROR;
}

int sqlite3_backup_step(sqlite3_backup *p, int nPage){
	if ( gs.m_pfnbackup_step) return gs.m_pfnbackup_step(p, nPage);
	return SQLITE_ERROR;
}

int sqlite3_busy_timeout(sqlite3 *db, int ms) {
	if ( gs.m_pfnbusy_timeout ) return gs.m_pfnbusy_timeout(db, ms);
	return SQLITE_ERROR;
}

int sqlite3_bind_null(sqlite3_stmt *pStmt, int i) {
	if ( gs.m_pfnbind_null ) return gs.m_pfnbind_null( pStmt, i);
	return SQLITE_ERROR;
}

int sqlite3_bind_blob(sqlite3_stmt *pStmt, int i, const void *zData, int nData, void (*xDel)(void*)) {
	if ( gs.m_pfnbind_blob ) return gs.m_pfnbind_blob(pStmt, i, zData, nData, xDel);
	return SQLITE_ERROR;
}

int sqlite3_bind_double(sqlite3_stmt *pStmt, int i, double rValue) {
	if ( gs.m_pfnbind_double ) return gs.m_pfnbind_double(pStmt, i, rValue);
	return SQLITE_ERROR;
}

int sqlite3_bind_int64(sqlite3_stmt *pStmt, int i, sqlite_int64 iValue) {
	if ( gs.m_pfnbind_int64 ) return gs.m_pfnbind_int64(pStmt, i, iValue);
	return SQLITE_ERROR;
}

int sqlite3_config(int op, ...) {
	if ( gs.m_pfnconfig ) {
		int n;
		va_list args;
		va_start(args, op);
		n = gs.m_pfnconfig(op, args);
		va_end(args);
		return n;
	}

	return SQLITE_ERROR;
}

double sqlite3_column_double(sqlite3_stmt *pStmt, int i) {
	if ( gs.m_pfncolumn_double ) return gs.m_pfncolumn_double(pStmt, i);
	return 0.0;
}

int sqlite3_column_bytes(sqlite3_stmt* pStmt, int iCol) {
	if ( gs.m_pfncolumn_bytes ) return gs.m_pfncolumn_bytes(pStmt, iCol);
	return 0;
}

const void* sqlite3_column_blob(sqlite3_stmt* pStmt, int iCol) {
	if ( gs.m_pfncolumn_blob ) return gs.m_pfncolumn_blob(pStmt, iCol);
	return NULL;
}

int  sqlite3_column_int(sqlite3_stmt *pStmt, int i) {
	if ( gs.m_pfncolumn_int ) return gs.m_pfncolumn_int(pStmt, i );
	return 0;
}

sqlite_int64 sqlite3_column_int64(sqlite3_stmt *pStmt, int i){
	if ( gs.m_pfncolumn_int64 ) return gs.m_pfncolumn_int64(pStmt, i );
	return 0;
}

int	sqlite3_create_function(sqlite3 *db, const char *zFunc, int nArg, int enc, void *p, void (*xFunc)(sqlite3_context*,int,sqlite3_value **), void (*xStep)(sqlite3_context*,int,sqlite3_value **), void (*xFinal)(sqlite3_context*)) {
	if ( gs.m_pfncreate_function ) return gs.m_pfncreate_function(db, zFunc, nArg, enc, p, xFunc, xStep, xFinal);
	return SQLITE_ERROR;
}

int	sqlite3_extended_errcode(sqlite3 *db) {
	if ( gs.m_pfnextended_errcode ) return gs.m_pfnextended_errcode(db);
	return SQLITE_ERROR;
}

int sqlite3_db_status(sqlite3* db, int op, int *pCur, int *pHiwtr, int resetFlg) {
	if ( gs.m_pfndb_status ) return gs.m_pfndb_status(db, op, pCur, pHiwtr, resetFlg);
	return SQLITE_ERROR;
}

int sqlite3_initialize(void) {
	if ( gs.m_pfninitialize ) return gs.m_pfninitialize();
	return SQLITE_ERROR;
}

int sqlite3_file_control(sqlite3 *db, const char *zDbName, int op, void *pArg) {
	if ( gs.m_pfnfile_control ) return gs.m_pfnfile_control(db, zDbName, op, pArg);
	return SQLITE_ERROR;
}

int	sqlite3_get_autocommit(sqlite3 *db) {
	if ( gs.m_pfnget_autocommit ) return gs.m_pfnget_autocommit(db);
	return SQLITE_ERROR;
}

int sqlite3_open(const char *filename, sqlite3 **ppDb) {
	if ( gs.m_pfnopen ) return gs.m_pfnopen(filename, ppDb);
	return SQLITE_ERROR;
}

void sqlite3_result_text(sqlite3_context *pCtx, const char *z, int n, void (*xDel)(void *)) {
	if ( gs.m_pfnresult_text ) gs.m_pfnresult_text(pCtx, z, n, xDel);
}

void sqlite3_result_blob(sqlite3_context *pCtx, const void *z, int n, void (*xDel)(void *)) {
	if ( gs.m_pfnresult_blob ) gs.m_pfnresult_blob(pCtx, z, n, xDel);
}

void sqlite3_result_int64(sqlite3_context *pCtx, sqlite3_int64 iVal) {
	if ( gs.m_pfnresult_int64 ) gs.m_pfnresult_int64(pCtx, iVal);
}

int sqlite3_strglob(const char *zGlobPattern, const char *zString) {
	if ( gs.m_pfnstrglob ) return gs.m_pfnstrglob (zGlobPattern, zString);
	return 0;
}

int sqlite3_sleep(int ms) {
	if ( gs.m_pfnsleep ) return gs.m_pfnsleep(ms);
	return SQLITE_ERROR;
}

char* sqlite3_snprintf(int n, char *zBuf, const char *zFormat, ...) {
	if ( gs.m_pfnsnprintf ) {
		char* p;
		va_list args;
		va_start(args, zFormat);
		p = gs.m_pfnsnprintf(n, zBuf, zFormat, args);
		va_end(args);
		return p;
	}

	return NULL;
}

const char*	sqlite3_sourceid(void) {
	if ( gs.m_pfnsourceid ) return gs.m_pfnsourceid();
	return NULL;
}

const char* sqlite3_sql(sqlite3_stmt *pStmt) {
	if ( gs.m_pfnsql ) return gs.m_pfnsql(pStmt);
	return NULL;
}

int	sqlite3_stmt_status(sqlite3_stmt* pStmt, int op,int resetFlg) {
	if ( gs.m_pfnstmt_status ) return gs.m_pfnstmt_status(pStmt, op, resetFlg);
	return SQLITE_ERROR;
}

int sqlite3_status(int op, int *pCurrent, int *pHighwater, int resetFlag) {
	if ( gs.m_pfnstatus ) return gs.m_pfnstatus(op, pCurrent, pHighwater, resetFlag);
	return SQLITE_ERROR;
}

int sqlite3_strnicmp(const char *zLeft, const char *zRight, int n) {
	if ( gs.m_pfnstrnicmp ) return gs.m_pfnstrnicmp(zLeft, zRight, n);
	return SQLITE_ERROR;
}

int sqlite3_stricmp(const char *zLeft, const char *zRight) {
	if ( gs.m_pfnstricmp ) return gs.m_pfnstricmp(zLeft, zRight);
	return SQLITE_ERROR;
}


void* sqlite3_realloc(void *pOld, int n) {
	if ( gs.m_pfnrealloc ) return gs.m_pfnrealloc(pOld, n);
	return NULL;
}

void* sqlite3_malloc(int n) {
	if ( gs.m_pfnmalloc )  return gs.m_pfnmalloc(n);
	return NULL;
}

char *sqlite3_vmprintf(const char *zFormat, va_list ap) {
	if ( gs.m_pfnvmprintf ) return gs.m_pfnvmprintf(zFormat, ap);
	return NULL;
}

char* sqlite3_mprintf(const char *zFormat, ...) {
	if ( gs.m_pfnmprintf ) {
		char* p = NULL;
		va_list args;
		va_start(args, zFormat);
		p = sqlite3_vmprintf(zFormat, args);
		va_end(args);
		return p;
	}
	return NULL;
}

int sqlite3_test_control(int op, ...) {
	if ( gs.m_pfntest_control ) {
		int n ;
		va_list args;
		va_start(args, op);
		n = gs.m_pfntest_control(op, args);
		va_end(args);
		return n;
	}

	return 0;
}

void* sqlite3_trace(sqlite3 *db, void (*xTrace)(void*,const char*), void *pArg) {
	if ( gs.m_pfntrace ) return gs.m_pfntrace(db, xTrace, pArg);
	return NULL;
}

int	sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail) {
	if ( gs.m_pfnprepare_v2 ) return gs.m_pfnprepare_v2(db, zSql, nByte, ppStmt, pzTail);
	return SQLITE_ERROR;
}

const unsigned char* sqlite3_value_text(sqlite3_value *pVal) {
	if ( gs.m_pfnvalue_text ) return gs.m_pfnvalue_text(pVal);
	return NULL;
}

int	sqlite3_value_bytes(sqlite3_value *pVal) {
	if ( gs.m_pfnvalue_bytes ) return gs.m_pfnvalue_bytes(pVal);
	return 0;
}

const void* sqlite3_value_blob(sqlite3_value *pVal) {
	if ( gs.m_pfnvalue_blob ) return gs.m_pfnvalue_blob(pVal);
	return NULL;
}

sqlite3_vfs* sqlite3_vfs_find(const char *zVfsName) {
	if ( gs.m_pfnvfs_find ) return gs.m_pfnvfs_find(zVfsName);
	return NULL;
}

int sqlite3_vfs_register(sqlite3_vfs* v, int makeDflt) {
	if ( gs.m_pfnvfs_register ) return gs.m_pfnvfs_register(v, makeDflt);
	return SQLITE_ERROR;
}

int	sqlite3_vfs_unregister(sqlite3_vfs* v) {
	if ( gs.m_pfnvfs_unregister ) gs.m_pfnvfs_unregister(v);
	return SQLITE_ERROR;
}