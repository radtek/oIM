#pragma once

//加载组件辅助类
//SOUI组件需要提供SCreateInstance接口。接口定义必须是PFN_SCreateInstance
//oIM组件需要提供CreateInstance接口。接口定义必须是PFN_CreateInstance
class SComLoader
{
    typedef BOOL (*PFN_SCreateInstance)(IObjRef **);
	typedef int (__stdcall* PFN_CreateInterface)(const TCHAR*, IObjRef**);

public:
    SComLoader()
    : m_hMod(NULL)
	, m_fnCreateInst(NULL)
	, m_fnSCreateInst(NULL)
    {
        m_szDllPath[0]=0;
    }

    ~SComLoader()
    {
		Unload();
    }

	void Unload()
	{
		if(m_hMod) 
			FreeLibrary(m_hMod);

		m_hMod = NULL;
		m_fnSCreateInst = NULL;
		m_fnCreateInst  = NULL;
	}

    BOOL CreateInstance(LPCTSTR pszDllPath, IObjRef **ppObj, LPCSTR pszFnName = "SCreateInstance")
    {
		if(!m_fnSCreateInst)
		{
			m_hMod = LoadLibrary(pszDllPath);
			if(!m_hMod) return FALSE;
			m_fnSCreateInst = (PFN_SCreateInstance)GetProcAddress(m_hMod,pszFnName);
			if(!m_fnSCreateInst)
			{
				Unload();
				return FALSE;
			}
			_tcscpy(m_szDllPath,pszDllPath);
		}
		return m_fnSCreateInst(ppObj);
    }

	BOOL CreateInstance(LPCTSTR pszDllPath,IObjRef **ppObj, LPCTSTR pctszIID, LPCSTR pszFnName = "CreateInterface")
	{
		if(!m_fnCreateInst)
		{
			m_hMod = LoadLibrary(pszDllPath);
			if(!m_hMod) return FALSE;
			m_fnCreateInst = (PFN_CreateInterface)GetProcAddress(m_hMod, pszFnName);
			if(!m_fnCreateInst)
			{
				Unload();
				return FALSE;
			}
			_tcscpy(m_szDllPath,pszDllPath);
		}
		return SUCCEEDED(m_fnCreateInst(pctszIID, ppObj));
	}

protected:
    HMODULE m_hMod;
    PFN_SCreateInstance m_fnSCreateInst;
	PFN_CreateInterface m_fnCreateInst;
    TCHAR   m_szDllPath[MAX_PATH];
};