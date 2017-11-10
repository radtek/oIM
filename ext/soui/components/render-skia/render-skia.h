#pragma once


#include <helper/color.h>
#include <unknown/obj-ref-impl.hpp>
#include <Shlwapi.h>
#include <core\SkCanvas.h>
#include <core\SkBitmap.h>
#include <core\SkTypeface.h>
#include <helper\SAttrCracker.h>
#include <string\tstring.h>
#include <string\strcpcvt.h>
#include <interface/render-i.h>
#include <souicoll.h>

namespace SOUI
{
	//////////////////////////////////////////////////////////////////////////
	// SRenderFactory_Skia
	class SRenderFactory_Skia : public TObjRefImpl<IRenderFactory>
	{
	public:
		SRenderFactory_Skia()
		{
		}
        
        ~SRenderFactory_Skia()
        {
        }
        
		virtual BOOL CreateRenderTarget(IRenderTarget ** ppRenderTarget,int nWid,int nHei);
        virtual BOOL CreateFont(IFont ** ppFont , const LOGFONT &lf);
        virtual BOOL CreateBitmap(IBitmap ** ppBitmap);
        virtual BOOL CreateRegion(IRegion **ppRgn);
        
        virtual void SetImgDecoderFactory(IImgDecoderFactory *pImgDecoderFac){m_imgDecoderFactory=pImgDecoderFac;}
        
        virtual IImgDecoderFactory * GetImgDecoderFactory(){return m_imgDecoderFactory;}
    protected:
        CAutoRefPtr<IImgDecoderFactory> m_imgDecoderFactory;
	};

    
	//////////////////////////////////////////////////////////////////////////
	// TSkiaRenderObjImpl
	template<class T>
	class TSkiaRenderObjImpl : public TObjRefImpl<SObjectImpl<T>>
	{
	public:
		TSkiaRenderObjImpl(IRenderFactory * pRenderFac):m_pRenderFactory(pRenderFac)
		{

		}

        virtual ~TSkiaRenderObjImpl(){}

		virtual IRenderFactory * GetRenderFactory() const
		{
			return m_pRenderFactory;
		}


	protected:
		IRenderFactory *m_pRenderFactory;
	};


	//////////////////////////////////////////////////////////////////////////
	// SPen_Skia
	class SPen_Skia : public TSkiaRenderObjImpl<IPen>
	{
		SOUI_CLASS_NAME(SPen_Skia,L"pen")
	public:
		SPen_Skia(IRenderFactory * pRenderFac,int iStyle=PS_SOLID,COLORREF cr=0xFF000000,int cWidth=1)
			:TSkiaRenderObjImpl<IPen>(pRenderFac)
			,m_nWidth(cWidth),m_style(iStyle),m_cr(cr)
		{
		}

		int GetWidth(){return m_nWidth;}

		int GetStyle(){return m_style;}

		void SetWidth(int nWid) {m_nWidth=nWid;}

		void SetStyle(int nStyle){m_style = nStyle;}

		COLORREF GetColor(){return m_cr;}

		void SetColor(COLORREF cr){m_cr = cr;}
	protected:
	
		int		m_style;
		int		m_nWidth;
		COLORREF	m_cr;
	};

	//////////////////////////////////////////////////////////////////////////
	// SFont_Skia
	class SFont_Skia: public TSkiaRenderObjImpl<IFont>
	{
		SOUI_CLASS_NAME(SFont_Skia,L"font")
	public:
		SFont_Skia(IRenderFactory * pRenderFac,const LOGFONT * plf);

        virtual ~SFont_Skia();

        virtual const LOGFONT * LogFont() const {return &m_lf;}

        virtual LPCTSTR FamilyName()
        {
            return m_lf.lfFaceName;
        }
        virtual int TextSize(){return m_lf.lfHeight;}
        virtual BOOL IsBold(){ return m_lf.lfWeight == FW_BOLD;}
        virtual BOOL IsUnderline(){return m_lf.lfUnderline;}
        virtual BOOL IsItalic(){return m_lf.lfItalic;}
        virtual BOOL IsStrikeOut(){return m_lf.lfStrikeOut;}

        const SkPaint  GetPaint() const {return m_skPaint;}
        SkTypeface *GetFont()const {return m_skFont;}

