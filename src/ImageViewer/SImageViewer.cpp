#include "StdAfx.h"
#include "SImageViewer.h"
#include <helper/SplitString.h>
#include "../../ext/soui/controls.extend/FileHelper.h"

#define TIMER_MOIVE			121
#define TIMER_GIF			123
#define TIMER_GIF_ELAPSE	(100)	// 0.1s

#define LOADIMAGE_(N, R) do {R=LOADIMAGE(_T("file"), N); m_bError=FALSE; if (R==NULL) {R=LOADIMAGE(_T("IMG"), _T("IMG_ERROR")); m_bError=TRUE; }}while(0)

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
		, m_nAngle(0)
		, m_ptCenter(0)
		, m_bSwitched(FALSE)
		, m_bImgMovable(FALSE)
		, m_bError(FALSE)
		, m_eMove(eMOVE_NONE)
	{

	}

	SImageViewer::~SImageViewer()
	{
		RemoveAll();
	}

	inline RECT SImageViewer::GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg)
	{
		if ( m_bSwitched )
		{	// �״�������ʾ��ǰͼƬ�� ��ʾĬ�ϵ�ȫͼ
			m_bImgMovable = FALSE;	// Ĭ�ϲ������ƶ�
			m_bSwitched = FALSE;
			if ( m_pImgGif )
				SetTimer(TIMER_GIF, TIMER_GIF_ELAPSE);	// 100ms

			SetRect(&rtImg, 0, 0, szImg.cx, szImg.cy);
			m_ptCenter.SetPoint(szImg.cx/2, szImg.cy/2);
			RECT rtDst = GetDefaultDest(rtWnd, szImg, &m_fRatio);
		
			// �����Ŵ��ʸ����¼�
			EventRatioChanged evt(this, m_fRatio);
			FireEvent(evt);

			return rtDst;
		}

		BOOL bOutWnd = FALSE;	// �Ƿ��г���rtWnd
		SIZE szReal  = {(LONG)(szImg.cx * m_fRatio), (LONG)(szImg.cy * m_fRatio)};			// �õ�ʵ��Ҫ��ʾ�Ĵ�С
		POINT ptReal = {(LONG)(m_ptCenter.x * m_fRatio), (LONG)(m_ptCenter.y * m_fRatio)};	// �õ�����λ�õ�ʵ��Ҫ��ʾ��λ��
		CRect rtDst  = rtWnd;

		if ( rtWnd.Width() >= szReal.cx && rtWnd.Height() >= szReal.cy )
		{	// ����ʾ����֮���ȫͼ
			m_bImgMovable = FALSE;	// Ĭ�ϲ������ƶ�
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
				int iOffsetX = (ptReal.x  - szReal.cx / 2);		// ����ʾͼƬ��ˮƽ����λ�õ�ƫ����
				int iDeltaX1 = (szReal.cx - rtDst.Width()) / 2;
				
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
						rtImg.right = szReal.cx;
						rtImg.left  = rtImg.right - rtDst.Width();
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
						rtImg.left	= 0;
						rtImg.right = rtDst.Width();
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
						rtImg.bottom = szReal.cy;
						rtImg.top	 = rtImg.bottom - rtDst.Height();
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
						rtImg.top	 = 0;
						rtImg.bottom = rtDst.Height();
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

			if ( bOutWnd && m_eMove == eMOVE_STOP )
			{	// �������ĵ�
				m_eMove = eMOVE_NONE;
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

	inline BOOL SImageViewer::DrawImage(IRenderTarget *pRT, IBitmap *pBmp, CRect& rtWnd, int i32Index)
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

			if ( !m_rtImgSrc.EqualRect(szSrcOld) || // ͼƬԴ����ı���
				 !m_rtImgDst.EqualRect(szDstOld) || // ͼƬĿ������ı��ˣ����ڴ�С���ˣ�
				  m_pImgGif )						// ��GIF��������ͼҪˢ�¶���
			{	// ͼƬ����ʾ������ˣ���֪ͨ
				EventImagePosChanged evt(this, m_bImgMovable, !!m_pImgGif, m_rtImgSrc, m_fRatio, m_bError, m_pImgSel);
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

	BOOL  SImageViewer::Reset(BOOL bNoAngle, BOOL bCloneSel)
	{
		RemoveTempImage();

		m_rtImgSrc.SetRectEmpty();	// ���
		EventImagePosChanged evt(this, FALSE, FALSE, m_rtImgSrc, FALSE, NULL);
		FireEvent(evt);				// ���ص�ͼ

		if ( !bNoAngle )
			m_nAngle = 0;

		m_bSwitched = TRUE;
		m_eMove = eMOVE_NONE;
		m_ptCenter.SetPoint(0, 0);	// ��λ���ĵ�


		SAFE_RELEASE_(m_pImgNext);
		if ( bCloneSel && m_pImgSel )
			m_pImgSel->Clone(&m_pImgNext);

		SAFE_RELEASE_(m_pImgSel);
		SAFE_RELEASE_(m_pImgGif);

		return TRUE;
	}

	BOOL  SImageViewer::Switch(int iSelect, BOOL bMoive, LPSIZE pSize)
	{
		if (iSelect >= (int)m_vectImage.size() || iSelect < 0)
			return FALSE;

		CRect rcWnd = GetClientRect();
		if(m_bTimerMove)
			return TRUE;	// ������ʾ����Ч��ʱ�������л�

		Reset(FALSE, TRUE);
		m_bError = FALSE;
		m_iMoveWidth = (m_iSelected - iSelect)*rcWnd.Width();
		m_iSelected  = iSelect;
		
		// ���¼��ص�ͼƬ
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

		if ( bMoive && m_pImgNext )
		{
			m_iTimesMove = (m_iMoveWidth > 0 ? m_iMoveWidth : -m_iMoveWidth) / 30;
			if(m_iTimesMove < 20)
				m_iTimesMove = 20;

			SetTimer(TIMER_MOIVE, 30);
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
			Move(eMOVE_START);
			SetCapture();
		}
	}

	void SImageViewer::OnMouseMove(UINT nFlags,CPoint pt)
	{
		__super::OnMouseMove(nFlags,pt);
		BOOL bLPress = (nFlags & MK_LBUTTON);
		if ( m_bImgMovable && bLPress )
		{
			CPoint ptCenter(m_ptCenterOld.x - (LONG)((pt.x - m_ptMoveStart.x) / m_fRatio),
							m_ptCenterOld.y - (LONG)((pt.y - m_ptMoveStart.y) / m_fRatio));
			Move(eMOVE_MOVING, &ptCenter);
		}
	}

	void SImageViewer::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		__super::OnLButtonUp(nFlags,pt);
		if ( m_bImgMovable )
		{
			ReleaseCapture();
			Move(eMOVE_STOP);
		}
	}
		
	BOOL SImageViewer::RealSize()
	{
		m_fRatio = RATIO_100 / 100.f;
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

		// �����Ŵ��ʸ����¼�
		EventRatioChanged evt(this, m_fRatio);
		FireEvent(evt);

		Invalidate();

		return m_fRatio;
	}

	BOOL SImageViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		BOOL bRet	= __super::OnMouseWheel(nFlags, zDelta, pt);
		BOOL bFixed = GetKeyState(VK_CONTROL)&0x8000;	// Ctrl + Wheel
		float fDelta= (zDelta / WHEEL_DELTA) / 100.0f;

		if ( bFixed )
			Zoom(fDelta * 10.0f, TRUE);	// �̶���Delta
		else
			Zoom(fDelta);

		return bRet;
	}

	void SImageViewer::OnTimer(char nIDEvent)
	{
		if ( nIDEvent == TIMER_MOIVE )
		{	// ��ҳ����
			if(m_iMoveWidth > 0)
			{
				if(m_iMoveWidth - m_iTimesMove <= 0)
				{
					m_iMoveWidth = 0;
					Invalidate();
					KillTimer(TIMER_MOIVE);
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
					KillTimer(TIMER_MOIVE);
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
		{	// GIF�������Թ̶�֡��(0.1s)��ʾ
			if ( m_pImgGif == NULL )
			{	// û����ʾGIF�ˣ���ֹͣ
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

	CPoint SImageViewer::Move(E_MoveType eType, LPPOINT ptCenter)
	{
		m_eMove = eType;
		switch( eType )
		{
		case eMOVE_START:
			m_ptCenterOld = m_ptCenter;
			return m_ptCenterOld;
		case eMOVE_MOVING:
			if ( ptCenter && (m_ptCenter.x != ptCenter->x || m_ptCenter.y != ptCenter->y) )
			{
				m_ptCenter.SetPoint(ptCenter->x, ptCenter->y);
				Invalidate();
			}
			return m_ptCenter;
		case eMOVE_STOP:
			m_ptCenterOld= m_ptCenter;
			return m_ptCenterOld;
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

		if ( !m_szTmpImg.IsEmpty() )
			szImage = m_szTmpImg;

		if ( CopyFile(szImage, dlgFile.GetPathName(), FALSE) )
		{
			if ( bOpenFolder )
				OpenFolder(dlgFile.GetPathName());
			return TRUE;
		}

		return FALSE;
	}

#include <Gdiplus.h>
	using namespace Gdiplus;
	inline SStringT SImageViewer::GetTempImgFile(TCHAR* pszExt)
	{
		TCHAR szTmpPath[MAX_PATH * 2] = { 0 };
		TCHAR szTmpFile[MAX_PATH * 2] = { 0 };

		GetTempPath(_countof(szTmpPath), szTmpPath);		// ��ȥ���ʧ��
		GetTempFileName(szTmpPath, _T("iv"), 0, szTmpFile);
		_tcscat(szTmpFile, pszExt);

		return szTmpFile;
	}

	inline BOOL SImageViewer::RemoveTempImage()
	{
		if ( PathFileExists(m_szTmpImg) )
			DeleteFile(m_szTmpImg);	// ɾ���ɵ���ʱ�ļ�

		m_szTmpImg.Empty();
		return TRUE;
	}

	inline SStringT SImageViewer::GetImageFormat(TCHAR* pszExt)
	{
		if ( _tcsicmp(pszExt, _T(".bmp")) == 0 )
			return _T("image/bmp");
		else if ( _tcsicmp(pszExt, _T(".jpg")) == 0 || _tcsicmp(pszExt, _T(".jpeg")) == 0 )
			return _T("image/jpeg");
		else if ( _tcsicmp(pszExt, _T(".gif")) == 0 )
			return _T("image/gif");
		else if ( _tcsicmp(pszExt, _T(".tif")) == 0 || _tcsicmp(pszExt, _T(".tiff")) == 0 )
			return _T("image/tiff");
		else //if ( _tcsicmp(pszExt, _T(".png")) == 0 )
			return _T("image/png");	// Default
	}

	BOOL SImageViewer::Rotate(BOOL bRight)
	{
		if(m_bTimerMove || m_bError )
			return TRUE;		// ������ʾ����Ч��ʱ�����ٺ�ɫ��ת

		m_nAngle += bRight ? 90 : -90;
		SStringT szImg = m_vectImage[m_iSelected];
		if ( TCHAR* pszExt = PathFindExtension(szImg) )
		{
			Image img(szImg);

			 if ( m_nAngle == 90 || m_nAngle == -270 )
				img.RotateFlip(Rotate90FlipNone);
			else if ( m_nAngle == 180 || m_nAngle == -180 )
				img.RotateFlip(Rotate180FlipNone);
			else if ( m_nAngle == 270 || m_nAngle == -90 )
				img.RotateFlip(Rotate270FlipNone);
			else
				m_nAngle = 0;

			CLSID clsidEncoder;
			if (GetEncoderClsid(GetImageFormat(pszExt), &clsidEncoder) < 0)
				return FALSE;
			
			Reset(TRUE, FALSE);
			m_szTmpImg = GetTempImgFile(pszExt);
			if ( Ok == img.Save(m_szTmpImg, &clsidEncoder) )
			{
				LOADIMAGE_(m_szTmpImg, m_pImgSel);
				Invalidate();
				return TRUE;
			}
		}

		return FALSE;
	}

	inline int SImageViewer::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes
		ImageCodecInfo* pImageCodecInfo = NULL;

		GetImageEncodersSize(&num, &size);
		if(size == 0)
			return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if(pImageCodecInfo == NULL)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);
		for(UINT j = 0; j < num; ++j)
		{
			if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}    
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}
}