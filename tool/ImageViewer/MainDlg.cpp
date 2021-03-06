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

#define UPDATE_FULLSCREEN_NONE		(-1)	// 不更新状态
#define UPDATE_FULLSCREEN_NOTOGGLE	(0)		// Not toggled
#define UPDATE_FULLSCREEN_TOGGLE	(1)		// Toggled

#define TIMER_RATIO			120		// 放大率隐藏Timer
#define RATIO_SHOW_TIMES	1200	// 放大率显示时长（ms)

CMainDlg::CMainDlg() 
	: SHostWnd(_T("LAYOUT:XML_MAINWND"))
	, m_bLayoutInited(FALSE)
	, m_dwOutScreen(0)
	, m_pImageSwitcher(NULL)
	, m_pPrevImageBtn(NULL)
	, m_pNextImageBtn(NULL)
	, m_pRatio(NULL)
	, m_pToolbarToggle(NULL)
	, m_pToolbarView(NULL)
	, m_pThumbToggle(NULL)
	, m_pThumbView(NULL)
	, m_pImgThumb(NULL)
	, m_pImgPos(NULL)
	, m_p1To1Btn(NULL)
	, m_pFullscreenTgl(NULL)
	, m_pZoominBtn(NULL)
	, m_pZoomoutBtn(NULL)
	, m_pRotateBtn(NULL)
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

BOOL CMainDlg::InitWndSize(const SIZE& szImg)
{
	HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO minfo = {sizeof(MONITORINFO), 0};
	CRect rtWnd    = GetWindowRect();
	CRect rtPos(0, 0, (LONG)(rtWnd.Width() * args.GetScale()/100.f), (LONG)(rtWnd.Height() * args.GetScale()/100.f) );
	
	SDispatchMessage(UM_SETSCALE, args.GetScale(), 0);	
	::GetMonitorInfo(hMonitor, &minfo);
	if ( szImg.cx > rtWnd.Width() )
		rtPos.right = szImg.cx;		// 设置为图片的宽度

	if ( rtPos.left < minfo.rcWork.left )
	{
		rtPos.left = minfo.rcWork.left;
		m_dwOutScreen++;
	}

	if ( rtPos.right > minfo.rcWork.right )
	{
		rtPos.right = minfo.rcWork.right;	// 不能超过屏幕宽度
		m_dwOutScreen++;
	}

	/////////////////////////////////
	if ( szImg.cy > rtWnd.Height() )
		rtPos.bottom = szImg.cy;	// 设置主图片的高度

	if ( rtPos.top < minfo.rcWork.top ) 
	{
		rtPos.top = minfo.rcWork.top;
		m_dwOutScreen++;
	}

	if ( rtPos.bottom > minfo.rcWork.bottom )
	{
		rtPos.bottom = minfo.rcWork.bottom;	// 不能超过屏幕的高度
		m_dwOutScreen++;
	}

	MoveWindow(&rtPos);				// 调整窗口大小【主程序会再局中】

	return TRUE;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	m_pImageSwitcher= FindChildByName2<SImageViewer>(R.name.imageViewer);	SASSERT(m_pImageSwitcher);
	m_pPrevImageBtn = FindChildByName2<SButton>(R.name.btn_prev);	SASSERT(m_pPrevImageBtn);
	m_pNextImageBtn = FindChildByName2<SButton>(R.name.btn_next);	SASSERT(m_pNextImageBtn);
	m_pRatio        = FindChildByName2<SStatic>(R.name.txt_ratio);	SASSERT(m_pRatio);
	m_pToolbarView  = FindChildByName2<SWindow>(R.name.toolbar_view);SASSERT(m_pToolbarView);
	m_pThumbView    = FindChildByName2<SWindow>(R.name.thumb_view);	SASSERT(m_pThumbView);
	m_pImgThumb		= FindChildByName2<SImageWnd>(R.name.img_thumb);SASSERT(m_pImgThumb);
	m_pImgPos		= FindChildByName2<SImageWnd>(R.name.img_pos);	SASSERT(m_pImgPos);
	m_pToolbarToggle= FindChildByName2<SToggle>(R.name.toolbar_toggle);		SASSERT(m_pToolbarToggle);
	m_pThumbToggle	= FindChildByName2<SToggle>(R.name.thumb_toggle);		SASSERT(m_pThumbToggle);
	m_p1To1Btn		= FindChildByName2<SButton>(R.name.btn_toolbar_1to1);	SASSERT(m_p1To1Btn);
	m_pFullscreenTgl= FindChildByName2<SToggle>(R.name.btn_toolbar_fullscreen);	SASSERT(m_pFullscreenTgl);
	m_pZoominBtn	= FindChildByName2<SButton>(R.name.btn_toolbar_zoomin);		SASSERT(m_pZoominBtn);
	m_pZoomoutBtn	= FindChildByName2<SButton>(R.name.btn_toolbar_zoomout);	SASSERT(m_pZoomoutBtn);
	m_pRotateBtn	= FindChildByName2<SButton>(R.name.btn_toolbar_rotate);		SASSERT(m_pRotateBtn);

	SIZE szImg     = { 0 };
	m_pRatio->SetVisible(FALSE);
	m_pImageSwitcher->InitImages(args.m_vectImage);
	SwitchImage((int)args.m_u64Fid, &szImg); 
	InitWndSize(szImg);
	ImmDisableIME(0);		// 禁止IME

	return 0;
}

