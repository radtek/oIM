#include "StdAfx.h"
#include "MainWnd.h"
#include "helper/SMenu.h"
#include "../controls.extend/FileHelper.h"

#include<string>
#include<iostream>
#include <ShellAPI.h>

CMainWnd::CMainWnd(void) : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited=FALSE;
}

CMainWnd::~CMainWnd(void)
{
}


BOOL CMainWnd::OnInitDialog( HWND hWnd, LPARAM lParam )
{
	// ����Ƥ��������
	m_dlgSkinMgr.Create(m_hWnd);
	m_dlgSkinMgr.GetNative()->SendMessage(WM_INITDIALOG);

	// ����������Ӵ���
	m_winBox.Create(m_hWnd);
	m_winBox.m_boxParent=(void *)this;
	m_winBox.GetNative()->SendMessage(WM_INITDIALOG);

	menu_sortord.LoadMenu(_T("menu_playlist_sortord"),_T("LAYOUT"));	//����tabҳ1���б�����ʽ�˵�

	menu_icon.LoadMenu(_T("menu_test"),_T("LAYOUT"));					//icon��ť�ĵ����˵�

	menu_PlayArea.LoadMenu(_T("menu_open"),_T("LAYOUT"));				//����������ļ���ť�ĵ����˵�

	menu_PlayMode.LoadMenu(_T("menu_playmode"),_T("LAYOUT"));			//����ģʽ�˵�
	::CheckMenuItem(menu_PlayMode.m_hMenu,11101,MF_CHECKED);
	::CheckMenuItem(menu_PlayMode.m_hMenu,11102,MF_UNCHECKED);
	::CheckMenuItem(menu_PlayMode.m_hMenu,11103,MF_UNCHECKED);
	::CheckMenuItem(menu_PlayMode.m_hMenu,11104,MF_UNCHECKED);
	::CheckMenuItem(menu_PlayMode.m_hMenu,11105,MF_UNCHECKED);

	m_bLayoutInited=TRUE;

	up_or_down = 0;			//����ʽ,���ϻ�����,��ʼ��Ϊ0
	popularity_up_or_down = TRUE;	//����ʽΪ�����ڡ�ʱ�����ϻ�����,��ʼ��ΪTRUE

	return 0;
}

void CMainWnd::OnBtnIcon()	// ���Ͻ�icon��ť
{



	CRect rc_menu;
	SWindow * pBtn = FindChildByName(L"btn_icon");
	if(pBtn) 
	{
		pBtn->GetClientRect(&rc_menu);
		ClientToScreen(&rc_menu);
		menu_icon.TrackPopupMenu(0, rc_menu.left, rc_menu.bottom, m_hWnd);
	}
}

void CMainWnd::OnBtnFeedback()	//�������
{
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), _T("http://www.zhihu.com/"), NULL, SW_SHOW);
}

void CMainWnd::OnBtnSkins()//��Ƥ������
{
	CRect rc_temp;
	SWindow * pBtn = FindChildByName(L"btn_skins");
	if(pBtn) 
	{
		pBtn->GetClientRect(&rc_temp);
		ClientToScreen(&rc_temp);

		m_dlgSkinMgr.SetWindowPos(HWND_TOP, rc_temp.left, rc_temp.bottom,  360, 350, NULL); 
		m_dlgSkinMgr.ShowWindow(SW_SHOWNORMAL);
	}
}

