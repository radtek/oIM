/****************************************************************************
*                                                                           *
* eimuidef.h -- Basic UI Type Definitions                                   *
*                                                                           *
* Copyright (c) sxit Corporation. All rights reserved.                      *
*                                                                           *
****************************************************************************/


#ifndef _EIMUIDEF_
#define _EIMUIDEF_
#pragma once

#define ISNULLRETURN(n,r)	if(n == NULL) return r
#define DELETEOBJ(n)  {::DeleteObject(n); n = NULL;}

#define HSL2RGB(c) (RGB(GetBValue(c),GetGValue(c),GetRValue(c)))

#ifndef DUI_INLINE
#define DUI_INLINE inline /*__forceinline*/
#endif


#endif /* _EIMUIDEF_ */