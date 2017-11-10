#include "StdAfx.h"
#include "WinBox.h"
#include "MainWnd.h"

CWinBox::CWinBox(void) : SHostWnd(_T("LAYOUT:XML_WINBOX"))
{
	m_bLayoutInited=FALSE;
	m_boxParent=NULL;
}

CWinBox::~CWinBox(void)
{
	m_boxParent=NULL;
}

BOOL CWinBox::OnInitDialog( HWND hWnd, LPARAM lParam )
{
	SetMsgHandled(FALSE);
	m_bLayoutInited=TRUE;

	

	return 0;
}

void CWinBox::OnClose()
{
	ShowWindow(SW_HIDE);
	CMainWnd* pTemp=(CMainWnd *)m_boxParent;
	pTemp->OnBtnBoxHide();
	pTemp=NULL;
}

void CWinBox::OnBtnBack()			//����
{
	SMessageBox(NULL,_T("����"),_T("������"),MB_OK|MB_ICONEXCLAMATION);
}

void CWinBox::OnBtnForward()		//ǰ��
{
	SMessageBox(NULL,_T("ǰ��"),_T("������"),MB_OK|MB_ICONEXCLAMATION);
}

void CWinBox::OnBtnRefresh()		//ˢ��
{
	SMessageBox(NULL,_T("ˢ��"),_T("������"),MB_OK|MB_ICONEXCLAMATION);
}

void CWinBox::OnBtnMaximize()		//���
{
	RECT rt;
	::GetWindowRect(this->m_hWnd, &rt);
	::GetWindowRect(this->m_hWnd, &rect_box);

	
	int cx=GetSystemMetrics(SM_CYSCREEN);
	int offset_cx=GetSystemMetrics(SM_CYCAPTION);
	rt.top = 0;
	rt.bottom = cx - offset_cx;
	this->MoveWindow(&rt,TRUE);
	
	SWindow* pBtn = FindChildByName(L"btn_box_maximize");
	if(pBtn) pBtn->SetVisible(FALSE,TRUE);

	pBtn = FindChildByName(L"btn_box_restore");
	if(pBtn) pBtn->SetVisible(TRUE,TRUE);
}

void CWinBox::OnBtnRestore()		//��ԭ
{
	//SendMessage(WM_SYSCOMMAND,SC_RESTORE);
	this->MoveWindow(&rect_box,TRUE);

	SWindow* pBtn = FindChildByName(L"btn_box_maximize");
	if(pBtn) pBtn->SetVisible(TRUE,TRUE);

	pBtn = FindChildByName(L"btn_box_restore");
	if(pBtn) pBtn->SetVisible(FALSE,TRUE);
}
