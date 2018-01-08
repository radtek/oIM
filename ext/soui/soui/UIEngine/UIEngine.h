//SOUI组件配置

#pragma  once

#include <com-def.h>
#include <com-loaderex.hpp>
#include <interface\szip-i.h>

# define UIENGINE_DLL	_T("UIEngine.dll")

class SUIEngine
{
public:
    SUIEngine()
    {
    }

    BOOL CreateZip(IObjRef **ppObj)
    {
        return UIEngine.CreateInstance(UIENGINE_DLL, ppObj, INAME_ZIP);
    }

	BOOL Create7Zip(IObjRef **ppObj)
	{
		return UIEngine.CreateInstance(UIENGINE_DLL, ppObj, INAME_7ZIP);
	}

protected:
    SComLoaderEx UIEngine;

};
