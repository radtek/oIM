#pragma once


////////////////////////////////////////////////////////////////////////////////////////////
// class  CGdiobj

_DUIWIN_INLINE CGdiobj::~CGdiobj()
{ 
	if (m_Obj)
	{
		DELETEOBJ(m_Obj);
	}
}
_DUIWIN_INLINE HGDIOBJ CGdiobj::GetSafeHandle()
{ 
	return m_Obj ;
}

_DUIWIN_INLINE BOOL	CGdiobj::IsNull()
{ 
	return (m_Obj == NULL);
}

_DUIWIN_INLINE BOOL		CGdiobj::Attach(HGDIOBJ obj)
{
	if (m_Obj)
	{
		DELETEOBJ(m_Obj);
	}
	m_Obj = obj;
	return TRUE;
}

_DUIWIN_INLINE HGDIOBJ	CGdiobj::Detach()
{
	HGDIOBJ hRet = NULL;
	hRet = m_Obj;
	m_Obj = NULL;
	return hRet;
}

_DUIWIN_INLINE BOOL		CGdiobj::DeleteObject()
{
	if (m_Obj)
	{
		DELETEOBJ(m_Obj);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//class CEIM_Bitmap

_DUIWIN_INLINE  CEIM_Bitmap::CEIM_Bitmap(void)
{
}

_DUIWIN_INLINE CEIM_Bitmap::~CEIM_Bitmap(void)
{
}

_DUIWIN_INLINE BOOL	CEIM_Bitmap::Attach(HBITMAP hBitmap)  // Attach/Detach affects only the Output HBITMAP
{
	ASSERT(hBitmap);
	return CGdiobj::Attach((HGDIOBJ)hBitmap);
}

_DUIWIN_INLINE HBITMAP	CEIM_Bitmap::Detach()
{
	return (HBITMAP)CGdiobj::Detach();
}

_DUIWIN_INLINE HBITMAP	CEIM_Bitmap::GetSafeHandle()
{
	return (HBITMAP)CGdiobj::GetSafeHandle();
}

_DUIWIN_INLINE UINT	CEIM_Bitmap::GetWith()
{
	ASSERT(m_Obj);
	BITMAP bmp;
	::GetObject(m_Obj,sizeof(BITMAP), (LPBYTE)&bmp);
	return bmp.bmWidth;
}
_DUIWIN_INLINE UINT	CEIM_Bitmap::GetHight()
{
	ASSERT(m_Obj);
	BITMAP bmp;
	::GetObject(m_Obj, sizeof(BITMAP), (LPBYTE)&bmp);
	return bmp.bmHeight;
}

_DUIWIN_INLINE BOOL	CEIM_Bitmap::CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight)
{
	CGdiobj::DeleteObject();
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc,nWidth,nHeight);
	CGdiobj::Attach((HGDIOBJ)hBitmap);
	return hBitmap != NULL;
}

_DUIWIN_INLINE BOOL	CEIM_Bitmap::CreateDiscardableBitmap(HDC hdc, int nWidth, int nHeight)
{
	CGdiobj::DeleteObject();
	HBITMAP hBitmap = ::CreateDiscardableBitmap(hdc,nWidth,nHeight);
	CGdiobj::Attach((HGDIOBJ)hBitmap);
	return hBitmap != NULL;
}

_DUIWIN_INLINE void	CEIM_Bitmap::LoadFromResource( HINSTANCE hInstance, LPCTSTR pszResourceName )
{
	HBITMAP hBitmap;

	hBitmap = HBITMAP( ::LoadImage( hInstance, pszResourceName, IMAGE_BITMAP, 0, 
		0, LR_CREATEDIBSECTION ) );

	CGdiobj::Attach((HGDIOBJ)hBitmap );
}
_DUIWIN_INLINE void	CEIM_Bitmap::LoadFromResource( HINSTANCE hInstance, UINT nIDResource )
{
	LoadFromResource( hInstance, MAKEINTRESOURCE( nIDResource ) );
}


//////////////////////////////////////////////////////////////////////////////////////////
//class CEIM_Bitmap

