#pragma once

/**
by ������WIFI
*/
#include <core/Swnd.h>

namespace SOUI
{
	class SImageSwitcher : public SWindow
	{
		SOUI_CLASS_NAME(SImageSwitcher,L"imageSwitcher")
	public:
		SImageSwitcher();
		virtual ~SImageSwitcher();

	public:
		BOOL	Switch(int iSelect, BOOL bMoive = TRUE);
		BOOL	InsertImage(IBitmap * pImage, int iTo = -1);
		BOOL	InsertImage(const SStringT& szImage, int iTo = -1);

		void	RemoveAll();
		int		GetCurSel();
		size_t	GetCount();

	private:
		SArray<IBitmap*> m_lstImages;

		BOOL	m_bImgMovable;		// ͼƬ�Ƿ�����ƶ����޷���ȫͼʱ��
		BOOL	m_bTimerMove;		// ��ҳ������Timer���ñ�ʶ
		int		m_iMoveWidth;		// ��ҳ�����ƶ��Ĵ�С
		int		m_iTimesMove;		// ��ҳ�������ƶ��Ĵ���
		int		m_iSelected;		// ��ǰ��ʾ��ͼƬ����
		float	m_fRatio;			// �Ŵ���ʣ�1.0f ��ԭʼͼƬ��С
		CPoint	m_ptMoveStart;		// ��갴��ʱ������λ��
		CPoint	m_ptCenterOld;		// ��갴��ʱԭ��������λ��
		CPoint	m_ptCenter;			// ����λ��
		CRect	m_rtImgSrc;			// ͼƬ��ʾ������
//		CRect m_rtDrawDst;
		 
	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags,CPoint pt);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnTimer(char nIDEvent);
		HRESULT OnAttrImages(const SStringT& strValue,BOOL bLoading);
		
		BOOL DrawImage(IRenderTarget *pRT, CRect& rcWnd, int i32Index);
		RECT GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, BOOL bRatio = TRUE);
		RECT GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg);

	protected:
		SOUI_MSG_MAP_BEGIN()	
			MSG_WM_PAINT_EX(OnPaint)    //���ڻ�����Ϣ
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"images",OnAttrImages)
		SOUI_ATTRS_END()
	};
}