void CMainWnd::OnBtnBgOpen()	//����������ļ���ť
{
	CFileDialogEx openDlg(TRUE,_T("rmvb"),0,6,_T("��Ƶ�ļ�(*.rmvb)\0*.rmvb\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
		SMessageBox(NULL,_T("OnBtnBgOpen"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnBgOpenMenu()	//����������ļ���ť�ĵ����˵�
{
	CRect rc_menu;
	SWindow * pBtn = FindChildByName(L"btn_bg_open");
	if(pBtn) 
	{
		pBtn->GetClientRect(&rc_menu);
		ClientToScreen(&rc_menu);

		menu_PlayArea.TrackPopupMenu(0, rc_menu.left, rc_menu.bottom, m_hWnd);
	}
}
/******************************* ������ ***************************************************/
void CMainWnd::OnBtnTools()		// ������
{
	SWindow * pBtn = FindChildByName(L"win_tools");
	if(pBtn) pBtn->SetVisible(!pBtn->IsVisible(TRUE),TRUE);
}

void CMainWnd::OnBtnLEye()	// ����
{
	SWindow * pBtn = FindChildByName(L"btn_left_eye");
	if(pBtn) pBtn->SetVisible(FALSE);

	pBtn = FindChildByName(L"btn_left_eyed");
	if(pBtn) pBtn->SetVisible(TRUE);
}

void CMainWnd::OnBtnLEyed()	// �ر�����
{
	SWindow * pBtn = FindChildByName(L"btn_left_eye");
	if(pBtn) pBtn->SetVisible(TRUE);

	pBtn = FindChildByName(L"btn_left_eyed");
	if(pBtn) pBtn->SetVisible(FALSE);
}

void CMainWnd::OnBtnStop()	// ֹͣ
{
	SMessageBox(NULL,_T("OnBtnStop"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnPageUp()	//��һ��
{
	SMessageBox(NULL,_T("OnBtnPageUp"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnPlay()	//����
{
	SWindow * pBtn = FindChildByName(L"btn_play");
	if(pBtn) pBtn->SetVisible(FALSE);

	pBtn = FindChildByName(L"btn_pause");
	if(pBtn) pBtn->SetVisible(TRUE);
}

void CMainWnd::OnBtnPause()	//��ͣ
{
	SWindow * pBtn = FindChildByName(L"btn_play");
	if(pBtn) pBtn->SetVisible(TRUE);

	pBtn = FindChildByName(L"btn_pause");
	if(pBtn) pBtn->SetVisible(FALSE);
}

void CMainWnd::OnBtnPageDown()	//��һ��
{
	SMessageBox(NULL,_T("OnBtnPageDown"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnOpen()	//���ļ�
{
	CFileDialogEx openDlg(TRUE,_T("mp4"),0, OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT,_T("��Ƶ�ļ�(*.mp4)\0*.mp4\0All files (*.*)\0*.*\0\0"));
	if(openDlg.DoModal()==IDOK)
		//SMessageBox(NULL,_T("OnBtnOpen"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
	{
		int i32Total = 0;
		SStringT szFiles;
		if ( TCHAR* pszPos = openDlg.GetStartPosition() )
		{
			do{
				szFiles += openDlg.GetNextPathName(pszPos);
				szFiles += _T("\n");
			}while(pszPos);
		}

		printf("%d",i32Total);
		SMessageBox(NULL, szFiles, TEXT("MultiSelect"), MB_OK);
	}
}


void CMainWnd::OnBtnVolume()	//����
{
	SWindow * pBtn = FindChildByName(L"btn_volume_mute");
	if(pBtn) pBtn->SetVisible(TRUE);

	pBtn = FindChildByName(L"btn_volume");
	if(pBtn) pBtn->SetVisible(FALSE);
}

void CMainWnd::OnBtnVolumeQuit()	//�˳�����
{
	SWindow * pBtn = FindChildByName(L"btn_volume_mute");
	if(pBtn) pBtn->SetVisible(FALSE);

	pBtn = FindChildByName(L"btn_volume");
	if(pBtn) pBtn->SetVisible(TRUE);
}

void CMainWnd::OnBtnFullscreen()	//ȫ��
{
	SWindow * pBtn = FindChildByName(L"btn_quit_fullscreen");
	if(pBtn) pBtn->SetVisible(TRUE);

	pBtn = FindChildByName(L"btn_fullscreen");
	if(pBtn) pBtn->SetVisible(FALSE);

	//	SMessageBox(NULL,_T("OnBtnFullscreen"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnQuitFullscreen()	//�ر�ȫ��
{
	SWindow * pBtn = FindChildByName(L"btn_quit_fullscreen");
	if(pBtn) pBtn->SetVisible(FALSE);

	pBtn = FindChildByName(L"btn_fullscreen");
	if(pBtn) pBtn->SetVisible(TRUE);

	//	SMessageBox(NULL,_T("OnBtnQuitFullscreen"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnListShow()	//��ʾ�����б�
{
	SWindow * pBtn = FindChildByName(L"play_list");
	if(pBtn) 
	{
		pBtn->SetVisible(TRUE,TRUE);
	}

	pBtn = FindChildByName(L"btn_list_showed");
	if(pBtn) pBtn->SetVisible(TRUE,TRUE);

	pBtn = FindChildByName(L"btn_list_show");
	if(pBtn) pBtn->SetVisible(FALSE,TRUE);
}

void CMainWnd::OnBtnListShowed()	//���ز����б�
{
	SWindow * pBtn = FindChildByName(L"play_list");
	if(pBtn) 
	{
	pBtn->SetVisible(FALSE,TRUE);
	}


	pBtn = FindChildByName(L"btn_list_showed");
	if(pBtn) pBtn->SetVisible(FALSE,TRUE);

	pBtn = FindChildByName(L"btn_list_show");
	if(pBtn) pBtn->SetVisible(TRUE,TRUE);
}

void CMainWnd::OnBtnBox()	//�򿪱������
{
	//m_winBox.ShowWindow(SW_SHOWNORMAL);

	SWindow	*pBtn = FindChildByName(L"mainWnd");
	CRect rc_temp;
	if(pBtn) 
	{
		pBtn->GetClientRect(&rc_temp);
		ClientToScreen(&rc_temp);

		m_winBox.SetWindowPos(HWND_TOP, rc_temp.right -5, rc_temp.top -3,  250, rc_temp.Height() +10, NULL); 
		m_winBox.ShowWindow(SW_SHOWNORMAL);
	}

	pBtn = FindChildByName(L"btn_boxed");
	if(pBtn) pBtn->SetVisible(TRUE,TRUE);

	pBtn = FindChildByName(L"btn_box");
	if(pBtn) pBtn->SetVisible(FALSE,TRUE);
}

void CMainWnd::OnBtnBoxHide()	//�رձ������
{
	m_winBox.ShowWindow(SW_HIDE);

	SWindow* pBtn = FindChildByName(L"btn_boxed");
	if(pBtn) pBtn->SetVisible(FALSE,TRUE);

	pBtn = FindChildByName(L"btn_box");
	if(pBtn) pBtn->SetVisible(TRUE,TRUE);
}
/******************************* �����䴰�� ********************************************/
void CMainWnd::OnBtnToolsWinClose()		//�رհ�ť
{
	SWindow* pWin = FindChildByName(L"win_tools");
	if(pWin) pWin->SetVisible(FALSE,TRUE);
}

void CMainWnd::OnBtnTool3D()		//3D
{
	SMessageBox(NULL,_T("3D"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolsWinPrev()		//��һ��
{
	SWindow* pWin = FindChildByName(L"tool_page2");
	if(pWin) pWin->SetVisible(FALSE,TRUE);

	pWin = FindChildByName(L"tool_page1");
	if(pWin) pWin->SetVisible(TRUE,TRUE);
}

void CMainWnd::OnBtnToolsWinNext()		//��һ��
{
	SWindow* pWin = FindChildByName(L"tool_page2");
	if(pWin) pWin->SetVisible(TRUE,TRUE);

	pWin = FindChildByName(L"tool_page1");
	if(pWin) pWin->SetVisible(FALSE,TRUE);
}

void CMainWnd::OnBtnToolLefteye()		//����
{
	SMessageBox(NULL,_T("����"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolSurronudsound()		//������
{
	SMessageBox(NULL,_T("������"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolFlyscreen()		//����
{
	SMessageBox(NULL,_T("����"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolGame()			//��Ϸ
{
	SMessageBox(NULL,_T("��Ϸ"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolDownload()		//���ع���
{
	SMessageBox(NULL,_T("���ع���"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolTranscode()		//ת��
{
	SMessageBox(NULL,_T("ת��"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolMovielib()		//Ӱ�ӿ�
{
	SMessageBox(NULL,_T("Ӱ�ӿ�"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolBarrage()		//��Ļ
{
	SMessageBox(NULL,_T("��Ļ"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolDlna()			//dlna
{
	SMessageBox(NULL,_T("dlna"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolNews()			//��Ѷ
{
	SMessageBox(NULL,_T("��Ѷ"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolScreebshots()	//��ͼ
{
	SMessageBox(NULL,_T("��ͼ"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnToolShoot()			//����
{
	SMessageBox(NULL,_T("����"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

/****************************** �����б�tabҳ1 **************************************************/
void CMainWnd::OnBtnPage1SortordMenu()			//�����б�����ʽ
{

	CRect rc_menu;
	SWindow * pBtn = FindChildByName(L"btn_sortord_menu");
	if(pBtn) 
	{
		pBtn->GetClientRect(&rc_menu);
		ClientToScreen(&rc_menu);

		menu_sortord.TrackPopupMenu(0, rc_menu.left - 50, rc_menu.bottom, m_hWnd);
	}
}

void CMainWnd::OnBtnPage1Sortord()			//�����б����������»�����
{

	SWindow *pDown = FindChildByName(L"sortord_down");
	SWindow *pUp = FindChildByName(L"sortord_up");
	if(pDown && pUp) 
	{
		if(!(pDown->IsVisible(FALSE)) && !(pUp->IsVisible(FALSE)))
			up_or_down = 0;
		else if(pDown->IsVisible(FALSE) && !(pUp->IsVisible(FALSE)))
			up_or_down = 1;
		else if(!(pDown->IsVisible(FALSE)) && pUp->IsVisible(FALSE))
			up_or_down = 2;
		else
			up_or_down = -1;
	}

	if(popularity_up_or_down)
	{
		switch(up_or_down)
		{
		case 0:
			pDown->SetVisible(TRUE, TRUE);
			pUp->SetVisible(FALSE, TRUE);
			up_or_down = 1;
			break;
		case 1:
			pDown->SetVisible(FALSE, TRUE);
			pUp->SetVisible(TRUE, TRUE);
			up_or_down = 2;
			break;
		case 2:
			pDown->SetVisible(FALSE, TRUE);
			pUp->SetVisible(FALSE, TRUE);
			up_or_down = 0;
		default:
			break;
		}
	}
	else
		switch(up_or_down)
	{
		case 1:
			pDown->SetVisible(FALSE, TRUE);
			pUp->SetVisible(TRUE, TRUE);
			up_or_down = 2;
			break;
		case 2:
			pDown->SetVisible(TRUE, TRUE);
			pUp->SetVisible(FALSE, TRUE);
			up_or_down = 1;
		default:
			break;
	}
}

/****************************** �����б�tabҳ2 **************************************************/
void CMainWnd::OnBtnAll()					//��ȫ����
{
	SMessageBox(NULL,_T("��ȫ����"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnAdd()					//��+��
{
	SMessageBox(NULL,_T("��+��"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnDelete()				//��-��
{
	SMessageBox(NULL,_T("��-��"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnClear()				//����б�ť
{
	SMessageBox(NULL,_T("����б�ť"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CMainWnd::OnBtnOrderPlay()			//˳�򲥷�
{
	POINT pt;
	GetCursorPos(&pt);
	menu_PlayMode.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
}

void CMainWnd::OnBtnSinglePlay()			//��������
{
	POINT pt;
	GetCursorPos(&pt);
	menu_PlayMode.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
}

void CMainWnd::OnBtnRandomPlay()			//�������
{
	POINT pt;
	GetCursorPos(&pt);
	menu_PlayMode.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
}

void CMainWnd::OnBtnSingleCycle()			//����ѭ��
{
	POINT pt;
	GetCursorPos(&pt);
	menu_PlayMode.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
}

void CMainWnd::OnBtnListCycle()			//�б�ѭ��
{
	POINT pt;
	GetCursorPos(&pt);
	menu_PlayMode.TrackPopupMenu(0,pt.x,pt.y,m_hWnd);
}



//��Ӧ�˵��¼�
void CMainWnd::OnCommand( UINT uNotifyCode, int nID, HWND wndCtl )
{
	if(uNotifyCode==0)
	{
		if(nID==1101)
		{//nID==1101��Ӧmenu_playlist_sortord�˵��ĵ�һ��
			SWindow* pBtn = FindChildByName(L"btn_sortord");
		//	if(pBtn) pBtn->SetWindowText (L"����");

			pBtn = FindChildByName(L"sortord_down");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"sortord_up");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			up_or_down = -1;
			popularity_up_or_down = TRUE;

			::CheckMenuItem(menu_sortord.m_hMenu,1101,MF_CHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1102,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1103,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1104,MF_UNCHECKED);
		}
		else if(nID==1102)
		{//nID==1101��Ӧmenu_playlist_sortord�˵��ĵڶ���
			SWindow* pBtn = FindChildByName(L"btn_sortord");
		//	if(pBtn) pBtn->SetWindowText (L"����");

			pBtn = FindChildByName(L"sortord_down");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"sortord_up");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			up_or_down = 0;
			popularity_up_or_down = FALSE;

			::CheckMenuItem(menu_sortord.m_hMenu,1101,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1102,MF_CHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1103,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1104,MF_UNCHECKED);
		}
		else if(nID==1103)
		{//nID==1103��Ӧmenu_playlist_sortord�˵��ĵ�����
			SWindow* pBtn = FindChildByName(L"btn_sortord");
		//	if(pBtn) pBtn->SetWindowText (L"����");

			pBtn = FindChildByName(L"sortord_down");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"sortord_up");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			up_or_down = 0;
			popularity_up_or_down = FALSE;

			::CheckMenuItem(menu_sortord.m_hMenu,1101,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1102,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1103,MF_CHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1104,MF_UNCHECKED);
		}
		else if(nID==1104)
		{//nID==1104��Ӧmenu_playlist_sortord�˵��ĵ�����
			SWindow* pBtn = FindChildByName(L"btn_sortord");
		//	if(pBtn) pBtn->SetWindowText (L"����");

			pBtn = FindChildByName(L"sortord_down");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"sortord_up");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			up_or_down = 0;
			popularity_up_or_down = FALSE;

			::CheckMenuItem(menu_sortord.m_hMenu,1101,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1102,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1103,MF_UNCHECKED);
			::CheckMenuItem(menu_sortord.m_hMenu,1104,MF_CHECKED);
		}

		else if(nID==11101)
		{//nID==11101��Ӧmenu_PlayMode�˵��ĵ�һ��
			SWindow* pBtn = FindChildByName(L"btn_OrderPlay");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"btn_SinglePlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_RandomPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SingleCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_ListCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);


			::CheckMenuItem(menu_PlayMode.m_hMenu,11101,MF_CHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11102,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11103,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11104,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11105,MF_UNCHECKED);
		}
		else if(nID==11102)
		{//nID==11102��Ӧmenu_PlayMode�˵��ĵڶ���
			SWindow* pBtn = FindChildByName(L"btn_OrderPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SinglePlay");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"btn_RandomPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SingleCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_ListCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);


			::CheckMenuItem(menu_PlayMode.m_hMenu,11101,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11102,MF_CHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11103,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11104,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11105,MF_UNCHECKED);
		}
		else if(nID==11103)
		{//nID==11103��Ӧmenu_PlayMode�˵��ĵ�����
			SWindow* pBtn = FindChildByName(L"btn_OrderPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SinglePlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_RandomPlay");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"btn_SingleCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_ListCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);


			::CheckMenuItem(menu_PlayMode.m_hMenu,11101,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11102,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11103,MF_CHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11104,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11105,MF_UNCHECKED);
		}
		else if(nID==11104)
		{//nID==11104��Ӧmenu_PlayMode�˵��ĵ�����
			SWindow* pBtn = FindChildByName(L"btn_OrderPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SinglePlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_RandomPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SingleCycle");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);

			pBtn = FindChildByName(L"btn_ListCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);


			::CheckMenuItem(menu_PlayMode.m_hMenu,11101,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11102,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11103,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11104,MF_CHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11105,MF_UNCHECKED);
		}
		else if(nID==11105)
		{//nID==11105��Ӧmenu_PlayMode�˵��ĵ�����
			SWindow* pBtn = FindChildByName(L"btn_OrderPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SinglePlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_RandomPlay");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_SingleCycle");
			if(pBtn) pBtn->SetVisible(FALSE,TRUE);

			pBtn = FindChildByName(L"btn_ListCycle");
			if(pBtn) pBtn->SetVisible(TRUE,TRUE);


			::CheckMenuItem(menu_PlayMode.m_hMenu,11101,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11102,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11103,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11104,MF_UNCHECKED);
			::CheckMenuItem(menu_PlayMode.m_hMenu,11105,MF_CHECKED);
		}
	}
}