#pragma once

#include "SLayoutSize.h"

#define POSFLAG_REFCENTER      '|'        //�ο�����������
#define POSFLAG_REFPREV_NEAR   '['        //�ο�ǰһ���ֵܴ������Լ����ı�
#define POSFLAG_REFNEXT_NEAR   ']'        //�ο���һ���ֵܴ������Լ����ı�
#define POSFLAG_REFPREV_FAR    '{'        //�ο�ǰһ���ֵܴ������Լ�Զ�ı�
#define POSFLAG_REFNEXT_FAR    '}'        //�ο���һ���ֵܴ������Լ�Զ�ı�
#define POSFLAG_PERCENT        '%'        //�����ڸ����ڵİٷֱȶ�������
#define POSFLAG_SIZE           '@'        //��pos�����ж��崰�ڵ�size��ֻ���ڶ���x2,y2ʱ��Ч

namespace SOUI{

	//��������
	enum PIT{

		PIT_NULL=0,        //��Ч����
		PIT_NORMAL,        //ê������
		PIT_CENTER,        //�ο����������ĵ�,��"|"��ʼ
		PIT_PERCENT,       //ָ���ڸ�����������еİٷֱ�,��"%"��ʼ
		PIT_PREV_NEAR,     //�ο�ǰһ���ֵܴ������Լ����ı�,��"["��ʼ
		PIT_NEXT_NEAR,     //�ο���һ���ֵܴ������Լ����ı�,��"]"��ʼ
		PIT_PREV_FAR,      //�ο�ǰһ���ֵܴ������Լ�Զ�ı�,��"{"��ʼ
		PIT_NEXT_FAR,      //�ο���һ���ֵܴ������Լ�Զ�ı�,��"}"��ʼ
		PIT_SIZE,          //ָ�����ڵĿ���߸�,��"@"��ʼ
		PIT_SIB_LEFT=10,       //�ֵܽ���left,����X
		PIT_SIB_TOP=10,        //�ֵܽ���top����left��ͬ������Y
		PIT_SIB_RIGHT=11,      //�ֵܽ���right,����X 
		PIT_SIB_BOTTOM=11,      //�ֵܽ���bottom,��right��ͬ,����Y 
	};

	struct POS_INFO
	{
		PIT     pit;        /**<�������� */
		int     nRefID;     /**<����ID�����ֵܴ���ʱʹ�õ�ID,-1�����ο��ض�ID���ֵ�,ʹ��ID���õĸ�ʽΪ"sib.left@200:10"���Ƶĸ�ʽ */
		char    cMinus;     /**<�����ֵ����"-", ����-0����ֱ����nPos��ʾ����Ҫһ�������ı�־λ */
		SLayoutSize   nPos; /**<����ֵ*/
	};

	struct SouiLayoutParamStruct
	{
		int  nCount;                /**< ������������ */
		POS_INFO posLeft,posRight;       /**< ��pos���Զ����ֵ, nCount >0 ʱ��Ч*/
		POS_INFO posTop,posBottom;       /**< ��pos���Զ����ֵ, nCount >0 ʱ��Ч*/

		float fOffsetX,fOffsetY;    /**< ��������ƫ����, x += fOffsetX * width, y += fOffsetY * height  */

		SLayoutSize  width;        /**<ʹ��width���Զ���Ŀ� nCount==0 ʱ��Ч*/
		SLayoutSize  height;       /**<ʹ��height���Զ���ĸ� nCount==0 ʱ��Ч*/
	};
}
