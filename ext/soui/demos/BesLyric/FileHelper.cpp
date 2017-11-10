/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

#include "stdafx.h"
#include "FileHelper.h"

LONG g_lOriWndProc = NULL;
BOOL g_bReplaced = FALSE;

//���´����2���������ԣ�http://www.cnblogs.com/kocpp/p/5349467.html
/*
		//�ļ�������
		if(bFloder)
		{
			m_ofn.hInstance = (HMODULE)GetCurrentProcess();//��Ҫʹ��NULL,��������޷����Ƶ�����
			m_ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc;
		}
*/

//ѡ���ļ���ʱ������HookProc: m_ofn.lpfnHook = (COMMDLGPROC)MyFolderProc; 

LRESULT  __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
    switch(uMsg)
    {
    case WM_COMMAND:
        {
            if( wParam == IDOK )
            {
                wchar_t wcDirPath[MAX_PATH] = {0};
                HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                if (hComboAddr != NULL)
                {
                    GetWindowText(hComboAddr, wcDirPath, MAX_PATH);
                }
                if (!wcslen(wcDirPath))
                {
                    break;
                }
                DWORD dwAttr = GetFileAttributes(wcDirPath);
                if(dwAttr != -1 && (FILE_ATTRIBUTE_DIRECTORY & dwAttr))
                {
                    LPOPENFILENAMEW oFn = (LPOPENFILENAME)GetProp(hwnd, L"OPENFILENAME");
                    if (oFn)
                    {
                        int size = oFn->nMaxFile > MAX_PATH?MAX_PATH: oFn->nMaxFile;
                        memcpy(oFn->lpstrFile, wcDirPath, size * sizeof(wchar_t));
                        RemoveProp(hwnd, L"OPENFILENAME");
                        EndDialog(hwnd, 1);
                    }
                    else
                    {
                        EndDialog(hwnd, 0);
                    }
                }
                break;
            }
            //////////////////////////////////////////////////////////////////////////
            //��������toolbar������WM_COMMOND��Ϣ���������ߵ�toolbar��, �����OK��ť�Ե���Ͽ�
            HWND hCtrl = (HWND)lParam;
            if (hCtrl == NULL)
            {
                break;
            }
            int ctrlId = GetDlgCtrlID(hCtrl);
            if (ctrlId == ID_LEFT_TOOBAR)
            {
                HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                if (hComboAddr != NULL)
                {
                    SetWindowTextW(hComboAddr, L"");
                }
            }
        }
    break;
    }    
    int i = CallWindowProc( (WNDPROC) g_lOriWndProc , hwnd, uMsg, wParam ,lParam );
    return i;
}

UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    //�ο�reactos��֪��hdlg ��һ�����صĶԻ����丸����Ϊ���ļ��Ի��� OK��CANCEL��ť�ȿؼ�����Ϣ�ڸ����ڴ���
    if(uiMsg == WM_NOTIFY)
    {
        LPOFNOTIFY lpOfNotify = (LPOFNOTIFY)lParam;
        if (lpOfNotify->hdr.code == CDN_INITDONE )
        {
            SetPropW(GetParent(hdlg), L"OPENFILENAME", (HANDLE)(lpOfNotify->lpOFN));
            g_lOriWndProc  = ::SetWindowLongW( ::GetParent( hdlg ), GWLP_WNDPROC , (LONG)_WndProc );    
        }
        if (lpOfNotify->hdr.code == CDN_SELCHANGE)
        {
            wchar_t wcDirPath[MAX_PATH] = {0};
            CommDlg_OpenSave_GetFilePathW(GetParent( hdlg ), wcDirPath, sizeof(wcDirPath));            
            HWND hComboAddr = GetDlgItem(GetParent(hdlg), ID_COMBO_ADDR);
            if (hComboAddr != NULL)
            {
                if (wcslen(wcDirPath))
                {
                    //ȥ���ļ��п�ݷ�ʽ�ĺ�׺����
                    int pathSize = wcslen(wcDirPath);
                    if (pathSize >= 4)
                    {
                        wchar_t* wcExtension = PathFindExtensionW(wcDirPath);
                        if (wcslen(wcExtension))
                        {
                            wcExtension = CharLowerW(wcExtension);
                            if(!wcscmp(wcExtension, L".lnk"))
                            {
                                wcDirPath[pathSize - 4] = L'\0';
                            }
                        }
                    }
                    SetWindowTextW(hComboAddr, wcDirPath);
                }
                else
                {
                    SetWindowTextW(hComboAddr, L"");
                }                
            }
        }
    }
    return 1;
}
