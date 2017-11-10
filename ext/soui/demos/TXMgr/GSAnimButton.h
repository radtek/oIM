/* $Copyright (c) 2006-2017 Green Net World
 * 
 * Author	:	cherish  version 1.0
 * DateTime	:	2017-4-10 14:18:09
 *
 *
 * GSAnimButton  ����
 */

#ifndef __GSANIMBUTTON_9485438C_70CA_42C3_96E4_5AD0B40589C1__
#define __GSANIMBUTTON_9485438C_70CA_42C3_96E4_5AD0B40589C1__

/** GSAnimButton */
class GSAnimButton : public SButton
{

	SOUI_CLASS_NAME(GSAnimButton, L"gsanibtn")

public:
	GSAnimButton();     /** Ĭ�Ϲ��캯�� */
	~GSAnimButton();    /** Ĭ���������� */

protected:
	virtual void OnNextFrame();

	/**
	* SLink::StopCurAnimate
	* @brief    ֹͣ����
	*
	* Describe  ֹͣ����
	*/
	void StopCurAnimate();

	/**
	* SButton::OnStateChanged
	* @brief    ״̬�ı䴦����
	* @param    DWORD dwOldState -- ��״̬
	* @param    DWORD dwNewState -- ��״̬
	*
	* Describe  ״̬�ı䴦����
	*/
	virtual void OnStateChanged(DWORD dwOldState, DWORD dwNewState);

	void OnPaint(IRenderTarget *pRT);

protected:
	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
	SOUI_MSG_MAP_END()

public:

	int					m_nHoveState;
	int					m_nHoveAnim;
	int					m_nElapseTime;
};


#endif //__GSANIMBUTTON_9485438C_70CA_42C3_96E4_5AD0B40589C1__

