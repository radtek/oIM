#pragma once
#include "sqlite3\I_SQLite.h"
#include "public\C_CommandLineBase.h"

typedef UINT64 QFID;
typedef UINT64 QSID;
typedef UINT64 QMID;

#define args C_CommandLine::GetObject()

class C_CommandLine: public C_CommandLineBase
{
private:
	C_CommandLine(void);
	inline DWORD	FindImages(const SStringT& szFindPath, const SStringT& szFilter, VectImage& vectImage);

	inline BOOL		GetMsgInfo(I_SQLite3* pIDb, const QFID& fid, QSID& sid, DWORD& dwTimestamp);
	inline BOOL		ParseImgMsg(const char* const pszMsgUI, DWORD& dwNowIndex);
	inline DWORD	LoadImages();
	inline BOOL		Parse(LPWSTR* pszArgs, int nArgs);

	static int __cdecl CompareFunc(void *pvlocale, const void *item1, const void *item2);

public:
	friend class CMainDlg;
	~C_CommandLine(void);
	static C_CommandLine& GetObject();

private:
	int			m_i32Days;		// ͼƬ��Ϣ�������
	BOOL		m_bFindedFid;	// �Ѿ��ҵ���ָ��FID��ͼƬ
	int			m_i32Speed;		// �л��������ٶ�
	UINT64		m_u64Fid;		// ��ǰͼƬ��ʹ��"-db"ʱ����QFID�������Ǵ�0��ʼ������
	SStringT	m_szDbFile;		// ��Ϣ���ݿ�(msg.db) ��·��
	VectImage	m_vectImage;	// ͼƬ�ļ��б�
	SStringT	m_szImgFilter;	// ͼƬ�������������Զ���������"-ipath"λ�õ�ͼƬ
	SStringT	m_szImgPath;	// ͼƬĬ��λ��
};

