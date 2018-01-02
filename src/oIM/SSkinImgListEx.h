#ifndef __SSKIN_IMGLISTEX_20180102_HEADER__
#define __SSKIN_IMGLISTEX_20180102_HEADER__

class  SSkinImgListEx : public SSkinImgFrame
{
	SOUI_CLASS_NAME(SSkinImgListEx, L"imglistex")

public:
	SSkinImgListEx();
	virtual ~SSkinImgListEx();

	virtual SIZE GetSkinSize();

public:
	HRESULT			OnSetImgList(const SStringW & strValue, BOOL bLoading);
	HRESULT			OnSetImgPath(const SStringW & strValue, BOOL bLoading);
	void			OnSetSkinState(int nState){ m_nSkinState = nState; }
	
protected:
	virtual void	_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha);
	HRESULT			OnAttrMask(const SStringW & strValue, BOOL bLoading);
	BOOL			_DrawAttrImg(IRenderTarget *pRT, LPCRECT rcDraw, const RECT& rcSrc, BYTE byAlpha);

	IBitmap*		GetImage(DWORD dwState);
	HRESULT			OnAttrOffset(const SStringW & strValue, BOOL bLoading);

	SOUI_ATTRS_BEGIN()
		ATTR_INT(L"size-x", m_nSizeX, FALSE)  //子图固定宽
		ATTR_INT(L"size-y", m_nSizeX, FALSE)  //子图固定高
		ATTR_CUSTOM(L"mask", OnAttrMask)	  //image.a
		ATTR_CUSTOM(L"imglist", OnSetImgList)	// 使用多文件方式
		ATTR_CUSTOM(L"imgpath", OnSetImgPath)	// 使用文件目录方式
		ATTR_CUSTOM(L"offset", OnAttrOffset)
	SOUI_ATTRS_END()

private:
	SIZE	GetOffsetSize(DWORD dwState);

private:
	int						m_nSizeX;
	int						m_nSizeY;
	int						m_nSkinState;

	CAutoRefPtr<IBitmap>    m_bmpMask;
	CAutoRefPtr<IBitmap>    m_bmpAttrImg;

	SArray<IBitmap *>		m_arrBitMap;				// imglist存储
	bool					m_bImgList;				// 使用imglist方式
	float fOffsetX, fOffsetY;    /**< 窗口坐标偏移量, x += fOffsetX * width, y += fOffsetY * height  */

};


#endif //__SSKIN_IMGLISTEX_20180102_HEADER__


