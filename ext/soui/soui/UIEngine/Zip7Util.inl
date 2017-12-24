#include <souistd.h>
#include <interface\szip-i.h>
#include <resprovider-7zip\Zip7Archive.h>

namespace SOUI{

	class S7Zip
		: public CZip7Archive
		, public IZip
	{
	public:
		S7Zip(): m_lRef(1){}
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
			return CZip7Archive::Open(pszFileName, pszPassword);
		}

		virtual BOOL Open(HMODULE hModule, LPCTSTR pszName, LPCSTR pszPassword, LPCTSTR pszType = _T("ZIP"))
		{
			return CZip7Archive::Open(hModule, pszName, S_CA2T(pszPassword), pszType);
		}

		virtual void Close()
		{
			CZip7Archive::Close();
		}

		virtual BOOL IsOpen() const 
		{
			return CZip7Archive::IsOpen();
		}

		virtual BYTE* GetFile(LPCTSTR pszFileName, DWORD* pdwFileSize)
		{
			m_zipFile.Close();
			if ( pdwFileSize )
				*pdwFileSize = 0;

			if ( !CZip7Archive::GetFile(pszFileName, m_zipFile) )
				return NULL;

			if ( pdwFileSize )
				*pdwFileSize = m_zipFile.GetSize();

			return m_zipFile.GetData();
		}

	private:
		CZip7File m_zipFile;
		long m_lRef;
	};
	
	namespace ZIP7_UTILITIE
	{
         SOUI_COM_C BOOL SOUI_COM_API SCreateInstance( IObjRef ** ppObj )
		{
			*ppObj = new S7Zip;
			return TRUE;
		}
    }

}//namespace SOUI