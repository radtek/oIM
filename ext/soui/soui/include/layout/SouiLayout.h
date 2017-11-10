#pragma once
#include "interface/slayout-i.h"
#include "SouiLayoutParamStruct.h"

namespace SOUI{



	class SouiLayoutParam: public SObjectImpl<TObjRefImpl<ILayoutParam>>
						 , protected SouiLayoutParamStruct
	{
		SOUI_CLASS_NAME(SouiLayoutParam,L"SouiLayoutParam")

		friend class SouiLayout;
	public:
		SouiLayoutParam();

		virtual bool IsMatchParent(ORIENTATION orientation) const;

		virtual bool IsSpecifiedSize(ORIENTATION orientation) const;

		virtual bool IsWrapContent(ORIENTATION orientation) const;

		virtual SLayoutSize  GetSpecifiedSize(ORIENTATION orientation) const;

		virtual void Clear();

		virtual void SetMatchParent(ORIENTATION orientation);

		virtual void SetWrapContent(ORIENTATION orientation);

		virtual void SetSpecifiedSize(ORIENTATION orientation, const SLayoutSize& layoutSize);

		virtual void * GetRawData();
	public:
		bool IsOffsetRequired(ORIENTATION orientation) const;
        int  GetExtraSize(ORIENTATION orientation,int nScale) const;
	protected:
		HRESULT OnAttrWidth(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrHeight(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrSize(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrPos(const SStringW & strValue,BOOL bLoading);

		HRESULT OnAttrOffset(const SStringW & strValue,BOOL bLoading);

		SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"width",OnAttrWidth)
            ATTR_CUSTOM(L"height",OnAttrHeight)
			ATTR_CUSTOM(L"pos",OnAttrPos)
			ATTR_CUSTOM(L"size",OnAttrSize)
			ATTR_CUSTOM(L"offset",OnAttrOffset)
        SOUI_ATTRS_BREAK()

    protected:
        //���ַ�������������ת����POSITION_ITEM
        BOOL StrPos2ItemPos(const SStringW &strPos,POS_INFO & posItem);

        //������pos�ж����ǰ����λ��
        BOOL ParsePosition12(const SStringW & pos1, const SStringW &pos2);

        //������pos�ж���ĺ�����λ��
        BOOL ParsePosition34(const SStringW & pos3, const SStringW &pos4);


	};

	class SouiLayout: public SObjectImpl<TObjRefImpl<ILayout>>
	{
		SOUI_CLASS_NAME_EX(SouiLayout,L"SouiLayout",Layout)

	public:
		SouiLayout(void);
		~SouiLayout(void);

		static HRESULT CreateLayout(IObjRef ** ppObj);

		static HRESULT CreateLayoutParam(IObjRef ** ppObj);

		virtual LayoutType GetLayoutType() const {return Layout_Soui;}

        virtual bool IsParamAcceptable(ILayoutParam *pLayoutParam) const;

        virtual void LayoutChildren(SWindow * pParent);

        virtual ILayoutParam * CreateLayoutParam() const;

        virtual CSize MeasureChildren(SWindow * pParent,int nWidth,int nHeight) const;
    protected:
        struct WndPos{
            SWindow *pWnd;
            CRect    rc;
			bool     bWaitOffsetX;
			bool	 bWaitOffsetY;
        };

        void CalcPositionEx(SList<WndPos> *pListChildren,int nWidth,int nHeight) const;
        int CalcPostion(SList<WndPos> *pListChildren,int nWidth,int nHeight) const;

		int PositionItem2Value(SList<WndPos> *pLstChilds,SPOSITION position,const POS_INFO &pos , int nMax,BOOL bX,int nScale) const;
        
        int CalcChildLeft(SWindow *pWindow,SouiLayoutParam *pParam);
        int CalcChildRight(SWindow *pWindow,SouiLayoutParam *pParam);
        int CalcChildTop(SWindow *pWindow,SouiLayoutParam *pParam);
        int CalcChildBottom(SWindow *pWindow,SouiLayoutParam *pParam);


        BOOL IsWaitingPos( int nPos ) const;
		SWindow * GetRefSibling(SWindow *pCurWnd,int uCode);
        CRect GetWindowLayoutRect(SWindow *pWindow);
    };


}
