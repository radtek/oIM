#include "StdAfx.h"
#include "SImageSwitcher.h"
#include <helper/SplitString.h>

#define TIMER_MOVE		1

namespace SOUI
{
	SImageSwitcher::SImageSwitcher()
		:m_bWantMove(FALSE)
		,m_iDownX(0)
		,m_iMoveWidth(0)
		,m_bTimerMove(0)
		,m_iSelected(0)
		,m_iTimesMove(0)
	{
	}

	SImageSwitcher::~SImageSwitcher()
	{
		RemoveAll();
	}

	BOOL SImageSwitcher::DrawImage(IRenderTarget *pRT, CRect& rcWnd, int i32Index)
	{
		if ( i32Index >= 0 && i32Index < (int)m_lstImages.GetCount() )
		{
			RECT rtImg;
			IBitmap *pBmp = m_lstImages[i32Index];
			SIZE szImg    = pBmp->Size();

			rtImg.left	= rcWnd.left + (i32Index * rcWnd.Width() - (m_iSelected * rcWnd.Width()) + m_iMoveWidth);
			rtImg.right = rtImg.left + rcWnd.Width();
			rtImg.top	= rcWnd.top; 
			rtImg.bottom= rtImg.top + rcWnd.Height();
			pRT->DrawBitmapEx(&rtImg, pBmp, CRect(CPoint(), szImg), EM_STRETCH);
		}

		return FALSE;
	}

	void SImageSwitcher::OnPaint(IRenderTarget *pRT)
	{
		CRect rcWnd = GetClientRect();

		if ( m_iMoveWidth == 0 )
		{	// ��ʾ��ǰͼƬ
			IBitmap *pBmp = m_lstImages[m_iSelected];
			SIZE szImg  = pBmp->Size();
			pRT->DrawBitmapEx(rcWnd, pBmp, CRect(CPoint(),szImg), EM_STRETCH);
		}
		else
		{	// ͼƬ�л��Ķ���Ч��[����ͼƬ����ʵ��໭���ţ�������������]
			DrawImage(pRT, rcWnd, m_iSelected - 1);
			DrawImage(pRT, rcWnd, m_iSelected);
			DrawImage(pRT, rcWnd, m_iSelected + 1);
		}
	}

	BOOL  SImageSwitcher::Switch(int iSelect)
	{
		if (iSelect >= (int)m_lstImages.GetCount() || iSelect < 0)
			return FALSE;

		CRect rcWnd = GetClientRect();
		if(m_bTimerMove)
			return TRUE;	// ������ʾ����Ч��ʱ�������л�

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