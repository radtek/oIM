#ifndef EIM_IMPORT_MSG_MGR_H
#define EIM_IMPORT_MSG_MGR_H

class C_eIMImportMsgMgr : public I_EIMImportMsgMgr
{
public:
	C_eIMImportMsgMgr();
	~C_eIMImportMsgMgr();

	virtual BOOL ImportMsg(LPCTSTR lpszFullPath, int i32DbType);

	DECALRE_PLUGIN_(C_eIMImportMsgMgr)
private:
	//BOOL Init();
	//BOOL Uninit();
	//I_SQLite3*	m_pIDb;
	DWORD		m_dwRef;

	BOOL ImportContactsDb(LPCTSTR lpszFullPath);
	BOOL ImportMsgDb(LPCTSTR lpszFullPath);

	BOOL ImportTable(I_SQLite3*	pIDb, const char* const pszSrcTable, const char* const pszDstTable);
};

#endif