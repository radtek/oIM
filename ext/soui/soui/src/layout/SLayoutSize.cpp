#include "souistd.h"
#include "layout/SLayoutSize.h"
#include <math.h>

namespace SOUI
{
	static const wchar_t* s_pszUnit[] =
	{
		L"px",L"dp",L"dip",L"sp"
	};

	SLayoutSize::SLayoutSize() :fSize(0.0f),unit(px)
	{

	}

	static int fround(float v)
	{
		return (int)floor(v+0.5f);
	}

	static bool fequal(float a, float b)
	{
		return fabs(a-b)<0.00000001f;
	}

	SStringW SLayoutSize::toString() const
	{
		SStringW strValue = SStringW().Format(L"%f",fSize);
		//ȥ��sprintf("%f")���ɵ�С���������Ч��0
		LPCWSTR pszData = strValue;
		for(int i=strValue.GetLength()-1;i>=0;i--)
		{
			if(pszData[i]!=L'0')
			{
				if(pszData[i]==L'.') i--;
				strValue = strValue.Left(i+1);
				break;
			}
		}
		return SStringW().Format(L"%s%s",strValue,s_pszUnit[unit]);
	}


	bool SLayoutSize::isMatchParent() const
	{
		return fequal(fSize , SIZE_MATCH_PARENT);
	}

	void SLayoutSize::setMatchParent()
	{
		fSize = SIZE_MATCH_PARENT;
	}

	bool SLayoutSize::isWrapContent() const
	{
		return fequal(fSize , SIZE_WRAP_CONTENT);
	}

	void SLayoutSize::setWrapContent()
	{
		fSize = SIZE_WRAP_CONTENT;
	}

	void SLayoutSize::setSize(float fSize, Unit unit)
	{
		this->fSize = fSize;
		this->unit = unit;
	}

	bool SLayoutSize::isSpecifiedSize() const
	{
		return fSize>=SIZE_SPEC;
	}

	int SLayoutSize::toPixelSize(int scale) const
	{
		if(isMatchParent()) 
			return SIZE_MATCH_PARENT;
		else if(isWrapContent()) 
			return SIZE_WRAP_CONTENT;
		else if (unit == px)
			return (int)fSize;
		else//if(unit == dp || unit == dip || unit= sp)
			return (int)fround(fSize*scale / 100);
	}

	void SLayoutSize::setInvalid()
	{
		fSize = SIZE_UNDEF;
	}

	bool SLayoutSize::isValid() const
	{
		return !fequal(fSize,SIZE_UNDEF);
	}

	bool SLayoutSize::isZero() const
	{
		return fequal(fSize, 0.0f);
	}

	void SLayoutSize::parseString(const SStringW & strSize)
	{
		if(strSize.IsEmpty()) return;
		SStringW strUnit = strSize.Right(2);
		strUnit.MakeLower();
		unit = px;
		for(int i=0; i< ARRAYSIZE(s_pszUnit);i++)
		{
			if(strUnit.Compare(s_pszUnit[i]) == 0)
			{
				unit = (Unit)i;
				break;
			}
		}
		fSize = (float)_wtof(strSize);
	}

	//ֻ������ֵ,�����Ʒ���
	SLayoutSize & SLayoutSize::operator=(const SLayoutSize & src)
	{
		fSize = src.fSize;
		unit = src.unit;
		return *this;
	}

	SLayoutSize SLayoutSize::fromString(const SStringW & strSize)
	{
		SLayoutSize ret;
		ret.parseString(strSize);
		return ret;
	}

}
