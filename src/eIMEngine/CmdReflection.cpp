#include "stdafx.h"
#include "eIMEngine\CmdReflection.h"

void  C_CmdObjectMap::RegisterCmdClass(int i32CmdId, PFN_CmdCreate pfnCreateCmdInstance)
{
	if( GetMaps().find(i32CmdId) != GetMaps().end())
		return;			// Exist

	GetMaps()[i32CmdId] = pfnCreateCmdInstance;
}

void* C_CmdObjectMap::CreateInstance(int i32CmdId, const void* p)
{
	MapCmdObjIt it = GetMaps().find(i32CmdId);
	if(	 it == GetMaps().end())
	{
		TRACE_(_T("Not find CmdId: %d, can not be instanced. maybe you not call REGISTER_CMDCLASS(CmdObj, Id) to register it."), i32CmdId);
		return NULL;	// Not exist
	}

	return (it->second)(p);
}	

MapCmdObj& C_CmdObjectMap::GetMaps()
{
	static MapCmdObj s_mapCmdObj;
	return s_mapCmdObj;
}

