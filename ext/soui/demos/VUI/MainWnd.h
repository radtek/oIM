#pragma once
#include "SkinMgr.h"
#include "WinBox.h"
#include "helper/SMenu.h"

class CMainWnd : public SHostWnd
{
public:
	CMainWnd();
	~CMainWnd();

	void OnClose()
	{
		PostMessage(WM_QUIT);
	}
	void OnMaximize()
	{
		SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE);
	}
	void OnRestore()
	{
		SendMessage(WM_SYSCOMMAND,SC_RESTORE);
	}
	void OnMinimize()
	{
		SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
	}

	void OnSize(UINT nType, CSize size)
	{
		SetMsgHandled(FALSE);
		if(!m_bLayoutInited) return;
		if(nType==SIZE_MAXIMIZED)
		{
			FindChildByName(L"btn_restore")->SetVisible(TRUE);
			FindChildByName(L"btn_max")->SetVisible(FALSE);
		}else if(nType==SIZE_RESTORED)
		{
			FindChildByName(L"btn_restore")->SetVisible(FALSE);
			FindChildByName(L"btn_max")->SetVisible(TRUE);
		}
	}

	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	//��ť�ؼ�����Ӧ
	void OnBtnIcon();				//���Ͻ�icon��ť
	void OnBtnFeedback();			//�������
	void OnBtnSkins();				//������ť	
	void OnBtnBgOpen();				//����������ļ�
	void OnBtnBgOpenMenu();			//��������򿪲˵�
	void OnBtnTools();				//������
	void OnBtnLEye();				//����
	void OnBtnLEyed();				//�ر�����
	void OnBtnStop();				//ֹͣ
	void OnBtnPageUp();				//��һ��
	void OnBtnPlay();				//����
	void OnBtnPause();				//��ͣ
	void OnBtnPageDown();			//��һ��
	void OnBtnOpen();				//���ļ�
	void OnBtnVolume();				//����
	void OnBtnVolumeQuit();				//����
	void OnBtnFullscreen();			//ȫ��
	void OnBtnQuitFullscreen();		//�ر�ȫ��
	void OnBtnListShow();			//��ʾ�����б�
	void OnBtnListShowed();			//���ز����б�
	void OnBtnBox();				//�򿪱������
	void OnBtnBoxHide();				//�رձ������

	void OnBtnToolsWinClose();		//�����䴰�ڵĹرհ�ť
	void OnBtnToolsWinPrev();		//��һ��
	void OnBtnToolsWinNext();		//��һ��
	void OnBtnTool3D();				//3D
	void OnBtnToolLefteye();		//����
	void OnBtnToolSurronudsound();	//������
	void OnBtnToolFlyscreen();		//����
	void OnBtnToolGame();			//��Ϸ
	void OnBtnToolDownload();		//���ع���
	void OnBtnToolTranscode();		//ת��
	void OnBtnToolMovielib();		//Ӱ�ӿ�
	void OnBtnToolBarrage();		//��Ļ
	void OnBtnToolDlna();			//dlna
	void OnBtnToolNews();			//��Ѷ
	void OnBtnToolScreebshots();	//��ͼ
	void OnBtnToolShoot();			//����

	void OnBtnPage1SortordMenu();	//�����б�����ʽ
	void OnBtnPage1Sortord();	//�����б����������»�����

	//�����б�tabҳ2
	void OnBtnAll()	;				//��ȫ����
	void OnBtnAdd()	;				//��+��
	void OnBtnDelete()	;			//��-��
	void OnBtnClear()	;			//����б�ť
	void OnBtnOrderPlay();			//˳�򲥷�
	void OnBtnSinglePlay()	;		//��������
	void OnBtnRandomPlay()	;		//�������
	void OnBtnSingleCycle()	;		//����ѭ��
	void OnBtnListCycle()	;		//�б�ѭ��

	//DUI�˵���Ӧ����
	void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);

	SMenu menu_sortord;				//tabҳ1���б�����ʽ�˵�
	SMenu menu_icon;				//icon��ť�ĵ����˵�
	SMenu menu_PlayArea;			//����������ļ���ť�ĵ����˵�
	SMenu menu_PlayMode;			//����ģʽ�˵�

	int up_or_down;					//����ʽ���ϻ�����
	BOOL popularity_up_or_down;		//����ʽΪ�����ڡ�ʱ�����ϻ�����

