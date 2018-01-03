#include "stdafx.h"
#include "STabBtn.h"
#include <algorithm>
#include "souistd.h"

STabBtn::STabBtn()
{
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

	if ((dwState & WndState_Check) || (dwState & WndState_PushDown))
		return 2;

	if ( dwState & WndState_Normal )
		return 0;

	if ( dwState & WndState_Hover )
		return 1;

	return 3;	// WndState_Disable
}

void STabBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
    if((GetState()&WndState_PushDown) && GetWindowRect().PtInRect(point))
		SetCheck(!IsChecked());

    SWindow::OnLButtonUp(nFlags,point);
}

HRESULT STabBtn::OnAttrCheck( const SStringT& strValue, BOOL bLoading )
{
    SetCheck(strValue != _T("0"));
    return S_FALSE;
}

