#pragma once

/**
by 多点免费WIFI
*/
#include <core/Swnd.h>

namespace SOUI
{
	class SImageSwitcher : public SWindow
	{
		SOUI_CLASS_NAME(SImageSwitcher,L"imageSwitcher")
	public:
		SImageSwitcher();
		virtual ~SImageSwitcher();

	public:
		BOOL	Switch(int iSelect, BOOL bMoive = TRUE);
		BOOL	InsertImage(IBitmap * pImage, int iTo = -1);
		BOOL	InsertImage(const SStringT& szImage, int iTo = -1);

		void	RemoveAll();
		int		GetCurSel();
		size_t	GetCount();

	private:
		SArray<IBitmap*> m_lstImages;

		BOOL	m_bImgMovable;		// 图片是否可以移动（无法显全图时）
		BOOL	m_bTimerMove;		// 翻页动画的Timer启用标识
		int		m_iMoveWidth;		// 翻页动画移动的大小
		int		m_iTimesMove;		// 翻页动画，移动的次数
		int		m_iSelected;		// 当前显示的图片索引
		float	m_fRatio;			// 放大比率，1.0f 是原始图片大小
		CPoint	m_ptMoveStart;		// 鼠标按下时的坐标位置
		CPoint	m_ptCenterOld;		// 鼠标按下时原来的中心位置
		CPoint	m_ptCenter;			// 中心位置
		CRect	m_rtImgSrc;			// 图片显示的区域
//		CRect m_rtDrawDst;
		 
	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags,CPoint pt);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnTimer(char nIDEvent);
		HRESULT OnAttrImages(const SStringT& strValue,BOOL bLoading);
		
		BOOL DrawImage(IRenderTarget *pRT, CRect& rcWnd, int i32Index);
		RECT GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, BOOL bRatio = TRUE);
		RECT GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg);

	protected:
		SOUI_MSG_MAP_BEGIN()	
			MSG_WM_PAINT_EX(OnPaint)    //窗口绘制消息
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"images",OnAttrImages)
		SOUI_ATTRS_END()
	};
}