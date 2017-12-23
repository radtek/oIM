#ifndef __ZIPARCHIVE_H__
#define __ZIPARCHIVE_H__

#pragma once


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>


#include "SevenZip/FileStream.h"

typedef struct ZIP_FIND_DATA
{
	TCHAR		szFileName[MAX_PATH];
	TCHAR		szComment[64];
	DWORD		nFileSizeCompressed;
	DWORD		nFileSizeUncompressed;
	FILETIME	ftCreationDate;
	DWORD		dwFileAttributes;
	int			nIndex;
} ZIP_FIND_DATA, *LPZIP_FIND_DATA;

class CZip7File;
class CZip7Archive;

//	ZIP file wrapper from zip archive
class CZip7File
{
	friend CZip7Archive;
protected:
	DWORD	m_dwPos;
public:
	CZip7File(DWORD dwSize=0);
	~CZip7File();
	BOOL Read(void* pBuffer, DWORD dwSize, LPDWORD pdwRead = NULL);
	BOOL Close();
	BOOL IsOpen() const ;
	BYTE* GetData() ;
	DWORD GetSize() const;
	DWORD GetPosition() const;
	DWORD Seek(DWORD dwOffset, UINT nFrom);

	BOOL Attach(LPBYTE pData, DWORD dwSize);
	void Detach();
	BlobBuffer &getBlob();
protected: 
	BlobBuffer m_blob;
};

//	ZIP Archive class, load files from a zip archive
class CZip7Archive
{
protected: 
	CZip7File		m_fileRes;	
	char			m_szPassword[64];
public:
	CZip7Archive();
	~CZip7Archive();

	BOOL Open(LPCTSTR pszFileName, LPCSTR pszPassword);
	BOOL Open(HMODULE hModule, LPCTSTR pszName, LPCTSTR pszPassword, LPCTSTR pszType = _T("ZIP"));

	void Close();
	BOOL IsOpen() const;
	
	BOOL SetPassword(LPCSTR pstrPassword);
	 
	BOOL GetFile(LPCTSTR pszFileName, CZip7File& file); 
	DWORD GetFileSize(LPCTSTR pszFileName);
	 
protected:
	BOOL OpenZip();
	void CloseFile();

	DWORD ReadFile(void* pBuffer, DWORD dwBytes);
private:
	CFileStream m_fileStreams;
};

#endif	//	__ZIP7ARCHIVE_H__