		virtual HRESULT DefAttributeProc(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading);
		virtual void OnInitFinished(pugi::xml_node xmlNode); 
		SOUI_ATTRS_BEGIN()
			ATTR_ENUM_BEGIN(L"blurStyle",SkBlurStyle,FALSE)
				ATTR_ENUM_VALUE(L"normal",kNormal_SkBlurStyle)
				ATTR_ENUM_VALUE(L"solid",kSolid_SkBlurStyle)
				ATTR_ENUM_VALUE(L"outer",kOuter_SkBlurStyle)
				ATTR_ENUM_VALUE(L"inner",kInner_SkBlurStyle)
			ATTR_ENUM_END(m_blurStyle)
			ATTR_FLOAT(L"blurRadius",m_blurRadius,FALSE)
		SOUI_ATTRS_END()
	protected:
        SkTypeface *m_skFont;   //��������
        SkPaint     m_skPaint;  //�������ֻ�������
        LOGFONT     m_lf;
		SkBlurStyle m_blurStyle;
		SkScalar	m_blurRadius;
	};

	class SBrush_Skia : public TSkiaRenderObjImpl<IBrush>
	{
		SOUI_CLASS_NAME(SBrush_Skia,L"brush")

	public:
		static SBrush_Skia * CreateSolidBrush(IRenderFactory * pRenderFac,COLORREF cr){
			return new SBrush_Skia(pRenderFac,cr);
		}

		static SBrush_Skia * CreateBitmapBrush(IRenderFactory * pRenderFac,SkBitmap bmp)
		{
			return new SBrush_Skia(pRenderFac,bmp);
		}

		SkBitmap GetBitmap(){return m_bmp;}

		COLORREF GetColor() {return m_cr;}

		BOOL IsBitmap(){return m_fBmp;}
	protected:
		SBrush_Skia(IRenderFactory * pRenderFac,COLORREF cr)
			:TSkiaRenderObjImpl<IBrush>(pRenderFac),m_cr(cr),m_fBmp(FALSE)
		{

		}
		SBrush_Skia(IRenderFactory * pRenderFac,SkBitmap bmp)
			:TSkiaRenderObjImpl<IBrush>(pRenderFac),m_bmp(bmp),m_fBmp(TRUE)
		{

		}

		COLORREF m_cr;		//��ɫ��ˢ
		SkBitmap m_bmp;		//λͼ��ˢ
		BOOL	 m_fBmp;
	};

	//////////////////////////////////////////////////////////////////////////
	// SBitmap_Skia
	class SBitmap_Skia : public TSkiaRenderObjImpl<IBitmap>
	{
		SOUI_CLASS_NAME(SBitmap_Skia,L"bitmap")

	public:
		SBitmap_Skia(IRenderFactory *pRenderFac);
        ~SBitmap_Skia();

		virtual HRESULT Init(int nWid,int nHei,const LPVOID pBits=NULL);
        virtual HRESULT Init(IImgFrame *pFrame);
		virtual HRESULT LoadFromFile(LPCTSTR pszFileName);
		virtual HRESULT LoadFromMemory(LPBYTE pBuf,size_t szLen);

        virtual UINT Width() const;
        virtual UINT Height() const;
        virtual SIZE Size() const;
        virtual LPVOID  LockPixelBits();
        virtual void    UnlockPixelBits(LPVOID);
        virtual const LPVOID  GetPixelBits() const;
        
		SkBitmap & GetSkBitmap(){return m_bitmap;}
		HBITMAP  GetGdiBitmap(){return m_hBmp;}
	protected:
	    HBITMAP CreateGDIBitmap(int nWid,int nHei,void ** ppBits);
	    
        HRESULT ImgFromDecoder(IImgX *imgDecoder);

		SkBitmap    m_bitmap;   //skia �����BITMAP
		HBITMAP     m_hBmp;     //��׼��32λλͼ����m_bitmap�����ڴ�
	};

	//////////////////////////////////////////////////////////////////////////
	//	SRegion_Skia
	class SRegion_Skia: public TSkiaRenderObjImpl<IRegion>
	{
		SOUI_CLASS_NAME(SRegion_Skia,L"region")

	public:
		SRegion_Skia(IRenderFactory *pRenderFac);
        virtual ~SRegion_Skia();

