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
	BOOL		m_bFindedFid;			// 已经找到了指定FID的图片
	int			m_nScale;		// 放大率：【100，125，150，200】
	UINT64		m_u64Fid;		// 当前图片，使用"-db"时，是QFID；否则是从0开始的索引
	SStringT	m_szLang;		// 语言：【R.string.lang_cn, R.string.lang_en】
	SStringT	m_szDbFile;		// 消息数据库(msg.db) 的路径
	SStringT	m_szDbKey;		// 数据库密钥
	VectImage	m_vectImage;	// 图片文件列表
	SStringT	m_szImgFilter;	// 图片过滤器，用于自动搜索参数"-ipath"位置的图片
	SStringT	m_szImgPath;	// 图片默认位置
};

