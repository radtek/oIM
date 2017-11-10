/*****************************************************************************
����֧�֣�
  1��ͼ��(logo):   session_item_logo.bkimage
  2�����(frame):  session_item_logo.normalimage
  3��״̬(status): session_item_logo.foreimage(Ⱥ�ĵĺ�㣬���ĵ�����״̬)
  4������(title):  session_item_title.text
*****************************************************************************/
#ifndef __EIM_LIST_CONTACT_ELEMENT_UI_HEADER_20170417_YFGZ__
#define __EIM_LIST_CONTACT_ELEMENT_UI_HEADER_20170417_YFGZ__
#pragma once

#include "Public\EventProxy.h"

typedef enum tagContactItemType
{
	eTYPE_EMP,		  	// Ա��Item
	eTYPE_DEPT,		  	// ����Item
	eTYPE_DEPT_TOP,		// ���㲿��Item
}E_ContactItemType, *PE_ContactItemType;
typedef const enum tagContactItemType* PE_ContactItemType_;

#define CONTACT_STATE_NONE			(0)	// ��״̬
#define CONTACT_STATE_PC_ONLINE		(1)	// PC ����
#define CONTACT_STATE_MOBILE_ONLINE (2)	// �ֻ�����
#define CONTACT_STATE_PC_LEAVE		(3)	// PC �뿪
#define CONTACT_STATE_MOBILE_LEAVE	(4)	// �ֻ��뿪
#define CONTACT_STATE_OFFLINE		(7)	// ������
#define CONTACT_STATE_DEPT			(8)	// �ǲ��ţ������

#define CONTACT_TITLE_MAX			(256)	// ͨѶ¼Item��󳤶ȣ����û���/����������
typedef union tagSortOrder
{
	UINT64 u64Order;
	struct{
		UINT64 bitPinyin:32;
		UINT64 bitOrder:16;
		UINT64 bitRev:12;
		UINT64 bitState:4;
	};

	tagSortOrder(): u64Order(0){} 
	void SetState(DWORD dwTerm=UINT_MAX, DWORD dwStatus=UINT_MAX)
	{
		switch(dwStatus)
		{
		case IME_EMP_STATUS_ONLINE:	// ����
			bitState = (dwTerm == IME_EMP_TERM_PC) ? CONTACT_STATE_PC_ONLINE : CONTACT_STATE_MOBILE_ONLINE;
			break;
		case IME_EMP_STATUS_LEAVE:	// �뿪
			bitState = (dwTerm == IME_EMP_TERM_PC) ? CONTACT_STATE_PC_LEAVE : CONTACT_STATE_MOBILE_LEAVE;
			break;
		case UINT_MAX:
			bitState = CONTACT_STATE_DEPT;			
			break;
		default:					
			bitState = (dwTerm == UINT_MAX) ? CONTACT_STATE_NONE : CONTACT_STATE_OFFLINE;
			break;
		}
	}

	void SetOrder(DWORD dwUser, DWORD dwPinyin=UINT_MAX)
	{
		if ( dwPinyin == UINT_MAX )
		{	// ���ţ����ü���
			bitOrder = dwUser;	
			bitPinyin= 0;
		}
		else
		{
			bitOrder = USHRT_MAX - dwUser;	// ������ԭ�� �����������Ǵ�Ĵ�ǰ��С���ں�0�ǲ�����
			bitPinyin= dwPinyin;
		}
	}
}U_SortOrder, *PU_SortOrder;
typedef const union tagSortOrder* PU_SortOrder_;

