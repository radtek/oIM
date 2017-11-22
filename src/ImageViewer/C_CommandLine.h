#pragma once
#include "sqllite\I_SQLite.h"

#define args C_CommandLine::GetObject()

typedef UINT64 QFID;
typedef UINT64 QSID;
typedef UINT64 QMID;


class C_CommandLine
{
private:
	C_CommandLine(void);
	DWORD	 FindImages(const SStringT& szFindPath, const SStringT& szFilter, VectImage& vectImage);

	BOOL	 GetMsgInfo(I_SQLite3* pIDb, const QFID& fid, QSID& sid, DWORD& dwTimestamp);
	BOOL	 ParseImgMsg(const char* const pszMsgUI, DWORD& dwNowIndex);
	DWORD	 LoadImages();

	SStringT GetParamValue(LPWSTR pszArg, LPWSTR pszNext, int& i32Index);
	BOOL	 IsOption(LPWSTR pszArg, LPWSTR pszName);
	BOOL	 Parse(LPWSTR* pszArgs, int nArgs);
	static int __cdecl CompareFunc(void *pvlocale, const void *item1, const void *item2);

public:
	friend class CMainDlg;
	~C_CommandLine(void);
	static C_CommandLine& GetObject();
	BOOL IsTaskbar(){ return m_bShowInTaskbar; }
	BOOL Usage(HWND hParent);
	BOOL ParseCommandLine();
	const SStringT& GetLang() const { return m_szLang; }

private:
	BOOL		m_bShowInTaskbar;
	BOOL		m_bFindedFid;			// �Ѿ��ҵ���ָ��FID��ͼƬ
	int			m_nScale;		// �Ŵ��ʣ���100��125��150��200��
	UINT64		m_u64Fid;		// ��ǰͼƬ��ʹ��"-db"ʱ����QFID�������Ǵ�0��ʼ������
	SStringT	m_szLang;		// ���ԣ���R.string.lang_cn, R.string.lang_en��
	SStringT	m_szDbFile;		// ��Ϣ���ݿ�(msg.db) ��·��
	SStringT	m_szDbKey;		// ���ݿ���Կ
	VectImage	m_vectImage;	// ͼƬ�ļ��б�
	SStringT	m_szImgFilter;	// ͼƬ�������������Զ���������"-ipath"λ�õ�ͼƬ
	SStringT	m_szImgPath;	// ͼƬĬ��λ��
};

