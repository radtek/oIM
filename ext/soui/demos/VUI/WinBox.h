#pragma once

class CWinBox : public SHostWnd
{
public:
	CWinBox(void);
	~CWinBox(void);

	void OnClose();				//�رձ������
	void OnBtnBack();			//����
	void OnBtnForward();		//ǰ��
	void OnBtnRefresh();		//ˢ��
	void OnBtnMaximize();		//���
	void OnBtnRestore();		//��ԭ

	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

protected:
	//��ť�¼�����ӳ���
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_box_close",OnClose)
		EVENT_NAME_COMMAND(L"btn_box_back",OnBtnBack)
		EVENT_NAME_COMMAND(L"btn_box_forward",OnBtnForward)
		EVENT_NAME_COMMAND(L"btn_box_refresh",OnBtnRefresh)
		EVENT_NAME_COMMAND(L"btn_box_maximize",OnBtnMaximize)
		EVENT_NAME_COMMAND(L"btn_box_restore",OnBtnRestore)
		EVENT_MAP_END()    

		//������Ϣ����ӳ���
		BEGIN_MSG_MAP_EX(CWinBox)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		CHAIN_MSG_MAP(SHostWnd)//ע�⽫û�д������Ϣ�������ദ��
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	BOOL            m_bLayoutInited;
public:
	void*			m_boxParent;
	RECT			rect_box;
};