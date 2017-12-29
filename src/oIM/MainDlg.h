// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CMainDlg : public SHostWnd
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnHeadClick();
	void OnSize(UINT nType, CSize size);

	void OnBtnMsgBox();
	//托盘通知消息处理函数
	LRESULT OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL/* bHandled*/);
	//演示如何响应菜单事件
	void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);



protected:
	void OnLanguage(int nID);
	void OnLanguageBtnCN();
	void OnLanguageBtnEN();
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(R.name.btn_close, OnClose)
		EVENT_NAME_COMMAND(R.name.btn_min, OnMinimize)
		EVENT_NAME_COMMAND(R.name.btn_max, OnMaximize)
		EVENT_NAME_COMMAND(R.name.btn_restore, OnRestore)
		EVENT_NAME_COMMAND(R.name.mainbar_caption, OnHeadClick);
	//	EVENT_NAME_COMMAND(L"cn", OnLanguageBtnCN)
	//	EVENT_NAME_COMMAND(L"en", OnLanguageBtnEN)
	EVENT_MAP_END()

		//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
	//	MSG_WM_LBUTTONDOWN(OnLButtonUp)
		//托盘消息处理
		MESSAGE_HANDLER(WM_ICONNOTIFY, OnIconNotify)
		MSG_WM_COMMAND(OnCommand)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	BOOL			m_bLayoutInited;
	SShellNotifyIcon shellNotifyIcon;
};
