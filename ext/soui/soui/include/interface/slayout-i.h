/*
    SOUI���ڲ��ֽӿ�
*/
#pragma once

#include <sobject/sobject-i.h>

namespace SOUI{
    class SWindow;
	class SLayoutSize;


    enum ORIENTATION{
        Horz,Vert,Any,Both,
    };
    
	enum{
		SIZE_UNDEF = -3,
		SIZE_WRAP_CONTENT=-1,
		SIZE_MATCH_PARENT=-2,
		SIZE_SPEC = 0,
	};

	enum LayoutType
	{
		Layout_Soui=0,	//SOUI ����
		Layout_Linear,  //���Բ���
	};

    struct ILayoutParam :IObjRef, IObject
    {
		virtual void Clear() = 0;
        virtual bool IsMatchParent(ORIENTATION orientation) const = 0;
		virtual bool IsWrapContent(ORIENTATION orientation) const = 0;
        virtual bool IsSpecifiedSize(ORIENTATION orientation) const = 0;
        virtual SLayoutSize GetSpecifiedSize(ORIENTATION orientation) const = 0;
		virtual void SetMatchParent(ORIENTATION orientation) = 0;
		virtual void SetWrapContent(ORIENTATION orientation) = 0;
		virtual void SetSpecifiedSize(ORIENTATION orientation, const SLayoutSize& layoutSize) = 0;
		virtual void * GetRawData() = 0;
    };

    struct ILayout : IObject , IObjRef{
		virtual bool IsParamAcceptable(ILayoutParam *pLayoutParam) const = 0;
        virtual void LayoutChildren(SWindow * pParent) = 0;
        virtual ILayoutParam * CreateLayoutParam() const = 0;
		virtual CSize MeasureChildren(SWindow * pParent,int nWidth,int nHeight) const =0;
		virtual LayoutType GetLayoutType() const = 0;
    };
}