#define CONTACT_FLAG_MALE			(0x00000001)	// �����ԣ�û�����þ���Ů��
#define CONTACT_FLAG_PATH			(0x00000002)	// ��·��Item
#define CONTACT_FLAG_ADMIN			(0x00000004)	// ��Ⱥ������
#define CONTACT_FLAG_SORT_ID		(0x00000100)	// ����ʹ��ID������ʹ��ƴ�����򣨽������Ա����
#define CONTACT_FLAG_SORT_XSTATUS	(0x00000200)	// ����ʹ��״̬ 
#define CONTACT_FLAG_SORT_XMOBILE	(0x00000400)	// ���������ֻ������ֻ����ȣ�
#define CONTACT_FLAG_SORT_MASK		(0x00000F00)	// ��������
#define	CONTACT_FLAG_DISABLE_EVENT	(0x80000000)	// �������е��¼����Ự�����б���Ϣ�������ȿ��Բ�����Ӧ��ʱ״̬�仯��

typedef struct tagContactItemInfo
{
	E_ContactItemType eType;	// Item ����
	union{
		QDID did;				// ����ID
		QEID eid;				// Ա��ID
	};
	DWORD  dwFlag;				// ��ʶ
	DWORD  dwPurview;			// Ȩ�ޱ�ʶ
	U_SortOrder  uSortOrder;	// �����ʶ
	TCHAR  szTitle[CONTACT_TITLE_MAX + 1];		// ����

	tagContactItemInfo()
		: eType(eTYPE_EMP)
		, eid(0)
		, dwFlag(0)
		, dwPurview(0)
	{
		szTitle[0] = _T('\0');
	}

	tagContactItemInfo& operator= (const tagContactItemInfo& s)
	{
		eid     = s.eid;
		eType   = s.eType;
		dwFlag  = s.dwFlag;
		dwPurview = s.dwPurview;
		uSortOrder.u64Order = s.uSortOrder.u64Order;
		_tcsncpy(szTitle, s.szTitle, CONTACT_TITLE_MAX);
		return *this;
	}
}S_ContactItemInfo, *PS_ContactItemInfo;
typedef const struct tagContactItemInfo* PS_ContactItemInfo_;

const TCHAR* const kListContactElement =  _T("ListContactElementUI");

#if defined(EIMUICORE_EXPORTS)
class __declspec(dllexport) CListContactElementUI : public CListContainerElementUI
#else
class __declspec(dllimport) CListContactElementUI : public CListContainerElementUI
#endif
{
public:
	CListContactElementUI();
	~CListContactElementUI();

	// Event
	virtual void OnEvent(unsigned int u32Event, void* lpvParam);
	inline void OnUpdateStatus(PS_EmpInfo psEmpInfo);
	inline void OnUpdateHeadImage(QEID eid);
	inline void OnUpdateMyself(int i32Type);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual UiLib::unUserData GetAttribute( LPCTSTR pstrName);
	virtual void Init();
    virtual LPCTSTR GetClass() const;
	virtual void SetEnabled(bool bEnable = true);
	virtual void DoEvent(TEventUI& event);
	virtual void DrawItemText(HDC hDC, const RECT& rcItem);
	virtual UINT GetControlFlags() const;

	LPCTSTR GetLogo();
	void	SetLogo(LPCTSTR pStrImage);

	int		GetStatus();
	void	SetStatus(DWORD dwTerm=UINT_MAX, DWORD dwStatus=UINT_MAX);

	LPCTSTR GetTitle();
	BOOL	SetTitle(LPCTSTR pszTitle);

	PS_ContactItemInfo GetInfo();
	BOOL	SetInfo(const S_ContactItemInfo& sCInfo);


private:
	C_EventProxy<CListContactElementUI> m_EventProxy;
	S_ContactItemInfo	 m_UICInfo;
	CHorizontalLayoutUI* m_phlItem;		// HorizontalLayout
	CHorizontalLayoutUI* m_phlFrame;	// HorizontalLayout

	CPictureUI*	m_ppicLogo;
	CLabelUI*	m_plblStatus;
	CLabelUI*	m_plblTitle;
	CButtonUI*	m_pbtnTitle;
	CLabelUI*	m_plblFlag;

};

#endif // __EIM_LIST_CONTACT_ELEMENT_UI_HEADER_20170417_YFGZ__