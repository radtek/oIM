#pragma once

#define args C_CommandLine::GetObject()

class C_CommandLine
{
private:
	C_CommandLine(void);
	DWORD FindImages(const SStringT& szFindPath, const SStringT& szFilter, VectImage& vectImage);
	SStringT GetParamValue(LPWSTR pszArg, LPWSTR pszNext, int& i32Index);
	BOOL IsOption(LPWSTR pszArg, LPWSTR pszName);
	BOOL Parse(LPWSTR* pszArgs, int nArgs);

public:
	friend class CMainDlg;
	~C_CommandLine(void);
	static C_CommandLine& GetObject();
	BOOL Usage();
	BOOL ParseCommandLine();

private:
	UINT64		m_u64Fid;		// ��ǰͼƬ��ʹ��"-db"ʱ����QFID�������Ǵ�0��ʼ������
	SStringT	m_szDbFile;		// ��Ϣ���ݿ�(msg.db) ��·��
	VectImage	m_vectImage;	// ͼƬ�ļ��б�
	SStringT	m_szImgFilter;	// ͼƬ�������������Զ���������"-ipath"λ�õ�ͼƬ
	SStringT	m_szImgPath;	// ͼƬĬ��λ��

};

