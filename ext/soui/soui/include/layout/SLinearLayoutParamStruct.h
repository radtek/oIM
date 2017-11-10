#pragma once

#include "SLayoutSize.h"
namespace SOUI
{
	enum Gravity{
		G_Undefined=-1,
		G_Left=0,G_Top=0,
		G_Center=1,
		G_Right=2,G_Bottom=2,
	};

	struct SLinearLayoutParamStruct
	{
		SLayoutSize width;
		SLayoutSize height;
		SLayoutSize extend_left,extend_right;//�൱��android��margin����
		SLayoutSize extend_top,extend_bottom;//�൱��android��margin����
		float weight;
		Gravity gravity;
	};
}