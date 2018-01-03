#ifndef __STABBTN_BY_YFGZ_20180102_HEADER__
#define __STABBTN_BY_YFGZ_20180102_HEADER__


class STabBtn: public SWindow
{
    SOUI_CLASS_NAME(STabBtn, L"tabbtn")
public:
    STabBtn();

protected:
    UINT _GetDrawState();
	virtual CSize GetDesiredSize(LPCRECT pRcContainer);
	virtual BOOL NeedRedrawWhenStateChange() { return TRUE; }
	virtual BOOL IsSiblingsAutoGroupped() {return TRUE;}
	virtual SWindow * GetSelectedSiblingInGroup();
    virtual void OnStateChanging(DWORD dwOldState,DWORD dwNewState);
	void OnSetFocus(SWND wndOld,CFocusManager::FocusChangeReason reason);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnPaint(IRenderTarget *pRT);
	HRESULT OnAttrCheck(const SStringT& strValue, BOOL bLoading);

    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"checked",OnAttrCheck)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
    SOUI_MSG_MAP_END()
};


#endif //__STABBTN_BY_YFGZ_20180102_HEADER__

