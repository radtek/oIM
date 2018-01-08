#include "stdafx.h"
#include "SToggleEx.h"

namespace SOUI
{
	void SToggleEx::OnLButtonUp(UINT nFlags,CPoint pt)
	{
		if(GetWindowRect().PtInRect(pt)) 
			SetToggle(!GetToggle());
		__super::OnLButtonUp(nFlags,pt);
	}

}
 