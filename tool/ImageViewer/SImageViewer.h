#pragma once
#include <core/Swnd.h>
#include "../../ext/soui/controls.extend/gif/SSkinAPNG.h"

namespace SOUI
{
#define SAFE_RELEASE_(P) do { if ( P ) P->Release(); P = NULL; } while(0)

#define EVT_RATIO_CHANGED	(EVT_EXTERNAL_BEGIN+120)
#define EVT_IMGPOS_CHANGED	(EVT_EXTERNAL_BEGIN+121)

#define RATIO_MIN		(4)		// 4%
#define RATIO_100		(100)	// 100%
#define RATIO_MAX		(2000)	// 20000%
#define RATIOF_(F)		((F) / 100.f)

#define ZOOM_DELTA		(0.035f)
typedef enum tagMoveType
{
	eMOVE_NONE = 0,	// û���ƶ�
	eMOVE_START,	// ��ʼ�ƶ�
	eMOVE_MOVING,	// �ƶ�������
	eMOVE_STOP,		// �ƶ����
}E_MoveType, *PE_MoveType;

	class EventRatioChanged : public TplEventArgs<EventRatioChanged>
	{	// �Ŵ��ʸı��¼�
		SOUI_CLASS_NAME(EventRatioChanged, L"on_ratio_changed")
	public:
		EventRatioChanged(SObject *pSender, float fRatio)
			: TplEventArgs<EventRatioChanged>(pSender)
			, m_fRatio(fRatio){}
		enum {EventID=EVT_RATIO_CHANGED};
		float m_fRatio;	// ��ǰ�ķŴ���
	};

	class EventImagePosChanged : public TplEventArgs<EventImagePosChanged>
	{	// ͼƬ��ʾλ�øı��¼�
		SOUI_CLASS_NAME(EventRatioChanged, L"on_image_pos_changed")
	public:
		EventImagePosChanged(SObject *pSender, BOOL bShow, BOOL bGif, const CRect& rtImgPos, float fRatio, BOOL bError, IBitmap* pImg = NULL)
			: TplEventArgs<EventImagePosChanged>(pSender)
			, m_pImg(pImg)
			, m_bShow(bShow)
			, m_bGif(bGif)
			, m_bError(bError)
			, m_fRatio(fRatio)
			, m_rtImgPos(rtImgPos){}
		enum {EventID=EVT_IMGPOS_CHANGED};
		CRect	 m_rtImgPos;	// ��ʾ��ͼƬԴλ��(�����ԭʼͼƬ)
		IBitmap* m_pImg;		// ��ǰ��ʾ��ͼƬ�ӿڶ���
		BOOL	 m_bShow;		// �Ƿ���ʾ��ͼ
		BOOL	 m_bGif;		// �Ƿ���GIF
		BOOL	 m_bError;		// �Ƿ����ͼƬʧ��
		float	 m_fRatio;		// ��ǰ�ķŴ���
	};

	class SImageViewer : public SWindow
	{
		SOUI_CLASS_NAME(SImageViewer,L"imageViewer")
	public:
		SImageViewer();
		virtual ~SImageViewer();
		BOOL	Switch(int iSelect, int i32Speed = 0, LPSIZE pSize = NULL);	// �л���ʾͼƬ
		BOOL	InitImages(VectImage& vectImg, int i32Sel = -1);	// ʹ�ô��ڳ�ʼ����ʾͼƬ�б�
		BOOL	InsertImage(const SStringT& szImage, int iTo = -1);	// ����ͼƬ

