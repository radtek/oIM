//SOUI组件配置

#pragma  once

#include <com-def.h>
#include <com-loader.hpp>
#include <interface\szip-i.h>
#include "public\utils.h"
#include "ZipUtil.inl"
#include "Zip7Util.inl"

namespace SOUI{

	SOUI_COM_C int SOUI_COM_API __stdcall CreateInterface(const TCHAR* pctszIID, void** ppvIObject)
	{
		CHECK_NULL_RET_(pctszIID, ERR_INVALID_POINTER);
		CHECK_NULL_RET_(ppvIObject, ERR_INVALID_POINTER);

		if ( _tcsnicmp(pctszIID, INAME_ZIP, _tcslen(INAME_ZIP)) == 0 )
		{
			if ( SZip* pObj = new SZip)
			{
				*ppvIObject = pObj;
				return ERR_NO_ERROR;
			}

		}
		else if ( _tcsnicmp(pctszIID, INAME_7ZIP, _tcslen(INAME_7ZIP)) == 0 )
		{
			if ( S7Zip* pObj = new S7Zip)
			{
				*ppvIObject = pObj;
				return ERR_NO_ERROR;
			}
		}

		return ERR_NOT_IMPL;
	}
};