bool CMainDlg::OnRatioChanged( EventArgs *e )
{
	if ( EventRatioChanged* pEvent = sobj_cast<EventRatioChanged>(e) )
	{
		SStringT szRatio;
		int i32Ratio = (int)((pEvent->m_fRatio + 0.009f) * 100);	// floor to 0.01(is 1%)
		szRatio.Format(GETSTRING(R.string.ratio), i32Ratio);
		m_pRatio->SetWindowText(szRatio);
		if ( m_dwRatioElapse == 0 )
			SetTimer(TIMER_RATIO, 100);

		m_dwRatioElapse = GetTickCount();
		m_pRatio->SetVisible(TRUE, TRUE);

		m_p1To1Btn->EnableWindow( i32Ratio != 100);
		m_p1To1Btn->SetToolTipText(i32Ratio == 100 ? GETSTRING(R.string.btnToolbarIs1to1) : GETSTRING(R.string.btnToolbar1to1));

		m_pZoominBtn->EnableWindow(i32Ratio < RATIO_MAX);
		m_pZoomoutBtn->EnableWindow(i32Ratio > RATIO_MIN);

		return true;
	}

	return false;
}

bool CMainDlg::OnImagePosChanged( EventArgs *e )
{
	if ( EventImagePosChanged* pEvent = sobj_cast<EventImagePosChanged>(e) )
	{	// 因为 skin 是CAutoRefPtr<IBitmap>, 它会自动管理引用计数与对象的Release()
		IBitmap* pImg = (IBitmap*)m_pImgThumb->GetUserData();
		BOOL bShow = pEvent->m_bShow && !m_pThumbToggle->GetToggle();
		m_pThumbView->SetVisible(bShow, !pEvent->m_bGif);	// 显示GIF时，不能刷新，会导致Layer窗口刷新问题，就没有动画了。
		m_pThumbToggle->SetVisible(pEvent->m_bShow, !pEvent->m_bGif);
		m_pRotateBtn->EnableWindow(!pEvent->m_bGif && !pEvent->m_bError, !pEvent->m_bGif);
		if ( pEvent->m_bShow )
		{	// 可见
			if ( pEvent->m_pImg != pImg )
			{
				m_pImgThumb->SetImage(pEvent->m_pImg);				// 设置图片
				m_pImgThumb->SetUserData((ULONG_PTR)pEvent->m_pImg);// 设置UserData
			}

			CRect rtWnd = m_pThumbView->GetClientRect();
			CRect rtDst = m_pImageSwitcher->GetDefaultDest(rtWnd, pEvent->m_pImg->Size(), &m_fThumbRatio);
			CRect rtPos = pEvent->m_rtImgPos;

			rtPos.left  = (LONG)(rtDst.left + pEvent->m_rtImgPos.left  * m_fThumbRatio);
			rtPos.right = (LONG)(rtDst.left + pEvent->m_rtImgPos.right * m_fThumbRatio);
			rtPos.top   = (LONG)(rtDst.top  + pEvent->m_rtImgPos.top   * m_fThumbRatio);
			rtPos.bottom= (LONG)(rtDst.top  + pEvent->m_rtImgPos.bottom* m_fThumbRatio);
			
			if ( rtPos.right - rtPos.left < 1 )
				rtPos.right = rtPos.left + 1;	// 保证最小宽度1

			if ( rtPos.bottom - rtPos.top < 1 )
				rtPos.bottom = rtPos.top + 1;	// 保证最小高度1

			m_pImgThumb->Move(rtDst);		
			m_pImgPos->Move(rtPos);
		}
		else if ( pImg )
		{	// 不可见
			m_pImgThumb->SetImage(NULL);	// 去掉图片，原引用的图片会自动Release()
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
	int i32Count  = (int)m_pImageSwitcher->GetCount();
	int i32CurSel = m_pImageSwitcher->GetCurSel();

	if ( i == SWITCH_NEXT )
		m_pImageSwitcher->Switch(i32CurSel - 1, args.m_i32Speed);
	else if ( i == SWITCH_PREV )
		m_pImageSwitcher->Switch(i32CurSel + 1, args.m_i32Speed);
	else
		m_pImageSwitcher->Switch(i, 0, pSize);
	
	i32CurSel = m_pImageSwitcher->GetCurSel();
	szTooltip.Format(szPageN, i32CurSel + 1, i32Count);

	if ( i32Count <= 1 )
	{
		m_pPrevImageBtn->SetToolTipText(szTooltip);
		m_pPrevImageBtn->EnableWindow(FALSE);

		m_pNextImageBtn->SetToolTipText(GETSTRING(R.string.tipLast));
		m_pNextImageBtn->EnableWindow(FALSE);
	}
	else if ( i32CurSel == 0 )
	{
		m_pPrevImageBtn->SetToolTipText(szTooltip);
		m_pPrevImageBtn->EnableWindow(TRUE);

		m_pNextImageBtn->SetToolTipText(GETSTRING(R.string.tipLast));
		m_pNextImageBtn->EnableWindow(FALSE);
	}
	else if ( i32CurSel == i32Count -1 )
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
			CSimpleWnd::SetCapture();
			SetMsgHandled(TRUE);
			m_bPosMoving = TRUE;
			m_ptPosStart = pt;
			m_ptCenterOld= m_pImageSwitcher->Move(eMOVE_START);
		}
	}
	else if ( !IsZoomed() )
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
		{
			ReleaseCapture();
			m_pImageSwitcher->Move(eMOVE_STOP);
			SetMsgHandled(TRUE);
		}
	}

	m_bPosMoving = FALSE;
	m_bWndMoving = FALSE;
}

