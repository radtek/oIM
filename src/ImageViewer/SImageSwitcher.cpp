#include "StdAfx.h"
#include "SImageSwitcher.h"
#include <helper/SplitString.h>

#define TIMER_MOVE		1

namespace SOUI
{
	SImageSwitcher::SImageSwitcher()
		: m_bWantMove(FALSE)
		, m_iDownX(0)
		, m_iMoveWidth(0)
		, m_bTimerMove(0)
		, m_iSelected(0)
		, m_iTimesMove(0)
		, m_iRatio(100)
	{
	}

	SImageSwitcher::~SImageSwitcher()
	{
		RemoveAll();
	}

	RECT SImageSwitcher::GetDefaultDest(const CRect& rtWnd, const SIZE szImg)
	{
		RECT rtImg = { 0 };

		if ( rtWnd.Width() >= szImg.cx && rtWnd.Height() >= szImg.cy )
		{	// 窗口能显示全图
			rtImg.left += (LONG)((rtWnd.Width() - szImg.cx) / 2.0);
			rtImg.top  += (LONG)((rtWnd.Height() - szImg.cy) / 2.0);
			rtImg.right = rtImg.left + szImg.cx;
			rtImg.bottom= rtImg.top + szImg.cy;
		}
		else if ( rtWnd.Width() < szImg.cx )
		{	// 窗口宽度不能显示全图
			float fHeight = ((float)rtWnd.Width() / szImg.cx * szImg.cy);
			rtImg.left    = rtWnd.left;
			rtImg.right   = rtWnd.right;
			rtImg.top    += (LONG)((rtWnd.Height() - fHeight) / 2.0);
			rtImg.bottom  = rtImg.top + (LONG)fHeight;
		}
		else
		{	// 窗口高度不能显示全图
			float fWidth = ((float)rtWnd.Height() / szImg.cy * szImg.cx);
			rtImg.left    = (LONG)((rtWnd.Width() - fWidth) / 2.0);
			rtImg.right   = rtImg.left + fWidth;
			rtImg.right   = rtWnd.right;
			rtImg.top    += (LONG)((rtWnd.Height() - fWidth) / 2.0);
			rtImg.bottom  = rtImg.top + (LONG)fWidth;
		}

		return rtImg;
	}

	BOOL SImageSwitcher::DrawImage(IRenderTarget *pRT, CRect& rtWnd, int i32Index)
	{
		if ( i32Index >= 0 && i32Index < (int)m_lstImages.GetCount() )
		{
			IBitmap *pBmp = m_lstImages[i32Index];
			SIZE szImg    = pBmp->Size();
			RECT rtImg    = GetDefaultDest(rtWnd, szImg);

			rtImg.left	+= (i32Index * rtWnd.Width() - (m_iSelected * rtWnd.Width()) + m_iMoveWidth);
			pRT->DrawBitmapEx(&rtImg, pBmp, CRect(CPoint(), szImg), EM_NULL);
		}

		return FALSE;
	}

	void SImageSwitcher::OnPaint(IRenderTarget *pRT)
	{
		CRect rtWnd = GetClientRect();

		if ( m_iMoveWidth == 0 )
		{	// 显示当前图片
			IBitmap *pBmp = m_lstImages[m_iSelected];
			SIZE szImg  = pBmp->Size();
			RECT rtImg  = GetDefaultDest(rtWnd, szImg);

			pRT->DrawBitmapEx(&rtImg, pBmp, CRect(CPoint(),szImg), EM_NULL);
		}
		else
		{	// 图片切换的动画效果[过滤图片，其实最多画两张，这儿不纠结这点]
			DrawImage(pRT, rtWnd, m_iSelected - 1);
			DrawImage(pRT, rtWnd, m_iSelected);
			DrawImage(pRT, rtWnd, m_iSelected + 1);
		}
	}

	BOOL  SImageSwitcher::Switch(int iSelect)
	{
		if (iSelect >= (int)m_lstImages.GetCount() || iSelect < 0)
			return FALSE;

		CRect rcWnd = GetClientRect();
		if(m_bTimerMove)
			return TRUE;	// 正在显示过渡效果时，不再切换

		m_iMoveWidth = (iSelect-m_iSelected)*rcWnd.Width();
		m_iSelected = iSelect;

		m_iTimesMove = (m_iMoveWidth > 0 ? m_iMoveWidth : -m_iMoveWidth) / 10;
		if(m_iTimesMove < 20)
			m_iTimesMove = 20;
		SetTimer(TIMER_MOVE, 30);
		m_bTimerMove = TRUE;

		return TRUE;
	}

