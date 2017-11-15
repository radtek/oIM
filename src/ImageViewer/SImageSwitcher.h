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
		SArray<IBitmap *> m_lstImages;

		BOOL m_bWantMove;
		int  m_iDownX;
		BOOL m_bTimerMove;
		BOOL m_bMovable;
		BOOL m_bMoved;
		int  m_iMoveWidth;
		int  m_iSelected;
		int	 m_iTimesMove;
		float  m_fRatio;
		CPoint m_ptMoveOld;	// 移动时的上一次的位置。
		CPoint m_ptCenterOld;
		CPoint m_ptCenter;
		CRect  m_rtImgSrc;
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