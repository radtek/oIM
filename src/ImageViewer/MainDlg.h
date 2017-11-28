// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "SImageViewer.h"
#include "C_CommandLine.h"

class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnPrevImage();
	void OnNextImage();

	void OnRealSize();
	void OnFullscreen();
	void OnZoomin();
	void OnZoomout();
	void OnOpenFolder();
	void OnSaveas();
	void OnRotate();

	void OnThumbToggle();
	void OnToolbarToggle();

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BOOL IsOutofScreen();
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	void OnLanguage(int nID);
	void OnTimer(char nIDEvent);
	void OnLButtonDown(UINT nFlags,CPoint pt);
	void OnLButtonUp(UINT nFlags,CPoint pt);
	void OnLButtonDblClick(UINT nFlags,CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnSize(UINT nType, CSize size);
	bool OnRatioChanged( EventArgs *e );
	bool OnImagePosChanged( EventArgs *e );

	void Fullscreen(int i32Type);
	BOOL InitWndSize(const SIZE& sizeImg);
	BOOL SwitchImage(int i, LPSIZE pSize = NULL);	// -1: Next; -2: Prev; >=0: the index

	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(R.name.btn_close, OnClose)
		EVENT_NAME_COMMAND(R.name.btn_prev, OnPrevImage)
		EVENT_NAME_COMMAND(R.name.btn_next, OnNextImage)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_1to1, OnRealSize)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_fullscreen, OnFullscreen)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_zoomin, OnZoomin)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_zoomout, OnZoomout)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_folder, OnOpenFolder)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_saveas, OnSaveas)
		EVENT_NAME_COMMAND(R.name.btn_toolbar_rotate, OnRotate)
		EVENT_NAME_COMMAND(R.name.thumb_toggle, OnThumbToggle)
		EVENT_NAME_COMMAND(R.name.toolbar_toggle, OnToolbarToggle)
		EVENT_NAME_HANDLER(R.name.imageViewer, EVT_RATIO_CHANGED, OnRatioChanged);
		EVENT_NAME_HANDLER(R.name.imageViewer, EVT_IMGPOS_CHANGED, OnImagePosChanged);

	EVENT_MAP_END()

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick);
		MSG_WM_KEYDOWN(OnChar)
		MSG_WM_TIMER_EX(OnTimer)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	SImageViewer*	m_pImageSwitcher;
	BOOL		m_bLayoutInited;
	DWORD		m_dwOutScreen;
	DWORD		m_dwRatioElapse;
	SButton*	m_pPrevImageBtn;
	SButton*	m_pNextImageBtn;
	SStatic*	m_pRatio;
	SButton*	m_p1To1Btn;
	SToggle*	m_pFullscreenTgl;
	SButton*	m_pZoominBtn;
	SButton*	m_pZoomoutBtn;
	SButton*	m_pRotateBtn;

	// 地图
	float		m_fThumbRatio;
	CPoint		m_ptCenterOld;
	CPoint		m_ptPosStart;
	BOOL		m_bPosMoving;
	SWindow*	m_pToolbarView;
	SToggle*	m_pToolbarToggle;
	SToggle*	m_pThumbToggle;
	SWindow*	m_pThumbView;
	SImageWnd*	m_pImgThumb;
	SImageWnd*	m_pImgPos;

	// 窗口移动
	BOOL	m_bWndMoving;
	CRect	m_rtWndStart;
	CPoint	m_ptWndStart;


};
