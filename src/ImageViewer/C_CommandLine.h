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
	int			m_i32Days;		// 图片消息最大天数
	BOOL		m_bFindedFid;	// 已经找到了指定FID的图片
	int			m_i32Speed;		// 切换动画的速度
	UINT64		m_u64Fid;		// 当前图片，使用"-db"时，是QFID；否则是从0开始的索引
	SStringT	m_szDbFile;		// 消息数据库(msg.db) 的路径
	VectImage	m_vectImage;	// 图片文件列表
	SStringT	m_szImgFilter;	// 图片过滤器，用于自动搜索参数"-ipath"位置的图片
	SStringT	m_szImgPath;	// 图片默认位置
};

