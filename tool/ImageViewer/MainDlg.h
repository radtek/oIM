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

	void OnClose();			// 退出
	void OnPrevImage();		// 上一张图片
	void OnNextImage();		// 下一张图片（第0张对应容器的Index 0）

	void OnRealSize();		// 显示实际图片大小
	void OnFullscreen();	// 全屏/退出全屏
	void OnZoomin();		// 放大
	void OnZoomout();		// 缩小
	void OnOpenFolder();	// 打开图片所在的文件夹
	void OnSaveas();		// 图片另存为
	void OnRotate();		// 旋转

	void OnThumbToggle();	// 显示/隐藏 地图
	void OnToolbarToggle();	// 显示/隐藏 工具栏

	BOOL IsOutofScreen();	// 当超过屏幕的长宽时，最大化

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	void OnLanguage(int nID);
	void OnTimer(char nIDEvent);
	void OnLButtonDown(UINT nFlags,CPoint pt);
	void OnLButtonUp(UINT nFlags,CPoint pt);
	void OnLButtonDblClick(UINT nFlags,CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnSize(UINT nType, CSize size);
	bool OnRatioChanged( EventArgs *e );			// 自定义放大率改变事件
	bool OnImagePosChanged( EventArgs *e );			// 自定义图片位置改变事件

	void Fullscreen(int i32Type);					// i32Type 参考 UPDATE_FULLSCREEN_*
	BOOL InitWndSize(const SIZE& sizeImg);			// 初始化窗口大小
	BOOL SwitchImage(int i, LPSIZE pSize = NULL);	// i 参考 SWITCH_*; >=0: the index

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
	SImageViewer*	m_pImageSwitcher;	// 图片预览控件
	BOOL		m_bLayoutInited;		// Layout 是否已经初始化
	DWORD		m_dwOutScreen;			// 初始化显示的图片，是否超过了屏幕长宽
	DWORD		m_dwRatioElapse;		// 放大率控件隐藏的时长
	SButton*	m_pPrevImageBtn;		// 【下一张】按钮
	SButton*	m_pNextImageBtn;		// 【上一张】按钮
	SStatic*	m_pRatio;				// 放大率显示控件
	SButton*	m_p1To1Btn;				// 1:1显示 按钮
	SToggle*	m_pFullscreenTgl;		// 全屏/退出合并控件
	SButton*	m_pZoominBtn;			// 【放大】按钮
	SButton*	m_pZoomoutBtn;			// 【缩小】按钮
	SButton*	m_pRotateBtn;			// 【旋转】按钮

	// 地图
	float		m_fThumbRatio;			// 地图显示的放大率
	CPoint		m_ptCenterOld;			// 旧的中心点
	CPoint		m_ptPosStart;			// 图片移动时的起始位置
	BOOL		m_bPosMoving;			// 是否在移动图片
	SWindow*	m_pToolbarView;			// 工具栏
	SToggle*	m_pToolbarToggle;		// 工具栏 显示/隐藏控件
	SToggle*	m_pThumbToggle;			// 地图 显示/隐藏控件
	SWindow*	m_pThumbView;			// 地图
	SImageWnd*	m_pImgThumb;			// 地图 的图片显示控件
	SImageWnd*	m_pImgPos;				// 地图 的图片位置控件

	// 窗口移动
	BOOL	m_bWndMoving;				// 窗口在移动
	CRect	m_rtWndStart;				// 窗口移动的起始位置
	CPoint	m_ptWndStart;				// 窗口移动的起始鼠标位置


};
