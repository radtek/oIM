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
		, m_fRatio(1.f)
		, m_ptCenter(0)
		, m_bMovable(FALSE)
	{

	}

	SImageSwitcher::~SImageSwitcher()
	{
		RemoveAll();
	}

	RECT SImageSwitcher::GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg)
	{
		m_bMovable = FALSE;	// Ĭ�ϲ������ƶ�
		if ( m_ptCenter.x == 0 || m_ptCenter.y == 0 )
		{	// �״�������ʾ��ǰͼƬ�� ��ʾĬ�ϵ�ȫͼ
			SetRect(&rtImg, 0, 0, szImg.cx, szImg.cy);
			m_ptCenter.SetPoint(szImg.cx/2, szImg.cy/2);
			return GetDefaultDest(rtWnd, szImg);
		}

		SIZE szReal  = {(LONG)(szImg.cx * m_fRatio), (LONG)(szImg.cy * m_fRatio)};			// �õ�ʵ��Ҫ��ʾ�Ĵ�С
		POINT ptReal = {(LONG)(m_ptCenter.x * m_fRatio), (LONG)(m_ptCenter.y * m_fRatio)};	// �õ�����λ�õ�ʵ��Ҫ��ʾ��λ��
		CRect rtDst  = rtWnd;

		if ( rtWnd.Width() >= szReal.cx && rtWnd.Height() >= szReal.cy )
		{	// ����ʾ����֮���ȫͼ
			rtDst = GetDefaultDest(rtWnd, szReal, FALSE);		// ��ʵ��Ҫ��ʾ�Ĵ�С��������Ŀ��λ��
			rtImg.SetRect(0, 0, szImg.cx, szImg.cy);	
			m_ptCenter.SetPoint(szImg.cx / 2, szImg.cy / 2);	// ��λ���ĵ�ΪͼƬ����
		}
		else
		{	// ������ʾȫͼ
			m_bMovable = TRUE;	// �����ƶ�
			rtImg.SetRect(0, 0, szReal.cx, szReal.cy);
			if ( szReal.cx >= rtDst.Width() )
			{	// ͼƬ��� >= ���ڿ��
				int iOffsetX= (ptReal.x  - szReal.cx / 2);		// ����ʾͼƬ��ˮƽ����λ�õ�ƫ����
				int iDeltaX1= (szReal.cx - rtDst.Width()) / 2;
				
				// ��ͼƬ����λ��������λ��
				rtImg.left += iDeltaX1;
				rtImg.right-= iDeltaX1;	

				// ����Xƫ��
				if ( iOffsetX >= 0 )
				{	// ���ĵ�����ƫ��
					if ( rtImg.right + iOffsetX <= szReal.cx )
					{	// ����ƫ�ƺ���Ȼ��ͼƬ��С֮��
						rtImg.left += iOffsetX;
						rtImg.right+= iOffsetX;
					}
					else
					{	// ͼƬ���ұ�
						rtImg.left += (szReal.cx - rtImg.right);
						rtImg.right = szReal.cx;
					}
				}
				else
				{	// ���ĵ�����ƫ��
					if ( rtImg.left + iOffsetX >= 0 )
					{	// ����ƫ�ƺ���Ȼ��ͼƬ��С֮��
						rtImg.left += iOffsetX;
						rtImg.right+= iOffsetX;
					}
					else
					{	// ͼƬ�����
						rtImg.right-= rtImg.left;
						rtImg.left	= 0;
					}
				}
			}
			else
			{	// ͼƬ��� < ���ڿ��
				int iDeltaX2= (rtDst.Width() - szReal.cx ) / 2;
				rtDst.left += iDeltaX2;
				rtDst.right-= iDeltaX2;
			}

			if ( szReal.cy >= rtDst.Height() )
			{	// ͼƬ�߶� >= ���ڸ߶�
				int iOffsetY = (ptReal.y  - szReal.cy / 2);		// ����ʾͼƬ�Ĵ�ֱ����λ�õ�ƫ����
				int iDeltaY1 = (szReal.cy - rtDst.Height()) / 2;

				// ��ͼƬ����λ��������λ��
				rtImg.top   += iDeltaY1;	
				rtImg.bottom-= iDeltaY1;

				// ����Yƫ��
				if ( iOffsetY >= 0 )
				{	// ���ĵ�����ƫ��
					if ( rtImg.bottom + iOffsetY <= szReal.cy )
					{	// ����ƫ�ƺ���Ȼ��ͼƬ��С֮��
						rtImg.top	+= iOffsetY;
						rtImg.bottom+= iOffsetY;
					}
					else
					{	// ͼƬ���±�
						rtImg.top	+= (szReal.cy - rtImg.bottom);
						rtImg.bottom = szReal.cy;
					}
				}
				else
				{	// ���ĵ�����ƫ��
					if ( rtImg.top + iOffsetY >= 0 )
					{	// ����ƫ�ƺ���Ȼ��ͼƬ��С֮��
						rtImg.top	+= iOffsetY;
						rtImg.bottom+= iOffsetY;
					}
					else
					{	// ͼƬ���ϱ�
						rtImg.bottom-= rtImg.top;
						rtImg.top	 = 0;
					}
				}
			}
			else
			{	// ͼƬ�߶� < ���ڸ߶�
				int iDeltaY2 = (rtDst.Height() - szReal.cy) / 2;
				rtDst.top   += iDeltaY2;
				rtDst.bottom-= iDeltaY2;
			}

			// �ٻ����ԭͼ������
			rtImg.left  = (LONG)(rtImg.left / m_fRatio);
			rtImg.top   = (LONG)(rtImg.top / m_fRatio);
			rtImg.right = (LONG)(rtImg.right / m_fRatio);
			rtImg.bottom= (LONG)(rtImg.bottom / m_fRatio);

			// �������ĵ�
			m_ptCenter.x = rtImg.left + rtImg.Width() / 2;
			m_ptCenter.y = rtImg.top  + rtImg.Height() / 2;
		}

		return rtDst;
	}

	RECT SImageSwitcher::GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, BOOL bRatio)
	{	// ����Ĭ�ϵ�Ŀ��λ��
		RECT rtDst = { 0 };

		if ( rtWnd.Width() >= szImg.cx && rtWnd.Height() >= szImg.cy )
		{	// ��������ʾȫͼ
			if ( bRatio )
				m_fRatio = 1.f;

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
			{	// ���ڿ�Ȳ�����ʾȫͼ, ���Կ�Ƚ�������
				if ( bRatio )
					m_fRatio = fRatioX;

				LONG lHeight= (LONG)(fRatioX * szImg.cy);
				rtDst.left	= rtWnd.left;
				rtDst.right = rtWnd.right;
				rtDst.top   = rtWnd.top + (LONG)((rtWnd.Height() - lHeight) / 2.0);
				rtDst.bottom= rtDst.top + lHeight;
			}
			else
			{	// ���ڸ߶Ȳ�����ʾȫͼ, ���Ը߶Ƚ�������
				if ( bRatio )
					m_fRatio = fRatioY;

				LONG lWidth = (LONG)(fRatioY * szImg.cx);
				rtDst.top   = rtWnd.top;
				rtDst.bottom= rtWnd.bottom;
				rtDst.left  = rtWnd.left + (LONG)((rtWnd.Width() - lWidth) / 2.0);
				rtDst.right = (LONG)(rtDst.left + lWidth);
			}
		}

		return rtDst;
	}

	BOOL SImageSwitcher::DrawImage(IRenderTarget *pRT, CRect& rtWnd, int i32Index)
	{
		if ( i32Index >= 0 && i32Index < (int)m_lstImages.GetCount() )
		{
			IBitmap *pBmp = m_lstImages[i32Index];
			SIZE szImg	= pBmp->Size();
			RECT rtDst	= GetDefaultDest(rtWnd, szImg);
			int i32Delta= (i32Index * rtWnd.Width() - (m_iSelected * rtWnd.Width()) + m_iMoveWidth);
			rtDst.left += i32Delta;
			rtDst.right+= i32Delta;
			pRT->DrawBitmapEx(&rtDst, pBmp, CRect(CPoint(), szImg), EM_STRETCH);
		}

		return FALSE;
	}

	void SImageSwitcher::OnPaint(IRenderTarget *pRT)
	{
		if ( m_lstImages.IsEmpty() )
			return;

		CRect rtWnd = GetClientRect();
		if ( m_iMoveWidth == 0 )
		{	// ��ʾ��ǰͼƬ
			IBitmap *pBmp = m_lstImages[m_iSelected];
			RECT rtDst = GetDest(rtWnd, pBmp->Size(), m_rtImgSrc);

			pRT->DrawBitmapEx(&rtDst, pBmp, &m_rtImgSrc, EM_STRETCH);
		}
		else
		{	
			if ( m_iMoveWidth > 0 )
			{	// ��һ֡
				DrawImage(pRT, rtWnd, m_iSelected - 1);
				DrawImage(pRT, rtWnd, m_iSelected);
			}
			else
			{	// ��һ֡
				DrawImage(pRT, rtWnd, m_iSelected);
				DrawImage(pRT, rtWnd, m_iSelected + 1);
			}
		}
	}

	BOOL  SImageSwitcher::Switch(int iSelect, BOOL bMoive)
	{
		if (iSelect >= (int)m_lstImages.GetCount() || iSelect < 0)
			return FALSE;

		CRect rcWnd = GetClientRect();
		if(m_bTimerMove)
			return TRUE;	// ������ʾ����Ч��ʱ�������л�

		m_ptCenter.SetPoint(0, 0);	// Reset
		if ( bMoive == FALSE )
		{	// ����ʾ����
			m_iSelected = iSelect;
			return TRUE;
		}

		m_iMoveWidth = (iSelect-m_iSelected)*rcWnd.Width();
		m_iSelected = iSelect;

		m_iTimesMove = (m_iMoveWidth > 0 ? m_iMoveWidth : -m_iMoveWidth) / 20;
		if(m_iTimesMove < 20)
			m_iTimesMove = 20;

		SetTimer(TIMER_MOVE, 30);
		m_bTimerMove = TRUE;

		return TRUE;
	}

	void SImageSwitcher::OnLButtonDown(UINT nFlags, CPoint point)
	{
		m_ptMoveOld  = point;
		m_ptCenterOld= m_ptCenter;
		SetCapture();
		__super::OnLButtonDown(nFlags,point);
	}

	void SImageSwitcher::OnMouseMove(UINT nFlags,CPoint pt)
	{
		__super::OnMouseMove(nFlags,pt);
		if ( m_bMovable && (nFlags & MK_LBUTTON) )
		{
			m_ptCenter.x = m_ptCenterOld.x - (LONG)((pt.x - m_ptMoveOld.x) / m_fRatio);
			m_ptCenter.y = m_ptCenterOld.y - (LONG)((pt.y - m_ptMoveOld.y) / m_fRatio);
			Invalidate();
		}
	}

	void SImageSwitcher::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		__super::OnLButtonUp(nFlags,pt);
		ReleaseCapture();
		if ( m_bMovable )
		{
			m_ptCenter.x = m_ptCenterOld.x - (LONG)((pt.x - m_ptMoveOld.x) / m_fRatio);
			m_ptCenter.y = m_ptCenterOld.y - (LONG)((pt.y - m_ptMoveOld.y) / m_fRatio);
			Invalidate();
		}
	}

	BOOL SImageSwitcher::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		BOOL bRet	= __super::OnMouseWheel(nFlags, zDelta, pt);
		BOOL bFixed = GetAsyncKeyState(VK_CONTROL);	// Ctrl + Wheel
		float fDelta= (zDelta / WHEEL_DELTA) / 100.0f;

		if ( bFixed )
		{	
			m_fRatio += fDelta * 10.0f;	// �̶���Delta
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

			if ( m_fRatio > 20.0f )
				m_fRatio = 20.0f;	// Max as 2000%
		}

		Invalidate();
		return bRet;
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

	BOOL SImageSwitcher::InsertImage(const SStringT& szImage, int iTo)
	{
		if(IBitmap * pImg = LOADIMAGE(_T("file"), szImage)) 
			return InsertImage(pImg, iTo);
		
		return FALSE;
	}

	BOOL SImageSwitcher::InsertImage(IBitmap * pImage, int iTo)
	{
		if(iTo<0) iTo = m_lstImages.GetCount();
		m_lstImages.InsertAt(iTo,pImage);
		pImage->AddRef();
		//Invalidate();

		return TRUE;
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