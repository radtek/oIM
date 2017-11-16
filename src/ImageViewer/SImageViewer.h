#pragma once
#include <core/Swnd.h>

namespace SOUI
{
#define SAFE_RELEASE_(P) do { if ( P ) P->Release(); P = NULL; } while(0)

#define MOVE_POS_START		1	// ��ʼ�ƶ�
#define MOVE_POS_MOVING		2	// �ƶ�������
#define MOVE_POS_STOP		3	// �ƶ����

#define EVT_RATIO_CHANGED	(EVT_EXTERNAL_BEGIN+120)
#define EVT_IMGPOS_CHANGED	(EVT_EXTERNAL_BEGIN+121)

	class EventRatioChanged : public TplEventArgs<EventRatioChanged>
	{
		SOUI_CLASS_NAME(EventRatioChanged, L"on_ratio_changed")
	public:
		EventRatioChanged(SObject *pSender, float fRatio)
			: TplEventArgs<EventRatioChanged>(pSender)
			, m_fRatio(fRatio){}
		enum {EventID=EVT_RATIO_CHANGED};
		float m_fRatio;
	};

	class EventImagePosChanged : public TplEventArgs<EventImagePosChanged>
	{
		SOUI_CLASS_NAME(EventRatioChanged, L"on_image_pos_changed")
	public:
		EventImagePosChanged(SObject *pSender, BOOL bShow, const CRect& rtImgPos, float fRatio, IBitmap* pImg = NULL)
			: TplEventArgs<EventImagePosChanged>(pSender)
			, m_pImg(pImg)
			, m_bShow(bShow)
			, m_fRatio(fRatio)
			, m_rtImgPos(rtImgPos){}
		enum {EventID=EVT_IMGPOS_CHANGED};
		CRect	 m_rtImgPos;
		IBitmap* m_pImg;
		BOOL	 m_bShow;
		float	 m_fRatio;
	};

	class SImageViewer : public SWindow
	{
		SOUI_CLASS_NAME(SImageViewer,L"imageViewer")
	public:
		SImageViewer();
		virtual ~SImageViewer();
		BOOL	Switch(int iSelect, BOOL bMoive = TRUE, LPSIZE pSize = NULL);
		BOOL	InitImages(VectImage& vectImg, int i32Sel = -1);
		BOOL	InsertImage(const SStringT& szImage, int iTo = -1);

		void	RemoveAll();
		int		GetCurSel();
		size_t	GetCount();
		float   GetRatio()const { return m_fRatio; }
		CRect	GetImgSrcPos()const { return m_rtImgSrc; }
		BOOL	IsImgMovable() const { return m_bImgMovable; }
		RECT	GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, float* pfRatio = NULL);
		CPoint	Move(int i32Oper, LPPOINT ptCenter = NULL);

	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags,CPoint pt);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnTimer(char nIDEvent);
		
		BOOL DrawImage(IRenderTarget *pRT, IBitmap *pBmp, CRect& rcWnd, int i32Index);
		RECT GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg);

	protected:
		SOUI_MSG_MAP_BEGIN()	
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
		SOUI_ATTRS_END()

	private:
		VectImage m_vectImage;		// ͼƬ�б�����ͼƬ��ȫ·��������ȫ�����أ����ܻ�����ڴ治�㣩
		IBitmap*  m_pImgSel;		// ��ǰ��ʾ��ͼƬ
		IBitmap*  m_pImgNext;		// ��/��һ��ͼƬ����ʾ���˶���ʱʹ��

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
	};
}