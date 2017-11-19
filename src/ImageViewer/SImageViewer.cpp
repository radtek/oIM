#include "StdAfx.h"
#include "SImageViewer.h"
#include <helper/SplitString.h>
#include "../../ext/soui/controls.extend/FileHelper.h"

#define TIMER_MOVE			125
#define TIMER_GIF			126
#define TIMER_GIF_ELAPSE	(100)	// 0.1s

namespace SOUI
{
	SImageViewer::SImageViewer()
		: m_pImgSel(NULL)
		, m_pImgNext(NULL)
		, m_pImgGif(NULL)
		, m_iMoveWidth(0)
		, m_bTimerMove(FALSE)
		, m_iSelected(0)
		, m_iTimesMove(0)
		, m_fRatio(1.f)
		, m_ptCenter(0)
		, m_bSwitched(FALSE)
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
		if ( m_bSwitched )
		{	// �״�������ʾ��ǰͼƬ�� ��ʾĬ�ϵ�ȫͼ
			m_bSwitched = FALSE;
			if ( m_pImgGif )
				SetTimer(TIMER_GIF, TIMER_GIF_ELAPSE);	// 100ms

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
			CRect szSrcOld = m_rtImgSrc;
			CRect szDstOld = m_rtImgDst;
			m_rtImgDst = GetDest(rtWnd, m_pImgSel->Size(), m_rtImgSrc);
			pRT->DrawBitmapEx(&m_rtImgDst, m_pImgSel, &m_rtImgSrc, EM_STRETCH);
			SAFE_RELEASE_(m_pImgNext);	// ��/��һ��ͼƬ�����ɶ���ʱ����Ҫ

			if ( !m_rtImgSrc.EqualRect(szSrcOld) || !m_rtImgDst.EqualRect(szDstOld) || m_pImgGif )
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

	BOOL  SImageViewer::Switch(int iSelect, BOOL bMoive, LPSIZE pSize)
	{
#define LOADIMAGE_(N, R) do {R=LOADIMAGE(_T("file"), N); if (R==NULL) R=LOADIMAGE(_T("IMG"), _T("IMG_ERROR")); }while(0)

		if (iSelect >= (int)m_vectImage.size() || iSelect < 0)
			return FALSE;

		CRect rcWnd = GetClientRect();
		if(m_bTimerMove)
			return TRUE;	// ������ʾ����Ч��ʱ�������л�

		m_rtImgSrc.SetRectEmpty();	// ���
		EventImagePosChanged evt(this, FALSE, m_rtImgSrc, NULL);
		FireEvent(evt);				// ���ص�ͼ

		m_bSwitched = TRUE;
		m_ptCenter.SetPoint(0, 0);	// Reset
		m_iMoveWidth = (m_iSelected - iSelect)*rcWnd.Width();
		m_iSelected = iSelect;

		// ���¼��ص�ͼƬ
		SAFE_RELEASE_(m_pImgSel);
		SAFE_RELEASE_(m_pImgNext);
		SAFE_RELEASE_(m_pImgGif);
		SStringT szExt = PathFindExtension(m_vectImage[m_iSelected]);
		if ( szExt.CompareNoCase(_T(".gif")) == 0 )
		{
			m_pImgGif = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(SSkinAPNG::GetClassName());
			if ( m_pImgGif && m_pImgGif->LoadFromFile(m_vectImage[m_iSelected]) > 1 )
			{
				m_pImgGif->AddRef();
				m_pImgSel = m_pImgGif->GetFrameImage();
			}
			else
				SAFE_RELEASE_(m_pImgGif);	// ���Ǵ���һ֡��GIF��������ͨͼƬ
		}
		
		if ( m_pImgSel == NULL )
			LOADIMAGE_(m_vectImage[m_iSelected], m_pImgSel);

		if ( pSize )
			*pSize = m_pImgSel->Size();

		int i32Next = m_iMoveWidth > 0 ? m_iSelected + 1 : m_iSelected - 1;
		if ( i32Next >= 0 && i32Next < (int)m_vectImage.size() )
			LOADIMAGE_(m_vectImage[i32Next], m_pImgNext);
		else
			bMoive = FALSE;	// û���ˣ���ֹ��ҳ����

		if ( bMoive )
		{
			m_iTimesMove = (m_iMoveWidth > 0 ? m_iMoveWidth : -m_iMoveWidth) / 30;
			if(m_iTimesMove < 20)
				m_iTimesMove = 20;

			SetTimer(TIMER_MOVE, 30);
			m_bTimerMove = TRUE;
			return TRUE;
		}

		m_iMoveWidth = 0;
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
		
	BOOL SImageViewer::RealSize()
	{
		m_fRatio = 1.0f;
		Invalidate();
			
		// �����Ŵ��ʸ����¼�
		EventRatioChanged evt(this, m_fRatio);
		FireEvent(evt);
		return TRUE;
	}

	float SImageViewer::Zoom(float fDelta, BOOL bFixed)
	{
		if ( bFixed )
		{	
			m_fRatio += fDelta;	// �̶���Delta
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

		if ( m_fRatio < RATIOF_(RATIO_MIN) )
			m_fRatio =  RATIOF_(RATIO_MIN);		// Min 
		else if ( m_fRatio > 0.1f )
		{
			if ( bFixed == FALSE )
			{
				INT i32Ratio = (INT)(m_fRatio * 10.0f);
				m_fRatio = i32Ratio / 10.0f;
			}

			if ( m_fRatio > RATIOF_(RATIO_MAX) )
				m_fRatio = RATIOF_(RATIO_MAX);	// Max
		}

		Invalidate();

		// �����Ŵ��ʸ����¼�
		EventRatioChanged evt(this, m_fRatio);
		FireEvent(evt);

		return m_fRatio;
	}

	BOOL SImageViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		BOOL bRet	= __super::OnMouseWheel(nFlags, zDelta, pt);
		BOOL bFixed = GetAsyncKeyState(VK_CONTROL);	// Ctrl + Wheel
		float fDelta= (zDelta / WHEEL_DELTA) / 100.0f;

		if ( bFixed )
			Zoom(fDelta * 10.0f, TRUE);	// �̶���Delta
		else
			Zoom(fDelta);

		return bRet;
	}

	void SImageViewer::OnTimer(char nIDEvent)
	{
		if ( nIDEvent == TIMER_MOVE )
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
		else if ( nIDEvent == TIMER_GIF )
		{
			if ( m_pImgGif == NULL )
			{
				KillTimer(TIMER_GIF);
				return;
			}

			m_pImgGif->ActiveNextFrame();
			m_pImgSel = m_pImgGif->GetFrameImage();
			Invalidate();
		}
		else
			return SetMsgHandled(FALSE);
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

	CPoint SImageViewer::Move(int i32Oper, LPPOINT ptCenter)
	{
		switch( i32Oper )
		{
		case MOVE_POS_START:
			m_ptCenterOld = m_ptCenter;
			return m_ptCenterOld;
		case MOVE_POS_MOVING:
		case MOVE_POS_STOP:
			if ( ptCenter && (m_ptCenter.x != ptCenter->x || m_ptCenter.y != ptCenter->y) )
			{
				m_ptCenter.SetPoint(ptCenter->x, ptCenter->y);
				Invalidate();
			}
			return m_ptCenter;
		default:
			SASSERT(FALSE);
			break;
		}

		return m_ptCenter;
	}

	BOOL SImageViewer::OpenFolder(LPCTSTR pszPathImage)
	{
		if ( m_iSelected < 0 || m_iSelected >= (int)m_vectImage.size() )
			return FALSE;

		SStringT szImage = pszPathImage ? pszPathImage : m_vectImage[m_iSelected];
		if ( !PathFileExists(szImage) )
		{
			TCHAR szPath[MAX_PATH] = { 0 };

			_tcsncpy(szPath, szImage, MAX_PATH);
			PathRemoveFileSpec(szPath);
			::ShellExecute(NULL, NULL, _T("explorer.exe"), szPath, NULL, SW_SHOW);
		}
		else
		{
			SStringT szCmd;

			if ( PathIsDirectory(szImage) )
				szCmd = szImage;
			else
				szCmd.Format(_T("/select,\"%s\""), szImage);

			::ShellExecute(NULL, NULL, _T("explorer.exe"), szCmd, NULL, SW_SHOW);
		}

		return TRUE;
	}

	BOOL SImageViewer::Saveas(BOOL bOpenFolder)
	{
		if ( m_iSelected < 0 || m_iSelected >= (int)m_vectImage.size() )
			return FALSE;
		
		SStringT szImage = m_vectImage[m_iSelected];
		if ( !PathFileExists(szImage) )
			return FALSE;

		SStringT szFilter;
		TCHAR* pszExt = PathFindExtension(szImage);
		TCHAR* pszName= PathFindFileName(szImage);
		szFilter.Format(_T("Image file(*%s)|*%s||"), pszExt, pszExt);
		CFileDialogEx dlgFile(FALSE, szImage, pszName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter );

		if ( IDOK != dlgFile.DoModal() )
			return FALSE;

		if ( CopyFile(szImage, dlgFile.GetPathName(), FALSE) )
		{
			if ( bOpenFolder )
				OpenFolder(dlgFile.GetPathName());
			return TRUE;
		}

		return FALSE;
	}

	BOOL SImageViewer::Rotate(BOOL bRight)
	{
		return TRUE;
	}

}