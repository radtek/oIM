#include "stdafx.h"
#include "STabBtn.h"

STabBtn::STabBtn()
{
	m_nBindTabIndex = 0;
	m_pBgSkin   = NULL;
    m_bFocusable= FALSE;
}

void STabBtn::OnPaint(IRenderTarget *pRT)
{
   if ( !m_pBgSkin ) return;
 
   CRect rcClient;
   GetClientRect(&rcClient);

   m_pBgSkin->Draw(pRT, rcClient, _GetDrawState());
    __super::OnPaint(pRT);
}

CSize STabBtn::GetDesiredSize(LPCRECT pRcContainer)
{
    SASSERT(m_pBgSkin);
    return __super::GetDesiredSize(pRcContainer);
}

UINT STabBtn::_GetDrawState()
{
	if ( !m_pBgSkin ) return 0;
	DWORD dwState = GetState();

	if ( dwState & WndState_Check )
		return 2;

	if ( dwState & WndState_Normal )
		return 0;

	if ( dwState & WndState_Hover )
		return 1;

	return 3;	// WndState_Disable
}

void STabBtn::OnLButtonUp(UINT nFlags, CPoint pt)
{
    __super::OnLButtonUp(nFlags, pt);
    if(!IsChecked())
		SetCheck(TRUE);
}

SWindow* STabBtn::GetSelectedSiblingInGroup()
{
    SWindow *pParent=GetParent();
    SASSERT(pParent);
    SWindow *pSibling=pParent->GetWindow(GSW_FIRSTCHILD);
    while(pSibling)
    {
        if(pSibling->IsClass(GetClassName()))
        {
            SRadioBox * pRadio=(SRadioBox*)pSibling;
            if(pRadio->IsChecked()) return pRadio;
        }
        pSibling=pSibling->GetWindow(GSW_NEXTSIBLING);
    }
    return NULL;
}

HRESULT STabBtn::OnAttrCheck( const SStringT& strValue, BOOL bLoading )
{
    if(bLoading)
    {
        GetEventSet()->setMutedState(true);
        SetCheck(strValue != L"0");
        GetEventSet()->setMutedState(false);
    }else
    {
        SetCheck(strValue != L"0");
    }
    return S_FALSE;
}

void STabBtn::OnSetFocus(SWND wndOld, CFocusManager::FocusChangeReason reason)
{
	if(reason != CFocusManager::kReasonFocusRestore)
	{
		if(!IsChecked()) SetCheck(TRUE);
	}
    Invalidate();
}

void STabBtn::OnStateChanging( DWORD dwOldState,DWORD dwNewState )
{
    if((dwNewState & WndState_Check) && !(dwOldState & WndState_Check))
    {
        SRadioBox *pCurChecked=(SRadioBox*)GetSelectedSiblingInGroup();
        if(pCurChecked)
        {
            pCurChecked->GetEventSet()->setMutedState(true);
            pCurChecked->SetCheck(FALSE);
            pCurChecked->GetEventSet()->setMutedState(false);
        }
    }
}