	void SImageSwitcher::OnLButtonDown(UINT nFlags, CPoint point)
	{
		//if(m_bWantMove)
		//	return;

		//if(m_bTimerMove)
		//{
		//	m_bTimerMove = FALSE;
		//	KillTimer(TIMER_MOVE);
		//	m_bWantMove = TRUE;
		//	m_iDownX = point.x - m_iMoveWidth;
		//	SetCapture();

		//}
		//else
		//{
		//	m_bWantMove = TRUE;
		//	m_iDownX = point.x;
		//	SetCapture();
		//}
		__super::OnLButtonDown(nFlags,point);
	}
	void SImageSwitcher::OnMouseMove(UINT nFlags,CPoint pt)
	{
		__super::OnMouseMove(nFlags,pt);

		//CRect rcWnd = GetWindowRect();

		//if(m_bWantMove)
		//{
		//	m_iMoveWidth = pt.x - m_iDownX;
		//	if(m_iSelected == 0 && m_iMoveWidth > rcWnd.Width())
		//	{
		//		m_iMoveWidth = rcWnd.Width();
		//	}
		//	if(m_iSelected == (int)m_lstImages.GetCount()-1 && -m_iMoveWidth > GetWindowRect().Width())
		//	{
		//		m_iMoveWidth = -rcWnd.Width();
		//	}
		//	Invalidate();
		//	return;
		//}

	}
	void SImageSwitcher::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		__super::OnLButtonUp(nFlags,pt);

		//CRect rcWnd = GetClientRect();

		//if(m_bWantMove)
		//{
		//	m_bWantMove = FALSE;
		//	ReleaseCapture();
		//	if(m_iMoveWidth > 0)
		//	{
		//		if(m_iSelected > 0 && m_iMoveWidth > rcWnd.Width()/4)
		//		{
		//			m_iMoveWidth -= rcWnd.Width();
		//			m_iSelected--;
		//		}
		//	}
		//	else
		//	{
		//		if(m_iSelected < (int)m_lstImages.GetCount()-1 && -m_iMoveWidth > rcWnd.Width()/4)
		//		{
		//			m_iMoveWidth += rcWnd.Width();
		//			m_iSelected++;
		//		}
		//	}
		//	m_iTimesMove = (m_iMoveWidth>0?m_iMoveWidth:-m_iMoveWidth)/10;
		//	if(m_iTimesMove < 20)m_iTimesMove = 20;
		//	SetTimer(TIMER_MOVE, 30);
		//	m_bWantMove = FALSE;
		//	m_bTimerMove = TRUE;
		//	return;
		//}
	}

	void SImageSwitcher::OnTimer(char nIDEvent)
	{
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

	void SImageSwitcher::InsertImage(int iTo, IBitmap * pImage)
	{
		if(iTo<0) iTo = m_lstImages.GetCount();
		m_lstImages.InsertAt(iTo,pImage);
		pImage->AddRef();
		Invalidate();
	}

	void SImageSwitcher::RemoveAll()
	{
		for(UINT i=0;i<m_lstImages.GetCount();i++)
		{
			m_lstImages[i]->Release();
		}
		m_lstImages.RemoveAll();
	}

	int SImageSwitcher::GetCurSel()
	{
		return m_iSelected;
	}

	HRESULT SImageSwitcher::OnAttrImages(const SStringT& strValue,BOOL bLoading)
	{
		SStringWList imgLstSrc ;
		SplitString(strValue,L'|',imgLstSrc);
		for(UINT i=0;i<imgLstSrc.GetCount();i++)
		{
			IBitmap * pImg = LOADIMAGE2(imgLstSrc[i]);
			if(pImg) 
			{
				m_lstImages.Add(pImg);
			}
		}
		return bLoading?S_OK:S_FALSE;
	}

	size_t SImageSwitcher::GetCount()
	{
		return m_lstImages.GetCount();
	}


}