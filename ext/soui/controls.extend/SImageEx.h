#pragma once
#include "core/SWnd.h"

namespace SOUI
{
class SOUI_EXP SImageEx : public SImageWnd
{
    SOUI_CLASS_NAME(SImageWnd, L"imgex")
public:
    SImageEx();
    ~SImageEx();

protected:
    HRESULT OnSrc(const SStringW& strValue, BOOL bLoading);

    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"src", OnSrc)
    SOUI_ATTRS_END()
};

};