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
		m_bImgMovable = FALSE;	// Ĭ�ϲ������ƶ�
		if ( m_ptCenter.x == 0 || m_ptCenter.y == 0 )
		{	// �״�������ʾ��ǰͼƬ�� ��ʾĬ�ϵ�ȫͼ
			SetRect(&rtImg, 0, 0, szImg.cx, szImg.cy);
			m_ptCenter.SetPoint(szImg.cx/2, szImg.cy/2);
			return GetDefaultDest(rtWnd, szImg, &m_fRatio);
		}

		BOOL bOutWnd = FALSE;	// �Ƿ��г���rtWnd
		SIZE szReal  = {(LONG)(szImg.cx * m_fRatio), (LONG)(szImg.cy * m_fRatio)};			// �õ�ʵ��Ҫ��ʾ�Ĵ�С
		POINT ptReal = {(LONG)(m_ptCenter.x * m_fRatio), (LONG)(m_ptCenter.y * m_fRatio)};	// �õ�����λ�õ�ʵ��Ҫ��ʾ��λ��
		CRect rtDst  = rtWnd;

		if ( rtWnd.Width() >= szReal.cx && rtWnd.Height() >= szReal.cy )
		{	// ����ʾ����֮���ȫͼ
			float fRatio = 0;	// dummy
			rtDst = GetDefaultDest(rtWnd, szReal);		// ��ʵ��Ҫ��ʾ�Ĵ�С��������Ŀ��λ��
			rtImg.SetRect(0, 0, szImg.cx, szImg.cy);	
			m_ptCenter.SetPoint(szImg.cx / 2, szImg.cy / 2);	// ��λ���ĵ�ΪͼƬ����
		}
		else
		{	// ������ʾȫͼ
			m_bImgMovable = TRUE;	// �����ƶ�
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
						bOutWnd = TRUE;
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
						bOutWnd = TRUE;
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
						bOutWnd = TRUE;
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
						bOutWnd = TRUE;
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

			if ( bOutWnd )
			{	// �������ĵ�
				if ( rtImg.Width() % 2) 
					rtImg.right--;	// ���ż��

				if ( rtImg.Height() % 2)
					rtImg.bottom--;	// ���ż��

				m_ptCenter.x = rtImg.left + rtImg.Width() / 2;
				m_ptCenter.y = rtImg.top  + rtImg.Height() / 2;
			}
		}

		return rtDst;
	}

	RECT SImageViewer::GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, float* pfRatio)
	{	// ����Ĭ�ϵ�Ŀ��λ��
		RECT rtDst = { 0 };

		if ( rtWnd.Width() >= szImg.cx && rtWnd.Height() >= szImg.cy )
		{	// ��������ʾȫͼ
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
			{	// ���ڿ�Ȳ�����ʾȫͼ, ���Կ�Ƚ�������
				if ( pfRatio )
					*pfRatio = fRatioX;

				LONG lHeight= (LONG)(fRatioX * szImg.cy);
				rtDst.left	= rtWnd.left;
				rtDst.right = rtWnd.right;
				rtDst.top   = rtWnd.top + (LONG)((rtWnd.Height() - lHeight) / 2.0);
				rtDst.bottom= rtDst.top + lHeight;
			}
			else
			{	// ���ڸ߶Ȳ�����ʾȫͼ, ���Ը߶Ƚ�������
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
		{	// ��ʾ��ǰͼƬ
			CRect szOld = m_rtImgSrc;
			RECT  rtDst = GetDest(rtWnd, m_pImgSel->Size(), m_rtImgSrc);
			pRT->DrawBitmapEx(&rtDst, m_pImgSel, &m_rtImgSrc, EM_STRETCH);
			SAFE_RELEASE_(m_pImgNext);	// ��/��һ��ͼƬ�����ɶ���ʱ����Ҫ

			if ( !m_rtImgSrc.EqualRect(szOld) )
			{	// ͼƬ����ʾ������ˣ���֪ͨ
				EventImagePosChanged evt(this, m_bImgMovable, m_rtImgSrc, m_fRatio, m_pImgSel);
				FireEvent(evt);
			}
		}
		else
		{	
			if ( m_iMoveWidth > 0 )
			{	// ��һ֡
				DrawImage(pRT, m_pImgNext, rtWnd, m_iSelected - 1);
				DrawImage(pRT, m_pImgSel,  rtWnd, m_iSelected);
			}
			else
			{	// ��һ֡
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
			return TRUE;	// ������ʾ����Ч��ʱ�������л�

		m_rtImgSrc.SetRectEmpty();	// ���
		EventImagePosChanged evt(this, FALSE, m_rtImgSrc, NULL);
		FireEvent(evt);				// ���ص�ͼ

		m_ptCenter.SetPoint(0, 0);	// Reset
		if ( bMoive == FALSE )
		{	// ����ʾ����
			m_iSelected = iSelect;
			m_pImgSel = LOADIMAGE(_T("file"), m_vectImage[m_iSelected]);
			return TRUE;
		}

		m_iMoveWidth = (iSelect-m_iSelected)*rcWnd.Width();
		m_iSelected = iSelect;

		// ���¼��ص�ͼƬ
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

			if ( m_fRatio > 16.0f )
				m_fRatio = 16.0f;	// Max as 1600%
		}

		Invalidate();

		// �����Ŵ��ʸ����¼�
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