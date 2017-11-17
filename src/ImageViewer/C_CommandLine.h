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
	const SStringT& GetLang() const { return m_szLang; }

private:
	int			m_nScale;		// 放大率：【100，125，150，200】
	UINT64		m_u64Fid;		// 当前图片，使用"-db"时，是QFID；否则是从0开始的索引
	SStringT	m_szLang;		// 语言：【R.string.lang_cn, R.string.lang_en】
	SStringT	m_szDbFile;		// 消息数据库(msg.db) 的路径
	VectImage	m_vectImage;	// 图片文件列表
	SStringT	m_szImgFilter;	// 图片过滤器，用于自动搜索参数"-ipath"位置的图片
	SStringT	m_szImgPath;	// 图片默认位置

};

