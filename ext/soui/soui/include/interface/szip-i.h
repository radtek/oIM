/*
    SOUI窗口布局接口
*/
#pragma once

#include <unknown/obj-ref-i.h>

namespace SOUI{
#define INAME_ZIP	_T("ZIP.ZIP")
#define INAME_ZIP7	_T("ZIP.7ZIP")

    struct IZip : IObjRef{
		virtual BOOL Open(LPCTSTR pszFileName, LPCSTR pszPassword) = 0;
		virtual BOOL Open(HMODULE hModule, LPCTSTR pszName, LPCSTR pszPassword, LPCTSTR pszType = _T("ZIP")) = 0;

		virtual void Close() = 0;
		virtual BOOL IsOpen() const = 0;

		virtual BYTE* GetFile(LPCTSTR pszFileName, DWORD* pdwFileSize) = 0; 
    };
}