_DUIWIN_INLINE CEIM_Dc::CEIM_Dc(void)
{
	m_hDc = NULL;
}


_DUIWIN_INLINE CEIM_Dc::~CEIM_Dc(void)
{
	if (m_hDc)
	{
		::DeleteDC(m_hDc);
		m_hDc = NULL;
	}
}

_DUIWIN_INLINE BOOL	CEIM_Dc::CreateCompatibleDC(CEIM_Dc *pDC)
{
	if (m_hDc)
	{
		::DeleteDC(m_hDc);
		m_hDc = NULL;
	}
	ASSERT(pDC);
	if (pDC)
	{
		m_hDc =  ::CreateCompatibleDC(pDC->GetSafeHdc());
		return m_hDc != NULL;
	}
	return m_hDc != NULL;
}

_DUIWIN_INLINE BOOL	CEIM_Dc::CreateCompatibleDC(HDC dc)
{
	if (m_hDc)
	{
		::DeleteDC(m_hDc);
		m_hDc = NULL;
	}
	ASSERT(dc);
	if (dc)
	{
		m_hDc =  ::CreateCompatibleDC(dc);
		return m_hDc != NULL;
	}
	return m_hDc != NULL;
}


_DUIWIN_INLINE HDC	CEIM_Dc::GetSafeHdc()
{
	return m_hDc;
}

_DUIWIN_INLINE HGDIOBJ CEIM_Dc::SelectObject(__in HGDIOBJ h)
{
	ASSERT(m_hDc);
	return ::SelectObject(m_hDc,h);
}

_DUIWIN_INLINE HGDIOBJ CEIM_Dc::SelectObject(__in CGdiobj *hObj)
{
	ASSERT(hObj);
	ASSERT(hObj->GetSafeHandle());
	return ::SelectObject(m_hDc,hObj->GetSafeHandle());
}

_DUIWIN_INLINE BOOL	CEIM_Dc::Attach(HDC hDC)  // Attach/Detach affects only the Output DC
{
	if (m_hDc)
	{
		::DeleteDC(m_hDc);
		m_hDc = NULL;
	}
	m_hDc = hDC;
	return TRUE;
}

_DUIWIN_INLINE HDC	CEIM_Dc::Detach()
{
	HDC hRetDc = m_hDc;
	m_hDc = NULL;
	return hRetDc;
}

_DUIWIN_INLINE COLORREF CEIM_Dc::SetBkColor(COLORREF crColor)
{
	ASSERT(m_hDc);
	return ::SetBkColor(m_hDc,crColor);
}

_DUIWIN_INLINE int		CEIM_Dc::SetBkMode(int nBkMode)
{
	ASSERT(m_hDc);
	return ::SetBkMode(m_hDc,nBkMode);
}

_DUIWIN_INLINE void	CEIM_Dc::FillSolidRect(LPRECT lpRect, COLORREF clr)
{
	ASSERT(m_hDc);
	::SetBkColor(m_hDc, clr);
	::ExtTextOut(m_hDc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}

_DUIWIN_INLINE void	CEIM_Dc::FillSolidRect(int x, int y, int cx, int cy, COLORREF clr)
{
	ASSERT(m_hDc);
	RECT Rect = {x,y,cx,cy};
	::SetBkColor(m_hDc, clr);
	::ExtTextOut(m_hDc, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
}

_DUIWIN_INLINE  BOOL    CEIM_Dc::MoveTo( __in int x, __in int y)
{
	ASSERT(m_hDc);
	POINT ptOut;
	return ::MoveToEx(m_hDc,x,y,&ptOut);
}
_DUIWIN_INLINE  BOOL    CEIM_Dc::MoveTo(POINT pt)
{
	return MoveTo(pt.x,pt.y);
}

_DUIWIN_INLINE  BOOL    CEIM_Dc::LineTo( __in int x, __in int y)
{
	ASSERT(m_hDc);
	return ::LineTo(m_hDc,x,y);
}
_DUIWIN_INLINE  BOOL    CEIM_Dc::LineTo(POINT pt)
{
	return LineTo(pt.x,pt.y);
}