		virtual void CombineRect(LPCRECT lprect,int nCombineMode);
		virtual void CombineRoundRect(LPCRECT lprect, POINT ptRadius, int nCombineMode);
		virtual void CombineEllipse(LPCRECT lprect , int nCombineMode);
        virtual void CombineRgn(const IRegion * pRgnSrc,int nCombineMode );
        virtual void SetRgn(const HRGN hRgn);
		virtual BOOL PtInRegion(POINT pt);
		virtual BOOL RectInRegion(LPCRECT lprect);
		virtual void GetRgnBox(LPRECT lprect);
		virtual BOOL IsEmpty();
        virtual void Offset(POINT pt);
        virtual void Clear();
        
        SkRegion GetRegion() const;
        
        void SetRegion(const SkRegion & rgn);
        
        static SkRegion::Op RGNMODE2SkRgnOP(UINT mode);


	protected:
        SkRegion    m_rgn;
	};


	//////////////////////////////////////////////////////////////////////////
	//	SRenderTarget_Skia
	//////////////////////////////////////////////////////////////////////////
	class SRenderTarget_Skia: public TObjRefImpl<IRenderTarget>
	{
	public:
		SRenderTarget_Skia(IRenderFactory* pRenderFactory,int nWid,int nHei);
		~SRenderTarget_Skia();

		//ֻ֧�ִ���λͼ����
		virtual HRESULT CreateCompatibleRenderTarget(SIZE szTarget,IRenderTarget **ppRenderTarget);

		virtual HRESULT CreatePen(int iStyle,COLORREF cr,int cWidth,IPen ** ppPen);
		virtual HRESULT CreateSolidColorBrush(COLORREF cr,IBrush ** ppBrush);
		virtual HRESULT CreateBitmapBrush( IBitmap *pBmp,IBrush ** ppBrush );
		virtual HRESULT CreateRegion(IRegion ** ppRegion);

		virtual HRESULT Resize(SIZE sz);

        virtual HRESULT OffsetViewportOrg(int xOff, int yOff, LPPOINT lpPoint=NULL);
        virtual HRESULT GetViewportOrg(LPPOINT lpPoint);
        virtual HRESULT SetViewportOrg( POINT pt );

		virtual HRESULT PushClipRect(LPCRECT pRect,UINT mode=RGN_AND);
        virtual HRESULT PushClipRegion(IRegion *pRegion,UINT mode=RGN_AND);
		virtual HRESULT PopClip();

        virtual HRESULT ExcludeClipRect(LPCRECT pRc);
        virtual HRESULT IntersectClipRect(LPCRECT pRc);

        virtual HRESULT SaveClip(int *pnState);
        virtual HRESULT RestoreClip(int nState=-1);

        virtual HRESULT GetClipRegion(IRegion **ppRegion);
        virtual HRESULT GetClipBox(LPRECT prc);
        
		virtual HRESULT BitBlt(LPCRECT pRcDest,IRenderTarget *pRTSour,int xSrc,int ySrc,DWORD dwRop=SRCCOPY);
        virtual HRESULT AlphaBlend(LPCRECT pRcDest,IRenderTarget *pRTSrc,LPCRECT pRcSrc,BYTE byAlpha);

		virtual HRESULT DrawText( LPCTSTR pszText,int cchLen,LPRECT pRc,UINT uFormat);
		virtual HRESULT MeasureText(LPCTSTR pszText,int cchLen, SIZE *psz );

		virtual HRESULT DrawRectangle(LPCRECT pRect);
		virtual HRESULT FillRectangle(LPCRECT pRect);
        virtual HRESULT FillSolidRect(LPCRECT pRect,COLORREF cr);
        virtual HRESULT ClearRect(LPCRECT pRect,COLORREF cr);
        virtual HRESULT InvertRect(LPCRECT pRect);

        virtual HRESULT DrawEllipse(LPCRECT pRect);
        virtual HRESULT FillEllipse(LPCRECT pRect);
        virtual HRESULT FillSolidEllipse(LPCRECT pRect,COLORREF cr);

        virtual HRESULT DrawArc(LPCRECT pRect,float startAngle,float sweepAngle,bool useCenter);
        virtual HRESULT FillArc(LPCRECT pRect,float startAngle,float sweepAngle);