		void	RemoveAll();	// ���ͼƬ
		int		GetCurSel();	// ��ȡ��ǰ��ʾ��ͼƬ����
		size_t	GetCount();		// ��ȡͼƬ����
		float   GetRatio()const { return m_fRatio; }			// ��ȡͼƬ�Ŵ���
		CRect	GetImgSrcPos()const { return m_rtImgSrc; }		// ��ȡͼƬԴ��ʾλ��
		BOOL	IsImgMovable() const { return m_bImgMovable; }	// �Ƿ�ͼƬ�����ƶ�
		RECT	GetDefaultDest(const CRect& rtWnd, const SIZE& szImg, float* pfRatio = NULL);	// ��ȡĬ����ʾλ��
		CPoint	Move(E_MoveType eType, LPPOINT ptCenter = NULL);// �ƶ�ͼƬ
		BOOL	RealSize();										// ��ʾʵ�ʴ�С
		float	Zoom(float fDelta, BOOL bFixed = FALSE);		// ����
		BOOL	OpenFolder(LPCTSTR pszPathImage = NULL);		// ���ļ���
		BOOL	Saveas(BOOL bOpenFolder = TRUE);				// ���Ϊ
		BOOL	Rotate(BOOL bRight = TRUE);						// ��ת

	protected:
		void OnPaint(IRenderTarget *pRT);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags,CPoint pt);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnTimer(char nIDEvent);
		
		inline BOOL		DrawImage(IRenderTarget *pRT, IBitmap *pBmp, CRect& rcWnd, int i32Index);	// ��ҳʱ��ʾ����
		inline RECT		GetDest(const CRect& rtWnd, const SIZE& szImg, CRect& rtImg);				// ���ݵ�ǰ�ķŴ��ʣ�Դλ�ã�����λ��
		
		// ���Ϊ���API
		inline int		GetEncoderClsid(const WCHAR* format, CLSID* pClsid);	// ��ȡͼƬEncoder��CLSID
		inline SStringT GetTempImgFile(TCHAR* pszExt);							// ��ȡ��ʱͼƬ·��
		inline BOOL		RemoveTempImage();										// ɾ����ʱͼƬ
		inline BOOL		Reset(BOOL bNoAngle, BOOL bCloneSel);					// ��λͼƬ
		inline SStringT GetImageFormat(TCHAR* pszExt);							// �õ�ͼƬ��ʽ

	protected:
		SOUI_MSG_MAP_BEGIN()	
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER_EX(OnTimer)
		SOUI_MSG_MAP_END()

	private:
		ULONG_PTR m_gdiplusToken;

		VectImage m_vectImage;		// ͼƬ�б�����ͼƬ��ȫ·��������ȫ�����أ����ܻ�����ڴ治�㣩
		IBitmap*  m_pImgSel;		// ��ǰ��ʾ��ͼƬ
		IBitmap*  m_pImgNext;		// ��/��һ��ͼƬ����ʾ���˶���ʱʹ��
		SSkinAni* m_pImgGif;		// GIF

		E_MoveType	m_eMove;		// �ƶ�ͼƬ����������
		BOOL	m_bError;			// ����ͼƬʧ��
		BOOL	m_bSwitched;		// �Ƿ����л�ͼƬ
		BOOL	m_bImgMovable;		// ͼƬ�Ƿ�����ƶ����޷���ȫͼʱ��
		BOOL	m_bTimerMove;		// ��ҳ������Timer���ñ�ʶ
		int		m_iMoveWidth;		// ��ҳ�����ƶ��Ĵ�С
		int		m_iTimesMove;		// ��ҳ�������ƶ��Ĵ���
		int		m_iSelected;		// ��ǰ��ʾ��ͼƬ����
		float	m_fRatio;			// �Ŵ���ʣ�1.0f ��ԭʼͼƬ��С
		int		m_nAngle;			// ��ת�Ƕ�[0,90,180,270]
		SStringT m_szTmpImg;		// ��תʱʹ�õ���ʱ�ļ�
		CPoint	m_ptMoveStart;		// ��갴��ʱ������λ��
		CPoint	m_ptCenterOld;		// ��갴��ʱԭ��������λ��
		CPoint	m_ptCenter;			// ����λ��
		CRect	m_rtImgSrc;			// ͼƬ��ʾ������src
		CRect	m_rtImgDst;			// ͼƬ��ʾ������dst
	};
}