protected:
	//��ť�¼�����ӳ���
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close",OnClose)
		EVENT_NAME_COMMAND(L"btn_min",OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max",OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore",OnRestore)

		EVENT_NAME_COMMAND(L"btn_icon",OnBtnIcon)
		EVENT_NAME_COMMAND(L"btn_skins",OnBtnSkins)
		EVENT_NAME_COMMAND(L"btn_feedback",OnBtnFeedback)
		EVENT_NAME_COMMAND(L"btn_bg_open",OnBtnBgOpen)
		EVENT_NAME_COMMAND(L"btn_bg_openmenu",OnBtnBgOpenMenu)
		EVENT_NAME_COMMAND(L"btn_tools",OnBtnTools)
		EVENT_NAME_COMMAND(L"btn_left_eye",OnBtnLEye)
		EVENT_NAME_COMMAND(L"btn_left_eyed",OnBtnLEyed)
		EVENT_NAME_COMMAND(L"btn_stop",OnBtnStop)
		EVENT_NAME_COMMAND(L"btn_page_up",OnBtnPageUp)
		EVENT_NAME_COMMAND(L"btn_play",OnBtnPlay)
		EVENT_NAME_COMMAND(L"btn_pause",OnBtnPause)
		EVENT_NAME_COMMAND(L"btn_page_down",OnBtnPageDown)
		EVENT_NAME_COMMAND(L"btn_open",OnBtnOpen)
		EVENT_NAME_COMMAND(L"btn_volume",OnBtnVolume)
		EVENT_NAME_COMMAND(L"btn_volume_mute",OnBtnVolumeQuit)
		EVENT_NAME_COMMAND(L"btn_fullscreen",OnBtnFullscreen)
		EVENT_NAME_COMMAND(L"btn_quit_fullscreen",OnBtnQuitFullscreen)
		EVENT_NAME_COMMAND(L"btn_list_show",OnBtnListShow)
		EVENT_NAME_COMMAND(L"btn_list_showed",OnBtnListShowed)
		EVENT_NAME_COMMAND(L"btn_box",OnBtnBox)
		EVENT_NAME_COMMAND(L"btn_boxed",OnBtnBoxHide)

		EVENT_NAME_COMMAND(L"btn_tools_close",OnBtnToolsWinClose)
		EVENT_NAME_COMMAND(L"btn_tool_prev",OnBtnToolsWinPrev)
		EVENT_NAME_COMMAND(L"btn_tool_next",OnBtnToolsWinNext)
		EVENT_NAME_COMMAND(L"btn_tool_3D",OnBtnTool3D)
		EVENT_NAME_COMMAND(L"btn_tool_lefteye",OnBtnToolLefteye)
		EVENT_NAME_COMMAND(L"btn_tool_surround_sound",OnBtnToolSurronudsound)
		EVENT_NAME_COMMAND(L"btn_tool_flyscreen",OnBtnToolFlyscreen)
		EVENT_NAME_COMMAND(L"btn_tool_game",OnBtnToolGame)
		EVENT_NAME_COMMAND(L"btn_tool_download",OnBtnToolDownload)
		EVENT_NAME_COMMAND(L"btn_tool_transcode",OnBtnToolTranscode)
		EVENT_NAME_COMMAND(L"btn_tool_movielib",OnBtnToolMovielib)
		EVENT_NAME_COMMAND(L"btn_tool_barrage",OnBtnToolBarrage)
		EVENT_NAME_COMMAND(L"btn_tool_dlna",OnBtnToolDlna)
		EVENT_NAME_COMMAND(L"btn_tool_news",OnBtnToolNews)
		EVENT_NAME_COMMAND(L"btn_tool_screenshots",OnBtnToolScreebshots)
		EVENT_NAME_COMMAND(L"btn_tool_shoot",OnBtnToolShoot)

		EVENT_NAME_COMMAND(L"btn_sortord_menu",OnBtnPage1SortordMenu)
		EVENT_NAME_COMMAND(L"btn_sortord",OnBtnPage1Sortord)

		EVENT_NAME_COMMAND(L"btn_all",OnBtnAll)
		EVENT_NAME_COMMAND(L"btn_add",OnBtnAdd)
		EVENT_NAME_COMMAND(L"btn_delete",OnBtnDelete)
		EVENT_NAME_COMMAND(L"btn_clear",OnBtnClear)
		EVENT_NAME_COMMAND(L"btn_OrderPlay",OnBtnOrderPlay)
		EVENT_NAME_COMMAND(L"btn_SinglePlay",OnBtnSinglePlay)
		EVENT_NAME_COMMAND(L"btn_RandomPlay",OnBtnRandomPlay)
		EVENT_NAME_COMMAND(L"btn_SingleCycle",OnBtnSingleCycle)
		EVENT_NAME_COMMAND(L"btn_ListCycle",OnBtnListCycle)
		EVENT_MAP_END()    

		//������Ϣ����ӳ���
		BEGIN_MSG_MAP_EX(CMainWnd)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_COMMAND(OnCommand)
		CHAIN_MSG_MAP(SHostWnd)//ע�⽫û�д������Ϣ�������ദ��
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	BOOL            m_bLayoutInited;
	CSkinMgr		m_dlgSkinMgr;		//Ƥ��������
	CWinBox			m_winBox;				//�������
};