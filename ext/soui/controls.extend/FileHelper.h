/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       FileHelper.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/03
* 
* Describe    实现两个文件相关的helper
*/

#pragma once

#include <commdlg.h>
#include <string/tstring.h>
using namespace SOUI;
/**
* BuildFilePath
* @brief    递归创建子目录
* @param    LPCTSTR pszCurPath --  创建路径的起始位置
* @param    LPCTSTR pszNewPath --  新路径
* @param    BOOL bPath --  pszNewPath是一个路径标识
* @return   BOOL -- true创建成功
* Describe  pszNewPath指向一个文件名时，只创建文件路径部分
*/    
inline BOOL BuildFilePath(LPCTSTR pszCurPath,LPCTSTR pszNewPath,BOOL bPath=TRUE)
{
    TCHAR szCurDir[MAX_PATH+1];
    GetCurrentDirectory(MAX_PATH,szCurDir);
    if(!SetCurrentDirectory(pszCurPath)) return FALSE;
    TCHAR szNewPath[MAX_PATH+1];
    _tcscpy_s(szNewPath,_countof(szNewPath),pszNewPath);
    if(bPath)
    {
        int nLen=_tcslen(szNewPath);
        if(szNewPath[nLen-1]!=_T('\\'))
            _tcscat(szNewPath,_T("\\"));
    }
    LPTSTR pszPath=_tcschr(szNewPath,_T('\\'));
    while(pszPath)
    {
        *pszPath=0;
        if(!CreateDirectory(szNewPath,NULL)) return FALSE;
        *pszPath=_T('\\');
        pszPath=_tcschr(pszPath+1,_T('\\'));
    }
    SetCurrentDirectory(szCurDir);
    return TRUE;
}

class CFileDialogEx
{
protected:
    OPENFILENAME m_ofn;
    BOOL m_bOpenFileDialog;            // TRUE for file open, FALSE for file save
    TCHAR m_szFileTitle[_MAX_FNAME];   // contains file title after return
	TCHAR*m_pszFileName;

public:
    CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,
        LPCTSTR lpszFilter = NULL,
        HWND hWndParent = NULL)
    {
        memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
        m_szFileTitle[0] = _T('\0');
		m_pszFileName	 = m_szFileTitle;
		m_ofn.nMaxFile   = (dwFlags & OFN_ALLOWMULTISELECT) ? (MAX_PATH * 1024) : MAX_PATH;

		m_pszFileName = new TCHAR[m_ofn.nMaxFile];
		memset(m_pszFileName, 0, sizeof(TCHAR) * m_ofn.nMaxFile);

        m_bOpenFileDialog = bOpenFileDialog;
        m_ofn.lStructSize = sizeof(m_ofn);
        m_ofn.lpstrFile = m_pszFileName;
        m_ofn.lpstrDefExt = lpszDefExt;
        m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
        m_ofn.nMaxFileTitle = _MAX_FNAME;
        m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING| OFN_NOCHANGEDIR;
        m_ofn.lpstrFilter = lpszFilter;
        m_ofn.hwndOwner = hWndParent;

        // setup initial file name
        if(lpszFileName != NULL)
            _tcscpy_s(m_pszFileName, m_ofn.nMaxFile, lpszFileName);
    }

	~CFileDialogEx()
	{
		if ( m_pszFileName )
			delete []m_pszFileName;
		m_pszFileName = NULL;
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

	// 获取 OFN
	const OPENFILENAME& GetOFN() const
	{
		return m_ofn;
	} 

	// 获取 OFN
	OPENFILENAME& GetOFN()	
	{
		return m_ofn;
	} 

	// 单选方式时，获取文件名
	SStringT GetFileName() const
	{
		return PathFindFileName(m_ofn.lpstrFile);
	}
	
	// 单选方式时，获取文件全路径；多选时，获取路径
	SStringT GetPathName() const
	{
		return m_ofn.lpstrFile;
	}

	TCHAR* GetStartPosition() const
	{ 
		return m_ofn.lpstrFile;
	}

	SStringT GetNextPathName(TCHAR*& pos) const
	{
		if ( pos == m_ofn.lpstrFile )
		{
			if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
			{
				pos = NULL;
				return m_ofn.lpstrFile;
			}

			pos += _tcslen(pos) + 1;	// 跳过Path

			if (*pos == 0)
			{
				pos = NULL;
				return m_ofn.lpstrFile;
			}
		}

		SStringT szRet = m_ofn.lpstrFile;
		szRet += _T("\\");
		szRet += pos;
		pos   += _tcslen(pos) + 1;	
		if (*pos == 0)
			pos = NULL;

		return szRet;

	}

};