void CMainDlg::OnLButtonDblClick(UINT nFlags,CPoint pt)
{
	SetMsgHandled(FALSE);
	if ( IsZoomed() ||	// 最大化时，禁止双击退出
		 m_pToolbarView->IsVisible() && m_pToolbarView->GetClientRect().PtInRect(pt) ||
		 m_pThumbView->IsVisible() && m_pThumbView->GetClientRect().PtInRect(pt) )
		return;

	if ( m_pPrevImageBtn->GetClientRect().PtInRect(pt) ||
		 m_pNextImageBtn->GetClientRect().PtInRect(pt) ||
		 m_pToolbarToggle->GetClientRect().PtInRect(pt) ||
		 m_pThumbToggle->GetClientRect().PtInRect(pt) )
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
			SetMsgHandled(TRUE);
			CPoint ptCenter((LONG)((pt.x - m_ptPosStart.x) / m_fThumbRatio) + m_ptCenterOld.x,
							(LONG)((pt.y - m_ptPosStart.y) / m_fThumbRatio) + m_ptCenterOld.y);
			m_pImageSwitcher->Move(eMOVE_MOVING, &ptCenter);
		}
	}
	else if ( m_bWndMoving )
	{
		POINT ptNow;
		CPoint ptDelta;

		GetCursorPos(&ptNow);	// 不能使用参数pt, 会有扰动
		ptDelta.SetPoint(ptNow.x - m_ptWndStart.x, ptNow.y - m_ptWndStart.y);	// 不能使用增量，会放大误差

		MoveWindow(m_rtWndStart.left + ptDelta.x, m_rtWndStart.top + ptDelta.y, m_rtWndStart.Width(), m_rtWndStart.Height());
	}
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;

	if (nType == SIZE_MAXIMIZED)
	{
		m_pFullscreenTgl->SetToolTipText(GETSTRING(R.string.btnToolbarExitFullscreen));
	}
	else if (nType == SIZE_RESTORED)
	{
		m_pFullscreenTgl->SetToolTipText(GETSTRING(R.string.btnToolbarFullscreen));
	}
}

