#include "StdAfx.h"
#include "SImageViewer.h"
#include <helper/SplitString.h>

#define TIMER_MOVE		1

namespace SOUI
{
	SImageViewer::SImageViewer()
		: m_pImgSel(NULL)
		, m_pImgNext(NULL)
		, m_iMoveWidth(0)
		, m_bTimerMove(FALSE)
		, m_iSelected(0)
		, m_iTimesMove(0)
		, m_fRatio(1.f)
		, m_ptCenter(0)
		, m_bImgMovable(FALSE)
	{

	}

	SImageViewer::~SImageViewer()
	{
		RemoveAll();
	}

	RECT SImageViewer::GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg)
	{
		m_bImgMovable = FALSE;	// 默认不可以移动
		if ( m_ptCenter.x == 0 || m_ptCenter.y == 0 )
		{	// 首次正常显示当前图片， 显示默认的全图
			SetRect(&rtImg, 0, 0, szImg.cx, szImg.cy);
			m_ptCenter.SetPoint(szImg.cx/2, szImg.cy/2);
			return GetDefaultDest(rtWnd, szImg, &m_fRatio);
		}

		BOOL bOutWnd = FALSE;	// 是否有超出rtWnd
		SIZE szReal  = {(LONG)(szImg.cx * m_fRatio), (LONG)(szImg.cy * m_fRatio)};			// 得到实际要显示的大小
		POINT ptReal = {(LONG)(m_ptCenter.x * m_fRatio), (LONG)(m_ptCenter.y * m_fRatio)};	// 得到中心位置的实际要显示的位置
		CRect rtDst  = rtWnd;

		if ( rtWnd.Width() >= szReal.cx && rtWnd.Height() >= szReal.cy )
		{	// 能显示缩放之后的全图
			float fRatio = 0;	// dummy
			rtDst = GetDefaultDest(rtWnd, szReal);		// 用实际要显示的大小，来计算目标位置
			rtImg.SetRect(0, 0, szImg.cx, szImg.cy);	
			m_ptCenter.SetPoint(szImg.cx / 2, szImg.cy / 2);	// 复位中心点为图片中心
		}
		else
		{	// 不能显示全图
			m_bImgMovable = TRUE;	// 可以移动
			rtImg.SetRect(0, 0, szReal.cx, szReal.cy);
			if ( szReal.cx >= rtDst.Width() )
			{	// 图片宽度 >= 窗口宽度
				int iOffsetX= (ptReal.x  - szReal.cx / 2);		// 与显示图片的水平中心位置的偏移量
				int iDeltaX1= (szReal.cx - rtDst.Width()) / 2;
				
				// 以图片中心位置来计算位置
				rtImg.left += iDeltaX1;
				rtImg.right-= iDeltaX1;	

				// 计算X偏移
				if ( iOffsetX >= 0 )
				{	// 中心点向右偏移
					if ( rtImg.right + iOffsetX <= szReal.cx )
					{	// 向右偏移后，仍然在图片大小之内
						rtImg.left += iOffsetX;
						rtImg.right+= iOffsetX;
					}
					else
					{	// 图片靠右边
						rtImg.left += (szReal.cx - rtImg.right);
						rtImg.right = szReal.cx;
						bOutWnd = TRUE;
					}
				}
				else
				{	// 中心点向左偏移
					if ( rtImg.left + iOffsetX >= 0 )
					{	// 向左偏移后，仍然在图片大小之内
						rtImg.left += iOffsetX;
						rtImg.right+= iOffsetX;
					}
					else
					{	// 图片靠左边
						rtImg.right-= rtImg.left;
						rtImg.left	= 0;
						bOutWnd = TRUE;
					}
				}
			}
			else
			{	// 图片宽度 < 窗口宽度
				int iDeltaX2= (rtDst.Width() - szReal.cx ) / 2;
				rtDst.left += iDeltaX2;
				rtDst.right-= iDeltaX2;
			}

			if ( szReal.cy >= rtDst.Height() )
			{	// 图片高度 >= 窗口高度
				int iOffsetY = (ptReal.y  - szReal.cy / 2);		// 与显示图片的垂直中心位置的偏移量
				int iDeltaY1 = (szReal.cy - rtDst.Height()) / 2;

				// 以图片中心位置来计算位置
				rtImg.top   += iDeltaY1;	
				rtImg.bottom-= iDeltaY1;

				// 计算Y偏移
				if ( iOffsetY >= 0 )
				{	// 中心点向下偏移
					if ( rtImg.bottom + iOffsetY <= szReal.cy )
					{	// 向右偏移后，仍然在图片大小之内
						rtImg.top	+= iOffsetY;
						rtImg.bottom+= iOffsetY;
					}
					else
					{	// 图片靠下边
						rtImg.top	+= (szReal.cy - rtImg.bottom);
						rtImg.bottom = szReal.cy;
						bOutWnd = TRUE;
					}
				}
				else
				{	// 中心点向上偏移
					if ( rtImg.top + iOffsetY >= 0 )
					{	// 向上偏移后，仍然在图片大小之内
						rtImg.top	+= iOffsetY;
						rtImg.bottom+= iOffsetY;
					}
					else
					{	// 图片靠上边
						rtImg.bottom-= rtImg.top;
						rtImg.top	 = 0;
						bOutWnd = TRUE;
					}
				}
			}
			else
			{	// 图片高度 < 窗口高度
				int iDeltaY2 = (rtDst.Height() - szReal.cy) / 2;
				rtDst.top   += iDeltaY2;
				rtDst.bottom-= iDeltaY2;
			}

			// 再换算加原图的坐标
			rtImg.left  = (LONG)(rtImg.left / m_fRatio);
			rtImg.top   = (LONG)(rtImg.top / m_fRatio);
			rtImg.right = (LONG)(rtImg.right / m_fRatio);
			rtImg.bottom= (LONG)(rtImg.bottom / m_fRatio);

			if ( bOutWnd )
			{	// 修正中心点
				if ( rtImg.Width() % 2) 
					rtImg.right--;	// 变成偶数

				if ( rtImg.Height() % 2)
					rtImg.bottom--;	// 变成偶数

				m_ptCenter.x = rtImg.left + rtImg.Width() / 2;
				m_ptCenter.y = rtImg.top  + rtImg.Height() / 2;
			}
		}

		return rtDst;
	}

	RECT SImageViewer::GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, float* pfRatio)
	{	// 计算默认的目标位置
		RECT rtDst = { 0 };

		if ( rtWnd.Width() >= szImg.cx && rtWnd.Height() >= szImg.cy )
		{	// 窗口能显示全图
			if ( pfRatio )
				*pfRatio = 1.f;

			rtDst.left  = rtWnd.left + (LONG)((rtWnd.Width() - szImg.cx) / 2.0);
			rtDst.top	= rtWnd.top  + (LONG)((rtWnd.Height() - szImg.cy) / 2.0);
			rtDst.right = rtDst.left + szImg.cx;
			rtDst.bottom= rtDst.top  + szImg.cy;
		}
		else
		{
			float fRatioX = (float)rtWnd.Width() / szImg.cx;
			float fRatioY = (float)rtWnd.Height() / szImg.cy;
			if ( fRatioX < fRatioY )
			{	// 窗口宽度不能显示全图, 就以宽度进行缩放
				if ( pfRatio )
					*pfRatio = fRatioX;

				LONG lHeight= (LONG)(fRatioX * szImg.cy);
				rtDst.left	= rtWnd.left;
				rtDst.right = rtWnd.right;
				rtDst.top   = rtWnd.top + (LONG)((rtWnd.Height() - lHeight) / 2.0);
				rtDst.bottom= rtDst.top + lHeight;
			}
			else
			{	// 窗口高度不能显示全图, 就以高度进行缩放
				if ( pfRatio )
					*pfRatio = fRatioY;

				LONG lWidth = (LONG)(fRatioY * szImg.cx);
				rtDst.top   = rtWnd.top;
				rtDst.bottom= rtWnd.bottom;
				rtDst.left  = rtWnd.left + (LONG)((rtWnd.Width() - lWidth) / 2.0);
				rtDst.right = (LONG)(rtDst.left + lWidth);
			}
		}

		return rtDst;
	}

	BOOL SImageViewer::DrawImage(IRenderTarget *pRT, IBitmap *pBmp, CRect& rtWnd, int i32Index)
	{
		if ( !pBmp )
			return FALSE;

		SIZE szImg	= pBmp->Size();
		RECT rtDst	= GetDefaultDest(rtWnd, szImg, &m_fRatio);
		int i32Delta= (i32Index * rtWnd.Width() - (m_iSelected * rtWnd.Width()) + m_iMoveWidth);
		rtDst.left += i32Delta;
		rtDst.right+= i32Delta;
		pRT->DrawBitmapEx(&rtDst, pBmp, CRect(CPoint(), szImg), EM_STRETCH);

		return TRUE;
	}

	void SImageViewer::OnPaint(IRenderTarget *pRT)
	{
		if ( m_pImgSel == NULL )
			return;

		CRect rtWnd = GetClientRect();
		if ( m_iMoveWidth == 0 )
		{	// 显示当前图片
			CRect szOld = m_rtImgSrc;
			RECT  rtDst = GetDest(rtWnd, m_pImgSel->Size(), m_rtImgSrc);
			pRT->DrawBitmapEx(&rtDst, m_pImgSel, &m_rtImgSrc, EM_STRETCH);
			SAFE_RELEASE_(m_pImgNext);	// 上/下一张图片，过渡动画时才需要

			if ( !m_rtImgSrc.EqualRect(szOld) )
			{	// 图片的显示区域变了，才通知
				EventImagePosChanged evt(this, m_bImgMovable, m_rtImgSrc, m_fRatio, m_pImgSel);
				FireEvent(evt);
			}
		}
		else
		{	
			if ( m_iMoveWidth > 0 )
			{	// 上一帧
				DrawImage(pRT, m_pImgNext, rtWnd, m_iSelected - 1);
				DrawImage(pRT, m_pImgSel,  rtWnd, m_iSelected);
			}
			else
			{	// 下一帧
				DrawImage(pRT, m_pImgSel,  rtWnd, m_iSelected);
				DrawImage(pRT, m_pImgNext, rtWnd, m_iSelected + 1);
			}
		}
	}

	BOOL  SImageViewer::Switch(int iSelect, BOOL bMoive)
	{
		if (iSelect >= (int)m_vectImage.size() || iSelect < 0)
			return FALSE;

		CRect rcWnd = GetClientRect();
		if(m_bTimerMove)
			return TRUE;	// 正在显示过渡效果时，不再切换

		m_rtImgSrc.SetRectEmpty();	// 清空
		EventImagePosChanged evt(this, FALSE, m_rtImgSrc, NULL);
		FireEvent(evt);				// 隐藏地图

		m_ptCenter.SetPoint(0, 0);	// Reset
		if ( bMoive == FALSE )
		{	// 不显示动画
			m_iSelected = iSelect;
			m_pImgSel = LOADIMAGE(_T("file"), m_vectImage[m_iSelected]);
			return TRUE;
		}

		m_iMoveWidth = (iSelect-m_iSelected)*rcWnd.Width();
		m_iSelected = iSelect;

		// 更新加载的图片
		SAFE_RELEASE_(m_pImgSel);
		SAFE_RELEASE_(m_pImgNext);
		m_pImgSel = LOADIMAGE(_T("file"), m_vectImage[m_iSelected]);
		int i32Next = m_iMoveWidth > 0 ? m_iSelected - 1 : m_iSelected + 1;
		if ( i32Next >= 0 && i32Next < (int)m_vectImage.size() )
			m_pImgNext = LOADIMAGE(_T("file"), m_vectImage[i32Next]);

		m_iTimesMove = (m_iMoveWidth > 0 ? m_iMoveWidth : -m_iMoveWidth) / 30;
		if(m_iTimesMove < 20)
			m_iTimesMove = 20;

		SetTimer(TIMER_MOVE, 30);
		m_bTimerMove = TRUE;

		return TRUE;
	}

	void SImageViewer::OnLButtonDown(UINT nFlags, CPoint point)
	{
		__super::OnLButtonDown(nFlags,point);
		if ( m_bImgMovable )
		{
			m_ptMoveStart= point;
			m_ptCenterOld= m_ptCenter;
			SetCapture();
		}
	}

	void SImageViewer::OnMouseMove(UINT nFlags,CPoint pt)
	{
		__super::OnMouseMove(nFlags,pt);
		BOOL bLPress = (nFlags & MK_LBUTTON);
		if ( m_bImgMovable && bLPress )
		{
			m_ptCenter.x = m_ptCenterOld.x - (LONG)((pt.x - m_ptMoveStart.x) / m_fRatio);
			m_ptCenter.y = m_ptCenterOld.y - (LONG)((pt.y - m_ptMoveStart.y) / m_fRatio);
			Invalidate();
		}

	}

	void SImageViewer::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		__super::OnLButtonUp(nFlags,pt);
		if ( m_bImgMovable )
		{
			ReleaseCapture();
			if (!(m_ptCenter.x == m_ptCenterOld.x && m_ptCenter.y == m_ptCenterOld.y))
			{
				m_ptCenter.x = m_ptCenterOld.x - (LONG)((pt.x - m_ptMoveStart.x) / m_fRatio);
				m_ptCenter.y = m_ptCenterOld.y - (LONG)((pt.y - m_ptMoveStart.y) / m_fRatio);
				Invalidate();
			}
		}
	}

	BOOL SImageViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		BOOL bRet	= __super::OnMouseWheel(nFlags, zDelta, pt);
		BOOL bFixed = GetAsyncKeyState(VK_CONTROL);	// Ctrl + Wheel
		float fDelta= (zDelta / WHEEL_DELTA) / 100.0f;

		if ( bFixed )
		{	
			m_fRatio += fDelta * 10.0f;	// 固定的Delta
		}
		else
		{
			if ( m_fRatio < 0.1f )
				m_fRatio += fDelta;
			else if ( m_fRatio < 1.0f )
				m_fRatio += fDelta * 10.0f;
			else if ( m_fRatio < 5.0f )
				m_fRatio += fDelta * 20.0f;
			else if ( m_fRatio < 10.0f )
				m_fRatio += fDelta * 30.0f;
			else
				m_fRatio += fDelta * 50.0f;
		}

		if ( m_fRatio < 0.04f )
			m_fRatio = 0.04f;		// Min as 4%
		else if ( m_fRatio > 0.1f )
		{
			if ( bFixed == FALSE )
			{
				INT i32Ratio = (INT)(m_fRatio * 10.0f);
				m_fRatio = i32Ratio / 10.0f;
			}

			if ( m_fRatio > 16.0f )
				m_fRatio = 16.0f;	// Max as 1600%
		}

		Invalidate();

		// 激发放大率更新事件
		EventRatioChanged evt(this, m_fRatio);
		FireEvent(evt);
		
		return bRet;
	}

	void SImageViewer::OnTimer(char nIDEvent)
	{
		if ( nIDEvent != TIMER_MOVE )
			return SetMsgHandled(FALSE);

		if(m_iMoveWidth > 0)
		{
			if(m_iMoveWidth - m_iTimesMove <= 0)
			{
				m_iMoveWidth = 0;
				Invalidate();
				KillTimer(TIMER_MOVE);
				m_bTimerMove = FALSE;
			}
			else
			{
				m_iMoveWidth-= m_iTimesMove;
				Invalidate();
			}
		}
		else
		{
			if(m_iMoveWidth + m_iTimesMove >= 0)
			{
				m_iMoveWidth = 0;
				Invalidate();
				KillTimer(TIMER_MOVE);
				m_bTimerMove = FALSE;
			}
			else
			{
				m_iMoveWidth+= m_iTimesMove;
				Invalidate();
			}
		}
	}

	BOOL SImageViewer::InsertImage(const SStringT& szImage, int iTo)
	{
		if( iTo < 0 || iTo >= (int)m_vectImage.size() )
			m_vectImage.push_back(szImage);
		else
			m_vectImage.insert(m_vectImage.begin() + iTo, szImage);
		
		return TRUE;
	}

	BOOL SImageViewer::InitImages(VectImage& vectImg, int i32Sel)
	{
		m_vectImage.swap(vectImg);
		if ( i32Sel >= 0 && i32Sel < (int)m_vectImage.size() )
			return Switch(i32Sel, FALSE);

		return TRUE;
	}

	void SImageViewer::RemoveAll()
	{
		m_vectImage.clear();
		SAFE_RELEASE_(m_pImgSel);
		SAFE_RELEASE_(m_pImgNext);
	}

	int SImageViewer::GetCurSel()
	{
		return m_iSelected;
	}

	size_t SImageViewer::GetCount()
	{
		return m_vectImage.size();
	}


}