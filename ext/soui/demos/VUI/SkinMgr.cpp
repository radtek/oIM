#include "StdAfx.h"
#include "SkinMgr.h"

CSkinMgr::CSkinMgr(void) : SHostWnd(_T("LAYOUT:XML_WINSKINS"))
{
	m_bLayoutInited=FALSE;
}

CSkinMgr::~CSkinMgr(void)
{
}

BOOL CSkinMgr::OnInitDialog( HWND hWnd, LPARAM lParam )
{
	m_bLayoutInited=TRUE;

	return 0;
}

void CSkinMgr::OnBtnMaxspeed()		//����
{
	SMessageBox(NULL,_T("����"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnDeepblue()		//�����
{
	SMessageBox(NULL,_T("�����"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnSelfdefine()	//�Զ���
{
	SMessageBox(NULL,_T("�Զ���"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnBigbang()		//��Ƭ�籩
{
	SMessageBox(NULL,_T("��Ƭ�籩"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnPrev()			//��һ��Ƥ��
{
	SMessageBox(NULL,_T("��һ��Ƥ��"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnChoosing1()			//���е�Ƥ��1
{
	SMessageBox(NULL,_T("���е�Ƥ��1"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnChoosing2()			//���е�Ƥ��2
{
	SMessageBox(NULL,_T("���е�Ƥ��2"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnNext()				//��һ��Ƥ��
{
	SMessageBox(NULL,_T("��һ��Ƥ��"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor1()				//��ɫ1
{
	SMessageBox(NULL,_T("��ɫ1"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor2()				//��ɫ2
{
	SMessageBox(NULL,_T("��ɫ2"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor3()				//��ɫ3
{
	SMessageBox(NULL,_T("��ɫ3"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor4()				//��ɫ4
{
	SMessageBox(NULL,_T("��ɫ4"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor5()				//��ɫ5
{
	SMessageBox(NULL,_T("��ɫ5"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor6()				//��ɫ6
{
	SMessageBox(NULL,_T("��ɫ6"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor7()				//��ɫ7
{
	SMessageBox(NULL,_T("��ɫ7"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor8()				//��ɫ8
{
	SMessageBox(NULL,_T("��ɫ8"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor9()				//��ɫ9
{
	SMessageBox(NULL,_T("��ɫ9"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor10()				//��ɫ10
{
	SMessageBox(NULL,_T("��ɫ10"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor11()				//��ɫ11
{
	SMessageBox(NULL,_T("��ɫ11"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}

void CSkinMgr::OnBtnColor12()				//��ɫ12
{
	SMessageBox(NULL,_T("��ɫ12"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
}