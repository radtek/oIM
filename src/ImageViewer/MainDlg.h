// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "SImageViewer.h"

class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnPrevImage();
	void OnNextImage();
	BOOL SwitchImage(int i);	// -1: Next; -2: Prev; >=0: the index

	void OnBtnMsgBox();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

protected:
	void OnLanguage(int nID);
	void OnTimer(char nIDEvent);
	void OnLButtonDown(UINT nFlags,CPoint pt);
	void OnLButtonUp(UINT nFlags,CPoint pt);
	void OnLButtonDblClick(UINT nFlags,CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint point);

	bool OnRatioChanged( EventArgs *e );
	bool OnImagePosChanged( EventArgs *e );

	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(R.name.btn_close, OnClose)
		EVENT_NAME_COMMAND(R.name.btn_prev, OnPrevImage)
		EVENT_NAME_COMMAND(R.name.btn_next, OnNextImage)
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
		MSG_WM_TIMER_EX(OnTimer)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	SImageViewer*	m_pImageSwitcher;
	BOOL		m_bLayoutInited;
	DWORD		m_dwRatioElapse;
	SButton*	m_pPrevImageBtn;
	SButton*	m_pNextImageBtn;
	SStatic*	m_pRatio;
	
	// 地图
	float		m_fThumbRatio;
	CPoint		m_ptMoveStart;		// 鼠标按下时的坐标位置
	SWindow*	m_pThumbView;
	SImageWnd*	m_pImgThumb;
	SImageWnd*	m_pImgPos;

	// 窗口移动
	BOOL	m_bWndMoving;
	CRect	m_rtWndStart;
	CPoint	m_ptWndStart;


};
