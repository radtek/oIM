#pragma once
#include <Windows.h>
#include "sqlite3.h"
//#include "sqliteInt.h"

	// 这儿只定义了目前会用到的，其它需要的请追加...
	typedef int			(*pfn_open16)(const void *file, sqlite3**);
	typedef int			(*pfn_close)(sqlite3*);

	typedef int			(*pfn_exec)(sqlite3*,const char*,sqlite3_callback,void*,char**);
	typedef int			(*pfn_complete)(const char*sql);
	typedef int			(*pfn_complete16)(const void*sql);
	typedef int			(*pfn_finalize)(sqlite3_stmt*pStmt);

	typedef void		(*pfn_free)(void*);
	typedef void		(*pfn_free_table)(char**result);
	typedef int			(*pfn_get_table)(sqlite3*,const char*,char***,int*,int*,char**);
	typedef int			(*pfn_key)(sqlite3 *db, const void *pKey, int nKey);
	typedef int			(*pfn_rekey)(sqlite3 *db,  const void *pKey, int nKey);
	typedef int			(*pfn_reset)(sqlite3_stmt*pStmt);
	typedef int			(*pfn_changes)(sqlite3*);
	typedef int			(*pfn_step)(sqlite3_stmt*);

	typedef int			(*pfn_column_count)(sqlite3_stmt*pStmt);
	typedef int			(*pfn_bind_int)(sqlite3_stmt*,int,int);
	typedef int			(*pfn_bind_text)(sqlite3_stmt*,int,const char*,int n,void(*)(void*));
	typedef int			(*pfn_bind_text16)(sqlite3_stmt*,int,const void*,int,void(*)(void*));
	typedef int			(*pfn_bind_parameter_count)(sqlite3_stmt*);
	typedef int			(*pfn_bind_parameter_index)(sqlite3_stmt*,const char*zName);
	typedef const char* (*pfn_bind_parameter_name)(sqlite3_stmt*,int);

	typedef const char* (*pfn_column_name)(sqlite3_stmt*,int);
	typedef const void* (*pfn_column_name16)(sqlite3_stmt*,int);
	typedef const unsigned char* ( *pfn_column_text)(sqlite3_stmt*,int iCol);
	typedef const void* (*pfn_column_text16)(sqlite3_stmt*,int iCol);
	typedef int			(*pfn_column_type)(sqlite3_stmt*,int iCol);

	typedef int			(*pfn_prepare)(sqlite3*,const char*,int,sqlite3_stmt**,const char**);
	typedef int			(*pfn_prepare16)(sqlite3*,const void*,int,sqlite3_stmt**,const void**);

	typedef int			(*pfn_errcode)(sqlite3*db);
	typedef const char* (*pfn_errmsg)(sqlite3*);
	typedef const void*	(*pfn_errmsg16)(sqlite3*);

	/////////////////////////////////////////
	typedef sqlite3_backup*	(*pfn_backup_init)(sqlite3 *pDest, const char *zDestName, sqlite3 *pSource, const char *zSourceName);
	typedef int				(*pfn_backup_finish)(sqlite3_backup *p);
	typedef int				(*pfn_backup_step)(sqlite3_backup *p, int nPage);

	typedef int				(*pfn_busy_timeout)(sqlite3 *db, int ms);
	typedef int				(*pfn_bind_null)(sqlite3_stmt *pStmt, int i);
	typedef int				(*pfn_bind_blob)(sqlite3_stmt *pStmt, int i, const void *zData, int nData, void (*xDel)(void*));
	typedef int				(*pfn_bind_double)(sqlite3_stmt *pStmt, int i, double rValue);
	typedef int				(*pfn_bind_int64)(sqlite3_stmt *pStmt, int i, sqlite_int64 iValue);

	typedef int				(*pfn_config)(int, ...);
	typedef double			(*pfn_column_double)(sqlite3_stmt *pStmt, int i);
	typedef int				(*pfn_column_bytes)(sqlite3_stmt*, int iCol);
	typedef const void*		(*pfn_column_blob)(sqlite3_stmt*, int iCol);
	typedef int				(*pfn_column_int)(sqlite3_stmt *pStmt, int i);
	typedef sqlite_int64	(*pfn_column_int64)(sqlite3_stmt *pStmt, int i);
	typedef int				(*pfn_create_function)(sqlite3 *db, const char *zFunc, int nArg, int enc, void *p, void (*xFunc)(sqlite3_context*,int,sqlite3_value **), void (*xStep)(sqlite3_context*,int,sqlite3_value **), void (*xFinal)(sqlite3_context*));

	typedef int				(*pfn_extended_errcode)(sqlite3 *db);
	typedef int				(*pfn_db_status)(sqlite3*, int op, int *pCur, int *pHiwtr, int resetFlg);
	typedef int				(*pfn_initialize)(void);
	typedef int				(*pfn_file_control)(sqlite3 *db, const char *zDbName, int op, void *pArg);
	typedef int				(*pfn_get_autocommit)(sqlite3 *db);

	typedef	const char*		(*pfn_libversion)(void);

	typedef int				(*pfn_open)(const char *filename, sqlite3 **ppDb);
	typedef void			(*pfn_result_text)(sqlite3_context *pCtx, const char *z, int n, void (*xDel)(void *));
	typedef void			(*pfn_result_blob)(sqlite3_context *pCtx, const void *z, int n, void (*xDel)(void *));
	typedef void			(*pfn_result_int64)(sqlite3_context *pCtx, sqlite3_int64 iVal);

	typedef int				(*pfn_strglob)(const char *zGlobPattern, const char *zString);
	typedef int				(*pfn_sleep)(int ms);
	typedef char*			(*pfn_snprintf)(int,char*,const char*, ...);
	typedef char*			(*pfn_vmprintf)(const char *zFormat, va_list ap);
	typedef const char*		(*pfn_sourceid)(void);
	typedef const char*		(*pfn_sql)(sqlite3_stmt *pStmt);
	typedef int				(*pfn_stmt_status)(sqlite3_stmt*, int op,int resetFlg);
	typedef int				(*pfn_status)(int,int*,int*,int);
	typedef int				(*pfn_strnicmp)(const char *zLeft, const char *zRight, int N);
	typedef int				(*pfn_stricmp)(const char *zLeft, const char *zRight);

	typedef void*			(*pfn_realloc)(void*, int);
	typedef void*			(*pfn_malloc)(int);
	typedef char*			(*pfn_mprintf)(const char *zFormat, ...);
	typedef int				(*pfn_test_control)(int op, ...);
	typedef void*			(*pfn_trace)(sqlite3 *db, void (*xTrace)(void*,const char*), void *pArg);

	typedef int				(*pfn_prepare_v2)(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);

	typedef const unsigned char* (*pfn_value_text)(sqlite3_value *pVal);
	typedef int					 (*pfn_value_bytes)(sqlite3_value *pVal);
	typedef const void *		 (*pfn_value_blob)(sqlite3_value *pVal);

	typedef sqlite3_vfs*	(*pfn_vfs_find)(const char *zVfsName);
	typedef int				(*pfn_vfs_register)(sqlite3_vfs*, int makeDflt);
	typedef int				(*pfn_vfs_unregister)(sqlite3_vfs*);