void CMainDlg::OnRealSize()
{
	m_pImageSwitcher->RealSize();
}

void CMainDlg::Fullscreen(int i32Type)
{
	if ( i32Type != UPDATE_FULLSCREEN_NONE )
		m_pFullscreenTgl->SetToggle(i32Type);

	if ( m_pFullscreenTgl->GetToggle() )
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
	else
		SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}

void CMainDlg::OnFullscreen()
{
	Fullscreen(UPDATE_FULLSCREEN_NONE);
}

BOOL CMainDlg::IsOutofScreen()
{
	if ( m_dwOutScreen >= 2 )
	{
		Fullscreen(UPDATE_FULLSCREEN_TOGGLE);
		return TRUE;
	}

	return FALSE;
}

void CMainDlg::OnZoomin()
{
	m_pImageSwitcher->Zoom(ZOOM_DELTA);
}

void CMainDlg::OnZoomout()
{
	m_pImageSwitcher->Zoom(-ZOOM_DELTA);
}

void CMainDlg::OnOpenFolder()
{
	m_pImageSwitcher->OpenFolder();
}

void CMainDlg::OnSaveas()
{
	m_pImageSwitcher->Saveas();
}

void CMainDlg::OnRotate()
{
	BOOL bCtrl = GetKeyState(VK_CONTROL)&0x8000;
	BOOL bRet  = m_pImageSwitcher->Rotate(bCtrl == FALSE);
	if ( bRet == FALSE )
		m_pRotateBtn->EnableWindow(FALSE, TRUE);	// 旋转失败，禁用
}

void CMainDlg::OnThumbToggle()
{
	if ( m_pThumbToggle->GetToggle() )
		return m_pThumbView->SetVisible(FALSE, TRUE);

	m_pThumbView->SetVisible(TRUE, TRUE);
}

void CMainDlg::OnToolbarToggle()
{
	if ( m_pToolbarToggle->GetToggle() )
		return m_pToolbarView->SetVisible(FALSE, TRUE);

	m_pToolbarView->SetVisible(TRUE, TRUE);
}

void CMainDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ADD || nChar == VK_OEM_PLUS)
		return OnZoomin();
	else if(nChar == VK_SUBTRACT || nChar == VK_OEM_MINUS)
		return OnZoomout();
	else if (nChar >= VK_LEFT && nChar <= VK_DOWN && m_pImageSwitcher->IsImgMovable() )
	{
		CPoint pt(0, 0);
		if ( nChar == VK_LEFT )
			pt.x -= 5;
		else if ( nChar == VK_RIGHT )
			pt.x += 5;
		else if ( nChar == VK_UP )
			pt.y -= 5;
		else if ( nChar == VK_DOWN )
			pt.y += 5;

		m_ptCenterOld = m_pImageSwitcher->Move(eMOVE_START);
		CPoint ptCenter((LONG)(pt.x / m_fThumbRatio) + m_ptCenterOld.x, (LONG)(pt.y / m_fThumbRatio) + m_ptCenterOld.y);
		m_pImageSwitcher->Move(eMOVE_MOVING, &ptCenter);
		m_pImageSwitcher->Move(eMOVE_STOP);

		return;
	}
	else if ( nChar == VK_HOME )
		SwitchImage(m_pImageSwitcher->GetCount() - 1, FALSE);
	else if ( nChar == VK_END )
		SwitchImage(0, FALSE);
	else if ( nChar == VK_F11 )
	{	// 这儿要先切换状态
		return Fullscreen(!m_pFullscreenTgl->GetToggle());
	}
	else if ( nChar == VK_ESCAPE )
		return OnClose();

	SetMsgHandled(FALSE);
}