        virtual HRESULT DrawRoundRect(LPCRECT pRect,POINT pt);
        virtual HRESULT FillRoundRect(LPCRECT pRect,POINT pt);
        virtual HRESULT FillSolidRoundRect(LPCRECT pRect,POINT pt,COLORREF cr);

        virtual HRESULT DrawLines(LPPOINT pPt,size_t nCount);
        virtual HRESULT GradientFill(LPCRECT pRect,BOOL bVert,COLORREF crBegin,COLORREF crEnd,BYTE byAlpha=0xFF);
        virtual HRESULT GradientFillEx( LPCRECT pRect,const POINT* pts,COLORREF *colors,float *pos,int nCount,BYTE byAlpha=0xFF );

		virtual HRESULT TextOut(
			int x,
			int y,
			LPCTSTR lpszString,
			int nCount);

        virtual HRESULT DrawIconEx(int xLeft, int yTop, HICON hIcon, int cxWidth,int cyWidth,UINT diFlags);
        virtual HRESULT DrawBitmap(LPCRECT pRcDest,IBitmap *pBitmap,int xSrc,int ySrc,BYTE byAlpha=0xFF);
        virtual HRESULT DrawBitmapEx(LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,UINT expendMode, BYTE byAlpha=0xFF);
        virtual HRESULT DrawBitmap9Patch(LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,LPCRECT pRcSourMargin,UINT expendMode,BYTE byAlpha=0xFF);

		virtual IRenderObj * GetCurrentObject(OBJTYPE uType);
        virtual HRESULT SelectDefaultObject(OBJTYPE uType,IRenderObj ** ppOldObj = NULL);
        virtual HRESULT SelectObject(IRenderObj *pObj,IRenderObj ** ppOldObj = NULL);


		virtual COLORREF GetTextColor()
		{
			return m_curColor.toCOLORREF();
		}
		
		virtual COLORREF SetTextColor(COLORREF color)
		{
			COLORREF crOld=m_curColor.toCOLORREF();
 			m_curColor.setRGB(color);
			return crOld;
		}
		
        virtual HDC GetDC(UINT uFlag=0);

		virtual void ReleaseDC(HDC hdc);

        virtual HRESULT QueryInterface(REFGUID iid,IObjRef ** ppObj);
        
        virtual HRESULT SetTransform(const IxForm * pXForm,IxForm *pOldXFrom=NULL);

        virtual HRESULT GetTransform(IxForm * pXForm) const;

		virtual COLORREF GetPixel( int x, int y );

		virtual COLORREF SetPixel( int x, int y, COLORREF cr );

		virtual HRESULT GradientFill2(LPCRECT pRect,GradientType type,COLORREF crStart,COLORREF crCenter,COLORREF crEnd,float fLinearAngle,float fCenterX,float fCenterY,int nRadius,BYTE byAlpha=0xff);

    public:
        SkCanvas *GetCanvas(){return m_SkCanvas;}

		virtual SStringW GetAttribute(const SStringW & strAttr) const
		{
			if(strAttr.CompareNoCase(L"antiAlias") == 0)
				return m_bAntiAlias?L"1":L"0";
			return __super::GetAttribute(strAttr);
		}

		SOUI_ATTRS_BEGIN()
			ATTR_BOOL(L"antiAlias",m_bAntiAlias,FALSE)
		SOUI_ATTRS_END()

    protected:
		SkCanvas *m_SkCanvas;
        SColor            m_curColor;
		CAutoRefPtr<SBitmap_Skia> m_curBmp;
		CAutoRefPtr<SPen_Skia> m_curPen;
		CAutoRefPtr<SBrush_Skia> m_curBrush;
        CAutoRefPtr<SFont_Skia> m_curFont;
    
        SkPoint         m_ptOrg;
        
        //ע�Ᵽ��4��Ĭ�ϵ�RenderObject����
        CAutoRefPtr<IBitmap> m_defBmp;
        CAutoRefPtr<IPen> m_defPen;
        CAutoRefPtr<IBrush> m_defBrush;
        CAutoRefPtr<IFont> m_defFont;

		CAutoRefPtr<IRenderFactory> m_pRenderFactory;

        HDC m_hGetDC;
        UINT m_uGetDCFlag;

		bool			m_bAntiAlias;
	};
	
	namespace RENDER_SKIA
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppRenderFactory);
    }
}

