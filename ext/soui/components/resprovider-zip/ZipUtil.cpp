#include "stdafx.h"
#pragma warning(disable:4251)

#include <interface\szip-i.h>
#include "ZipArchive.h"

namespace SOUI{

	class SZip
		: public CZipArchive
		, public IZip
	{
	public:
		SZip(): m_lRef(1){}
		virtual long AddRef()
		{
			return InterlockedIncrement(&m_lRef);
		}

		virtual long Release()
		{
			long lRet = InterlockedDecrement(&m_lRef);
			if ( lRet <= 0)
				OnFinalRelease();

			return lRet;
		}

		virtual void OnFinalRelease()
		{
			delete this;
		}

		virtual BOOL Open(LPCTSTR pszFileName, LPCSTR pszPassword)
		{
			if (!CZipArchive::Open(pszFileName))
				return FALSE;

			return CZipArchive::SetPassword(pszPassword);			
		}

		virtual BOOL Open(HMODULE hModule, LPCTSTR pszName, LPCSTR pszPassword, LPCTSTR pszType = _T("ZIP"))
		{
			if (!CZipArchive::Open(hModule, pszName, pszType))
				return FALSE;

			return CZipArchive::SetPassword(pszPassword);			
		}

		virtual void Close()
		{
			CZipArchive::Close();
		}

		virtual BOOL IsOpen() const 
		{
			return CZipArchive::IsOpen();
		}

		virtual BYTE* GetFile(LPCTSTR pszFileName, DWORD* pdwFileSize)
		{
			m_zipFile.Close();
			if ( pdwFileSize )
				*pdwFileSize = 0;

			if ( !CZipArchive::GetFile(pszFileName, m_zipFile) )
				return NULL;

			if ( pdwFileSize )
				*pdwFileSize = m_zipFile.GetSize();

			return m_zipFile.GetData();
		}

	private:
		CZipFile m_zipFile;
		long m_lRef;
	};
	
	namespace ZIP_UTILITIE
	{
         SOUI_COM_C BOOL SOUI_COM_API SCreateInstance( IObjRef ** ppObj )
		{
			*ppObj = new SZip;
			return TRUE;
		}
    }

}//namespace SOUI