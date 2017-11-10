/* $Copyright (c) 2006-2017 Green Net World
 * 
 * Author	:	cherish  version 1.0
 * DateTime	:	2017-4-13 14:13:46
 *
 *
 * GSTabCtrl  ����
 */

#ifndef __GSTABCTRL_BFFA82AF_9970_4395_8F9C_CF25F8700172__
#define __GSTABCTRL_BFFA82AF_9970_4395_8F9C_CF25F8700172__

/** CGSTabCtrl */
class GSTabCtrl : public STabCtrl , protected ITimelineHandler
{
	SOUI_CLASS_NAME(GSTabCtrl, L"gstabctrl")

public:
	GSTabCtrl();     /** Ĭ�Ϲ��캯�� */
	~GSTabCtrl();    /** Ĭ���������� */


protected:

	/**
	* STabCtrl::OnPaint
	* @brief    �滭��Ϣ
	* @param    IRenderTarget *pRT -- �����豸���
	*
	* Describe  �˺�������Ϣ��Ӧ����
	*/
	void OnPaint(IRenderTarget *pRT);

	/**
	* STabCtrl::DrawItem
	* @brief    ����item
	* @param    IRenderTarget *pRT -- �����豸
	* @param    const CRect &rcItem -- ��������
	* @param    int iItem  -- ����
	* @param    DWORD dwState  -- ״̬
	*
	* Describe  ����item
	*/
	virtual void DrawItem(IRenderTarget *pRT, const CRect &rcItem, int iItem, DWORD dwState);

	/**
	* STabCtrl::OnMouseMove
	* @brief    ����ƶ��¼�
	* @param    UINT nFlags -- ��־
	* @param    CPoint point -- �������
	*
	* Describe  �˺�������Ϣ��Ӧ����
	*/
	void OnMouseMove(UINT nFlags, CPoint point);

	/**
	* STabCtrl::OnMouseLeave
	* @brief    ����뿪�¼�
	*
	* Describe  �˺�������Ϣ��Ӧ����
	*/
	void OnMouseLeave()
	{
		OnMouseMove(0, CPoint(-1, -1));
	}


	virtual void OnNextFrame();
protected:

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
	SOUI_MSG_MAP_END()


	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"anihover", m_pSkinAniHover, FALSE)
		ATTR_SKIN(L"anidown", m_pSkinAniDown, FALSE)
		ATTR_INT(L"tab_show_name", m_nTabShowName, FALSE)
	SOUI_ATTRS_END()

private:
	ISkinObj *					m_pSkinAniHover; /**< ISkibObj���� */
	ISkinObj *					m_pSkinAniDown; /**< ISkibObj���� */

	SArray<int>					m_arrHoverNumber;
	int							m_nElapseTime;
	int							m_nTabShowName;
};


#endif //__GSTABCTRL_BFFA82AF_9970_4395_8F9C_CF25F8700172__

