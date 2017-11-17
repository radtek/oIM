// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif

#define SWITCH_NEXT	(-1)			// Switch to next image
#define SWITCH_PREV (-2)			// Switch to prev image

#define TIMER_RATIO			120		// 放大率隐藏Timer
#define RATIO_SHOW_TIMES	1500	// 放大率显示时长（ms)

CMainDlg::CMainDlg() 
	: SHostWnd(_T("LAYOUT:XML_MAINWND"))
	, m_bLayoutInited(FALSE)
	, m_pImageSwitcher(NULL)
	, m_pPrevImageBtn(NULL)
	, m_pNextImageBtn(NULL)
	, m_pRatio(NULL)
	, m_pThumbView(NULL)
	, m_pImgThumb(NULL)
	, m_pImgPos(NULL)
	, m_dwRatioElapse(0)
	, m_bWndMoving(FALSE)
	, m_fThumbRatio(1.0f)
	, m_bPosMoving(FALSE)
{
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	m_pImageSwitcher= FindChildByName2<SImageViewer>(R.name.imageViewer);	SASSERT(m_pImageSwitcher);
	m_pPrevImageBtn = FindChildByName2<SButton>(R.name.btn_prev);	SASSERT(m_pPrevImageBtn);
	m_pNextImageBtn = FindChildByName2<SButton>(R.name.btn_next);	SASSERT(m_pNextImageBtn);
	m_pRatio        = FindChildByName2<SStatic>(R.name.txt_ratio);	SASSERT(m_pRatio);
	m_pThumbView    = FindChildByName2<SWindow>(R.name.thumb_view);	SASSERT(m_pThumbView);
	m_pImgThumb		= FindChildByName2<SImageWnd>(R.name.img_thumb);SASSERT(m_pImgThumb);
	m_pImgPos		= FindChildByName2<SImageWnd>(R.name.img_pos);	SASSERT(m_pImgPos);

	SDispatchMessage(UM_SETSCALE, args.m_nScale, 0);	

	SIZE szImg     = { 0 };
	SIZE szScreen  = {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	CRect rtWnd    = GetWindowRect();
	CRect rtPos(0, 0, (LONG)(rtWnd.Width() * args.m_nScale/100.f), (LONG)(rtWnd.Height() * args.m_nScale/100.f) );
	m_pRatio->SetVisible(FALSE);
	m_pImageSwitcher->InitImages(args.m_vectImage);
	SwitchImage((int)args.m_u64Fid, &szImg); 
	if ( szImg.cx > rtWnd.Width() )
		rtPos.right = szImg.cx;		// 设置为图片的宽度

	if ( rtPos.right > szScreen.cx )
		rtPos.right = szScreen.cx;	// 不能超过屏幕宽度

	if ( szImg.cy > rtWnd.Height() )
		rtPos.bottom = szImg.cy;	// 设置主图片的高度

	if ( rtPos.bottom > szScreen.cy )
		rtPos.bottom = szScreen.cy;	// 不能超过屏幕的高度

	MoveWindow(&rtPos);				// 调整窗口大小【主程序会再局中】

	return 0;
}

bool CMainDlg::OnRatioChanged( EventArgs *e )
{
	if ( EventRatioChanged* pEvent = sobj_cast<EventRatioChanged>(e) )
	{
		SStringT szRatio;

		szRatio.Format(GETSTRING(R.string.ratio), pEvent->m_fRatio * 100);
		m_pRatio->SetWindowText(szRatio);
		if ( m_dwRatioElapse == 0 )
			SetTimer(TIMER_RATIO, 100);

		m_dwRatioElapse = GetTickCount();
		m_pRatio->SetVisible(TRUE, TRUE);
		return true;
	}

	return false;
}

bool CMainDlg::OnImagePosChanged( EventArgs *e )
{
	if ( EventImagePosChanged* pEvent = sobj_cast<EventImagePosChanged>(e) )
	{
		IBitmap* pImg = (IBitmap*)m_pImgThumb->GetUserData();
		m_pThumbView->SetVisible(pEvent->m_bShow ); 

		if ( pEvent->m_bShow )
		{	// 可见
			if ( pEvent->m_pImg != pImg )
			{
				m_pImgThumb->SetImage(pEvent->m_pImg);				// 设置图片
				m_pImgThumb->SetUserData((ULONG_PTR)pEvent->m_pImg);// 设置UserData

				CRect rtWnd = m_pThumbView->GetClientRect();
				CRect rtDst = m_pImageSwitcher->GetDefaultDest(rtWnd, pEvent->m_pImg->Size(), &m_fThumbRatio);
				m_pImgThumb->Move(rtDst);		
			}

			CRect rtDst = m_pImgThumb->GetClientRect();
			CRect rtPos = pEvent->m_rtImgPos;

			rtPos.left  = (LONG)(rtDst.left + pEvent->m_rtImgPos.left  * m_fThumbRatio);
			rtPos.right = (LONG)(rtDst.left + pEvent->m_rtImgPos.right * m_fThumbRatio);
			rtPos.top   = (LONG)(rtDst.top  + pEvent->m_rtImgPos.top   * m_fThumbRatio);
			rtPos.bottom= (LONG)(rtDst.top  + pEvent->m_rtImgPos.bottom* m_fThumbRatio);
			m_pImgPos->Move(rtPos);
		}
		else if ( pImg )
		{	// 不可见
			m_pImgThumb->SetImage(NULL);	// 去掉图片
			m_pImgThumb->SetUserData(0);	// 去掉UserData
		}
	}

	return true;
}

void CMainDlg::OnLanguage(int nID)
{
	ITranslatorMgr *pTransMgr = SApplication::getSingletonPtr()->GetTranslator();
	SASSERT(pTransMgr);
	bool bCnLang = nID == 1;

	pugi::xml_document xmlLang;
	if (SApplication::getSingletonPtr()->LoadXmlDocment(xmlLang, bCnLang ? _T("lang_cn") : _T("lang_en"), _T("translator")))
	{
		CAutoRefPtr<ITranslator> lang;
		pTransMgr->CreateTranslator(&lang);
		lang->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
		pTransMgr->SetLanguage(lang->name());
		pTransMgr->InstallTranslator(lang);
		SDispatchMessage(UM_SETLANGUAGE,0,0);
	}
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnPrevImage()
{
	SwitchImage(SWITCH_PREV);
}

void CMainDlg::OnNextImage()
{
	SwitchImage(SWITCH_NEXT);
}

BOOL CMainDlg::SwitchImage(int i, LPSIZE pSize)
{
	SStringT szTooltip;
	SStringT szPageN = GETSTRING(R.string.tipPageN);
	size_t nCount = m_pImageSwitcher->GetCount();
	int i32CurSel = m_pImageSwitcher->GetCurSel();

	if ( i == SWITCH_NEXT )
		m_pImageSwitcher->Switch(i32CurSel - 1);
	else if ( i == SWITCH_PREV )
		m_pImageSwitcher->Switch(i32CurSel + 1);
	else
		m_pImageSwitcher->Switch(i, FALSE, pSize);
	
	i32CurSel = m_pImageSwitcher->GetCurSel();
	szTooltip.Format(szPageN, i32CurSel + 1, nCount);
	if ( i32CurSel == 0 )
	{
		m_pPrevImageBtn->SetToolTipText(szTooltip);
		m_pPrevImageBtn->EnableWindow(TRUE);

		m_pNextImageBtn->SetToolTipText(GETSTRING(R.string.tipLast));
		m_pNextImageBtn->EnableWindow(FALSE);
	}
	else if ( i32CurSel == nCount -1 )
	{
		m_pNextImageBtn->SetToolTipText(szTooltip);
		m_pNextImageBtn->EnableWindow(TRUE);

		m_pPrevImageBtn->SetToolTipText(GETSTRING(R.string.tipInRecord));
		m_pPrevImageBtn->EnableWindow(FALSE);
	}
	else
	{
		m_pNextImageBtn->SetToolTipText(szTooltip);
		m_pNextImageBtn->EnableWindow(TRUE);

		m_pPrevImageBtn->SetToolTipText(szTooltip);
		m_pPrevImageBtn->EnableWindow(TRUE);
	}

	return FALSE;
}

void CMainDlg::OnTimer(char nIDEvent)
{
	if ( nIDEvent == TIMER_RATIO )
	{
		if ( GetTickCount() - m_dwRatioElapse > RATIO_SHOW_TIMES )
		{
			m_dwRatioElapse = 0;
			KillTimer(TIMER_RATIO);
			m_pRatio->SetVisible(FALSE, TRUE);
		}
	}
	else
	{
		 SetMsgHandled(FALSE);
	}
}

void CMainDlg::OnLButtonDown(UINT nFlags,CPoint pt)
{
	SetMsgHandled(FALSE);
	if ( m_pImageSwitcher->IsImgMovable() )
	{
		if ( m_pImgPos->GetClientRect().PtInRect(pt) )
		{
			m_ptCenterOld= m_pImageSwitcher->Move(MOVE_POS_START);
			m_ptPosStart = pt;
			m_bPosMoving = TRUE;
		}

		return;
	}

	if ( !IsZoomed() )
	{
		m_bWndMoving = TRUE;
		m_rtWndStart = GetWindowRect();
		GetCursorPos(&m_ptWndStart);	// 不能使用参数pt, 会有扰动
	}
}

void CMainDlg::OnLButtonUp(UINT nFlags,CPoint pt)
{
	SetMsgHandled(FALSE);

	if ( m_pImageSwitcher->IsImgMovable() )
	{
		if ( m_bPosMoving )
			m_pImageSwitcher->Move(MOVE_POS_STOP);
	}

	m_bPosMoving = FALSE;
	m_bWndMoving = FALSE;
}

void CMainDlg::OnLButtonDblClick(UINT nFlags,CPoint pt)
{
	SetMsgHandled(FALSE);

	if ( m_pPrevImageBtn->GetClientRect().PtInRect(pt) ||
		 m_pNextImageBtn->GetClientRect().PtInRect(pt) ||
		 m_pThumbView->GetClientRect().PtInRect(pt) )
		 return;	// 防止在这些控件上双击时关闭

	OnClose();
}

void CMainDlg::OnMouseMove(UINT nFlags, CPoint pt)
{
	SetMsgHandled(FALSE);
	if ( m_pImageSwitcher->IsImgMovable() )
	{
		if ( m_bPosMoving )
		{
			CPoint ptCenter;

			ptCenter.x = (LONG)((pt.x - m_ptPosStart.x) / m_fThumbRatio) + m_ptCenterOld.x;
			ptCenter.y = (LONG)((pt.y - m_ptPosStart.y) / m_fThumbRatio) + m_ptCenterOld.y;

			m_pImageSwitcher->Move(MOVE_POS_MOVING, &ptCenter);
		}

		return;
	}

	if ( m_bWndMoving )
	{
		POINT ptNow;
		CPoint ptDelta;

		GetCursorPos(&ptNow);	// 不能使用参数pt, 会有扰动
		ptDelta.SetPoint(ptNow.x - m_ptWndStart.x, ptNow.y - m_ptWndStart.y);	// 不能使用增量，会放大误差

		MoveWindow(m_rtWndStart.left + ptDelta.x, m_rtWndStart.top + ptDelta.y, m_rtWndStart.Width(), m_rtWndStart.Height());
	}
}