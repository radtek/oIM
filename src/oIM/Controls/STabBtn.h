// STabBtn 参考 Ratio控件修改而成，主要是只有有三种状态：正常、高亮、选中
// 同时支持 STabCtrl的绑定，选中时自动切换Page
#ifndef __STABBTN_BY_YFGZ_20180102_HEADER__
#define __STABBTN_BY_YFGZ_20180102_HEADER__


class STabBtn: public SWindow
{
    SOUI_CLASS_NAME(STabBtn, L"tabbtn")
	int		 m_nBindTabIndex;	// 绑定到TabCtrl 的索引
	SStringT m_szBindTabName;	// 绑定到TabCtrl 的名字

public:
    STabBtn();

protected:
	virtual CSize GetDesiredSize(LPCRECT pRcContainer);
	virtual BOOL NeedRedrawWhenStateChange() { return TRUE; }
	virtual BOOL IsSiblingsAutoGroupped() {return TRUE;}
	virtual SWindow * GetSelectedSiblingInGroup();
    virtual void OnStateChanging(DWORD dwOldState,DWORD dwNewState);
	
	void OnSetFocus(SWND wndOld,CFocusManager::FocusChangeReason reason);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnPaint(IRenderTarget *pRT);
	BOOL ShowTabPage(BOOL bCheck);
	HRESULT OnAttrCheck(const SStringT& strValue, BOOL bLoading);
    UINT _GetDrawState();

    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"checked",OnAttrCheck)
		ATTR_INT(L"bindtabindex", m_nBindTabIndex, FALSE);
		ATTR_STRINGT(L"bindtabname", m_szBindTabName, FALSE);
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
    SOUI_MSG_MAP_END()
};


#endif //__STABBTN_BY_YFGZ_20180102_HEADER__

