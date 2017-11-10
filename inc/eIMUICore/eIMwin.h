/****************************************************************************
*                                                                           *
* eIMwin.h -- Basic UI gdi							                        *
*                                                                           *
* Copyright (c) sxit Corporation. All rights reserved.                      *
*                                                                           * 
* author		: resonly                                                   *
*                                                                           * 
* make data		: 2013-11-15								                *
*                                                                           * 
* Last Change   : 2013-11-15                                                *
*                                                                           * 
* contact		: resonly@foxmail.com                                       *
*                                                                           * 
****************************************************************************/

#ifndef _EIMWIND_
#define _EIMWIND_
#pragma once

#include "eIMdef.h"

#define _DUIWIN_INLINE DUI_INLINE

/////////////////////////////////////////////////////////////////////////////
// class CGdiobj is the gdi of all compliant objects, auto delete HGDIOBJ

class CGdiobj
{
public:
	CGdiobj(){ m_Obj = NULL;}
	CGdiobj(HGDIOBJ obj){ m_Obj = obj;}
	
	HGDIOBJ GetSafeHandle();
	BOOL	IsNull();
	BOOL	Attach(HGDIOBJ obj);   // Attach/Detach affects only the Output 
	HGDIOBJ	Detach();
	BOOL	DeleteObject();
public:
	virtual ~CGdiobj();
protected:
	HGDIOBJ m_Obj;
};

/////////////////////////////////////////////////////////////////////////////
// class CEIM_Bitmap is the HBITMAP of all compliant objects, auto delete HBITMAP

class CEIM_Bitmap : public CGdiobj
{
public:
	CEIM_Bitmap(void);
	
	BOOL		Attach(HBITMAP hBitmap);   // Attach/Detach affects only the Output HBITMAP
	HBITMAP		Detach();
	HBITMAP		GetSafeHandle();

	void		LoadFromResource( HINSTANCE hInstance, LPCTSTR pszResourceName );
	void		LoadFromResource( HINSTANCE hInstance, UINT nIDResource );

	BOOL		CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight);
	BOOL		CreateDiscardableBitmap(HDC hdc, int nWidth, int nHeight);

	UINT		GetWith();
	UINT		GetHight();

public:
	virtual ~CEIM_Bitmap(void);
};

/////////////////////////////////////////////////////////////////////////////
// class CEIM_Dc is the HBITMAP of all compliant objects, auto delete HBITMAP

class  CEIM_Dc
{
public:
	CEIM_Dc(void);
	
	BOOL	CreateCompatibleDC(CEIM_Dc *pDC);
	BOOL	CreateCompatibleDC(HDC dc);
	HDC		GetSafeHdc();
	HGDIOBJ SelectObject(__in HGDIOBJ h);
	HGDIOBJ SelectObject(__in CGdiobj * hObj);
	BOOL	Attach(HDC hDC);   // Attach/Detach affects only the Output DC
	HDC		Detach();
	virtual COLORREF SetBkColor(COLORREF crColor);
	int		SetBkMode(int nBkMode);

	void	FillSolidRect(LPRECT lpRect, COLORREF clr);
	void	FillSolidRect(int x, int y, int cx, int cy, COLORREF clr);

	BOOL    MoveTo( __in int x, __in int y);
	BOOL    MoveTo(POINT pt);
	BOOL    LineTo( __in int x, __in int y);
	BOOL    LineTo(POINT pt);

public:
	virtual ~CEIM_Dc(void);
protected:
	HDC		m_hDc;
};


#include "eIMwin.inl"

#endif /* _EIMWIND_ */
