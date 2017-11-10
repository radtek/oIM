#ifndef __EIM_CMD_REFLECTION_HEADER_2017_04_15_YFGZ__
#define __EIM_CMD_REFLECTION_HEADER_2017_04_15_YFGZ__
#pragma once

#if defined(EIMENGINE_EXPORTS)
#  define UILIB_API __declspec(dllexport)
#else
#  define UILIB_API __declspec(dllimport)
#endif

#include <map>

typedef void* (*PFN_CmdCreate)(const void*);
typedef std::map<int, PFN_CmdCreate> MapCmdObj;
typedef MapCmdObj::iterator MapCmdObjIt;

class UILIB_API C_CmdObjectMap
{
private:
	C_CmdObjectMap(){};

public:
	static void  RegisterCmdClass(int i32CmdId, PFN_CmdCreate pfnCreateCmdInstance);
	static void* CreateInstance(int i32CmdId, const void* p);
	static MapCmdObj& GetMaps();
};

template<typename T>
class C_DelegateObject 
{
public:
	C_DelegateObject(int i32CmdId)
	{
		C_CmdObjectMap::RegisterCmdClass(i32CmdId,(PFN_CmdCreate)&(C_DelegateObject::CreateInstance));
	}

	static T* CreateInstance(const void* P)
	{
		return static_cast<T*>(new T((T::PTCMD)P));
	}
};

#ifndef REGISTER_CMDCLASS
#define REGISTER_CMDCLASS(CmdObj, Id) C_DelegateObject<CmdObj> __class_##CmdObj( Id );
#endif

template<class TObj>
TObj CmdCreateInstance(int i32CmdId, const void* p)
{
	return static_cast<TObj>(C_CmdObjectMap::CreateInstance(i32CmdId,p));
}

#endif // __EIM_CMD_REFLECTION_HEADER_2017_04_15_YFGZ__