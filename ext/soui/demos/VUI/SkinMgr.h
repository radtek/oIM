#pragma once

class CSkinMgr : public SHostWnd
{
public:
	CSkinMgr(void);
	~CSkinMgr(void);

	void OnClose()
	{
		ShowWindow(SW_HIDE);
	}

	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	void OnBtnMaxspeed();			//����
	void OnBtnDeepblue();			//�����
	void OnBtnSelfdefine();			//�Զ���
	void OnBtnBigbang();			//��Ƭ�籩
	void OnBtnPrev();				//��һ��Ƥ��
	void OnBtnChoosing1();			//���е�Ƥ��1
	void OnBtnChoosing2();			//���е�Ƥ��2
	void OnBtnNext();				//��һ��Ƥ��
	void OnBtnColor1();				//��ɫ1
	void OnBtnColor2();				//��ɫ2
	void OnBtnColor3();				//��ɫ3
	void OnBtnColor4();				//��ɫ4
	void OnBtnColor5();				//��ɫ5
	void OnBtnColor6();				//��ɫ6
	void OnBtnColor7();				//��ɫ7
	void OnBtnColor8();				//��ɫ8
	void OnBtnColor9();				//��ɫ9
	void OnBtnColor10();			//��ɫ10
	void OnBtnColor11();			//��ɫ11
	void OnBtnColor12();			//��ɫ12

protected:
	//��ť�¼�����ӳ���
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_skin_close",OnClose)

		EVENT_NAME_COMMAND(L"btn_skinmgr_maxspeed",OnBtnMaxspeed)
		EVENT_NAME_COMMAND(L"btn_skinmgr_deepblue",OnBtnDeepblue)
		EVENT_NAME_COMMAND(L"btn_skinmgr_selfdefine",OnBtnSelfdefine)
		EVENT_NAME_COMMAND(L"btn_skinmgr_bigbang",OnBtnBigbang)
		EVENT_NAME_COMMAND(L"btn_skinmgr_prev",OnBtnPrev)
		EVENT_NAME_COMMAND(L"btn_skinmgr_choosing1",OnBtnChoosing1)
		EVENT_NAME_COMMAND(L"btn_skinmgr_choosing2",OnBtnChoosing2)
		EVENT_NAME_COMMAND(L"btn_skinmgr_next",OnBtnNext)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color1",OnBtnColor1)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color2",OnBtnColor2)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color3",OnBtnColor3)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color4",OnBtnColor4)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color5",OnBtnColor5)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color6",OnBtnColor6)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color7",OnBtnColor7)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color8",OnBtnColor8)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color9",OnBtnColor9)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color10",OnBtnColor10)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color11",OnBtnColor11)
		EVENT_NAME_COMMAND(L"btn_skinmgr_color12",OnBtnColor12)
		EVENT_MAP_END()    

		//������Ϣ����ӳ���
		BEGIN_MSG_MAP_EX(CSkinMgr)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		CHAIN_MSG_MAP(SHostWnd)//ע�⽫û�д������Ϣ�������ദ��
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	BOOL            m_bLayoutInited;
};