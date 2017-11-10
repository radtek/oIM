/*
	BesLyric  һ�� �����򵥡�����ʵ�õ� ר�������������������ֹ�����ʵ� ������������
    Copyright (C) 2017  BensonLaur

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       FileHelper.h
* @version    v1.0      
* @author     BensonLaur   
* @date       2017/01/08
* 
* Describe    File�ࣨ����򿪵��ļ��� ��CFileDialogEx �ࣨ�öԻ���ѡ���ļ����ļ��У��Ķ���
*/

#pragma once
#include "stdafx.h"
#include <windows.h>
#include <commdlg.h>

//�������ļ�ÿһ�е�����ַ���
#define MAX_CHAR_COUNT_OF_LINE 200
#define MAX_WCHAR_COUNT_OF_LINE MAX_CHAR_COUNT_OF_LINE/2

//��дGetOpenFileNameʱ�õ���ϵͳʹ�ÿؼ�����ԴID
#define  ID_COMBO_ADDR 0x47c
#define  ID_LEFT_TOOBAR 0x4A0

//ʹ��GetOpenFileName���ļ���ʱ �����滻 m_ofn.lpfnHook �Ļص����� 
UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );

/*
*   @brief Ӧ��RAII˼�룬�����ļ��ļ�����Դ
*/
class File{

public:
	File():m_pf(NULL),m_lpszPathFile(NULL),m_lpszMode(NULL){}
	
	File(LPCTSTR pathFile,LPCTSTR mode)
	{
		m_pf=_tfopen(pathFile,mode);

		m_lpszPathFile = pathFile;
		m_lpszMode = mode;
	}

	BOOL openFile(LPCTSTR pathFile,LPCTSTR mode)
	{
		//����Ѵ��ڣ����ͷ���Դ
		if(m_pf)
			fclose(m_pf);
		File(pathFile,mode);
	}

	BOOL isValidFile()
	{
		return m_pf!=NULL;
	}

	~File()
	{
		//����Ѵ��ڣ����ͷ���Դ
		if(m_pf)
			fclose(m_pf);
	}
public:
	LPCTSTR m_lpszPathFile;		/* �ļ�·�������ִ�*/
	LPCTSTR m_lpszMode;			/* ���ļ���ģʽ */

	FILE *m_pf;					/* ��ŵ�ǰ���ļ���ָ��*/

};

/*
*	@���ڴ��ļ� �� �ļ��У� �Լ� �����ļ�
*/
class CFileDialogEx
{
public:

    OPENFILENAME m_ofn;
    BOOL m_bOpenFileDialog;            // TRUE for file open, FALSE for file save
    TCHAR m_szFileTitle[_MAX_FNAME];   // contains file title after return
    TCHAR m_szFileName[_MAX_PATH];     // contains full path name after return

    CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,
        LPCTSTR lpszFilter = NULL,
        HWND hWndParent = NULL,
		BOOL bFloder = FALSE)
    {
        memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
        m_szFileName[0] = _T('\0');
        m_szFileTitle[0] = _T('\0');

        m_bOpenFileDialog = bOpenFileDialog;
        m_ofn.lStructSize = sizeof(m_ofn);
        m_ofn.lpstrFile = m_szFileName;
        m_ofn.nMaxFile = _MAX_PATH;
        m_ofn.lpstrDefExt = lpszDefExt;
        m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
        m_ofn.nMaxFileTitle = _MAX_FNAME;
        m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING| OFN_NOCHANGEDIR;
        m_ofn.lpstrFilter = lpszFilter;
        m_ofn.hwndOwner = hWndParent;

        // setup initial file name
        if(lpszFileName != NULL)
            _tcscpy_s(m_szFileName, _countof(m_szFileName), lpszFileName);

		//�ļ�������
		if(bFloder)
		{
			m_ofn.hInstance = (HMODULE)GetCurrentProcess();//��Ҫʹ��NULL,��������޷����Ƶ�����
			m_ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc;
		}
    }

    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
    {
        if(m_ofn.hwndOwner == NULL)   // set only if not specified before
            m_ofn.hwndOwner = hWndParent;

        if(m_bOpenFileDialog)
            return ::GetOpenFileName(&m_ofn);
        else
            return ::GetSaveFileName(&m_ofn);
    }

public:
	/**
	*   @brief ����ļ����Ƿ����Ҫ���ʽ��������������ϵĸ�ʽ��
	*	@param  format  ֧�ֵļ���ʽ		��ͨ�ļ���ʽ���� *.txt�� *.mp3  ("*." �Ǳ���ģ��Һ�׺����������һ���ַ�)
	*										�ļ��и�ʽ��..
	*			toChecked ������·���ַ���
	*	@return TRUE ����Ҫ��
	*	@note	
	*/

	static BOOL checkPathName(LPCTSTR format,LPCTSTR toChecked)
	{
		int i;
		bool isFloder = false;
		//TODO���쳣�׳�����
		int len = _tcslen(format);
		if(_tcscmp(format,_T(".."))==0)
		{
			isFloder = true;
		}
		else if(len < 3 || format[0]!=_T('*') || format[1]!=_T('.'))
			return FALSE;  //TODO���쳣
		

		//��ȡ����� ������·���ַ��� toChecked ����Ϣ
		TCHAR pathName[_MAX_PATH];
		TCHAR ext[_MAX_EXT];

		int lenPathName = 0, pos =-1;

		_tcscpy(pathName,toChecked);
		lenPathName = _tcslen(pathName);	//�õ�·���ܳ�
		if(!lenPathName)
			return FALSE;

		//�õ�·�������һ����.����λ������pos��
		for( i=0; i< lenPathName; i++)
		{
			if(_T('.')==pathName[i])
				pos = i;
		}

		if(isFloder) //����ļ�������
		{
			if(pos == -1)//����Ĭ���ļ��е�·���������κε�'.'
				return TRUE;
			else
				return FALSE;
		}
		else //�����ͨ��׺������
		{
			_tcscpy(ext,&pathName[pos+1]);  //�õ�·���ĺ�׺����������.����
			if(_tcscmp(&format[2],ext)==0)	//�� �����ṩ�ĺ�׺�Ա�
				return TRUE;
			else
				return FALSE;
		}
	}

};