typedef struct SQLite3Api
{
	HMODULE m_hSQLite3;
	pfn_open16					m_pfnopen16;
	pfn_close					m_pfnclose;

	pfn_exec					m_pfnexec;
	pfn_complete				m_pfncomplete;
	pfn_complete16				m_pfncomplete16;
	pfn_finalize				m_pfnfinalize;

	pfn_free					m_pfnfree;
	pfn_free_table				m_pfnfree_table;
	pfn_get_table				m_pfnget_table;
	pfn_key						m_pfnkey;
	pfn_rekey					m_pfnrekey;
	pfn_reset					m_pfnreset;
	pfn_changes					m_pfnchanges;
	pfn_step					m_pfnstep;

	pfn_column_count			m_pfncolumn_count;
	pfn_bind_int				m_pfnbind_int;
	pfn_bind_text				m_pfnbind_text;
	pfn_bind_text16				m_pfnbind_text16;
	pfn_bind_parameter_count	m_pfnbind_parameter_count;
	pfn_bind_parameter_index	m_pfnbind_parameter_index;
	pfn_bind_parameter_name		m_pfnbind_parameter_name;

	pfn_column_name				m_pfncolumn_name;
	pfn_column_name16			m_pfncolumn_name16;
	pfn_column_text				m_pfncolumn_text;
	pfn_column_text16			m_pfncolumn_text16;
	pfn_column_type				m_pfncolumn_type;

	pfn_prepare					m_pfnprepare;
	pfn_prepare16				m_pfnprepare16;
	pfn_errcode					m_pfnerrcode;
	pfn_errmsg					m_pfnerrmsg;
	pfn_errmsg16				m_pfnerrmsg16;

	///////////////////////////////////////////////////
	pfn_backup_init			m_pfnbackup_init;
	pfn_backup_finish		m_pfnbackup_finish;
	pfn_backup_step			m_pfnbackup_step;

	pfn_busy_timeout		m_pfnbusy_timeout;
	pfn_bind_null			m_pfnbind_null;
	pfn_bind_blob			m_pfnbind_blob;
	pfn_bind_double			m_pfnbind_double;
	pfn_bind_int64			m_pfnbind_int64;

	pfn_config				m_pfnconfig;
	pfn_column_double		m_pfncolumn_double;
	pfn_column_bytes		m_pfncolumn_bytes;
	pfn_column_blob			m_pfncolumn_blob;
	pfn_column_int			m_pfncolumn_int;
	pfn_column_int64		m_pfncolumn_int64;
	pfn_create_function		m_pfncreate_function;

	pfn_extended_errcode	m_pfnextended_errcode;
	pfn_db_status			m_pfndb_status;
	pfn_initialize			m_pfninitialize;
	pfn_file_control		m_pfnfile_control;
	pfn_get_autocommit		m_pfnget_autocommit;

	pfn_libversion			m_pfnlibversion;

	pfn_open				m_pfnopen;
	pfn_result_text			m_pfnresult_text;
	pfn_result_blob			m_pfnresult_blob;
	pfn_result_int64		m_pfnresult_int64;

	pfn_strglob				m_pfnstrglob;
	pfn_sleep				m_pfnsleep;
	pfn_snprintf			m_pfnsnprintf;
	pfn_sourceid			m_pfnsourceid;
	pfn_sql					m_pfnsql;
	pfn_stmt_status			m_pfnstmt_status;
	pfn_status				m_pfnstatus;
	pfn_strnicmp			m_pfnstrnicmp;
	pfn_stricmp				m_pfnstricmp;

	pfn_realloc				m_pfnrealloc;
	pfn_malloc				m_pfnmalloc;
	pfn_mprintf				m_pfnmprintf;
	pfn_vmprintf			m_pfnvmprintf;
	pfn_test_control		m_pfntest_control;
	pfn_trace				m_pfntrace;

	pfn_prepare_v2			m_pfnprepare_v2;

	pfn_value_text			m_pfnvalue_text;
	pfn_value_bytes			m_pfnvalue_bytes;
	pfn_value_blob			m_pfnvalue_blob;

	pfn_vfs_find			m_pfnvfs_find;
	pfn_vfs_register		m_pfnvfs_register;
	pfn_vfs_unregister		m_pfnvfs_unregister;


}S_SQLite3Api, *PS_SQLite3Api;

BOOL Load(const TCHAR* pSqlite3Dll);
BOOL Unload();

extern S_SQLite3Api gs;
