#pragma once
#include <core/Swnd.h>
#include "../../ext/soui/controls.extend/gif/SSkinAPNG.h"

namespace SOUI
{
#define SAFE_RELEASE_(P) do { if ( P ) P->Release(); P = NULL; } while(0)

#define EVT_RATIO_CHANGED	(EVT_EXTERNAL_BEGIN+120)
#define EVT_IMGPOS_CHANGED	(EVT_EXTERNAL_BEGIN+121)

#define RATIO_MIN		(4)		// 4%
#define RATIO_100		(100)	// 100%
#define RATIO_MAX		(2000)	// 20000%
#define RATIOF_(F)		((F) / 100.f)

#define ZOOM_DELTA		(0.035f)
typedef enum tagMoveType
{
	eMOVE_NONE = 0,	// 没有移动
	eMOVE_START,	// 开始移动
	eMOVE_MOVING,	// 移动过程中
	eMOVE_STOP,		// 移动完成
}E_MoveType, *PE_MoveType;

	class EventRatioChanged : public TplEventArgs<EventRatioChanged>
	{	// 放大率改变事件
		SOUI_CLASS_NAME(EventRatioChanged, L"on_ratio_changed")
	public:
		EventRatioChanged(SObject *pSender, float fRatio)
			: TplEventArgs<EventRatioChanged>(pSender)
			, m_fRatio(fRatio){}
		enum {EventID=EVT_RATIO_CHANGED};
		float m_fRatio;	// 当前的放大率
	};

	class EventImagePosChanged : public TplEventArgs<EventImagePosChanged>
	{	// 图片显示位置改变事件
		SOUI_CLASS_NAME(EventRatioChanged, L"on_image_pos_changed")
	public:
		EventImagePosChanged(SObject *pSender, BOOL bShow, BOOL bGif, const CRect& rtImgPos, float fRatio, BOOL bError, IBitmap* pImg = NULL)
			: TplEventArgs<EventImagePosChanged>(pSender)
			, m_pImg(pImg)
			, m_bShow(bShow)
			, m_bGif(bGif)
			, m_bError(bError)
			, m_fRatio(fRatio)
			, m_rtImgPos(rtImgPos){}
		enum {EventID=EVT_IMGPOS_CHANGED};
		CRect	 m_rtImgPos;	// 显示的图片源位置(相对于原始图片)
		IBitmap* m_pImg;		// 当前显示的图片接口对象
		BOOL	 m_bShow;		// 是否显示地图
		BOOL	 m_bGif;		// 是否是GIF
		BOOL	 m_bError;		// 是否加载图片失败
		float	 m_fRatio;		// 当前的放大率
	};

	class SImageViewer : public SWindow
	{
		SOUI_CLASS_NAME(SImageViewer,L"imageViewer")
	public:
		SImageViewer();
		virtual ~SImageViewer();
		BOOL	Switch(int iSelect, int i32Speed = 0, LPSIZE pSize = NULL);	// 切换显示图片
		BOOL	InitImages(VectImage& vectImg, int i32Sel = -1);	// 使用窗口初始化显示图片列表
		BOOL	InsertImage(const SStringT& szImage, int iTo = -1);	// 插入图片

		void	RemoveAll();	// 清空图片
		int		GetCurSel();	// 获取当前显示的图片索引
		size_t	GetCount();		// 获取图片总数
		float   GetRatio()const { return m_fRatio; }			// 获取图片放大率
		CRect	GetImgSrcPos()const { return m_rtImgSrc; }		// 获取图片源显示位置
		BOOL	IsImgMovable() const { return m_bImgMovable; }	// 是否图片可以移动
		RECT	GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, float* pfRatio = NULL);	// 获取默认显示位置
		CPoint	Move(E_MoveType eType, LPPOINT ptCenter = NULL);// 移动图片
		BOOL	RealSize();										// 显示实际大小
		float	Zoom(float fDelta, BOOL bFixed = FALSE);		// 缩放
		BOOL	OpenFolder(LPCTSTR pszPathImage = NULL);		// 打开文件夹
		BOOL	Saveas(BOOL bOpenFolder = TRUE);				// 另存为
		BOOL	Rotate(BOOL bRight = TRUE);						// 旋转

	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags,CPoint pt);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnTimer(char nIDEvent);
		
		inline BOOL		DrawImage(IRenderTarget *pRT, IBitmap *pBmp, CRect& rcWnd, int i32Index);	// 翻页时显示动画
		inline RECT		GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg);				// 根据当前的放大率，源位置，计算位置
		
		// 另存为相关API
		inline int		GetEncoderClsid(const WCHAR* format, CLSID* pClsid);	// 获取图片Encoder的CLSID
		inline SStringT GetTempImgFile(TCHAR* pszExt);							// 获取临时图片路径
		inline BOOL		RemoveTempImage();										// 删除临时图片
		inline BOOL		Reset(BOOL bNoAngle, BOOL bCloneSel);					// 复位图片
		inline SStringT GetImageFormat(TCHAR* pszExt);							// 得到图片格式

	protected:
		SOUI_MSG_MAP_BEGIN()	
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

	private:
		ULONG_PTR m_gdiplusToken;

		VectImage m_vectImage;		// 图片列表（所有图片的全路径，不能全部加载，可能会造成内存不足）
		IBitmap*  m_pImgSel;		// 当前显示的图片
		IBitmap*  m_pImgNext;		// 上/下一张图片，显示过滤动画时使用
		SSkinAni* m_pImgGif;		// GIF

		E_MoveType	m_eMove;		// 移动图片操作的类型
		BOOL	m_bError;			// 加载图片失败
		BOOL	m_bSwitched;		// 是否有切换图片
		BOOL	m_bImgMovable;		// 图片是否可以移动（无法显全图时）
		BOOL	m_bTimerMove;		// 翻页动画的Timer启用标识
		int		m_iMoveWidth;		// 翻页动画移动的大小
		int		m_iTimesMove;		// 翻页动画，移动的次数
		int		m_iSelected;		// 当前显示的图片索引
		float	m_fRatio;			// 放大比率，1.0f 是原始图片大小
		int		m_nAngle;			// 旋转角度[0,90,180,270]
		SStringT m_szTmpImg;		// 旋转时使用的临时文件
		CPoint	m_ptMoveStart;		// 鼠标按下时的坐标位置
		CPoint	m_ptCenterOld;		// 鼠标按下时原来的中心位置
		CPoint	m_ptCenter;			// 中心位置
		CRect	m_rtImgSrc;			// 图片显示的区域src
		CRect	m_rtImgDst;			// 图片显示的区域dst
	};
}