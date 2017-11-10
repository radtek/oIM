#include "stdafx.h"
#include "DlgNewLayout.h"
#include "CDebug.h"

namespace SOUI
{

	SDlgNewLayout::SDlgNewLayout(LPCTSTR pszXmlName, SStringT strProPath):SHostDialog(pszXmlName)
	{
		m_strProPath = strProPath;
	}

	//TODO:��Ϣӳ��
	void SDlgNewLayout::OnClose()
	{
		SHostDialog::OnCancel();
	}

	void SDlgNewLayout::OnOK()
	{
		m_strPath = m_edtPath->GetWindowText();
		m_strName = m_edtName->GetWindowText();
		if (m_strPath.IsEmpty() || m_strName.IsEmpty())
		{
			CDebug::Debug(_T("��Դ���ƻ�·������Ϊ��"));
			return;

		}

		int n = m_strPath.Find(m_strProPath);
		if (n != 0)
		{
			CDebug::Debug(_T("�뽫��Դ���浽uiresĿ¼��"));
			return;
		}

		SHostDialog::OnOK();
	}

	BOOL SDlgNewLayout::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
	{
		
		m_edtName = FindChildByName2<SEdit>(L"NAME_UIDESIGNER_edit_ZY");
		m_edtPath = FindChildByName2<SEdit>(L"NAME_UIDESIGNER_edit_Path");
		

		return TRUE;
	}

    void SDlgNewLayout::OnBtnDlgOpenFile()	
	{
		CFileDialogEx OpenDlg(FALSE, NULL, NULL, 6, _T("�����ļ� (*.*)\0*.*\0\0"));
		if (IDOK ==OpenDlg.DoModal())
		{
			SStringT strFileName = OpenDlg.GetPathName();
			int n = strFileName.Find(m_strProPath);
			if (n != 0)
			{
				SMessageBox(NULL, _T("�뽫��Դ���浽uiresĿ¼��"), _T("��ʾ"), MB_OK);
				return;
			}


			n = strFileName.Find(_T(".xml"));
			if(n == -1)
			{
				strFileName = strFileName + _T(".xml");
			}

			m_edtPath->SetWindowText(strFileName);

		}
	}
}


