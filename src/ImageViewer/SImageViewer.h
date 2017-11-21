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
	{
		SOUI_CLASS_NAME(EventRatioChanged, L"on_ratio_changed")
	public:
		EventRatioChanged(SObject *pSender, float fRatio)
			: TplEventArgs<EventRatioChanged>(pSender)
			, m_fRatio(fRatio){}
		enum {EventID=EVT_RATIO_CHANGED};
		float m_fRatio;
	};

	class EventImagePosChanged : public TplEventArgs<EventImagePosChanged>
	{
		SOUI_CLASS_NAME(EventRatioChanged, L"on_image_pos_changed")
	public:
		EventImagePosChanged(SObject *pSender, BOOL bShow, BOOL bGif, const CRect& rtImgPos, float fRatio, IBitmap* pImg = NULL)
			: TplEventArgs<EventImagePosChanged>(pSender)
			, m_pImg(pImg)
			, m_bShow(bShow)
			, m_bGif(bGif)
			, m_fRatio(fRatio)
			, m_rtImgPos(rtImgPos){}
		enum {EventID=EVT_IMGPOS_CHANGED};
		CRect	 m_rtImgPos;
		IBitmap* m_pImg;
		BOOL	 m_bShow;
		BOOL	 m_bGif;
		float	 m_fRatio;
	};

	class SImageViewer : public SWindow
	{
		SOUI_CLASS_NAME(SImageViewer,L"imageViewer")
	public:
		SImageViewer();
		virtual ~SImageViewer();
		BOOL	Switch(int iSelect, BOOL bMoive = TRUE, LPSIZE pSize = NULL);
		BOOL	InitImages(VectImage& vectImg, int i32Sel = -1);
		BOOL	InsertImage(const SStringT& szImage, int iTo = -1);

		void	RemoveAll();
		int		GetCurSel();
		size_t	GetCount();
		float   GetRatio()const { return m_fRatio; }
		CRect	GetImgSrcPos()const { return m_rtImgSrc; }
		BOOL	IsImgMovable() const { return m_bImgMovable; }
		RECT	GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, float* pfRatio = NULL);
		CPoint	Move(E_MoveType eType, LPPOINT ptCenter = NULL);
		BOOL	RealSize();
		float	Zoom(float fDelta, BOOL bFixed = FALSE);
		BOOL	OpenFolder(LPCTSTR pszPathImage = NULL);
		BOOL	Saveas(BOOL bOpenFolder = TRUE);
		BOOL	Rotate(BOOL bRight = TRUE);

	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags,CPoint pt);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnTimer(char nIDEvent);
		
		BOOL DrawImage(IRenderTarget *pRT, IBitmap *pBmp, CRect& rcWnd, int i32Index);
		RECT GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg);
		
		inline int		GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
		inline SStringT GetTempImgFile(TCHAR* pszExt);
		inline BOOL		RemoveTempImage();
		inline BOOL		Reset();

	protected:
		SOUI_MSG_MAP_BEGIN()	
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
		SOUI_ATTRS_END()

	private:
		VectImage m_vectImage;		// 图片列表（所有图片的全路径，不能全部加载，可能会造成内存不足）
		IBitmap*  m_pImgSel;		// 当前显示的图片
		IBitmap*  m_pImgNext;		// 上/下一张图片，显示过滤动画时使用
		SSkinAni* m_pImgGif;		// GIF

		E_MoveType	m_eMove;		// 移动图片操作的类型
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